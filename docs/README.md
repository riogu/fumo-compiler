---

## Best Practices

### Code Organization
- Use namespaces to group related functionality
- Prefer static constructor methods over direct struct initialization for complex setup
- Keep external C function declarations at the top of files
- Use descriptive names for variables and functions

### Memory Management
- Always pair `malloc` with `free`
- Implement `destroy()` methods for structs that allocate memory
- Check for null pointers before dereferencing
- Use stack allocation when possible

### Type Usage
- Use explicit types for function parameters and return values
- Rely on type inference for local variables when the type is obvious
- Prefer specific integer types (`i32`, `i64`) over generic `int`

### Error Handling
- Use return codes for functions that can fail
- Document expected error conditions in comments
- Check return values from C library functions

---

## Standard Library

Currently, Fumo relies on C standard library functions for most operations. You can declare and use C functions directly:

```cpp
// C standard library declarations
fn malloc(size: i64) -> i8*;
fn free(ptr: i8*) -> void;
fn memcpy(dest: i8*, src: i8*, n: i64) -> void;
fn printf(format: const i8*, ...) -> i32;
fn scanf(format: const i8*, ...) -> i32;

// Usage
let ptr = malloc(100);
printf("Allocated memory at: %p\n", ptr);
free(ptr);
```

### Future Standard Library

Planned standard library components will be in the `fm` namespace:

- `fm::str` - String manipulation
- `fm::vec<T>` - Dynamic arrays  
- `fm::map<K,V>` - Hash maps
- `fm::optional<T>` - Optional values

---

## Compiler Options

```bash
fumo [options] source.fm

Options:
  -o <file>     Output executable name
  -c            Compile only (no linking)
  --emit-llvm   Output LLVM IR instead of executable
  --help        Show help message
  --version     Show compiler version

Examples:
  fumo hello.fm                    # Compile and run
  fumo hello.fm -o hello          # Compile to 'hello' executable
  fumo hello.fm --emit-llvm       # Output LLVM IR
```

## Roadmap

### Current Features ✅
- Static typing with inference
- Functions and structs
- Basic control flow (if/while)
- Namespaces
- Pointer operations
- C interoperability

### In Progress 🚧
- Comprehensive documentation
- Better error messages
- More examples and tutorials

### Planned Features 🎯
- **Type System**: Generics, sum types, pattern matching
- **Control Flow**: `foreach`, `break`/`continue`
- **Standard Library**: Collections, strings, memory management
- **Language Features**: Type casting, better array support

### Future Considerations 💭
- Package system
- Async/await
- Compile-time evaluation
- IDE integration

---

### Reporting Issues

If you encounter bugs, unexpected behavior, or have suggestions for improvements, please make an issue. When reporting issues, include:

- Fumo source code that reproduces the problem
- Expected vs actual behavior
- Full error messages or output (with backtrace)
------

## Error Reference

### Common Compile Errors

**Type Mismatch:**
```
error: cannot assign f64 to i32
  let x: i32 = 3.14;
               ^^^^
```
*Solution:* Use correct type or explicit conversion.

**Undefined Symbol:**
```
error: undefined symbol 'undeclared_var'
  printf("%d", undeclared_var);
               ^^^^^^^^^^^^^^
```
*Solution:* Declare the variable before use.

**Invalid Member Access:**
```
error: struct 'Point' has no member 'z'
  let val = point.z;
                  ^
```
*Solution:* Check struct definition for available members.

---# Fumo Programming Language Documentation

## Table of Contents

