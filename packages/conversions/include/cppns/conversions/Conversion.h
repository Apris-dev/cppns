// ReSharper disable CppRedundantCastExpression
#pragma once

#include <type_traits>
#include <string>

#define MAKE_CONVERTER(to, from, ...) \
    inline to convert(from, Tag<to>) __VA_ARGS__

namespace Convert {
    template<typename To> struct Tag {};

    template<typename To, typename From, typename = void>
    struct is_convertible : std::bool_constant<std::is_same_v<To, From>> {};

    template<typename To, typename From>
    struct is_convertible<To, From, std::void_t<decltype(convert(std::declval<From>(), Tag<To>{}))>> : std::true_type {};

    template<typename To, typename From>
    constexpr bool is_convertible_v = is_convertible<To, From>::value;

    template<typename To, typename From>
    requires is_convertible_v<To, From>
    constexpr To to(From f) {
        if constexpr (std::is_same_v<To, From>) {
            return f;
        } else {
            return convert(f, Tag<To>{});
        }
    }
}

// Bool Convertersions
namespace Convert {
    MAKE_CONVERTER(char, const bool x, { return x ? 't' : 'f'; })
    MAKE_CONVERTER(unsigned char, const bool x, { return x ? 't' : 'f'; })
    MAKE_CONVERTER(signed char, const bool x, { return x ? 't' : 'f'; })
    MAKE_CONVERTER(wchar_t, const bool x, { return x ? L't' : L'f'; })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const bool x, { return x ? u8't' : u8'f'; })
#endif
    MAKE_CONVERTER(char16, const bool x, { return x ? u't' : u'f'; })
    MAKE_CONVERTER(char32, const bool x, { return x ? U't' : U'f'; })

    MAKE_CONVERTER(unsigned short, const bool x, { return x ? 1u : 0u; })
    MAKE_CONVERTER(short, const bool x, { return x ? 1 : 0; })

    MAKE_CONVERTER(unsigned int, const bool x, { return x ? 1u : 0u; })
    MAKE_CONVERTER(int, const bool x, { return x ? 1 : 0; })

    MAKE_CONVERTER(unsigned long, const bool x, { return x ? 1ul : 0ul; })
    MAKE_CONVERTER(long, const bool x, { return x ? 1l : 0l; })

    MAKE_CONVERTER(unsigned long long, const bool x, { return x ? 1ull : 0ull; })
    MAKE_CONVERTER(long long, const bool x, { return x ? 1ll : 0ll; })

    MAKE_CONVERTER(float, const bool x, { return x ? 1.f : 0.f; })
    MAKE_CONVERTER(double, const bool x, { return x ? 1.0 : 0.0; })
    MAKE_CONVERTER(long double, const bool x, { return x ? 1.0 : 0.0; })

    MAKE_CONVERTER(std::string, const bool x, { return x ? "true" : "false"; })
    MAKE_CONVERTER(std::wstring, const bool x, { return x ? L"true" : L"false"; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const bool x, { return x ? u8"true" : u8"false"; })
#endif
    MAKE_CONVERTER(std::u16string, const bool x, { return x ? u"true" : u"false"; })
    MAKE_CONVERTER(std::u32string, const bool x, { return x ? U"true" : U"false"; })
}

// Char Conversions
namespace Convert {
    template <typename TType>
    constexpr bool is_char_v = std::_Is_any_of_v<std::remove_cv_t<TType>, char, signed char, unsigned char, wchar_t,
#ifdef __cpp_char8_t
        char8_t,
#endif
        char16_t, char32_t>;

    template <typename TChar>
    requires is_char_v<TChar>
    bool convert(TChar inType, Tag<bool>) {
        return inType == static_cast<TChar>('t');
    }

#define CONVERT_TO_SIMPLE(type) \
    template <typename TChar> \
    requires is_char_v<TChar> \
    type convert(TChar inType, Tag<type>) { \
        return static_cast<type>(inType); \
    }

    CONVERT_TO_SIMPLE(char)
    CONVERT_TO_SIMPLE(unsigned char)
    CONVERT_TO_SIMPLE(signed char)
    CONVERT_TO_SIMPLE(wchar_t)
    CONVERT_TO_SIMPLE(char8)
    CONVERT_TO_SIMPLE(char16)
    CONVERT_TO_SIMPLE(char32)
    CONVERT_TO_SIMPLE(unsigned short)
    CONVERT_TO_SIMPLE(short)
    CONVERT_TO_SIMPLE(unsigned int)
    CONVERT_TO_SIMPLE(int)
    CONVERT_TO_SIMPLE(unsigned long)
    CONVERT_TO_SIMPLE(long)
    CONVERT_TO_SIMPLE(unsigned long long)
    CONVERT_TO_SIMPLE(long long)
    CONVERT_TO_SIMPLE(float)
    CONVERT_TO_SIMPLE(double)
    CONVERT_TO_SIMPLE(long double)

#undef CONVERT_TO_SIMPLE

