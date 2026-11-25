#include <iostream>

#include "cppns/conversions/Conversion.h"
#include "cppns/string/String.h"

cppns_main() {

    std::cout << "Began Bool Tests" << std::endl;

    assert(Convert::to<char>(false) == 'f');
    assert(Convert::to<unsigned char>(true) == 't');
    assert(Convert::to<signed char>(false) == 'f');
    assert(Convert::to<wchar_t>(true) == L't');
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(false) == u8'f');
#endif
    assert(Convert::to<char16>(false) == u'f');
    assert(Convert::to<char32>(true) == U't');
    assert(Convert::to<unsigned short>(false) == 0);
    assert(Convert::to<short>(true) == 1);
    assert(Convert::to<unsigned int>(false) == 0);
    assert(Convert::to<int>(true) == 1);
    assert(Convert::to<unsigned long>(false) == 0ul);
    assert(Convert::to<long>(true) == 1l);
    assert(Convert::to<unsigned long long>(false) == 0ull);
    assert(Convert::to<long long>(true) == 1ll);
    assert(Convert::to<float>(false) == 0.f);
    assert(Convert::to<double>(true) == 1.0);
    assert(Convert::to<long double>(false) == 0.0);
    assert(Convert::to<std::string>(false) == "false");
    assert(Convert::to<std::wstring>(true) == L"true");
#ifdef __cpp_char8_t
    assert(Convert::to<std::u8string>(false) == u8"false");
#endif
    assert(Convert::to<std::u16string>(true) == u"true");
    assert(Convert::to<std::u32string>(false) == U"false");

    std::cout << "Began Char Tests" << std::endl;

    assert(Convert::to<bool>('t') == true);
    assert(Convert::to<bool>('a') == false);
    assert(Convert::to<unsigned char>('c') == 'c');
    assert(Convert::to<signed char>('t') == 't');
    assert(Convert::to<wchar_t>('h') == L'h');
#ifdef __cpp_char8_t
    assert(Convert::to<char8>('e') == u8'e');
#endif
    assert(Convert::to<char16>('u') == u'u');
    assert(Convert::to<char32>('k') == U'k');
    assert(Convert::to<unsigned short>('k') == 'k');
    assert(Convert::to<short>('p') == 'p');
    assert(Convert::to<unsigned int>('z') == 'z');
    assert(Convert::to<int>('v') == 'v');
    assert(Convert::to<unsigned long>('[') == '[');
    assert(Convert::to<long>('\0') == '\0');
    assert(Convert::to<unsigned long long>('\n') == '\n');
    assert(Convert::to<long long>('t') == 't');
    assert(Convert::to<float>('a') == 'a');
    assert(Convert::to<double>(';') == ';');
    assert(Convert::to<long double>('\'') == '\'');
    assert(Convert::to<std::string>('h') == "h");
    assert(Convert::to<std::wstring>('q') == L"q");
#ifdef __cpp_char8_t
    assert(Convert::to<std::u8string>('p') == u8"p");
#endif
    assert(Convert::to<std::u16string>('2') == u"2");
    assert(Convert::to<std::u32string>('n') == U"n");

    std::cout << "Began UnsignedChar Tests" << std::endl;

    assert((Convert::to<bool, unsigned char>('t') == true));
    assert((Convert::to<char, unsigned char>('t') == 't'));
    assert((Convert::to<signed char, unsigned char>('t') == 't'));
    assert((Convert::to<wchar_t, unsigned char>('t') == 't'));
#ifdef __cpp_char8_t
    assert((Convert::to<char8, unsigned char>('t') == 't'));
#endif
    assert((Convert::to<char16, unsigned char>('t') == 't'));
    assert((Convert::to<char32, unsigned char>('t') == 't'));
    assert((Convert::to<unsigned short, unsigned char>('t') == 't'));
    assert((Convert::to<short, unsigned char>('t') == 't'));
    assert((Convert::to<unsigned int, unsigned char>('t') == 't'));
    assert((Convert::to<int, unsigned char>('t') == 't'));
    assert((Convert::to<unsigned long, unsigned char>('t') == 't'));
    assert((Convert::to<long, unsigned char>('t') == 't'));
    assert((Convert::to<unsigned long long, unsigned char>('t') == 't'));
    assert((Convert::to<long long, unsigned char>('t') == 't'));
    assert((Convert::to<float, unsigned char>('t') == 't'));
    assert((Convert::to<double, unsigned char>('t') == 't'));
    assert((Convert::to<long double, unsigned char>('t') == 't'));
    assert((Convert::to<std::string, unsigned char>('t') == std::string{'t'}));
    assert((Convert::to<std::wstring, unsigned char>('t') == std::wstring{'t'}));
