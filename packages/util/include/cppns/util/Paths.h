#pragma once

#include <string>
#include <filesystem>
#include <cassert>

#include "cppns/util/PlatformDefinition.h"

#if USING_WINDOWS
#include <windows.h>
#include <libloaderapi.h>
#endif

#if USING_LINUX
#include <unistd.h>
#endif

#if USING_APPLE
#include <mach-o/dyld.h>
#endif

//inline std::string gTempPath = std::filesystem::temp_directory_path().string();

inline std::string gExecutablePath = []() -> std::string {
    std::string path;
#if USING_WINDOWS
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    path = std::string(buffer);
#elif USING_LINUX
    char buffer[4097];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    buffer[len] = '\0';
    path = std::string(buffer);
#elif USING_APPLE
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);

    if (_NSGetExecutablePath(buffer, &size) == 0) {
        char resolved[PATH_MAX];

        if (realpath(buffer, resolved) != nullptr) {
            path = std::string(resolved);
        }
    }
#else
    std::cerr << "Not implemented" << std::endl;
#endif

#ifdef USING_DEBUG
    // Change binary dir to root instead for debug builds

    for (size_t i = path.length() - 1; i > 0; --i) {
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