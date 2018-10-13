#pragma once

#include <cerrno>
#include <string>
#include <sstream>
#include <exception>
#include <type_traits>
#include <android/log.h>

#include "config.h"

#define TAG "Riru"

using std::string;
using std::exception;

class Log {
public:
    class EndType {} static END;
    class LogStream {
    public:
        LogStream(int t ,bool d = false) : type(t) ,disable(d) {};

    public:
        std::ostringstream stream;
        int type;
        bool disable;
    };

public:
    static LogStream i;
    static LogStream e;
    static LogStream w;
    static LogStream d;
    static LogStream v;
};

std::ostringstream& operator<<(std::ostringstream& s ,Log::EndType t);

template<typename T>
Log::LogStream& operator<<(Log::LogStream& log ,T const &v) {
    if ( log.disable ) return log;

    if ( std::is_same<Log::EndType ,T>::value ) {
        __android_log_print(log.type ,TAG ,"%s" ,log.stream.str().c_str());
        log.stream.str(string());
    }
    else
        log.stream << v;
    return log;
}