#ifdef __cpp_char8_t
    assert((Convert::to<std::u8string, unsigned char>('t') == std::u8string{'t'}));
#endif
    assert((Convert::to<std::u16string, unsigned char>('t') == std::u16string{'t'}));
    assert((Convert::to<std::u32string, unsigned char>('t') == std::u32string{'t'}));

    std::cout << "Began SignedChar Tests" << std::endl;

    assert((Convert::to<bool, signed char>('t') == true));
    assert((Convert::to<char, signed char>('t') == 't'));
    assert((Convert::to<unsigned char, signed char>('t') == 't'));
    assert((Convert::to<wchar_t, signed char>('t') == 't'));
#ifdef __cpp_char8_t
    assert((Convert::to<char8, signed char>('t') == 't'));
#endif
    assert((Convert::to<char16, signed char>('t') == 't'));
    assert((Convert::to<char32, signed char>('t') == 't'));
    assert((Convert::to<unsigned short, signed char>('t') == 't'));
    assert((Convert::to<short, signed char>('t') == 't'));
    assert((Convert::to<unsigned int, signed char>('t') == 't'));
    assert((Convert::to<int, signed char>('t') == 't'));
    assert((Convert::to<unsigned long, signed char>('t') == 't'));
    assert((Convert::to<long, signed char>('t') == 't'));
    assert((Convert::to<unsigned long long, signed char>('t') == 't'));
    assert((Convert::to<long long, signed char>('t') == 't'));
    assert((Convert::to<float, signed char>('t') == 't'));
    assert((Convert::to<double, signed char>('t') == 't'));
    assert((Convert::to<long double, signed char>('t') == 't'));
    assert((Convert::to<std::string, signed char>('t') == std::string{'t'}));
    assert((Convert::to<std::wstring, signed char>('t') == std::wstring{'t'}));
#ifdef __cpp_char8_t
    assert((Convert::to<std::u8string, signed char>('t') == std::u8string{'t'}));
#endif
    assert((Convert::to<std::u16string, signed char>('t') == std::u16string{'t'}));
    assert((Convert::to<std::u32string, signed char>('t') == std::u32string{'t'}));

    std::cout << "Began WChar Tests" << std::endl;

    assert(Convert::to<bool>(L't') == true);
    assert(Convert::to<char>(L't') == 't');
    assert(Convert::to<unsigned char>(L't') == 't');
    assert(Convert::to<signed char>(L't') == 't');
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(L't') == u8't');
#endif
    assert(Convert::to<char16>(L't') == u't');
    assert(Convert::to<char32>(L't') == U't');
    assert(Convert::to<unsigned short>(L't') == 't');
    assert(Convert::to<short>(L't') == 't');
    assert(Convert::to<unsigned int>(L't') == 't');
    assert(Convert::to<int>(L't') == 't');
    assert(Convert::to<unsigned long>(L't') == 't');
    assert(Convert::to<long>(L't') == 't');
    assert(Convert::to<unsigned long long>(L't') == 't');
    assert(Convert::to<long long>(L't') == 't');
    assert(Convert::to<float>(L't') == 't');
    assert(Convert::to<double>(L't') == 't');
    assert(Convert::to<long double>(L't') == 't');
    assert(Convert::to<std::string>(L't') == std::string{'t'});
    assert(Convert::to<std::wstring>(L't') == std::wstring{L't'});
#ifdef __cpp_char8_t
    assert(Convert::to<std::u8string>(L't') == std::u8string{u8't'});
#endif
    assert(Convert::to<std::u16string>(L't') == std::u16string{u't'});
    assert(Convert::to<std::u32string>(L't') == std::u32string{U't'});

