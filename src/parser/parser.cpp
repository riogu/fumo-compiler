#include "parser/parser.hpp"
#include "parser/ast_node.hpp"
#include "parser/parser_errors.hpp"

Vec<unique_ptr<ASTNode>> Parser::parse_tokens(Vec<Token>& tkns) {
    tokens = tkns; prev_tkn = tkns.begin(); curr_tkn = tkns.begin();
    Vec<unique_ptr<ASTNode>> AST;

    while (curr_tkn + 1 != tkns.end()) {
        if (token_is_keyword(fn)) //
            AST.push_back(function_declaration());
        else if (token_is_keyword(let))
            AST.push_back(variable_declaration());
        else
            AST.push_back(statement());
    }
    return AST;
}

// <function-declaration> ::=  <declarator> "(" {<parameter-list>}? ")"
//                            "->" {<declaration-specifier>}+ {<pointer>}* 
//                            {<compound-statement>}?
// fn cool_func(i32 a, float b) -> const i32* {}
[[nodiscard]] unique_ptr<ASTNode> Parser::function_declaration() {
    expect_token(identifier);
    Function function {.name = std::get<str>(prev_tkn->literal.value())};
    Token token = *prev_tkn;

    expect_token_str("(");
    function.parameters = parameter_list(); // could be an empty vector
    expect_token_str(")");

    expect_token(->);
    function.type = declaration_specifier();

    if (token_is_str("{")) {
        function.body = compound_statement();
        expect_token_str("}");
    } else
        expect_token(;);

    return ASTNode{token, NodeKind::function_declaration, std::move(function)};
}

// <variable-declaration> ::= <declarator-list> {":"}? 
//                            {<declaration-specifier>}+ {"=" <initializer>}?
[[nodiscard]] unique_ptr<ASTNode> Parser::variable_declaration() {
    // TODO: should be identifier list (add later)
    expect_token(identifier);
    auto node = ASTNode {*prev_tkn, NodeKind::variable_declaration};
    Variable variable {.name = std::get<str>(prev_tkn->literal.value())};
    // TODO: should also allow user types 
    if (token_is(:)) variable.type = declaration_specifier();

    variable.value = token_is(=) ? std::make_optional(initializer()) : std::nullopt;
    node.branch = std::move(variable);

    expect_token(;);
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
    InitializerList init_list {};
    init_list.nodes.push_back(initializer());
    while (1) {
        if (token_is_str(",")) {
            if (peek_token_str("}")) { // allow optional hanging comma
                return ASTNode {*prev_tkn, NodeKind::initializer_list, std::move(init_list)};
            }
            init_list.nodes.push_back(initializer());
            continue;
        }
        return ASTNode {*prev_tkn, NodeKind::initializer_list, std::move(init_list)};
    }
}

// <statement> ::= <expression-statement>
[[nodiscard]] unique_ptr<ASTNode> Parser::statement() {
    if (token_is_keyword(return)) {
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
            // NOTE: error wont work later when we add postfix operators
            // you can change this to  == NodeKind::literal and its fine
            report_error((&node->source_token),
                         "expected lvalue on left-hand side of assignment.");
        }
        return ASTNode {*prev_tkn,
                        NodeKind::assignment,
                        Binary {std::move(node), initializer()}};
    }
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
        if (token_is(!=))
            return ASTNode {*prev_tkn,
                            NodeKind::not_equal,
                            Binary {std::move(node), relational()}};
        return node;
    }
}

// <relational> ::= <add> { ("<" | ">" | "<=" | ">=")  <add> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::relational() {
    auto node = add();
    if (token_is(<))
        return ASTNode {*prev_tkn, NodeKind::less_than,   Binary {std::move(node), add()}};
    if (token_is(>))
        return ASTNode {*prev_tkn, NodeKind::less_than,   Binary {add(), std::move(node)}};
    if (token_is(<=))
        return ASTNode {*prev_tkn, NodeKind::less_equals, Binary {std::move(node), add()}};
    if (token_is(>=))
        return ASTNode {*prev_tkn, NodeKind::less_equals, Binary {add(), std::move(node)}};
    return node;
}

// <add> ::=  <multiplication> { "+" <multiplication> | "-" <multiplication> }*
[[nodiscard]] unique_ptr<ASTNode> Parser::add() {
    auto node = multiply();
    while (1) {
        if (token_is(+)) {
            node = ASTNode {*prev_tkn, NodeKind::add, Binary {std::move(node), multiply()}};
            continue;
        }
        if (token_is(-)) {
            node = ASTNode {*prev_tkn, NodeKind::sub, Binary {std::move(node), multiply()}};
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
            node = ASTNode {*prev_tkn, NodeKind::multiply, Binary {std::move(node), unary()}};
            continue;
        }
        if (token_is(/)) {
            node = ASTNode {*prev_tkn, NodeKind::divide, Binary {std::move(node), unary()}};
            continue;
        }
        return node;
    }
}

// <unary> ::= ("-" | "!" | "~" | "+") <unary>
//           | <primary>
[[nodiscard]] unique_ptr<ASTNode> Parser::unary() {
    if (token_is(-)) return ASTNode {*prev_tkn, NodeKind::negate, Unary {unary()}};
    if (token_is(!)) return ASTNode {*prev_tkn, NodeKind::logic_not, Unary {unary()}};
    if (token_is(~)) return ASTNode {*prev_tkn, NodeKind::bitwise_not, Unary {unary()}};
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
    if (token_is(identifier)) {
        return ASTNode {*prev_tkn, NodeKind::identifier, Primary {prev_tkn->literal.value()}};
    }
    if (token_is(int) || token_is(float) || token_is(string)) {
        return ASTNode {*prev_tkn, NodeKind::literal, Primary {prev_tkn->literal.value()}};
    }
    report_error(curr_tkn, "expected expression.");
}
