#include "base_definitions/ast_node.hpp"
#include "parser/parser.hpp"

ASTNode* Parser::parse_tokens(vec<Token>& tkns) {
    tokens = tkns;
    prev_tkn = tkns.begin();
    curr_tkn = tkns.begin();

    while (curr_tkn + 1 != tkns.end()) {
        if (token_is_keyword(let)) AST.push_back(variable_declaration());
        else if (token_is_keyword(fn))
            AST.push_back(function_declaration());
        else if (token_is_keyword(namespace))
            AST.push_back(namespace_declaration());
        else if (token_is_keyword(struct))
            AST.push_back(struct_declaration());
        else if (token_is_keyword(enum))
            AST.push_back(enum_declaration());
        else
            AST.push_back(statement()); /* NOTE: no longer valid in global */
        // report_error((*curr_tkn), "expected declaration.");
    }
    auto id = push(ASTNode {*tkns.begin(), Identifier {Identifier::declaration_name, "fumo_module"}});
    return push(ASTNode {*tkns.begin(), NamespaceDecl {NamespaceDecl::translation_unit, id, std::move(AST)}});
}

// <statement> ::= <expression-statement>
[[nodiscard]] ASTNode* Parser::statement() {
    if (token_is_keyword(return)) {
        return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::return_statement, expression_statement()}});
    }
    return expression_statement();
}

// <expression-statement> = <expression> ";"
[[nodiscard]] ASTNode* Parser::expression_statement() {
    // not allowing empty expressions for now
    // if (token_is(;)) {}
    auto node = expression();
    expect_token(;); // terminate if we dont find a semicolon
    return node;
}

// <expression> ::= <assignment>
[[nodiscard]] ASTNode* Parser::expression() {
    return assignment();
}

// <assignment> ::= <equality> {"=" <equality>}?
[[nodiscard]] ASTNode* Parser::assignment() {
    auto node = equality();

    if (token_is(=)) {
        if (!is_branch<Identifier, PostfixExpr>(node)) {
            report_error(node->source_token, "expression is not assignable.");
        }
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::assignment, node, equality()}});
    }
    return node;
}

// <equality> ::= <relational> {("==" | "!=") <relational>}*
[[nodiscard]] ASTNode* Parser::equality() {
    auto node = relational();
    while (1) {
        if (token_is(==)) return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::equal, node, relational()}});
        if (token_is(!=)) return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::not_equal, node, relational()}});
        return node;
    }
}

// <relational> ::= <add> { ("<" | ">" | "<=" | ">=")  <add> }*
[[nodiscard]] ASTNode* Parser::relational() {
    auto node = add();
    if (token_is(<)) return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::less_than, node, add()}});
    if (token_is(>)) return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::less_than, add(), node}});
    if (token_is(<=)) return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::less_equals, node, add()}});
    if (token_is(>=)) return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::less_equals, add(), node}});
    return node;
}

// <add> ::=  <multiplication> { "+" <multiplication> | "-" <multiplication> }*
[[nodiscard]] ASTNode* Parser::add() {
    auto node = multiply();
    while (1) {
        if (token_is(+)) {
            node = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::add, node, multiply()}});
            continue;
        }
        if (token_is(-)) {
            node = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::sub, node, multiply()}});
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
            node = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::multiply, node, unary()}});
            continue;
        }
        if (token_is(/)) {
            node = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::divide, node, unary()}});
            continue;
        }
        return node;
    }
}

// <unary> ::= ("-" | "!" | "~" | "+" | "*") <unary>
//           | <postfix>
[[nodiscard]] ASTNode* Parser::unary() {
    if (token_is(-)) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::negate,      unary()}});
    if (token_is(!)) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::logic_not,   unary()}});
    if (token_is(~)) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::bitwise_not, unary()}});
    if (token_is(*)) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::dereference, unary()}});
    return postfix();
}

