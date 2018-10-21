#include "hooker.h"

#include <jni.h>
#include <cstring>

#include "xhook/xhook.h"
#include "config.h"
#include "modules.h"

void Hooker::install(void) {
    Log::info() << "[Hooker] install " << Log::END;

    if ( xhook_register(PATTERN_JNI_REGISTER_METHODS_LIBRARY , JNI_REGISTER_METHODS_SYMBOL ,
                        reinterpret_cast<void*>(&Hooker::onJniRegisterMethods) ,
                   reinterpret_cast<void **>(&originalJniRegisterMethods)) )
        throw std::runtime_error("xhook_register return non-zero");

    if ( xhook_refresh(0) )
        throw std::runtime_error("xhook_refresh return non-zero");
    else
        xhook_clear();
}

jint Hooker::onJniRegisterMethods(JNIEnv *env, const char *class_name, const JNINativeMethod *methods, jint length) {
    Log::debug() << "[Hooker] onJniRegisterMethods " << class_name << Log::END;

    vector<JNINativeMethod> modifiable_methods(methods ,methods + length);

    if (InternalHook::handleJniRegisterNativeMethods(class_name, modifiable_methods) ||
            Modules::get().handleJniRegisterNativeMethods(class_name, modifiable_methods) )
        return originalJniRegisterMethods(env ,class_name ,&modifiable_methods[0] ,(int)modifiable_methods.size());

    return originalJniRegisterMethods(env , class_name ,methods ,length);
}

Hooker::FunctionJniRegisterMethods Hooker::originalJniRegisterMethods;

bool InternalHook::handleJniRegisterNativeMethods(string const &class_name,
                                                  vector<JNINativeMethod> &methods) {
    if ( class_name != "com/android/internal/os/Zygote" ) return false;

    Log::info() << "[InternalHook] handleJniRegisterNativeMethods com/android/internal/os/Zygote" << Log::END;

    for ( JNINativeMethod &method : methods ) {
        if ( !strcmp(method.name ,NATIVE_FORK_AND_SPECIALIZE_METHOD) ) {
            Log::info() << "[InternalHook] Found " NATIVE_FORK_AND_SPECIALIZE_METHOD << Log::END;
            if      ( !strcmp(method.signature ,NATIVE_FORK_AND_SPECIALIZE_MARSHMALLOW_SIGNATURE) ) {
                originalNativeForkAndSpecializeMarshmallow = (NativeForkAndSpecializeMarshmallowFunction) method.fnPtr;
                method.fnPtr = (void *) &InternalHook::onNativeForkAndSpecializeMarshmallow;
            }
            else if ( !strcmp(method.signature ,NATIVE_FORK_AND_SPECIALIZE_OREO_SIGNATURE) ) {
                originalNativeForkAndSpecializeOreo = (NativeForkAndSpecializeOreoFunction) method.fnPtr;
                method.fnPtr = (void *) &InternalHook::onNativeForkAndSpecializeOreo;
            }
            else if ( !strcmp(method.signature ,NATIVE_FORK_AND_SPECIALIZE_PIE_SIGNATURE) ) {
                originalNativeForkAndSpecializePie = (NativeForkAndSpecializePieFunction) method.fnPtr;
                method.fnPtr = (void *) &InternalHook::onNativeForkAndSpecializePie;
            }
            else
                Log::warn() << "[InternalHook] " NATIVE_FORK_AND_SPECIALIZE_METHOD " signature match failure." << Log::END;
        }
        else if ( !strcmp(method.name ,NATIVE_FORK_SYSTEM_SERVER_METHOD) ) {
            Log::info() << "[InternalHook] Found " NATIVE_FORK_SYSTEM_SERVER_METHOD << Log::END;
            if ( !strcmp(method.signature ,NATIVE_FORK_SYSTEM_SERVER_SIGNATURE) ) {
                originalNativeForkSystemServer = (NativeForkSystemServerFunction) method.fnPtr;
                method.fnPtr = (void *) &InternalHook::onNativeForkSystemServer;
            }
            else
                Log::warn() << "[InternalHook] " NATIVE_FORK_SYSTEM_SERVER_METHOD " signature match failure." << Log::END;
        }
    }

    return true;
}

jint InternalHook::onNativeForkAndSpecializeMarshmallow(JNIEnv *env, jclass clazz, jint uid, jint gid,
                                                        jintArray gids, jint debug_flags,
                                                        jobjectArray rlimits, jint mount_external,
                                                        jstring se_info, jstring se_name,
                                                        jintArray fdsToClose, jstring instructionSet,
                                                        jstring appDataDir) {
    Log::info() << "[InternalHook] onNativeForkAndSpecializeMarshmallow" << Log::END;

    if (!shouldSkipUid(uid))
        Modules::get().invokeAllNativeForkAndSpecializePre(env ,clazz ,uid ,gid ,gids ,debug_flags ,rlimits ,
                                                       mount_external ,se_info ,se_name ,fdsToClose ,
                                                       nullptr ,0 ,instructionSet ,appDataDir);

    int result = originalNativeForkAndSpecializeMarshmallow(env ,clazz ,uid ,gid ,gids ,debug_flags ,
            rlimits ,mount_external ,se_info ,se_name ,fdsToClose ,instructionSet ,appDataDir);

    if (!shouldSkipUid(uid))
        Modules::get().invokeAllNativeForkAndSpecializePost(env ,clazz ,result);

    return result;
}

