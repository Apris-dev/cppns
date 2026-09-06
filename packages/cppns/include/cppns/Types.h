#pragma once

#include <cstddef>
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