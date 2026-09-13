# M2 — Portable core build

M2 establishes the first native Linux build path for FellowNG without modifying or replacing the existing Visual Studio build.

## Scope

The initial portable target is deliberately small and real: it builds existing Fellow emulator code that is already free of Win32 dependencies.

Current portable slice:

- `WinFellow.Core/CustomChipset/Registers.cpp`
- `WinFellow.Core/CustomChipset/RegisterUtility.cpp`

These sources use standard C++ types and do not depend on `windows.h`, DirectX, WinMM, or MSVC-only APIs.

The CMake target is named:

`FellowNG.Core.Portable`

The produced static library is named `fellowng-core-portable`.

## Build

On Linux:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

CMake 3.20+ and a C++20 compiler are required.

## Test

`tests/portable-core/portable_core_smoke.cpp` links against the portable library and exercises chipset register decoding, including:

- lores/hires selection
- dual playfield
- HAM
- interlace
- bitplane count
- playfield priority
- master/bitplane/disk DMA
- blitter priority

This ensures the Linux CI performs a real compile, link, and execution of Fellow core code rather than only checking repository structure.

## Compatibility policy

The existing `WinFellow.sln` and `.vcxproj` files remain authoritative for the legacy Windows build during the port. M2 does not rewrite or remove those files.

The portable target will be expanded incrementally as dependencies are separated in M3. `Portable.h` is currently Windows-specific and therefore is not treated as the portability layer for FellowNG.

## M2 exit criteria

- CMake configures on Ubuntu x86-64.
- Existing Fellow core sources compile into a static library.
- A linked smoke test executes through CTest.
- GitHub Actions performs configure/build/test on Ubuntu.
- No change is required to the existing Visual Studio project files.
