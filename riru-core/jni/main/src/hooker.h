#pragma once

#include <jni.h>

#include <vector>

#include "log.h"

using std::vector;

class Hooker {
public:
    typedef jint (*FunctionJniRegisterMethods)(JNIEnv *env ,const char *class_name ,const JNINativeMethod* methods, jint length);

public:
    static void install(void);

private:
    static jint onJniRegisterMethods(JNIEnv *env ,const char *class_name ,const JNINativeMethod* methods, jint length);
    static FunctionJniRegisterMethods originalJniRegisterMethods;
};

class InternalHook {
public:
    static bool handleJniRegisterNativeMethods(string const &class_name,
                                               vector<JNINativeMethod> &methods);

public:
    static jint onNativeForkAndSpecializeMarshmallow(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint debug_flags, jobjectArray rlimits, jint mount_external, jstring se_info, jstring se_name, jintArray fdsToClose, jstring instructionSet, jstring appDataDir);
    static jint onNativeForkAndSpecializeOreo(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint debug_flags, jobjectArray rlimits, jint mount_external, jstring se_info, jstring se_name, jintArray fdsToClose, jintArray fdsToIgnore, jstring instructionSet, jstring appDataDir);
    static jint onNativeForkAndSpecializePie(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint runtime_flags, jobjectArray rlimits, jint mount_external, jstring se_info, jstring se_name, jintArray fdsToClose, jintArray fdsToIgnore, jboolean is_child_zygote, jstring instructionSet, jstring appDataDir);
    static jint onNativeForkSystemServer(JNIEnv *env, jclass clazz, uid_t uid, gid_t gid, jintArray gids, jint debug_flags, jobjectArray rlimits, jlong permittedCapabilities, jlong effectiveCapabilities);

public:
    typedef jint (*NativeForkAndSpecializeMarshmallowFunction)(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint debug_flags, jobjectArray rlimits, jint mount_external, jstring se_info, jstring se_name, jintArray fdsToClose, jstring instructionSet, jstring appDataDir);
    typedef jint (*NativeForkAndSpecializeOreoFunction)(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint debug_flags, jobjectArray rlimits, jint mount_external, jstring se_info, jstring se_name, jintArray fdsToClose, jintArray fdsToIgnore, jstring instructionSet, jstring appDataDir);
    typedef jint (*NativeForkAndSpecializePieFunction)(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids, jint runtime_flags, jobjectArray rlimits, jint mount_external, jstring se_info, jstring se_name, jintArray fdsToClose, jintArray fdsToIgnore, jboolean is_child_zygote, jstring instructionSet, jstring appDataDir);
    typedef jint (*NativeForkSystemServerFunction)(JNIEnv *env, jclass clazz, uid_t uid, gid_t gid, jintArray gids, jint debug_flags, jobjectArray rlimits, jlong permittedCapabilities, jlong effectiveCapabilities);

public:
    static NativeForkAndSpecializeMarshmallowFunction originalNativeForkAndSpecializeMarshmallow;
    static NativeForkAndSpecializeOreoFunction        originalNativeForkAndSpecializeOreo;
    static NativeForkAndSpecializePieFunction         originalNativeForkAndSpecializePie;
    static NativeForkSystemServerFunction             originalNativeForkSystemServer;

private:
    static bool shouldSkipUid(int uid);
};