#include "modules.h"

#include <dlfcn.h>

Module::Pointer Module::load(string const &name ,string const &path) {
    Pointer result = Pointer(new Module());

    void *handle             = dlopen(path.c_str() ,RTLD_LAZY);
    if ( handle == nullptr )
        throw std::runtime_error("Unable to open " + path + " " + dlerror());

    result->name                    = name;
    result->handle                  = handle;
    result->nativeHookList          = dlsym(handle ,NATIVE_HOOK_LIST_SYMBOL);
    result->onModuleLoaded          = dlsym(handle ,ON_MODULE_LOADED_SYMBOL);
    result->onForkAndSpecializePre  = dlsym(handle ,NATIVE_FORK_AND_SPECIALIZE_PRE_SYMBOL);
    result->onForkAndSpecializePost = dlsym(handle ,NATIVE_FORK_AND_SPECIALIZE_POST_SYMBOL);
    result->onForkSystemServerPre   = dlsym(handle ,NATIVE_FORK_SYSTEM_SERVER_PRE_SYMBOL);
    result->onForkSystemServerPost  = dlsym(handle ,NATIVE_FORK_SYSTEM_SERVER_POST_SYMBOL);

    return result;
}

Module::Module() {

}

void Module::unload(void) {
    dlclose(handle);
}

void Module::invokeModuleLoaded(void) {
    if ( onModuleLoaded )
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
    if ( onForkAndSpecializePre )
        reinterpret_cast<NativeForkAndSpecializePreFunction>(onForkAndSpecializePre)(env ,clazz ,uid ,gid ,gids ,
                                                                                 runtime_flags ,rlimits ,mount_external ,se_info ,se_name ,
                                                                                 fdsToClose ,fdsToIgnore ,is_child_zygote ,instructionSet ,appDataDir);
}

int Module::invokeNativeForkAndSpecializePost(JNIEnv *env, jclass clazz, jint res) {
    if ( onForkAndSpecializePost )
        return reinterpret_cast<NativeForkAndSpecializePostFunction>(onForkAndSpecializePost)(env ,clazz ,res);
    return 0;
}

void Module::invokeNativeForkSystemServerPre(JNIEnv *env, jclass clazz, uid_t uid, gid_t gid, jintArray gids,
                                             jint debug_flags, jobjectArray rlimits, jlong permittedCapabilities,
                                             jlong effectiveCapabilities) {
    if ( onForkSystemServerPre )
        reinterpret_cast<NativeForkSystemServerPreFunction>(onForkSystemServerPre)(env ,clazz ,uid ,gid ,gids ,
                                                                                   debug_flags ,rlimits ,permittedCapabilities ,
                                                                                   effectiveCapabilities);
}

int Module::invokeNativeForkSystemServerPost(JNIEnv *env, jclass clazz, jint res) {
    if ( onForkSystemServerPost )
        return reinterpret_cast<NativeForkSystemServerPostFunction>(onForkSystemServerPost)(env ,clazz ,res);
    return 0;
}

Module::NativeHookListArray Module::getNativeHookList(void) noexcept {
    static riru_module_native_hook_list_t EMPTY_LIST[] = {RIRU_NATIVE_HOOK_LIST_END};

    if ( nativeHookList )
        return reinterpret_cast<NativeHookListArray>(nativeHookList);

    return EMPTY_LIST;
}

Module::~Module() {
    this->unload();
}

Modules &Modules::get(void) noexcept {
    return instance;
}

void Modules::loadAll(void) {
    vector<string> module_names = Utils::listDirectory(RIRU_MODULES_CONFIGS_DIR ,false ,DT_DIR);

    for ( string module_name : module_names ) {
        try {
            modules[module_name] = Module::load(module_name ,RIRU_MODULES_LIBRARY_PATH(module_name));
        }
        catch (exception const &e) {
            Log::w << "Load module " << module_name << " failure. " << e.what() << Log::END;
        }
    }
}

Modules Modules::instance;
