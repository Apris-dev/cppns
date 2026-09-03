#pragma once

// Allows you to optionally define whether a function exists or not. Must be constexpr and static
#define ENABLE_FUNC_IF(cond) template<bool b = cond, std::enable_if_t<b, int> = 0>
