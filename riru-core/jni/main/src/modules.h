#pragma once

#include <memory>
#include <string>
#include <map>
#include <list>
#include <jni.h>

#include "utils.h"
#include "riru.h"

using std::string;

class Module {
public:
    typedef std::shared_ptr<Module> Pointer;

public:
    typedef void (*ModuleLoadedFunction)(void);
    typedef void (*NativeForkAndSpecializePreFunction)(JNIEnv *, jclass, jint, jint, jintArray, jint, jobjectArray, jint, jstring, jstring, jintArray, jintArray, jboolean, jstring, jstring);
    typedef int  (*NativeForkAndSpecializePostFunction)(JNIEnv *, jclass, jint);
    typedef void (*NativeForkSystemServerPreFunction)(JNIEnv *, jclass, uid_t, gid_t, jintArray, jint, jobjectArray, jlong, jlong);
    typedef int  (*NativeForkSystemServerPostFunction)(JNIEnv *, jclass, jint);
    typedef riru_module_native_hook_list_t* NativeHookListPointer;

public:
    static Pointer load(string const &name ,string const &path);

public:
    void unload(void);

public:
    void invokeModuleLoaded(void);
    void invokeNativeForkAndSpecializePre(JNIEnv *, jclass, jint, jint, jintArray, jint, jobjectArray, jint, jstring, jstring, jintArray, jintArray, jboolean, jstring, jstring);
    int  invokeNativeForkAndSpecializePost(JNIEnv *, jclass, jint);
    void invokeNativeForkSystemServerPre(JNIEnv *, jclass, uid_t, gid_t, jintArray, jint, jobjectArray, jlong, jlong);
    int  invokeNativeForkSystemServerPost(JNIEnv *, jclass, jint);
    NativeHookListPointer getNativeHookList(void) noexcept;

private:
    Module();

public:
    ~Module();

private:
    string name;
    void *handle;
    void *nativeHookList;
    void *onModuleLoaded;
    void *onForkAndSpecializePre;
    void *onForkAndSpecializePost;
    void *onForkSystemServerPre;
    void *onForkSystemServerPost;
};

class Modules {
private:
    Modules() {};

public:
    static Modules& get(void);

public:
    void loadAll(void);
    void refreshCache(void);

public:
    void invokeAllModuleLoaded(void);
    void invokeAllNativeForkAndSpecializePre(JNIEnv *, jclass, jint, jint, jintArray, jint, jobjectArray, jint, jstring, jstring, jintArray, jintArray, jboolean, jstring, jstring);
    void invokeAllNativeForkAndSpecializePost(JNIEnv *, jclass, jint);
    void invokeAllNativeForkSystemServerPre(JNIEnv *, jclass, uid_t, gid_t, jintArray, jint, jobjectArray, jlong, jlong);
    void invokeAllNativeForkSystemServerPost(JNIEnv *, jclass, jint);

public:
    bool handleJniRegisterNativeMethods(string const &class_name, vector<JNINativeMethod> &methods);

private:
    static string methodId(string const &name ,string const &signature);

private:
    std::vector<Module::Pointer>                                             modules;    // name to module
    std::map<string ,std::multimap<string ,Module::NativeHookListPointer>>   hookCache;  // class name to ( method to hook entry )
};