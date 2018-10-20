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
#define JNI_REGISTER_METHODS_SYMBOL          "jniRegisterNativeMethods"

#define RIRU_MODULES_CONFIGS_DIR             "/data/misc/riru/modules"

#define NATIVE_FORK_AND_SPECIALIZE_METHOD "nativeForkAndSpecialize"
#define NATIVE_FORK_SYSTEM_SERVER_METHOD  "nativeForkSystemServer"

#define NATIVE_FORK_AND_SPECIALIZE_MARSHMALLOW_SIGNATURE "(II[II[[IILjava/lang/String;Ljava/lang/String;[ILjava/lang/String;Ljava/lang/String;)I"
#define NATIVE_FORK_AND_SPECIALIZE_OREO_SIGNATURE        "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[ILjava/lang/String;Ljava/lang/String;)I"
#define NATIVE_FORK_AND_SPECIALIZE_PIE_SIGNATURE         "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;)I"
#define NATIVE_FORK_SYSTEM_SERVER_SIGNATURE              "(II[II[[IJJ)I"