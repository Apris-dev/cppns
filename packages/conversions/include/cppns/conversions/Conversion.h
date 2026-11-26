// ReSharper disable CppRedundantCastExpression
#pragma once

#include <type_traits>
#include <string>
#include "cppns/util/fast_float.h"

#define MAKE_CONVERTER(to, from, ...) \
    constexpr to convert(from, Tag<to>) __VA_ARGS__

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

	template <typename TChar>
	constexpr static void appendUTF8(std::basic_string<TChar>& out, char32 cp) {
		if (!isUCS4(cp)) cp = ReplacementChar;

		if (cp < 0x80) {
			out.push_back(static_cast<TChar>(cp));
		} else if (cp < 0x800) {
			out.push_back(static_cast<TChar>(0xC0 | (cp >> 6)));
			out.push_back(static_cast<TChar>(0x80 | (cp & 0x3F)));
		} else if (cp < 0x10000) {
			out.push_back(static_cast<TChar>(0xE0 | (cp >> 12)));
			out.push_back(static_cast<TChar>(0x80 | ((cp >> 6) & 0x3F)));
			out.push_back(static_cast<TChar>(0x80 | (cp & 0x3F)));
		} else {
			out.push_back(static_cast<TChar>(0xF0 | (cp >> 18)));
			out.push_back(static_cast<TChar>(0x80 | ((cp >> 12) & 0x3F)));
			out.push_back(static_cast<TChar>(0x80 | ((cp >> 6) & 0x3F)));
			out.push_back(static_cast<TChar>(0x80 | (cp & 0x3F)));
		}
	}

	template <typename TChar>
	constexpr static void appendUTF16(std::basic_string<TChar>& out, char32 cp) {
		if (!isUCS4(cp)) cp = ReplacementChar;

		if (cp < 0x10000) {
			out.push_back(static_cast<TChar>(cp));
		} else {
			cp -= 0x10000;
			out.push_back(static_cast<TChar>(0xD800 | (cp >> 10)));
			out.push_back(static_cast<TChar>(0xDC00 | (cp & 0x3FF)));
		}
	}

    // A lead char is when the first char in a codepoint
    // In the case of ASCII, it is also the only char
    // lo and hi are the ranges that are possible for the second byte
    struct LeadCharInfo {
	    size_t length;
	    uint8 lo, hi;
	};

    struct LeadCharInfo16 {
        size_t length;
        uint16 lo, hi;
    };

    constexpr static LeadCharInfo classifyLeadChar(const uint8 b0) noexcept {
	    if (b0 < 0x80)
	        return {.length = 1, .lo = 0x80, .hi = 0xBF};
	    if (b0 < 0xC2)
	        return {.length = 0, .lo = 0, .hi = 0};
	    if (b0 <= 0xDF)
	        return {.length = 2, .lo = 0x80, .hi = 0xBF};
	    if (b0 == 0xE0)
	        return {.length = 3, .lo = 0xA0, .hi = 0xBF};
	    if (b0 == 0xED)
	        return {.length = 3, .lo = 0x80, .hi = 0x9F};
	    if (b0 <= 0xEF)
	        return {.length = 3, .lo = 0x80, .hi = 0xBF};
	    if (b0 == 0xF0)
	        return {.length = 4, .lo = 0x90, .hi = 0xBF};
	    if (b0 == 0xF4)
	        return {.length = 4, .lo = 0x80, .hi = 0x8F};
	    if (b0 <= 0xF3)
	        return {.length = 4, .lo = 0x80, .hi = 0xBF};
	    return {.length = 0, .lo = 0, .hi = 0};
	}

    constexpr static LeadCharInfo16 classifyLeadChar16(const uint16 b0) noexcept {
	    if (b0 < 0xD800 || b0 > 0xDFFF)
	        return {.length = 1, .lo = 0, .hi = 0};
	    if (b0 <= 0xDBFF)
	        return {.length = 2, .lo = 0xDC00, .hi = 0xDFFF};
	    return {.length = 0, .lo = 0, .hi = 0};
	}

    // Result of decode
    struct DecodeResult {
        char32 cp;
        size_t length;
        bool valid;
    };

    // Decodes UTF8 into a UTF32 char, assumes std::string_view contains UTF8
	// If non-UTF8 is present, it is replaced with 'ReplacementChar' and marked as invalid
	template <typename TChar>
	constexpr static DecodeResult decodeUTF8(const std::basic_string<TChar>& str, const size_t i) {
		if (i >= str.size())
				return {.cp = ReplacementChar, .length = 0, .valid = false};

		const auto b0 = static_cast<unsigned char>(str[i]);
		const auto [length, lo, hi] = classifyLeadChar(b0);

		if (length == 0)
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		if (length == 1)
			return {.cp = b0, .length = 1, .valid = true};

		// Keep the useful bits of the first byte (the rest just says the length).
		char32 cp = b0 & (0x7F >> length);

		for (size_t k = 1; k < length; ++k) {
			if (i + k >= str.size())
				return {.cp = ReplacementChar, .length = k, .valid = false};

			const auto b = static_cast<unsigned char>(str[i + k]);

			if (!(k == 1 ? b >= lo && b <= hi : (b & 0xC0) == 0x80))
				return {.cp = ReplacementChar, .length = k, .valid = false};

			// Add this byte's 6 bits
			cp = (cp << 6) | (b & 0x3F);
		}

		return {.cp = cp, .length = length, .valid = true};
	}

	// Decodes UTF16 into a UTF32 char
	template <typename TChar>
	constexpr static DecodeResult decodeUTF16(const std::basic_string<TChar>& s, const size_t i) noexcept {
		if (i >= s.size())
			return {.cp = ReplacementChar, .length = 0, .valid = false};

		const TChar b0 = s[i];
		const auto info = classifyLeadChar16(b0);

		// Invalid
		if (info.length == 0)
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		// Codepoint fits inside the 2 bytes, ie: not surrogate
		if (info.length == 1)
			return {.cp = b0, .length = 1, .valid = true};

		// First half with no second half
		if (i + 1 >= s.size())
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		// First half and second half cannot fit together
		const TChar second = s[i + 1];
		if (second < info.lo || second > info.hi)
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		// Add the two halves together
		const char32 cp = 0x10000 + ((static_cast<char32>(b0) - 0xD800) << 10)
									+ (static_cast<char32>(second) - 0xDC00);

		return {.cp = cp, .length = 2, .valid = true};
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
    	utf::appendUTF8(str, c);
    	return str;
    }

    constexpr std::wstring convert(const wchar_t& inType, Tag<std::wstring>) {
        return std::wstring{inType};
    }

    constexpr std::u8string convert(const wchar_t& inType, Tag<std::u8string>) {
    	std::u8string str;
    	str.reserve(sizeof(wchar_t));
    	const auto c = Convert::to<char32>(inType);
    	utf::appendUTF8(str, c);
    	return str;
    }

    constexpr std::u16string convert(const wchar_t& inType, Tag<std::u16string>) {
#if USING_WINDOWS
        return std::u16string{Convert::to<char16>(inType)};
#else
        std::u16string str;
        str.reserve(sizeof(wchar_t) / sizeof(std::u16string::value_type));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF16(str, c);
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
        utf::appendUTF8(str, c);
        return str;
    }

    constexpr std::wstring convert(const char16& inType, Tag<std::wstring>) {
        return std::wstring{Convert::to<wchar_t>(inType)};
    }

    constexpr std::u8string convert(const char16& inType, Tag<std::u8string>) {
        std::u8string str;
        str.reserve(sizeof(char16));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF8(str, c);
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
        utf::appendUTF8(str, c);
        return str;
    }

    constexpr std::wstring convert(const char32& inType, Tag<std::wstring>) {
#if USING_WINDOWS
        std::wstring str;
        str.reserve(sizeof(char32) / sizeof(std::wstring::value_type));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF16(str, c);
        return str;
#else
        return std::wstring{Convert::to<wchar_t>(inType)};
#endif
    }

    constexpr std::u8string convert(const char32& inType, Tag<std::u8string>) {
        std::u8string str;
        str.reserve(sizeof(char32));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF8(str, c);
        return str;
    }

    constexpr std::u16string convert(const char32& inType, Tag<std::u16string>) {
        std::u16string str;
        str.reserve(sizeof(char32) / sizeof(std::u16string::value_type));
        const auto c = Convert::to<char32>(inType);
        utf::appendUTF16(str, c);
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
    constexpr bool convert(const std::string& inType, Tag<bool>) { return inType == "true"; }
    constexpr bool convert(const std::wstring& inType, Tag<bool>) { return inType == L"true"; }
    constexpr bool convert(const std::u8string& inType, Tag<bool>) { return inType == u8"true"; }
    constexpr bool convert(const std::u16string& inType, Tag<bool>) { return inType == u"true"; }
    constexpr bool convert(const std::u32string& inType, Tag<bool>) { return inType == U"true"; }

    template <typename To, typename TChar>
    requires is_char_v<TChar> && is_char_v<To>
    constexpr To convert(const std::basic_string<TChar>& inType, Tag<To>) {
        return Convert::to<To>(inType.at(0));
    }

    template <typename To, typename TChar>
    requires fast_float::is_supported_char_type<TChar>::value && (fast_float::is_supported_integer_type<To>::value || fast_float::is_supported_float_type<To>::value) && (!is_char_v<To>) && (!std::is_same_v<To, bool>)
    constexpr To convert(const std::basic_string<TChar>& inType, Tag<To>) {
        To to;
        fast_float::from_chars<To>(inType.data(), inType.data() + inType.size(), to);
        return to;
    }

    template <typename TChar>
    constexpr long double convert(const std::basic_string<TChar>& inType, Tag<long double>) {
        double to;
        fast_float::from_chars(inType.data(), inType.data() + inType.size(), to);
        // Probably lose precision but it shouldn't matter much
        return static_cast<long double>(to);
    }

    constexpr std::wstring convert(const std::string& inType, Tag<std::wstring>) {
#if USING_WINDOWS
        std::wstring str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF8(inType, i);
            utf::appendUTF16(str, res.cp);
            i += res.length;
        }
        return str;
#else
        std::wstring str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF8(inType, i);
            str.push_back(res.cp);
            i += res.length;
        }
        return str;
