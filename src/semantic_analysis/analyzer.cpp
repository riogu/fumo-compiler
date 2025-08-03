#include "semantic_analysis/analyzer.hpp"
#include "base_definitions/tokens.hpp"

void Analyzer::semantic_analysis(ASTNode* file_root_node) {

    symbol_tree.push_scope("", ScopeKind::Namespace);

    // NOTE: this is here so the global namespace is unnamed
    // in reality, everything global starts with "::"
    // removing this changes nothing in functionality, only in debug printing
    symbol_tree.curr_scope_name = "";
    symbol_tree.scope_stack.begin()->isolated_name = "";
    symbol_tree.scope_stack.begin()->name = symbol_tree.curr_scope_name;

    for (auto& node : get<NamespaceDecl>(file_root_node).nodes) analyze(*node);

    symbol_tree.curr_scope_name = "::";
    symbol_tree.pop_scope();
}

void Analyzer::determine_type(ASTNode& node) {
    match(node) {
        holds(Identifier) {}
        holds(PrimaryExpr) {}
        holds(UnaryExpr) {}
        holds(BinaryExpr) {}
        holds(PostfixExpr) {}
        holds(VariableDecl) {}
        holds(FunctionDecl) {}
        holds(BlockScope) {}
        holds(NamespaceDecl) {}
        holds(TypeDecl) {}
        _default {}
    }
}

void Analyzer::analyze(ASTNode& node) {

    match(node) {

        holds(Identifier, &id) {
            // NOTE: initializer lists might be analyzed and get their types later
            if (id.kind == Identifier::type_name && id.name == "Undetermined Type") return;

            id.declaration = symbol_tree.find_declaration(id);

            if (id.declaration) {
                node.type = id.declaration.value()->type;
            } else {
                report_error(node.source_token, "use of undeclared identifier '{}'", id.name);
            }
        }

        holds(PrimaryExpr, prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:
                case PrimaryExpr::floating_point:
                case PrimaryExpr::str: break;
                default: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
            }
        }

        holds(UnaryExpr, &un) {
            // TODO: add checks for '!' and such to only work on arithmetic types
            analyze(*un.expr);
            switch (un.kind) {
                case UnaryExpr::negate:
                case UnaryExpr::bitwise_not:
                    // if (!is_arithmetic_t(un.expr->type)) {
                    //     report_error(node.source_token,
                    //                  "invalid type '{}' for unary expression.",
                    //                  get_name(un.expr->type));
                    // }
                    // break;
                case UnaryExpr::return_statement:
                case UnaryExpr::logic_not:
                    // default: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
            }
            node.type = un.expr->type;
        }
        holds(BinaryExpr, &bin) {
            // let var: foo::bar = {123};
            // let var = foo::bar {213};
            // var = foo::bar {123};
            // var = {123};
            // let var = {}; should error
            // let var = {123}; OK
            // let var = {123, 213}; should error
            analyze(*bin.lhs);
            analyze(*bin.rhs);

            if (bin.kind == BinaryExpr::assignment) {
                if (bin.lhs->type.kind == Type::Undetermined && bin.rhs->type.kind == Type::Undetermined) {
                    report_error(node.source_token,
                                 "cannot deduce type for '{}' from assignment.",
                                 bin.lhs->source_token.to_str());
                }
                if (bin.lhs->type.kind == Type::Undetermined) bin.lhs->type = bin.rhs->type;
                if (bin.rhs->type.kind == Type::Undetermined) bin.rhs->type = bin.lhs->type;
            }
            if (bin.lhs->type.kind != bin.rhs->type.kind) {
                // NOTE: this should be removed later (it is okay because all builtin types are numeric atm)
                if (!(is_builtin_type(get_name(bin.lhs->type)) && is_builtin_type(get_name(bin.rhs->type))))
                    report_binary_error(node, bin);
            }
            // if (!is_compatible_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
            node.type = bin.lhs->type;
        }

        holds(VariableDecl, &var) {
            if (node.type.kind == Type::Undetermined) analyze(*node.type.identifier);
            if (symbol_tree.curr_scope_kind == ScopeKind::Namespace)
                var.kind = VariableDecl::global_var_declaration;
            add_declaration(node);
        }

        holds(FunctionDecl, &func) {
            add_declaration(node); // also sets func.kind in this call

            ScopeKind scope_kind;
            switch (func.kind) {
                case FunctionDecl::function_declaration: scope_kind = ScopeKind::FunctionBody; break;
                case FunctionDecl::member_func_declaration: scope_kind = ScopeKind::MemberFuncBody; break;
            }

            symbol_tree.push_scope(get_name(func), scope_kind);

            if (node.type.kind == Type::Undetermined) analyze(*node.type.identifier);

            for (auto& param : func.parameters) {
                analyze(*param);
                if (param->type.kind == Type::Undetermined) analyze(*param->type.identifier);
                // TODO: analyzing a type should automatically set the node's type
                // param->type = param->type.identifier->type;
            }

            if (func.body) {
                func.body.value()->type = node.type; // passing the function's type to the body
                for (auto& node : get<BlockScope>(func.body.value()).nodes) analyze(*node);
            }

            symbol_tree.pop_scope();
        }

        holds(BlockScope, &scope) {
            // NOTE: consider taking the last node and passing that value to the scope (and returning it like rust)
            switch (scope.kind) {
                case BlockScope::compound_statement:
                    // TODO: make sure that return statements match the function's return type
                    symbol_tree.push_scope("", ScopeKind::CompoundStatement);
                    for (auto& node : scope.nodes) analyze(*node);
                    symbol_tree.pop_scope();
                    break;
                case BlockScope::initializer_list:
                    if (node.type.kind == Type::Undetermined) analyze(*node.type.identifier);
                case BlockScope::argument_list: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
            }
        }

        holds(NamespaceDecl, const& nmspace_decl) {
            add_declaration(node);
            symbol_tree.push_scope(get_name(nmspace_decl), ScopeKind::Namespace);
            for (auto& node : nmspace_decl.nodes) analyze(*node);
            symbol_tree.pop_scope();
        }

        holds(TypeDecl, const& type_decl) {

            switch (type_decl.kind) {
                case TypeDecl::struct_declaration:
                    add_declaration(node);

                    symbol_tree.push_scope(get_name(type_decl), ScopeKind::TypeBody);
                    if (type_decl.definition_body) {
                        for (auto& node : type_decl.definition_body.value()) analyze(*node);
                    }
                    symbol_tree.pop_scope();
                    break;
                default: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
            }
        }

        holds(PostfixExpr, &postfix) {
            INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
            for (auto& node : postfix.nodes) analyze(*node);
        }

        _default INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
    }
}

