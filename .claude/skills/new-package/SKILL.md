---
name: new-package
description: Scaffold a new cppns package (public header, CMakeLists, test target, registration in packages/CMakeLists.txt) following the repo's conventions. Use when the user asks to add or create a new package.
argument-hint: <package-name> [dependency packages...]
---

Create a new cppns package from: $ARGUMENTS

The first argument is the package name, in camelCase like the existing ones (`io`, `dependencyGraph`). Any further
arguments are packages it depends on, written without the `cppns-` prefix. If no name was given, or it's unclear what
the primary header should be called, ask before creating anything.

1. Read `packages/CMakeLists.txt` and `packages/io/` (the smallest complete package) as the reference shape.
   Confirm each dependency exists in the `PACKAGES` list.

2. Create `packages/<name>/include/cppns/<name>/<Header>.h`:
   - `#pragma once`, then includes for the dependency headers it actually needs.
   - Don't include `cppns/Common.h`, `PlatformDefinition.h`, `Types.h` or `Paths.h`; the precompiled header provides them.
   - 4-space indentation and the naming conventions in `.claude/CLAUDE.md`.

3. Create `packages/<name>/CMakeLists.txt`:
   ```cmake
   add_interface_package(cppns-<name>
           include/cppns/<name>/<Header>.h
   )

   link_package(INTERFACE cppns-<dep>)
   ```
   Use one `link_package` line per dependency. Only use `add_static_package`/`add_shared_package` if the user asks
   for compiled sources.

4. Create `packages/<name>/test/CMakeLists.txt`:
   ```cmake
   add_package_test(<Name>Test
           <Name>Test.cpp
   )
   ```
   Add `link_package_test(cppns-<pkg>)` only for packages the new package doesn't already link. `cppns-io`, for
   printing, is the usual one.

5. Create `packages/<name>/test/<Name>Test.cpp`:
   ```cpp
   #include "cppns/<name>/<Header>.h"

   cppns_main() {

       {
           Assert::True(/* condition */, "What should have held");
       }

       return 0;
   }
   ```
   Use `Assert::True`/`Assert::False` only, never the standard `assert`.

6. Add `<name>` to the `PACKAGES` list in `packages/CMakeLists.txt`, placed after all of its dependencies. The test
   directory is picked up automatically; don't add `add_subdirectory(test)`.

7. Run `git status` and confirm every new file shows up. The root `.gitignore` is a whitelist, so a file with an
   unusual extension would be silently ignored.

8. Verify with the `/test-package <name>` flow (runs through WSL; see that skill).
