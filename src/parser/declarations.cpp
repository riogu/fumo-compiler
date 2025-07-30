#include "parser/parser.hpp"

// <variable-declaration> ::= <declarator-list> {":"}?
//                            {<declaration-specifier>}+ {"=" <initializer>}?
[[nodiscard]] ASTNode* Parser::variable_declaration() {
    // TODO: should be identifier list (add later)
    expect_token(identifier);
    auto node = ASTNode {*prev_tkn};

    VariableDecl variable {VariableDecl::variable_declaration, identifier()};

    if (token_is(:)) node.type = declaration_specifier();

    node.branch = std::move(variable);

    if (token_is(=)) {
        ASTNode* assignment = push(ASTNode {*prev_tkn,
                                            BinaryExpr {BinaryExpr::assignment, push(std::move(node)), equality()}});
        expect_token(;);
        return assignment;
    } else if (node.type.name == "Undetermined") {
        report_error((*prev_tkn), "declaring a variable with deduced type requires an initializer.");
    }

    expect_token(;);
    return push(std::move(node));
}

// <function-declaration> ::=  <declarator> "(" {<parameter-list>}? ")"
//                            "->" {<declaration-specifier>}+
//                            {<compound-statement>}?
[[nodiscard]] ASTNode* Parser::function_declaration() {
    expect_token(identifier);
    FunctionDecl function {FunctionDecl::function_declaration, identifier()};
    ASTNode node {*prev_tkn};

    function.parameters = parameter_list(); // could be an empty vector

    expect_token(->);
    Type type = declaration_specifier();
    node.type = type;

    if (token_is_str("{")) function.body = compound_statement();
    else
        expect_token(;);

    node.branch = function;
    return push(node);
}

// <parameter> ::=  <identifier> ":" <declarator-specifier>
// <parameter-list> ::= <parameter>
//                    | <parameter-list> "," <parameter>
[[nodiscard]] vec<ASTNode*> Parser::parameter_list() {
    expect_token_str("(");
    if (token_is_str(")")) return {};

    vec<ASTNode*> parameters {};
    while (1) {
        expect_token(identifier);
        ASTNode node {*prev_tkn, VariableDecl {VariableDecl::parameter, identifier()}};

        expect_token(:);
        node.type = declaration_specifier();

        if (auto& kind = node.type.kind; kind == Type::enum_ || kind == Type::struct_) {
            report_error((*prev_tkn), "type cannot be defined in a parameter type.");
        }

        parameters.push_back(push(std::move(node)));

        if (token_is_str(",")) continue;
        if (token_is_str(")")) return parameters;
    }
}

// <compound-statement> ::= { {<declaration>}* {<statement>}* {<compound-statement}* }
[[nodiscard]] ASTNode* Parser::compound_statement() {
    vec<ASTNode*> nodes {};
    while (!token_is_str("}")) {
        if (token_is_keyword(let)) //
            nodes.push_back(variable_declaration());
        else if (token_is_str("{"))
            nodes.push_back(compound_statement());
        else if (token_is_keyword(fn))
            nodes.push_back(function_declaration());
        else if (token_is_keyword(struct))
            nodes.push_back(struct_declaration());
        else if (token_is_keyword(enum))
            nodes.push_back(enum_declaration());
        else
            nodes.push_back(statement());
    }
    return push(ASTNode {*prev_tkn, BlockScope {BlockScope::compound_statement, nodes}});
}

// <declaration-specifier> ::= {<type-qualifier> | <type-specifier>}+ {<pointer>}*
// <type-specifier> ::= void | i8 | i32 | i64 | f32 | f64 | str
//                    | <struct-or-union-specifier> | <enum-specifier>
// <type-qualifier> ::= const | volatile| static | extern
[[nodiscard]] Type Parser::declaration_specifier() {
    Type type {};

    // TODO: add extern later
    while (token_is_keyword(const) || token_is_keyword(volatile)
           || token_is_keyword(static) || token_is_keyword(extern)) {}
    // we recognize but ignore these keywords atm
    
    if (token_is(builtin_type)) {
        type.name = std::get<str>(prev_tkn->literal.value());
        type.kind = builtin_type_kind(type.name);
        // NOTE: consider redoing the ptr implementation
        while (token_is(*)) type.ptr_count++;
        
        return type;
    }
    if (token_is(identifier)) {
        type.name =  std::get<str>(prev_tkn->literal.value());
        while (token_is(::)) expect_token(identifier), type.name += "::" + std::get<str>(prev_tkn->literal.value());

        type.kind = Type::Undetermined;

        while (token_is(*)) type.ptr_count++;
        
        return type;
    }
    report_error((*curr_tkn), "expected type, found '{}'.", curr_tkn->to_str());
}

[[nodiscard]] ASTNode* Parser::namespace_declaration() {

    expect_token(identifier);
    ASTNode node {*prev_tkn};

    NamespaceDecl nmspace {NamespaceDecl::namespace_declaration, identifier()};

    expect_token_str("{");
    while (!token_is_str("}")) {
        if (token_is_keyword(let)) nmspace.nodes.push_back(variable_declaration());
        else if (token_is_keyword(fn))
            nmspace.nodes.push_back(function_declaration());
        else if (token_is_keyword(namespace))
            nmspace.nodes.push_back(namespace_declaration());
        else if (token_is_keyword(struct))
            nmspace.nodes.push_back(struct_declaration());
        else if (token_is_keyword(enum))
            nmspace.nodes.push_back(enum_declaration());
        else
            report_error((*curr_tkn), "expected namespace member declaration.");
    }
    node.branch = nmspace;
    return push(node);
}

[[nodiscard]] ASTNode* Parser::struct_declaration() {
    ASTNode node {*prev_tkn};
    expect_token(identifier);
    TypeDecl type_decl {TypeDecl::struct_declaration, identifier()};

    if (token_is_str("{")) {
        vec<ASTNode*> nodes {};
        while (!token_is_str("}")) {
            if (token_is_keyword(let)) nodes.push_back(variable_declaration());
            else if (token_is_keyword(fn))
                nodes.push_back(function_declaration());
            else if (token_is_keyword(struct))
                nodes.push_back(struct_declaration());
            else if (token_is_keyword(enum))
                nodes.push_back(enum_declaration());
            else
                report_error((*curr_tkn), "expected struct member declaration.");
        }
        type_decl.definition_body = std::move(nodes);
    }

    node.branch = type_decl;
    expect_token(;);
    return push(node);
}

[[nodiscard]] ASTNode* Parser::enum_declaration() {
    INTERNAL_PANIC("enums are not implemented.");
}
