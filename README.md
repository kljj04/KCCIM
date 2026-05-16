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

## Build and Run

Run the following script in your PowerShell terminal from the `TestApp` directory to clean, compile, and run the application:

```powershell
Remove-Item kccim.dll -ErrorAction SilentlyContinue
Remove-Item bin\Debug\net10.0-windows\kccim.dll -ErrorAction SilentlyContinue
Remove-Item ..\kccim.dll -ErrorAction SilentlyContinue

# Compiled using only the valid source files listed in the project structure
gcc -shared -o bin\Debug\net10.0-windows\kccim.dll ../src/automata.c ../src/table.c ../src/kccim.c -DKCCIM_EXPORTS

Copy-Item bin\Debug\net10.0-windows\kccim.dll -Destination .
Copy-Item bin\Debug\net10.0-windows\kccim.dll -Destination ..\

dotnet run

```

## Usage and Testing

1. Launch the application and enter any text.
2. Click anywhere inside the text box to reposition the caret.
3. Continue typing in Korean; the caret remains stable at the insertion point, and only the trailing character reflects the composition highlight.
4. Press the Right Alt key to switch to English input mode and verify direct text input.

```
