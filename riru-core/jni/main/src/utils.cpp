#include "utils.h"

#include <cstdlib>
#include <limits.h>

string Utils::getCurrentExecutablePath(void) {
    return resolveRealPath(CURRENT_PROCESS_EXE_PATH);
}

string Utils::resolveRealPath(string const &path) {
    char buffer[PATH_MAX+1];

    if ( realpath(path.c_str() ,buffer) == nullptr )
        throw std::runtime_error(string("realpath: ") + strerror(errno));

    return string(buffer);
}
