#pragma once

/*
 * The Class that helps define platform specific things
 * For example: path separators, line endings, and the size of 'long'
 */

// TODO: support more than linux, windows, and macOS.  Would need help from outside to support mobile platforms and etc.

#ifdef _WIN32
    #define USING_WINDOWS 1
#else
    #define USING_WINDOWS 0
#endif

#ifdef __linux__
    #define USING_LINUX 1
#else
    #define USING_LINUX 0
#endif

#ifdef __APPLE__
    #define USING_APPLE 1
#else
    #define USING_APPLE 0
#endif

#ifdef __clang__
    #define USING_CLANG 1
#else
    #define USING_CLANG 0
#endif

#if ((defined(__GNUC__) || defined(__GNUG__)) && !defined(USING_CLANG))
    #define USING_GCC 1
#else
    #define USING_GCC 0
#endif

#if USING_WINDOWS
    // On Windows, only two compilers are supported
    #ifdef _MSC_VER
        #define USING_MSVC 1
    #else
        #define USING_MSVC 0
    #endif

        // MinGW uses either GCC or Clang under the hood
    #if USING_GCC || USING_CLANG
        #define USING_MINGW 1
    #else
        #define USING_MINGW 0
    #endif
#endif

#if USING_WINDOWS
    #define PATH_SEPARATOR '\\'
    #define LINE_ENDING "\r\n"
#else
    #define PATH_SEPARATOR '/'
    #define LINE_ENDING "\n"
#endif

#if USING_MSVC
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

#define EXPORTC extern "C" EXPORT