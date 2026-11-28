// ReSharper disable CppRedundantCastExpression
#pragma once

#include <type_traits>
#include <string>
#include "cppns/util/fast_float.h"
#include "cppns/util/UTF.h"

#define MAKE_CONVERTER(to, from, ...) \
    constexpr to convert(from, Tag<to>) __VA_ARGS__

namespace Convert {
    template<typename To> struct Tag {};

    template<typename To, typename... TArgs>
    concept is_convertible_v = (sizeof...(TArgs) == 1 && (std::is_same_v<To, TArgs> && ...)) ||
    requires(TArgs... args) {
        convert(args..., Tag<To>{});
    };

    template<typename To, typename From, typename... TFromArgs>
    requires is_convertible_v<To, From, TFromArgs...>
    constexpr To to(const From& from, TFromArgs... fromArgs) {
        if constexpr (std::is_same_v<To, From> && sizeof...(TFromArgs) <= 0) {
            return from;
        } else {
            return convert(from, std::forward<TFromArgs>(fromArgs)..., Tag<To>{});
        }
    }
}


// Bool Conversions
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
    template <typename TType, typename... TArgs>
    constexpr bool any_of_v = (std::is_same_v<TType, TArgs> || ...);

    template <typename TType>
    constexpr bool is_char_v = any_of_v<std::remove_cv_t<TType>, char, signed char, unsigned char, wchar_t,
#ifdef __cpp_char8_t
        char8_t,
#endif
        char16_t, char32_t>;

    template <typename TChar>
    requires is_char_v<TChar>
    constexpr bool convert(const TChar& inType, Tag<bool>) {
        return inType == static_cast<TChar>('t');
    }

#define CONVERT_TO_SIMPLE(type) \
    template <typename TChar> \
    requires is_char_v<TChar> \
    constexpr type convert(const TChar& inType, Tag<type>) { \
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

    template <typename TChar>
    requires is_char_v<TChar>
    constexpr std::basic_string<TChar> convert(const char& inType, Tag<std::basic_string<TChar>>) {
        return std::basic_string<TChar>{Convert::to<TChar>(inType)};
    }

    template <typename TChar>
    requires is_char_v<TChar>
    constexpr std::basic_string<TChar> convert(const unsigned char& inType, Tag<std::basic_string<TChar>>) {
        return std::basic_string<TChar>{Convert::to<TChar>(inType)};
    }

    template <typename TChar>
    requires is_char_v<TChar>
    constexpr std::basic_string<TChar> convert(const signed char& inType, Tag<std::basic_string<TChar>>) {
        return std::basic_string<TChar>{Convert::to<TChar>(inType)};
    }

    template <typename TChar>
    requires is_char_v<TChar>
    constexpr std::basic_string<TChar> convert(const char8& inType, Tag<std::basic_string<TChar>>) {
        return std::basic_string<TChar>{Convert::to<TChar>(inType)};
    }

    constexpr std::string convert(const wchar_t& inType, Tag<std::string>) {
    	std::string str;
    	str.reserve(sizeof(wchar_t));
    	const auto c = Convert::to<char32>(inType);
    	UTF::appendUTF(str, c);
    	return str;
    }

    constexpr std::wstring convert(const wchar_t& inType, Tag<std::wstring>) {
        return std::wstring{inType};
    }

    constexpr std::u8string convert(const wchar_t& inType, Tag<std::u8string>) {
    	std::u8string str;
    	str.reserve(sizeof(wchar_t));
    	const auto c = Convert::to<char32>(inType);
    	UTF::appendUTF(str, c);
    	return str;
    }

    constexpr std::u16string convert(const wchar_t& inType, Tag<std::u16string>) {
#if USING_WINDOWS
        return std::u16string{Convert::to<char16>(inType)};
#else
        std::u16string str;
        str.reserve(sizeof(wchar_t) / sizeof(std::u16string::value_type));
        const auto c = Convert::to<char32>(inType);
        UTF::appendUTF(str, c);
        return str;
#endif
    }

    constexpr std::u32string convert(const wchar_t& inType, Tag<std::u32string>) {
        return std::u32string{Convert::to<char32>(inType)};
    }

    constexpr std::string convert(const char16& inType, Tag<std::string>) {
        std::string str;
        str.reserve(sizeof(char16));
        const auto c = Convert::to<char32>(inType);
        UTF::appendUTF(str, c);
        return str;
    }

    constexpr std::wstring convert(const char16& inType, Tag<std::wstring>) {
        return std::wstring{Convert::to<wchar_t>(inType)};
    }

    constexpr std::u8string convert(const char16& inType, Tag<std::u8string>) {
        std::u8string str;
        str.reserve(sizeof(char16));
        const auto c = Convert::to<char32>(inType);
        UTF::appendUTF(str, c);
        return str;
    }

    constexpr std::u16string convert(const char16& inType, Tag<std::u16string>) {
        return std::u16string{Convert::to<char16>(inType)};
    }

    constexpr std::u32string convert(const char16& inType, Tag<std::u32string>) {
        return std::u32string{Convert::to<char32>(inType)};
    }

    constexpr std::string convert(const char32& inType, Tag<std::string>) {
        std::string str;
        str.reserve(sizeof(char32));
        const auto c = Convert::to<char32>(inType);
        UTF::appendUTF(str, c);
        return str;
    }

    constexpr std::wstring convert(const char32& inType, Tag<std::wstring>) {
#if USING_WINDOWS
        std::wstring str;
        str.reserve(sizeof(char32) / sizeof(std::wstring::value_type));
        const auto c = Convert::to<char32>(inType);
        UTF::appendUTF(str, c);
        return str;
#else
        return std::wstring{Convert::to<wchar_t>(inType)};
#endif
    }

    constexpr std::u8string convert(const char32& inType, Tag<std::u8string>) {
        std::u8string str;
        str.reserve(sizeof(char32));
        const auto c = Convert::to<char32>(inType);
        UTF::appendUTF(str, c);
        return str;
    }

    constexpr std::u16string convert(const char32& inType, Tag<std::u16string>) {
        std::u16string str;
        str.reserve(sizeof(char32) / sizeof(std::u16string::value_type));
        const auto c = Convert::to<char32>(inType);
        UTF::appendUTF(str, c);
        return str;
    }

    constexpr std::u32string convert(const char32& inType, Tag<std::u32string>) {
        return std::u32string{Convert::to<char32>(inType)};
    }
}

