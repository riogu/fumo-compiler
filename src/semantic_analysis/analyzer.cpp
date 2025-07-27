#include "semantic_analysis/analyzer.hpp"
#include "base_definitions/ast_node.hpp"

void Analyzer::semantic_analysis(ASTNode* file_root_node) {
    symbol_tree.symbols_to_nodes.push_back({});
    match(*file_root_node) {
        holds(const NamedScope&, file_scope) for (auto& node : file_scope.nodes) analyze(*node);
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
                    // if(!is_arithmetic_t(un.expr->type)) {
                    //     report_error(node.source_token,
                    //                  "invalid type '{}' for unary expression.", un.expr->type.name);
                    // }
                    break;
                case UnaryExpr::return_statement:
                case UnaryExpr::logic_not:
                default: 
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
            node.type = un.expr->type;
        }
        holds(BinaryExpr&, bin) {
            // TODO: add type inference to BinaryExpr::assignment
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

            if (node.type.kind == Type::struct_) push_named_scope(node); // TODO: will become global not local (fix)
            if (node.type.kind == Type::enum_)   push_named_scope(node); // TODO: will become global not local (fix)

            push_to_scope(node);
        }
        holds(FunctionDecl&, func) {
            push_named_scope(node); // TODO: add parameters to the body's symbol_tree.depth
            open_scope();
            if (func.body) {
                func.body.value()->type = node.type;
                match(*func.body.value()) {
                    holds(BlockScope&, scope) for (auto& node_ : scope.nodes) analyze(*node_);
                    _default { INTERNAL_PANIC("expected compound statement, got '{}'.", node.kind_name()); }
                }
            }
            close_scope();
        }
        holds(BlockScope&, scope) {
            // NOTE: consider taking the last node and passing that value to the scope (and returning it like rust)
            switch(scope.kind) {
                case BlockScope::compound_statement:
                    open_scope();
                    for (auto& node : scope.nodes) analyze(*node);
                    close_scope();
                    break;
                case BlockScope::initializer_list: 
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }

        holds(NamedScope&, n_scope) push_named_scope(node);
        
        _default INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
    }
}

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


void Analyzer::push_named_scope(ASTNode& node) {
    // TODO: resolve the identifiers by replacing their names with the internal mangling rules
    //   namespace foo  {struct bar {};} => struct "foo::bar"    {};
    //   struct    foo  {struct bar {};} => struct "foo{}::bar"  {};
    //   fn f() -> void {struct bar {};} => struct "foo()::bar"  {};
    //   struct    foo  {fn func()->void;} => fn "foo{}::func"(this: foo*) -> void;
    match(node) {
        holds(FunctionDecl&, func) {
            // TODO: member function calls should be rewritten to take
            // a pointer to an instance of the class they are defined in.
            // the "this" implicit pointer in C++
            auto [node_iterator, was_inserted] = symbol_tree.push_named_scope(func.name, node);
            if (!was_inserted && func.body) {
                match(*node_iterator->second) {
                    holds(FunctionDecl&, func_) 
                        if(func_.body) report_error(node.source_token, "Redefinition of '{}'.", func_.name);
                    _default { INTERNAL_PANIC("expected function, got '{}'.", node.kind_name()); }
                }
            }
        }
        holds(NamedScope&, scope) {
            switch(scope.kind) {
                case NamedScope::struct_declaration:    INTERNAL_PANIC("not implemented.");
                case NamedScope::enum_declaration:      INTERNAL_PANIC("not implemented.");
                case NamedScope::namespace_declaration: INTERNAL_PANIC("not implemented."); 
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }
        _default { INTERNAL_PANIC("expected named scope, got '{}'.", node.kind_name()); }
    }
}

void Analyzer::push_to_scope(ASTNode& node) {
    match(node) {
        holds(VariableDecl&, var) {
            auto [_, was_inserted] = symbol_tree.push_to_scope(var.name, node);
            if (!was_inserted) report_error(node.source_token, "Redefinition of '{}'.", var.name);
        }
        _default { INTERNAL_PANIC("expected variable, got '{}'.", node.kind_name()); }
    }
}

[[nodiscard]] ASTNode* Analyzer::find_node(std::string_view var_name)                { INTERNAL_PANIC("not implemented."); }
[[nodiscard]] constexpr bool Analyzer::is_compatible_t(const Type& a, const Type& b) { INTERNAL_PANIC("not implemented."); }
[[nodiscard]] constexpr bool Analyzer::is_arithmetic_t(const Type& a) { INTERNAL_PANIC("not implemented."); }
