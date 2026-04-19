# CVM++ — Stack-Based Virtual Machine & Custom Compiler

> A lightweight scripting language built from scratch in C++.  
> Source code → Lexer → Parser → AST → Bytecode Compiler → Stack-based VM

Built as part of the **IIT Guwahati Coding Club Even Semester Projects 2026**.

---

## Table of Contents
- [Overview](#overview)
- [Architecture](#architecture)
- [Language Reference](#language-reference)
- [Building](#building)
- [Running](#running)
- [Debug Mode](#debug-mode)
- [Sample Scripts](#sample-scripts)
- [Instruction Set Architecture (ISA)](#instruction-set-architecture-isa)
- [Project Structure](#project-structure)

---

## Overview

CVM++ is a complete compiler + runtime pipeline for a custom scripting language (`.cvm` files). It demonstrates every stage of language implementation:

```
source.cvm  →  [Lexer]  →  Tokens
            →  [Parser] →  Abstract Syntax Tree (AST)
            →  [Compiler] → Bytecode ([]uint8)
            →  [VM]     →  Execution output
```

---

## Architecture

### Lexer (`lexer.h / lexer.cpp`)
Converts raw source text into a flat list of tokens. Handles keywords, identifiers, integer literals, boolean literals, operators, and single-line comments (`//`).

### Parser (`parser.h / parser.cpp`)
Recursive descent parser. Converts the token stream into an **Abstract Syntax Tree** using precedence climbing:
```
parseExpr → parseComparison → parseAddSub → parseMulDiv → parseUnary → parsePrimary
```

### AST (`ast.h`)
Defines the node types for both expressions and statements using `unique_ptr` ownership:
- **Expressions:** `NumberExpr`, `BoolExpr`, `VarExpr`, `BinaryExpr`, `UnaryExpr`
- **Statements:** `LetStmt`, `AssignStmt`, `PrintStmt`, `InputStmt`, `IfStmt`, `WhileStmt`

### AST Printer (`ast_printer.h / ast_printer.cpp`)
Walks the AST and renders a human-readable tree to stdout (used in `--debug` mode).

### Compiler (`compiler.h / compiler.cpp`)
Walks the AST and emits a flat `vector<uint8_t>` of bytecode instructions. Uses **little-endian 4-byte integers** for operands. Handles:
- Forward jump patching for `if/else`
- Back-jump emission for `while` loops
- Variable slot allocation via a hash map

### VM (`vm.h / vm.cpp`)
A register-free, **stack-based** execution engine. Maintains:
- `stack` — operand stack (`vector<Value>`)
- `vars` — variable slots (`vector<Value>`)
- `ip` — instruction pointer

---

## Language Reference

### Data Types
| Type    | Examples          |
|---------|-------------------|
| Integer | `0`, `42`, `-7`   |
| Boolean | `true`, `false`   |

### Variable Declaration & Assignment
```
let x = 10;
let flag = true;
x = x + 1;          // re-assignment (no 'let')
```

### Operators
| Category    | Operators                      |
|-------------|-------------------------------|
| Arithmetic  | `+`  `-`  `*`  `/`            |
| Comparison  | `==`  `!=`  `<`  `<=`  `>`  `>=` |
| Unary       | `-` (negation)                |

> Note: Comparison operators always return a `bool`. Arithmetic operators require `int` operands.

### Control Flow
```
if (condition) {
    // ...
} else {
    // optional
}

while (condition) {
    // ...
}
```

### I/O
```
print expr;     // prints an integer or boolean, followed by newline
input x;        // reads one integer from stdin into variable x
```

### Comments
```
// This is a single-line comment
```

---

## Building

### Prerequisites
- C++17 compiler (`g++` ≥ 8, or `clang++` ≥ 7)
- CMake ≥ 3.16 (optional — you can also compile directly)

### With CMake (recommended)
```bash
git clone https://github.com/<your-username>/cvm-plus-plus.git
cd cvm-plus-plus
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
# Binary: build/cvm
```

### Direct compile (quick)
```bash
g++ -std=c++17 -O2 -o cvm \
    src/main.cpp src/lexer.cpp src/parser.cpp \
    src/compiler.cpp src/vm.cpp src/ast_printer.cpp
```

---

## Running

### Run a `.cvm` file
```bash
./cvm tests/test_arithmetic.cvm
```

### Interactive REPL
```bash
./cvm
```
```
╔══════════════════════════════════════════════════╗
║          CVM++ Interactive REPL  v1.0           ║
║  Type 'exit' or 'quit' to leave.                ║
║  End multi-line input with a blank line.        ║
╚══════════════════════════════════════════════════╝

cvm>> let x = 5;
cvm>> print x * 2;
10
```

### Pipe input
```bash
echo "7" | ./cvm tests/test_input.cvm
```

---

## Debug Mode

Pass `--debug` (or `-d`) to see all three internal representations:

```bash
./cvm --debug tests/test_if_else.cvm
```

This prints:
1. **Token Stream** — every token with its type and lexeme
2. **Abstract Syntax Tree** — indented tree of AST nodes
3. **Bytecode Disassembly** — each opcode with its operand
4. **Execution output** — the actual program result

Example output:
```
╔══════════════════════════════════════╗
║              TOKEN STREAM            ║
╚══════════════════════════════════════╝
  [LET]  'let'
  [IDENTIFIER]  'x'
  ...

╔══════════════════════════════════════╗
║         ABSTRACT SYNTAX TREE         ║
╚══════════════════════════════════════╝
LetStmt("x")
  └─ init: NumberExpr(42)

╔══════════════════════════════════════╗
║          BYTECODE DISASSEMBLY        ║
╚══════════════════════════════════════╝
  [   0]  OP_PUSH_INT  42
  [   5]  OP_STORE  0
  ...

╔══════════════════════════════════════╗
║              EXECUTION               ║
╚══════════════════════════════════════╝
```

---

## Sample Scripts

All sample scripts are in the `tests/` directory:

| File | What it tests |
|------|--------------|
| `test_arithmetic.cvm` | `let`, `+`, `-`, `*`, `/`, unary `-`, parentheses |
| `test_booleans.cvm` | `true`/`false`, all comparison operators, boolean equality |
| `test_if_else.cvm` | `if`/`else`, nested conditionals, max of two numbers |
| `test_while.cvm` | `while` loop, counter, sum 1–10, factorial 5! |
| `test_fizzbuzz.cvm` | FizzBuzz 1–20 (integer-encoded: 3=Fizz, 5=Buzz, 15=FizzBuzz) |
| `test_input.cvm` | `input` keyword, stdin reading |

---

## Instruction Set Architecture (ISA)

All integers are encoded **little-endian 32-bit** immediately after the opcode byte.

| Opcode | Operand | Description |
|--------|---------|-------------|
| `OP_PUSH_INT` | `int32` | Push integer literal onto stack |
| `OP_PUSH_BOOL` | `uint8 (0/1)` | Push boolean literal onto stack |
| `OP_LOAD` | `int32 slot` | Push value of variable[slot] |
| `OP_STORE` | `int32 slot` | Pop top of stack → variable[slot] |
| `OP_ADD` | — | Pop a, b; push a+b |
| `OP_SUB` | — | Pop a, b; push a-b |
| `OP_MUL` | — | Pop a, b; push a*b |
| `OP_DIV` | — | Pop a, b; push a/b (errors on div-by-zero) |
| `OP_EQ` | — | Pop a, b; push (a==b) as bool |
| `OP_NEQ` | — | Pop a, b; push (a!=b) as bool |
| `OP_LT` | — | Pop a, b; push (a<b) as bool |
| `OP_LTE` | — | Pop a, b; push (a<=b) as bool |
| `OP_GT` | — | Pop a, b; push (a>b) as bool |
| `OP_GTE` | — | Pop a, b; push (a>=b) as bool |
| `OP_NEGATE` | — | Pop a; push -a |
| `OP_JUMP` | `int32 target` | Unconditional jump to absolute address |
| `OP_JUMP_IF_FALSE` | `int32 target` | Pop bool; jump if false |
| `OP_PRINT` | — | Pop and print value to stdout |
| `OP_INPUT` | `int32 slot` | Read int from stdin → variable[slot] |
| `OP_HALT` | — | Stop execution |

---

## Project Structure

```
cvm-plus-plus/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp          # Entry point: REPL, file runner, debug pipeline
│   ├── lexer.h / .cpp    # Tokenizer
│   ├── parser.h / .cpp   # Recursive descent parser → AST
│   ├── ast.h             # AST node definitions
│   ├── ast_printer.h / .cpp  # Debug AST pretty-printer
│   ├── compiler.h / .cpp # AST → Bytecode compiler
│   └── vm.h / .cpp       # Stack-based bytecode VM
└── tests/
    ├── test_arithmetic.cvm
    ├── test_booleans.cvm
    ├── test_if_else.cvm
    ├── test_while.cvm
    ├── test_fizzbuzz.cvm
    └── test_input.cvm
```

---

## References

- [Crafting Interpreters](https://craftinginterpreters.com/) by Robert Nystrom — the definitive guide for this project architecture
- [Writing a Lexer in C++](https://llvm.org/docs/tutorial/)
- [Understanding Stack-Based Virtual Machines](https://en.wikipedia.org/wiki/Stack_machine)

---