// Integer Conversions
namespace Convert {
    template <typename TType>
    constexpr bool is_integer_v = std::is_integral_v<TType> && !std::is_same_v<std::remove_cv_t<TType>, bool> && !is_char_v<TType>;

    template <typename TType>
    requires is_integer_v<TType>
    constexpr bool convert(const TType& inType, Tag<bool>) {
        return inType != 0;
    }

#define CONVERT_TO_SIMPLE(type) \
    template <typename TType> \
    requires is_integer_v<TType> \
    constexpr type convert(const TType& inType, Tag<type>) { \
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
    constexpr std::string convert(const TType& inType, Tag<std::string>) {
        return std::to_string(inType);
    }

    template <typename TType>
    requires is_integer_v<TType>
    constexpr std::wstring convert(const TType& inType, Tag<std::wstring>) {
        return std::to_wstring(inType);
    }

    template <typename TType>
    requires is_integer_v<TType>
    constexpr std::u8string convert(const TType& inType, Tag<std::u8string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u8string{str.begin(), str.end()};
    }

    template <typename TType>
    requires is_integer_v<TType>
    constexpr std::u16string convert(const TType& inType, Tag<std::u16string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u16string{str.begin(), str.end()};
    }

    template <typename TType>
    requires is_integer_v<TType>
    constexpr std::u32string convert(const TType& inType, Tag<std::u32string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u32string{str.begin(), str.end()};
    }
}

// Floating point Conversions
namespace Convert {
    template <typename TType>
    requires std::is_floating_point_v<TType>
    constexpr bool convert(const TType& inType, Tag<bool>) {
        return inType != 0.f;
    }

#define CONVERT_TO_SIMPLE(type) \
    template <typename TType> \
    requires std::is_floating_point_v<TType> \
    constexpr type convert(const TType& inType, Tag<type>) { \
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
    constexpr std::string convert(const TType& inType, Tag<std::string>) {
        return std::to_string(inType);
    }

    template <typename TType>
    requires std::is_floating_point_v<TType>
    constexpr std::wstring convert(const TType& inType, Tag<std::wstring>) {
        return std::to_wstring(inType);
    }

    template <typename TType>
    requires std::is_floating_point_v<TType>
    constexpr std::u8string convert(const TType& inType, Tag<std::u8string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u8string{str.begin(), str.end()};
    }

    template <typename TType>
    requires std::is_floating_point_v<TType>
    constexpr std::u16string convert(const TType& inType, Tag<std::u16string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u16string{str.begin(), str.end()};
    }

    template <typename TType>
    requires std::is_floating_point_v<TType>
    constexpr std::u32string convert(const TType& inType, Tag<std::u32string>) {
        auto str = Convert::to<std::string>(inType);
        return std::u32string{str.begin(), str.end()};
    }
}

// String Conversions
namespace Convert {
    constexpr bool convert(const std::string_view inType, Tag<bool>) { return inType == "true"; }
    constexpr bool convert(const std::wstring_view inType, Tag<bool>) { return inType == L"true"; }
    constexpr bool convert(const std::u8string_view inType, Tag<bool>) { return inType == u8"true"; }
    constexpr bool convert(const std::u16string_view inType, Tag<bool>) { return inType == u"true"; }
    constexpr bool convert(const std::u32string_view inType, Tag<bool>) { return inType == U"true"; }

