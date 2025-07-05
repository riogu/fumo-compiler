#include "parser/parser.hpp"
#include "lexer/token_definitions.hpp"

Vec<unique_ptr<ASTNode>> Parser::parse_tokens(Vec<Token>& tkns) {
    tokens = tkns;
    curr_tkn = tkns.begin();
    Vec<unique_ptr<ASTNode>> AST;

    while (!tokens.empty()) AST.push_back(statement());

    return AST;
}

// <statement> ::= <expression-statement>
[[nodiscard]] unique_ptr<ASTNode> Parser::statement() {
    return ASTNode {*curr_tkn, NodeKind::statement, Unary {expression_statement()}};
}

// <expression-statement> = <expression> ";"
[[nodiscard]] unique_ptr<ASTNode> Parser::expression_statement() {

    auto node = ASTNode {*curr_tkn, NodeKind::expression_statement, Unary {expression()}};
    expect_tkn(;); // terminate if we dont find a semicolon
    return node;
}

// <expression> ::= <assignment>
[[nodiscard]] unique_ptr<ASTNode> Parser::expression() {
    return ASTNode {*curr_tkn, NodeKind::expression, Unary {assignment()}};
}

// <assignment> ::= <equality> {"=" <assignment>}?
[[nodiscard]] unique_ptr<ASTNode> Parser::assignment() {
    auto node = equality();
    if (tkn_is(=))
        return ASTNode {*curr_tkn,
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
}

// <relational> ::= <add> { ("<" | ">" | "<=" | ">=")  <add> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::relational() {
    auto node = add();
    if (tkn_is(<))
        return ASTNode {*curr_tkn, NodeKind::less_than, Binary {std::move(node), add()}};

    else if (tkn_is(>))
        return ASTNode {*curr_tkn, NodeKind::less_than, Binary {add(), std::move(node)}};

    else if (tkn_is(<=))
        return ASTNode {*curr_tkn,
                        NodeKind::less_equals,
                        Binary {std::move(node), add()}};
    else if (tkn_is(>=))
        return ASTNode {*curr_tkn,
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
                ASTNode {*curr_tkn, NodeKind::add, Binary {std::move(node), multiply()}};
            continue;
        } else if (tkn_is(-)) {
            node =
                ASTNode {*curr_tkn, NodeKind::sub, Binary {std::move(node), multiply()}};
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
            node = ASTNode {*curr_tkn, NodeKind::add, Binary {std::move(node), unary()}};
            continue;
        } else if (tkn_is(/)) {
            node = ASTNode {*curr_tkn, NodeKind::sub, Binary {std::move(node), unary()}};
            continue;
        } else
            return node;
    }
}

// <unary> ::= ("-" | "!" | "~" | "+") <unary>
//           | <primary>
[[nodiscard]] unique_ptr<ASTNode> Parser::unary() {

    if (tkn_is(-)) return ASTNode {*curr_tkn, NodeKind::negate, Unary {unary()}};
    else if (tkn_is(!))
        return ASTNode {*curr_tkn, NodeKind::logic_not, Unary {unary()}};
    else if (tkn_is(~))
        return ASTNode {*curr_tkn, NodeKind::bitwise_not, Unary {unary()}};
    else
        return primary();
}
// <primary> ::= "(" <expression> ")"
//             | <identifier>
//             | <literal
[[nodiscard]] unique_ptr<ASTNode> Parser::primary() {
    // FIXME: finish primary() (does nothing right now)

    if (consume_tkn(str_to_tkn_type("("))) {
        auto node = ASTNode {*curr_tkn, NodeKind::expression, Unary {expression()}};
        consume_tkn_or_error(str_to_tkn_type(")"), ")");

    } else if (tkn_is(identifier))
        return ASTNode {*curr_tkn,
                        NodeKind::identifier,
                        Primary {curr_tkn->value.value()}};
    else if (tkn_is(int) || tkn_is(float) || tkn_is(string))
        return ASTNode {*curr_tkn, NodeKind::literal, Primary {curr_tkn->value.value()}};

    report_error("expected expression.");
}
