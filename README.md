# fumo-compiler
fumo lang example:
```cpp
// Forward declarations for libc functions
fn printf(format: i8*, ...) -> i32;

struct Fumo {
    let character: i8*;
    let size: i32;
    let is_premium: bool;
    let squish_count: i32;
    
    fn squish() -> void {
        squish_count = squish_count + 1;
        printf("*squish* %s fumo has been squished! Count: %d\n", character, squish_count);
    }
    
    fn display() -> void {
        if (is_premium) {
            printf(">> %s fumo (%dcm) - PREMIUM EDITION! Squished: %d times\n", character, size, squish_count);
        } else {
            printf(">> %s fumo (%dcm) - Standard edition. Squished: %d times\n", character, size, squish_count);
        }
    }
    
    fn get_rarity_score() -> i32 {
        if (is_premium) {
            return size * 3 + squish_count * 2;
        } else {
            return size + squish_count;
        }
    }
}

// Function that takes parameters and uses pointers
fn compare_fumos(first: Fumo*, second: Fumo*) -> Fumo* {
    let first_score = first.get_rarity_score();
    let second_score = second.get_rarity_score();
    
    printf("Comparing %s (score: %d) vs %s (score: %d)\n", 
           first.character, first_score, second.character, second_score);
    
    if (first_score > second_score) {
        return first;
    } else {
        return second;
    }
}

fn squish_fumo_n_times(fumo: Fumo*, times: i32) -> void {
    printf("Squishing %s fumo %d times:\n", fumo.character, times);
    let count = 0;
    
    // Simple loop using if and goto-like behavior (simulated)
    if (times > 0) {
        fumo.squish();
        if (times > 1) {
            fumo.squish();
            if (times > 2) {
                fumo.squish();
            }
        }
    }
}

fn get_fumo_by_name(collection: Fumo*, name: i8*) -> Fumo* {
    // This would normally be a loop, but using if chains for now
    if (collection.character == name) {
        return collection;
    }
    // In real implementation, this would iterate through array
    return collection; // fallback
}

let reimu: Fumo = Fumo {"Reimu", 20, true, 0};
let marisa: Fumo = Fumo {"Marisa", 18, false, 0};
let cirno: Fumo = Fumo {"Cirno", 15, true, 0};

fn main() -> i32 {
    printf("=== Welcome to the Fumo Collection! ===\n");
    
    // Display all fumos using pointers
    let reimu_ptr: Fumo* = &reimu;
    let marisa_ptr: Fumo* = &marisa;
    let cirno_ptr: Fumo* = &cirno;
    
    reimu_ptr.display();
    marisa_ptr.display();
    cirno_ptr.display();
    
    printf("\nSquishing session begins!\n");
    
    // Use function with parameters
    squish_fumo_n_times(reimu_ptr, 2);
    squish_fumo_n_times(cirno_ptr, 3);
    squish_fumo_n_times(marisa_ptr, 1);
    
    printf("\nUpdated status:\n");
    reimu.display();
    marisa.display();
    cirno.display();
    
    // Compare fumos using pointers and function parameters
    printf("\nFumo battles!\n");
    let winner1: Fumo* = compare_fumos(reimu_ptr, marisa_ptr);
    let winner2: Fumo* = compare_fumos(cirno_ptr, winner1);
    
    printf("\nChampion fumo: %s!\n", winner2.character);
    
    // Demonstrate pointer dereferencing
    let champion_score = winner2.get_rarity_score();
    printf("Final champion score: %d points\n", champion_score);
    
    return 0;
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
