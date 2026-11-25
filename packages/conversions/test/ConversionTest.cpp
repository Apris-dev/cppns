#include <iostream>

#include "cppns/conversions/Conversion.h"
#include "cppns/string/String.h"

template <typename TChar>
requires Convert::is_char_v<TChar>
static void runTest(const char* testName) {
    std::cout << "Began " << testName << " Tests" << std::endl;

    auto value = static_cast<TChar>('t');

    assert((Convert::to<bool, TChar>(value) == true));
    assert((Convert::to<char, TChar>(value) == 't'));
    assert((Convert::to<unsigned char, TChar>(value) == 't'));
    assert((Convert::to<signed char, TChar>(value) == 't'));
    assert((Convert::to<wchar_t, TChar>(value) == L't'));
#ifdef __cpp_char8_t
    assert((Convert::to<char8, TChar>(value) == u8't'));
#endif
    assert((Convert::to<char16, TChar>(value) == u't'));
    assert((Convert::to<unsigned short, TChar>(value) == 't'));
    assert((Convert::to<short, TChar>(value) == 't'));
    assert((Convert::to<unsigned int, TChar>(value) == 't'));
    assert((Convert::to<int, TChar>(value) == 't'));
    assert((Convert::to<unsigned long, TChar>(value) == 't'));
    assert((Convert::to<long, TChar>(value) == 't'));
    assert((Convert::to<unsigned long long, TChar>(value) == 't'));
    assert((Convert::to<long long, TChar>(value) == 't'));
    assert((Convert::to<float, TChar>(value) == 't'));
    assert((Convert::to<double, TChar>(value) == 't'));
    assert((Convert::to<long double, TChar>(value) == 't'));
    assert((Convert::to<std::string, TChar>(value) == std::string{'t'}));
    assert((Convert::to<std::wstring, TChar>(value) == std::wstring{L't'}));
#ifdef __cpp_char8_t
    assert((Convert::to<std::u8string, TChar>(value) == std::u8string{u8't'}));
#endif
    assert((Convert::to<std::u16string, TChar>(value) == std::u16string{u't'}));
    assert((Convert::to<std::u32string, TChar>(value) == std::u32string{U't'}));
}

template <typename TType>
requires Convert::is_integer_v<TType>
static void runTest(const char* testName) {
    std::cout << "Began " << testName << " Tests" << std::endl;

    auto value = static_cast<TType>(5);

    assert((Convert::to<bool, TType>(value) == true));
    assert((Convert::to<char, TType>(value) == 5));
    assert((Convert::to<unsigned char, TType>(value) == 5u));
    assert((Convert::to<signed char, TType>(value) == 5));
    assert((Convert::to<wchar_t, TType>(value) == 5));
#ifdef __cpp_char8_t
    assert((Convert::to<char8, TType>(value) == 5));
#endif
    assert((Convert::to<char16, TType>(value) == 5));
    assert((Convert::to<char32, TType>(value) == 5));
    assert((Convert::to<short, TType>(value) == 5));
    assert((Convert::to<unsigned int, TType>(value) == 5u));
    assert((Convert::to<int, TType>(value) == 5));
    assert((Convert::to<unsigned long, TType>(value) == 5ul));
    assert((Convert::to<long, TType>(value) == 5l));
    assert((Convert::to<unsigned long long, TType>(value) == 5ull));
    assert((Convert::to<long long, TType>(value) == 5ll));
    assert((Convert::to<float, TType>(value) == 5.f));
    assert((Convert::to<double, TType>(value) == 5.0));
    assert((Convert::to<long double, TType>(value) == 5.0l));
    assert((Convert::to<std::string, TType>(value) == std::string{'5'}));
    assert((Convert::to<std::wstring, TType>(value) == std::wstring{L'5'}));
#ifdef __cpp_char8_t
    assert((Convert::to<std::u8string, TType>(value) == std::u8string{u8'5'}));
#endif
    assert((Convert::to<std::u16string, TType>(value) == std::u16string{u'5'}));
    assert((Convert::to<std::u32string, TType>(value) == std::u32string{U'5'}));
}

template <typename TType>
requires std::is_floating_point_v<TType>
static void runTest(const char* testName) {
    std::cout << "Began " << testName << " Tests" << std::endl;

    auto value = static_cast<TType>(5.f);

    assert((Convert::to<bool, TType>(value) == true));
    assert((Convert::to<char, TType>(value) == 5));
    assert((Convert::to<unsigned char, TType>(value) == 5u));
    assert((Convert::to<signed char, TType>(value) == 5));
    assert((Convert::to<wchar_t, TType>(value) == 5));
#ifdef __cpp_char8_t
    assert((Convert::to<char8, TType>(value) == 5));
#endif
    assert((Convert::to<char16, TType>(value) == 5));
    assert((Convert::to<char32, TType>(value) == 5));
    assert((Convert::to<short, TType>(value) == 5));
    assert((Convert::to<unsigned int, TType>(value) == 5u));
    assert((Convert::to<int, TType>(value) == 5));
    assert((Convert::to<unsigned long, TType>(value) == 5ul));
    assert((Convert::to<long, TType>(value) == 5l));
    assert((Convert::to<unsigned long long, TType>(value) == 5ull));
    assert((Convert::to<long long, TType>(value) == 5ll));
    assert((Convert::to<float, TType>(value) == 5.f));
    assert((Convert::to<double, TType>(value) == 5.0));
    assert((Convert::to<long double, TType>(value) == 5.0l));
    assert((Convert::to<std::string, TType>(value) == std::string{"5.000000"}));
    assert((Convert::to<std::wstring, TType>(value) == std::wstring{L"5.000000"}));
#ifdef __cpp_char8_t
    assert((Convert::to<std::u8string, TType>(value) == std::u8string{u8"5.000000"}));
#endif
    assert((Convert::to<std::u16string, TType>(value) == std::u16string{u"5.000000"}));
    assert((Convert::to<std::u32string, TType>(value) == std::u32string{U"5.000000"}));
}

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

    runTest<char>("Char");
    runTest<unsigned char>("UChar");
    runTest<signed char>("SChar");
    runTest<wchar_t>("WChar");
#ifdef __cpp_char8_t
    runTest<char8>("Char8");
#endif
    runTest<char16>("Char16");
    runTest<char32>("Char32");
    runTest<unsigned short>("UShort");
    runTest<short>("Short");
    runTest<unsigned int>("UInt");
    runTest<int>("Int");
    runTest<unsigned long>("ULong");
    runTest<long>("Long");
    runTest<unsigned long long>("ULongLong");
    runTest<long long>("LongLong");
    runTest<float>("Float");
    runTest<double>("Double");
    runTest<long double>("LongDouble");

    return 0;
}
