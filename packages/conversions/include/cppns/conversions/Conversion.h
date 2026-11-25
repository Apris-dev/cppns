#pragma once

#include <type_traits>

#define MAKE_CONVERTER(to, from, func) \
    inline to convert(from, Tag<to>) func

namespace Convert {
    template<typename To> struct Tag {};

    template<typename To, typename From, typename = void>
    struct is_convertible : std::false_type {};

    template<typename To, typename From>
    struct is_convertible<To, From, std::void_t<decltype(convert(std::declval<From>(), Tag<To>{}))>> : std::true_type {};

    template<typename To, typename From>
    constexpr bool is_convertible_v = is_convertible<To, From>::value;

    template<typename To, typename From>
    constexpr To to(From f) { return convert(f, Tag<To>{}); }
}

// Bool Convert
namespace Convert {
    MAKE_CONVERTER(char, const bool b, { return b ? 't' : 'f'; })
    MAKE_CONVERTER(unsigned char, const bool b, { return b ? 't' : 'f'; })
    MAKE_CONVERTER(signed char, const bool b, { return b ? 't' : 'f'; })

    MAKE_CONVERTER(char8, const bool b, { return b ? u8't' : u8'f'; })
    MAKE_CONVERTER(char16, const bool b, { return b ? u't' : u'f'; })
    MAKE_CONVERTER(char32, const bool b, { return b ? U't' : U'f'; })

    MAKE_CONVERTER(unsigned short, const bool b, { return b ? 1u : 0u; })
    MAKE_CONVERTER(short, const bool b, { return b ? 1 : 0; })

    MAKE_CONVERTER(unsigned int, const bool b, { return b ? 1u : 0u; })
    MAKE_CONVERTER(int, const bool b, { return b ? 1 : 0; })

    MAKE_CONVERTER(unsigned long, const bool b, { return b ? 1ul : 0ul; })
    MAKE_CONVERTER(long, const bool b, { return b ? 1l : 0l; })

    MAKE_CONVERTER(unsigned long long, const bool b, { return b ? 1ull : 0ull; })
    MAKE_CONVERTER(long long, const bool b, { return b ? 1ll : 0ll; })

    MAKE_CONVERTER(float, const bool b, { return b ? 1.f : 0.f; })
    MAKE_CONVERTER(double, const bool b, { return b ? 1.0 : 0.0; })
    MAKE_CONVERTER(long double, const bool b, { return b ? 1.0 : 0.0; })

    MAKE_CONVERTER(std::string, const bool b, { return b ? "true" : "false"; })
    MAKE_CONVERTER(std::u8string, const bool b, { return b ? u8"true" : u8"false"; })
    MAKE_CONVERTER(std::u16string, const bool b, { return b ? u"true" : u"false"; })
    MAKE_CONVERTER(std::u32string, const bool b, { return b ? U"true" : U"false"; })
}

// Char Convert
namespace Convert {
    MAKE_CONVERTER(bool, const char c, { return c == 't'; })
    MAKE_CONVERTER(unsigned char, const char c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const char c, { return static_cast<signed char>(c); })

    // Char is narrower than other char types, so no data gets lost
    MAKE_CONVERTER(char8, const char c, { return static_cast<char8>(c); })
    MAKE_CONVERTER(char16, const char c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const char c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const char c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const char c, { return c; })

    MAKE_CONVERTER(unsigned int, const char c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const char c, { return c; })

    MAKE_CONVERTER(unsigned long, const char c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const char c, { return c; })

    MAKE_CONVERTER(unsigned long long, const char c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const char c, { return c; })

    MAKE_CONVERTER(float, const char c, { return c; })
    MAKE_CONVERTER(double, const char c, { return c; })
    MAKE_CONVERTER(long double, const char c, { return c; })

    MAKE_CONVERTER(std::string, const char c, { return std::string{c}; })
    MAKE_CONVERTER(std::u8string, const char c, { return std::u8string{Convert::to<char8>(c)}; })
    MAKE_CONVERTER(std::u16string, const char c, { return std::u16string{Convert::to<char16>(c)}; })
    MAKE_CONVERTER(std::u32string, const char c, { return std::u32string{Convert::to<char32>(c)}; })
}