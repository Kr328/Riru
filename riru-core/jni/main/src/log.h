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
        LogStream(int t ,bool d = false) : stream() ,type(t) ,disable(d) {};

    public:
        std::stringstream stream;
        int  type;
        bool disable;
    };

public:
    static LogStream &info(void);
    static LogStream &error(void);
    static LogStream &warn(void);
    static LogStream &debug(void);
    static LogStream &verbose(void);
};

Log::LogStream& operator<<(Log::LogStream& log ,Log::EndType const &e);

template<typename T>
Log::LogStream& operator<<(Log::LogStream& log ,T const &v) {
    if ( log.disable ) return log;

    log.stream << v;

    return log;
}