#include "log.h"

Log::EndType Log::END;

Log::LogStream& operator<<(Log::LogStream& log ,Log::EndType const &e) {
    if ( log.disable ) return log;

    __android_log_print(log.type ,TAG ,"%s" ,log.stream.str().c_str());
    log.stream.str("");

    return log;
}

Log::LogStream &Log::info(void) {
    static LogStream *instance = new LogStream(ANDROID_LOG_INFO);
    return *instance;
}

Log::LogStream &Log::error(void) {
    static LogStream *instance = new LogStream(ANDROID_LOG_ERROR);
    return *instance;
}

Log::LogStream &Log::warn(void) {
    static LogStream *instance = new LogStream(ANDROID_LOG_WARN);
    return *instance;
}

Log::LogStream &Log::debug(void) {
    static LogStream *instance = new LogStream(ANDROID_LOG_DEBUG ,!DEBUG);
    return *instance;
}

Log::LogStream &Log::verbose(void) {
    static LogStream *instance = new LogStream(ANDROID_LOG_VERBOSE);
    return *instance;
}
