# fumo-compiler
fumo lang example:
```cpp
let x = 69;
namespace foo {
    let x: i32 = 69420;
    namespace inner {
        let x: i32;
        fn func() -> void { x = 222222; }
    }
    fn func(a: i32, b: f64) -> void;
    struct SomeStruct {
        struct InnerStruct {
            let x = 123;
            let y = "string example";
        };
        let var = InnerStruct {69420, "passing a string"};
        let some_number: i32;
    };
}

fn foo::func(a: i32, b: f64) -> void {
    x = 111111 + a * b;
    inner::x = 333333;
}

let global_example: foo::SomeStruct = {{11111, "example"}, 6969};
fn some_func() -> i32* {
    return &global_example.some_number;
}
```

## Completed Features

### Language Frontend
- Lexer/Tokenizer with full token recognition
- Recursive descent parser with AST generation
- Symbol table and scoped name resolution
- Type system with primitives (`i32`, `f64`, `void`, pointers, strings)

### Language Constructs
- Variable declarations with type inference and explicit typing
- Function declarations and definitions with parameters
- Nested namespaces with qualified name access
- Struct definitions with nested structs
- Binary expressions (arithmetic, assignment)
- Global variable initialization
- Pointer operations and address-of operator
- Member access for struct fields

### Code Generation
- LLVM IR backend with optimization support
- Function code generation with proper linkage
- Global variable handling with initialization
- Cross-namespace symbol resolution
- Memory management for stack allocations
- Function calls and return value handling

### Development Tools
- Command-line interface with file input
- Multiple output formats (`--emit=asm`, `--emit=obj`, `--emit=llvm-ir`, `--emit=ast`)
- Debug output options (`--print-ir`, `--print-asm`, `--print-ast`)
- Error reporting and diagnostics

## Currently Working On

### Core LLVM Codegen
- Variable handling and storage optimization
- Arithmetic expression codegen refinement
- Function call mechanics and parameter passing
- Return value handling for all function types

### Struct Implementation
- Struct memory layout and alignment
- Member access code generation
- Nested struct support in codegen
- Struct initialization and constructor calls

### Pointer System
- Pointer load/store operations
- Address-of operator (`&`) implementation
- Pointer arithmetic and dereferencing
- Memory safety and bounds considerations

### Control Flow & String Literals
- `if/else` statement implementation
- `while` loop code generation
- String literal handling and storage

