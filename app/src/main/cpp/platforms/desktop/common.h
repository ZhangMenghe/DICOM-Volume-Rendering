#ifndef PLATFORMS_DESKTOP_COMMON_H
#define PLATFORMS_DESKTOP_COMMON_H

#include <string>
#include <iostream>
#define STRING(...) std::string(__VA_ARGS__)
#define PATH(FILENAME) std::string(RESOURCE_DESKTOP_DIR) +std::string(FILENAME)

#endif
