#pragma once

#include <cstdlib>
#include <optional>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <vector>
#include <iostream>

#if USING_WINDOWS
#include <windows.h>
#include <libloaderapi.h>
#define PATH_SEPARATOR '\\'
#define PATH_LIST_SEPARATOR ';'
#define LINE_ENDING "\r\n"
#define USING_LF 0
#define USING_CRLF 1
#elif USING_LINUX
#include <unistd.h>
#define PATH_SEPARATOR '/'
#define PATH_LIST_SEPARATOR ':'
#define LINE_ENDING "\n"
#define USING_LF 1
#define USING_CRLF 0
#elif USING_APPLE
#include <mach-o/dyld.h>
#define PATH_SEPARATOR '/'
#define PATH_LIST_SEPARATOR ':'
#define LINE_ENDING "\n"
#define USING_LF 1
#define USING_CRLF 0
#endif

// The env var set by the bootstrapper to the executable's path
// If not set, gExecutablePath will fallback to bare OS calls
#define CPPNS_EXECUTABLE_PATH_ENV "CPPNS_EXECUTABLE_PATH"

// Use platform specific env functions to avoid warnings and keep thread safety
namespace Path {
    inline std::optional<std::string> getEnv(const char* name) {
#if USING_MSVC
        char* buffer = nullptr;
        size_t size = 0;
        if (_dupenv_s(&buffer, &size, name) != 0 || buffer == nullptr) {
            return std::nullopt;
        }
        std::string value(buffer);
        free(buffer);
        return value;
#else
        const char* value = std::getenv(name);
        if (value == nullptr) {
            return std::nullopt;
        }
        return std::string(value);
#endif
    }

    inline void setEnv(const char* name, const char* value) {
#if USING_WINDOWS
        _putenv_s(name, value);
#else
        setenv(name, value, 1);
#endif
    }

    inline void clearEnv(const char* name) {
#if USING_WINDOWS
        _putenv_s(CPPNS_EXECUTABLE_PATH_ENV, "");
#else
        unsetenv(CPPNS_EXECUTABLE_PATH_ENV);
#endif
    }
}

//inline std::string gTempPath = std::filesystem::temp_directory_path().string();

inline std::string gExecutablePath = []() -> std::string {
    std::string path;

    // Try to look for the executable path set by the bootstrapper, if it was set
    if (const auto envDir = Path::getEnv(CPPNS_EXECUTABLE_PATH_ENV); envDir.has_value() && !envDir->empty()) {
        path = *envDir;

        if (path.back() != PATH_SEPARATOR) {
            path += PATH_SEPARATOR;
        }
    } else {
#if USING_WINDOWS
        std::vector<char> buffer(MAX_PATH);
        while (true) {
            const DWORD len = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));

            if (len == 0)
                throw std::runtime_error("GetModuleFileNameA failed to resolve the executable path!");

            if (len < buffer.size()) {
                path.assign(buffer.data(), len);
                break;
            }

            // This happens when the path is truncated, so double the buffer and retry.
            buffer.resize(buffer.size() * 2);
        }
#elif USING_LINUX
        std::vector<char> buffer(4096);
        while (true) {
            const ssize_t len = readlink("/proc/self/exe", buffer.data(), buffer.size());

            if (len < 0)
                throw std::runtime_error("readlink(\"/proc/self/exe\") failed to resolve the executable path!");

            if (static_cast<size_t>(len) < buffer.size()) {
                path.assign(buffer.data(), static_cast<size_t>(len));
                break;
            }

            // This happens when the path is truncated, so double the buffer and retry.
            buffer.resize(buffer.size() * 2);
        }
#elif USING_APPLE
        std::vector<char> buffer(PATH_MAX);
        uint32_t size = static_cast<uint32_t>(buffer.size());

        if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
            // Retry with the size that GetExecutablePath returned
            buffer.resize(size);
            if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
                throw std::runtime_error("_NSGetExecutablePath failed to resolve the executable path!");
            }
        }

        char resolved[PATH_MAX];
        if (realpath(buffer.data(), resolved) == nullptr) {
            throw std::runtime_error("realpath failed to resolve the executable path!");
        }
        path = std::string(resolved);
#else
        throw std::runtime_error("Could not resolve executable path, platform not implemented!");
#endif

        const auto loc = path.find_last_of(PATH_SEPARATOR, path.size() - 2);
        if (loc == std::string::npos) {
            throw std::runtime_error("Resolved executable path has no directory separator: " + path);
        }
        path.erase(loc + 1);
    }

#ifdef USING_DEBUG
    // Change binary dir to root instead for debug builds
    for (size_t i = path.length() - 1; i > 0; --i) {
        if (path.substr(0, i) == DEBUG_BINARY_ROOT_DIR) {
            path = DEBUG_ROOT_DIR + path.substr(i);
            break;
        }
    }
#endif

    return path;
}();