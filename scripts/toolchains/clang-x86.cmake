set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

# No extra packages should need to be installed
set(CMAKE_C_COMPILER /bin/clang)
set(CMAKE_CXX_COMPILER /bin/clang++)

set(CMAKE_C_COMPILER_TARGET i386-linux-gnu)
set(CMAKE_CXX_COMPILER_TARGET i386-linux-gnu)

set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN /usr)
set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN /usr)

# Requires qemu-user-static on debian
set(CMAKE_CROSSCOMPILING_EMULATOR
        "/usr/bin/qemu-i386-static;-L;/usr/i386-linux-gnu"
        CACHE STRING "x86 32-bit QEMU emulator"
)