#endif
    }

    constexpr std::u8string convert(const std::string& inType, Tag<std::u8string>) {
        return std::u8string{inType.begin(), inType.end()};
    }

    constexpr std::u16string convert(const std::string& inType, Tag<std::u16string>) {
        std::u16string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF8(inType, i);
            utf::appendUTF16(str, res.cp);
            i += res.length;
        }
        return str;
    }

    constexpr std::u32string convert(const std::string& inType, Tag<std::u32string>) {
        std::u32string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF8(inType, i);
            str.push_back(res.cp);
            i += res.length;
        }
        return str;
    }

    constexpr std::string convert(const std::wstring& inType, Tag<std::string>) {
#if USING_WINDOWS
        std::string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF16(inType, i);
            utf::appendUTF8(str, res.cp);
            i += res.length;
        }
        return str;
#else
        std::string str;
        str.reserve(inType.size());
        for (const auto& c : inType)
            utf::appendUTF8(str, c);
        return str;
#endif
    }

    constexpr std::u8string convert(const std::wstring& inType, Tag<std::u8string>) {
#if USING_WINDOWS
        std::u8string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF16(inType, i);
            utf::appendUTF8(str, res.cp);
            i += res.length;
        }
        return str;
#else
        std::u8string str;
        str.reserve(inType.size());
        for (const auto& c : inType)
            utf::appendUTF8(str, c);
        return str;
