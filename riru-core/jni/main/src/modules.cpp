#include "modules.h"
#include "riru.h"

#include <dlfcn.h>
#include <jni.h>

Module::Pointer Module::load(string const &name ,string const &path) {
    void *handle             = dlopen(path.c_str() ,RTLD_LAZY);
    if ( handle == nullptr )
        throw std::runtime_error(dlerror());

    Pointer result = Pointer(new Module());

    result->name                          = name;
    result->handle                        = handle;
    result->nativeHookList                = dlsym(handle ,NATIVE_HOOK_LIST_SYMBOL);
    result->onModuleLoaded                = dlsym(handle ,ON_MODULE_LOADED_SYMBOL);
    result->onNativeForkAndSpecializePre  = dlsym(handle ,NATIVE_FORK_AND_SPECIALIZE_PRE_SYMBOL);
    result->onNativeForkAndSpecializePost = dlsym(handle ,NATIVE_FORK_AND_SPECIALIZE_POST_SYMBOL);
    result->onNativeForkSystemServerPre   = dlsym(handle ,NATIVE_FORK_SYSTEM_SERVER_PRE_SYMBOL);
    result->onNativeForkSystemServerPost  = dlsym(handle ,NATIVE_FORK_SYSTEM_SERVER_POST_SYMBOL);

    return result;
}

Module::Module() {

}

void Module::unload(void) {
    dlclose(handle);
}

void Module::invokeModuleLoaded(void) {
    if ( !onModuleLoaded ) return;

    Log::info() << "[Module] " << name << ": moduleLoaded" << Log::END;

    reinterpret_cast<ModuleLoadedFunction>(onModuleLoaded)();
}

void Module::invokeNativeForkAndSpecializePre(JNIEnv *env, jclass clazz, jint uid, jint gid,
                                              jintArray gids,
                                              jint runtime_flags, jobjectArray rlimits,
                                              jint mount_external,
                                              jstring se_info, jstring se_name,
                                              jintArray fdsToClose,
                                              jintArray fdsToIgnore,
                                              jboolean is_child_zygote, jstring instructionSet,
                                              jstring appDataDir) {
    if ( !onNativeForkAndSpecializePre ) return;

    Log::info() << "[Module] " << name << ": nativeForkAndSpecializePre" << Log::END;

    reinterpret_cast<NativeForkAndSpecializePreFunction>(onNativeForkAndSpecializePre)(env ,clazz ,uid ,gid ,gids ,
                                                                                 runtime_flags ,rlimits ,mount_external ,se_info ,se_name ,
                                                                                 fdsToClose ,fdsToIgnore ,is_child_zygote ,instructionSet ,appDataDir);
}

int Module::invokeNativeForkAndSpecializePost(JNIEnv *env, jclass clazz, jint res) {
    if ( !onNativeForkAndSpecializePost ) return 0;

    Log::info() << "[Module] " << name << ": nativeForkAndSpecializePost" << Log::END;

    return reinterpret_cast<NativeForkAndSpecializePostFunction>(onNativeForkAndSpecializePost)(env ,clazz ,res);
}

void Module::invokeNativeForkSystemServerPre(JNIEnv *env, jclass clazz, uid_t uid, gid_t gid, jintArray gids,
                                             jint debug_flags, jobjectArray rlimits, jlong permittedCapabilities,
                                             jlong effectiveCapabilities) {
    if ( !onNativeForkSystemServerPre ) return ;

    Log::info() << "[Module] " << name << ": nativeForkSystemServerPre" << Log::END;

    reinterpret_cast<NativeForkSystemServerPreFunction>(onNativeForkSystemServerPre)(env ,clazz ,uid ,gid ,gids ,
                                                                               debug_flags ,rlimits ,permittedCapabilities ,
                                                                               effectiveCapabilities);
}

int Module::invokeNativeForkSystemServerPost(JNIEnv *env, jclass clazz, jint res) {
    if ( !onNativeForkSystemServerPost ) return 0;

    Log::info() << "[Module] " << name << ": nativeForkSystemServerPost" << Log::END;

    return reinterpret_cast<NativeForkSystemServerPostFunction>(onNativeForkSystemServerPost)(env ,clazz ,res);
}

Module::NativeHookListPointer Module::getNativeHookList(void) noexcept {
    static riru_module_native_hook_list_t EMPTY_LIST[] = {RIRU_NATIVE_HOOK_LIST_END};

    if ( nativeHookList )
        return reinterpret_cast<NativeHookListPointer>(nativeHookList);

    return EMPTY_LIST;
}

Module::~Module() {
    this->unload();
}

