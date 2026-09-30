#include <iostream>

#include "cppns/string/String.h"

constexpr static bool isValidUTF8(const CString& s) noexcept {
    const size_t n = s.getSize();

    size_t i = 0;
    while (i < n) {
        const auto b0 = static_cast<unsigned char>(s[i]);

        // Is ASCII
        if (b0 < 0x80) { ++i; continue; }

        size_t len;
        unsigned char lo = 0x80, hi = 0xBF;

        // Check for codepoint's spanning multiple bytes
        // Necessary because non-ASCII codepoints can appear invalid
        if (b0 >= 0xC2 && b0 <= 0xDF) {
            len = 2;
        }else if (b0 == 0xE0) {
            len = 3; lo = 0xA0;
        } else if (b0 == 0xED) {
            len = 3; hi = 0x9F;
        } else if (b0 >= 0xE1 && b0 <= 0xEF) {
            len = 3;
        } else if (b0 == 0xF0) {
            len = 4; lo = 0x90;
        } else if (b0 == 0xF4) {
            len = 4; hi = 0x8F;
        } else if (b0 >= 0xF1 && b0 <= 0xF3) {
            len = 4;
        } else {
            return false;
        }

        // Goes beyond size of s
        if (n - i < len)
            return false;

        if (const auto b1 = static_cast<unsigned char>(s[i + 1]); b1 < lo || b1 > hi)
            return false;

        for (std::size_t k = 2; k < len; ++k)
            if ((static_cast<unsigned char>(s[i + k]) & 0xC0) != 0x80)
                return false;

        i += len;
    }
    return true;
}

cppns_main() {

    {
        CString str = "hello";
        assert(!str.isEmpty());

        assert(str == "hello");
        assert(str.contains("hello"));

        str.append(" world");
        str += "!";

        assert(str.getSize() == 12);
        assert(str.contains("world"));
        assert(str.containsAll("hello", "world"));
        assert(str.containsOne("world", "hey"));
        assert(!str.containsAll("world", "hey"));

        assert(str.find("world") == 6);
        assert(str.find('w') == 6);

        assert(str.find([](const char& c) { return c == 'w'; }) == 6);

        assert(str.findFirst('o') == 4);
        assert(str.findFirstNot('h') == 1);
        assert(str.findLast('o') == 7);
        assert(str.findLastNot('!') == 10);

        assert(str[3] == 'l');
    }

    {
        CString str;
        assert(str.isEmpty());
        str.resize(3);
        assert(!str.isEmpty());
        assert(str.getSize() == 3);
        str.push();
        assert(str.getSize() == 4);
        str.push('o');
        assert(str.getSize() == 5);
        assert(str.bottom() == 'o');
        str.push(0, 'f');
        assert(str.getSize() == 6);
        assert(str.top() == 'f');
        str.replace(0, 'm');
        assert(str.getSize() == 6);
        assert(str.top() == 'm');
        str.pop();
        assert(str.getSize() == 5);
        assert(str.bottom() != 'o');
        str.popAt(0);
        assert(str.getSize() == 4);
        assert(str.top() != 'm');
        str.clear();
        assert(str.isEmpty());
    }

    {
        CString str = "cba";

        assert(str == "cba");
        str.sort();
        assert(str == "abc");

        CString otr;

        str.transfer(otr, 0);
        assert(str == "bc");
        assert(otr == "a");

        str.erase("bc");
        assert(str.isEmpty());
    }

    {
        const CString str = "héllo 😀";

        assert(!str.isEmpty());
        assert(str == "héllo 😀");

        const String::UTF8 utf8 = str.toUTF8();
        const String::UTF16 utf16 = str.toUTF16();
        const String::UTF32 utf32 = str.toUTF32();

        assert(utf8 == u8"héllo 😀");
        assert(utf16 == u"héllo 😀");
        assert(utf32 == U"héllo 😀");

        // é is split between two bytes on UTF8
        {
            constexpr auto p = u8"é";
            constexpr auto first = p[0];
            constexpr auto second = p[1];
            assert(utf8[1] == first);
            assert(utf8[2] == second);
        }

        // 😀 is split between four bytes on UTF8
        {
            constexpr auto p = u8"😀";
            constexpr auto first = p[0];
            constexpr auto second = p[1];
            constexpr auto third = p[2];
            constexpr auto fourth = p[3];
            assert(utf8[7] == first);
            assert(utf8[8] == second);
            assert(utf8[9] == third);
            assert(utf8[10] == fourth);
        }

        // 😀 is split between two bytes on UTF16
        {
            constexpr auto p = u"😀";
            constexpr auto first = p[0];
            constexpr auto second = p[1];
            assert(utf16[6] == first);
            assert(utf16[7] == second);
        }

        // é is a single codepoint on utf16
        assert(utf16[1] == u'é');

        // é is a single codepoint on utf32
        assert(utf32[1] == U'é');

        // 😀 is a single codepoint on utf32
        assert(utf32[6] == U'😀');
    }

    // These are all invalid UTF8, they should not be converted properly
    {
        const CString validStr = "héllo 😀";
        CString invalidStr0 = "caf\xE9";
        CString invalidStr1 = "\xC0\x80";
        CString invalidStr2 = "\xED\xA0\x80";
        CString invalidStr3 = "\xF4\x90\x80\x80";
        assert(validStr == "héllo 😀");
        assert(isValidUTF8(validStr));
        assert(invalidStr0 == "caf");
        assert(isValidUTF8(invalidStr0));
        assert(invalidStr1.isEmpty());
        assert(isValidUTF8(invalidStr1));
        assert(invalidStr2.isEmpty());
        assert(isValidUTF8(invalidStr2));
        assert(invalidStr3.isEmpty());
        assert(isValidUTF8(invalidStr3));

        // Weak guarantee
        // Direct data access cannot be prevented
        CString invalidStr4;
        invalidStr4.resize(1);
        invalidStr4[0] = '\xE9';
        assert(!isValidUTF8(invalidStr4));
    }

    try {
        CString str;
        str.push('\xE9');
        // Code path should not run, exception should be thrown and caught
        assert(false);
    } catch (Error::String::NonUTF8Character& e) {}

    return 0;
}
