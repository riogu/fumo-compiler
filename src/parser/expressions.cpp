#include "base_definitions/ast_node.hpp"
#include "parser/parser.hpp"
#include "utils/common_utils.hpp"

ASTNode* Parser::parse_tokens(vec<Token>& tkns) {
    tokens = tkns;
    prev_tkn = tkns.begin();
    curr_tkn = tkns.begin();

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
        else if (token_is(;)) {}
        else
            report_error((*curr_tkn), "expected declaration.");
        // AST.push_back(statement()); /* NOTE: no longer valid in global, only for testing parsing */
    }
    auto id = push(ASTNode {*tkns.begin(), Identifier {Identifier::declaration_name, "fumo_module"}});
    return push(ASTNode {*tkns.begin(), NamespaceDecl {NamespaceDecl::translation_unit, id, std::move(AST)}});
}

// <statement> ::= <return-statement>
//               | <if-statement>
//               | <expression-statement>
[[nodiscard]] ASTNode* Parser::statement() {
    if (token_is_keyword(return)) {
        if (token_is(;)) 
            return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::return_statement, std::nullopt}});
        else {
            auto* node = initializer();
            expect_token(;);
            return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::return_statement, node}});
        }
    }
    if (token_is_keyword(if)) return if_statement(IfStmt::if_statement);
    if (token_is_keyword(while)) return while_loop();

    return expression_statement();
}

[[nodiscard]] ASTNode* Parser::while_loop() {
    WhileStmt while_loop {};
    Token tkn = *prev_tkn;
    while_loop.condition = if_value(logical())
                           else_error((*curr_tkn), "expected condition in while loop");

    expect_token_str("{");
    while_loop.body = compound_statement();

    return push(ASTNode {tkn, while_loop});
}
// <if-statement> ::= "if" { "(" }? <expression> { ")" }? <compound-statement> 
//                    { "else" (<compound-statement> | <if-statement>) }?
[[nodiscard]] ASTNode* Parser::if_statement(IfStmt::Kind kind) { 
    IfStmt ifstmt {kind}; // maybe make it into an expression later (not a statement)
    Token tkn = *prev_tkn;

    if (token_is_keyword(let)) {
        report_error((*prev_tkn), "variable declarations in if conditions must be sorrounded by parentheses.");
    }
    if (peek_token_str("(") && peek_keyword_amount("let", 1)) {
        // here to enforce parentheses around variable declarations in the condition
        expect_token_str("("); // consume the parentheses
        token_is_keyword(let);
        ifstmt.condition = variable_declaration(true);
        expect_token_str(")");
    } else
        ifstmt.condition = if_value(logical())
                           else_error((*curr_tkn), "expected condition in if statement");

    expect_token_str("{");
    ifstmt.body = compound_statement();

    ifstmt.else_stmt = else_statement();

    return push(ASTNode {tkn, ifstmt});
}

[[nodiscard]] Opt<ASTNode*> Parser::else_statement() { 
    IfStmt ifstmt {IfStmt::else_statement}; // maybe make it into an expression later (not a statement)
    Token tkn = *prev_tkn;

    if (token_is_keyword(else)) {
        if (token_is_keyword(if)) return if_statement(IfStmt::else_if_statement);
    } else {
        return std::nullopt;
    }
    expect_token_str("{");
    ifstmt.body = compound_statement();
    ifstmt.else_stmt = else_statement();

    return push(ASTNode {tkn, ifstmt});
}

// <expression-statement> = <expression> ";"
[[nodiscard]] ASTNode* Parser::expression_statement() {
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
    // NOTE: assignments as rvalues have not been tested
    auto node = initializer(); // dont need the optionality above this function
    if (!node) report_error((*curr_tkn), "expected expression.");

    if (token_is(=)) {
        if (!is_branch<Identifier, PostfixExpr, UnaryExpr>(node)) {
            report_error(node->source_token, "expression is not assignable.");
        }
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::assignment, node, initializer()}});
    }
    if (token_is(+=)) {
        if (!is_branch<Identifier, PostfixExpr, UnaryExpr>(node)) {
            report_error(node->source_token, "expression is not assignable.");
        }
        auto* temp = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::add, node, initializer()}});
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::assignment, node, temp}});
    }
    if (token_is(-=)) {
        if (!is_branch<Identifier, PostfixExpr, UnaryExpr>(node)) {
            report_error(node->source_token, "expression is not assignable.");
        }
        auto* temp = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::sub, node, initializer()}});
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::assignment, node, temp}});
    }
    if (token_is(*=)) {
        if (!is_branch<Identifier, PostfixExpr, UnaryExpr>(node)) {
            report_error(node->source_token, "expression is not assignable.");
        }
        auto* temp = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::multiply, node, initializer()}});
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::assignment, node, temp}});
    }
    if (token_is(/=)) {
        if (!is_branch<Identifier, PostfixExpr, UnaryExpr>(node)) {
            report_error(node->source_token, "expression is not assignable.");
        }
        auto* temp = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::divide, node, initializer()}});
        return push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::assignment, node, temp}});
    }
    return node;
}

