#pragma once

#include <memory>
#include <string>
#include <map>
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
    typedef riru_module_native_hook_list_t* NativeHookListArray;

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
    NativeHookListArray getNativeHookList(void) noexcept;

private:
    Module();
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
    static Modules& get(void) noexcept;

public:
    void loadAll(void);

private:
    std::map<string ,Module::Pointer> modules;

private:
    static Modules instance;
};