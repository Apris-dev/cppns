# cppns — "(Let's make) C++ Not Suck"

A set of mostly header-only C++ libraries ("packages"). Minimum C++20. CI builds and tests every change on
C++20 **and** C++23 with MSVC, GCC, Clang and MinGW across x64, x86, arm64 and arm (Linux arm/x86 run under QEMU).
Code that only works on the compiler in front of you is not done.

## Layout

- `packages/<name>/include/cppns/<name>/*.h`: public headers (exception: `dependencyGraph` uses `cppns/dg/`).
- `packages/<name>/CMakeLists.txt`: `add_interface_package(cppns-<name> <headers...>)` then `link_package(INTERFACE cppns-<dep>)`.
- `packages/<name>/test/`: auto-discovered when `CPPNS_TEST` is on. `add_package_test(<Name>Test <Name>Test.cpp)`
  links the owning package automatically; use `link_package_test(cppns-<dep>)` only for packages it doesn't already pull in.
- `packages/CMakeLists.txt`: the `PACKAGES` list. New packages go here, **after** everything they depend on.
- `packages/cppns/`: precompiled-header package (`Common.h`, `PlatformDefinition.h`, `Types.h`, `Paths.h`) applied to
  every target. Its macros (`USING_*`, `LINE_ENDING`, `cppns_main`, `constexpr_20/23`, `EXPORT`) and types
  (`uint8`…`int64`, `char8/16/32`) are available everywhere; don't include these headers explicitly.
- `scripts/packages.cmake`: all CMake helpers. Use them instead of raw `add_library`/`target_link_libraries`.

## Conventions

- Indentation: 4 spaces. Some older files use tabs. When editing one, stay consistent within that file rather than
  reindenting it. New files always use 4 spaces.
- Type prefixes: `C` classes (`CPrinter`), `T` templates and type aliases (`TVector`, `TType`), `S` plain structs
  (`SLineBuffer`), `E` free-standing enums (`EContainerType`). Enums scoped in a domain namespace drop the prefix
  (`File::OpenType`, `Path::Filter`).
- Members: `mName` for public/protected, `m_Name` for private. Existing code isn't fully consistent. Apply it to
  new and edited code, but don't mass-rename.
- Parameters: `inName` for inputs, `outName` for outputs.
- Functions are camelCase with `get`/`is`/`can` prefixes; mark getters `[[nodiscard]]`.
- Group free functions, enums and errors in domain namespaces: `File::`, `Path::`, `String::`, `Error::<Domain>::`, `Assert::`.
- Headers use `#pragma once`. Globals defined in headers must be `inline` (e.g. `inline class CPrinter {...} print;`).
- Platform branches use the project macros: `#if USING_WINDOWS` / `USING_MSVC` / `USING_MINGW` / `USING_APPLE` /
  `USING_LINUX` / `USING_64_BIT` / `USING_CRLF` / `USING_CXX23`. Never use raw `_WIN32`/`_MSC_VER` outside
  `PlatformDefinition.h`. MinGW is Windows but not MSVC: gate CRT `_s` functions on `USING_MSVC` and Win32 APIs on `USING_WINDOWS`.
- Use `constexpr_20` / `constexpr_23` for things that can only be `constexpr` on newer standards. Guard C++23-only
  library features with `USING_CXX23` so the C++20 CI jobs still build.

## Errors and assertions

- Define errors with `CREATE_ERROR_TYPE(Name, "Display Name", "Description with {} args", Error::Runtime);` inside
  `namespace Error::<Domain>`. Throw by value (`throw Error::File::Open(path)`), never `throw new`.
- Check invariants with `Assert::True(cond, "reason")`, `Assert::False`, `Assert::NoError(errno)` and `Assert::FileOK(file)`.
  They always run (they are not stripped by `NDEBUG`) and throw `Error::Runtime`.
- Don't use, include or redefine the standard `assert`/`<cassert>`. CI tests run in **Release**, where `NDEBUG`
  removes standard asserts entirely. The vendored `util/fast_float.h` is the only exception; leave it as-is.

## Tests

- A test is an executable: `cppns_main() { ...; return 0; }`. A thrown exception (e.g. a failed `Assert::True`) fails it.
- CTest names are `cppns-<package>-<TestName>` (e.g. `cppns-io-IoTest`).
- Print with `print << ... << EOL;` (`cppns-io`). `CPrinter` writes via `fwrite(stdout)`, so tests that check output
  must redirect `stdout` itself. See `captureStdout` in `packages/io/test/IoTest.cpp`.

## Building

- Presets: configure `ninja-multi-<compiler>-<arch>`, build `ninja-<debug|release>-<compiler>-<arch>`, test
  `test-<debug|release>-<compiler>-<arch>`. Output goes to `bin/<configure-preset>/` (gitignored).
- Claude builds and tests through **WSL** (`gcc-x64`/`clang-x64`, both confirmed installed there — `cmake`, `ninja`,
  `gcc`/`g++`, `clang`/`clang++`), via `wsl.exe -- bash -lc "cd /mnt/c/Repos/cppns && <command>"`. This is the same
  repo checkout, mounted at `/mnt/c/Repos/cppns` — no separate clone. The `/test-package` skill wraps this; use it
  instead of reconstructing the commands by hand.
- MSVC is out of scope for Claude here — the toolset needs an interactive Visual Studio developer environment that
  isn't available non-interactively, and the user verifies MSVC themselves natively. Don't attempt `msvc-*` presets.
- Other compiler-arch presets (`gcc-arm64`, `clang-arm`, `mingw-x64`, …) exist but their cross toolchains aren't
  confirmed installed in WSL; check before assuming one works (see `/test-package`).
- `CMakeUserPresets.json` holds personal CLion presets and is not tracked.

## Git

- Work on `feature/<name>` branches and open PRs into `dev`.  Only open a PR into `main` from `dev`.
- Commit messages are plain sentence-style summaries with no conventional-commit prefixes.
- The root `.gitignore` ignores everything not whitelisted by extension (`.h`, `.cpp`, `.md`, `CMakeLists.txt`, …).
  Files with other extensions are silently untracked, so check `git status` after adding files.
- Use the Bash tool for git/cmake/ctest; the read-only and build commands are pre-approved in `.claude/settings.json`.

## Review

Run the `cross-platform-reviewer` agent after non-trivial changes to headers, platform code or CMake, before committing.
