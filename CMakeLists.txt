# CMake Parent File Requirements
cmake_minimum_required(VERSION 3.29...4.2)

project(
        cppns
        DESCRIPTION "A set of libraries designed to make usage of C++ as simple as possible"
        VERSION 1.0.0
        LANGUAGES CXX C
)

if (${PROJECT_IS_TOP_LEVEL})
    # Set default CXX_STANDARD if top level project
    if (NOT CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 23)
    endif ()

    if (NOT CMAKE_CROSSCOMPILING OR CMAKE_CROSSCOMPILING_EMULATOR)
        option(CPPNS_TEST "Enable testing cppns features" On)
    endif ()

endif ()

# Enable testing if a top level project, otherwise it can be enabled optionally
if (NOT DEFINED CPPNS_TARGET)
    option(CPPNS_TEST "Enable testing cppns features" Off)
endif ()

if(CPPNS_TEST)
    message(STATUS "cppns: Enabled Testing")
    enable_testing()
endif()

# If CXX Standard is not defined, fallback to c++20
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    message(WARNING "cppns: Could not find parent CXX standard; Using default.")
endif()

message(STATUS "cppns: Using CXX version ${CMAKE_CXX_STANDARD}")

include(scripts/packages.cmake)

# Project Definition
define_project(
        cppns # Name
        ${CMAKE_CXX_STANDARD} # CXX Version
)

add_subdirectory(packages)

add_library(cppns INTERFACE)

get_target_property(CPPNS_PACKAGES cppns-settings PACKAGES)

foreach (PACKAGE IN LISTS CPPNS_PACKAGES)
    target_link_libraries(cppns INTERFACE ${PACKAGE})
endforeach ()