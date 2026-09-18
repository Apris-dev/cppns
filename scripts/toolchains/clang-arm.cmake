set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# No extra packages should need to be installed
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_COMPILER_TARGET arm-linux-gnueabihf)
set(CMAKE_CXX_COMPILER_TARGET arm-linux-gnueabihf)

set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN /usr)
set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN /usr)

# Requires qemu-user-static on debian
set(CMAKE_CROSSCOMPILING_EMULATOR
        "qemu-arm-static;-L;/usr/arm-linux-gnu"
        CACHE STRING "ARM QEMU emulator"
)
