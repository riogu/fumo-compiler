#include "semantic_analysis/analyzer.hpp"

void Analyzer::semantic_analysis(NamedScope& file_scope) {
    for (auto& node : file_scope.nodes) analyze(*node);
}
void Analyzer::analyze(ASTNode& node) {
    match(node) {
        holds(PrimaryExpr&, prim) {
            switch (node.kind) {
                case ASTNode::integer:
                    node.type.name = "i32", node.type.kind = Type::i32_; break;
                case ASTNode::floating_point:
                    node.type.name = "f64", node.type.kind = Type::f64_; break;
                case ASTNode::str:
                    node.type.name = "str", node.type.kind = Type::str_; break;
                case ASTNode::identifier:
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
            node.type = un.expr->type;
        }
        holds(BinaryExpr&, bin) {
            analyze(*bin.lhs);
            analyze(*bin.rhs);
            if (!is_compatible_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
        }
        holds(VariableDecl&, var) {
            if (symbol_tree.symbols_to_nodes.size() == 1) node.kind = ASTNode::global_var_declaration;

            if (var.value) analyze(*var.value.value());
            if (node.type.kind == Type::Undetermined)
                report_error(node.source_token, "declaring a variable with deduced type requires an initializer.");
            if (node.type.kind == Type::struct_) push_named_scope(node); // TODO: will become global not local (fix)
            if (node.type.kind == Type::enum_)   push_named_scope(node); // TODO: will become global not local (fix)

            push_to_scope(node);
        }
        holds(FunctionDecl&, func) {
            // push_named_scope(node); // TODO: add parameters to the body's symbol_tree.depth
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
            switch(node.kind) {
                case ASTNode::compound_statement:
                    open_scope();
                    for (auto& node : scope.nodes) analyze(*node);
                    close_scope();
                    break;
                case ASTNode::initializer_list: 
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }

        holds(NamedScope&, n_scope) push_named_scope(node);
        
        _default INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
    }
}

void Analyzer::report_binary_error(const ASTNode& node, const BinaryExpr& bin) {
    switch (node.kind) {
      case ASTNode::add:       case ASTNode::sub:
      case ASTNode::multiply:  case ASTNode::divide:
      case ASTNode::equal:     case ASTNode::not_equal:
      case ASTNode::less_than: case ASTNode::less_equals:
          report_error(node.source_token, "invalid operands to binary expression: '{}' {} '{}'.",
                       bin.lhs->type.name, node.source_token.to_str(), bin.rhs->type.name);
      case ASTNode::assignment:
          report_error(node.source_token,"assigning to '{}'from incompatible type '{}'.",
                       bin.lhs->type.name, bin.rhs->type.name);
        default:
            INTERNAL_PANIC("expected binary node for error, got '{}'.", node.kind_name());
    }
}


void Analyzer::push_named_scope(ASTNode& node) {
    match(node) {
        holds(FunctionDecl&, func) {
            auto [node_iterator, was_inserted] = symbol_tree.push_named_scope(func.name, node);
            if (!was_inserted && func.body) {
                match(*node_iterator->second) {
                    holds(FunctionDecl&, _func) 
                        if(_func.body) report_error(node.source_token, "Redefinition of '{}'.", _func.name);
                    _default { INTERNAL_PANIC("expected function, got '{}'.", node.kind_name()); }
                }
            }
        }
        holds(NamedScope&, scope) {
            switch(node.kind) {
                case ASTNode::struct_declaration:    INTERNAL_PANIC("not implemented.");
                case ASTNode::enum_declaration:      INTERNAL_PANIC("not implemented.");
                case ASTNode::namespace_declaration: INTERNAL_PANIC("not implemented."); 
                default:
                    INTERNAL_PANIC("expected named scope declaration, got '{}'.", node.kind_name());
            }
        }
        _default {}
    }
}

void Analyzer::push_to_scope(ASTNode& node) {
    match(node) {
        holds(VariableDecl&, var) {
            auto [_, was_inserted] = symbol_tree.push_to_scope(var.name, node);
            if (!was_inserted) report_error(node.source_token, "Redefinition of '{}'.", var.name);
        }
        _default { INTERNAL_PANIC("expected block scope, got '{}'.", node.kind_name()); }
    }
}

[[nodiscard]] ASTNode* Analyzer::find_node(std::string_view var_name) {
    INTERNAL_PANIC("not implemented.");
}

[[nodiscard]] constexpr bool Analyzer::is_compatible_t(const Type& a, const Type& b) {
    PANIC("not implemented");
}
