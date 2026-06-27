#include <iostream>

#include "TestTrueMain.h"

extern "C" EXPORT int run() {
    std::cout << "TestExec Called" << std::endl;

    std::cout << "Loaded Modules:" << std::endl;

    for (auto& mod : getModules()) {
        std::cout << mod << std::endl;
    }

    return 0;
}