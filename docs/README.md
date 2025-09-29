# Fumo Programming Language Documentation

## Table of Contents

### Getting Started
- [Overview](#overview)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Your First Program](#your-first-program)
- [Compilation and Execution](#compilation-and-execution)

### Language Guide
- [Language Tour](#language-tour)
- [Syntax Reference](#syntax-reference)
- [Type System](#type-system)
- [Control Flow](#control-flow)
- [Functions](#functions)
- [Structs and Objects](#structs-and-objects)
- [Namespaces](#namespaces)
- [Memory Management](#memory-management)

### Examples and Tutorials
- [Common Patterns](#common-patterns)
- [Comparisons to C/C++](#comparisons-to-cc)

### Reference
- [Grammar Specification](#grammar-specification)
- [Compiler Options](#compiler-options)
- [Error Messages](#error-messages)
- [Operator Precedence](#operator-precedence)
- [Current Limitations](#current-limitations)

### Development
- [Development Setup](#development-setup)
- [Compiler Architecture](#compiler-architecture)
- [Feedback and Issues](#feedback-and-issues)

---

## Overview

Fumo is a statically-typed systems programming language that compiles to native code via LLVM. 

It implements what i consider to be the "basic essential" features of a systems programming language.

The goal of this compiler is to have a working compiler with all the basics well tested and implemented,
so that i can later use it as a base to explore language design decisions, compiler backend optimizations,
and generally use it to continue learning about compilers.

Fumo source files use the `.fm` extension.

## Quick Start

### Installation

**Prerequisites:**
- LLVM 20+
- GCC 12+ or Clang 16+ (C++23 compatible compiler)
- CMake 3.20+

**Build from source:**
```bash
git clone https://github.com/yourusername/fumo-compiler.git
cd fumo-compiler
./initialize_build.sh
./install.sh  # Installs 'fumo' command system-wide
```

### Your First Program

Create `hello.fm`:
```cpp
fn printf(format: char*, ...) -> i32;

fn main() -> void {
    printf("Hello, Fumo!\n");
}
```

**Compile and run:**
```bash
fumo hello.fm          # Compile and execute
# OR
fumo hello.fm -o hello # Compile to executable
./hello
```

---

## Language Tour

### Comments

Fumo supports C-style comments:
```cpp
// Single line comment


fn main() -> i32 {
    // Comments can appear anywhere
    return 0; 
}
```

### Variables and Types

Fumo is statically typed with type inference support:

```cpp
// Explicit typing
let x: i32 = 42;
let pi: f64 = 3.14159;

// Type inference
let count = 10;        // inferred as i32
let name = "Fumo";     // inferred as char*
let flag = true;       // inferred as bool

// Uninitialized declaration
let value: i32;        // Must specify type when uninitialized
// external declarations. if "extern" keyword is not added, globals are 0 initialized.
// this means that to link with libc globals, you need to use extern.
let extern stdin: FILE*; 
let extern stdout: FILE*;
```

**Primitive Types:**
- Integers: `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`
- Floating point: `f32`, `f64`
- Boolean: `bool`
- Character: `char`
- Void: `void`

**String Literals:**
String literals support common escape sequences: `\n` (newline), `\t` (tab), `\"` (quote), `\\` (backslash).

### Control Flow

**If statements** (parentheses optional):
```cpp
let score = 85;

if score >= 90 {
    printf("Grade A\n");
} else if score >= 80 {
    printf("Grade B\n");
} else {
    printf("Grade C or below\n");
}

// Condition with declaration
// you can only declare variables in if statements if you sorround them in parentheses
if (let result = calculate_something()) {
    printf("Got result: %d\n", result);
}
```

**While loops:**
```cpp
let i = 0;
while i < 10 {
    printf("%d ", i);
    i = i + 1;
}
```

### Functions

```cpp
// Function definition
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

// Function with no return value
fn print_message(msg: char*) -> void {
    printf("Message: %s\n", msg);
}

// External function declaration (for C interop)
fn printf(format: char*, ...) -> i32;
```

### Structs and Methods

```cpp
struct Point {
    let x: f64;
    let y: f64;
    
    // Instance method
    fn distance_from_origin() -> f64 {
        return sqrt(x * x + y * y);
    }
    
    // Static method (constructor)
    fn static new(x: f64, y: f64) -> Point {
        return Point {x, y};
    }
}

// Usage
let p1 = Point::new(3.0, 4.0);    // Static method call
let p2 = Point {1.0, 2.0};        // Direct initialization
let distance = p1.distance_from_origin();  // Instance method call
```

### Namespaces

```cpp
namespace geometry {
    struct Circle {
        let radius: f64;
        
        fn area() -> f64 {
            return 3.14159 * radius * radius;
        }
    }
    
    namespace utils {
        fn print_circle(c: Circle*) -> void {
            printf("Circle with radius %.2f\n", c->radius);
        }
    }
}

// Usage
let circle = geometry::Circle {5.0};
geometry::utils::print_circle(&circle);
```

### Pointers

```cpp
// Basic pointer usage
let value: i32 = 42;
let ptr: i32* = &value;    // Address-of operator
let deref: i32 = *ptr;     // Dereference

// Note: Pointer arithmetic exists but should be used carefully
// Separate variables aren't guaranteed to be adjacent in memory
let first: i32 = 10;
let second: i32 = 20;
let ptr1: i32* = &first;
// ptr1 + 1 may not point to 'second'
```
# The `any*` Type

The `any*` type in Fumo is equivalent to C's `void*` - a type-erased pointer that can store the address of any object. It exists only for C interoperability.

## Semantics

### Implicit Conversions
`any*` supports bidirectional implicit conversion with all single-level pointer types:

```fumo
let x: i32 = 42;
let s: char* = "hello";

let generic: any*;
generic = &x;     // i32* -> any* (implicit)
generic = s;      // char* -> any* (implicit)

let int_ptr: i32* = generic;   // any* -> i32* (implicit)
let str_ptr: char* = generic;  // any* -> char* (implicit)
```

### Null Compatibility
The `null` literal is of type `any*`, allowing it to be assigned to any pointer type:

```fumo
let int_ptr: i32* = null;     // any* -> i32* (implicit)
let str_ptr: char* = null;    // any* -> char* (implicit)
let any_ptr: any* = null;     // Direct assignment

if !any_ptr {                 // Null check with !
    printf("Pointer is null\n");
}

```

### Comparisons
`any*` can be compared with any pointer type for equality:

```fumo
let x: i32 = 10;
let int_ptr: i32* = &x;
let any_ptr: any* = &x;

if int_ptr == any_ptr {       // i32* == any* comparison
    printf("Same address\n");
}

if any_ptr == null {          // any* == null comparison
    printf("Pointer is null\n");
}
```

## Restrictions

`any*` cannot be used for operations that require knowing the pointed-to type:

```fumo
let any_ptr: any* = &some_value;

// ❌ These operations are forbidden:
*any_ptr              // Cannot dereference
any_ptr->member       // Cannot access members
any_ptr + 1           // Cannot do pointer arithmetic
```

## Use Cases

C interoperability:

```fumo
extern fn malloc(size: i64) -> any*;
extern fn memcpy(dst: any*, src: any*, size: i64) -> any*;
extern fn free(ptr: any*) -> void;

let buffer: any* = malloc(1024);
let data: i32 = 42;
memcpy(buffer, &data, 4);
free(buffer);
```

## Design Philosophy

`any*` exists solely for C interoperability. C libraries use `void*` for generic pointers, and `any*` provides direct compatibility without requiring wrapper functions or type conversions.

Avoid `any*` in application code. Use typed pointers instead. Once generics are added to Fumo, they will replace `any*` usage for type-safe generic programming.

## Memory Layout

`any*` is implemented as a raw pointer with the same size and alignment as any other pointer type (8 bytes on 64-bit systems). It carries no runtime type information - the programmer must track types manually.

---

## Syntax Reference

### Variable Declaration
```cpp
let name: type = value;          // Explicit type
let name = value;                // Type inference
let name: type;                  // Declaration only (uninitialized)
```

### Function Definition
```cpp
fn name(param1: type1, param2: type2) -> return_type {
    // body
}
```

### Struct Definition
```cpp
struct Name {
    let field1: type1;
    let field2: type2;
    
    fn method_name() -> return_type {
        // method body
    }
    
    fn static static_method() -> return_type {
        // static method body
    }

    fn other_method() -> return_type;
}
// allows out-of-line definitions
fn Name::other_method() -> return_type { 
    // definition of this function
}
```

### Control Flow
```cpp
// If statement
if condition {
    // body
} else if condition {
    // body
} else {
    // body
}

// While loop
while condition {
    // body
}
```

### Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Logical: `&&`, `||`, `!`
- Assignment: `=`
- Pointer: `&` (address-of), `*` (dereference)
- Member access: `.` (direct), `->` (through pointer)

---

## Type System

### Type Inference
Fumo can infer types in many contexts:
```cpp
let x = 42;           // i32
let y = 3.14;         // f64
let z = true;         // bool
let ptr = &x;         // i32*
```

### Type Conversion
```cpp
// Implicit conversions (widening)
let a: i32 = 42;
let b: i64 = a;       // i32 -> i64 (safe)

// Explicit casting syntax is planned but not yet implemented
```

### Composite Types
```cpp
// Pointers
let ptr: i32* = &value;

// Structs
struct Point { let x: f64; let y: f64; }
```

---

## Common Patterns

### Constructor Pattern
```cpp
struct Counter {
    let value: i32;
    
    fn static new() -> Counter {
        return Counter {0};
    }
    
    fn static with_value(val: i32) -> Counter {
        return Counter {val};
    }
    
    fn increment() -> void {
        value = value + 1;
    }
}

let counter1 = Counter::new();
let counter2 = Counter::with_value(10);
```

### Memory Management
```cpp
fn malloc(size: i64) -> any*;
fn free(ptr: char*) -> void;

struct Buffer {
    let data: char*;
    let size: i64;
    
    fn static new(size: i64) -> Buffer {
        return Buffer {malloc(size), size};
    }
    
    fn destroy() -> void {
        free(data);
    }
}

let buffer = Buffer::new(1024);
// Use buffer...
buffer.destroy();
```

### Complete Example Program
```cpp
fn printf(format: char*, ...) -> i32;
fn sqrt(x: f64) -> f64;

struct Vector2D {
    let x: f64;
    let y: f64;
    
    fn static new(x: f64, y: f64) -> Vector2D {
        return Vector2D {x, y};
    }
    
    fn magnitude() -> f64 {
        return sqrt(x * x + y * y);
    }
    
    fn add(other: Vector2D) -> Vector2D {
        return Vector2D {x + other.x, y + other.y};
    }
}

fn main() -> i32 {
    let v1 = Vector2D::new(3.0, 4.0);
    let v2 = Vector2D {1.0, 2.0};
    let v3 = v1.add(v2);
    
    printf("v1 magnitude: %.2f\n", v1.magnitude());
    printf("v3 = (%.1f, %.1f)\n", v3.x, v3.y);
    
    return 0;
}
```

---

## Comparisons to C/C++

### Key Differences

| C/C++ | Fumo | Notes |
|-------|------|-------|
| `int x = 5;` | `let x: i32 = 5;` | Explicit `let` keyword required |
| `int x;` | `let x: i32;` | Must specify type when uninitialized |
| `void func() {}` | `fn func() -> void {}` | Different function syntax |
| `// comment` | `// comment` | Same comment syntax |
| `struct.member` | `struct.member` | Same for direct access |
| `ptr->member` | `ptr->member` | Same for pointer access |
| `MyClass obj;` | `let obj = MyClass {};` | Must use initializer syntax |
| `int arr[5];` | *No direct equivalent* | Use pointers and manual allocation |

### What's Similar
- Pointer syntax and semantics
- Struct member access (`.` and `->`)
- C calling convention for external functions
- Manual memory management
- Namespace syntax (`::`-based)
- Comment syntax
- Operator precedence

### What's Different
- Static typing with inference
- `let` keyword for all variable declarations
- `fn` keyword for functions
- Static methods instead of constructors
- No built-in array types

---

## Operator Precedence

From highest to lowest precedence:

1. **Postfix**: `()` `.` `->`
2. **Unary**: `&` `*` `-` `!` (right-to-left)
3. **Multiplicative**: `*` `/` `%` (left-to-right)
4. **Additive**: `+` `-` (left-to-right)
5. **Relational**: `<` `<=` `>` `>=` (left-to-right)
6. **Equality**: `==` `!=` (left-to-right)
7. **Logical AND**: `&&` (left-to-right)
8. **Logical OR**: `||` (left-to-right)
9. **Assignment**: `=` (right-to-left)

---

## Planned Features
### Core Language
- Type casting system
- Generics/templates for containers and functions
- Sum types (tagged unions) with exhaustiveness checking
- Pattern matching for control flow and destructuring

### Control Flow
- `foreach` iteration over generic containers
- `break` and `continue` statements

### Standard Library
- Generic array and vector types (`fm::vec<T>, fm::array<T>`)
- String manipulation library (`fm::str`)
- Memory management utilities and `unique_ptr<T>` implementation
- `Optional<T>` impllementation
- Basic I/O beyond C interop

### Known Limitations
- No built-in array types (use pointers and manual allocation)
- No automatic memory management (manual `malloc`/`free` required)
- Limited standard library
- No exception handling mechanism
- Single-threaded compilation only

---

## Error Messages

The Fumo compiler provides detailed error messages with file location and context.

### Syntax Errors

**Missing semicolon:**
```
[error] in file 'example.fm' at line 12:
|     let x = y = SomeStruct {1, 2};
|             ^ expected ';'.
```

**Invalid assignment target:**
```
[error] in file 'example.fm' at line 11:
|     func(SomeStruct {1, 2}) = 5;
|                           ^ expression is not assignable.
```

**Unexpected initializer list:**
```
[error] in file 'example.fm' at line 6:
|     func() {1, 2, 3};
|          ^ expected identifier before initializer list.
```

### Type Errors

**Type mismatch:**
```
[error] in file 'example.fm' at line 8:
|     let x: i32 = 3.14;
|                  ^^^^ cannot assign f64 to i32.
```

**Undefined symbol:**
```
[error] in file 'example.fm' at line 15:
|     printf("%d", undeclared_var);
|                  ^^^^^^^^^^^^^^ undefined symbol 'undeclared_var'.
```

**Invalid member access:**
```
[error] in file 'example.fm' at line 20:
|     let val = point.z;
|                     ^ struct 'Point' has no member 'z'.
```

---

## Grammar Specification

Please check [the language specification](language_specification/fumo_bnf.md) for the language specification.

---

## Development Setup

### Building the Compiler

```bash
# Initial setup
git clone <repository>
cd fumo-compiler
bash initialize_build.sh

# Development cycle
bash rebuild.sh        # Rebuild after changes
bash test.sh           # Run test suite (use ./test.sh -help for more information)
```

### Project Structure

```
src/
├── lexer/              # Tokenization
├── parser/             # AST generation
├── semantic_analysis/  # Type checking, symbol resolution
├── codegen/            # LLVM IR generation
├── base_definitions/   # Core types and utilities
└── tests/              # Compiler tests
```

### Compiler Architecture

```
Source Code (.fm)
     ↓
   Lexer (Tokenization)
     ↓
   Parser (AST Generation)
     ↓
   Semantic Analysis (Type Checking, Symbol Resolution)
     ↓
   Code Generation (LLVM IR)
     ↓
   LLVM Backend (Optimization, Machine Code)
     ↓
   Executable
```

### Testing Strategy

Tests are organized by feature in the `tests/` directory:

- **Positive Tests**: Valid programs in directories like `if-statements/`, `while-tests/`, `structs-and-postfix/`
- **Negative Tests**: Invalid programs in `fail-*` directories that should produce specific errors

Example test organization:
```
tests/
├── if-statements/          # Valid if statement tests
├── fail-control-flow/      # Invalid control flow tests
├── structs-and-postfix/    # Valid struct usage
└── fail-static-functions/  # Invalid static method usage
```
the test script has a `-help` option for displaying all the options available for testing.

it allows passing individual files, passing specific directories, passing flags directly to the compiler, etc.

check `-help` for more info:
```
Test Runner Usage:
  ./test_runner [options] [test_directories...]
  ./test_runner --test <file_path> [options]

Options:
  --test <file>       - Run a single test file (can be relative or absolute path)
  --config <name>     - Use predefined configuration
  --opt <option>      - Add compiler option (can be used multiple times)
  --verbose, -v       - Show compiler output for passing tests
  --clear-outputs     - Clear all *-outputs directories and exit
  --list-configs      - List available configurations
  --help              - Show this help
```
