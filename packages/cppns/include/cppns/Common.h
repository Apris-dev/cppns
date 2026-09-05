#pragma once

#ifdef USING_BOOTSTRAPPER
    #define cppns_main EXPORTC int run
#else
    #define cppns_main int main
#endif

// MSVC might not set __cplusplus properly
#if defined(_MSVC_LANG) && _MSVC_LANG > __cplusplus
#define USING_CXX11 (_MSVC_LANG > 199711L)
#define USING_CXX14 (_MSVC_LANG > 201103L)
#define USING_CXX17 (_MSVC_LANG > 201402L)
#define USING_CXX20 (_MSVC_LANG > 201703L)
#define USING_CXX23 (_MSVC_LANG > 202002L)
#define USING_CXX26 (_MSVC_LANG > 202302L)
#else
#define USING_CXX11 (__cplusplus > 199711L)
#define USING_CXX14 (__cplusplus > 201103L)
#define USING_CXX17 (__cplusplus > 201402L)
#define USING_CXX20 (__cplusplus > 201703L)
#define USING_CXX23 (__cplusplus > 202002L)
#define USING_CXX26 (__cplusplus > 202302L)
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