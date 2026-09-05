#pragma once

#include "PlatformDefinition.h"

#define _CPPNS_CONCAT(x, y) x##y
#define CONCAT(x, y) _CPPNS_CONCAT(x, y)

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

// MSVC might not set __cplusplus properly
#if defined(_MSVC_LANG) && _MSVC_LANG > __cplusplus
    #define CXX_VERSION _MSVC_LANG
#else
    #define CXX_VERSION __cplusplus
#endif

#define USING_CXX11 (CXX_VERSION > 199711L)
#define USING_CXX14 (CXX_VERSION > 201103L)
#define USING_CXX17 (CXX_VERSION > 201402L)
#define USING_CXX20 (CXX_VERSION > 201703L)
#define USING_CXX23 (CXX_VERSION > 202002L)
#define USING_CXX26 (CXX_VERSION > 202302L)

#if USING_CXX17
    #define constexpr_17 constexpr
#else
    #define constexpr_17 inline
#endif

#if USING_CXX20
    #define constexpr_20 constexpr
#else
    #define constexpr_20 inline
#endif

#if USING_CXX23
    #define constexpr_23 constexpr
#else
    #define constexpr_23 inline
#endif