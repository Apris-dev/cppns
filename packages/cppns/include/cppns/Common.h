#pragma once

#ifdef USING_BOOTSTRAPPER
    #define cppns_main EXPORTC int run
#else
    #define cppns_main int main
#endif

#define USING_CXX20 (__cplusplus >= 202002L)
#define USING_CXX23 (__cplusplus >= 202100L)

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