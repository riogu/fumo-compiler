#include "semantic_analysis/analyzer.hpp"

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
    // TODO: move type checks from analyze() to this function
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
                report_error(node.source_token, "use of undeclared identifier '{}'.", id.mangled_name);
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
                case UnaryExpr::logic_not:
                    if (!is_arithmetic_t(un.expr->type)) {
                        report_error(node.source_token,
                                     "invalid type '{}' for unary expression.", get_name(un.expr->type));
                    }
                    break;
                case UnaryExpr::return_statement:
                // TODO: return should be moved to a new struct later
                case UnaryExpr::dereference:
                    // NOTE: unary expr doesnt have to do anything here other than
                    // check that the held expr is a pointer.

                    // default: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
            }
            node.type = un.expr->type;
        }
        holds(BinaryExpr, &bin) {
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
            if (!is_compatible_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
            node.type = bin.lhs->type;
        }

        holds(VariableDecl, &var) {
            if (node.type.kind == Type::Undetermined) {
                analyze(*node.type.identifier);
                node.type = node.type.identifier->type;
            }
            if (symbol_tree.curr_scope_kind == ScopeKind::Namespace) var.kind = VariableDecl::global_var_declaration;
            add_declaration(node);
        }

        holds(FunctionDecl, &func) {
            add_declaration(node);
            Identifier& id = get_id(func);

            iterate_qualified_names(func);

            if (node.type.kind == Type::Undetermined) { // NOTE: this should be moved to determine_type()
                analyze(*node.type.identifier);
                node.type = node.type.identifier->type;
            }

            for (auto& param : func.parameters) {
                analyze(*param);
                if (param->type.kind == Type::Undetermined) {
                    analyze(*param->type.identifier);
                    param->type = param->type.identifier->type;
                }
                // TODO: analyzing a type should automatically set the node's type
            }

            if (func.body) {
                func.body.value()->type = node.type; // passing the function's type to the body
                for (auto& node : get<BlockScope>(func.body.value()).nodes) analyze(*node);
            }

            for (int i = 0; i <= id.scope_counts; i++) symbol_tree.pop_scope();
        }

        holds(BlockScope, &scope) {
            // NOTE: consider taking the last node and passing that value to the scope (and returning it like rust)
            switch (scope.kind) {
                // TODO: make sure that return statements match the function's return type
                case BlockScope::compound_statement:
                    if (symbol_tree.curr_scope_kind == ScopeKind::MemberFuncBody
                     || symbol_tree.curr_scope_kind == ScopeKind::MemberCompoundStatement) {
                        symbol_tree.push_scope("", ScopeKind::MemberCompoundStatement);
                    } else {
                        symbol_tree.push_scope("", ScopeKind::CompoundStatement);
                    }
                    for (auto& node : scope.nodes) analyze(*node);
                    symbol_tree.pop_scope();
                    break;
                case BlockScope::initializer_list:
                    for (auto& node : scope.nodes) analyze(*node);
                    if (node.type.kind == Type::Undetermined) analyze(*node.type.identifier);
                    node.type = node.type.identifier->type;
                    break;
                    // INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
                case BlockScope::argument_list:
                    for (auto& node : scope.nodes) analyze(*node);
                    break;
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

                    symbol_tree.push_scope(get_name(node.type), ScopeKind::TypeBody);
                    if (type_decl.definition_body) {
                        for (auto& node : type_decl.definition_body.value()) analyze(*node);
                    }
                    symbol_tree.pop_scope();
                    break;
                default: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
            }
        }

        holds(PostfixExpr, &postfix) { // nodes is never empty
            str prev_name = "";
            for (auto node_it = postfix.nodes.begin(); node_it != postfix.nodes.end(); ++node_it) {
                auto& node = *node_it;
                if_holds(<UnaryExpr>(node), un) {
                    if_holds(<Identifier>(un->expr), id) {
                        switch (id->kind) {
                            case (Identifier::func_call_name, Identifier::member_func_call_name)
                                id->mangled_name = prev_name + id->name; 
                                prev_name = ""; 
                                break;
                            case (Identifier::var_name, Identifier::member_var_name)
                                id->mangled_name = prev_name + id->name; 
                                break; 
                            default:
                                INTERNAL_PANIC("wrong node branch pushed to postfix expression.");
                        }
                    }
                }
                else if_holds(<Identifier>(node), id) {
                    switch (id->kind) {
                        case(Identifier::func_call_name, Identifier::member_func_call_name, Identifier::member_var_name)
                            id->mangled_name = prev_name + id->name; 
                            prev_name = ""; 
                            break;
                        case (Identifier::var_name)
                            id->mangled_name = prev_name + id->name; break; 
                        default:
                            INTERNAL_PANIC("wrong node branch pushed to postfix expression.");
                    }
                }
                else if (is_branch<BlockScope>(node)) {
                    node->type = (*(node_it - 1))->type;
                    prev_name = ""; 
                }
                else INTERNAL_PANIC("wrong node branch pushed to postfix expression.");

                analyze(*node);
                prev_name += get_id(node->type).mangled_name + "::";
            }
            node.type = postfix.nodes.back()->type;
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
            Identifier& id = get_id(func);
            // TODO: member function calls should be rewritten to take a pointer
            // to an instance of the class they are defined in. the "this" implicit pointer in C++
            // TODO: dont allow redeclarations of struct member functions inside the struct body itself

            auto [node_iterator, was_inserted] = symbol_tree.push_function_decl(id, node);
            auto& func_ = get<FunctionDecl>(node_iterator->second);

            if (!was_inserted) {
                if (func.body) {
                    if (func_.body) report_error(node.source_token, "Redefinition of '{}'.", id.mangled_name);
                    func_.body = func.body;
                } else {
                    func.body = func_.body;
                }
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
            Identifier& id = get_id(node.type);
            const auto& [node_iterator, was_inserted] = symbol_tree.push_type_decl(id, node);

            // TODO: add each member of the struct to the type decl info.
            // we need to store offsets and the names of the members (for solving lookups later)
            // and also the conversion from normal function name -> mangled function name
            auto& t_decl = get<TypeDecl>(node_iterator->second);

            if (t_decl.kind != type_decl.kind || symbol_tree.namespace_decls.contains(id.mangled_name)) {
                report_error(node.source_token, "Redefinition of '{}' as a different kind of symbol.", id.mangled_name);
            }

            if (!was_inserted) {
                if (type_decl.definition_body) {
                    if (t_decl.definition_body) report_error(node.source_token, "Redefinition of '{}'.", id.mangled_name);
                    t_decl.definition_body = type_decl.definition_body;

                } else {
                    type_decl.definition_body = t_decl.definition_body;
                }
            }
        }

        _default INTERNAL_PANIC("expected variable, got '{}'.", node.kind_name());
        
    }
}
