#include "parser/parser.hpp"

// <declaration-specifier> ::= <type-qualifier> | <type-specifier>
// <type-specifier> ::= void | i8 | i32 | i64 | f32 | f64 | str
//                    | <struct-or-union-specifier> | <enum-specifier>
// <type-qualifier> ::= const | volatile| static | extern
[[nodiscard]] Type Parser::declaration_specifier() {
    Type type {};
    while (token_is_keyword(const) || token_is_keyword(volatile) 
            || token_is_keyword(static) || token_is_keyword(extern)) {
        // we recognize but ignore these keywords atm
        // TODO: add static and extern later
    }
    if (token_is(builtin_type)) {
        type.name = std::get<str>(prev_tkn->literal.value());
        type.kind = builtin_type_kind(type.name);
        while (token_is(*)) { // NOTE: consider redoing the ptr implementation
            type.name += "*";
            type.ptr_count++;
        }
        return type;
    }
    if (token_is_keyword(struct)) {
        return Type {.name = "struct" + std::get<str>(prev_tkn->literal.value()),
                     .kind = TypeKind::_struct,
                     .struct_type = struct_declaration()};
    }
    if (token_is_keyword(enum)) {
        return Type {.name = "enum" + std::get<str>(prev_tkn->literal.value()),
                     .kind = TypeKind::_enum,
                     .enum_type = enum_declaration()};
    }
    if (token_is(identifier)) {
        return Type {std::get<str>(prev_tkn->literal.value()), TypeKind::_undetermined};
    }
    report_error(curr_tkn, "expected type, found '{}'.", curr_tkn->to_str());
}

// <function-declaration> ::=  <declarator> "(" {<parameter-list>}? ")"
//                            "->" {<declaration-specifier>}+ {<pointer>}* 
//                            {<compound-statement>}?
[[nodiscard]] unique_ptr<ASTNode> Parser::function_declaration() {
    // fn cool_func(i32 a, float b) -> const i32* {}
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

    return ASTNode {token, NodeKind::function_declaration, std::move(function)};
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