void Analyzer::add_declaration(ASTNode& node) {
    // NOTE: dont forget we cant redefine a outer namespaced identifier inside another namespace
    // namespace foo{ fn func() -> void;}
    // namespace bar {
    //     fn foo::func() -> void {}
    // }   ^ this is not allowed.

    match(node) {

        holds(FunctionDecl, &func) {
            // FIXME: we should add each individual part of the identifier to the scope stack
            Identifier& id = get_id(func);
            // TODO: member function calls should be rewritten to take a pointer
            // to an instance of the class they are defined in. the "this" implicit pointer in C++
            // TODO: dont allow redeclarations of struct member functions inside the struct body itself

            if (id.qualifier == Identifier::qualified) { // check if its a member function
                str temp = id.name;
                while (temp.back() != ':') temp.pop_back();
                temp.pop_back(), temp.pop_back();
                if find_value(temp, symbol_tree.type_decls) func.kind = FunctionDecl::member_func_declaration;
            }

            auto [node_iterator, was_inserted] = symbol_tree.push_function_decl(id, node);

            if (!was_inserted && func.body) {
                auto& func_ = get<FunctionDecl>(node_iterator->second);
                if (func_.body) report_error(node.source_token, "Redefinition of '{}'.", id.mangled_name);
            }
        }

        holds(VariableDecl, &var) {
            Identifier& id = get_id(var);
            auto [_, was_inserted] = symbol_tree.push_variable_decl(id, node);

            if (!was_inserted && var.kind != VariableDecl::parameter)
                report_error(node.source_token, "Redefinition of '{}'.", id.mangled_name);
        }

        holds(NamespaceDecl, &nmspace_decl) {
            Identifier& id = get_id(nmspace_decl);
            symbol_tree.push_namespace_decl(id, node);

            if (symbol_tree.type_decls.contains(id.mangled_name))
                report_error(node.source_token,
                             "Redefinition of '{}' as a different kind of symbol.",
                             id.mangled_name);
        }

        holds(TypeDecl, &type_decl) {
            Identifier& id = get_id(type_decl);
            const auto& [node_iterator, was_inserted] = symbol_tree.push_type_decl(id, node);

            // TODO: add each member of the struct to the type decl info.
            // we need to store offsets and the names of the members (for solving lookups later)
            // and also the conversion from normal function name -> mangled function name
            auto& t_decl = get<TypeDecl>(node_iterator->second);

            if (t_decl.kind != type_decl.kind || symbol_tree.namespace_decls.contains(id.mangled_name))
                report_error(node.source_token,
                             "Redefinition of '{}' as a different kind of symbol.",
                             id.mangled_name);

            if (!was_inserted && type_decl.definition_body && t_decl.definition_body) {
                report_error(node.source_token, "Redefinition of '{}'.", id.mangled_name);
            }
        }

        _default {
            INTERNAL_PANIC("expected variable, got '{}'.", node.kind_name());
        }
    }
}

// struct gaming {let ahhh: i32;};
// let x: gaming;
// fn func() -> void {
//      struct gaming {let bbbb: f32;};
//      let x: gaming = 123123;
//      {
//          struct gaming {let vvvvv = 321;};
//          let x: gaming;
//          funcfunc();
//      }
// }
//
// somefunc();
// namespace huh {
//    let x = somefunc();
//    struct foo {
//         let member: i32;
//         fn somefunc() -> void;
//
//         fn another() -> void {
//             somefunc();
//             struct bar {};
//             {
//             somefunc();
//             }
//         }
//    };
//   let var: foo = {213123};
//   let x: foo = var;
// }