    template <typename TChar, typename TOtherChar>
    requires is_char_v<TChar> && is_char_v<TOtherChar>
    std::basic_string<TOtherChar> convert(TChar inType, Tag<std::basic_string<TOtherChar>>) {
        return std::basic_string<TOtherChar>{Convert::to<TOtherChar>(inType)};
    }
}

// Integer Conversions
namespace Convert {
    template <typename TType>
    constexpr bool is_integer_v = std::is_integral_v<TType> && !std::is_same_v<std::remove_cv_t<TType>, bool> && !is_char_v<TType>;

    template <typename TType>
    requires is_integer_v<TType>
    bool convert(TType inType, Tag<bool>) {
        return inType != 0;
    }

#define CONVERT_TO_SIMPLE(type) \
    template <typename TType> \
    requires is_integer_v<TType> \
    type convert(TType inType, Tag<type>) { \
        return static_cast<type>(inType); \
    }

    CONVERT_TO_SIMPLE(char)
    CONVERT_TO_SIMPLE(unsigned char)
    CONVERT_TO_SIMPLE(signed char)
    CONVERT_TO_SIMPLE(wchar_t)
    CONVERT_TO_SIMPLE(char8)
    CONVERT_TO_SIMPLE(char16)
    CONVERT_TO_SIMPLE(char32)
    CONVERT_TO_SIMPLE(unsigned short)
    CONVERT_TO_SIMPLE(short)
    CONVERT_TO_SIMPLE(unsigned int)
    CONVERT_TO_SIMPLE(int)
    CONVERT_TO_SIMPLE(unsigned long)
    CONVERT_TO_SIMPLE(long)
    CONVERT_TO_SIMPLE(unsigned long long)
    CONVERT_TO_SIMPLE(long long)
    CONVERT_TO_SIMPLE(float)
    CONVERT_TO_SIMPLE(double)
    CONVERT_TO_SIMPLE(long double)

#undef CONVERT_TO_SIMPLE

    template <typename TType>
    requires is_integer_v<TType>
    std::string convert(TType inType, Tag<std::string>) {
        return std::to_string(inType);
    }

    template <typename TType>
    requires is_integer_v<TType>
    std::wstring convert(TType inType, Tag<std::wstring>) {
        return std::to_wstring(inType);
    }

    template <typename TType>
    requires is_integer_v<TType>
    std::u8string convert(TType inType, Tag<std::u8string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u8string{str.begin(), str.end()};
    }

    template <typename TType>
    requires is_integer_v<TType>
    std::u16string convert(TType inType, Tag<std::u16string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u16string{str.begin(), str.end()};
    }

    template <typename TType>
    requires is_integer_v<TType>
    std::u32string convert(TType inType, Tag<std::u32string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u32string{str.begin(), str.end()};
    }
}

// Floating point Conversions
namespace Convert {
    template <typename TType>
    requires std::is_floating_point_v<TType>
    bool convert(TType inType, Tag<bool>) {
        return inType != 0.f;
    }

#define CONVERT_TO_SIMPLE(type) \
    template <typename TType> \
    requires std::is_floating_point_v<TType> \
    type convert(TType inType, Tag<type>) { \
        return static_cast<type>(inType); \
    }

    CONVERT_TO_SIMPLE(char)
    CONVERT_TO_SIMPLE(unsigned char)
    CONVERT_TO_SIMPLE(signed char)
    CONVERT_TO_SIMPLE(wchar_t)
    CONVERT_TO_SIMPLE(char8)
    CONVERT_TO_SIMPLE(char16)
    CONVERT_TO_SIMPLE(char32)
    CONVERT_TO_SIMPLE(unsigned short)
    CONVERT_TO_SIMPLE(short)
    CONVERT_TO_SIMPLE(unsigned int)
    CONVERT_TO_SIMPLE(int)
    CONVERT_TO_SIMPLE(unsigned long)
    CONVERT_TO_SIMPLE(long)
    CONVERT_TO_SIMPLE(unsigned long long)
    CONVERT_TO_SIMPLE(long long)
    CONVERT_TO_SIMPLE(float)
    CONVERT_TO_SIMPLE(double)
    CONVERT_TO_SIMPLE(long double)

#undef CONVERT_TO_SIMPLE

    template <typename TType>
    requires std::is_floating_point_v<TType>
    std::string convert(TType inType, Tag<std::string>) {
        return std::to_string(inType);
    }

    template <typename TType>
    requires std::is_floating_point_v<TType>
    std::wstring convert(TType inType, Tag<std::wstring>) {
        return std::to_wstring(inType);
    }

    template <typename TType>
    requires std::is_floating_point_v<TType>
    std::u8string convert(TType inType, Tag<std::u8string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u8string{str.begin(), str.end()};
    }

    template <typename TType>
    requires std::is_floating_point_v<TType>
    std::u16string convert(TType inType, Tag<std::u16string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u16string{str.begin(), str.end()};
    }

    template <typename TType>
    requires std::is_floating_point_v<TType>
    std::u32string convert(TType inType, Tag<std::u32string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u32string{str.begin(), str.end()};
    }
}