    template <typename To, typename TChar>
    requires is_char_v<TChar> && is_convertible_v<To, std::basic_string_view<TChar>>
    constexpr To convert(const std::basic_string<TChar>& inType, Tag<To>) {
        return convert(std::basic_string_view<TChar>{inType}, Tag<To>{});
    }

    template <typename To, typename TChar>
    requires is_char_v<TChar> && is_convertible_v<To, std::basic_string_view<TChar>>
    constexpr To convert(const std::basic_string<TChar>& inType, UTF::Endian endian, Tag<To>) {
        return convert(std::basic_string_view<TChar>{inType}, endian, Tag<To>{});
    }

    template <typename To, typename TChar>
    requires is_char_v<TChar> && is_char_v<To>
    constexpr To convert(const std::basic_string_view<TChar> inType, Tag<To>) {
        return Convert::to<To>(inType.at(0));
    }

    template <typename To, typename TChar>
    requires fast_float::is_supported_char_type<TChar>::value && (fast_float::is_supported_integer_type<To>::value || fast_float::is_supported_float_type<To>::value) && (!is_char_v<To>) && (!std::is_same_v<To, bool>)
    constexpr To convert(const std::basic_string_view<TChar> inType, Tag<To>) {
        To to;
        fast_float::from_chars<To>(inType.data(), inType.data() + inType.size(), to);
        return to;
    }

    template <typename TChar>
    constexpr long double convert(const std::basic_string_view<TChar> inType, Tag<long double>) {
        double to;
        fast_float::from_chars(inType.data(), inType.data() + inType.size(), to);
        // Probably lose precision but it shouldn't matter much
        return static_cast<long double>(to);
    }

    template <typename TChar, typename TOtherChar>
    requires is_char_v<TChar> && is_char_v<TOtherChar>
    constexpr std::basic_string<TOtherChar> convert(const std::basic_string_view<TChar> inType, Tag<std::basic_string<TOtherChar>>) {
        // Fast path: they use the same size char type it can be copied directly
        if constexpr (sizeof(TChar) == sizeof(TOtherChar)) {
            return std::basic_string<TOtherChar>{inType.begin(), inType.end()};
        } else {
            std::basic_string<TOtherChar> str;
            str.reserve(inType.size());
            for (size_t i = 0; i < inType.size();) {
                const auto res = UTF::decodeUTF(inType, i);
                UTF::appendUTF(str, res.cp);
                i += res.length;
            }
            return str;
        }
    }

    template <typename TChar, typename TOtherChar>
    requires is_char_v<TChar> && is_char_v<TOtherChar>
    constexpr std::basic_string<TOtherChar> convert(const std::basic_string_view<TChar> inType, UTF::Endian endian, Tag<std::basic_string<TOtherChar>>) {
        // Cant use the fast path if we need to switch endian
        if constexpr (sizeof(TChar) == sizeof(TOtherChar) && endian == UTF::Endian::NATIVE) {
            return std::basic_string<TOtherChar>{inType.begin(), inType.end()};
        } else {
            std::basic_string<TOtherChar> str;
            str.reserve(inType.size());
            for (size_t i = 0; i < inType.size();) {
                const auto res = UTF::decodeUTF(inType, i);
                UTF::appendUTF(str, res.cp, endian);
                i += res.length;
            }
            return str;
        }
    }
}

// Array Conversions
namespace Convert {
    template <typename TChar, typename TType>
    requires is_char_v<TChar> && is_convertible_v<TType, std::basic_string_view<TChar>>
    constexpr TType convert(const TChar* inType, Tag<TType>) {
        return Convert::to<TType>(std::basic_string_view<TChar>(inType));
    }

    template <typename TChar, typename TType>
    requires is_char_v<TChar> && is_convertible_v<TType, std::basic_string_view<TChar>>
    constexpr TType convert(const TChar* inType, const size_t inCount, Tag<TType>) {
        return Convert::to<TType>(std::basic_string_view<TChar>(inType, inCount));
    }
}