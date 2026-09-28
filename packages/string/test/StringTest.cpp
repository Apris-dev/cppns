#include <iostream>
#include <string>

#include "cppns/string/String.h"

cppns_main() {

    std::string str;

    TString s{'c'};

    s.push('h');

    s.append("ello");

    assert(s.contains('c'));
    assert(s.contains('h'));
    assert(s.contains('e'));
    assert(s.find('h') == 1);
    assert(s.contains("hello"));

    for (const auto& c : s)
        std::cout << c;
    std::cout << std::endl;

    return 0;
}
