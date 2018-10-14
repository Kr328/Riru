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

vector<string> Utils::listDirectory(string const &path ,bool with_prefix ,int type) {
    DIR *dir = opendir(path.c_str());
    if ( dir == nullptr )
        throw std::runtime_error("opendir " + path + " : " + strerror(errno));

    vector<string> result;

    errno = 0;
    dirent *ent = nullptr;
    while ((ent = readdir(dir))) {
        if ( ent->d_type & type )
            result.push_back(with_prefix ? path + "/" + ent->d_name : ent->d_name);
    }

    if ( errno != 0 )
        throw std::runtime_error("readdir " + path + " : " + strerror(errno));

    return result;
}
