set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Requires both gcc-arm-linux-gnueabihf and g++-arm-linux-gnueabihf on debian
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(QEMU_LD_PREFIX "/usr/arm-linux-gnu/lib")

# Requires qemu-user-static on debian
set(CMAKE_CROSSCOMPILING_EMULATOR
        "qemu-arm-static;-L;/usr/arm-linux-gnu"
        CACHE STRING "ARM QEMU emulator"
)
