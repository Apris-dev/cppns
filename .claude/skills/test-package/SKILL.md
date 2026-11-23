---
name: test-package
description: Build and run the tests for one cppns package (or all of them) through WSL using the CMake presets. Use after changing a package to verify it compiles and its tests pass.
argument-hint: <package-name|all> [debug|release] [compiler-arch, default gcc-x64]
---

Build and test: $ARGUMENTS

Defaults: package `all`, mode `debug`, compiler-arch `gcc-x64`. `release` matches what CI runs. `clang-x64` is the
other confirmed-working option. MSVC is out of scope here — the user tests that themselves on native Windows; don't
attempt the `msvc-*` presets.

Everything below runs inside WSL via `wsl.exe`, against the same files at `/mnt/c/Repos/cppns` (no separate clone,
no syncing needed). Every command is prefixed with `cd /mnt/c/Repos/cppns &&` for this reason.

1. Check the toolchain:
   `wsl.exe -- bash -lc "cmake --version && ninja --version"`
   and, matching whichever compiler-arch you're targeting:
   `wsl.exe -- bash -lc "gcc --version && g++ --version"` (for `gcc-*`)
   `wsl.exe -- bash -lc "clang --version && clang++ --version"` (for `clang-*`)
   If anything is missing, stop and tell the user what's missing (e.g. `sudo apt install ninja-build` inside WSL).
   Don't try to install it yourself — `sudo` in this WSL install needs a password you can't supply. Don't fall back
   to a native Windows toolchain or guess at install paths.

2. Configure if `bin/ninja-multi-<compiler-arch>/CMakeCache.txt` doesn't exist:
   `wsl.exe -- bash -lc "cd /mnt/c/Repos/cppns && cmake --preset ninja-multi-<compiler-arch>"`

3. Build. For a single package, build only its test targets. Their names are `cppns-<package>-<TestName>`, with
   `<TestName>` taken from each `add_package_test(...)` in `packages/<package>/test/CMakeLists.txt`:
   `wsl.exe -- bash -lc "cd /mnt/c/Repos/cppns && cmake --build --preset ninja-<mode>-<compiler-arch> --target cppns-<package>-<TestName>"`
   For `all`, omit `--target`.

4. Run:
   `wsl.exe -- bash -lc "cd /mnt/c/Repos/cppns && ctest --preset test-<mode>-<compiler-arch> -R '^cppns-<package>-' --output-on-failure"`
   For `all`, omit `-R`.

5. If the change touches standard-sensitive code (`constexpr_23`, `USING_CXX23`, newer library features), also build
   once on C++20 since CI runs both standards, then reconfigure back to C++23 so the build directory is left as it
   was — same commands, with `-DCMAKE_CXX_STANDARD=20` then `-DCMAKE_CXX_STANDARD=23` appended to the configure step.

6. Other `<compiler-arch>` values (e.g. `gcc-arm64`, `clang-arm`, `mingw-x64`) exist as presets but their cross
   toolchains aren't confirmed installed in this WSL distro (CI installs them per-job via `qemu`/cross-gcc packages).
   If asked to use one, check it the same way as step 1 first and tell the user what to install rather than assuming.

7. Report each test as passed or failed. For failures, quote the first compiler error or the thrown assertion
   message, with file:line, and say whether it looks platform-specific.
