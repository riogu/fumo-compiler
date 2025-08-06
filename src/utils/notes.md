# Missing (out of the planned features for ver 0.1):
   - all of the codegen for the language.
   - string literals
   - printf() (to show string literals in the output, needs linking with libc)
   - codegen for structs
   - codegen for postfix operators on structs
   - semantic analysis and codegen for pointers
   - if, else, for, while statements (basic control flow)
   - match construct
   - enums (done as sum types)
   - pointers
   - remove const keyword and add mut keyword, make const the default

# Plan for continuing after vacation:
   - start by adding pointers to the semantic analysis
   - add the missing code for struct declaration semantic analysis
   - fix a few TODO that might be relevant before moving to codegen
   - make codegen for everything we have done sem analysis for until this point:
      - calculate offsets for struct member access with the size of each type
      - make the llvm codegen for a basic struct and GEP for accessing members
      - finish functionc alls, add parameters to the function bodies
      - add codegen for pointers to local variables, and passing *this pointer to member functions
   if we do all this, and focus mainly on the codegen, we should have a lot done already and working.
   mainly should be 'finish the missing sem analysis' -> 'do the codegen up till where the language is at'.
   - after that, add control flow (if, while, for, etc)

# Codegen:
   - generate code for all structs first
   - generate code for all global variables
   - generate each function declaration sequentially
      (generates its own local variables and such but not local struct declarations)
   - 