[[nodiscard]] ASTNode* Parser::initializer() {
    auto temp_node = logical();

    if (token_is_str("{")) {
        auto init_list = initializer_list();
        if (temp_node) {
            if (auto* id = get_if<Identifier>(temp_node.value())) {
                if (id->is_generic_wrapper()) {
                    id->kind = Identifier::generic_wrapper_type_name;
                } else {
                    id->kind = Identifier::type_name;
                    // NOTE: we must promote this to generic_type_name if it turns out to be
                    // for example, 'T' taken from the current generic context
                }
            } else {
                report_error(temp_node.value()->source_token, "expected identifier before initializer list.");
            }
            init_list->type.identifier = temp_node.value();
        }
        expect_token_str("}");
        return init_list;
    }
    return if_value(temp_node) else_error((*curr_tkn), "expected expression.");
}
// <initializer-list> ::= <postfix> {","}?
//                      | <postfix> , <initializer-list>
[[nodiscard]] ASTNode* Parser::initializer_list() {
    // TODO: add optional named elements syntax "{.foo = 123123}"

    BlockScope init_list {BlockScope::initializer_list};
    Token start_tkn = *prev_tkn;

    if (peek_token_str("}")) return push(ASTNode {start_tkn, std::move(init_list)});

    init_list.nodes.push_back(expression());
    while (1) {
        if (token_is_str(",")) {
            if (peek_token_str("}")) { // allow optional hanging comma
                return push(ASTNode {start_tkn, std::move(init_list)});
            }
            init_list.nodes.push_back(expression());
            continue;
        }
        return push(ASTNode {start_tkn, std::move(init_list)});
    }
}
// <logical> ::= <relational> { ("<" | ">" | "<=" | ">=")  <relational> }*
[[nodiscard]] Opt<ASTNode*> Parser::logical() {
    auto node = equality().value_or(nullptr);
    if (!node) return std::nullopt;

    Token temp_tkn = *curr_tkn;
    while (1) {
        if (token_is(&&)) {
            auto temp = if_value(equality()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::logical_and, node, temp}});
            continue;
        }
        if (token_is(||)) {
            auto temp = if_value(equality()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::logical_or, node, temp}});
            continue;
        }
        return node;
    }
}

// <equality> ::= <relational> {("==" | "!=") <relational>}*
[[nodiscard]] Opt<ASTNode*> Parser::equality() {
    auto node = relational().value_or(nullptr);
    if (!node) return std::nullopt; // due to initializer list optional type, propagate nullopt
    Token temp_tkn = *curr_tkn;
    while (1) { 
        if (token_is(==)) {
            auto temp = if_value(relational()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::equal, node, temp}});
            continue;
        }
        if (token_is(!=)) {
            auto temp = if_value(relational()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::not_equal, node, temp}});
            continue;
        }
        return node;
    }
}

// <relational> ::= <add> { ("<" | ">" | "<=" | ">=")  <add> }*
[[nodiscard]] Opt<ASTNode*> Parser::relational() {
    auto node = add().value_or(nullptr);
    if (!node) return std::nullopt;

    Token temp_tkn = *curr_tkn;
    while (1) {
        if (token_is(<)) {
            auto temp = if_value(add()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::less_than, node, temp}});
            continue;
        }
        if (token_is(>)) {
            auto temp = if_value(add()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::less_than, temp, node}});
            continue;
        }
        if (token_is(<=)) {
            auto temp = if_value(add()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::less_equals, node, temp}});
            continue;
        }
        if (token_is(>=)) {
            auto temp = if_value(add()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::less_equals, temp, node}});
            continue;
        }
        return node;
    }
}

