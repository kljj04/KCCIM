# KCCIM (Korean Character Combination Input Method)

KCCIM is a C-based Korean automata library integrated with a C# WinForms text editor. It provides precise, low-level control over Korean character combination and insertion by intercepting Windows messages via `WndProc`, resolving common issues found in standard IME behaviors during mid-sentence editing.

## Features

* **Single-Character Highlighting**: Unlike standard IMEs that highlight an entire block, this implementation precisely highlights only the final actively changing character with a blue underline.
* **Input Synchronization**: Resolves duplicate key events (e.g., duplicate spacebar inputs) triggered by the OS during Korean text composition by intercepting inputs at the Windows message pump level.
* **Input Mode Toggle**: Supports clean switching between Korean composition and direct English input using the Right Alt or Hangul key.
* **Native C Integration**: The core Korean text composition state machine is implemented in pure C, compiled into a dynamic link library (`kccim.dll`), and invoked seamlessly via P/Invoke in C#.

## Project Structure

```text
C:.
│  kccim.dll
│  Makefile
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
└─src
        automata.c
        kccim.c
        table.c

```

## Build

From this repository root, build the native library with GCC:

```powershell
gcc -Wall -Wextra -O2 -shared -DKCCIM_EXPORTS -Iinclude -Iinternal -o kccim.dll src/automata.c src/table.c src/kccim.c
```

If GNU Make is available:

```powershell
make
```

The generated `kccim.dll` can then be copied next to the C# executable that loads it through P/Invoke.

## Test

The regression tests are written as a small .NET console project. It builds `kccim.dll` into the test output directory before running:

```powershell
dotnet run --project tests/Kccim.Tests/Kccim.Tests.csproj
```

For manual GUI testing:

```powershell
dotnet run --project tests/Kccim.Gui/Kccim.Gui.csproj
```

## Usage and Testing

1. Launch the application and enter any text.
2. Click anywhere inside the text box to reposition the caret.
3. Continue typing in Korean; the caret remains stable at the insertion point, and only the trailing character reflects the composition highlight.
4. Press the Right Alt key to switch to English input mode and verify direct text input.

```
