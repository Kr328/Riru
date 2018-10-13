#pragma once

#ifndef DEBUG
#define DEBUG true
#endif

#ifdef __LP64__
#define APP_PROCESS_PATH "/system/bin/app_process64"
#else
#define APP_PROCESS_PATH "/system/bin/app_process"
#endif

#define CURRENT_PROCESS_EXE_PATH "/proc/self/exe"

#define PATTERN_JNI_REGISTER_METHODS_LIBRARY ".*android_runtime.*"
#define JNI_REGISTER_METHODS_SYMBOL          "jniRegisterMethods"

