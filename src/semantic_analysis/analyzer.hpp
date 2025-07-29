#pragma once
#include "base_definitions/ast_node.hpp"
#include <llvm/IR/Function.h>
#include <map>
/*  
    turn identifiers into nodes that represent their respective variable declarations,
    or provide an error
    also performs basic type checking
*/

// works just like a stack of scopes
// we push a std::map when entering a new scope
// we pop the current std::map when leaving a scope
struct SymbolTableStack {
    // std::map<str, ASTNode*> local_declarations {};
    vec<std::map<str, ASTNode*>> symbols_to_nodes {{}};
    // struct | enum | namespace 
    std::map<str, ASTNode*> named_scopes {}; // these are kept separately
    std::map<str, ASTNode*> function_names {}; // these are kept separately
    // all declarations are flattened and identifiers are changed to match them
    // for example:
    //   namespace foo  {struct bar {};} => struct "foo::bar"    {};
    //   struct    foo  {struct bar {};} => struct "foo{}::bar"  {};
    //   fn f() -> void {struct bar {};} => struct "foo()::bar"  {};
    //   struct    foo  {fn func()->void;} => fn "foo{}::func"(this: foo*) -> void;
    //   namespace foo  {fn func()->void;} => fn "foo::func"() -> void;
    // they are "global" but renamed internally
    // enums cant have other declarations inside them
    str curr_scope_name = "";
    auto push_to_scope(str identifier, ASTNode& node) {
        return symbols_to_nodes.back().insert({curr_scope_name + identifier, &node});
    }
    auto push_named_scope(str identifier, ASTNode& node) {
        return named_scopes.insert({curr_scope_name + identifier, &node});
    }
    auto push_function(str identifier, ASTNode& node) {
        return function_names.insert({curr_scope_name + identifier, &node});
    }


    // let x: i32 = 123123;                                  
    // let a: i32 = 123123;                                  
    // let z: i32 = 123123;                                  
    // fn func_name(a: i32, b: f64) -> const i32* {          
    //     x = 69420;                                        
    //     a = 69420;                                        
    //     let x = 1111111;                                  
    //     let var = foo::func();
    //     {                                                 
    //        z = 69;                                        
    //        let x: f64;                                    
    //        x = 12.0f;                                     
    //     }                                                 
    //     x = 213;                                          
    // }                                                     
};

// namespace foo {
//     void func();
//     struct bar {
//         struct inner {
//             static int func();
//         };
//         str x; 
//         int y;
//         void f();
//     };
//     struct wow;
// }
//
// struct gaming {
//     int x;
//     void func() {
//         x++;
//     }
//     int y;
// };


// struct foo::bar {};
// struct foo::bar::inner {};
// fn foo::func() -> void;

struct Analyzer {
    Analyzer(const File& file) { file_stream << file.contents; }
    void semantic_analysis(ASTNode* file_scope);

  private:
    std::stringstream file_stream;
    SymbolTableStack symbol_tree {};

    void analyze(ASTNode& node);
    void report_binary_error(const ASTNode& node, const BinaryExpr& bin);

    [[nodiscard]] str push_scope(str name, ASTNode& node);  
    void pop_scope(str prev_scope_name, ASTNode& node); 

    void add_to_scope(ASTNode& node);

    [[nodiscard]] ASTNode* find_node(std::string_view var_name);
    [[nodiscard]] constexpr bool is_compatible_t(const Type& a, const Type& b);
    [[nodiscard]] constexpr bool is_arithmetic_t(const Type& a);

};
