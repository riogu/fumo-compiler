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
Making a compiler based on a subset of the [ISO/IEC 9899:1999 specification](https://www.open-std.org/jtc1/sc22/WG14/www/docs/n1256.pdf) which i am using as a base for FumoLang.

## Doing:
- LLVM backend
- semantic analysis
## Currently done:
- Lexer (Tokenizer)
- Parser/AST (API is done, new additions are slowly added)
