# TODO:
   - Finish find_declaration()
   - add function call postfix semantic analysis
   - add codegen for function declarations and function calls
   - add codegen for global variables
   - make tests for the codegen

> this would make already a pretty nice "basic" imperative language subset of fumolang
> only add more stuff if you finish everything in time somehow

# Missing (out of the planned features for ver 0.1):
   - string literals
   - printf() (to show string literals in the output, needs linking with libc)
   - semantic analysis for postfix operators
   - codegen for structs
   - codegen for postfix operators on structs
   - semantic analysis and codegen for pointers
   - if, else, for, while statements (basic control flow)
   - match construct
   - enums (done as sum types)
