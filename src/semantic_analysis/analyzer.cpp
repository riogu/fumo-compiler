#include "semantic_analysis/analyzer.hpp"
#include "base_definitions/ast_node.hpp"
#include "utils/common_utils.hpp"

void Analyzer::semantic_analysis(ASTNode* file_root_node) {
    root_node = file_root_node;

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


    // analyze control flow
    for (auto& [_, func] : symbol_tree.function_decls) {
        analyze_function_control_flow(*func); 
    }
    auto map_node = symbol_tree.function_decls.extract("main");

    if (!map_node.empty()) {
        // rename 'main' to link with libc later
        auto& id = get_id(get<FunctionDecl>(map_node.mapped()));
        map_node.key() = "fumo.user_main";
        id.mangled_name = "fumo.user_main";
        id.name = "fumo.user_main";
        symbol_tree.function_decls.insert(std::move(map_node));
        map_node = symbol_tree.all_declarations.extract("main");
        map_node.key() = "fumo.user_main";
        symbol_tree.all_declarations.insert(std::move(map_node));
    }
    for(auto& [_, func]: symbol_tree.member_function_decls)  analyze_function_control_flow(*func); 
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
                    // this allows variables to implicitly be used as member variables in member functions
                    // we store the base struct name for lookup later
                    id.kind = Identifier::member_var_name;
                    str temp = iter->first;
                    while (temp.back() != ':') temp.pop_back(); temp.pop_back(), temp.pop_back();
                    id.base_struct_name = temp;
                }
            } else {
                // we need better error reporting for generics:
                if (id.is_generic_wrapper()) {
                    report_error(node.source_token, "failed generic instantiation: couldn't find base declaration for  {} '{}'.",
                                 id.is_func_call() ? "function" : "struct", full_mangled_name(&node));
                }
                report_error(node.source_token, "couldn't find declaration for '{}'.", full_mangled_name(&node));
            }
            switch (id.kind) {
                case Identifier::var_name:
                case Identifier::member_var_name:
                case Identifier::func_call_name:
                case Identifier::member_func_call_name:
                case Identifier::generic_wrapper_type_name:
                    curr_postfix_id = &id; // for solving postfix expression identifiers
                case Identifier::unknown_name:
                case Identifier::declaration_name:
                case Identifier::type_name:
                case Identifier::generic_type_name:
                    break;
                default: internal_error(node.source_token, "semantic analyis not implemented for '{}'", node.name());
            }
        }

        holds(PrimaryExpr, &prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:
                case PrimaryExpr::floating_point:
                case PrimaryExpr::str:
                case PrimaryExpr::bool_:
                case PrimaryExpr::void_:
                case PrimaryExpr::char_literal:
                    break;
                default:
                    internal_panic("semantic analysis missing for '{}'.", node.name());
            }
        }

        holds(UnaryExpr, &un) {
            if (un.kind == UnaryExpr::return_statement) {
                if (un.expr) {
                    analyze(*un.expr.value());
                    node.type = un.expr.value()->type;
                } else // allow returning nothing for void functions (control flow)
                    node.type = {push({node.source_token, Identifier {Identifier::type_name, "void"}}), Type::void_};
                return;
            }
            analyze(*un.expr.value());
            switch (un.kind) {
                case UnaryExpr::logic_not:
                    if (!is_arithmetic_t(un.expr.value()->type) && !is_ptr_t(un.expr.value()->type)) {
                        report_error(node.source_token,
                                     "invalid type '{}' for unary expression.",
                                     type_name(un.expr.value()->type));
                    }
                    node.type = un.expr.value()->type;
                    break;
                case UnaryExpr::negate:
                case UnaryExpr::bitwise_not:
                    if (!is_arithmetic_t(un.expr.value()->type)) {
                        report_error(node.source_token,
                                     "invalid type '{}' for unary expression.",
                                     type_name(un.expr.value()->type));
                    }
                    node.type = un.expr.value()->type;
                    break;
                // TODO: return should be moved to a new struct later
                case UnaryExpr::dereference:
                    if (!un.expr.value()->type.ptr_count) {
                        report_error(node.source_token, "dereference requires pointer operand, '{}' is invalid.",
                                     type_name(un.expr.value()->type));
                    }
                    if(un.expr.value()->type.kind == Type::any_) {
                        report_error(node.source_token, "can't deference value of type '{}'.", type_name(un.expr.value()->type));
                    }
                    node.type = un.expr.value()->type;
                    node.type.ptr_count--;
                    break;
                case UnaryExpr::address_of:
                    node.type = un.expr.value()->type;
                    if (auto* un_expr = get_if<UnaryExpr>(un.expr.value()); // cant use '&' on results of unaryExpr
                        un_expr && un_expr->kind == UnaryExpr::address_of) {
                        if (!node.type.ptr_count) internal_panic("you passed a non ptr and got '&' issues somehow.");
                        report_error(node.source_token, "Cannot take the address of an rvalue of type '{}'.",
                                     type_name(un.expr.value()->type));
                    }
                    node.type.ptr_count++;
                    break;
                default: internal_panic("semantic analysis missing for '{}'.", node.name());
            }
        }

        holds(BinaryExpr, &bin) {
            analyze(*bin.lhs);
            analyze(*bin.rhs);
            switch (bin.kind) {
                case BinaryExpr::assignment: {
                    if (is_branch<FunctionCall>(bin.lhs)) { // didnt have a dereference so its invalid
                        report_error(node.source_token, "cannot assign to temporary function return value.");
                    }
                    if (bin.lhs->type.kind == Type::Undetermined && bin.rhs->type.kind == Type::Undetermined) {
                        if (auto* init_list = get_if<BlockScope>(bin.rhs)) {
                            // allow this case: let var = {123};
                            if (init_list->nodes.size() == 1) bin.rhs->type = init_list->nodes[0]->type;
                            else report_error(node.source_token, "cannot deduce type for '{}' from assignment.",
                                                bin.lhs->source_token.to_str());
                        } else report_error(node.source_token, "cannot deduce type for '{}' from assignment.",
                                            bin.lhs->source_token.to_str());
                    }
                    if (bin.lhs->type.kind == Type::Undetermined) bin.lhs->type = bin.rhs->type;
                    if (bin.rhs->type.kind == Type::Undetermined) bin.rhs->type = bin.lhs->type;
                    // this extra check avoids the case where you use another type with the same layout on the rhs
                    if (!is_compatible_or_generic_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
                    // ----------------------------------------------------------------------------
                    // checks for initializer lists (make sure they match the original type)
                    // also allow those edge cases with pointers and initializer lists
                    check_initializer_lists(node, bin);
                    // ----------------------------------------------------------------------------
                    node.type = bin.lhs->type; // since we take the lhs type, any* is always cast correctly
                    break;
                }
                case BinaryExpr::add: 
                case BinaryExpr::sub:
                    if (is_int_t(bin.lhs->type) && is_ptr_t(bin.rhs->type)) {
                        report_error(bin.rhs->source_token,
                                     "pointer arithmetic is only allowed on the rhs of an expression ex: (ptr + 3).");
                    }
                    if (is_ptr_t(bin.lhs->type) && is_int_t(bin.rhs->type) && bin.lhs->type.kind != Type::any_) {
                        // special cases for pointer arithmetic
                        // dont check compatibility for this case
                    } 
                    else if (!is_compatible_or_generic_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
                    node.type = bin.lhs->type;
                    break;

                case BinaryExpr::equal: {
                case BinaryExpr::not_equal:// we can compare pointers and numbers only
                    if (!is_compatible_or_generic_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
                    // we know they are equal so we only check lhs
                    if (!is_ptr_t(bin.lhs->type) && !is_arithmetic_t(bin.lhs->type)) report_binary_error(node, bin);
                    node.type.kind = Type::bool_;
                    auto& id = get_id(node.type); id.name = "bool"; id.mangled_name = "bool";
                    break;
                }
                case BinaryExpr::multiply:
                case BinaryExpr::divide:
                case BinaryExpr::modulus:
                    if (!is_compatible_or_generic_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
                    node.type = bin.lhs->type;
                    break; 
                case BinaryExpr::less_than:
                case BinaryExpr::less_equals: {
                    if (!is_compatible_or_generic_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
                    node.type.kind = Type::bool_;
                    auto& id = get_id(node.type); id.name = "bool"; id.mangled_name = "bool";
                    break; 
                }
                case BinaryExpr::logical_and:
                case BinaryExpr::logical_or: // these are always bools
                    node.type.kind = Type::bool_;
                    auto& id = get_id(node.type); id.name = "bool"; id.mangled_name = "bool";
                    break; // dont have to be the same type, so we ignore comparisons
            }
        }

        holds(VariableDecl, &var) {
            if (symbol_tree.curr_scope_kind == ScopeKind::Namespace) var.kind = VariableDecl::global_var_declaration;
            if (symbol_tree.curr_scope_kind == ScopeKind::TypeBody)  var.kind = VariableDecl::member_var_declaration;

            check_for_generic_instantiation(node); // example: let var: Node[i32];

            if (node.type.kind == Type::Undetermined || node.type.kind == Type::Generic) {
                analyze(*node.type.identifier);
                if (auto decl = get_id(node.type).declaration) {
                    node.type.identifier = decl.value()->type.identifier;
                    node.type.kind = decl.value()->type.kind;
                }
            }

            add_declaration(node);
        }

        holds(FunctionDecl, &func) {
            vec<Scope> scopes = iterate_qualified_names(func, node);

            check_for_generic_instantiation(node);

            Identifier& id = get_id(func);
            // func.scopes = scopes; // save for later
            push_generic_context(id.generic_identifiers);
            // -------------------------------------------------------------------
            // hack to get the type checked correctly
            // will also work even if its a generic return type
            for (auto& scope : scopes) symbol_tree.push_scope(scope.name, scope.kind);

            if (node.type.kind == Type::Undetermined || node.type.kind == Type::Generic) {
                analyze(*node.type.identifier);
                if (auto decl = get_id(node.type).declaration) {
                    node.type.identifier = decl.value()->type.identifier;
                    node.type.kind = decl.value()->type.kind;
                    // we lose the pointers because of the declaration not having any
                    // if we simply assign the whole type
                }
            }
            for (auto& param : func.parameters) analyze(*param);
            for (int i = 0; i <= id.scope_counts; i++) symbol_tree.pop_scope();
            // -------------------------------------------------------------------
            add_declaration(node); // should be done first
            // -------------------------------------------------------------------

            for (auto& scope : scopes) symbol_tree.push_scope(scope.name, scope.kind);
            if (func.body) {
                func.body.value()->type = node.type; // passing the function's type to the body
                for (auto& node : get<BlockScope>(func.body.value()).nodes) analyze(*node);
                if (func.body_should_move) func.body = std::nullopt;
            }
            for (int i = 0; i <= id.scope_counts; i++) symbol_tree.pop_scope();
            pop_generic_context(id.generic_identifiers);
            // -------------------------------------------------------------------
            // checks for static member functions (only allowed in structs for now)
            if (node.type.qualifiers.contains(Type::static_)) {
                if (func.kind == FunctionDecl::function_declaration) {
                    // we dont allow other kinds of static functions, since they arent implemented yet
                    // (the ones that define the linkage of a function)
                    report_error(node.source_token, "static function declarations are only allowed in struct member functions.");
                }

            }
        }

        holds(FunctionCall, &func_call) {
            check_for_generic_instantiation(node); // example: let var = foo[i32]();
            
            analyze(*func_call.identifier);
            node.type = func_call.identifier->type;
            auto& id = get_id(func_call);
            auto* func_decl = &get<FunctionDecl>(id.declaration.value());
            // later use this new function declaration if it exists
            // like any other function declaration, but it was instantiated on usage (generic)

            if (func_decl->kind == FunctionDecl::member_func_declaration) {
                func_call.kind = FunctionCall::member_function_call;
                // as it is, we can call static member functions from instances of objects
            }
            const auto& params = func_decl->parameters;

            if (func_call.argument_list.size() != params.size()
            && !(func_decl->is_variadic && func_call.argument_list.size() > params.size())) {
                str is_variadic_str = func_decl->is_variadic? "or more" : "";
                report_error(node.source_token, "provided {} arguments, expected {} {}.",
                             func_call.argument_list.size(), params.size(), is_variadic_str);
            }

            for (auto* arg : func_call.argument_list) analyze(*arg);

            for (auto [arg, param] : std::views::zip(func_call.argument_list, params)) {
                if (!is_compatible_or_generic_t(arg->type, param->type)) {
                    report_error(arg->source_token,
                                 "argument of type '{}' is not compatible with parameter of type '{}'.",
                                 type_name(arg->type), type_name(param->type));
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
                case BlockScope::initializer_list: {
                    check_for_generic_instantiation(node); // example: let var = Foo[i32]{};

                    for (auto& node : scope.nodes) analyze(*node);

                    if (node.type.kind == Type::Undetermined || node.type.kind == Type::Generic) {
                        analyze(*node.type.identifier);
                        if (auto decl = get_id(node.type).declaration) {
                            node.type = decl.value()->type;
                        }
                    }
                    break;
                }
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
                    // if (get_id(node.type).is_generic_wrapper()) return;
                    push_generic_context(get_id(node.type).generic_identifiers);
                    
                    symbol_tree.push_scope(get_name(node.type), ScopeKind::TypeBody);
                    if (type_decl.definition_body) {
                        for (auto& node : type_decl.definition_body.value()) analyze(*node);

                        if (type_decl.body_should_move) type_decl.definition_body = std::nullopt;
                    }
                    symbol_tree.pop_scope();
                    pop_generic_context(get_id(node.type).generic_identifiers);
                    break;
                }
                default:
                    internal_panic("semantic analysis missing for '{}'.", node.name());
            }
        }
        // TODO: replace this with the code below it (after debugging that version)
        // also note this might break when we add generics and someone calls a generic member function
        holds(PostfixExpr, &postfix) { // nodes is never empty
            str prev_name = "";
            str curr_base_name = "";
            Identifier* curr_id = nullptr;
            for (auto* curr_node: postfix.nodes) {
                if (auto* un = get_if<UnaryExpr>(curr_node)) {
                    if (auto* id = get_if<Identifier>(un->expr.value())) curr_id = id;
                    else if (auto* func_call = get_if<FunctionCall>(un->expr.value())) {
                        curr_id = &get<Identifier>(func_call->identifier);
                    }
                } else if (auto* id = get_if<Identifier>(curr_node)) {
                    curr_id = id;
                } else if (auto* func_call = get_if<FunctionCall>(curr_node)) {
                    auto& id = get<Identifier>(func_call->identifier);
                    curr_id = &id;
                } else
                    internal_panic("wrong node branch pushed to postfix expression.");
                curr_id->mangled_name = prev_name + curr_id->name;
                curr_id->base_struct_name = curr_base_name;
                prev_name = ""; // reset the previous name 

                analyze(*curr_node);
                // we add the type name to the string, for lookup in the symbol table
                // struct gaming { let foo: i32; };
                // x.foo => "gaming::foo". we find the declaration of the member variable, or error
                auto& id = get_id(curr_node->type);
                prev_name += id.mangled_name;
                curr_base_name = prev_name;
                // report_error(curr_node->source_token, "found '{}'", prev_name);
                // foo->(***bar.x)->
                prev_name += "::";
            }
            node.type = postfix.nodes.back()->type;
        }

        // holds(PostfixExpr, &postfix) { // nodes is never empty
        //     // we add the type name to the string, for lookup in the symbol table
        //     // struct gaming { let foo: i32; }; // x.foo => "gaming::foo" (how its stored)
        //     str prev_name = "";
        //     str curr_base_name = "";
        //     for (auto* curr_node: postfix.nodes) {
        //         // i need the current id here
        //         if (curr_postfix_id) {
        //             curr_postfix_id.value()->mangled_name = prev_name + curr_postfix_id.value()->name;
        //             curr_postfix_id.value()->base_struct_name = curr_base_name;
        //         } else {
        //             internal_error(curr_node->source_token, "this node branch didnt provide an identifier.");
        //         }
        //         prev_name = ""; // reset the previous name
        //         analyze(*curr_node); // NOTE: we expect this call to set curr_postfix_id
        //
        //         prev_name += get_id(curr_node->type).mangled_name;
        //         curr_base_name = prev_name;
        //         prev_name += "::";
        //     }
        //     node.type = postfix.nodes.back()->type;
        // }

        holds(IfStmt, &if_stmt) {
            bool was_assignment = false;
            if (if_stmt.kind == IfStmt::if_statement || if_stmt.kind == IfStmt::else_if_statement) {
                auto* cond = if_value(if_stmt.condition)
                             else_panic_error(node.source_token, "somehow found no condition in '{}'.", node.name());
                if (is_branch<VariableDecl>(cond)) { 
                    // only happens if it WASNT assigned (else its wrapped by a BinaryExpr::assignment)
                    report_error(cond->source_token, "variable declaration in condition must have an initializer.");
                }
                if (auto* decl = get_if<BinaryExpr>(cond); decl && decl->kind == BinaryExpr::assignment) {
                    auto& nodes = get<BlockScope>(if_stmt.body).nodes;
                    nodes.insert(nodes.begin(), cond);
                    was_assignment = true;
                    // we want the variable declaration to be 'scoped' inside the if statement body
                    // we remove it from the body after name resolution
                } else {
                    analyze(*cond);
                }
                analyze(*if_stmt.body);
                if (!is_ptr_t(cond->type) && !is_arithmetic_t(cond->type)) {
                    report_error(cond->source_token, "value of type '{}' is not convertible to 'bool'",
                                 type_name(cond->type));
                }
            } else { // done this way so we correctly check the type after analyzing it above ^
                analyze(*if_stmt.body); 
            }
            if (was_assignment) { // removing the variable from the body (for codegen later)
                auto& nodes = get<BlockScope>(if_stmt.body).nodes;
                nodes.erase(nodes.begin());
            }
            if (if_stmt.else_stmt) analyze(*if_stmt.else_stmt.value());
        }
        holds(WhileStmt, &while_loop) {
            auto* cond = while_loop.condition;
            analyze(*cond);
            if (!is_ptr_t(cond->type) && !is_arithmetic_t(cond->type)) {
                report_error(cond->source_token, "value of type '{}' is not convertible to 'bool'",
                             type_name(cond->type));
            }
            analyze(*while_loop.body);
        }
        _default internal_panic("semantic analysis missing for '{}'.", node.name());
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
            // TODO: dont allow redeclarations of struct member functions inside the struct body itself
            auto [node_iterator, was_inserted] = symbol_tree.push_function_decl(id, node);
            auto& first_occurence = get<FunctionDecl>(node_iterator->second);

            if (was_inserted && id.qualifier == Identifier::qualified
                && func.kind == FunctionDecl::member_func_declaration) {
                str temp = full_mangled_name(func.identifier); while (temp.back() != ':') temp.pop_back(); 
                temp.pop_back(), temp.pop_back(); // get parent name
                str was_static = ""; // better errors for missing static qualifiers
                if find_value(id.mangled_name, symbol_tree.function_decls) {
                    if (iter->second->type.qualifiers.contains(Type::static_)) {
                        was_static = "NOTE: did you mean to declare this function as 'static'?";
                    }
                }
                report_error(node.source_token, "out-of-line definition of '{}' does not match any declaration in '{}'. {}",
                             full_mangled_name(func.identifier), temp, was_static);
            }
            if (!was_inserted) {
                str def_or_decl;
                if (func.body) {
                    if (first_occurence.body)
                        report_error(node.source_token, "Redefinition of '{}'.", full_mangled_name(func.identifier));
                    def_or_decl = "Redefinition";

                    first_occurence.body = func.body;
                    func.body_should_move = true; // avoid repeated function bodies
                    // we move the function body to the first forward declaration in the file
                    // and remove it from where it was defined
                } else {
                    def_or_decl = "Redeclaration";
                }
                if (first_occurence.is_variadic != func.is_variadic) {
                    report_error(node.source_token, "'{}' of '{}' with different parameters (variadic)",
                                 def_or_decl, full_mangled_name(func.identifier));
                }

                if (!is_same_t(node.type, node_iterator->second->type)) {
                    report_error(node.source_token,
                                 "{} of '{}' with a different return type '{}' (expected '{}').",
                                 def_or_decl, full_mangled_name(func.identifier),
                                 type_name(node.type), type_name(node_iterator->second->type));
                }
                if (func.parameters.size() != first_occurence.parameters.size()) {
                    report_error(node.source_token,
                                 "{} of '{}' with a different parameter count.",
                                 def_or_decl, full_mangled_name(func.identifier));
                }
                for (auto [arg1, arg2] : std::views::zip(func.parameters, first_occurence.parameters)) {
                    if (!is_same_t(arg1->type, arg2->type)) {
                        report_error(node.source_token,
                                     "{} of '{}' with different parameter types (was '{}', found '{}')",
                                     def_or_decl, full_mangled_name(func.identifier),
                                     type_name(arg2->type), type_name(arg1->type));
                    }
                    // if (get_id(get<VariableDecl>(arg1)).mangled_name != get_id(get<VariableDecl>(arg2)).mangled_name) {
                    //     report_error(node.source_token,
                    //                  "{} of '{}' with different parameter names.",
                    //                  def_or_decl,
                    //                  id.mangled_name);
                    // }
                }
            }
        }

        holds(VariableDecl, &var) {
            Identifier& id = get_id(var);
            auto [_, was_inserted] = symbol_tree.push_variable_decl(id, node);

            if (!was_inserted && var.kind != VariableDecl::parameter) {
                report_error(node.source_token, "Redefinition of '{}'.", id.mangled_name);
            }
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
                }
                // else {
                //     type_decl.definition_body = first_occurence.definition_body;
                // }
            }
        }

        _default internal_panic("expected variable, got '{}'.", node.kind_name());
    }
}

