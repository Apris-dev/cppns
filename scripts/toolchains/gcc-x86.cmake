set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

# Requires both gcc-i686-linux-gnu and g++-i686-linux-gnu on debian
set(CMAKE_C_COMPILER i686-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER i686-linux-gnu-g++)

set(QEMU_LD_PREFIX "/usr/i386-linux-gnu/lib")

# Requires qemu-user-static on debian
set(CMAKE_CROSSCOMPILING_EMULATOR
        "qemu-i386-static;-L;/usr/i386-linux-gnu"
        CACHE STRING "x86 32-bit QEMU emulator"
)
