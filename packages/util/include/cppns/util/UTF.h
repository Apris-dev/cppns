#pragma once

#include <bit>
#include <string>

namespace UTF {

    // Convert lowercase to cppns standard
    enum class Endian : uint8 {
#ifdef __cpp_lib_endian
        LITTLE = static_cast<uint8>(std::endian::little),
        BIG = static_cast<uint8>(std::endian::big),
        NATIVE = static_cast<uint8>(std::endian::native)
#else
        LITTLE = 0,
        BIG = 1,
    #if USING_MSVC // Windows: assume little endian
        NATIVE = LITTLE
    #else
        // GCC/Clang check for byte order macros, if they don't exist, assume little endian
        #if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            NATIVE = BIG
        #else
            NATIVE = LITTLE
        #endif
    #endif
#endif
    };

    template <typename TChar>
    constexpr static TChar swapBytes(const TChar c) noexcept {
        return c;
    }

    template <>
    constexpr wchar_t swapBytes<wchar_t>(const wchar_t c) noexcept {
#if USING_WINDOWS
        return static_cast<char16>((c << 8) | (c >> 8));
#else
        return ((c & 0x000000FFu) << 24) | ((c & 0x0000FF00u) << 8) |
               ((c & 0x00FF0000u) >> 8)  | ((c & 0xFF000000u) >> 24);
#endif
    }

    template <>
    constexpr char16 swapBytes<char16>(const char16 c) noexcept {
        return static_cast<char16>((c << 8) | (c >> 8));
    }

    template <>
    constexpr char32 swapBytes<char32>(const char32 c) noexcept {
        return ((c & 0x000000FFu) << 24) | ((c & 0x0000FF00u) << 8) |
               ((c & 0x00FF0000u) >> 8)  | ((c & 0xFF000000u) >> 24);
    }

    // Gives back the unit in the byte order we want, swapping only if the machine's own order is different.
    template <typename TType>
    constexpr static TType toEndian(TType c, const Endian endian) noexcept {
        if (endian == Endian::NATIVE)
            return c;
        return swapBytes(c);
    }

	constexpr static char32 ReplacementChar = 0xFFFD;

    // A lead char is when the first char in a codepoint
    // In the case of ASCII, it is also the only char
    // lo and hi are the ranges that are possible for the second byte
    template <typename TSize>
    struct LeadCharInfo {
        size_t length;
        TSize lo, hi;
    };

