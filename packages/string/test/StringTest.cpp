#include <iostream>

#include "cppns/string/String.h"

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
    }

    return 0;
}
