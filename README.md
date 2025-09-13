# fumo-compiler
fumo lang example:
```cpp
// Forward declarations for libc functions
fn printf(format: i8*, ...) -> i32;

struct Fumo {
    let name: i8*;
    let softness: i32;
    
    fn squish() -> void {
        softness = softness + 10;
        printf("*squish* %s is now %d%% soft!\n", name, softness);
    }
}

fn pet_fumo(fumo: Fumo*, times: i32) -> void {
    printf("Petting %s %d times...\n", fumo.name, times);
    fumo.squish();
}

let reimu: Fumo = Fumo {"Reimu", 50};
let cirno: Fumo = Fumo {"Cirno", 40};

fn main() -> i32 {
    printf("fumo lang example.\n");
    
    pet_fumo(&reimu, 3);
    pet_fumo(&cirno, 2);
    
    printf("\nfinal softness levels:\n");
    printf("Reimu: %d%% soft\n", reimu.softness);
    printf("Cirno: %d%% soft\n", cirno.softness);
    
    return 0;
}```

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
- Arithmetic expression codegen with full operator support
- Return value handling for all function types
- Standard C library integration and linking (libc functions available)
### Struct Implementation
- Member access code generation
- Nested struct support in codegen
- Struct initialization and constructor calls
### Pointer System
- Runtime null pointer dereference checks with error reporting
### User Features
- Command-line interface with file input
- Multiple output formats (`--emit=asm`, `--emit=obj`, `--emit=llvm-ir`, `--emit=ast`)
- Debug output options    (`--print-ir`, `--print-asm`, `--print-ast`)
- Error reporting and diagnostics
## Currently Working On
### Pointer System
- Pointer arithmetic operations
### Control Flow & String Literals
- `if/else` statement implementation
- `while` loop code generation
- String literal handling and storage
