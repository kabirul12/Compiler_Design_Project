# MyOwnCompiler — Starter Milestone 1

A C++17 educational interpreter for the custom language described in the project proposal.

## Requirements
- Windows 10/11
- VS Code
- MinGW-w64 with `g++` supporting C++17

## Folder structure
```text
MyOwnCompiler_Starter/
├── src/
│   ├── Token.h
│   ├── Lexer.h
│   ├── Lexer.cpp
│   ├── Value.h
│   ├── SymbolTable.h
│   ├── SymbolTable.cpp
│   ├── Parser.h
│   ├── Parser.cpp
│   └── main.cpp
├── examples/
│   ├── hello.moc
│   └── arithmetic.moc
├── tests/
│   └── invalid_undeclared.moc
└── README.md
```

## Build (run from project root in VS Code terminal)
```powershell
g++ -std=c++17 -Wall -Wextra src/main.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp -o myowncompiler.exe
```

## Run
```powershell
.\myowncompiler.exe examples/hello.moc
.\myowncompiler.exe examples/arithmetic.moc
```

## Show tokens
```powershell
.\myowncompiler.exe examples/hello.moc --tokens
```

## Test an error
```powershell
.\myowncompiler.exe tests/invalid_undeclared.moc
```
Expected: an error mentioning the undeclared variable `missing`.

## Currently supported
- `purno`, `doshomik`, `lekha` declarations
- Numeric and string literals
- Arithmetic expressions and precedence
- Variable references and reassignment
- `Dekhao(...)`
- `Nao()` numeric input
- Arithmetic built-ins: `Jog`, `Biyog`, `Gun`, `Vag`, `Vagsesh`, `Ghat`, `Borgomul`, `Porom`, `Boro`, `Choto`, `Gunfol`, `Gor`
- Comparison built-ins: `BoroJodi`, `ChotoJodi`, `Shoman`, `Oshoman`
- Basic lexical, syntax, and semantic/runtime errors

## Not yet implemented
`jodi/nahole` execution, loops, user-defined functions, block scope, a complete symbol-table report, TAC generation, and native machine-code generation are future milestones. This starter evaluates supported code directly; it is not yet a native-code compiler.
