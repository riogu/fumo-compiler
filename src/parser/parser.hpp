#pragma once
#include "lexer/token_definitions.hpp"


enum struct NodeKind {
    _variable,
    _function,
    _expression,
    _expression_statement,
    

};


struct Node {
    // AST node
    NodeKind kind;
    Token token; // token that originated this Node
};

namespace Parser {
Result<Vec<Node>, str> parse_tokens(Vec<Token> tokens);
} // namespace Parser
