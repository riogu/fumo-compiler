#include "parser/parser.hpp"

Vec<unique_ptr<ASTNode>> Parser::parse_tokens(Vec<Token>& tkns) {
    tokens = tkns;
    curr_tkn = tkns.begin();
    prev_tkn = tkns.begin();
    Vec<unique_ptr<ASTNode>> AST;

    while (curr_tkn+1 != tkns.end()) AST.push_back(statement());

    return AST;
}

// <statement> ::= <expression-statement>
[[nodiscard]] unique_ptr<ASTNode> Parser::statement() {
    return ASTNode {*prev_tkn, NodeKind::statement, Unary {expression_statement()}};
}

// <expression-statement> = <expression> ";"
[[nodiscard]] unique_ptr<ASTNode> Parser::expression_statement() {

    auto node = ASTNode {*prev_tkn, NodeKind::expression_statement, Unary {expression()}};
    expect_tkn(;); // terminate if we dont find a semicolon
    return node;
}

// <expression> ::= <assignment>
[[nodiscard]] unique_ptr<ASTNode> Parser::expression() {
    return ASTNode {*prev_tkn, NodeKind::expression, Unary {assignment()}};
}

// <assignment> ::= <equality> {"=" <assignment>}?
[[nodiscard]] unique_ptr<ASTNode> Parser::assignment() {
    auto node = equality();
    if (tkn_is(=))
        return ASTNode {*prev_tkn,
                        NodeKind::assignment,
                        Binary {std::move(node), assignment()}};
    else
        return node;
}

// <equality> ::= <relational> {("==" | "!=") <relational>}*
[[nodiscard]] unique_ptr<ASTNode> Parser::equality() {
    auto node = relational();
    while (1) {
        if (tkn_is(==))
            return ASTNode {*prev_tkn,
                            NodeKind::equal,
                            Binary {std::move(node), relational()}};
        else if (tkn_is(!=))
            return ASTNode {*prev_tkn,
                            NodeKind::not_equal,
                            Binary {std::move(node), relational()}};
        else
            return node;
    }
}

// <relational> ::= <add> { ("<" | ">" | "<=" | ">=")  <add> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::relational() {
    auto node = add();
    if (tkn_is(<))
        return ASTNode {*prev_tkn, NodeKind::less_than, Binary {std::move(node), add()}};

    else if (tkn_is(>))
        return ASTNode {*prev_tkn, NodeKind::less_than, Binary {add(), std::move(node)}};

    else if (tkn_is(<=))
        return ASTNode {*prev_tkn,
                        NodeKind::less_equals,
                        Binary {std::move(node), add()}};
    else if (tkn_is(>=))
        return ASTNode {*prev_tkn,
                        NodeKind::less_equals,
                        Binary {add(), std::move(node)}};
    else
        return node;
}

// <add> ::=  <multiplication> { "+" <multiplication> | "-" <multiplication> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::add() {
    auto node = multiply();
    while (1) {
        if (tkn_is(+)) {
            node =
                ASTNode {*prev_tkn, NodeKind::add, Binary {std::move(node), multiply()}};
            continue;
        } else if (tkn_is(-)) {
            node =
                ASTNode {*prev_tkn, NodeKind::sub, Binary {std::move(node), multiply()}};
            continue;
        } else
            return node;
    }
}

// <multiplication> ::=  <unary> { ("*" | "/") <unary> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::multiply() {
    auto node = unary();
    while (1) {
        if (tkn_is(*)) {
            node = ASTNode {*prev_tkn,
                            NodeKind::multiply,
                            Binary {std::move(node), unary()}};
            continue;
        } else if (tkn_is(/)) {
            node =
                ASTNode {*prev_tkn, NodeKind::divide, Binary {std::move(node), unary()}};
            continue;
        } else
            return node;
    }
}

// <unary> ::= ("-" | "!" | "~" | "+") <unary>
//           | <primary>
[[nodiscard]] unique_ptr<ASTNode> Parser::unary() {

    if (tkn_is(-)) return ASTNode {*prev_tkn, NodeKind::negate, Unary {unary()}};
    else if (tkn_is(!))
        return ASTNode {*prev_tkn, NodeKind::logic_not, Unary {unary()}};
    else if (tkn_is(~))
        return ASTNode {*prev_tkn, NodeKind::bitwise_not, Unary {unary()}};
    else
        return primary();
}

// <primary> ::= "(" <expression> ")"
//             | <identifier>
//             | <literal
[[nodiscard]] unique_ptr<ASTNode> Parser::primary() {

    if (is_tkn(str_to_tkn_type("("))) {
        auto node = ASTNode {*prev_tkn, NodeKind::expression, Unary {expression()}};
        consume_tkn_or_error(str_to_tkn_type(")"), ")");
        return node;

    } else if (tkn_is(identifier)) {
        return ASTNode {*prev_tkn,
                        NodeKind::identifier,
                        Primary {prev_tkn->value.value()}};

    } else if (tkn_is(int) || tkn_is(float) || tkn_is(string)) {
        return ASTNode {*prev_tkn, NodeKind::literal, Primary {prev_tkn->value.value()}};
    }

    report_error(curr_tkn, "expected expression.");
}
