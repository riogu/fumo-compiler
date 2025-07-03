#pragma once
#include "lexer/token_definitions.hpp"
#include <memory>

// clang-format off

enum struct NodeKind {
    // binary
    add,                     // +                          
    subtract,                // -                      
    multiply,                // *                      
    divide,                  // /
    equal,                   // ==
    not_equal,               // !=                     
    less_than,               // <                      
    greater_than,            // <=                     
    assignment,              // =
    // unary
    expression_statement,    // Expression statement
    negate,                  // unary -
    // variable
    variable,                // Variable
    // value
    integer,                 // Integer
};



// binary, unary, var, func, funccall, 


// clang-format on


struct Expr; struct BinaryNode; struct Stmt; struct Var;
using ASTNode = std::variant<Expr, BinaryNode, Stmt, Var>;

struct Node {
    NodeKind kind;
    Token token; // token that originated this Node
};

struct BinaryNode : Node {
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

struct Expr : std::variant<BinaryNode> {
    using variant::variant;
};

struct Stmt : Node {};


namespace Parser {
Result<Vec<ASTNode>, str> parse_tokens(Vec<Token>& tokens);

void expression();
void expression_statement();
void assignment();

} // namespace Parser



