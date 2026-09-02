#pragma once

#include <cstddef>

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

#if USING_WINDOWS
    // On Windows, only two compilers are supported
    #ifdef _MSC_VER
        #define USING_MSVC 1
        #define USING_MINGW 0
    #else
        #define USING_MSVC 0
        // MinGW can use either GCC or Clang under the hood, so multiple may seem to be enabled
        #define USING_MINGW 1
    #endif
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

#include <cstdint>

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using smallest = uint8;
#if USING_WINDOWS || USING_APPLE
using largest = size_t;
#else
using largest = std::size_t;
#endif

#ifdef __cpp_char8_t
using utf8 = char8_t;
#else
using utf8 = char;
#endif
using utf16 = char16_t;
using utf32 = char32_t;

// Allows you to optionally define whether a function exists or not. Must be constexpr and static
#define ENABLE_FUNC_IF(cond) template<bool b = cond, std::enable_if_t<b, int> = 0>