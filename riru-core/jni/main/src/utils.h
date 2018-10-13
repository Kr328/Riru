#pragma once

#include <string>

#include "config.h"
#include "log.h"

using std::string;

class Utils {
public:
    static string getCurrentExecutablePath(void);
    static string resolveRealPath(string const &path);
};
