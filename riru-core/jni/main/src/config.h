#pragma once

#ifndef DEBUG
#define DEBUG true
#endif

#ifdef __LP64__
#define APP_PROCESS_PATH "/system/bin/app_process64"
#define RIRU_MODULES_LIBRARY_PATH(name) ("/system/lib64/libriru_" + name + ".so")
#else
#define APP_PROCESS_PATH "/system/bin/app_process32"
#define RIRU_MODULES_LIBRARY_PATH(name) ("/system/lib/libriru_" + name + ".so")
#endif

#define CURRENT_PROCESS_EXE_PATH "/proc/self/exe"

#define PATTERN_JNI_REGISTER_METHODS_LIBRARY ".*android_runtime.*"
#define JNI_REGISTER_METHODS_SYMBOL          "jniRegisterMethods"

#define RIRU_MODULES_CONFIGS_DIR             "/data/riru/modules"
