# fumo-compiler
Making a compiler based on a subset of the [ISO/IEC 9899:1999 specification](https://www.open-std.org/jtc1/sc22/WG14/www/docs/n1256.pdf) which i am using as a base for FumoLang.

## Goal
- Implement maybe 30/40% of the specification for C99.
- Turn fumo-compiler into a compiler for FumoLang programming language.
## Doing:
- LLVM backend
- static analysis, expression evaluation, etc
## Currently done:
- Lexer (Tokenizer)
- Parser/AST (not finished, but it is trivial to add new syntax to it)