#ifdef __cpp_char8_t
    std::cout << "Began Char8 Tests" << std::endl;

    assert(Convert::to<bool>(u8't') == true);
    assert(Convert::to<char>(u8't') == 't');
    assert(Convert::to<unsigned char>(u8't') == 't');
    assert(Convert::to<signed char>(u8't') == 't');
    assert(Convert::to<wchar_t>(u8't') == L't');
    assert(Convert::to<char16>(u8't') == u't');
    assert(Convert::to<char32>(u8't') == U't');
    assert(Convert::to<unsigned short>(u8't') == 't');
    assert(Convert::to<short>(u8't') == 't');
    assert(Convert::to<unsigned int>(u8't') == 't');
    assert(Convert::to<int>(u8't') == 't');
    assert(Convert::to<unsigned long>(u8't') == 't');
    assert(Convert::to<long>(u8't') == 't');
    assert(Convert::to<unsigned long long>(u8't') == 't');
    assert(Convert::to<long long>(u8't') == 't');
    assert(Convert::to<float>(u8't') == 't');
    assert(Convert::to<double>(u8't') == 't');
    assert(Convert::to<long double>(u8't') == 't');
    assert(Convert::to<std::string>(u8't') == std::string{'t'});
    assert(Convert::to<std::wstring>(u8't') == std::wstring{L't'});
    assert(Convert::to<std::u8string>(u8't') == std::u8string{u8't'});
    assert(Convert::to<std::u16string>(u8't') == std::u16string{u't'});
    assert(Convert::to<std::u32string>(u8't') == std::u32string{U't'});
#endif

    std::cout << "Began Char16 Tests" << std::endl;

    assert(Convert::to<bool>(u't') == true);
    assert(Convert::to<char>(u't') == 't');
    assert(Convert::to<unsigned char>(u't') == 't');
    assert(Convert::to<signed char>(u't') == 't');
    assert(Convert::to<wchar_t>(u't') == L't');
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(u't') == u8't');
#endif
    assert(Convert::to<char32>(u't') == U't');
    assert(Convert::to<unsigned short>(u't') == 't');
    assert(Convert::to<short>(u't') == 't');
    assert(Convert::to<unsigned int>(u't') == 't');
    assert(Convert::to<int>(u't') == 't');
    assert(Convert::to<unsigned long>(u't') == 't');
    assert(Convert::to<long>(u't') == 't');
    assert(Convert::to<unsigned long long>(u't') == 't');
    assert(Convert::to<long long>(u't') == 't');
    assert(Convert::to<float>(u't') == 't');
    assert(Convert::to<double>(u't') == 't');
    assert(Convert::to<long double>(u't') == 't');
    assert(Convert::to<std::string>(u't') == std::string{'t'});
    assert(Convert::to<std::wstring>(u't') == std::wstring{L't'});
#ifdef __cpp_char8_t
    assert(Convert::to<std::u8string>(u't') == std::u8string{u8't'});
#endif
    assert(Convert::to<std::u16string>(u't') == std::u16string{u't'});
    assert(Convert::to<std::u32string>(u't') == std::u32string{U't'});

    std::cout << "Began Char32 Tests" << std::endl;

    assert(Convert::to<bool>(U't') == true);
    assert(Convert::to<char>(U't') == 't');
    assert(Convert::to<unsigned char>(U't') == 't');
    assert(Convert::to<signed char>(U't') == 't');
    assert(Convert::to<wchar_t>(U't') == L't');
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(U't') == u8't');
#endif
    assert(Convert::to<char16>(U't') == u't');
    assert(Convert::to<unsigned short>(U't') == 't');
    assert(Convert::to<short>(U't') == 't');
    assert(Convert::to<unsigned int>(U't') == 't');
    assert(Convert::to<int>(U't') == 't');
    assert(Convert::to<unsigned long>(U't') == 't');
    assert(Convert::to<long>(U't') == 't');
    assert(Convert::to<unsigned long long>(U't') == 't');
    assert(Convert::to<long long>(U't') == 't');
    assert(Convert::to<float>(U't') == 't');
    assert(Convert::to<double>(U't') == 't');
    assert(Convert::to<long double>(U't') == 't');
    assert(Convert::to<std::string>(U't') == std::string{'t'});
    assert(Convert::to<std::wstring>(U't') == std::wstring{L't'});
#ifdef __cpp_char8_t
    assert(Convert::to<std::u8string>(U't') == std::u8string{u8't'});
