#include "base_definitions/ast_node.hpp"
#include "parser/parser.hpp"

ASTNode* Parser::parse_tokens(vec<Token>& tkns) {
    tokens = tkns; prev_tkn = tkns.begin(); curr_tkn = tkns.begin();

    while (curr_tkn + 1 != tkns.end()) {
        if (token_is_keyword(let))
            AST.push_back(variable_declaration());
        else if (token_is_keyword(fn))
            AST.push_back(function_declaration());
        else if (token_is_keyword(namespace))
            AST.push_back(namespace_declaration());
        else if (token_is_keyword(struct))
            AST.push_back(struct_declaration());
        else if (token_is_keyword(enum))
            AST.push_back(enum_declaration());
        else
            // AST.push_back(statement()); NOTE: no longer valid in global
            report_error((*curr_tkn), "expected declaration.");
    }
    return push(ASTNode {*tkns.begin(), NamespaceDecl {NamespaceDecl::translation_unit, "fumo_module", std::move(AST)}});
}

// <statement> ::= <expression-statement>
[[nodiscard]] ASTNode* Parser::statement() {
    if (token_is_keyword(return)) {
        return push(ASTNode {*prev_tkn,
                             UnaryExpr {UnaryExpr::return_statement, expression_statement()}});
    }
    return expression_statement();
}

// <expression-statement> = <expression> ";"
[[nodiscard]] ASTNode* Parser::expression_statement() {
    // not allowing empty expressions for now
    // if (token_is(;)) {
    //     return ASTNode {*prev_tkn, NodeKind::empty_expr, Primary {}};
    // }
    auto node = expression();
    expect_token(;); // terminate if we dont find a semicolon
    return node;
}

// <expression> ::= <assignment>
[[nodiscard]] ASTNode* Parser::expression() {
    return assignment();
}

// <assignment> ::= <equality> {"=" <initializer>}?
[[nodiscard]] ASTNode* Parser::assignment() {
    auto node = equality();

    if (token_is(=)) {
        // TODO: this error should be handled by semantic analysis
        // report_error(node->source_token,
        //              "expected lvalue on left-hand side of assignment.");
        return push(ASTNode {*prev_tkn,
                             BinaryExpr {BinaryExpr::assignment, node, initializer()}});
    }
    return node;
}

// <initializer> ::= "{" <initializer-list> "}"
//                 | <equality>
[[nodiscard]] ASTNode* Parser::initializer() {
    if (token_is_str("{")) {
        auto node = initializer_list();
        expect_token_str("}");
        return node;
    }
    return equality();
}

// <initializer-list> ::= <initializer> {","}?
//                      | <initializer> , <initializer-list>
[[nodiscard]] ASTNode* Parser::initializer_list() {
    // TODO: add optional named elements syntax "{.foo = 123123}"
    BlockScope init_list {BlockScope::initializer_list};
    init_list.nodes.push_back(initializer());
    while (1) {
        if (token_is_str(",")) {
            if (peek_token_str("}")) { // allow optional hanging comma
                return push(ASTNode {*prev_tkn, std::move(init_list)});
            }
            init_list.nodes.push_back(initializer());
            continue;
        }
        return push(ASTNode {*prev_tkn, std::move(init_list)});
    }
}

// <equality> ::= <relational> {("==" | "!=") <relational>}*
[[nodiscard]] ASTNode* Parser::equality() {
    auto node = relational();
    while (1) {
        if (token_is(==))
            return push(ASTNode {*prev_tkn,
                                 BinaryExpr {BinaryExpr::equal, node, relational()}});
        if (token_is(!=))
            return push(ASTNode {*prev_tkn,
                                 BinaryExpr {BinaryExpr::not_equal, node, relational()}});
        return node;
    }
}

// <relational> ::= <add> { ("<" | ">" | "<=" | ">=")  <add> }*
[[nodiscard]] ASTNode* Parser::relational() {
    auto node = add();
    if (token_is(<))
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::less_than,   node, add()}});
    if (token_is(>))
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::less_than,   add(), node}});
    if (token_is(<=))
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::less_equals, node, add()}});
    if (token_is(>=))
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::less_equals, add(), node}});
    return node;
}

// <add> ::=  <multiplication> { "+" <multiplication> | "-" <multiplication> }*
[[nodiscard]] ASTNode* Parser::add() {
    auto node = multiply();
    while (1) {
        if (token_is(+)) {
            node = push(ASTNode { *prev_tkn, BinaryExpr {BinaryExpr::add, node, multiply()}});
            continue;
        }
        if (token_is(-)) {
            node = push(ASTNode { *prev_tkn, BinaryExpr {BinaryExpr::sub, node, multiply()}});
            continue;
        }
        return node;
    }
}

// <multiplication> ::=  <unary> { ("*" | "/") <unary> }*
[[nodiscard]] ASTNode* Parser::multiply() {
    auto node = unary();
    while (1) {
        if (token_is(*)) {
            node = push( ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::multiply, node, unary()}});
            continue;
        }
        if (token_is(/)) {
            node = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::divide, node, unary()}});
            continue;
        }
        return node;
    }
}

// <unary> ::= ("-" | "!" | "~" | "+") <unary>
//           | <primary>
[[nodiscard]] ASTNode* Parser::unary() {
    if (token_is(-))
        return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::negate, unary()}});
    if (token_is(!))
        return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::logic_not, unary()}});
    if (token_is(~))
        return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::bitwise_not, unary()}});
    return primary();
}

// <primary> ::= "(" <expression> ")"
//             | <identifier>
//             | <literal
[[nodiscard]] ASTNode* Parser::primary() {
    if (token_is_str("(")) {
        auto node = equality();
        expect_token_str(")");
        return node;
    }
    if (token_is(int))
        return push(ASTNode {*prev_tkn,
                             PrimaryExpr {PrimaryExpr::integer, prev_tkn->literal.value()}});
    if (token_is(float))
        return push(ASTNode {*prev_tkn,
                             PrimaryExpr {PrimaryExpr::floating_point, prev_tkn->literal.value()}});
    if (token_is(string))
        return push(ASTNode {*prev_tkn,
                             PrimaryExpr {PrimaryExpr::str, prev_tkn->literal.value()}});
    if (token_is(identifier))
        return push(ASTNode {*prev_tkn,
                             PrimaryExpr {PrimaryExpr::identifier, prev_tkn->literal.value()}});

    report_error((*curr_tkn), "expected expression.");
}
