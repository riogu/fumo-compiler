#include "parser/parser.hpp"

Result<Vec<ASTNode_ptr>, str> Parser::parse_tokens(Vec<Token>& tkns) {
    tokens = tkns;

    Vec<ASTNode_ptr> AST;

    ASTNode_ptr program = std::make_unique<ASTNode>();

    while (!tokens.empty()) {
        statement();
    }

    return AST;
}

void Parser::statement() {
    expression_statement();
}

void Parser::expression_statement() {}

void Parser::expression() {}

void Parser::assignment() {}

void Parser::equality() {}

void Parser::relational() {}

void Parser::add() {}

void Parser::subtract() {}

void Parser::multiply() {}

void Parser::unary() {}

void Parser::primary() {}