#endif
    assert(Convert::to<std::u16string>(U't') == std::u16string{u't'});
    assert(Convert::to<std::u32string>(U't') == std::u32string{U't'});

    std::cout << "Began UShort Tests" << std::endl;

    assert((Convert::to<bool, unsigned short>(5) == true));
    assert((Convert::to<char, unsigned short>(5) == 5));
    assert((Convert::to<unsigned char, unsigned short>(5) == 5u));
    assert((Convert::to<signed char, unsigned short>(5) == 5));
    assert((Convert::to<wchar_t, unsigned short>(5) == 5));
#ifdef __cpp_char8_t
    assert((Convert::to<char8, unsigned short>(5) == 5));
#endif
    assert((Convert::to<char16, unsigned short>(5) == 5));
    assert((Convert::to<char32, unsigned short>(5) == 5));
    assert((Convert::to<short, unsigned short>(5) == 5));
    assert((Convert::to<unsigned int, unsigned short>(5) == 5u));
    assert((Convert::to<int, unsigned short>(5) == 5));
    assert((Convert::to<unsigned long, unsigned short>(5) == 5ul));
    assert((Convert::to<long, unsigned short>(5) == 5l));
    assert((Convert::to<unsigned long long, unsigned short>(5) == 5ull));
    assert((Convert::to<long long, unsigned short>(5) == 5ll));
    assert((Convert::to<float, unsigned short>(5) == 5.f));
    assert((Convert::to<double, unsigned short>(5) == 5.0));
    assert((Convert::to<long double, unsigned short>(5) == 5.0l));

    std::cout << "Began Short Tests" << std::endl;

    assert((Convert::to<bool, short>(5) == true));
    assert((Convert::to<char, short>(5) == 5));
    assert((Convert::to<unsigned char, short>(5) == 5u));
    assert((Convert::to<signed char, short>(5) == 5));
    assert((Convert::to<wchar_t, short>(5) == 5));
#ifdef __cpp_char8_t
    assert((Convert::to<char8, short>(5) == 5));
#endif
    assert((Convert::to<char16, short>(5) == 5));
    assert((Convert::to<char32, short>(5) == 5));
    assert((Convert::to<unsigned short, short>(5) == 5));
    assert((Convert::to<unsigned int, short>(5) == 5u));
    assert((Convert::to<int, short>(5) == 5));
    assert((Convert::to<unsigned long, short>(5) == 5ul));
    assert((Convert::to<long, short>(5) == 5l));
    assert((Convert::to<unsigned long long, short>(5) == 5ull));
    assert((Convert::to<long long, short>(5) == 5ll));
    assert((Convert::to<float, short>(5) == 5.f));
    assert((Convert::to<double, short>(5) == 5.0));
    assert((Convert::to<long double, short>(5) == 5.0l));

    std::cout << "Began UInt Tests" << std::endl;

    assert(Convert::to<bool>(5u) == true);
    assert(Convert::to<char>(5u) == 5);
    assert(Convert::to<unsigned char>(5u) == 5u);
    assert(Convert::to<signed char>(5u) == 5);
    assert(Convert::to<wchar_t>(5u) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5u) == 5u);
#endif
    assert(Convert::to<char16>(5u) == 5u);
    assert(Convert::to<char32>(5u) == 5u);
    assert(Convert::to<unsigned short>(5u) == 5u);
    assert(Convert::to<short>(5u) == 5);
    assert(Convert::to<int>(5u) == 5);
    assert(Convert::to<unsigned long>(5u) == 5ul);
    assert(Convert::to<long>(5u) == 5l);
    assert(Convert::to<unsigned long long>(5u) == 5ull);
    assert(Convert::to<long long>(5u) == 5ll);
    assert(Convert::to<float>(5u) == 5.f);
    assert(Convert::to<double>(5u) == 5.0);
    assert(Convert::to<long double>(5u) == 5.0l);

    std::cout << "Began Int Tests" << std::endl;

    assert(Convert::to<bool>(5) == true);
    assert(Convert::to<char>(5) == 5);
    assert(Convert::to<unsigned char>(5) == 5u);
    assert(Convert::to<signed char>(5) == 5);
    assert(Convert::to<wchar_t>(5) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5) == 5);
