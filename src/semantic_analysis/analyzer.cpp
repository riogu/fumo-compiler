#include "semantic_analysis/analyzer.hpp"
#include "base_definitions/ast_node.hpp"


void Analyzer::semantic_analysis(ASTNode* file_root_node) {
    // symbol_tree.symbols_to_nodes.push_back({});
    match(*file_root_node) {
        holds(const NamespaceDecl&, file_scope) for (auto& node : file_scope.nodes) analyze(*node);
        _default INTERNAL_PANIC("expected file scope, got '{}'.", file_root_node->kind_name());
    }
}

void Analyzer::analyze(ASTNode& node) {

    match(node) {

        holds(PrimaryExpr&, prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:        node.type.name = "i32"; node.type.kind = Type::i32_; break;
                case PrimaryExpr::floating_point: node.type.name = "f64"; node.type.kind = Type::f64_; break;
                case PrimaryExpr::str:            node.type.name = "str"; node.type.kind = Type::str_; break;
                case PrimaryExpr::identifier:
                    prim.var_declaration = find_node(std::get<str>(prim.value));
                    node.type = prim.var_declaration.value()->type;
                    break;
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
            if (symbol_tree.symbols_to_nodes.size() == 1) var.kind = VariableDecl::global_var_declaration;

            // TODO: will become global not local (fix)
            if(node.type.declaration) analyze(*node.type.declaration.value()); 

            add_to_scope(node);
        }

        holds(FunctionDecl&, func) {
            // FIXME: delete local structs and functions after closing a function/block scope
            // they are treated like normal variables in the case of functions
            add_to_scope(node); 

            if (func.body) {
                // TODO: add parameters to the body's symbol_tree.depth
                str prev_name = push_scope(func.name + "()::", node);

                func.body.value()->type = node.type;
                match(*func.body.value()) {
                    holds(BlockScope&, scope) for (auto& node_ : scope.nodes) analyze(*node_);
                    _default { INTERNAL_PANIC("expected compound statement, got '{}'.", node.kind_name()); }
                }

                pop_scope(prev_name, node);
                // should cleanup all local functions and structs from the declaration list
                // put them in a separate map for codegen, but wont show up in name lookups
            }
        }

        holds(const BlockScope&, scope) {
            // NOTE: consider taking the last node and passing that value to the scope (and returning it like rust)
            switch(scope.kind) {
                case BlockScope::compound_statement: {
                    str prev_scope = push_scope("", node);
                    // change the names of struct/function declarations that happen inside of functions
                    for (auto& node : scope.nodes) analyze(*node);
                    pop_scope(prev_scope, node);
                    break;
                }
                case BlockScope::initializer_list: 
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }

        holds(const NamespaceDecl&, nmspace_decl) {
        // TODO: (07-29) write tests to try out the mangled names for namespaces/structs and functions with inner scopes
            switch (nmspace_decl.kind) {
                case NamespaceDecl::namespace_declaration:  {
                    str prev_scope = push_scope(nmspace_decl.name + "::", node);
                    for (auto& node : nmspace_decl.nodes) analyze(*node);
                    pop_scope(prev_scope, node);
                    break;
                }
                case NamespaceDecl::translation_unit:
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
            
        }
        holds(const TypeDecl&, type_decl) {
            // struct foo {let var: i32; let b: f64;};
            switch (type_decl.kind) {
                case TypeDecl::struct_declaration: {

                    add_to_scope(node);
                    str prev_scope = push_scope(type_decl.name + "{}::", node);
                    if(type_decl.definition) for (auto& node: type_decl.definition.value()) analyze(*node);
                    pop_scope(prev_scope, node);

                    // symbol_tree.curr_scope_name  = temp;
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

    match(node) {
        holds(FunctionDecl&, func) {
            node.mangled_name = symbol_tree.curr_scope_name + func.name;
            // TODO: member function calls should be rewritten to take a pointer 
            // to an instance of the class they are defined in. the "this" implicit pointer in C++
            auto [node_iterator, was_inserted] = symbol_tree.push_function(func.name, node);
            if (!was_inserted && func.body) {
                match(*node_iterator->second) {
                    holds(FunctionDecl&, func_) 
                        if(func_.body) report_error(node.source_token, "Redefinition of '{}'.", func_.name);
                    _default { INTERNAL_PANIC("expected function, got '{}'.", node.kind_name()); }
                }
            }
        }
        holds(VariableDecl&, var) {
            node.mangled_name = symbol_tree.curr_scope_name + var.name;
            auto [_, was_inserted] = symbol_tree.push_to_scope(var.name, node);
            if (!was_inserted) report_error(node.source_token, "Redefinition of '{}'.", var.name);
        }
        holds(TypeDecl&, type_decl) {
            // FIXME: check redefinitions here
            node.mangled_name = symbol_tree.curr_scope_name + type_decl.name;
        }
        _default { INTERNAL_PANIC("expected variable, got '{}'.", node.kind_name()); }
    }
}

[[nodiscard]] str Analyzer::push_scope(str name, ASTNode& node) {
    str prev_scope_name = symbol_tree.curr_scope_name;
    symbol_tree.curr_scope_name += name;
    symbol_tree.symbols_to_nodes.push_back(std::map<str, ASTNode*> {});
    return prev_scope_name;
}

// 

void Analyzer::pop_scope(str prev_scope_name, ASTNode& node) {
    symbol_tree.curr_scope_name = prev_scope_name;
    symbol_tree.symbols_to_nodes.pop_back();
}

[[nodiscard]] ASTNode* Analyzer::find_node(std::string_view var_name) {

    INTERNAL_PANIC("not implemented.");
}
[[nodiscard]] constexpr bool Analyzer::is_compatible_t(const Type& a, const Type& b) { INTERNAL_PANIC("not implemented."); }
[[nodiscard]] constexpr bool Analyzer::is_arithmetic_t(const Type& a)                { INTERNAL_PANIC("not implemented."); }

void Analyzer::report_binary_error(const ASTNode& node, const BinaryExpr& bin) {
    switch (bin.kind) {
        case BinaryExpr::add:       case BinaryExpr::sub:
        case BinaryExpr::multiply:  case BinaryExpr::divide:
        case BinaryExpr::equal:     case BinaryExpr::not_equal:
        case BinaryExpr::less_than: case BinaryExpr::less_equals:
            report_error(node.source_token, "invalid operands to binary expression: '{}' {} '{}'.",
                         bin.lhs->type.name, node.source_token.to_str(), bin.rhs->type.name);
        case BinaryExpr::assignment:
            report_error(node.source_token,"assigning to '{}'from incompatible type '{}'.",
                         bin.lhs->type.name, bin.rhs->type.name);
        default:
            INTERNAL_PANIC("expected binary node for error, got '{}'.", node.kind_name());
    }
}
