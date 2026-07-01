# CMake Parent File Requirements
cmake_minimum_required(VERSION 3.28...4.2)

project(
        SimpleCPP
        DESCRIPTION "A set of libraries designed to make usage of C++ as simple as possible"
        VERSION 1.0.0
        LANGUAGES CXX C
)

# Enable testing if a top level project, otherwise it can be enabled optionally
option(SIMPLECPP_TEST "Enable testing SimpleCPP features" ${PROJECT_IS_TOP_LEVEL})

if(SIMPLECPP_TEST)
    message(STATUS "SimpleCPP: Enabled Testing")
    enable_testing()

    # Set default CXX_STANDARD if testing
    set(CMAKE_CXX_STANDARD 23)
endif()

# If CXX Standard is not defined, fallback to c++17
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    message(WARNING "SimpleCPP: Could not find parent CXX standard; Using default.")
endif()

message(STATUS "SimpleCPP: Using CXX version ${CMAKE_CXX_STANDARD}")

include(scripts/packages.cmake)

# Project Definition
define_project(
        SimpleCPP # Name
        ${CMAKE_CXX_STANDARD} # CXX Version
)

add_subdirectory(packages)

add_library(SimpleCPP INTERFACE)

get_target_property(SIMPLECPP_PACKAGES SimpleCPP-Settings PACKAGES)

foreach (PACKAGE IN LISTS SIMPLECPP_PACKAGES)
    target_link_libraries(SimpleCPP INTERFACE ${PACKAGE})
endforeach ()

# Enable testing for each package if requested
# We do this after creating the packages so the tests dont need to be ordered
if (SIMPLECPP_TEST)
    foreach (PACKAGE IN LISTS SIMPLECPP_PACKAGES)
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/packages/${PACKAGE}/test")
            # Ensure scope is set for tests
            set(CURRENT_SCOPE_PACKAGE ${PACKAGE})
            add_subdirectory(packages/${PACKAGE}/test)
            unset(CURRENT_SCOPE_PACKAGE)
        endif()
    endforeach()
endif()