#endif
    }

    constexpr std::u16string convert(const std::wstring& inType, Tag<std::u16string>) {
#if USING_WINDOWS
            return std::u16string{inType.begin(), inType.end()};
#else
        std::u16string str;
        str.reserve(inType.size());
        for (const auto& c : inType)
            utf::appendUTF16(str, c);
        return str;
#endif
    }

    constexpr std::u32string convert(const std::wstring& inType, Tag<std::u32string>) {
#if USING_WINDOWS
        std::u32string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF16(inType, i);
            str.push_back(res.cp);
            i += res.length;
        }
        return str;
#else
        return std::u32string{inType.begin(), inType.end()};
#endif
    }

    constexpr std::string convert(const std::u8string& inType, Tag<std::string>) {
        return std::string{inType.begin(), inType.end()};
    }

    constexpr std::wstring convert(const std::u8string& inType, Tag<std::wstring>) {
#if USING_WINDOWS
        std::wstring str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF8(inType, i);
            utf::appendUTF16(str, res.cp);
            i += res.length;
        }
        return str;
#else
        std::wstring str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF8(inType, i);
            str.push_back(res.cp);
            i += res.length;
        }
        return str;
#endif
    }

    constexpr std::u16string convert(const std::u8string& inType, Tag<std::u16string>) {
        std::u16string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF8(inType, i);
            utf::appendUTF16(str, res.cp);
            i += res.length;
        }
        return str;
    }

    constexpr std::u32string convert(const std::u8string& inType, Tag<std::u32string>) {
        std::u32string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF8(inType, i);
            str.push_back(res.cp);
            i += res.length;
        }
        return str;
    }

    constexpr std::string convert(const std::u16string& inType, Tag<std::string>) {
        std::string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF16(inType, i);
            utf::appendUTF8(str, res.cp);
            i += res.length;
        }
        return str;
    }

    constexpr std::wstring convert(const std::u16string& inType, Tag<std::wstring>) {
#if USING_WINDOWS
        std::wstring str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF16(inType, i);
            utf::appendUTF16(str, res.cp);
            i += res.length;
        }
        return str;
