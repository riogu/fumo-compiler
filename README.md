# Fumo Programming Language Compiler

A statically-typed systems programming language compiler targeting LLVM IR, implemented in C++23.

## Example Program
```cpp
fn printf(format: i8*, ...) -> i32;

namespace math {
    struct Point {
        let x: f64;
        let y: f64;
        
        fn distance_from_origin() -> f64 {
            return x * x + y * y;
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

fn demonstrate_features() -> void {
    let point = math::Point {3.0, 4.0};
    let stats = math::utils::Stats {0, 0.0};
    
    stats.add_value(point.x);
    stats.add_value(point.y * 2.0);
    
    process_point(&point);
    
    let ptr: i32* = &global_counter;
    printf("Global counter: %d, Address: %p\n", *ptr, ptr);
    
    let small_int: i8 = 42;
    let big_int: i64 = small_int;
    
    if big_int == global_counter {
        printf("Values match!\n");
    }
}

fn main() -> i32 {
    printf("Fumo language feature demonstration\n");
    demonstrate_features();
    
    let nested_stats = math::utils::Stats {5, 100.0};
    printf("Average: %.2f\n", nested_stats.average());
    
    return 0;
}
```
## Language Specification

### Type System
- Static typing with compile-time checking
- Primitive types: `i8`, `i16`, `i32`, `i64`, `f32`, `f64`, `void`, `bool`
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
- Hand-written tokenizer and recursive descent parser
- AST generation with typed nodes
- Symbol table with scoped resolution
- Type checking and semantic analysis

### Backend
- LLVM IR code generation
- Integration with LLVM optimization passes
- C calling convention for libc compatibility
- Stack-based memory allocation

### Compiler Pipeline
```
Source → Tokenizer → Parser → AST → Semantic Analysis → LLVM IR → Object Code
```

## Architecture Details

### Parser
- LL(1) recursive descent with operator precedence
- AST nodes with pattern matching for analysis passes
- Source location tracking for error reporting

### Type System
- Unification-based type inference for local variables
- Automatic integer promotions and conversions
- LLVM struct types with alignment

### Code Generation
- Direct LLVM IR generation in SSA form
- Standard calling conventions
- Phi node generation for control flow


## Implementation Status

### Completed
- Lexical analysis and parsing
- AST generation with source location tracking
- Symbol table with namespace resolution
- Type checking and inference
- LLVM IR code generation
- Function calls and struct member access
- Control flow implementation (`if/else`)
- Integer type conversions
- Runtime null pointer checks
- Command-line interface with multiple output formats

### In Progress
- Loop constructs (`while`, `for`)
- Control flow analysis

## Build Requirements
- LLVM 20+
- C++23 compiler
- CMake