### Getting Started
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
- [Migrating from C/C++](#migrating-from-cc)
- [Best Practices](#best-practices)

### Reference
- [Grammar Specification](#grammar-specification)
- [Standard Library](#standard-library)
- [Compiler Options](#compiler-options)
- [Error Messages](#error-messages)

### Development
- [Development Setup](#development-setup)
- [Compiler Architecture](#compiler-architecture)
- [Testing Guidelines](#testing-guidelines)
- [Feedback and Issues](#feedback-and-issues)

---

## Quick Start

### Installation

**Requirements:**
- LLVM 20+
- C++23 compatible compiler (GCC 12+, Clang 16+)
- CMake 3.20+

**Build from source:**
```bash
git clone https://github.com/yourusername/fumo-compiler.git
cd fumo-compiler
./initialize_build.sh
./install.sh  # Optional: install 'fumo' command system-wide
```

### Your First Program

Create `hello.fm`:
```cpp
fn printf(format: i8*, ...) -> i32;

fn main() -> i32 {
    printf("Hello, Fumo!\n");
    return 0;
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

### Variables and Types

Fumo is statically typed with type inference support:

```cpp
// Explicit typing
let x: i32 = 42;
let pi: f64 = 3.14159;

// Type inference
let count = 10;        // inferred as i32
let name = "Fumo";     // inferred as i8*
let flag = true;       // inferred as bool
```

**Primitive Types:**
- Integers: `i8`, `i16`, `i32`, `i64`
- Floating point: `f32`, `f64`
- Boolean: `bool`
- Character: `char`
- Void: `void`

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
fn print_message(msg: i8*) -> void {
    printf("Message: %s\n", msg);
}

// External function declaration (for C interop)
fn printf(format: i8*, ...) -> i32;
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
let value: i32 = 42;
let ptr: i32* = &value;    // Address-of operator
let deref: i32 = *ptr;     // Dereference

// Pointer arithmetic
let first: i32 = 10;
let second: i32 = 20;
let ptr1: i32* = &first;
let ptr2: i32* = &second;
printf("First: %d, Second: %d\n", *ptr1, *(ptr1 + 1));
```

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

// TODO: Explicit casting syntax (planned feature)
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
fn malloc(size: i64) -> i8*;
fn free(ptr: i8*) -> void;

struct Buffer {
    let data: i8*;
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

---

## Migrating from C/C++

### Key Differences

| C/C++ | Fumo | Notes |
|-------|------|-------|
| `int x = 5;` | `let x: i32 = 5;` | Explicit `let` keyword |
| `void func()` | `fn func() -> void` | Different function syntax |
| `struct.member` | `struct.member` | Same for direct access |
| `ptr->member` | `ptr->member` | Same for pointer access |
| `MyClass obj;` | `let obj = MyClass {};` | Explicit initialization |

### What's Similar
- Pointer syntax and semantics
- Struct member access
- C calling convention for external functions
- Manual memory management

### What's Different
- Static typing with inference
- `let` keyword for all variable declarations
- `fn` keyword for functions
- Static methods instead of constructors
- Namespace syntax
- No implicit conversions

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

```bnf
program ::= (namespace_decl | function_decl | struct_decl | var_decl)*

function_decl ::= 'fn' IDENTIFIER '(' param_list? ')' '->' type block_stmt

struct_decl ::= 'struct' IDENTIFIER '{' struct_member* '}'

struct_member ::= var_decl | function_decl

namespace_decl ::= 'namespace' IDENTIFIER '{' (namespace_decl | function_decl | struct_decl)* '}'

var_decl ::= 'let' IDENTIFIER (':' type)? ('=' expression)? ';'

type ::= primitive_type | pointer_type | qualified_type
primitive_type ::= 'i8' | 'i16' | 'i32' | 'i64' | 'f32' | 'f64' | 'void' | 'bool'
pointer_type ::= type '*'
qualified_type ::= (IDENTIFIER '::')* IDENTIFIER

statement ::= expression_stmt | if_stmt | while_stmt | return_stmt | block_stmt | var_decl

expression ::= assignment_expr
assignment_expr ::= logical_or_expr ('=' assignment_expr)?
logical_or_expr ::= logical_and_expr ('||' logical_and_expr)*
logical_and_expr ::= equality_expr ('&&' equality_expr)*
equality_expr ::= relational_expr (('==' | '!=') relational_expr)*
relational_expr ::= additive_expr (('<' | '<=' | '>' | '>=') additive_expr)*
additive_expr ::= multiplicative_expr (('+' | '-') multiplicative_expr)*
multiplicative_expr ::= unary_expr (('*' | '/' | '%') unary_expr)*
unary_expr ::= postfix_expr | ('&' | '*' | '-' | '!') unary_expr
postfix_expr ::= primary_expr postfix_suffix*
postfix_suffix ::= '.' IDENTIFIER | '->' IDENTIFIER | '(' argument_list? ')' | '[' expression ']'

primary_expr ::= IDENTIFIER | INTEGER_LITERAL | FLOAT_LITERAL | STRING_LITERAL | 
                CHAR_LITERAL | 'true' | 'false' | '(' expression ')' | 
                qualified_type '{' initializer_list? '}'
```

---

## Development Setup

### Building the Compiler

```bash
# Initial setup
git clone <repository>
cd fumo-compiler
./initialize_build.sh

# Development cycle
./rebuild.sh        # Rebuild after changes
./test.sh           # Run test suite
```

### Project Structure

```
src/
├── lexer/          # Tokenization
├── parser/         # AST generation
├── semantic_analysis/  # Type checking, symbol resolution
├── codegen/        # LLVM IR generation
├── base_definitions/   # Core types and utilities
└── tests/          # Compiler tests
```

### Compiler Architecture

```
Source Code
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

Tests are organized by feature in the `src/tests/` directory:

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

