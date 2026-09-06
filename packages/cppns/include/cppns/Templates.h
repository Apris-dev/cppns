#pragma once

// Allows you to optionally define whether a function exists or not. Must be constexpr and static
#if USING_CXX20
#define REQUIRES(...) > \
    requires (__VA_ARGS__)

#define REQUIRES_STATIC(...) REQUIRES(__VA_ARGS__)

#define _ENABLE_FUNC_IF(bname, ...) template<bool CONCAT(b, bname) = __VA_ARGS__> requires (CONCAT(b, bname))
#else
#define REQUIRES(...) , \
        std::enable_if_t<__VA_ARGS__, int> = 0 \
    >

#define _REQUIRES_STATIC_NAME(bname, ...) , bool CONCAT(b, bname) = __VA_ARGS__, \
        std::enable_if_t<CONCAT(b, bname), int> = 0 \
    >

#define REQUIRES_STATIC(...) _REQUIRES_STATIC_NAME(__COUNTER__, __VA_ARGS__)

#define _ENABLE_FUNC_IF(bname, ...) template<bool CONCAT(b, bname) = __VA_ARGS__, std::enable_if_t<CONCAT(b, bname), int> = 0>
#endif

#define ENABLE_FUNC_IF(...) _ENABLE_FUNC_IF(__COUNTER__, __VA_ARGS__)