// <add> ::=  <multiplication> { "+" <multiplication> | "-" <multiplication> }*
[[nodiscard]] Opt<ASTNode*> Parser::add() {
    auto node = multiply().value_or(nullptr);
    if (!node) return std::nullopt; 

    Token temp_tkn = *curr_tkn;
    while (1) {
        if (token_is(+)) {
            auto temp = if_value(multiply()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::add, node, temp}});
            continue;
        }
        if (token_is(-)) {
            auto temp = if_value(multiply()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::sub, node, temp}});
            continue;
        }
        return node;
    }
}

// <multiplication> ::=  <unary> { ("*" | "/") <unary> }*
[[nodiscard]] Opt<ASTNode*> Parser::multiply() {
    auto node = unary().value_or(nullptr);
    if (!node) return std::nullopt;

    Token temp_tkn = *curr_tkn;
    while (1) {
        if (token_is(*)) {
            auto temp = if_value(unary()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::multiply, node, temp}});
            continue;
        }
        if (token_is(/)) {
            auto temp = if_value(unary()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::divide, node, temp}});
            continue;
        }
        if (token_is(%)) {
            auto temp = if_value(unary()) else_error((*curr_tkn), "expected expression.");
            node = push(ASTNode {temp_tkn, BinaryExpr {BinaryExpr::divide, node, temp}});
            continue;
        }
        return node;
    }
}

// <unary> ::= ("-" | "!" | "~" | "+" | "*") <unary>
//           | <postfix>
[[nodiscard]] Opt<ASTNode*> Parser::unary() {
    if (token_is(-)) {
        if (auto temp = unary()) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::negate, temp.value()}});
        return std::nullopt;
    }
    if (token_is(+)) {
        if (auto temp = unary()) return temp.value();
        return std::nullopt;
    }
    if (token_is(!)) {
        if (auto temp = unary()) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::logic_not, temp.value()}});
        return std::nullopt;
    }
    if (token_is(~)) {
        if (auto temp = unary()) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::bitwise_not, temp.value()}});
        return std::nullopt;
    }
    if (token_is(*)) {
        if (auto temp = unary()) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::dereference, temp.value()}});
        return std::nullopt;
    }
    if (token_is(&)) {
        if (auto temp = unary()) return push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::address_of, temp.value()}});
        return std::nullopt;
    }
    return postfix();
}

// <postfix> ::= <primary>
//             | <identifier> "{" <initializer-list> "}" 
//             | <postfix> "->" <postfix>
//             | <postfix> "." <postfix>
//             | <postfix> "(" {<argument-list>}* ")" <postfix>
[[nodiscard]] Opt<ASTNode*> Parser::postfix() {

    auto temp_node = primary();
    if (!temp_node) return std::nullopt;
    auto node = temp_node.value();
    Token& tkn = node->source_token;

    vec<ASTNode*> nodes {};
    // NOTE: the current impl means you can't do "(this->that.this).this"
    // (can't use parentheses in a postfix expression)
    // just change this to not check for specific is_branch, and do some testing to see if it works
    // it should just work, but there might be a few edgecases (assuming you refactored semantic analysis)
    while (1) {
        if (token_is(->)) {
            // NOTE: this if should be removed later. we should allow anything to have a postfix
            // it will later be checked in other ways
            if(!is_branch<Identifier, FunctionCall>(node)) {
                report_error(node->source_token, "expected identifier before postfix operator.");
            }
            node = push(ASTNode {*prev_tkn, UnaryExpr {UnaryExpr::dereference, node}});
            nodes.push_back(node);

            node = identifier(Identifier::member_var_name);

            continue;
        }
        if (token_is(.)) {
            if(!is_branch<Identifier, FunctionCall>(node)) {
                report_error(node->source_token, "expected identifier before postfix operator.");
            }
            nodes.push_back(node);

            node = identifier(Identifier::member_var_name);
            continue;
        }
        if (token_is_str("(")) {
            FunctionCall func_call {};
            if (auto* id = get_if<Identifier>(node)) {
                id->kind = Identifier::member_func_call_name;
                func_call.kind = FunctionCall::member_function_call;
                func_call.identifier = node;
            } 
            else if (auto* un = get_if<UnaryExpr>(node)) {
                get<Identifier>(un->expr.value()).kind = Identifier::member_func_call_name;
                func_call.kind = FunctionCall::member_function_call;
                func_call.identifier = un->expr.value();
            } else {
                report_error(tkn, "expected function name before '()' operator.");
            }
            func_call.argument_list = argument_list();
            expect_token_str(")");

            node = push(ASTNode {*prev_tkn, func_call, node->type});
            // NOTE: we dont push_back(), only in the next iteration or at the end in the below if statement
            continue;
        }

        if (!nodes.empty()) {
            nodes.push_back(node);
            return push(ASTNode {tkn, PostfixExpr {PostfixExpr::postfix_expr, nodes}});
        }
        // NOTE: we dont know for sure if its not a member variable (implicit this pointer)
        // so we reset it to search later
        if (auto* func_call = get_if<FunctionCall>(node)) {
            get<Identifier>(func_call->identifier).kind = Identifier::func_call_name;
            func_call->kind = FunctionCall::function_call;
        }
        // normal function call by itself is also returned here if found
        return node;
    }
}

