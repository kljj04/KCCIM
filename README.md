# KCCIM (Korean Character Combination Input Method)

KCCIM is a small C library that implements a Korean input-state automaton. A C# WinForms test GUI and unit tests (under tests/) demonstrate how the native library is loaded via P/Invoke to handle Korean composition with finer control than the default IME.

## Key features

- Precise single-character composition highlighting (only the currently changing character is underlined).
- Intercepts Windows input at the message pump level to avoid duplicate events and keep caret stability during composition.
- Toggle between Korean composition and direct English input (Right Alt / Hangul key).
- Core composition engine implemented in portable C and exposed as a dynamic library (`kccim.dll`).

## Requirements

- Windows (tested on Windows 10/11)
- GCC (MinGW/MSYS2) or any C compiler that can produce a Windows DLL
- CMake >= 3.20 (optional)
- .NET SDK 8.0 (for the test projects and GUI)

## Project layout

```
C:.
│  kccim.dll                  # prebuilt binary (convenience)
│  CMakeLists.txt
│  build.bat
│  cpy.bat
│  README.md
│
├─include
│      kccim.h
│      table.h
│
├─internal
│      automata_states.h
│      kccim_internal.h
│
├─src
│      automata.c
│      kccim.c
│      table.c
│
└─tests
       ├─Kccim.Tests          # .NET test runner
       └─Kccim.Gui            # small WinForms GUI for manual testing
```

## Building

1) Quick build with GCC (MinGW / MSYS):

```powershell
# From repository root (Windows)
.
# using provided batch script:
build.bat

# or run manually with gcc:

gcc -Wall -Wextra -O2 -shared -DKCCIM_EXPORTS -Iinclude -Iinternal -o kccim.dll src/automata.c src/table.c src/kccim.c
```

3) Using CMake (recommended for IDEs / CLion / Visual Studio generators):

```powershell
# out-of-source build
cmake -S . -B build
cmake --build build --config Debug
# For MinGW: cmake -G "MinGW Makefiles" -S . -B build && cmake --build build
```

Notes: build.bat (and the CMake target) set -DKCCIM_EXPORTS to export symbols for the DLL.

## Copying the DLL to test output

The repo includes `cpy.bat` to copy the built `kccim.dll` into the GUI test project's output folder:

```powershell
cpy.bat
# or manually:
copy /Y kccim.dll tests\Kccim.Gui\bin\Debug\net8.0-windows\
```

A prebuilt `kccim.dll` is included for convenience; rebuild when making changes to the native code.

## Running tests and GUI

- Run unit/regression tests:

```powershell
dotnet run --project tests/Kccim.Tests/Kccim.Tests.csproj
```

- Run the manual test GUI (WinForms):

```powershell
dotnet run --project tests/Kccim.Gui/Kccim.Gui.csproj
```

## Development notes

- The C headers live in `include/` and internal helpers in `internal/`.
- Follow the build scripts / CMake setup when adding or renaming source files.
- Ensure the DLL is copied into the test GUI's runtime folder before launching the GUI.

## Contributing

Bug reports and PRs welcome. Keep changes focused and add tests when possible.
Please read CONTRIBUTING.md for more information.

