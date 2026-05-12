# CMake Parent File Requirements
cmake_minimum_required(VERSION 3.28...4.2)

# Steps:
# 1.  Make the launcher link with the target program as a static library. 
# 2. Rename main/WinMain and call it from a launcher after calling LoadLibrary for DLLs. 

# Project Definition
project(
        SimpleCPP_BuildScript
        DESCRIPTION "A script in C designed to handle runtime dependencies and static registration"
        VERSION 1.0.0
        LANGUAGES C CXX
)

add_executable(sbs
        json.h
        main.cpp
)

target_compile_features(sbs PUBLIC "cxx_std_17")
