# MyOwnCompiler Studio (Desktop IDE)

A small Code::Blocks-inspired desktop IDE for writing and running `.moc` programs. It includes a code editor with basic syntax highlighting, file open/save, Build, Build & Run (F5), a console/output panel, and a standard-input field for `Nao()`.

## Requirements (Windows)
1. Python 3 (Tkinter is included in the standard Windows installer; select Tcl/Tk support).
2. MinGW-w64 / MinGW with `g++` supporting C++17.
3. Add the MinGW `bin` folder (the folder containing `g++.exe`) to Windows PATH.

## Start the IDE
Double-click `run_ide.bat`, or open the project folder in VS Code terminal and run:

```powershell
python ide.py
```

## Use
1. Write MyOwnCompiler code in the large editor area.
2. Click **Build & Run** or press **F5**.
3. The IDE compiles the C++ compiler backend automatically and runs the current editor contents.
4. Program output and compiler/runtime errors appear in the bottom console.
5. Use **Open** and **Save** for `.moc` files. `Ctrl+O`, `Ctrl+S`, `Ctrl+N` are supported.
6. If your program uses `Nao()`, enter its input in the `Program input` field before running.

## Example
```cpp
purno a = 10;
purno b = 20;
purno result = Jog(a, b);
Dekhao(result);
```
Expected output:
```text
30
```

## Notes about the compiler language
The IDE is a graphical front end; it does not automatically add language features that the compiler backend has not implemented. The current backend supports the subset documented in `README.md`. In particular, `jodi/nahole`, loops, user-defined functions, and TAC generation are not implemented yet.

## Troubleshooting
- **g++ was not found:** install MinGW-w64 and add its `bin` directory to PATH; restart VS Code/IDE.
- **Python was not found:** install Python 3 and enable the PATH option.
- **Tkinter missing:** rerun the Python installer and add Tcl/Tk support.


**Output not visible?** The fixed layout reserves a dedicated bottom console panel. Click **Build & Run** or press **F5**; program results and errors appear in `OUTPUT / CONSOLE` at the bottom.
