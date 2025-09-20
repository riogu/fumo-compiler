# Fumo Programming Language Compiler

A statically-typed systems programming language compiler targeting LLVM IR, implemented in C++23.


For more detailed information, please check [docs/](docs/) for language specification and usage documentation.

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
    
    // static constructors and initializer lists
    let point1 = math::Point::new(3.0, 4.0);
    let point2 = math::Point {1.5, 2.5};
    let origin = math::Point::origin();
    
    let stats = math::utils::Stats::new();
    let preset_stats = math::utils::Stats {2, 10.0};
    
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
    printf("Preset average: %.2f\n", preset_stats.average());
    
    let ptr: i32* = &global_counter;
    printf("Global counter: %d\n", *ptr);
    
    return 0;
}
```
## Language Specification

### Type System
- Static typing with compile-time checking
- Primitive types: `i8`, `i16`, `i32`, `i64`, `f32`, `f64`, `void`, `bool`, etc
- Composite types: Pointers, structs with member functions
- Integer promotion following C conversion rules
- Null pointer dereference checking at runtime

### Language Features
- Hierarchical namespaces with qualified name resolution
- Struct definitions with member functions
- Control flow: `if/else` statements (parentheses optional)
- Variable declarations with explicit typing and type inference
- Function definitions with parameter passing
- Pointer arithmetic and address-of operations

## Implementation

### Frontend
- Hand-written lexer and recursive descent parser with AST generation
- Symbol table with scoped resolution
- Type checking and semantic analysis

### Backend
- LLVM IR code generation
- Integration with LLVM optimization passes
- C calling convention for libc compatibility

### Compiler Pipeline
```
Source → Lexer → Parser/AST → Semantic Analysis → LLVM IR Codegen → Object Code → Linking
```
## Testing
The compiler includes tests organized by feature:

**Core Language Features:**
- Control flow (`if-statements/`, `while-tests/`)
- Data types (`structs-and-postfix/`, `pointer-tests/`)
- Literals (`char-literals/`, `string-literals/`)
- Object initialization (`initializer-lists/`)
- Static member functions (`static-member-functions/`)

**Error Handling:**
- Correct parser errors (`fail-*` directories)
- Invalid syntax detection and reporting
- Type system violation detection

**Code Generation:**
- LLVM IR correctness verification
- C interop compatibility testing

## Build and Usage

### Requirements
- LLVM 20+
- C++23 compiler
- CMake

### Building
```bash
bash initialize_build.sh    # Initial setup
bash rebuild.sh             # Rebuild after changes
bash test.sh                # Run test suite (use ./test.sh -help for more information)
bash install.sh             # Install 'fumo' command system-wide
```

### Usage
```bash
fumo source.fm                # Compile and run
fumo source.fm -o output      # Compile to executable
fumo --help                   # Show all compiler options
```

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
- `optional<T>` implementation
- Basic I/O beyond C interop

## Final Note

As it is, this compiler only implements the "basic essential" features of a systems programming language.

The goal of this compiler is to have a working compiler with all the basics well tested and implemented,
so that i can later use it as a base to explore language design decisions, compiler backend optimizations,
and generally use it to continue learning about compilers.
