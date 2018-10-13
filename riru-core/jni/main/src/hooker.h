#pragma once

#include <jni.h>

#include <vector>

#include "log.h"

class Hooker {
public:
    typedef jint (*FunctionJniRegisterMethods)(JNIEnv *env ,const char *class_name ,const JNINativeMethod* methods, jint length);

public:
    static void install(void);

private:
    static jint onJniRegisterMethods(JNIEnv *env ,const char *class_name ,const JNINativeMethod* methods, jint length);
    static FunctionJniRegisterMethods originalRegisterMethod;
};