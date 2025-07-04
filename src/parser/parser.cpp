#include "parser/parser.hpp"
#include <algorithm>

Result<Vec<ASTNode_ptr>, str> Parser::parse_tokens(Vec<Token>& tkns) {
    tokens = tkns;
    curr_tkn = tkns.begin();
    Vec<ASTNode_ptr> AST;
    ASTNode_ptr program = std::make_unique<ASTNode>();

    while (!tokens.empty()) AST.push_back(statement());

    return AST;
}

// <statement> ::= <expression-statement>
[[nodiscard]] ASTNode_ptr Parser::statement() {
    return expression_statement();
}

// <expression-statement> = <expression> ";"
[[nodiscard]] ASTNode_ptr Parser::expression_statement() {
    return expression();
}

// <expression> ::= <assignment>
[[nodiscard]] ASTNode_ptr Parser::expression() {
    return assignment();
}


// <assignment> ::= <equality> {"=" <assignment>}?
[[nodiscard]] ASTNode_ptr Parser::assignment() {

    if (auto node = equality(); tkn_is(=))
        return ASTNode {*curr_tkn,
                        NodeKind::assignment,
                        Binary {std::move(node), assignment()}};
    else
        return node;
}

// <equality> ::= <relational> {"==" <relational>}*
[[nodiscard]] ASTNode_ptr Parser::equality() {

    if (auto node = relational(); tkn_is(==))
        return ASTNode {*curr_tkn,
                        NodeKind::equal,
                        Binary {std::move(node), relational()}};
    else if (tkn_is(!=))
        return ASTNode {*curr_tkn,
                        NodeKind::not_equal,
                        Binary {std::move(node), relational()}};
    else
        return node;
}

[[nodiscard]] ASTNode_ptr Parser::relational() {
    return {};
}

[[nodiscard]] ASTNode_ptr Parser::add() {
    return {};
}

[[nodiscard]] ASTNode_ptr Parser::subtract() {
    return {};
}

[[nodiscard]] ASTNode_ptr Parser::multiply() {
    return {};
}

[[nodiscard]] ASTNode_ptr Parser::unary() {
    return {};
}

[[nodiscard]] ASTNode_ptr Parser::primary() {
    return {};
}
