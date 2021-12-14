#include "util.hpp"


void systemCreateDirectory(std::string path) {
#if ARCH_WIN
    CreateDirectoryA(path.c_str(), NULL);
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
    unsigned minutes = unsigned (seconds / 60);
    return rack::string::f("%02u:%04.1f", minutes, seconds - minutes * 60);
}
