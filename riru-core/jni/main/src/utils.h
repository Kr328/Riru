#pragma once

#include <string>
#include <vector>
#include <dirent.h>

#include "config.h"
#include "log.h"

using std::string;
using std::vector;

class Utils {
public:
    static string getCurrentExecutablePath(void);
    static string resolveRealPath(string const &path);

public:
    static vector<string> listDirectory(string const &path ,bool with_prefix ,int type = DT_REG | DT_DIR);
};
