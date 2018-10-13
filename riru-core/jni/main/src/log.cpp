#include "log.h"

Log::EndType Log::END;

Log::LogStream Log::i(ANDROID_LOG_INFO);
Log::LogStream Log::e(ANDROID_LOG_ERROR);
Log::LogStream Log::w(ANDROID_LOG_WARN);
Log::LogStream Log::d(ANDROID_LOG_DEBUG ,!DEBUG);
Log::LogStream Log::v(ANDROID_LOG_VERBOSE);

std::ostringstream& operator<<(std::ostringstream& s ,Log::EndType t) {
    return s;
}