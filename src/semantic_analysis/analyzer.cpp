#include "semantic_analysis/analyzer.hpp"
#include "base_definitions/ast_node.hpp"
#include "utils/common_utils.hpp"

void Analyzer::semantic_analysis(ASTNode* file_root_node) {

    str prev_scope = push_scope("", *file_root_node, ScopeKind::Global);

    auto& file_scope = get<NamespaceDecl>(file_root_node);
    for (auto& node : file_scope.nodes) analyze(*node);

    pop_scope(prev_scope, *file_root_node);
}

void Analyzer::analyze(ASTNode& node) {

    match(node) {

        holds(Identifier, &id) {
            id.declaration = find_declaration(node);
            node.type = id.declaration.value()->type;
        }

        holds(PrimaryExpr, prim) {
            switch (prim.kind) {
                case integer:
                case floating_point:
                case str: break;
                // case PrimaryExpr::identifier:
                //     break;
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }

        holds(UnaryExpr, &un) {
            // TODO: add checks for '!' and such to only work on arithmetic types
            analyze(*un.expr);
            switch (un.kind) {
                case negate:
                case bitwise_not:
                    if(!is_arithmetic_t(un.expr->type)) {
                        report_error(node.source_token,
                                     "invalid type '{}' for unary expression.", get_name(un.expr->type));
                    }
                    break;
                case return_statement:
                case logic_not:
                default: 
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
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

        holds(VariableDecl, &var) {
            if (symbol_tree.curr_scope_kind == ScopeKind::Global) var.kind = global_var_declaration;
            add_to_scope(node);
        }

        holds(FunctionDecl, &func) {
            // FIXME: delete local structs and functions after closing a function/block scope
            // they are treated like normal variables in the case of functions
            add_to_scope(node);
            str prev_name = push_scope(get_name(func) + "()::", node, ScopeKind::Local);

            for (auto& param : func.parameters) analyze(*param);

            if (func.body) {
                func.body.value()->type = node.type; // NOTE: passing the function's type to the body
                auto& scope = get<BlockScope>(func.body.value());
                for (auto& node : scope.nodes) analyze(*node);
            }

            pop_scope(prev_name, node);
        }

        holds(BlockScope, const& scope) {
            // NOTE: consider taking the last node and passing that value to the scope (and returning it like rust)
            switch(scope.kind) {
                case compound_statement: {
                    str prev_scope = push_scope("", node, ScopeKind::Local);
                    for (auto& node : scope.nodes) analyze(*node);
                    pop_scope(prev_scope, node);
                    break;
                }
                case initializer_list: 
                    // TODO: we need to also solve the type for initializer lists
                    // they can either have a name `ASTNode {x = 123};` or have nothing `{x = 123};`
                    // in some contexts we might not be able to infer the type
                    // let var: ASTNode {.x = 213123};
                    // in most contexts, not providing a type is illegal (might be able to solve types anyway)
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
                    if (node.type.kind == Type::Undetermined) 
                case BlockScope::argument_list: break;
            }
        }

        holds(NamespaceDecl, const& nmspace_decl) {
            add_to_scope(node);

            str prev_scope = push_scope(get_name(nmspace_decl) + "::", node, ScopeKind::Global);
            for (auto& node : nmspace_decl.nodes) analyze(*node);
            pop_scope(prev_scope, node);
        }

        holds(TypeDecl, const& type_decl) {

            switch (type_decl.kind) {
                case struct_declaration: {
                    add_to_scope(node);

                    if(type_decl.definition_body) {
                        str prev_scope = push_scope(get_name(type_decl) + "{}::", node, ScopeKind::Local);
                        for (auto& node: type_decl.definition_body.value()) analyze(*node);
                        pop_scope(prev_scope, node);
                    }
                    break;
                }
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }
        
        _default INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
    }
}

void Analyzer::add_to_scope(ASTNode& node) {
    // NOTE: dont forget we cant redefine a outer namespaced identifier inside another namespace
    // namespace foo{ fn func() -> void;}
    // namespace bar {
    //     fn foo::func() -> void {}
    // }   ^ this is not allowed.

    match(node) {

        holds(FunctionDecl, &func) {
            // TODO: member function calls should be rewritten to take a pointer 
            // to an instance of the class they are defined in. the "this" implicit pointer in C++
            auto& id = get_id(func);
            id.mangled_name = symbol_tree.curr_scope_name + id.name;
            // symbol_tree.curr_scope_name = "";
            auto [node_iterator, was_inserted] = symbol_tree.push_function(id.name, node);

            if (!was_inserted && func.body) {
                auto& func_ = get<FunctionDecl>(node_iterator->second);
                if(func_.body) report_error(node.source_token, "Redefinition of '{}'.", id.name);
            }
        }

        holds(VariableDecl, &var) {
            auto& id = get_id(var);
            id.mangled_name = symbol_tree.curr_scope_name + id.name;
            auto [_, was_inserted] = symbol_tree.push_to_scope(id.name, node);

            if (!was_inserted) report_error(node.source_token, "Redefinition of '{}'.", id.name);
        }

        holds(NamespaceDecl, const& nmspace_decl) {
            auto& id = get_id(nmspace_decl);
            id.mangled_name = symbol_tree.curr_scope_name + id.name;
            auto [node_iterator, was_inserted] = symbol_tree.push_named_scope(id.name, node);

            if (!was_inserted && !std::holds_alternative<NamespaceDecl>(node_iterator->second->branch)) {
                report_error(node.source_token, "Redefinition of '{}' as a different kind of symbol.", id.name);
            }
        }

        holds(TypeDecl, &type_decl) {
            auto& id = get_id(type_decl);
            id.mangled_name = symbol_tree.curr_scope_name + id.name;
            auto [node_iterator, was_inserted] = symbol_tree.push_named_scope(id.name, node);

            // TODO: add each member of the struct to the type decl info.
            // we need to store offsets and the names of the members (for solving lookups later)
            // and also the conversion from normal function name -> mangled function name
            if (!was_inserted) {
                auto& t_decl = get_if<TypeDecl>(node_iterator->second) 
                               or_error(node.source_token,
                                        "Redefinition of '{}' as a different kind of symbol.", id.name);

                if (type_decl.definition_body && t_decl.definition_body) {
                    report_error(node.source_token, "Redefinition of '{}'.", id.name);
                }
            }
        }
        _default { INTERNAL_PANIC("expected variable, got '{}'.", node.kind_name()); }
    }
}
