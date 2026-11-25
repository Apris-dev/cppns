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

namespace Convert::utf {
	constexpr static bool isUCS4(const char32 cp) noexcept {
		return cp <= 0x10FFFF && (cp < 0xD800 || cp > 0xDFFF);
	}

	constexpr static char32 ReplacementChar = 0xFFFD;

	template <typename TType>
	static void appendUTF8(std::basic_string<TType>& out, char32 cp) {
		if (!isUCS4(cp)) cp = ReplacementChar;

		if (cp < 0x80) {
			out.push_back(static_cast<char>(cp));
		} else if (cp < 0x800) {
			out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
			out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
		} else if (cp < 0x10000) {
			out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
			out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
			out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
		} else {
			out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
			out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
			out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
			out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
		}
	}

	template <typename TType>
	static void appendUTF16(std::basic_string<TType>& out, char32 cp) {
		if (!isUCS4(cp)) cp = ReplacementChar;

		if (cp < 0x10000) {
			out.push_back(static_cast<TType>(cp));
		} else {
			cp -= 0x10000;
			out.push_back(static_cast<TType>(0xD800 | (cp >> 10)));
			out.push_back(static_cast<TType>(0xDC00 | (cp & 0x3FF)));
		}
	}
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

    template <typename TChar>
    requires is_char_v<TChar>
    std::basic_string<TChar> convert(const char inType, Tag<std::basic_string<TChar>>) {
        return std::basic_string<TChar>{Convert::to<TChar>(inType)};
    }

    template <typename TChar>
    requires is_char_v<TChar>
    std::basic_string<TChar> convert(const unsigned char inType, Tag<std::basic_string<TChar>>) {
        return std::basic_string<TChar>{Convert::to<TChar>(inType)};
    }

    template <typename TChar>
    requires is_char_v<TChar>
    std::basic_string<TChar> convert(const signed char inType, Tag<std::basic_string<TChar>>) {
        return std::basic_string<TChar>{Convert::to<TChar>(inType)};
    }

    template <typename TChar>
    requires is_char_v<TChar>
    std::basic_string<TChar> convert(const char8 inType, Tag<std::basic_string<TChar>>) {
        return std::basic_string<TChar>{Convert::to<TChar>(inType)};
    }

    inline std::string convert(const wchar_t inType, Tag<std::string>) {
    	std::string str;
    	str.reserve(sizeof(wchar_t));
    	const auto c = Convert::to<char32>(inType);
    	utf::appendUTF8(str, c);
    	return str;
    }

    inline std::wstring convert(const wchar_t inType, Tag<std::wstring>) {
        return std::wstring{inType};
    }

    inline std::u8string convert(const wchar_t inType, Tag<std::u8string>) {
    	std::u8string str;
    	str.reserve(sizeof(wchar_t));
    	const auto c = Convert::to<char32>(inType);
    	utf::appendUTF8(str, c);
    	return str;
    }

    //TODO: wchar_t is char16 on windows, char32 on linux
    inline std::u16string convert(const wchar_t inType, Tag<std::u16string>) {
        // WChar is 32-bit on Linux
        if constexpr (sizeof(wchar_t) > sizeof(std::u16string::value_type)) {
            std::u16string str;
            str.reserve(sizeof(wchar_t) / sizeof(std::u16string::value_type));
            const auto c = Convert::to<char32>(inType);
            utf::appendUTF16(str, c);
            return str;
        } else {
            return std::u16string{Convert::to<char16>(inType)};
        }
    }

    inline std::u32string convert(const wchar_t inType, Tag<std::u32string>) {
        return std::u32string{Convert::to<char32>(inType)};
    }

    inline std::string convert(const char16 inType, Tag<std::string>) {
        std::string str;
        str.reserve(sizeof(char16));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF8(str, c);
        return str;
    }

    inline std::wstring convert(const char16 inType, Tag<std::wstring>) {
        return std::wstring{Convert::to<wchar_t>(inType)};
    }

    inline std::u8string convert(const char16 inType, Tag<std::u8string>) {
        std::u8string str;
        str.reserve(sizeof(char16));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF8(str, c);
        return str;
    }

    inline std::u16string convert(const char16 inType, Tag<std::u16string>) {
        return std::u16string{Convert::to<char16>(inType)};
    }

    inline std::u32string convert(const char16 inType, Tag<std::u32string>) {
        return std::u32string{Convert::to<char32>(inType)};
    }

    inline std::string convert(const char32 inType, Tag<std::string>) {
        std::string str;
        str.reserve(sizeof(char32));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF8(str, c);
        return str;
    }

    inline std::wstring convert(const char32 inType, Tag<std::wstring>) {
        // WChar is 32-bit on Linux
        if constexpr (sizeof(wchar_t) > sizeof(std::u16string::value_type)) {
            return std::wstring{Convert::to<wchar_t>(inType)};
        } else {
            std::wstring str;
            str.reserve(sizeof(char32) / sizeof(std::wstring::value_type));
            const auto c = Convert::to<char32>(inType);
            utf::appendUTF16(str, c);
            return str;
        }
    }

    inline std::u8string convert(const char32 inType, Tag<std::u8string>) {
        std::u8string str;
        str.reserve(sizeof(char32));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF8(str, c);
        return str;
    }

    inline std::u16string convert(const char32 inType, Tag<std::u16string>) {
        std::u16string str;
        str.reserve(sizeof(char32) / sizeof(std::u16string::value_type));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF16(str, c);
        return str;
    }

    inline std::u32string convert(const char32 inType, Tag<std::u32string>) {
        return std::u32string{Convert::to<char32>(inType)};
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