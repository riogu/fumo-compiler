#include "semantic_analysis/analyzer.hpp"
#include "base_definitions/ast_node.hpp"

void Analyzer::semantic_analysis(ASTNode* file_root_node) {

    str prev_scope = push_scope("", *file_root_node, ScopeKind::Global);

    auto& file_scope = get<NamespaceDecl>(file_root_node);
    for (auto& node : file_scope.nodes) analyze(*node);

    pop_scope(prev_scope, *file_root_node);
}

void Analyzer::analyze(ASTNode& node) {

    match(node) {

        holds(Identifier&, id) {
            id.declaration = find_declaration(node);
            node.type = id.declaration.value()->type;
        }

        holds(PrimaryExpr&, prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:        node.type.name = "i32"; node.type.kind = Type::i32_; break;
                case PrimaryExpr::floating_point: node.type.name = "f64"; node.type.kind = Type::f64_; break;
                case PrimaryExpr::str:            node.type.name = "str"; node.type.kind = Type::str_; break;
                // case PrimaryExpr::identifier:
                //     break;
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }

        holds(UnaryExpr&, un) {
            // TODO: add checks for '!' and such to only work on arithmetic types
            analyze(*un.expr);
            switch (un.kind) {
                case UnaryExpr::negate:
                case UnaryExpr::bitwise_not:
                    if(!is_arithmetic_t(un.expr->type)) {
                        report_error(node.source_token,
                                     "invalid type '{}' for unary expression.", un.expr->type.name);
                    }
                    break;
                case UnaryExpr::return_statement:
                case UnaryExpr::logic_not:
                default: 
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
            node.type = un.expr->type;
        }

        holds(BinaryExpr&, bin) {
            analyze(*bin.lhs);
            analyze(*bin.rhs);

            if (bin.kind == BinaryExpr::assignment && bin.lhs->type.kind == Type::Undetermined) {
                bin.lhs->type = bin.rhs->type;
            }
            // if (!is_compatible_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
            node.type = bin.lhs->type;
        }

        holds(VariableDecl&, var) {
            if (symbol_tree.curr_scope_kind == ScopeKind::Global) var.kind = VariableDecl::global_var_declaration;
            add_to_scope(node);
        }

        holds(FunctionDecl&, func) {
            // FIXME: delete local structs and functions after closing a function/block scope
            // they are treated like normal variables in the case of functions
            add_to_scope(node);
            str prev_name = push_scope(get_id(func).name + "()::", node, ScopeKind::Local);

            for (auto& param : func.parameters) analyze(*param);

            if (func.body) {
                func.body.value()->type = node.type; // NOTE: passing the function's type to the body
                auto& scope = get<BlockScope>(func.body.value());
                for (auto& node : scope.nodes) analyze(*node);
            }

            pop_scope(prev_name, node);
        }

        holds(const BlockScope&, scope) {
            // NOTE: consider taking the last node and passing that value to the scope (and returning it like rust)
            switch(scope.kind) {
                case BlockScope::compound_statement: {
                    str prev_scope = push_scope("", node, ScopeKind::Local);
                    for (auto& node : scope.nodes) analyze(*node);
                    pop_scope(prev_scope, node);
                    break;
                }
                case BlockScope::initializer_list: 
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

        holds(const NamespaceDecl&, nmspace_decl) {
            add_to_scope(node);

            str prev_scope = push_scope(get_id(nmspace_decl).name + "::", node, ScopeKind::Global);
            for (auto& node : nmspace_decl.nodes) analyze(*node);
            pop_scope(prev_scope, node);
        }

        holds(const TypeDecl&, type_decl) {

            switch (type_decl.kind) {
                case TypeDecl::struct_declaration: {
                    add_to_scope(node);

                    if(type_decl.definition_body) {
                        str prev_scope = push_scope(get_id(type_decl).name + "{}::", node, ScopeKind::Local);
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

        holds(FunctionDecl&, func) {
            // TODO: member function calls should be rewritten to take a pointer 
            // to an instance of the class they are defined in. the "this" implicit pointer in C++
            get_id(func).mangled_name = symbol_tree.curr_scope_name + get_id(func).name;
            // symbol_tree.curr_scope_name = "";
            auto [node_iterator, was_inserted] = symbol_tree.push_function(get_id(func).name, node);

            if (!was_inserted && func.body) {
                match(*node_iterator->second) {
                    holds(FunctionDecl&, func_) 
                        if(func_.body) report_error(node.source_token, "Redefinition of '{}'.", get_id(func).name);
                    _default INTERNAL_PANIC("expected function, got '{}'.", node.kind_name()); 
                }
            }
        }

        holds(VariableDecl&, var) {
            get_id(var).mangled_name = symbol_tree.curr_scope_name + get_id(var).name;
            auto [_, was_inserted] = symbol_tree.push_to_scope(get_id(var).name, node);

            if (!was_inserted) report_error(node.source_token, "Redefinition of '{}'.", get_id(var).name);
        }

        holds(const NamespaceDecl&, nmspace_decl) {
            get_id(nmspace_decl).mangled_name = symbol_tree.curr_scope_name + get_id(nmspace_decl).name;
            auto [node_iterator, was_inserted] = symbol_tree.push_named_scope(get_id(nmspace_decl).name, node);

            if (!was_inserted && !std::holds_alternative<NamespaceDecl>(node_iterator->second->branch)) {
                report_error(node.source_token, "Redefinition of '{}' as a different kind of symbol.", get_id(nmspace_decl).name);
            }
        }

        holds(TypeDecl&, type_decl) {
            get_id(type_decl).mangled_name = symbol_tree.curr_scope_name + get_id(type_decl).name;
            auto [node_iterator, was_inserted] = symbol_tree.push_named_scope(get_id(type_decl).name, node);

            // TODO: add each member of the struct to the type decl info.
            // we need to store offsets and the names of the members (for solving lookups later)
            // and also the conversion from normal function name -> mangled function name

            if (!was_inserted) {
                match(*node_iterator->second) {
                    holds(TypeDecl&, t_decl) {
                        if (type_decl.definition_body && t_decl.definition_body) {
                            report_error(node.source_token, "Redefinition of '{}'.", get_id(type_decl).name);
                        }
                    }
                    _default  report_error(node.source_token,
                                           "Redefinition of '{}' as a different kind of symbol.", get_id(type_decl).name);
                }
            }
        }
        _default { INTERNAL_PANIC("expected variable, got '{}'.", node.kind_name()); }
    }
}
