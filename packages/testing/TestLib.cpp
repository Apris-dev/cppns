#include "TestLib.h"

#include <iostream>

#include "TestTrueMain.h"
struct SPrinter {
    SPrinter() {
        static int count = 0;
        ++count;

        std::cout << "TestLIB Library Loaded!" << count << std::endl;
        loaded = true;
        addModule("TestLib");
    }
    bool loaded = false;
};
SPrinter gPrinter;

void printHello() {
    std::cout << "Hello World!" << std::endl;
    std::cout << "loaded = " << (gPrinter.loaded ? "True" : "False") << std::endl;
}
