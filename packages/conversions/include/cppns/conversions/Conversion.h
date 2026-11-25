#pragma once

#include <type_traits>
#include <string>

#define MAKE_CONVERTER(to, from, func) \
    inline to convert(from, Tag<to>) func

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

// Bool Convert
namespace Convert {
    MAKE_CONVERTER(char, const bool b, { return b ? 't' : 'f'; })
    MAKE_CONVERTER(unsigned char, const bool b, { return b ? 't' : 'f'; })
    MAKE_CONVERTER(signed char, const bool b, { return b ? 't' : 'f'; })
    MAKE_CONVERTER(wchar_t, const bool b, { return b ? L't' : L'f'; })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const bool b, { return b ? u8't' : u8'f'; })
#endif
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
    MAKE_CONVERTER(std::wstring, const bool b, { return b ? L"true" : L"false"; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const bool b, { return b ? u8"true" : u8"false"; })
#endif
    MAKE_CONVERTER(std::u16string, const bool b, { return b ? u"true" : u"false"; })
    MAKE_CONVERTER(std::u32string, const bool b, { return b ? U"true" : U"false"; })
}

// Char Convert
namespace Convert {
    MAKE_CONVERTER(bool, const char c, { return c == 't'; })

    MAKE_CONVERTER(unsigned char, const char c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const char c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const char c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const char c, { return static_cast<char8>(c); })
#endif
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
    MAKE_CONVERTER(std::wstring, const char c, { return std::wstring{Convert::to<wchar_t>(c)}; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const char c, { return std::u8string{Convert::to<char8>(c)}; })
#endif
    MAKE_CONVERTER(std::u16string, const char c, { return std::u16string{Convert::to<char16>(c)}; })
    MAKE_CONVERTER(std::u32string, const char c, { return std::u32string{Convert::to<char32>(c)}; })
}

// UnsignedChar Convert
namespace Convert {
    MAKE_CONVERTER(bool, const unsigned char c, { return c == 't'; })

    MAKE_CONVERTER(char, const unsigned char c, { return static_cast<char>(c); })
    MAKE_CONVERTER(signed char, const unsigned char c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const unsigned char c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const unsigned char c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const unsigned char c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const unsigned char c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const unsigned char c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const unsigned char c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const unsigned char c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const unsigned char c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const unsigned char c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const unsigned char c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const unsigned char c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const unsigned char c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const unsigned char c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const unsigned char c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const unsigned char c, { return static_cast<long double>(c); })

    MAKE_CONVERTER(std::string, const unsigned char c, { return std::string{Convert::to<char>(c)}; })
    MAKE_CONVERTER(std::wstring, const unsigned char c, { return std::wstring{Convert::to<wchar_t>(c)}; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const unsigned char c, { return std::u8string{Convert::to<char8>(c)}; })
#endif
    MAKE_CONVERTER(std::u16string, const unsigned char c, { return std::u16string{Convert::to<char16>(c)}; })
    MAKE_CONVERTER(std::u32string, const unsigned char c, { return std::u32string{Convert::to<char32>(c)}; })
}

// SignedChar Convert
namespace Convert {
    MAKE_CONVERTER(bool, const signed char c, { return c == 't'; })

