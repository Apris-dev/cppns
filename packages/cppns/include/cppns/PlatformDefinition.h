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

#if ((defined(__GNUC__) || defined(__GNUG__)) && !USING_CLANG)
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

#if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64)
    #define USING_X64 1
    #define USING_64_BIT 1
#elif defined(__i386__) || defined(_M_IX86)
    #define USING_X86 1
    #define USING_32_BIT 1
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    #define USING_ARM64 1
    #define USING_64_BIT 1
#elif defined(__arm__) || defined(_M_ARM)
    #define USING_ARM 1
    #define USING_32_BIT 1
#else
    #error "Unsupported CPU architecture"
#endif

#ifndef USING_64_BIT
    #define USING_64_BIT 0
#endif

#ifndef USING_32_BIT
    #define USING_32_BIT 0
#endif

#ifndef USING_X64
    #define USING_X64 0
#endif

#ifndef USING_X86
    #define USING_X86 0
#endif

#ifndef USING_ARM64
    #define USING_ARM64 0
#endif

#ifndef USING_ARM
    #define USING_ARM 0
#endif