#include "semantic_analysis/analyzer.hpp"
#include "utils/common_utils.hpp"
#include <ranges>

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

    if (auto map_node = symbol_tree.function_decls.extract("main"); !map_node.empty()) {
        // rename 'main' to link with libc later
        auto& id = get_id(get<FunctionDecl>(map_node.mapped()));
        map_node.key() = "fumo.user_main";
        id.mangled_name = "fumo.user_main";
        id.name = "fumo.user_main";
        symbol_tree.function_decls.insert(std::move(map_node));
    }
}

void Analyzer::analyze(ASTNode& node) { // NOTE: also performs type checking

    match(node) {

        holds(Identifier, &id) {
            // NOTE: initializer lists might be analyzed and get their types later
            if (id.kind == Identifier::type_name && id.name == "Undetermined Type") return;

            id.declaration = symbol_tree.find_declaration(id);
            if (id.declaration) {
                node.type = id.declaration.value()->type;
                if find_value (id.mangled_name, symbol_tree.member_variable_decls) {
                    id.kind = Identifier::member_var_name;
                    str temp = iter->first;
                    std::size_t pos = temp.find("::");
                    if (pos != std::string::npos) {
                        id.base_struct_name = temp.substr(0, pos);
                    }
                    // "promote" identifier to member variable name after finding it
                }

            } else {
                report_error(node.source_token, "use of undeclared identifier '{}'.", id.mangled_name);
            }
        }

        holds(PrimaryExpr, prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:
                case PrimaryExpr::floating_point:
                case PrimaryExpr::str:
                    break;
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
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
                                     "invalid type '{}' for unary expression.",
                                     type_name(un.expr->type));
                    }
                    node.type = un.expr->type;
                    break;
                // TODO: return should be moved to a new struct later
                case UnaryExpr::dereference:
                    if (!un.expr->type.ptr_count) {
                        report_error(node.source_token, "Indirection requires pointer operand, '{}' is invalid.",
                                     type_name(un.expr->type));
                    }
                    node.type = un.expr->type;
                    node.type.ptr_count--;
                    break;
                case UnaryExpr::address_of:
                    node.type = un.expr->type;
                    if (auto* un_expr = get_if<UnaryExpr>(un.expr);
                        un_expr && un_expr->kind == UnaryExpr::address_of) {
                        if (!node.type.ptr_count) INTERNAL_PANIC("you passed a non ptr and got '&' issues somehow.");
                        report_error(node.source_token, "Cannot take the address of an rvalue of type '{}'.",
                                     type_name(un.expr->type));
                    }
                    node.type.ptr_count++;
                    break;
                case UnaryExpr::return_statement:
                    node.type = un.expr->type;
                    break;
            }
        }
        holds(BinaryExpr, &bin) {
            analyze(*bin.lhs);
            analyze(*bin.rhs);

            if (bin.kind == BinaryExpr::assignment) {
                if (is_branch<FunctionCall>(bin.lhs)) { // didnt have a dereference so its invalid
                    report_error(node.source_token, "cannot assign to temporary function return value.");
                }
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
                // this is pretty bad, consider refactoring type inference later
                if (auto decl = get_id(node.type).declaration) {
                    node.type.identifier = decl.value()->type.identifier;
                    node.type.kind = decl.value()->type.kind;
                }
            }

            if (symbol_tree.curr_scope_kind == ScopeKind::Namespace)
                var.kind = VariableDecl::global_var_declaration;
            if (symbol_tree.curr_scope_kind == ScopeKind::TypeBody)
                var.kind = VariableDecl::member_var_declaration;

            add_declaration(node);
        }

        holds(FunctionDecl, &func) {
            // TODO: check if all returns in the body have the correct type of the function
            add_declaration(node);
            Identifier& id = get_id(func);

            iterate_qualified_names(func);

            if (node.type.kind == Type::Undetermined) { // NOTE: this should be moved to determine_type()
                analyze(*node.type.identifier);
                if (auto decl = get_id(node.type).declaration) {
                    node.type.identifier = decl.value()->type.identifier;
                    node.type.kind = decl.value()->type.kind;
                }
            }
            if (func.kind == FunctionDecl::member_func_declaration) {
                ASTNode* node_ = push(ASTNode {node.source_token});
                // node_->branch =
                //     VariableDecl {VariableDecl::parameter,
                //                   push({node_->source_token, Identifier {Identifier::declaration_name, "this"}})};
                //
                // str temp = id.mangled_name;
                // while (temp.back() != ':') temp.pop_back();
                // temp.pop_back(), temp.pop_back();
                // Identifier temp_id = {.kind = Identifier::type_name, .name = temp};
                // // getting the type name of the struct that this function is a member of
                // // a bit hacky but its okay
                // node_->type = symbol_tree.find_declaration(temp_id).value()->type;
                // node_->type.ptr_count = 1;
                //
                // func.parameters.push_back(node_);
            }
            for (auto& param : func.parameters) {
                analyze(*param);
                // TODO: analyzing a type should automatically set the node's type
            }

            if (func.body) {
                func.body.value()->type = node.type; // passing the function's type to the body
                for (auto& node : get<BlockScope>(func.body.value()).nodes) analyze(*node);
                if (func.body_should_move) func.body = std::nullopt;
            }

            for (int i = 0; i <= id.scope_counts; i++) symbol_tree.pop_scope();
        }

        holds(FunctionCall, &func_call) {
            // TODO: this is probably wrong, wont find the function decl correctly
            analyze(*func_call.identifier);
            node.type = func_call.identifier->type;

            const auto& func_decl = get<FunctionDecl>(get_id(func_call).declaration.value());
            if (func_decl.kind == FunctionDecl::member_func_declaration) {
                func_call.kind = FunctionCall::member_function_call;
            }
            const auto& params = func_decl.parameters;

            if (func_call.argument_list.size() != params.size()) {
                report_error(node.source_token,
                             "provided {} arguments, expected {}.",
                             func_call.argument_list.size(),
                             params.size());
            }
            for (auto [arg, param] : std::views::zip(func_call.argument_list, params)) {
                analyze(*arg);
                if (!is_compatible_t(arg->type, param->type)) {
                    report_error(arg->source_token,
                                 "argument of type '{}' is not compatible with function declaration signature.",
                                 type_name(arg->type));
                }
            }
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
                    if (node.type.kind == Type::Undetermined) {
                        analyze(*node.type.identifier);
                        if (auto decl = get_id(node.type).declaration) {
                            node.type = decl.value()->type;
                        }
                    }
                    break;
            }
        }

        holds(NamespaceDecl, const& nmspace_decl) {
            add_declaration(node);
            symbol_tree.push_scope(get_name(nmspace_decl), ScopeKind::Namespace);
            for (auto& node : nmspace_decl.nodes) analyze(*node);
            symbol_tree.pop_scope();
        }

        holds(TypeDecl, &type_decl) {
            switch (type_decl.kind) {
                case TypeDecl::struct_declaration: {
                    add_declaration(node);

                    symbol_tree.push_scope(get_name(node.type), ScopeKind::TypeBody);
                    if (type_decl.definition_body) {
                        for (auto& node : type_decl.definition_body.value()) analyze(*node);

                        if (type_decl.body_should_move) type_decl.definition_body = std::nullopt;
                    }
                    symbol_tree.pop_scope();
                    break;
                }
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.name());
            }
        }

        // NOTE: id like to refactor this later
        holds(PostfixExpr, &postfix) { // nodes is never empty
            str prev_name = "";
            str curr_base_name = "";
            Identifier* curr_id = nullptr;
            for (auto node_it = postfix.nodes.begin(); node_it != postfix.nodes.end(); ++node_it) {
                auto& curr_node = *node_it;

                if (auto* un = get_if<UnaryExpr>(curr_node)) {
                    if (auto* id = get_if<Identifier>(un->expr)) curr_id = id;
                } else if (auto* id = get_if<Identifier>(curr_node)) {
                    curr_id = id;
                } else if (auto* func_call = get_if<FunctionCall>(curr_node)) {
                    auto& id = get<Identifier>(func_call->identifier);
                    curr_id = &id;
                } else
                    INTERNAL_PANIC("wrong node branch pushed to postfix expression.");

                curr_id->mangled_name = prev_name + curr_id->name;
                curr_id->base_struct_name = curr_base_name;
                prev_name = ""; // reset the previous name 

                analyze(*curr_node);
                // we add the type name to the string, for lookup in the symbol table
                // struct gaming { let foo: i32; };
                // x.foo => "gaming::foo"
                // we find the declaration of the member variable, or error
                
                auto& id = get_id(curr_node->type);
                prev_name += id.mangled_name;
                curr_base_name = prev_name;
                // report_error(curr_node->source_token, "found '{}'", prev_name);
                prev_name += "::";
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
            auto& first_occurence = get<FunctionDecl>(node_iterator->second);

            if (!was_inserted) {
                str def_or_decl;
                if (func.body) {
                    if (first_occurence.body)
                        report_error(node.source_token, "Redefinition of '{}'.", id.mangled_name);
                    def_or_decl = "Redefinition";

                    first_occurence.body = func.body;
                    func.body_should_move = true; // avoid repeated function bodies
                    // we move the function body to the first forward declaration in the file
                    // and remove it from where it was defined
                } else {
                    def_or_decl = "Redeclaration";
                }

                if (node.type.kind != node_iterator->second->type.kind) {
                    report_error(node.source_token,
                                 "{} of '{}' with a different return type.",
                                 def_or_decl,
                                 id.mangled_name);
                }
                if (func.parameters.size() != first_occurence.parameters.size()) {
                    report_error(node.source_token,
                                 "{} of '{}' with a different parameter count.",
                                 def_or_decl,
                                 id.mangled_name);
                }
                for (auto [arg1, arg2] : std::views::zip(func.parameters, first_occurence.parameters)) {
                    if (!is_same_t(arg1->type, arg2->type)) {
                        report_error(node.source_token,
                                     "{} of '{}' with different parameter types.",
                                     def_or_decl,
                                     id.mangled_name);
                    }
                    if (get_id(get<VariableDecl>(arg1)).mangled_name
                        != get_id(get<VariableDecl>(arg2)).mangled_name) {
                        report_error(node.source_token,
                                     "{} of '{}' with different parameter names.",
                                     def_or_decl,
                                     id.mangled_name);
                    }
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
            auto& first_occurence = get<TypeDecl>(node_iterator->second);

            if (first_occurence.kind != type_decl.kind || symbol_tree.namespace_decls.contains(id.mangled_name)) {
                report_error(node.source_token,
                             "Redefinition of '{}' as a different kind of symbol.",
                             id.mangled_name);
            }

            if (!was_inserted) {
                if (type_decl.definition_body) {
                    if (first_occurence.definition_body) {
                        report_error(node.source_token, "Redefinition of '{}'.", id.mangled_name);
                    }
                    first_occurence.definition_body = type_decl.definition_body;
                    type_decl.body_should_move = true;

                } else {
                    // type_decl.definition_body = first_occurence.definition_body;
                }
            }
        }

        _default INTERNAL_PANIC("expected variable, got '{}'.", node.kind_name());
    }
}