    MAKE_CONVERTER(char, const signed char c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const signed char c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(wchar_t, const signed char c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const signed char c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const signed char c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const signed char c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const signed char c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const signed char c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const signed char c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const signed char c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const signed char c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const signed char c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const signed char c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const signed char c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const signed char c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const signed char c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const signed char c, { return static_cast<long double>(c); })

    MAKE_CONVERTER(std::string, const signed char c, { return std::string{Convert::to<char>(c)}; })
    MAKE_CONVERTER(std::wstring, const signed char c, { return std::wstring{Convert::to<wchar_t>(c)}; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const signed char c, { return std::u8string{Convert::to<char8>(c)}; })
#endif
    MAKE_CONVERTER(std::u16string, const signed char c, { return std::u16string{Convert::to<char16>(c)}; })
    MAKE_CONVERTER(std::u32string, const signed char c, { return std::u32string{Convert::to<char32>(c)}; })
}

// WChar Convert
namespace Convert {
    MAKE_CONVERTER(bool, const wchar_t c, { return c == L't'; })

    MAKE_CONVERTER(char, const wchar_t c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const wchar_t c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const wchar_t c, { return static_cast<signed char>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const wchar_t c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const wchar_t c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const wchar_t c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const wchar_t c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const wchar_t c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const wchar_t c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const wchar_t c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const wchar_t c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const wchar_t c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const wchar_t c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const wchar_t c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const wchar_t c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const wchar_t c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const wchar_t c, { return static_cast<long double>(c); })

    MAKE_CONVERTER(std::string, const wchar_t c, { return std::string{Convert::to<char>(c)}; })
    MAKE_CONVERTER(std::wstring, const wchar_t c, { return std::wstring{c}; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const wchar_t c, { return std::u8string{Convert::to<char8>(c)}; })
#endif
    MAKE_CONVERTER(std::u16string, const wchar_t c, { return std::u16string{Convert::to<char16>(c)}; })
    MAKE_CONVERTER(std::u32string, const wchar_t c, { return std::u32string{Convert::to<char32>(c)}; })
}

#ifdef __cpp_char8_t
// Char8 Convert
namespace Convert {
    MAKE_CONVERTER(bool, const char8 c, { return c == u8't'; })

    MAKE_CONVERTER(char, const char8 c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const char8 c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const char8 c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const char8 c, { return static_cast<wchar_t>(c); })

    MAKE_CONVERTER(char16, const char8 c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const char8 c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const char8 c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const char8 c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const char8 c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const char8 c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const char8 c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const char8 c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const char8 c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const char8 c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const char8 c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const char8 c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const char8 c, { return static_cast<long double>(c); })

    MAKE_CONVERTER(std::string, const char8 c, { return std::string{Convert::to<char>(c)}; })
    MAKE_CONVERTER(std::wstring, const char8 c, { return std::wstring{Convert::to<wchar_t>(c)}; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const char8 c, { return std::u8string{Convert::to<char8>(c)}; })
#endif
    MAKE_CONVERTER(std::u16string, const char8 c, { return std::u16string{Convert::to<char16>(c)}; })
    MAKE_CONVERTER(std::u32string, const char8 c, { return std::u32string{Convert::to<char32>(c)}; })
}
#endif

// Char16 Convert
namespace Convert {
    MAKE_CONVERTER(bool, const char16 c, { return c == u't'; })

    MAKE_CONVERTER(char, const char16 c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const char16 c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const char16 c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const char16 c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const char16 c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char32, const char16 c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const char16 c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const char16 c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const char16 c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const char16 c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const char16 c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const char16 c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const char16 c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const char16 c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const char16 c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const char16 c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const char16 c, { return static_cast<long double>(c); })

    MAKE_CONVERTER(std::string, const char16 c, { return std::string{Convert::to<char>(c)}; })
    MAKE_CONVERTER(std::wstring, const char16 c, { return std::wstring{Convert::to<wchar_t>(c)}; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const char16 c, { return std::u8string{Convert::to<char8>(c)}; })
#endif
    MAKE_CONVERTER(std::u16string, const char16 c, { return std::u16string{Convert::to<char16>(c)}; })
    MAKE_CONVERTER(std::u32string, const char16 c, { return std::u32string{Convert::to<char32>(c)}; })
}

// Char32 Convert
namespace Convert {
    MAKE_CONVERTER(bool, const char32 c, { return c == U't'; })

    MAKE_CONVERTER(char, const char32 c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const char32 c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const char32 c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const char32 c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const char32 c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const char32 c, { return static_cast<char16>(c); })

    MAKE_CONVERTER(unsigned short, const char32 c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const char32 c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const char32 c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const char32 c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const char32 c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const char32 c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const char32 c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const char32 c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const char32 c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const char32 c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const char32 c, { return static_cast<long double>(c); })

    MAKE_CONVERTER(std::string, const char32 c, { return std::string{Convert::to<char>(c)}; })
    MAKE_CONVERTER(std::wstring, const char32 c, { return std::wstring{Convert::to<wchar_t>(c)}; })
#ifdef __cpp_char8_t
    MAKE_CONVERTER(std::u8string, const char32 c, { return std::u8string{Convert::to<char8>(c)}; })
#endif
    MAKE_CONVERTER(std::u16string, const char32 c, { return std::u16string{Convert::to<char16>(c)}; })
    MAKE_CONVERTER(std::u32string, const char32 c, { return std::u32string{Convert::to<char32>(c)}; })
}

// UShort Convert
namespace Convert {
    MAKE_CONVERTER(bool, const unsigned short c, { return c != 0; })

    MAKE_CONVERTER(char, const unsigned short c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const unsigned short c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const unsigned short c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const unsigned short c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const unsigned short c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const unsigned short c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const unsigned short c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(short, const unsigned short c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const unsigned short c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const unsigned short c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const unsigned short c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const unsigned short c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const unsigned short c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const unsigned short c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const unsigned short c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const unsigned short c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const unsigned short c, { return static_cast<long double>(c); })
}

// Short Convert
namespace Convert {
    MAKE_CONVERTER(bool, const short c, { return c != 0; })

    MAKE_CONVERTER(char, const short c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const short c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const short c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const short c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const short c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const short c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const short c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const short c, { return static_cast<unsigned short>(c); })

    MAKE_CONVERTER(unsigned int, const short c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const short c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const short c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const short c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const short c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const short c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const short c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const short c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const short c, { return static_cast<long double>(c); })
}

// UInt Convert
namespace Convert {
    MAKE_CONVERTER(bool, const unsigned int c, { return c != 0; })

    MAKE_CONVERTER(char, const unsigned int c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const unsigned int c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const unsigned int c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const unsigned int c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const unsigned int c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const unsigned int c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const unsigned int c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const unsigned int c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const unsigned int c, { return static_cast<short>(c); })

    MAKE_CONVERTER(int, const unsigned int c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const unsigned int c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const unsigned int c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const unsigned int c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const unsigned int c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const unsigned int c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const unsigned int c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const unsigned int c, { return static_cast<long double>(c); })
}

// Int Convert
namespace Convert {
    MAKE_CONVERTER(bool, const int c, { return c != 0; })

    MAKE_CONVERTER(char, const int c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const int c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const int c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const int c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const int c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const int c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const int c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const int c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const int c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const int c, { return static_cast<unsigned int>(c); })

    MAKE_CONVERTER(unsigned long, const int c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const int c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const int c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const int c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const int c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const int c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const int c, { return static_cast<long double>(c); })
}

// ULong Convert
namespace Convert {
    MAKE_CONVERTER(bool, const unsigned long c, { return c != 0; })

    MAKE_CONVERTER(char, const unsigned long c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const unsigned long c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const unsigned long c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const unsigned long c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const unsigned long c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const unsigned long c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const unsigned long c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const unsigned long c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const unsigned long c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const unsigned long c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const unsigned long c, { return static_cast<int>(c); })

    MAKE_CONVERTER(long, const unsigned long c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const unsigned long c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const unsigned long c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const unsigned long c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const unsigned long c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const unsigned long c, { return static_cast<long double>(c); })
}

// Long Convert
namespace Convert {
    MAKE_CONVERTER(bool, const long c, { return c != 0; })

    MAKE_CONVERTER(char, const long c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const long c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const long c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const long c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const long c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const long c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const long c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const long c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const long c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const long c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const long c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const long c, { return static_cast<unsigned long>(c); })

    MAKE_CONVERTER(unsigned long long, const long c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const long c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const long c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const long c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const long c, { return static_cast<long double>(c); })
}

// ULongLong Convert
namespace Convert {
    MAKE_CONVERTER(bool, const unsigned long long c, { return c != 0; })

    MAKE_CONVERTER(char, const unsigned long long c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const unsigned long long c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const unsigned long long c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const unsigned long long c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const unsigned long long c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const unsigned long long c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const unsigned long long c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const unsigned long long c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const unsigned long long c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const unsigned long long c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const unsigned long long c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const unsigned long long c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const unsigned long long c, { return static_cast<long>(c); })

    MAKE_CONVERTER(long long, const unsigned long long c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const unsigned long long c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const unsigned long long c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const unsigned long long c, { return static_cast<long double>(c); })
}

// LongLong Convert
namespace Convert {
    MAKE_CONVERTER(bool, const long long c, { return c != 0; })

    MAKE_CONVERTER(char, const long long c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const long long c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const long long c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const long long c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const long long c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const long long c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const long long c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const long long c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const long long c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const long long c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const long long c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const long long c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const long long c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const long long c, { return static_cast<unsigned long long>(c); })

    MAKE_CONVERTER(float, const long long c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const long long c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const long long c, { return static_cast<long double>(c); })
}

// Float Convert
namespace Convert {
    MAKE_CONVERTER(bool, const float c, { return c != 0; })

    MAKE_CONVERTER(char, const float c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const float c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const float c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const float c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const float c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const float c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const float c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const float c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const float c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const float c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const float c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const float c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const float c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const float c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const float c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(double, const float c, { return static_cast<double>(c); })
    MAKE_CONVERTER(long double, const float c, { return static_cast<long double>(c); })
}

// Double Convert
namespace Convert {
    MAKE_CONVERTER(bool, const double c, { return c != 0; })

    MAKE_CONVERTER(char, const double c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const double c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const double c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const double c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const double c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const double c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const double c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const double c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const double c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const double c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const double c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const double c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const double c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const double c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const double c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const double c, { return static_cast<float>(c); })
    MAKE_CONVERTER(long double, const double c, { return static_cast<long double>(c); })
}

// LongDouble Convert
namespace Convert {
    MAKE_CONVERTER(bool, const long double c, { return c != 0; })

    MAKE_CONVERTER(char, const long double c, { return static_cast<char>(c); })
    MAKE_CONVERTER(unsigned char, const long double c, { return static_cast<unsigned char>(c); })
    MAKE_CONVERTER(signed char, const long double c, { return static_cast<signed char>(c); })
    MAKE_CONVERTER(wchar_t, const long double c, { return static_cast<wchar_t>(c); })

#ifdef __cpp_char8_t
    MAKE_CONVERTER(char8, const long double c, { return static_cast<char8>(c); })
#endif
    MAKE_CONVERTER(char16, const long double c, { return static_cast<char16>(c); })
    MAKE_CONVERTER(char32, const long double c, { return static_cast<char32>(c); })

    MAKE_CONVERTER(unsigned short, const long double c, { return static_cast<unsigned short>(c); })
    MAKE_CONVERTER(short, const long double c, { return static_cast<short>(c); })

    MAKE_CONVERTER(unsigned int, const long double c, { return static_cast<unsigned int>(c); })
    MAKE_CONVERTER(int, const long double c, { return static_cast<int>(c); })

    MAKE_CONVERTER(unsigned long, const long double c, { return static_cast<unsigned long>(c); })
    MAKE_CONVERTER(long, const long double c, { return static_cast<long>(c); })

    MAKE_CONVERTER(unsigned long long, const long double c, { return static_cast<unsigned long long>(c); })
    MAKE_CONVERTER(long long, const long double c, { return static_cast<long long>(c); })

    MAKE_CONVERTER(float, const long double c, { return static_cast<float>(c); })
    MAKE_CONVERTER(double, const long double c, { return static_cast<double>(c); })
}