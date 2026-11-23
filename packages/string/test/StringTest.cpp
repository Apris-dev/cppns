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
        Assert::True(!str.isEmpty());

        Assert::True(str == "hello");
        Assert::True(str.contains("hello"));

        str.append(" world");
        str += "!";

        Assert::True(str.getSize() == 12);
        Assert::True(str.contains("world"));
        Assert::True(str.containsAll("hello", "world"));
        Assert::True(str.containsOne("world", "hey"));
        Assert::True(!str.containsAll("world", "hey"));

        Assert::True(str.find("world") == 6);
        Assert::True(str.find('w') == 6);

        Assert::True(str.find([](const char& c) { return c == 'w'; }) == 6);

        Assert::True(str.find('o') == 4);
        Assert::True(str.findLast('o') == 7);

        Assert::True(str[3] == 'l');
    }

    {
        CString str;
        Assert::True(str.isEmpty());
        str.resize(3);
        Assert::True(!str.isEmpty());
        Assert::True(str.getSize() == 3);
        str.push();
        Assert::True(str.getSize() == 4);
        str.push('o');
        Assert::True(str.getSize() == 5);
        Assert::True(str.bottom() == 'o');
        str.push(0, 'f');
        Assert::True(str.getSize() == 6);
        Assert::True(str.top() == 'f');
        str.replace(0, 'm');
        Assert::True(str.getSize() == 6);
        Assert::True(str.top() == 'm');
        str.pop();
        Assert::True(str.getSize() == 5);
        Assert::True(str.bottom() != 'o');
        str.popAt(0);
        Assert::True(str.getSize() == 4);
        Assert::True(str.top() != 'm');
        str.clear();
        Assert::True(str.isEmpty());
    }

    {
        CString str = "cba";

        Assert::True(str == "cba");
        str.sort();
        Assert::True(str == "abc");

        CString otr;

        str.transfer(otr, 0);
        Assert::True(str == "bc");
        Assert::True(otr == "a");

        str.erase("bc");
        Assert::True(str.isEmpty());
    }

    {
        const CString str = "héllo 😀";

        Assert::True(!str.isEmpty());
        Assert::True(str == "héllo 😀");

        const String::UTF8 utf8 = str.toUTF8();
        const String::UTF16 utf16 = str.toUTF16();
        const String::UTF32 utf32 = str.toUTF32();

        Assert::True(utf8 == u8"héllo 😀");
        Assert::True(utf16 == u"héllo 😀");
        Assert::True(utf32 == U"héllo 😀");

        // é is split between two bytes on UTF8
        {
            constexpr auto p = u8"é";
            constexpr auto first = p[0];
            constexpr auto second = p[1];
            Assert::True(utf8[1] == first);
            Assert::True(utf8[2] == second);
        }

        // 😀 is split between four bytes on UTF8
        {
            constexpr auto p = u8"😀";
            constexpr auto first = p[0];
            constexpr auto second = p[1];
            constexpr auto third = p[2];
            constexpr auto fourth = p[3];
            Assert::True(utf8[7] == first);
            Assert::True(utf8[8] == second);
            Assert::True(utf8[9] == third);
            Assert::True(utf8[10] == fourth);
        }

        // 😀 is split between two bytes on UTF16
        {
            constexpr auto p = u"😀";
            constexpr auto first = p[0];
            constexpr auto second = p[1];
            Assert::True(utf16[6] == first);
            Assert::True(utf16[7] == second);
        }

        // é is a single codepoint on utf16
        Assert::True(utf16[1] == u'é');

        // é is a single codepoint on utf32
        Assert::True(utf32[1] == U'é');

        // 😀 is a single codepoint on utf32
        Assert::True(utf32[6] == U'😀');
    }

    // These are all invalid UTF8, they should not be converted properly
    {
        const CString validStr = "héllo 😀";
        CString invalidStr0 = "caf\xE9";
        CString invalidStr1 = "\xC0\x80";
        CString invalidStr2 = "\xED\xA0\x80";
        CString invalidStr3 = "\xF4\x90\x80\x80";
        Assert::True(validStr == "héllo 😀");
        Assert::True(isValidUTF8(validStr));
        Assert::True(invalidStr0 == "caf");
        Assert::True(isValidUTF8(invalidStr0));
        Assert::True(invalidStr1.isEmpty());
        Assert::True(isValidUTF8(invalidStr1));
        Assert::True(invalidStr2.isEmpty());
        Assert::True(isValidUTF8(invalidStr2));
        Assert::True(invalidStr3.isEmpty());
        Assert::True(isValidUTF8(invalidStr3));

        // Weak guarantee
        // Direct data access cannot be prevented
        CString invalidStr4;
        invalidStr4.resize(1);
        invalidStr4[0] = '\xE9';
        Assert::True(!isValidUTF8(invalidStr4));
    }

    try {
        CString str;
        str.push('\xE9');
        // Code path should not run, exception should be thrown and caught
        Assert::True(false);
    } catch (Error::String::NonASCIICharacter& e) {}

    {
        CString str = u8"héllo 😀";
        Assert::True(str == "héllo 😀");

        Assert::True(str.contains("héllo"));
        Assert::True(str.contains("😀"));
        Assert::True(str.contains(u8"héllo"));
        Assert::True(str.contains(u8"😀"));
        Assert::True(str.contains(u"héllo"));
        Assert::True(str.contains(u"😀"));
        Assert::True(str.contains(U"héllo"));
        Assert::True(str.contains(U"😀"));
    }

    {
        CString str = u"héllo 😀";
        Assert::True(str == "héllo 😀");

        Assert::True(str.contains("héllo"));
        Assert::True(str.contains("😀"));
        Assert::True(str.contains('h'));
        Assert::True(str.contains(u8"héllo"));
        Assert::True(str.contains(u8"😀"));
        Assert::True(str.contains(u8'h'));
        Assert::True(str.contains(u"héllo"));
        Assert::True(str.contains(u"😀"));
        Assert::True(str.contains(u'é'));
        Assert::True(str.contains(U"héllo"));
        Assert::True(str.contains(U"😀"));
        Assert::True(str.contains(U'😀'));

        Assert::True(str.containsAll("héllo", "😀"));
        Assert::True(str.containsOne("hello", "😀"));
        Assert::True(str.containsAll(u8"héllo", u"😀"));
        Assert::True(str.containsOne(u8"hello", u"😀"));
        Assert::True(str.containsAll(u"héllo", U"😀"));
        Assert::True(str.containsOne(u"hello", U"😀"));
        Assert::True(str.containsAll("héllo", U"😀"));
        Assert::True(str.containsOne("hello", U"😀"));

        Assert::True(str.find("😀") == 7);
        Assert::True(str.find(u8"😀") == 7);
        Assert::True(str.find(u"😀") == 7);
        Assert::True(str.find(U"😀") == 7);

        Assert::True(str.find(u'é') == 1);
        Assert::True(str.find(U'😀') == 7);
    }

    {
        CString str = U"héllo 😀😀";

        Assert::True(str == "héllo 😀😀");

        Assert::True(str.contains("héllo"));
        Assert::True(str.contains("😀"));
        Assert::True(str.contains('h'));
        Assert::True(str.contains(u8"héllo"));
        Assert::True(str.contains(u8"😀"));
        Assert::True(str.contains(u8'h'));
        Assert::True(str.contains(u"héllo"));
        Assert::True(str.contains(u"😀"));
        Assert::True(str.contains(u'é'));
        Assert::True(str.contains(U"héllo"));
        Assert::True(str.contains(U"😀"));
        Assert::True(str.contains(U'😀'));

        Assert::True(str.containsAll("héllo", "😀"));
        Assert::True(str.containsOne("hello", "😀"));
        Assert::True(str.containsAll(u8"héllo", u"😀"));
        Assert::True(str.containsOne(u8"hello", u"😀"));
        Assert::True(str.containsAll(u"héllo", U"😀"));
        Assert::True(str.containsOne(u"hello", U"😀"));
        Assert::True(str.containsAll("héllo", U"😀"));
        Assert::True(str.containsOne("hello", U"😀"));

        Assert::True(str.find("😀") == 7);
        Assert::True(str.find(u8"😀") == 7);
        Assert::True(str.find(u"😀") == 7);
        Assert::True(str.find(U"😀") == 7);

        Assert::True(str.find(u'é') == 1);
        Assert::True(str.find(u'é') == 1);
        Assert::True(str.find(U'😀') == 7);
        Assert::True(str.find(U'😀') == 7);

        Assert::True(str.findLast("😀") == 11);
        Assert::True(str.findLast(u8"😀") == 11);
        Assert::True(str.findLast(u"😀") == 11);
        Assert::True(str.findLast(U"😀") == 11);

        Assert::True(str.findLast(u'é') == 1);
        Assert::True(str.findLast(U'😀') == 11);

        Assert::True(str.find("é") == str.findLast("é"));
    }

    {
        CString str;
        Assert::True(str.push(u8'h') == 0);
        Assert::True(str == "h");

        str.clear();
        Assert::True(str.push(u'é') == 0);
        Assert::True(str == "é");

        str.clear();
        Assert::True(str.push(U'😀') == 0);
        Assert::True(str == "😀");
    }

    {
        CString str = "é";
        str.push(0, u8'h');
        Assert::True(str == "hé");
    }

    {
        CString str = "é";
        str.push(0, u'é');
        Assert::True(str == "éé");
    }

    {
        CString str = "é";
        str.push(0, U'😀');
        Assert::True(str == "😀é");
    }

    {
        CString str = "héllo😀";
        str.erase(u8"h");
        Assert::True(str == "éllo😀");
        str.erase(u"é");
        Assert::True(str == "llo😀");
        str.erase(U"😀");
        Assert::True(str == "llo");
    }

    {
        CString str = "héllo😀";
        str.erase(u8'h');
        Assert::True(str == "éllo😀");
        str.erase(u'é');
        Assert::True(str == "llo😀");
        str.erase(U'😀');
        Assert::True(str == "llo");
    }

    {
        CString str = "héllo";
        str.append(" 😀");
        Assert::True(str == "héllo 😀");
        Assert::True(str == u8"héllo 😀");
        Assert::True(str == u"héllo 😀");
        Assert::True(str == U"héllo 😀");
    }

    {
        CString str = "héllo";
        str.append(u8" 😀");
        Assert::True(str == "héllo 😀");
        Assert::True(str == u8"héllo 😀");
        Assert::True(str == u"héllo 😀");
        Assert::True(str == U"héllo 😀");
    }

    {
        CString str = "héllo";
        str.append(u" 😀");
        Assert::True(str == "héllo 😀");
        Assert::True(str == u8"héllo 😀");
        Assert::True(str == u"héllo 😀");
        Assert::True(str == U"héllo 😀");
    }

    {
        CString str = "héllo";
        str.append(U" 😀");
        Assert::True(str == "héllo 😀");
        Assert::True(str == u8"héllo 😀");
        Assert::True(str == u"héllo 😀");
        Assert::True(str == U"héllo 😀");
    }

    {
        CString str = "HELLO";
        str.toLower();
        Assert::True(str == "hello");
        str.toCapitalized();
        Assert::True(str == "Hello");
        str.toUpper();
        Assert::True(str == "HELLO");
        str.toCapitalized();
        Assert::True(str == "Hello");
        str.toFlippedCase();
        Assert::True(str == "hELLO");
    }

    return 0;
}
