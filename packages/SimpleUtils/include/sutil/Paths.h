#pragma once

#include <string>
#include <filesystem>
#include <cassert>

#include "sutil/PlatformDefinition.h"

#ifdef _WIN32
#include <windows.h>
#include <libloaderapi.h>
#endif

#ifdef __linux__
#include <unistd.h>
#endif

//inline std::string gTempPath = std::filesystem::temp_directory_path().string();

inline std::string gExecutablePath = []() -> std::string {
    std::string path;
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    path = std::string(buffer);
#elif defined(__linux__)
    char buffer[4097];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    buffer[len] = '\0';
    path = std::string(buffer);
#elif defined(__APPLE__) //TODO: MacOS
    std::cerr << "Not implemented" << std::endl;
#else
    std::cerr << "Not implemented" << std::endl;
#endif

#ifdef USING_DEBUG
    // Change binary dir to root instead for debug builds
    for (size_t i = 0; i < path.length(); ++i) {
        if (path.substr(0, i) == DEBUG_BINARY_ROOT_DIR) {
            path = DEBUG_ROOT_DIR + path.substr(i);
            break;
        }
    }
#endif

    const auto loc = path.find_last_of(PATH_SEPARATOR, path.size() - 2);
    assert(loc != std::string::npos);
    path.erase(loc + 1);

    return path;
}();