#else
        std::wstring str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF16(inType, i);
            str.push_back(res.cp);
            i += res.length;
        }
        return str;
#endif
    }

    constexpr std::u8string convert(const std::u16string& inType, Tag<std::u8string>) {
        std::u8string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF16(inType, i);
            utf::appendUTF8(str, res.cp);
            i += res.length;
        }
        return str;
    }

    constexpr std::u32string convert(const std::u16string& inType, Tag<std::u32string>) {
        std::u32string str;
        str.reserve(inType.size());
        for (size_t i = 0; i < inType.size();) {
            const auto res = utf::decodeUTF16(inType, i);
            str.push_back(res.cp);
            i += res.length;
        }
        return str;
    }

    constexpr std::string convert(const std::u32string& inType, Tag<std::string>) {
        std::string str;
        str.reserve(inType.size());
        for (const char32 c : inType)
            utf::appendUTF8(str, c);
        return str;
    }

    constexpr std::wstring convert(const std::u32string& inType, Tag<std::wstring>) {
#if USING_WINDOWS
        std::wstring str;
        str.reserve(inType.size());
        for (const char32 c : inType)
            utf::appendUTF16(str, c);
        return str;
#else
        std::wstring str;
        str.reserve(inType.size());
        for (const char32 c : inType)
            str.push_back(c);
        return str;
#endif
    }

    constexpr std::u8string convert(const std::u32string& inType, Tag<std::u8string>) {
        std::u8string str;
        str.reserve(inType.size());
        for (const char32 c : inType)
            utf::appendUTF8(str, c);
        return str;
    }

    constexpr std::u16string convert(const std::u32string& inType, Tag<std::u16string>) {
        std::u16string str;
        str.reserve(inType.size());
        for (const char32 c : inType)
            utf::appendUTF16(str, c);
        return str;
    }
}