#include "parser/parser.hpp"

// <variable-declaration> ::= <declarator-list> {":"}?
//                            {<declaration-specifier>}+ {"=" <initializer>}?
[[nodiscard]] ASTNode* Parser::variable_declaration() {
    // TODO: should allow declarating multiple identifiers with separating commas (add later)
    expect_token(identifier);
    auto* node = push(ASTNode {*prev_tkn});

    VariableDecl variable {VariableDecl::variable_declaration, identifier(Identifier::declaration_name, node)};

    if (const auto& id = get_id(variable); id.qualifier == Identifier::qualified) {
        report_error(variable.identifier->source_token, 
                     "variable declaration '{}' can't be qualified.", id.mangled_name);
    }

    if (token_is(:)) {
        node->type = declaration_specifier();
    } else
        node->type.identifier = push(ASTNode {*prev_tkn, Identifier {Identifier::type_name, "Undetermined Type"}});

    node->branch = std::move(variable);

    if (token_is(=)) {
        ASTNode* assignment = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::assignment, node, equality()}});
        expect_token(;);
        return assignment;
    } else if (get_name(node->type) == "Undetermined Type") {
        report_error((*prev_tkn), "declaring a variable with deduced type requires an initializer.");
    }

    expect_token(;);
    return node;
}

// <function-declaration> ::=  <declarator> "(" {<parameter-list>}? ")"
//                            "->" {<declaration-specifier>}+
//                            {<compound-statement>}?
[[nodiscard]] ASTNode* Parser::function_declaration() {
    expect_token(identifier);
    auto* node = push(ASTNode {*prev_tkn});

    FunctionDecl function {FunctionDecl::function_declaration, identifier(Identifier::declaration_name, node)};

    function.parameters = parameter_list(); // could be an empty vector

    expect_token(->);
    Type type = declaration_specifier();
    node->type = type;

    if (is_tkn(str_to_tkn_type("{"))) function.body = compound_statement();
    else
        expect_token(;);

    node->branch = function;
    return node;
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
        ASTNode* node = push(ASTNode {*prev_tkn});
        node->branch = VariableDecl {VariableDecl::parameter, identifier(Identifier::declaration_name, node)};

        expect_token(:);
        node->type = declaration_specifier();


        parameters.push_back(std::move(node));

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
        type.identifier = push(ASTNode {*prev_tkn, 
                                        Identifier {Identifier::type_name,
                                                    std::get<str>(prev_tkn->literal.value())}});
        type.kind = builtin_type_kind(get_name(type));
        while (token_is(*)) type.ptr_count++; // NOTE: consider redoing the ptr implementation
        
        return type;
    }
    if (token_is(identifier)) {
        type.identifier = identifier(Identifier::type_name);
        type.kind = Type::Undetermined;

        while (token_is(*)) type.ptr_count++;
        
        return type;
    }
    report_error((*curr_tkn), "expected type, found '{}'.", curr_tkn->to_str());
}

[[nodiscard]] ASTNode* Parser::namespace_declaration() {

    expect_token(identifier);

    ASTNode* node = push(ASTNode {*prev_tkn});
    NamespaceDecl nmspace {NamespaceDecl::namespace_declaration, identifier(Identifier::declaration_name, node)};

    expect_token_str("{");
    while (!token_is_str("}")) {
        if (token_is_keyword(let)) 
            nmspace.nodes.push_back(variable_declaration());
        else if (token_is_keyword(namespace))
            nmspace.nodes.push_back(namespace_declaration());
        else if (token_is_keyword(fn))
            nmspace.nodes.push_back(function_declaration());
        else if (token_is_keyword(struct))
            nmspace.nodes.push_back(struct_declaration());
        else if (token_is_keyword(enum))
            nmspace.nodes.push_back(enum_declaration());
        else
            report_error((*curr_tkn), "expected namespace member declaration.");
    }
    node->branch = nmspace;
    return node;
}

[[nodiscard]] ASTNode* Parser::struct_declaration() {
    auto* node = push(ASTNode {*prev_tkn});

    expect_token(identifier);
    node->type = {identifier(Identifier::declaration_name, node), Type::struct_};

    TypeDecl type_decl {TypeDecl::struct_declaration};
    if (token_is_str("{")) {
        vec<ASTNode*> nodes {};
        while (!token_is_str("}")) {
            if (token_is_keyword(let)) {
                nodes.push_back(variable_declaration());
            } else if (token_is_keyword(fn)) {
                nodes.push_back(function_declaration());
                auto& function = get<FunctionDecl>(nodes.back());
                function.kind = FunctionDecl::member_func_declaration;
                if (const auto& id = get_id(function); id.qualifier == Identifier::qualified) {
                    report_error(function.identifier->source_token, 
                                 "member function declaration '{}' can't be qualified.", id.mangled_name);
                }

            } else if (token_is_keyword(struct))
                nodes.push_back(struct_declaration());
            else if (token_is_keyword(enum))
                nodes.push_back(enum_declaration());
            else
                report_error((*curr_tkn), "expected struct member declaration.");
        }
        type_decl.definition_body = std::move(nodes);
    }

    node->branch = type_decl;
    expect_token(;);
    return node;
}

[[nodiscard]] ASTNode* Parser::enum_declaration() {
    INTERNAL_PANIC("enums are not implemented.");
}
