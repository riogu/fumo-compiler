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
    return expression_statement();
}

// <expression-statement> = <expression> ";"
[[nodiscard]] unique_ptr<ASTNode> Parser::expression_statement() {

    return expression();
}

// <expression> ::= <assignment>
[[nodiscard]] unique_ptr<ASTNode> Parser::expression() {
    return assignment();
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

    // !var; +var; -12312; int epic = -var;
}

// epic = 123123;
//        ^ this guy is a primary literal.
// epic = wow;

// <primary> ::= "(" <expression> ")"
//             | <identifier>
//             | <literal
[[nodiscard]] unique_ptr<ASTNode> Parser::primary() {
    // FIXME: finish primary() (does nothing right now)

    if (consume_tkn(str_to_tkn_type("("))) {
        auto node =
            ASTNode {*curr_tkn, NodeKind::expression_statement, Unary {expression()}};
        consume_tkn_or_error(str_to_tkn_type(")"), ")");
    } else if (tkn_is(identifier))
        return ASTNode {*curr_tkn, NodeKind::variable, Unary {}};
    else if (tkn_is(int))
        return ASTNode {*curr_tkn, NodeKind::numeric_literal, Unary {}};
    else if (tkn_is(float))
        return ASTNode {*curr_tkn, NodeKind::numeric_literal, Unary {}};

    report_error("expected expression.");
}
