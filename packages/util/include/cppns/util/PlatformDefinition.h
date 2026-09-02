#pragma once

/*
 * The Class that helps define platform specific things
 * For example: path separators, line endings, and the size of 'long'
 */

// TODO: support more than linux, windows, and macOS.  Would need help from outside to support mobile platforms and etc.

#define USING_CXX20 (__cplusplus >= 202002L)
#define USING_CXX23 (__cplusplus >= 202100L)

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


#if USING_CXX20
    #define constexpr_20 constexpr
#else
    #define constexpr_20
#endif

#if USING_CXX23
    #define constexpr_23 constexpr
#else
    #define constexpr_23
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

#ifdef USING_BOOTSTRAPPER
    #define cppns_main EXPORTC int run
#else
    #define cppns_main int main
#endif

// Allows you to optionally define whether a function exists or not. Must be constexpr and static
#define ENABLE_FUNC_IF(cond) template<bool b = cond, std::enable_if_t<b, int> = 0>