#endif
    assert(Convert::to<char16>(5) == 5);
    assert(Convert::to<char32>(5) == 5);
    assert(Convert::to<unsigned short>(5) == 5u);
    assert(Convert::to<short>(5) == 5);
    assert(Convert::to<unsigned int>(5) == 5u);
    assert(Convert::to<unsigned long>(5) == 5ul);
    assert(Convert::to<long>(5) == 5l);
    assert(Convert::to<unsigned long long>(5) == 5ull);
    assert(Convert::to<long long>(5) == 5ll);
    assert(Convert::to<float>(5) == 5.f);
    assert(Convert::to<double>(5) == 5.0);
    assert(Convert::to<long double>(5) == 5.0l);

    std::cout << "Began ULong Tests" << std::endl;

    assert(Convert::to<bool>(5ul) == true);
    assert(Convert::to<char>(5ul) == 5);
    assert(Convert::to<unsigned char>(5ul) == 5u);
    assert(Convert::to<signed char>(5ul) == 5);
    assert(Convert::to<wchar_t>(5ul) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5ul) == 5);
#endif
    assert(Convert::to<char16>(5ul) == 5);
    assert(Convert::to<char32>(5ul) == 5);
    assert(Convert::to<unsigned short>(5ul) == 5u);
    assert(Convert::to<short>(5ul) == 5);
    assert(Convert::to<unsigned int>(5ul) == 5u);
    assert(Convert::to<int>(5ul) == 5);
    assert(Convert::to<long>(5ul) == 5l);
    assert(Convert::to<unsigned long long>(5ul) == 5ull);
    assert(Convert::to<long long>(5ul) == 5ll);
    assert(Convert::to<float>(5ul) == 5.f);
    assert(Convert::to<double>(5ul) == 5.0);
    assert(Convert::to<long double>(5ul) == 5.0l);

    std::cout << "Began Long Tests" << std::endl;

    assert(Convert::to<bool>(5l) == true);
    assert(Convert::to<char>(5l) == 5);
    assert(Convert::to<unsigned char>(5l) == 5u);
    assert(Convert::to<signed char>(5l) == 5);
    assert(Convert::to<wchar_t>(5l) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5l) == 5);
#endif
    assert(Convert::to<char16>(5l) == 5);
    assert(Convert::to<char32>(5l) == 5);
    assert(Convert::to<unsigned short>(5l) == 5u);
    assert(Convert::to<short>(5l) == 5);
    assert(Convert::to<unsigned int>(5l) == 5u);
    assert(Convert::to<int>(5l) == 5);
    assert(Convert::to<unsigned long>(5l) == 5ul);
    assert(Convert::to<unsigned long long>(5l) == 5ull);
    assert(Convert::to<long long>(5l) == 5ll);
    assert(Convert::to<float>(5l) == 5.f);
    assert(Convert::to<double>(5l) == 5.0);
    assert(Convert::to<long double>(5l) == 5.0l);

    std::cout << "Began ULongLong Tests" << std::endl;

    assert(Convert::to<bool>(5ull) == true);
    assert(Convert::to<char>(5ull) == 5);
    assert(Convert::to<unsigned char>(5ull) == 5u);
    assert(Convert::to<signed char>(5ull) == 5);
    assert(Convert::to<wchar_t>(5ull) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5ull) == 5);
#endif
    assert(Convert::to<char16>(5ull) == 5);
    assert(Convert::to<char32>(5ull) == 5);
    assert(Convert::to<unsigned short>(5ull) == 5u);
    assert(Convert::to<short>(5ull) == 5);
    assert(Convert::to<unsigned int>(5ull) == 5u);
    assert(Convert::to<int>(5ull) == 5);
    assert(Convert::to<unsigned long>(5ull) == 5ul);
    assert(Convert::to<long>(5ull) == 5l);
    assert(Convert::to<long long>(5ull) == 5ll);
    assert(Convert::to<float>(5ull) == 5.f);
    assert(Convert::to<double>(5ull) == 5.0);
    assert(Convert::to<long double>(5ull) == 5.0l);

    std::cout << "Began LongLong Tests" << std::endl;

    assert(Convert::to<bool>(5ll) == true);
    assert(Convert::to<char>(5ll) == 5);
    assert(Convert::to<unsigned char>(5ll) == 5u);
    assert(Convert::to<signed char>(5ll) == 5);
    assert(Convert::to<wchar_t>(5ll) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5ll) == 5);