    constexpr static LeadCharInfo<uint8> classifyLeadChar(const uint8 b0) noexcept {
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

    constexpr static LeadCharInfo<uint16> classifyLeadChar16(const uint16 b0) noexcept {
        if (b0 < 0xD800 || b0 > 0xDFFF)
            return {.length = 1, .lo = 0, .hi = 0};
        if (b0 <= 0xDBFF)
            return {.length = 2, .lo = 0xDC00, .hi = 0xDFFF};
        return {.length = 0, .lo = 0, .hi = 0};
    }

    constexpr static bool isASCII(const char c) noexcept {
        return classifyLeadChar(c).length == 1;
    }

    constexpr static bool isUCS2(const char16 c) noexcept {
        return classifyLeadChar16(c).length <= 2;
    }

    constexpr static bool isUCS4(const char32 cp) noexcept {
        return cp <= 0x10FFFF && (cp < 0xD800 || cp > 0xDFFF);
    }

	template <typename TChar>
    constexpr static void appendUTF(std::basic_string<TChar>& out, char32 cp, const Endian endian = Endian::NATIVE) {
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

    template <>
    constexpr void appendUTF<char16>(std::basic_string<char16>& out, char32 cp, const Endian endian) {
        if (!isUCS4(cp)) cp = ReplacementChar;

        if (cp < 0x10000) {
            out.push_back(toEndian(static_cast<char16>(cp), endian));
        } else {
            cp -= 0x10000;
            out.push_back(toEndian(static_cast<char16>(0xD800 | (cp >> 10)), endian));
            out.push_back(toEndian(static_cast<char16>(0xDC00 | (cp & 0x3FF)), endian));
        }
    }

    template <>
    constexpr void appendUTF<char32>(std::basic_string<char32>& out, char32 cp, const Endian endian) {
        if (!isUCS4(cp)) cp = ReplacementChar;
        out.push_back(toEndian(cp, endian));
    }

    template <>
    constexpr void appendUTF<wchar_t>(std::basic_string<wchar_t>& out, char32 cp, const Endian endian) {
#if USING_WINDOWS
        if (!isUCS4(cp)) cp = ReplacementChar;

        if (cp < 0x10000) {
            out.push_back(toEndian(static_cast<char16>(cp), endian));
        } else {
            cp -= 0x10000;
            out.push_back(toEndian(static_cast<char16>(0xD800 | (cp >> 10)), endian));
            out.push_back(toEndian(static_cast<char16>(0xDC00 | (cp & 0x3FF)), endian));
        }
#else
        if (!isUCS4(cp)) cp = ReplacementChar;
        out.push_back(toEndian(cp, endian));
#endif
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
	constexpr static DecodeResult decodeUTF(const std::basic_string_view<TChar> str, const size_t i) {
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
	template <>
	constexpr DecodeResult decodeUTF<char16>(const std::basic_string_view<char16> s, const size_t i) {
		if (i >= s.size())
			return {.cp = ReplacementChar, .length = 0, .valid = false};

		const char16 b0 = s[i];
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
		const char16 second = s[i + 1];
		if (second < info.lo || second > info.hi)
			return {.cp = ReplacementChar, .length = 1, .valid = false};

		// Add the two halves together
		const char32 cp = 0x10000 + ((static_cast<char32>(b0) - 0xD800) << 10)
									+ (static_cast<char32>(second) - 0xDC00);

		return {.cp = cp, .length = 2, .valid = true};
	}

    template <>
    constexpr DecodeResult decodeUTF<char32>(const std::basic_string_view<char32> s, const size_t i) {
        if (i >= s.size())
            return {.cp = ReplacementChar, .length = 0, .valid = false};

        const char32 cp = s[i];
        if (!isUCS4(cp))
            return {.cp = ReplacementChar, .length = 1, .valid = false};

        return {.cp = cp, .length = 1, .valid = true};
    }

    template <>
    constexpr DecodeResult decodeUTF<wchar_t>(const std::basic_string_view<wchar_t> s, const size_t i) {
#if USING_WINDOWS
	    if (i >= s.size())
	        return {.cp = ReplacementChar, .length = 0, .valid = false};

	    const char16 b0 = s[i];
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
	    const char16 second = s[i + 1];
	    if (second < info.lo || second > info.hi)
	        return {.cp = ReplacementChar, .length = 1, .valid = false};

	    // Add the two halves together
	    const char32 cp = 0x10000 + ((static_cast<char32>(b0) - 0xD800) << 10)
                                    + (static_cast<char32>(second) - 0xDC00);

	    return {.cp = cp, .length = 2, .valid = true};
#else
	    if (i >= s.size())
	        return {.cp = ReplacementChar, .length = 0, .valid = false};

	    const char32 cp = s[i];
	    if (!isUCS4(cp))
	        return {.cp = ReplacementChar, .length = 1, .valid = false};

	    return {.cp = cp, .length = 1, .valid = true};
#endif
	}

	struct EncodedChar {
		char bytes[5];
	};

    constexpr static EncodedChar encodeUTF8(const char32& cp) noexcept {
	    if (cp < 0x80)
	        return {static_cast<char>(cp)};
	    if (cp < 0x800)
	        return {static_cast<char>(0xC0 | (cp >> 6)),
                static_cast<char>(0x80 | (cp & 0x3F)),
                '\0'};
	    if (cp < 0x10000)
	        return {static_cast<char>(0xE0 | (cp >> 12)),
                static_cast<char>(0x80 | ((cp >> 6) & 0x3F)),
                static_cast<char>(0x80 | (cp & 0x3F)),
                '\0'};

	    return {static_cast<char>(0xF0 | (cp >> 18)),
            static_cast<char>(0x80 | ((cp >> 12) & 0x3F)),
            static_cast<char>(0x80 | ((cp >> 6) & 0x3F)),
            static_cast<char>(0x80 | (cp & 0x3F)),
            '\0'};
	}

	constexpr static EncodedChar encodeUTF8(const char16& cp) noexcept {
    	return encodeUTF8(static_cast<char32>(cp));
    }

	// Remove invalid UTF8
	template <typename TChar>
	constexpr static void removeInvalidUTF8(std::basic_string<TChar>& s) noexcept {
    	const size_t n = s.size();
    	// where we read from and where the next good byte goes
    	size_t r = 0, w = 0;

    	while (r < n) {
    		const DecodeResult ch = decodeUTF<TChar>(s, r);

    		if (ch.valid) {
    			// Only copy if something was deleted
    			if (w != r)
    				for (size_t k = 0; k < ch.length; ++k)
    					s[w + k] = s[r + k];
    			w += ch.length;
    		}
    		// Continue reading past regardless of validity
    		r += ch.length;
    	}

    	// Cut off the leftover tail
    	s.resize(w);
    }
}
