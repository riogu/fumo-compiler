#include "parser/parser.hpp"

Vec<unique_ptr<ASTNode>> Parser::parse_tokens(Vec<Token>& tkns) {
    tokens = tkns;
    curr_tkn = tkns.begin();
    prev_tkn = tkns.begin();
    Vec<unique_ptr<ASTNode>> AST;

    while (curr_tkn + 1 != tkns.end()) {
        if (token_is(keyword, let)) //
            AST.push_back(variable_declaration());
        else if (token_is(keyword, fn))
            AST.push_back(function_declaration());
        else
            AST.push_back(statement());
    }
    return AST;
}

[[nodiscard]] unique_ptr<ASTNode> Parser::variable_declaration() {
    if (token_is(identifier)) {
        // NOTE: should be identifier list (add later)
    }
    if(token_is(:)) {
        // extern const int;
        // NOTE: should also allow user types (add later)
        auto fix_should_be_type = declaration_specifier();
        return initializer();
    }
    return {};
}

[[nodiscard]] unique_ptr<ASTNode> Parser::function_declaration() {
    return {};
}
// FIXME: continue this tomorrow
[[nodiscard]] unique_ptr<ASTNode> Parser::initializer() {
    return {};
}

// <statement> ::= <expression-statement>
[[nodiscard]] unique_ptr<ASTNode> Parser::statement() {
    if (token_is(keyword, return)) {
        return ASTNode {*prev_tkn, NodeKind::return_statement, Unary {expression_statement()}};
    }
    return expression_statement();
}

// <expression-statement> = <expression> ";"
[[nodiscard]] unique_ptr<ASTNode> Parser::expression_statement() {
    if (token_is(;)) {
        return ASTNode {*prev_tkn, NodeKind::empty_expr, Primary {}};
    }
    auto node = expression();
    expect_token(;); // terminate if we dont find a semicolon
    return node;
}

// <expression> ::= <assignment>
[[nodiscard]] unique_ptr<ASTNode> Parser::expression() {
    return assignment();
}

// <assignment> ::= <equality> {"=" <equality>}?
[[nodiscard]] unique_ptr<ASTNode> Parser::assignment() {
    auto node = equality();

    if (token_is(=)) {
        if (node->kind != NodeKind::identifier) {
            // NOTE: wont work later when we add postfix operators
            // you can change this to NodeKind::literal and its fine
            report_error((&node->source_token),
                         "expected lvalue on left-hand side of assignment.");
        }
        return ASTNode {*prev_tkn,
                        NodeKind::assignment,
                        Binary {std::move(node), initializer()}};

    } else
        return node;
}

// <equality> ::= <relational> {("==" | "!=") <relational>}*
[[nodiscard]] unique_ptr<ASTNode> Parser::equality() {
    auto node = relational();
    while (1) {
        if (token_is(==))
            return ASTNode {*prev_tkn,
                            NodeKind::equal,
                            Binary {std::move(node), relational()}};
        else if (token_is(!=))
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
    if (token_is(<))
        return ASTNode {*prev_tkn, NodeKind::less_than, Binary {std::move(node), add()}};
    else if (token_is(>))
        return ASTNode {*prev_tkn, NodeKind::less_than, Binary {add(), std::move(node)}};
    else if (token_is(<=))
        return ASTNode {*prev_tkn, NodeKind::less_equals, Binary {std::move(node), add()}};
    else if (token_is(>=))
        return ASTNode {*prev_tkn, NodeKind::less_equals, Binary {add(), std::move(node)}};
    else
        return node;
}

// <add> ::=  <multiplication> { "+" <multiplication> | "-" <multiplication> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::add() {
    auto node = multiply();
    while (1) {
        if (token_is(+)) {
            node = ASTNode {*prev_tkn, NodeKind::add, Binary {std::move(node), multiply()}};
            continue;
        } else if (token_is(-)) {
            node = ASTNode {*prev_tkn, NodeKind::sub, Binary {std::move(node), multiply()}};
            continue;
        } else
            return node;
    }
}

// <multiplication> ::=  <unary> { ("*" | "/") <unary> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::multiply() {
    auto node = unary();
    while (1) {
        if (token_is(*)) {
            node = ASTNode {*prev_tkn, NodeKind::multiply, Binary {std::move(node), unary()}};
            continue;
        } else if (token_is(/)) {
            node = ASTNode {*prev_tkn, NodeKind::divide, Binary {std::move(node), unary()}};
            continue;
        } else
            return node;
    }
}

// <unary> ::= ("-" | "!" | "~" | "+") <unary>
//           | <primary>
[[nodiscard]] unique_ptr<ASTNode> Parser::unary() {
    if (token_is(-)) return ASTNode {*prev_tkn, NodeKind::negate, Unary {unary()}};
    else if (token_is(!))
        return ASTNode {*prev_tkn, NodeKind::logic_not, Unary {unary()}};
    else if (token_is(~))
        return ASTNode {*prev_tkn, NodeKind::bitwise_not, Unary {unary()}};
    else
        return primary();
}

// <primary> ::= "(" <expression> ")"
//             | <identifier>
//             | <literal
[[nodiscard]] unique_ptr<ASTNode> Parser::primary() {
    if (token_is_str("(")) {
        auto node = equality();
        expect_token_str(")");
        return node;
    } else if (token_is(identifier)) {
        return ASTNode {*prev_tkn, NodeKind::identifier, Primary {prev_tkn->value.value()}};
    } else if (token_is(int) || token_is(float) || token_is(string)) {
        return ASTNode {*prev_tkn, NodeKind::literal, Primary {prev_tkn->value.value()}};
    }
    report_error(curr_tkn, "expected expression.");
}