// FIXME: should error if generic argument names are used for any declaration
// let T: i32 = 1321; // this would shadow the type parameter, which shouldn't be allowed
void Analyzer::check_for_generic_instantiation(ASTNode& node) {
    match(node) {
        holds(VariableDecl, &var_decl)  instantiate_or_replace_generic(*node.type.identifier);
        holds(FunctionCall, &func_call) instantiate_or_replace_generic(*func_call.identifier);
        holds(BlockScope,   &scope)     instantiate_or_replace_generic(*node.type.identifier);
        holds(FunctionDecl, &func)      instantiate_or_replace_generic(*node.type.identifier);
        // function declaration might have a instantiation in its return type, for example:
        // fn func() -> Pair[i32, f32] { ... }
        _default internal_error(node.source_token, "'{}' can't instantiate generics.", node.kind_name());
    }
}

// expects to receive the original generic declaration (ex: struct Foo[T, U])
// doesnt recursively instantiate anything. that is handled by the next time we find a generic
// replacing T with i32 is one step, passing that onto a member with Node[T] is later handled
void Analyzer::instantiate_or_replace_generic(ASTNode& id_node) {
    // TODO: check if we depend on a generic argument,
    // we can only instantiate if we have no generics left
    // let var: Pair[i32, T]; if T is generic, then we cant instantiate anything yet
    if (!is_branch<Identifier>(&id_node)) internal_error(id_node.source_token, "expected identifier, received '{}'.", id_node.name());
    auto& id = get<Identifier>(&id_node); // identifiers are tied to a possible instantiation

    if (!id.is_generic_wrapper()) return; // wasnt generic

    ASTNode* generic_declaration = symbol_tree.find_declaration(id).value_or(nullptr);
    if (!generic_declaration) {
        report_error(id_node.source_token, "failed generic instantiation: couldn't find base declaration for  {} '{}'.",
                     id.is_func_call() ? "function" : "struct", full_mangled_name(&id_node));
    }

    vec<ASTNode*> generic_identifiers;
    if (auto* func = get_if<FunctionDecl>(generic_declaration)) {
        generic_identifiers = get<Identifier>(func->identifier).generic_identifiers;
    } else if (is_branch<TypeDecl>(generic_declaration)) {
        generic_identifiers = get<Identifier>(generic_declaration->type.identifier).generic_identifiers;
    } else {
        internal_error(id_node.source_token, "expected valid generic declaration, got '{}'.", id_node.name());
    }

    // ---------------------------------
    // get rid of the old generic name, replace it with the actual declaration name
    // so that analyze() can work as if it was any other normal function/type name
    str instantiation_name = full_mangled_name(&id_node); // get the actual generic we want to instantiate
    id.name = instantiation_name; // we now have a concrete, non generic type instance
    id.mangled_name = instantiation_name; 
    // ---------------------------------
    if (symbol_tree.find_declaration(id)) { // this means this specific instantiation already exists, so we skip it
        return;                             // (find declaration uses the "id.name" to find a declaration)
    }
    auto* generic_copy = copy_ast(generic_declaration);
    // all identifiers have been solved to the original 'T', etc
    // that means we are ONLY missing identifier.declaration to be replaced correctly, everything else needs no changes
    match(*generic_copy) {
        // TODO: use copy_ast() and then iterate through our copy. then simply call add_declaration() with our new node
        holds(TypeDecl, &type_decl) {
        }
        holds(FunctionDecl, &func) {}
        // maybe check if we will have issues with postfix expressions that have generics
        _default internal_error(id_node.source_token, "tried to instantiate invalid generic declaration for '{}'.", id_node.name());
    }
}

