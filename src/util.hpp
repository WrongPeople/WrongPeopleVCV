#pragma once

#include "rack.hpp"

#include <string>
#include <sys/stat.h>


#if ARCH_WIN

    #include <windows.h>
    #include <windowsx.h>
	#include <shellapi.h>

    #define DIR_SEP "\\"

#else

    #define DIR_SEP "/"

#endif


void systemCreateDirectoryA(std::string path);
bool systemIsDirectory(std::string path);
std::string timeToString(float seconds);
