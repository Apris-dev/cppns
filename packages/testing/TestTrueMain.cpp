#include "TestTrueMain.h"
#include <iostream>

std::vector<std::string> gLoadedModules;

struct SPrinter {
    SPrinter() {
        std::cout << "TestTrueMain Library Loaded!" << std::endl;
        loaded = true;
    }
    bool loaded = false;
};
SPrinter gPrinter;

std::vector<std::string> getModules() {
    return gLoadedModules;
}

void addModule(const std::string& str) {
    gLoadedModules.emplace_back(str);
}