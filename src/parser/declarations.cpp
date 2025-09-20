#include "parser/parser.hpp"

// <variable-declaration> ::= <declarator-list> {":"}?
//                            {<declaration-specifier>}+ {"=" <initializer>}?
[[nodiscard]] ASTNode* Parser::variable_declaration(bool optional_comma) {
    // TODO: should allow declarating multiple identifiers with separating commas (add later)


    bool was_extern = false;
    if (token_is_keyword(extern)) {
        was_extern = true;
    }
    auto* node = push(ASTNode {*curr_tkn});

    VariableDecl variable {VariableDecl::variable_declaration, unqualified_identifier(Identifier::declaration_name)};

    if (const auto& id = get_id(variable); id.qualifier == Identifier::qualified) {
        report_error(variable.identifier->source_token, 
                     "variable declaration '{}' can't be qualified.", id.mangled_name);
    }

    if (token_is(:)) node->type = declaration_specifier();
    if (was_extern) {
        node->type.qualifiers.insert(Type::extern_);
    }
    if (node->type.kind == Type::void_) {
        if (node->type.ptr_count) {
            report_error((*curr_tkn), "variable cannot have type '{}'. NOTE: did you mean to use 'any*'?", type_name(node->type));
        }
        report_error((*curr_tkn), "variable cannot have type 'void'. NOTE: only used in function return types.");
    }

    node->branch = std::move(variable);

    if (token_is(=)) {
        ASTNode* assignment = push(ASTNode {*prev_tkn, BinaryExpr {BinaryExpr::assignment, node, initializer()}});
        if (optional_comma) token_is(;);
        else expect_token(;);
        return assignment;
    } else if (get_name(node->type) == "Undetermined Type") {
        report_error((*prev_tkn), "declaring a variable with deduced type requires an initializer.");
    }

    if (optional_comma) token_is(;);
    else expect_token(;);

    return node;
}

// <function-declaration> ::=  <declarator> "(" {<parameter-list>}? ")"
//                            "->" {<declaration-specifier>}+
//                            {<compound-statement>}?
[[nodiscard]] ASTNode* Parser::function_declaration() {

    // while (token_is_keyword(const) || token_is_keyword(volatile)
    //        || token_is_keyword(static) || token_is_keyword(extern)) {
    // }
    std::unordered_set<Type::TypeQualifier> qualifiers {};
    if (token_is_keyword(static)) qualifiers.insert(Type::static_); // adding static functions

    auto* node = push(ASTNode {*curr_tkn});

    FunctionDecl function {FunctionDecl::function_declaration, declaration_identifier()};

    const auto& [params, is_variadic] = parameter_list(); // could be an empty vector
    function.parameters = params;
    function.is_variadic = is_variadic;

    expect_token(->);
    Type type = declaration_specifier();
    type.qualifiers.insert(qualifiers.begin(), qualifiers.end()); // add static qualifier to type
    node->type = type;

    if (token_is_str("{")) function.body = compound_statement();
    else
        expect_token(;);

    node->branch = function;
    return node;
}

// <parameter> ::=  <identifier> ":" <declarator-specifier>
// <parameter-list> ::= <parameter>
//                    | <parameter-list> "," <parameter>
[[nodiscard]] std::pair<vec<ASTNode*>, bool> Parser::parameter_list() {
    expect_token_str("(");
    if (token_is_str(")")) return {{}, false};

    vec<ASTNode*> parameters {};
    while (1) {
        if(peek_token(identifier)) {
            ASTNode* node = push(ASTNode {*prev_tkn});
            node->branch = VariableDecl {VariableDecl::parameter, unqualified_identifier(Identifier::declaration_name)};

            expect_token(:);
            node->type = declaration_specifier();
            if (node->type.kind == Type::void_) {
                if (node->type.ptr_count) {
                    report_error((*curr_tkn), "parameter cannot have type '{}'. NOTE: did you mean to use 'any*'?", type_name(node->type));
                }
                report_error((*curr_tkn), "parameter cannot have type 'void'. NOTE: only used in function return types.");
            }
            parameters.push_back(std::move(node));
        } else if (token_is(...)) {
            expect_token_str(")");
            return {parameters, true}; // is variadic 
        } else {
            report_error((*curr_tkn), "expected identifier. NOTE: forward declarations need parameter names.");
        }

        if (token_is_str(",")) continue;
        if (token_is_str(")")) return {parameters, false};
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
        else if (token_is(;)) {}
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
    // while (token_is_keyword(const) || token_is_keyword(static)) {}
    // we recognize but ignore these keywords atm
    if (token_is_keyword(const)) {
    }
    
    if (token_is(builtin_type)) {
        type.identifier = push(ASTNode {*prev_tkn, 
                                        Identifier {Identifier::type_name,
                                                    std::get<str>(prev_tkn->literal.value())}});
        type.kind = builtin_type_kind(get_name(type));
        while (token_is(*)) type.ptr_count++; // NOTE: consider redoing the ptr implementation
        if (type.kind == Type::any_ && !type.ptr_count) {
            report_error((*curr_tkn), "type 'any' must be a pointer. NOTE: did you mean 'any*'?");
        }
        return type;
    }
    if (peek_token(identifier)) {
        type.identifier = identifier(Identifier::type_name);
        type.kind = Type::Undetermined;
        while (token_is(*)) {
            type.ptr_count++;
            // std::println("found pointer for '{}'", get_id(type).name);
        }
        return type;
    }
    report_error((*curr_tkn), "expected type, found '{}'.", curr_tkn->to_str());
}

[[nodiscard]] ASTNode* Parser::namespace_declaration() {


    ASTNode* node = push(ASTNode {*curr_tkn});
    NamespaceDecl nmspace {NamespaceDecl::namespace_declaration, qualified_identifier()};

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
        else if (token_is(;)) {}
        else
            report_error((*curr_tkn), "expected namespace member declaration.");
    }
    node->branch = nmspace;
    return node;
}

[[nodiscard]] ASTNode* Parser::struct_declaration() {

    auto* node = push(ASTNode {*curr_tkn});
    node->type = {declaration_identifier(), Type::struct_};

    TypeDecl type_decl {TypeDecl::struct_declaration};
    if (token_is_str("{")) {
        int member_index = 0;
        vec<ASTNode*> nodes {};
        while (!token_is_str("}")) {
            if (token_is_keyword(let)) {
                nodes.push_back(variable_declaration());
                if (auto* var_decl = get_if<VariableDecl>(nodes.back())) {
                    var_decl->member_index = member_index++;
                } else if (auto* assign = get_if<BinaryExpr>(nodes.back())) {
                    get<VariableDecl>(assign->lhs).member_index = member_index++;
                }

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
            else if (token_is(;)) {}
            else
                report_error((*curr_tkn), "expected struct member declaration.");
        }
        type_decl.definition_body = std::move(nodes);
    }

    node->branch = type_decl;
    return node;
}

[[nodiscard]] ASTNode* Parser::enum_declaration() {
    internal_panic("enums are not implemented.");
}