jint InternalHook::onNativeForkAndSpecializeOreo(JNIEnv *env, jclass clazz, jint uid, jint gid,
                                                 jintArray gids, jint debug_flags,
                                                 jobjectArray rlimits, jint mount_external,
                                                 jstring se_info, jstring se_name,
                                                 jintArray fdsToClose, jintArray fdsToIgnore,
                                                 jstring instructionSet, jstring appDataDir) {
    Log::info() << "[InternalHook] onNativeForkAndSpecializeOreo" << Log::END;

    if (!shouldSkipUid(uid))
        Modules::get().invokeAllNativeForkAndSpecializePre(env ,clazz ,uid ,gid ,gids ,debug_flags ,rlimits ,mount_external ,
                                                       se_info ,se_name ,fdsToClose ,fdsToIgnore ,0 ,instructionSet ,appDataDir);

    int result = originalNativeForkAndSpecializeOreo(env ,clazz ,uid ,gid ,gids ,debug_flags ,rlimits ,mount_external ,
            se_info ,se_name ,fdsToClose ,fdsToIgnore ,instructionSet ,appDataDir);

    if (!shouldSkipUid(uid))
        Modules::get().invokeAllNativeForkAndSpecializePost(env ,clazz ,result);

    return result;
}

jint InternalHook::onNativeForkAndSpecializePie(JNIEnv *env, jclass clazz, jint uid, jint gid,
                                                jintArray gids, jint runtime_flags,
                                                jobjectArray rlimits, jint mount_external,
                                                jstring se_info, jstring se_name,
                                                jintArray fdsToClose, jintArray fdsToIgnore,
                                                jboolean is_child_zygote, jstring instructionSet,
                                                jstring appDataDir) {
    Log::info() << "[InternalHook] onNativeForkAndSpecializePie" << Log::END;

    if (!shouldSkipUid(uid))
        Modules::get().invokeAllNativeForkAndSpecializePre(env ,clazz ,uid ,gid ,gids ,runtime_flags ,rlimits , mount_external ,
                                                           se_info ,se_name ,fdsToClose ,fdsToIgnore ,is_child_zygote ,instructionSet ,appDataDir);

    int result = originalNativeForkAndSpecializePie(env ,clazz ,uid ,gid ,gids ,runtime_flags ,rlimits ,mount_external ,
                                       se_info ,se_name ,fdsToClose ,fdsToIgnore ,is_child_zygote ,instructionSet ,appDataDir);

    if (!shouldSkipUid(uid))
        Modules::get().invokeAllNativeForkAndSpecializePost(env ,clazz ,result);

    return result;
}

jint InternalHook::onNativeForkSystemServer(JNIEnv *env, jclass clazz, uid_t uid, gid_t gid,
                                            jintArray gids, jint debug_flags, jobjectArray rlimits,
                                            jlong permittedCapabilities,
                                            jlong effectiveCapabilities) {
    Log::info() << "[InternalHook] onNativeForkSystemServer" << Log::END;

    Modules::get().invokeAllNativeForkSystemServerPre(env ,clazz ,uid ,gid ,gids ,debug_flags ,
                                                      rlimits ,permittedCapabilities ,effectiveCapabilities);

    int result = originalNativeForkSystemServer(env ,clazz ,uid ,gid ,gids ,debug_flags ,rlimits ,permittedCapabilities ,effectiveCapabilities);

    Modules::get().invokeAllNativeForkSystemServerPost(env ,clazz ,result);

    return result;
}

bool InternalHook::shouldSkipUid(int uid) {
    int appId = uid % 100000;

    // limit only regular app, or strange situation will happen, such as zygote process not start (dead for no reason and leave no clues?)
    // https://android.googlesource.com/platform/frameworks/base/+/android-9.0.0_r8/core/java/android/os/UserHandle.java#151
    return !(10000 <= appId && appId <= 19999);
}

InternalHook::NativeForkAndSpecializeMarshmallowFunction InternalHook::originalNativeForkAndSpecializeMarshmallow;
InternalHook::NativeForkAndSpecializeOreoFunction        InternalHook::originalNativeForkAndSpecializeOreo;
InternalHook::NativeForkAndSpecializePieFunction         InternalHook::originalNativeForkAndSpecializePie;
InternalHook::NativeForkSystemServerFunction             InternalHook::originalNativeForkSystemServer;