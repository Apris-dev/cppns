#include <iostream>
#include <vector>

#include "TestTrueMain.h"

#ifndef USING_BOOTSTRAPPER
int main() {
    std::cout << "Bootstrapper is not enabled!" << std::endl;

    return 0;
}
#else
EXPORTC int run() {

    std::cout << "TestExec Called" << std::endl;

    std::cout << "Loaded Modules:" << std::endl;

    std::vector<std::string> modules = getModules();

    if (std::find(modules.begin(), modules.end(), "TestLib") != modules.end()) {
        std::cout << "Found TestLib!" << std::endl;
        return 0;
    }

    std::cout << "Could not find TestLib!" << std::endl;
    return 1;
}
#endif