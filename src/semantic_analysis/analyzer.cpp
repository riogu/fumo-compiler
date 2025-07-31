#include "semantic_analysis/analyzer.hpp"

void Analyzer::semantic_analysis(ASTNode* file_root_node) {

    symbol_tree.push_scope("", ScopeKind::Namespace);

    auto& file_scope = get<NamespaceDecl>(file_root_node);
    for (auto& node : file_scope.nodes) analyze(*node);

    symbol_tree.pop_scope();
}

void Analyzer::analyze(ASTNode& node) {

    match(node) {

        holds(Identifier, &id) {
            id.declaration = symbol_tree.find_declaration(id);
            if (id.declaration) {
                node.type = id.declaration.value()->type;
            } else {
                // report_error(node.source_token, "use of undeclared identifier '{}'", id.name);
            }
        }

        holds(PrimaryExpr, prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:
                case PrimaryExpr::floating_point:
                case PrimaryExpr::str: break;
                default: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }

        holds(UnaryExpr, &un) {
            // TODO: add checks for '!' and such to only work on arithmetic types
            analyze(*un.expr);
            switch (un.kind) {
                case UnaryExpr::negate:
                case UnaryExpr::bitwise_not:
                    if (!is_arithmetic_t(un.expr->type)) {
                        report_error(node.source_token,
                                     "invalid type '{}' for unary expression.",
                                     get_name(un.expr->type));
                    }
                    break;
                case UnaryExpr::return_statement:
                case UnaryExpr::logic_not:
                default: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
            node.type = un.expr->type;
        }

        holds(BinaryExpr, &bin) {
            analyze(*bin.lhs);
            analyze(*bin.rhs);

            if (bin.kind == BinaryExpr::assignment && bin.lhs->type.kind == Type::Undetermined) {
                bin.lhs->type = bin.rhs->type;
            }
            // if (!is_compatible_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
            node.type = bin.lhs->type;
        }

        holds(VariableDecl, &var) { // doesnt handle determining its own type, assignment does that
            analyze(*var.identifier);
            if (symbol_tree.curr_scope_kind == ScopeKind::Namespace) var.kind = VariableDecl::global_var_declaration;
            add_declaration(node);
        }

        holds(FunctionDecl, &func) {
            analyze(*func.identifier);
            add_declaration(node);
            symbol_tree.push_scope(get_name(func) + "()::", ScopeKind::CompoundStatement);

            for (auto& param : func.parameters) {
                if (param->type.kind == Type::Undetermined) analyze(*param->type.identifier);
                param->type = param->type.identifier->type;
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
                case BlockScope::compound_statement: {
                    symbol_tree.push_scope("::", ScopeKind::CompoundStatement);
                    for (auto& node : scope.nodes) analyze(*node);
                    symbol_tree.pop_scope();
                    break;
                }
                case BlockScope::initializer_list:
                    if (node.type.kind == Type::Undetermined) analyze(*node.type.identifier);
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
                case BlockScope::argument_list: break;
            }
        }

        holds(NamespaceDecl, const& nmspace_decl) {
            analyze(*nmspace_decl.identifier);
            add_declaration(node);
            symbol_tree.push_scope(get_name(nmspace_decl) + "::", ScopeKind::Namespace);
            for (auto& node : nmspace_decl.nodes) analyze(*node);
            symbol_tree.pop_scope();
        }

        holds(TypeDecl, const& type_decl) {
            analyze(*type_decl.identifier);

            switch (type_decl.kind) {
                case TypeDecl::struct_declaration: {
                    add_declaration(node);

                    if (type_decl.definition_body) {
                        symbol_tree.push_scope(get_name(type_decl) + "::", ScopeKind::TypeBody);
                        for (auto& node : type_decl.definition_body.value()) analyze(*node);
                        symbol_tree.pop_scope();
                    }
                    break;
                }
                default: INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }

        _default INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
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
            // TODO: member function calls should be rewritten to take a pointer
            // to an instance of the class they are defined in. the "this" implicit pointer in C++
            Identifier& id = get_id(func);
            id.declaration = &node;
            auto [node_iterator, was_inserted] = symbol_tree.push_function_decl(id.name, node);

            if (!was_inserted && func.body) {
                auto& func_ = get<FunctionDecl>(node_iterator->second);
                if (func_.body) report_error(node.source_token, "Redefinition of '{}'.", id.name);
            }
        }

        holds(VariableDecl, &var) {
            Identifier& id = get_id(var);
            id.declaration = &node;
            auto [_, was_inserted] = symbol_tree.push_variable_decl(id.name, node);

            if (!was_inserted) report_error(node.source_token, "Redefinition of '{}'.", id.name);
        }

        holds(NamespaceDecl, &nmspace_decl) {
            Identifier& id = get_id(nmspace_decl);
            id.declaration = &node;
            if (symbol_tree.type_decls.contains(id.name))
                report_error(node.source_token, "Redefinition of '{}' as a different kind of symbol.", id.name);
            symbol_tree.push_namespace_decl(id.name, node);
        }

        holds(TypeDecl, &type_decl) {
            Identifier& id = get_id(type_decl);
            auto [node_iterator, was_inserted] = symbol_tree.push_type_decl(id.name, node);

            // TODO: add each member of the struct to the type decl info.
            // we need to store offsets and the names of the members (for solving lookups later)
            // and also the conversion from normal function name -> mangled function name

            if (!was_inserted) {
                auto& t_decl = get_if<TypeDecl>(node_iterator->second)
                               or_error(node.source_token, "Redefinition of '{}' as a different kind of symbol.", id.name);

                if (type_decl.definition_body && t_decl.definition_body) {
                    report_error(node.source_token, "Redefinition of '{}'.", id.name);
                }
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
//    struct foo {
//         fn somefunc() -> void;
//
//         fn another() -> void {
//             somefunc();
//             struct bar {};
//             "huh::foo::another()::bar"
//             {
//             "huh::foo::another()::::somefunc"();
//             somefunc();
//             }
//         }
//    };
// }
#define find_value(key, map) (const auto& iter = map.find(key); iter != map.end())

[[nodiscard]] Opt<ASTNode*> SymbolTableStack::find_declaration(Identifier& id) {
    id.mangled_name = curr_scope_name + id.name;

    switch (id.kind) {

        case Identifier::unsolved_type_name:
            if find_value(id.name, type_decls) return iter->second;
            break;

        // TODO: function calls should not get mangled by the local function ever
        case Identifier::unsolved_func_call_name:
            switch (curr_scope_kind) {
                case ScopeKind::TypeBody: {
                    for (const auto& scope : scope_stack | std::views::reverse) {
                    }
                    break;
                }
                case ScopeKind::CompoundStatement:
                    if find_value(id.name, function_decls) return iter->second;
                    
                case ScopeKind::Namespace: INTERNAL_PANIC("function call can't be global.");
            }
            break;

        case Identifier::unsolved_var_name:
            // std::cerr <<  scope.name + id.name + " | ";
            for (const auto& scope : scope_stack | std::views::reverse) {
                if find_value (scope.name + id.name, local_variable_decls) return iter->second;
                if find_value (scope.name + id.name, global_variable_decls) return iter->second;
            }
            break;

        case Identifier::declaration_name: return id.declaration;
        case Identifier::unknown_name:
            INTERNAL_PANIC("forgot to set identifier name kind for {}.", id.mangled_name);
    }

    return std::nullopt;
}

