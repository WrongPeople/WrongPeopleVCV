#include "util.hpp"


void systemCreateDirectory(std::string path) {
#if ARCH_WIN
    CreateDirectory(path.c_str(), NULL);
#else
    mkdir(path.c_str(), 0755);
#endif
}

bool systemIsDirectory(std::string path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf))
        return false;
    return statbuf.st_mode & S_IFDIR;
}

std::string timeToString(float seconds) {
    return rack::string::f("%02u:%02u", (unsigned) (seconds / 60), (unsigned) seconds % 60);
}
