#include <iostream>

#include "TestTrueMain.h"

#ifdef USING_BOOTSTRAPPER
EXPORTC int run() {
#else
int main() {
#endif

    std::cout << "TestExec Called" << std::endl;

    std::cout << "Loaded Modules:" << std::endl;

    for (auto& mod : getModules()) {
        std::cout << mod << std::endl;
    }

    return 0;
}