// <postfix> ::= <primary>
//             | <postfix> "{" <initializer-list> "}" <postfix>
//             | <postfix> "->" <postfix>
//             | <postfix> "." <postfix>
//             | <postfix> "(" {<initializer>}* ")" <postfix>
[[nodiscard]] ASTNode* Parser::postfix() {

    auto temp_node = primary();

    if (token_is_str("{")) {
        auto init_list = initializer_list();
        if (temp_node) {
            if_holds(<Identifier>(temp_node.value()), id) {
                id->kind = Identifier::type_name;
            } else {
                report_error(temp_node.value()->source_token, "expected identifier before initializer list.");
            }
            init_list->type.identifier = temp_node.value();
        } else {
            init_list->type.identifier = push({*prev_tkn, Identifier {Identifier::type_name, "Undetermined Type"}});
        }
        expect_token_str("}");
        return init_list;
    }

    if (!temp_node) report_error((*curr_tkn), "expected expression.");

    auto node = temp_node.value();
    Token& tkn = node->source_token;

    vec<ASTNode*> nodes {};
    // NOTE: the current impl means you can't do "(this->that.this).this"
    // (can't use parentheses in a postfix expression)
    while (1) {
        if (token_is(->)) {
            if(!is_branch<Identifier, BlockScope>(node)) {
                report_error(node->source_token, "expected identifier before postfix operator");
            }
            node = push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::dereference, node}});
            nodes.push_back(node);

            expect_token(identifier);
            node = identifier(Identifier::member_var_name);

            continue;
        }
        if (token_is(.)) {
            if(!is_branch<Identifier, BlockScope>(node)) {
                report_error(node->source_token, "expected identifier before postfix operator");
            }
            nodes.push_back(node);

            expect_token(identifier);
            node = identifier(Identifier::member_var_name);
            continue;
        }
        if (token_is_str("(")) {
            if (auto* id = get_if<Identifier>(node)) {
                id->kind = Identifier::member_func_call_name;
            } else if (auto* un = get_if<UnaryExpr>(node)) {
                get<Identifier>(un->expr).kind = Identifier::member_func_call_name;
            } else {
                report_error(tkn, "expected identifier before postfix operator");
            }

            nodes.push_back(node);

            node = argument_list();
            
            expect_token_str(")");
            continue;
        }
        if (!nodes.empty()) {
            nodes.push_back(node);

            auto* first_node = *nodes.begin();
            Identifier* first_id = nullptr;
            if_holds(<Identifier>(first_node), id) first_id = id;
            else if_holds(<UnaryExpr>(first_node), un) first_id = &get<Identifier>(un->expr);
            else {
                report_error(tkn, "expected identifier before postfix operator.");
            }

            if (first_id->kind == Identifier::member_func_call_name) {
                first_id->kind = Identifier::func_call_name;
            } else {
                first_id->kind = Identifier::var_name;
            }
            return push(ASTNode {tkn, PostfixExpr {PostfixExpr::postfix_expr, nodes}});
        }
        return node;
    }
}

[[nodiscard]] ASTNode* Parser::argument_list() {

    if (peek_token_str(")")) return push(ASTNode {*prev_tkn, BlockScope {BlockScope::argument_list, {}}});

    vec<ASTNode*> arguments {};
    arguments.push_back(equality());
    while (1) {
        if (token_is_str(",")) {
            arguments.push_back(equality());
            continue;
        }
        return push(ASTNode {*prev_tkn, BlockScope {BlockScope::argument_list, arguments}});
    }
}

// <initializer-list> ::= <postfix> {","}?
//                      | <postfix> , <initializer-list>
[[nodiscard]] ASTNode* Parser::initializer_list() {
    // TODO: add optional named elements syntax "{.foo = 123123}"

    BlockScope init_list {BlockScope::initializer_list};

    if (peek_token_str("}")) return push(ASTNode {*prev_tkn, std::move(init_list)});

    init_list.nodes.push_back(equality());
    while (1) {
        if (token_is_str(",")) {
            if (peek_token_str("}")) { // allow optional hanging comma
                return push(ASTNode {*prev_tkn, std::move(init_list)});
            }
            init_list.nodes.push_back(equality());
            continue;
        }
        return push(ASTNode {*prev_tkn, std::move(init_list)});
    }
}

// <primary> ::= "(" <expression> ")"
//             | <identifier>
//             | <literal>
[[nodiscard]] Opt<ASTNode*> Parser::primary() {
    if (token_is_str("(")) {
        auto node = equality();
        expect_token_str(")");
        return node;
    }
    if (token_is(int))
        return push(
            ASTNode {*prev_tkn,
                     PrimaryExpr {PrimaryExpr::integer, prev_tkn->literal.value()},
                     Type {push(ASTNode {*prev_tkn, Identifier {Identifier::type_name, "i32"}}), Type::i32_}});
    if (token_is(float))
        return push(
            ASTNode {*prev_tkn,
                     PrimaryExpr {PrimaryExpr::floating_point, prev_tkn->literal.value()},
                     Type {push(ASTNode {*prev_tkn, Identifier {Identifier::type_name, "f64"}}), Type::f64_}});
    if (token_is(string))
        return push(
            ASTNode {*prev_tkn,
                     PrimaryExpr {PrimaryExpr::str, prev_tkn->literal.value()},
                     Type {push(ASTNode {*prev_tkn, Identifier {Identifier::type_name, "i32"}}), Type::i32_}});

    // NOTE: only variable names go through primary() to be found
    if (token_is(identifier)) return identifier(Identifier::var_name);

    return std::nullopt;
}

[[nodiscard]] ASTNode* Parser::identifier(Identifier::Kind id_kind, Opt<ASTNode*> declaration) {
    if (id_kind == Identifier::declaration_name && !declaration) [[unlikely]]
        INTERNAL_PANIC("forgot to provide declaration to identifier.");
    auto token = *prev_tkn;
    Identifier id {id_kind, std::get<str>(prev_tkn->literal.value()), Identifier::unqualified};
    while (token_is(::)) {
        expect_token(identifier);
        id.name += "::" + std::get<str>(prev_tkn->literal.value());
        id.qualifier = Identifier::qualified;
        id.scope_counts++;
    }
    id.mangled_name = id.name;
    return push(ASTNode {token, id,
                         Type {push(ASTNode {*prev_tkn, Identifier {Identifier::type_name, "Undetermined Type"}})}});
}
