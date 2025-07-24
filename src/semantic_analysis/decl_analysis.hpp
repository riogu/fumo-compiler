// make:
// symbol table
// type checker
// declaration checker

/*
i want each scope to be linked to each other after the analysis phase
    > maybe we can uniquely name each scope
 
fn somefunc(x: i32, y: i64) {
    { // this would be somefunc::0
        
        { // this is somefunc::0::0
            this is always a unique scope name
        }
        struct e{}; somefunc::0::e // i guess (?)
        // this makes it easier to allow namespaces to be a thing
        // we can allow this kind of access only on namespaced stuff (not functions ofc)
    
    }
    { // somefunc::1
    }
    if () { // somefunc.2
    }
    if () { // somefunc.3 
    }

    {
        int x; // renamed to somefunc.4.x
    }
    {
        { somefunc_5_0
            x++; // this doesnt work 
                 // we can only go backwards
            // semantic analysis garantees this is an error
            // because it knows we dont have access to somefunc::0::x from this scope
        }
        {

        }
    }
}

the global namespace would have:
    all the global declarations, 
    all the function declarations
    all the struct/enum declarations
    all the namespaces
*/

#include "parser/ast_node.hpp"
#include <llvm/IR/Function.h>
#include <map>
/*  
    turn identifiers into nodes that represent their respective variable declarations,
    or provide an error


*/

struct SymbolTable {
    // updates the ASTNodes to represent a fully compliant program
    vec<std::map<str, ASTNode>> scope_to_nodes;
    i64 depth;

    void open_scope();
    void close_scope();
    void insert_variable();
    void find_variable();
};