#endif
    assert(Convert::to<char16>(5ll) == 5);
    assert(Convert::to<char32>(5ll) == 5);
    assert(Convert::to<unsigned short>(5ll) == 5u);
    assert(Convert::to<short>(5ll) == 5);
    assert(Convert::to<unsigned int>(5ll) == 5u);
    assert(Convert::to<int>(5ll) == 5);
    assert(Convert::to<unsigned long>(5ll) == 5ul);
    assert(Convert::to<long>(5ll) == 5l);
    assert(Convert::to<unsigned long long>(5ll) == 5ull);
    assert(Convert::to<float>(5ll) == 5.f);
    assert(Convert::to<double>(5ll) == 5.0);
    assert(Convert::to<long double>(5ll) == 5.0l);

    std::cout << "Began Float Tests" << std::endl;

    assert(Convert::to<bool>(5.f) == true);
    assert(Convert::to<char>(5.f) == 5);
    assert(Convert::to<unsigned char>(5.f) == 5u);
    assert(Convert::to<signed char>(5.f) == 5);
    assert(Convert::to<wchar_t>(5.f) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5.f) == 5);
#endif
    assert(Convert::to<char16>(5.f) == 5);
    assert(Convert::to<char32>(5.f) == 5);
    assert(Convert::to<unsigned short>(5.f) == 5u);
    assert(Convert::to<short>(5.f) == 5);
    assert(Convert::to<unsigned int>(5.f) == 5u);
    assert(Convert::to<int>(5.f) == 5);
    assert(Convert::to<unsigned long>(5.f) == 5ul);
    assert(Convert::to<long>(5.f) == 5l);
    assert(Convert::to<unsigned long long>(5.f) == 5ull);
    assert(Convert::to<long long>(5.f) == 5ll);
    assert(Convert::to<double>(5.f) == 5.0);
    assert(Convert::to<long double>(5.f) == 5.0l);

    std::cout << "Began Double Tests" << std::endl;

    assert(Convert::to<bool>(5.0) == true);
    assert(Convert::to<char>(5.0) == 5);
    assert(Convert::to<unsigned char>(5.0) == 5u);
    assert(Convert::to<signed char>(5.0) == 5);
    assert(Convert::to<wchar_t>(5.0) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5.0) == 5);
#endif
    assert(Convert::to<char16>(5.0) == 5);
    assert(Convert::to<char32>(5.0) == 5);
    assert(Convert::to<unsigned short>(5.0) == 5u);
    assert(Convert::to<short>(5.0) == 5);
    assert(Convert::to<unsigned int>(5.0) == 5u);
    assert(Convert::to<int>(5.0) == 5);
    assert(Convert::to<unsigned long>(5.0) == 5ul);
    assert(Convert::to<long>(5.0) == 5l);
    assert(Convert::to<unsigned long long>(5.0) == 5ull);
    assert(Convert::to<long long>(5.0) == 5ll);
    assert(Convert::to<float>(5.0) == 5.f);
    assert(Convert::to<long double>(5.0) == 5.0l);

    std::cout << "Began LongDouble Tests" << std::endl;

    assert(Convert::to<bool>(5.0l) == true);
    assert(Convert::to<char>(5.0l) == 5);
    assert(Convert::to<unsigned char>(5.0l) == 5u);
    assert(Convert::to<signed char>(5.0l) == 5);
    assert(Convert::to<wchar_t>(5.0l) == 5);
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(5.0l) == 5);
#endif
    assert(Convert::to<char16>(5.0l) == 5);
    assert(Convert::to<char32>(5.0l) == 5);
    assert(Convert::to<unsigned short>(5.0l) == 5u);
    assert(Convert::to<short>(5.0l) == 5);
    assert(Convert::to<unsigned int>(5.0l) == 5u);
    assert(Convert::to<int>(5.0l) == 5);
    assert(Convert::to<unsigned long>(5.0l) == 5ul);
    assert(Convert::to<long>(5.0l) == 5l);
    assert(Convert::to<unsigned long long>(5.0l) == 5ull);
    assert(Convert::to<long long>(5.0l) == 5ll);
    assert(Convert::to<float>(5.0l) == 5.f);
    assert(Convert::to<double>(5.0l) == 5.0);

    return 0;
}