[[nodiscard]] vec<ASTNode*> Parser::argument_list() {

    if (peek_token_str(")")) return {};

    vec<ASTNode*> arguments {};
    arguments.push_back(expression());
    while (1) {
        if (token_is_str(",")) {
            arguments.push_back(expression());
            continue;
        }
        return arguments;
    }
}


// <primary> ::= "(" <expression> ")"
//             | <identifier>
//             | <literal>
[[nodiscard]] Opt<ASTNode*> Parser::primary() {
    if (token_is_str("(")) {
        auto node = expression();
        expect_token_str(")");
        return node;
    }
    if (token_is(int))
        return push(ASTNode {*prev_tkn,
                             PrimaryExpr {PrimaryExpr::integer, prev_tkn->literal.value()},
                             make_type(Type::i32_, "i32", 0)});
    if (token_is(float))
        return push(ASTNode {*prev_tkn,
                             PrimaryExpr {PrimaryExpr::floating_point, prev_tkn->literal.value()},
                             make_type(Type::f64_, "f64", 0)});
    if (token_is(string))
        return push(ASTNode {*prev_tkn,
                             PrimaryExpr {PrimaryExpr::str, prev_tkn->literal.value()},
                             make_type(Type::char_, "char", 1)});
    if (token_is(char_literal)) {
        return push(ASTNode {*prev_tkn,
                             PrimaryExpr {PrimaryExpr::char_literal, prev_tkn->literal.value()},
                             make_type(Type::char_, "char", 0)});
    }

    if (token_is_keyword(true))
        return push(ASTNode {*prev_tkn, PrimaryExpr {PrimaryExpr::bool_, true}, make_type(Type::bool_, "bool", 0)});
    if (token_is_keyword(false))
        return push(ASTNode {*prev_tkn, PrimaryExpr {PrimaryExpr::bool_, false}, make_type(Type::bool_, "bool", 0)});
    if (token_is_keyword(null))
        return push(ASTNode {*prev_tkn, PrimaryExpr {PrimaryExpr::void_, true}, make_type(Type::any_, "null", 1)});

    // NOTE: only variable names 
    if (peek_token(identifier)) return identifier(Identifier::var_name);

    return std::nullopt;
}

// FIXME: dont allow using the same name for 2 separate parameters on generics OR normal function calls
// it should give the user an error

// TODO: next add 
// fn "foo::bar::[{Node[{}]}]::thing::[{}]()"     { ... }
// fn  foo::bar::[i32]::thing::[f64]()    { ... }

