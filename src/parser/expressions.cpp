#include "parser/parser.hpp"

NamedScope Parser::parse_tokens(vec<Token>& tkns) {
    tokens = tkns; prev_tkn = tkns.begin(); curr_tkn = tkns.begin();
    vec<ASTNode*> AST;

    while (curr_tkn + 1 != tkns.end()) {
        if (token_is_keyword(let)) //
            AST.push_back(push(variable_declaration()));
        else if (token_is_keyword(fn))
            AST.push_back(push(function_declaration()));
        else if (token_is_keyword(struct))
            AST.push_back(push(struct_declaration()));
        else if (token_is_keyword(enum))
            AST.push_back(push(enum_declaration()));
        else
            AST.push_back(push(statement()));
    }
    return NamedScope {std::move(AST)};
}

// <statement> ::= <expression-statement>
[[nodiscard]] unique_ptr<ASTNode> Parser::statement() {
    if (token_is_keyword(return)) {
        return ASTNode {*prev_tkn,
                        ASTNode::return_statement,
                        UnaryExpr {push(expression_statement())}};
    }
    return expression_statement();
}

// <expression-statement> = <expression> ";"
[[nodiscard]] unique_ptr<ASTNode> Parser::expression_statement() {
    // not allowing empty expressions for now
    // if (token_is(;)) {
    //     return ASTNode {*prev_tkn, NodeKind::empty_expr, Primary {}};
    // }
    auto node = expression();
    expect_token(;); // terminate if we dont find a semicolon
    return node;
}

// <expression> ::= <assignment>
[[nodiscard]] unique_ptr<ASTNode> Parser::expression() {
    return assignment();
}

// <assignment> ::= <equality> {"=" <initializer>}?
[[nodiscard]] unique_ptr<ASTNode> Parser::assignment() {
    auto node = equality();

    if (token_is(=)) {
        if (node->kind != ASTNode::identifier) {
            // NOTE: error wont work later when we add postfix operators
            // you can change this to  == NodeKind::literal and its fine
            report_error(node->source_token,
                         "expected lvalue on left-hand side of assignment.");
        }
        return ASTNode {*prev_tkn,
                        ASTNode::assignment,
                        BinaryExpr {push(std::move(node)), push(initializer())}};
    }
    return node;
}

// <initializer> ::= "{" <initializer-list> "}"
//                 | <equality>
[[nodiscard]] unique_ptr<ASTNode> Parser::initializer() {
    if (token_is_str("{")) {
        auto node = initializer_list();
        expect_token_str("}");
        return node;
    }
    return equality();
}

// <initializer-list> ::= <initializer> {","}?
//                      | <initializer> , <initializer-list>
[[nodiscard]] unique_ptr<ASTNode> Parser::initializer_list() {
    // TODO: add optional named elements syntax "{.foo = 123123}"
    BlockScope init_list {};
    init_list.nodes.push_back(push(initializer()));
    while (1) {
        if (token_is_str(",")) {
            if (peek_token_str("}")) { // allow optional hanging comma
                return ASTNode {*prev_tkn,
                                ASTNode::initializer_list,
                                std::move(init_list)};
            }
            init_list.nodes.push_back(push(initializer()));
            continue;
        }
        return ASTNode {*prev_tkn, ASTNode::initializer_list, std::move(init_list)};
    }
}

// <equality> ::= <relational> {("==" | "!=") <relational>}*
[[nodiscard]] unique_ptr<ASTNode> Parser::equality() {
    auto node = relational();
    while (1) {
        if (token_is(==))
            return ASTNode {*prev_tkn,
                            ASTNode::equal,
                            BinaryExpr {push(std::move(node)), push(relational())}};
        if (token_is(!=))
            return ASTNode {*prev_tkn,
                            ASTNode::not_equal,
                            BinaryExpr {push(std::move(node)), push(relational())}};
        return node;
    }
}

// <relational> ::= <add> { ("<" | ">" | "<=" | ">=")  <add> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::relational() {
    auto node = add();
    if (token_is(<))
        return ASTNode {*prev_tkn,
                        ASTNode::less_than,
                        BinaryExpr {push(std::move(node)), push(add())}};
    if (token_is(>))
        return ASTNode {*prev_tkn,
                        ASTNode::less_than,
                        BinaryExpr {push(add()), push(std::move(node))}};
    if (token_is(<=))
        return ASTNode {*prev_tkn,
                        ASTNode::less_equals,
                        BinaryExpr {push(std::move(node)), push(add())}};
    if (token_is(>=))
        return ASTNode {*prev_tkn,
                        ASTNode::less_equals,
                        BinaryExpr {push(add()), push(std::move(node))}};
    return node;
}

// <add> ::=  <multiplication> { "+" <multiplication> | "-" <multiplication> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::add() {
    auto node = multiply();
    while (1) {
        if (token_is(+)) {
            node = ASTNode {*prev_tkn,
                            ASTNode::add,
                            BinaryExpr {push(std::move(node)), push(multiply())}};
            continue;
        }
        if (token_is(-)) {
            node = ASTNode {*prev_tkn,
                            ASTNode::sub,
                            BinaryExpr {push(std::move(node)), push(multiply())}};
            continue;
        }
        return node;
    }
}

// <multiplication> ::=  <unary> { ("*" | "/") <unary> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::multiply() {
    auto node = unary();
    while (1) {
        if (token_is(*)) {
            node = ASTNode {*prev_tkn,
                            ASTNode::multiply,
                            BinaryExpr {push(std::move(node)), push(unary())}};
            continue;
        }
        if (token_is(/)) {
            node = ASTNode {*prev_tkn,
                            ASTNode::divide,
                            BinaryExpr {push(std::move(node)), push(unary())}};
            continue;
        }
        return node;
    }
}

// <unary> ::= ("-" | "!" | "~" | "+") <unary>
//           | <primary>
[[nodiscard]] unique_ptr<ASTNode> Parser::unary() {
    if (token_is(-))
        return ASTNode {*prev_tkn, ASTNode::negate, UnaryExpr {push(unary())}};
    if (token_is(!))
        return ASTNode {*prev_tkn, ASTNode::logic_not, UnaryExpr {push(unary())}};
    if (token_is(~))
        return ASTNode {*prev_tkn, ASTNode::bitwise_not, UnaryExpr {push(unary())}};
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
    }
    if (token_is(int))
        return ASTNode {*prev_tkn,
                        ASTNode::integer,
                        PrimaryExpr {prev_tkn->literal.value()}};
    if (token_is(float))
        return ASTNode {*prev_tkn,
                        ASTNode::floating_point,
                        PrimaryExpr {prev_tkn->literal.value()}};
    if (token_is(string))
        return ASTNode {*prev_tkn,
                        ASTNode::str,
                        PrimaryExpr {prev_tkn->literal.value()}};
    if (token_is(identifier))
        return ASTNode {*prev_tkn,
                        ASTNode::identifier,
                        PrimaryExpr {prev_tkn->literal.value()}};

    report_error((*curr_tkn), "expected expression.");
}