[[nodiscard]] ASTNode* Analyzer::copy_ast(ASTNode* node) {
    ASTNode* new_node = push(*node);
    // end recursion for type.identifier.type.identifier
    if (new_node->type.identifier) new_node->type.identifier = copy_ast(new_node->type.identifier);

    match(*new_node) {
        holds(Identifier, &id) {
            // dont forget that all ".declaration" will break and identifiers wont map correctly to the same base node
            // unless you do this process before semantic analysis
            // many identifiers map to the same declaration node, so copying each one will
            // break that dependency between identifiers and declarations
            // NOTE: im not copying the declaration because it will be assigned by create_generic_declaration()
            // and it would be allocating a ton of unused nodes for no reason
            // if (id.declaration) id.declaration.value() = copy_ast(id.declaration.value());
            vec<ASTNode*> new_generic_ids {};
            for (auto* node : id.generic_identifiers) new_generic_ids.push_back(copy_ast(node));
            id.generic_identifiers = new_generic_ids;
        }
        holds(PrimaryExpr, &prim) {} // do nothing (nothing to copy)
        holds(UnaryExpr, &un) {
            if (un.expr) un.expr = copy_ast(un.expr.value());
        }
        holds(BinaryExpr, &bin) {
            bin.lhs = copy_ast(bin.lhs);
            bin.rhs = copy_ast(bin.rhs);
        }
        holds(PostfixExpr, &postfix) {
            vec<ASTNode*> new_nodes{};
            for (auto* node : postfix.nodes) new_nodes.push_back(copy_ast(node));
            postfix.nodes = new_nodes;
        }
        holds(VariableDecl, &var_decl) {
            var_decl.identifier = copy_ast(var_decl.identifier);
        }
        holds(FunctionDecl, &func) {
            func.identifier = copy_ast(func.identifier);
            vec<ASTNode*> new_params {};
            for (auto* node : func.parameters) new_params.push_back(copy_ast(node));
            func.parameters = new_params;
            if (func.body) func.body = copy_ast(func.body.value());
        }
        holds(FunctionCall, &func_call) {
            func_call.identifier = copy_ast(func_call.identifier);
            vec<ASTNode*> new_args {};
            for (auto* node : func_call.argument_list) new_args.push_back(copy_ast(node));
            func_call.argument_list = new_args;
        }
        holds(BlockScope, &scope) {
            vec<ASTNode*> new_nodes{};
            for (auto* node : scope.nodes) new_nodes.push_back(copy_ast(node));
            scope.nodes = new_nodes;
        }
        holds(NamespaceDecl, &nmspace) {
            nmspace.identifier = copy_ast(nmspace.identifier);
            vec<ASTNode*> new_nodes{};
            for (auto* node : nmspace.nodes) new_nodes.push_back(copy_ast(node));
            nmspace.nodes = new_nodes;
        }
        holds(TypeDecl, &type_decl) {
            if (type_decl.definition_body) {
                vec<ASTNode*> new_body {};
                for (auto* node : type_decl.definition_body.value()) new_body.push_back(copy_ast(node));
                type_decl.definition_body.value() = new_body;
            }
        }
        holds(IfStmt, &if_stmt) {
            if (if_stmt.condition) if_stmt.condition.value() = copy_ast(if_stmt.condition.value());
            if_stmt.body = copy_ast(if_stmt.body);
            if (if_stmt.else_stmt) if_stmt.else_stmt.value() = copy_ast(if_stmt.else_stmt.value());
        }
        holds(WhileStmt, &while_stmt) {
            while_stmt.condition = copy_ast(while_stmt.condition);
            while_stmt.body = copy_ast(while_stmt.body);
        }
        _default internal_error(node->source_token, "copy hasn't been implemented for '{}'", node->name());
    }
    
    return new_node;
}