// struct "foo::bar::[{}, {}, {}]()"      { ... }
// struct  foo::bar::[i32, f64, char*]()  { ... }
[[nodiscard]] ASTNode* Parser::identifier(Identifier::Kind id_kind) {
    if (id_kind == Identifier::generic_type_name) {
        if (!token_is(identifier) && !token_is(builtin_type)) {
            report_error((*curr_tkn), "expected identifier in generic declaration.");
        }
    } else {
        expect_token(identifier);
    }
    auto token = *prev_tkn;
    Identifier id {id_kind, std::get<str>(prev_tkn->literal.value()), Identifier::unqualified};


    if (peek_token([)) id.kind = Identifier::generic_wrapper_type_name;
    parse_generic_parameters(id);

    while (token_is(::)) {
        if (token_is(identifier)) {
            id.name += "::" + std::get<str>(prev_tkn->literal.value());
            id.qualifier = Identifier::qualified;
            id.scope_counts++;
            if (peek_token([)) id.kind = Identifier::generic_wrapper_type_name;
            parse_generic_parameters(id);
        } else {
            report_error((*curr_tkn), "expected identifier after '::'.");
        }
        
    }
    // let var = foo::bar::[Vec::[i32]](x, y);
    id.mangled_name = id.name;
    if (id.is_generic_wrapper() || id.kind == Identifier::generic_type_name) {
        auto* node = push(ASTNode {token, id});
        node->type.identifier = node;
        node->type.kind = Type::Generic;
        return node;
    }
    return push(ASTNode {token, id});
}
// fn  foo::bar[T]::thing[U]()    { ... }
// fn "foo::bar[{}]::thing[{}]()" { ... }
// struct  foo::bar[T, U, V]()      { ... }
// struct "foo::bar[{}]()"          { ... }
// the idea is that each identifer is solved normally for 'T', and then filled later
// we need to make a full copy of the function's AST, and then fill that
// a true copy with no pointer still belonging to the AST's generic declaration
[[nodiscard]] ASTNode* Parser::declaration_identifier() {
    expect_token(identifier);
    auto token = *prev_tkn;
    Identifier id {Identifier::declaration_name, std::get<str>(prev_tkn->literal.value()), Identifier::unqualified};

    if (token_is([)) {
        id.name += "[{}"; // for filling later with formatted arguments
        id.generic_identifiers.push_back(unqualified_identifier(Identifier::generic_type_name));
        while (1) {
            if (token_is_str(",")) {
                id.generic_identifiers.push_back(unqualified_identifier(Identifier::generic_type_name));
                id.name += ", {}"; // will be filled with the real type when instantiated.
                continue;
            }
            break;
        }
        expect_token(]);
        id.name += "]";
    }
    while (token_is(::)) {
        if (token_is(identifier)) {
            id.name += "::" + std::get<str>(prev_tkn->literal.value());
            id.qualifier = Identifier::qualified;
            id.scope_counts++;
            if (token_is([)) {
                id.name += "[{}"; // for filling later with formatted arguments
                id.generic_identifiers.push_back(unqualified_identifier(Identifier::generic_type_name));
                while (1) {
                    if (token_is_str(",")) {
                        id.generic_identifiers.push_back(unqualified_identifier(Identifier::generic_type_name));
                        id.name += ", {}"; // will be filled with the real type when instantiated.
                        continue;
                    }
                    break;
                }
                expect_token(]);
                id.name += "]";
            }
        } else {
            report_error((*curr_tkn), "expected identifier after '::'.");
        }
    }
    id.mangled_name = id.name;
    
    return push(ASTNode {token, id});
}

[[nodiscard]] ASTNode* Parser::unqualified_identifier(Identifier::Kind id_kind) {
    if (id_kind == Identifier::generic_type_name) {
        if (!token_is(identifier) && !token_is(builtin_type)) {
            report_error((*curr_tkn), "expected identifier in generic declaration.");
        }
    } else {
        expect_token(identifier);
    }
    auto token = *prev_tkn;
    Identifier id {id_kind, std::get<str>(prev_tkn->literal.value()), Identifier::unqualified};
    id.mangled_name = id.name;
    if (id.is_generic_wrapper() || id.kind == Identifier::generic_type_name) {
        auto* node = push(ASTNode {token, id});
        node->type.identifier = node;
        node->type.kind = Type::Generic;
        return node;
    }
    return push(ASTNode {token, id});
}

[[nodiscard]] ASTNode* Parser::qualified_identifier() {
    expect_token(identifier);
    auto token = *prev_tkn;
    Identifier id {Identifier::declaration_name, std::get<str>(prev_tkn->literal.value()), Identifier::unqualified};
    while (token_is(::)) {
        expect_token(identifier);
        id.name += "::" + std::get<str>(prev_tkn->literal.value());
        id.qualifier = Identifier::qualified;
        id.scope_counts++;
    }
    id.mangled_name = id.name;
    return push(ASTNode {token, id});
}
void Parser::parse_generic_parameters(Identifier& id) {
    if (token_is([)) {
        id.name += "[{}"; // for filling later with formatted arguments
        id.generic_identifiers.push_back(identifier(Identifier::generic_type_name));
        while (1) {
            if (token_is_str(",")) {
                id.generic_identifiers.push_back(identifier(Identifier::generic_type_name));
                id.name += ", {}"; // will be filled with the real type when instantiated.
                continue;
            }
            break;
        }
        expect_token(]);
        id.name += "]";
    }
}
