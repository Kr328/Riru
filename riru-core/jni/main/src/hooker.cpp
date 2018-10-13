#include "hooker.h"

#include "xhook/xhook.h"
#include "config.h"

void Hooker::install(void) {
    if ( xhook_register(PATTERN_JNI_REGISTER_METHODS_LIBRARY , JNI_REGISTER_METHODS_SYMBOL ,
                        reinterpret_cast<void*>(&Hooker::onJniRegisterMethods) ,
                   reinterpret_cast<void **>(&originalRegisterMethod)) )
        throw std::runtime_error("xhook_register return non-zero");

    if ( xhook_refresh(0) )
        throw std::runtime_error("xhook_refresh return non-zero");
    else
        xhook_clear();
}

jint Hooker::onJniRegisterMethods(JNIEnv *env, const char *class_name,
                                  const JNINativeMethod *methods, jint length) {
    std::vector<JNINativeMethod> modifiableMethodsArray(methods ,methods + length);

    return originalRegisterMethod(env , class_name , &modifiableMethodsArray[0] ,
                                  static_cast<jint>(modifiableMethodsArray.size()));
}

Hooker::FunctionJniRegisterMethods Hooker::originalRegisterMethod;