Modules &Modules::get(void) {
    static Modules *instance = new Modules();
    return *instance;
}

void Modules::loadAll(void) {
    Log::info() << "[Modules] loadAll" << Log::END;
    vector<string> module_names = Utils::listDirectory(RIRU_MODULES_CONFIGS_DIR ,false ,DT_DIR);

    for ( string module_name : module_names ) {
        try {
            Log::info() << "[Modules] Loading " << module_name << Log::END;
            modules.push_back(Module::load(module_name ,RIRU_MODULES_LIBRARY_PATH(module_name)));
        }
        catch (exception const &e) {
            Log::warn() << "[Modules] Load module " << module_name << " failure. " << e.what() << Log::END;
        }
    }
}

void Modules::refreshCache(void) {
    Log::info() << "[Modules] refreshCache " << Log::END;

    hookCache.clear();

    for ( auto module : modules ) {
        auto p = module->getNativeHookList();

        while ( p->class_name ) {
            auto it = hookCache.find(p->class_name);
            if ( it == hookCache.end() )
                hookCache[p->class_name] = std::multimap<string ,Module::NativeHookListPointer>();
            hookCache[p->class_name].insert(std::pair<string ,Module::NativeHookListPointer>(methodId(p->method_name ,p->signature) ,p));
            p++;
        }
    }
}

bool Modules::handleJniRegisterNativeMethods(string const &class_name,
                                             vector<JNINativeMethod> &methods) {
    Log::debug() << "[Modules] handleJniRegisterNativeMethods " << class_name << Log::END;

    auto current = hookCache.find(class_name);
    if ( current == hookCache.end() )
        return false;

    for ( JNINativeMethod &method : methods ) {
        string current_method_id = methodId(method.name ,method.signature);

        //build call chain
        auto method_hooks_iterator = (*current).second.equal_range(current_method_id);
        if ( method_hooks_iterator.first == method_hooks_iterator.second )
            continue;

        Log::info() << "[Modules] handle " << current_method_id << Log::END;

        void *current_hook_function = method.fnPtr;
        std::for_each(method_hooks_iterator.first ,method_hooks_iterator.second ,[&](std::pair<string ,Module::NativeHookListPointer> p) {
            *(p.second->original_function)   = current_hook_function;
            current_hook_function = p.second->hook_function;
        });
        method.fnPtr = current_hook_function;
    }

    return true;
}

string Modules::methodId(string const &name, string const &signature) {
    return name + "#" + signature;
}

void Modules::invokeAllModuleLoaded(void) {
    for ( auto module : modules )
        module->invokeModuleLoaded();
}

void Modules::invokeAllNativeForkAndSpecializePre(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids,
                                                  jint runtime_flags, jobjectArray rlimits,
                                                  jint mount_external, jstring se_info, jstring se_name,
                                                  jintArray fdsToClose, jintArray fdsToIgnore,
                                                  jboolean is_child_zygote, jstring instructionSet, jstring appDataDir) {
    Log::info() << "[Modules] invokeAllNativeForkAndSpecializePre" << Log::END;

    for ( auto module : modules ) {
        module->invokeNativeForkAndSpecializePre(env, clazz, uid, gid, gids,
                                                             runtime_flags, rlimits,
                                                             mount_external, se_info, se_name,
                                                             fdsToClose, fdsToIgnore,
                                                             is_child_zygote, instructionSet,
                                                             appDataDir);
    }
}

void Modules::invokeAllNativeForkAndSpecializePost(JNIEnv *env, jclass clazz, jint res) {
    Log::info() << "[Modules] invokeAllNativeForkAndSpecializePost" << Log::END;

    for ( auto module : modules )
        module->invokeNativeForkAndSpecializePost(env ,clazz ,res);
}

void Modules::invokeAllNativeForkSystemServerPre(JNIEnv *env, jclass clazz, uid_t uid, gid_t gid, jintArray gids, jint debug_flags,
                                                 jobjectArray rlimits, jlong permittedCapabilities, jlong effectiveCapabilities) {
    Log::info() << "[Modules] invokeAllNativeForkSystemServerPre" << Log::END;

    for ( auto module : modules )
        module->invokeNativeForkSystemServerPre(env ,clazz ,uid ,gid ,gids ,debug_flags ,
                                                            rlimits ,permittedCapabilities ,effectiveCapabilities);
}

void Modules::invokeAllNativeForkSystemServerPost(JNIEnv *env, jclass clazz, jint res) {
    Log::info() << "[Modules] invokeAllNativeForkSystemServerPost" << Log::END;

    for ( auto module : modules )
        module->invokeNativeForkSystemServerPost(env ,clazz ,res);
}
