#include "parser/parser.hpp"

// <variable-declaration> ::= <declarator-list> {":"}? 
//                            {<declaration-specifier>}+ {"=" <initializer>}?
[[nodiscard]] unique_ptr<ASTNode> Parser::variable_declaration() {
    // TODO: should be identifier list (add later)
    expect_token(identifier);
    auto node = ASTNode {*prev_tkn, NodeKind::variable_declaration};
    Variable variable {.name = std::get<str>(prev_tkn->literal.value())};

    if (token_is(:)) variable.type = declaration_specifier();

    variable.value = token_is(=) ? std::make_optional(initializer()) : std::nullopt;
    node.branch = std::move(variable);

    expect_token(;);
    return node;
}

// <function-declaration> ::=  <declarator> "(" {<parameter-list>}? ")"
//                            "->" {<declaration-specifier>}+ {<pointer>}* 
//                            {<compound-statement>}?
[[nodiscard]] unique_ptr<ASTNode> Parser::function_declaration() {
    // fn cool_func(i32 a, float b) -> const i32* {}
    expect_token(identifier);
    Function function {.name = std::get<str>(prev_tkn->literal.value())};
    Token token = *prev_tkn;

    function.parameters = parameter_list(); // could be an empty vector

    expect_token(->);
    function.type = declaration_specifier();
    if (const auto& kind = function.type.kind;
        kind == TypeKind::_union || kind == TypeKind::_enum || kind == TypeKind::_struct) {
        report_error(token, "type cannot be defined in the result type of a function.");
    }

    if (token_is_str("{")) {
        function.body = compound_statement();
    } else
        expect_token(;);

    return ASTNode {token, NodeKind::function_declaration, std::move(function)};
}

// <parameter> ::= <declarator-specifier> <identifier> 
// <parameter-list> ::= <parameter>
//                    | <parameter-list> "," <parameter> 
[[nodiscard]] Vec<Variable> Parser::parameter_list() {
    expect_token_str("(");
    Vec<Variable> parameters {};

    if (token_is_str(")")) return parameters;
    while (1) {
        expect_token(identifier);
        Variable variable {.name = std::get<str>(prev_tkn->literal.value())};

        expect_token(:);
        variable.type = declaration_specifier();

        if (const auto& kind = variable.type.kind;
            kind == TypeKind::_union || kind == TypeKind::_enum || kind == TypeKind::_struct) {
            report_error((*prev_tkn), "type cannot be defined in a parameter type.");
        }

        parameters.push_back(std::move(variable));

        if (token_is_str(",")) continue;
        if (token_is_str(")")) return parameters;
    }
}

// <compound-statement> ::= { {<declaration>}* {<statement>}* }
[[nodiscard]] unique_ptr<ASTNode> Parser::compound_statement() {
    Vec<ASTNode> nodes {};
    while(!token_is_str("}")) {
        if (token_is_keyword(let)) //
            nodes.push_back(std::move(*variable_declaration()));
        else if (token_is_keyword(fn))
            nodes.push_back(std::move(*function_declaration()));
        else if(token_is_str("{"))
            nodes.push_back(std::move(*compound_statement()));
        else
            nodes.push_back(std::move(*statement()));
    }
    return ASTNode {*prev_tkn, NodeKind::compound_statement, Scope {std::move(nodes)}};
}
// <declaration-specifier> ::= <type-qualifier> | <type-specifier>
// <type-specifier> ::= void | i8 | i32 | i64 | f32 | f64 | str
//                    | <struct-or-union-specifier> | <enum-specifier>
// <type-qualifier> ::= const | volatile| static | extern
[[nodiscard]] Type Parser::declaration_specifier() {
    Type type {};

    // TODO: add extern later
    while (token_is_keyword(const) || token_is_keyword(volatile) 
        || token_is_keyword(static)|| token_is_keyword(extern)) {}
        // we recognize but ignore these keywords atm
    
    if (token_is_keyword(struct)) {
        return Type {.name = "struct" + std::get<str>(prev_tkn->literal.value()),
                     .kind = TypeKind::_struct,
                     .struct_type = struct_declaration()};
    } // add unions later
    if (token_is_keyword(enum)) {
        return Type {.name = "enum" + std::get<str>(prev_tkn->literal.value()),
                     .kind = TypeKind::_enum,
                     .enum_type = enum_declaration()};
    }
    if (token_is(builtin_type)) {
        type.name = std::get<str>(prev_tkn->literal.value());
        type.kind = builtin_type_kind(type.name);
        // NOTE: consider redoing the ptr implementation
        while (token_is(*)) { type.name += "*"; type.ptr_count++; }
        return type;
    }
    if (token_is(identifier)) {
        type.name = std::get<str>(prev_tkn->literal.value());
        type.kind = TypeKind::_undetermined;
        while (token_is(*)) { type.name += "*"; type.ptr_count++; }
        return type;
    }
    report_error((*curr_tkn), "expected type, found '{}'.", curr_tkn->to_str());
}

[[nodiscard]] Struct Parser::struct_declaration() {
    // TODO: finish this 
    return {};
}

[[nodiscard]] Enum Parser::enum_declaration() {
    // TODO: finish this 
    return {};
}
