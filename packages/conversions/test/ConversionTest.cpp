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
    assert((Convert::to<std::string, TChar>(value) == "t"));
    assert((Convert::to<std::wstring, TChar>(value) == L"t"));
#ifdef __cpp_char8_t
    assert((Convert::to<std::u8string, TChar>(value) == u8"t"));
#endif
    assert((Convert::to<std::u16string, TChar>(value) == u"t"));
    assert((Convert::to<std::u32string, TChar>(value) == U"t"));
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

template <typename TChar>
requires Convert::is_char_v<TChar>
static void runStrTest(const char* testName) {
    std::cout << "Began " << testName << " Tests" << std::endl;

    auto numValue = Convert::to<std::basic_string<TChar>>(std::string("5"));
    auto value = Convert::to<std::basic_string<TChar>>(std::string("test"));

    assert(Convert::to<char>(numValue) == '5');
    assert(Convert::to<unsigned char>(numValue) == '5');
    assert(Convert::to<signed char>(numValue) == '5');
    assert(Convert::to<wchar_t>(numValue) == L'5');
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(numValue) == u8'5');
#endif
    assert(Convert::to<char16>(numValue) == u'5');
    assert(Convert::to<char32>(numValue) == U'5');
    assert(Convert::to<unsigned short>(numValue) == 5);
    assert(Convert::to<short>(numValue) == 5);
    assert(Convert::to<unsigned int>(numValue) == 5);
    assert(Convert::to<int>(numValue) == 5);
    assert(Convert::to<unsigned long>(numValue) == 5ul);
    assert(Convert::to<long>(numValue) == 5l);
    assert(Convert::to<unsigned long long>(numValue) == 5ull);
    assert(Convert::to<long long>(numValue) == 5ll);
    assert(Convert::to<float>(numValue) == 5.f);
    assert(Convert::to<double>(numValue) == 5.0);
    assert(Convert::to<long double>(numValue) == 5.0);
    assert(Convert::to<std::string>(value) == "test");
    assert(Convert::to<std::wstring>(value) == L"test");
#ifdef __cpp_char8_t
    assert(Convert::to<std::u8string>(value) == u8"test");
#endif
    assert(Convert::to<std::u16string>(value) == u"test");
    assert(Convert::to<std::u32string>(value) == U"test");
}

template <typename TChar>
requires Convert::is_char_v<TChar>
static void runArrayTest(const char* testName) {
    std::cout << "Began " << testName << " Tests" << std::endl;

    auto numValue = Convert::to<std::basic_string<TChar>>("5");
    auto value = Convert::to<std::basic_string<TChar>>("test");

    assert(Convert::to<char>(numValue.c_str()) == '5');
    assert(Convert::to<unsigned char>(numValue.c_str()) == '5');
    assert(Convert::to<signed char>(numValue.c_str()) == '5');
    assert(Convert::to<wchar_t>(numValue.c_str()) == L'5');
#ifdef __cpp_char8_t
    assert(Convert::to<char8>(numValue.c_str()) == u8'5');
#endif
    assert(Convert::to<char16>(numValue.c_str()) == u'5');
    assert(Convert::to<char32>(numValue.c_str()) == U'5');
    assert(Convert::to<unsigned short>(numValue.c_str()) == 5);
    assert(Convert::to<short>(numValue.c_str()) == 5);
    assert(Convert::to<unsigned int>(numValue.c_str()) == 5);
    assert(Convert::to<int>(numValue.c_str()) == 5);
    assert(Convert::to<unsigned long>(numValue.c_str()) == 5ul);
    assert(Convert::to<long>(numValue.c_str()) == 5l);
    assert(Convert::to<unsigned long long>(numValue.c_str()) == 5ull);
    assert(Convert::to<long long>(numValue.c_str()) == 5ll);
    assert(Convert::to<float>(numValue.c_str()) == 5.f);
    assert(Convert::to<double>(numValue.c_str()) == 5.0);
    assert(Convert::to<long double>(numValue.c_str()) == 5.0);
    assert(Convert::to<std::string>(value.c_str()) == "test");
    assert(Convert::to<std::wstring>(value.c_str()) == L"test");
#ifdef __cpp_char8_t
    assert(Convert::to<std::u8string>(value.c_str()) == u8"test");
#endif
    assert(Convert::to<std::u16string>(value.c_str()) == u"test");
    assert(Convert::to<std::u32string>(value.c_str()) == U"test");
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

    {
        assert(Convert::to<char32>(u'é') == U'é');

        assert(Convert::to<std::string>('h') == "h");
        assert(Convert::to<std::wstring>('h') == L"h");
        assert(Convert::to<std::u8string>('h') == u8"h");
        assert(Convert::to<std::u16string>('h') == u"h");
        assert(Convert::to<std::u32string>('h') == U"h");

        assert(Convert::to<std::string>(L'é') == "é");
        assert(Convert::to<std::wstring>(L'é') == L"é");
        assert(Convert::to<std::u8string>(L'é') == u8"é");
        assert(Convert::to<std::u16string>(L'é') == u"é");
        assert(Convert::to<std::u32string>(L'é') == U"é");

        assert(Convert::to<std::string>(u8'h') == "h");
        assert(Convert::to<std::wstring>(u8'h') == L"h");
        assert(Convert::to<std::u8string>(u8'h') == u8"h");
        assert(Convert::to<std::u16string>(u8'h') == u"h");
        assert(Convert::to<std::u32string>(u8'h') == U"h");

        assert(Convert::to<std::string>(u'é') == "é");
        assert(Convert::to<std::wstring>(u'é') == L"é");
        assert(Convert::to<std::u8string>(u'é') == u8"é");
        assert(Convert::to<std::u16string>(u'é') == u"é");
        assert(Convert::to<std::u32string>(u'é') == U"é");

        assert(Convert::to<std::string>(U'😀') == "😀");
        assert(Convert::to<std::wstring>(U'😀') == L"😀");
        assert(Convert::to<std::u8string>(U'😀') == u8"😀");
        assert(Convert::to<std::u16string>(U'😀') == u"😀");
        assert(Convert::to<std::u32string>(U'😀') == U"😀");
    }

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

    runStrTest<char>("std::string");
    runStrTest<wchar_t>("std::wstring");
#ifdef __cpp_char8_t
    runStrTest<char8>("std::u8string");
#endif
    runStrTest<char16>("std::u16string");
    runStrTest<char32>("std::u32string");

    runArrayTest<char>("char*");
    runArrayTest<wchar_t>("wchar_t*");
#ifdef __cpp_char8_t
    runArrayTest<char8>("char8*");
#endif
    runArrayTest<char16>("char16*");
    runArrayTest<char32>("char32*");

    return 0;
}
