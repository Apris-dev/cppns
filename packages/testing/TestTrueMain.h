#pragma once

#include <vector>
#include <string>

#include "sutil/PlatformDefinition.h"

EXPORT std::vector<std::string> getModules();

EXPORT void addModule(const std::string& str);