# Fumo Programming Language Compiler

A statically-typed systems programming language compiler targeting LLVM IR, implemented in C++23.

---

## Quick Overview

Fumo is a systems programming language featuring:
- Static typing with type inference
- C++ Namespaces and structs with member functions  
- Manual memory management with C/libc interoperability
- LLVM-based compilation to native code
For detailed information, see [docs/](docs/) for language specification and usage documentation.

---

## Example Program
```cpp
fn printf(format: char*, ...) -> i32;

namespace math {
    struct Point {
        let x: f64;
        let y: f64;
        
        fn distance_from_origin() -> f64 {
            return x * x + y * y;
        }
        
        fn static new(x: f64, y: f64) -> Point {
            return Point {x, y};
        }
        
        fn static origin() -> Point {
            return Point {0.0, 0.0};
        }
    }
    
    namespace utils {
        struct Stats {
            let count: i32;
            let sum: f64;
            
            fn add_value(val: f64) -> void {
                count = count + 1;
                sum = sum + val;
            }
            
            fn average() -> f64 {
                if count > 0 {
                    return sum / count;
                }
                return 0.0;
            }
            
            fn static new() -> Stats {
                return Stats {0, 0.0};
            }
        }
    }
}

let global_counter: i32 = 100;

fn process_point(pt: math::Point*) -> void {
    printf("Point: (%.2f, %.2f)\n", pt->x, pt->y);
    
    if (let dist: f64 = pt->distance_from_origin()) {
        if dist > 25.0 {
            printf("Point is far from origin: %.2f\n", dist);
        } else {
            printf("Point is close to origin: %.2f\n", dist);
        }
    }
}

fn main() -> i32 {
    printf("Fumo language demonstration\n");
    
    let point1 = math::Point::new(3.0, 4.0);
    let point2 = math::Point {1.5, 2.5};
    let origin = math::Point::origin();
    
    let stats = math::utils::Stats::new();
    
    let i: i32 = 0;
    while i < 3 {
        if i == 0 {
            stats.add_value(point1.x);
        } else if i == 1 {
            stats.add_value(point1.y);
        } else {
            stats.add_value(point2.y);
        }
        i = i + 1;
    }
    
    process_point(&point1);
    process_point(&origin);
    
    printf("Stats average: %.2f\n", stats.average());
    printf("Global counter: %d\n", global_counter);
    
    return 0;
}
```

---

## Compiler Features

### Language Features
- **Type System:** Static typing with compile-time checking and type inference
- **Primitive Types:** `i8`-`i64`, `u8`-`u64`, `f32`, `f64`, `bool`, `char`, `void`, `any*`
- **Composite Types:** Pointers, structs with member functions and static methods
- **Namespaces:** Hierarchical namespaces with qualified name resolution
- **Control Flow:** `if`/`else` statements (parentheses optional), `while` loops
- **C Interoperability:** C calling convention for libc compatibility

### Compiler Capabilities
- **Output Formats:** LLVM IR (`.ll`), Assembly (`.asm`), Object files (`.o`), Executables
- **Optimization Levels:** O0 (none), O1 (basic), O2 (default), O3 (aggressive)
- **Debug Outputs:** AST dumps, IR dumps, verbose compilation mode
- **Linking Options:** Static/dynamic linking, custom linker selection, library search paths

See `fumo --help` for complete compiler options.

---

## Implementation

### Frontend
- Hand-written lexer with full token support
- Recursive descent parser generating typed AST
- Symbol table with scoped resolution and namespace support
- Comprehensive type checking and semantic analysis

### Backend
- LLVM IR code generation with optimization pipeline integration
- Support for multiple optimization levels (O0-O3)
- C calling convention for external function compatibility

### Compiler Pipeline
```
Source → Lexer → Parser/AST → Semantic Analysis → LLVM IR → Optimization → Object Code → Linking
```

---

## Testing

The compiler includes 200+ tests organized by feature:

**Core Language Features:**
- Control flow (`if-statements/`, `while-tests/`)
- Data types (`structs-and-postfix/`, `pointer-tests/`)
- Literals (`char-literals/`, `string-literals/`)
- Object initialization (`initializer-lists/`)
- Static member functions (`static-member-functions/`)

**Error Handling:**
- Parser error detection (`fail-*` directories)
- Invalid syntax detection and reporting
- Type system violation detection

**Code Generation:**
- LLVM IR correctness verification
- C interop compatibility testing

---

## Build and Usage

### Requirements
- LLVM 20+
- C++23 compiler (GCC 12+ or Clang 16+)
- CMake 3.20+

### Building
```bash
bash initialize_build.sh    # Initial setup
bash rebuild.sh             # Rebuild after changes
bash test.sh                # Run test suite (use ./test.sh -help for options)
bash install.sh             # Install 'fumo' command system-wide
```

### Usage
```bash
fumo source.fm                # Compile and run
fumo source.fm -o output      # Compile to executable
fumo source.fm --emit-ir      # Generate LLVM IR
fumo source.fm --O3 -o fast   # Compile with aggressive optimizations
fumo --help                   # Show all compiler options
```

---

## Currently Working On

### Generics Implementation
Generic struct definitions with type parameters, multiple type parameters, generic function definitions, template instantiation for concrete types, and nested generic types.

**Current Status:** Core generic syntax parsing and type system integration working. Currently finishing monomorphization and semantic analysis for generic instances.

```cpp
struct Container[T] {
    let value: T;
    fn get() -> T { return value; }
    fn static make(v: T) -> Container[T];
}

fn static Container[T]::make(v: T) -> Container[T] { 
    return Container[T]{v}; 
}

fn main() -> void {
    let int_container = Container[i32]::make(42);
    let float_container = Container[f64]{3.14};
    let nested = Container[Container[i32]]::make(int_container);
}
```

---

## Planned Features

### Core Language
- Type casting system
- Sum types (tagged unions) with exhaustiveness checking
- Pattern matching for control flow and destructuring

### Control Flow
- `foreach` iteration over generic containers

### Standard Library
- Generic array and vector types (`fm::vec[T]`, `fm::array[T]`)
- String manipulation library (`fm::str`)
- Memory management utilities (`unique_ptr[T]`, `optional[T]`)
- Basic I/O beyond C interop

---

## Project Goals

This compiler implements the "basic essential" features of a systems programming language with a focus on correctness and completeness.

The goal is to have a working compiler with all fundamentals well-tested and implemented, serving as a foundation for exploring:
- Language design decisions
- Compiler optimization techniques
- Advanced type system features

---

## Documentation

Comprehensive documentation is available in the [docs/](docs/) directory:
- Language specification and grammar
- Type system details
- Usage examples and patterns
- Compiler architecture overview

---

## Project Structure

```
src/
├── lexer/              # Tokenization
├── parser/             # AST generation  
├── semantic_analysis/  # Type checking, symbol resolution
├── codegen/            # LLVM IR generation
├── base_definitions/   # Core types and utilities
└── tests/              # Compiler tests (200+)
```
