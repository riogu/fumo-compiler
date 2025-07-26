#include "semantic_analysis/analyzer.hpp"
#include "parser/ast_node.hpp"
#include "type_system/type_cheker.hpp"

void Analyzer::semantic_analysis(BlockScope& file_scope) {
    for (auto& node : file_scope.nodes) analyze(*node);
}

void Analyzer::analyze(ASTNode& node) {
    
    match(node) {
        holds(PrimaryExpr&, prim) {
            switch (node.kind) {
                case NodeKind::integer:
                    node.type.name = "i32", node.type.kind = TypeKind::i32_; break;
                case NodeKind::floating_point:
                    node.type.name = "f64", node.type.kind = TypeKind::f64_; break;
                case NodeKind::str:
                    node.type.name = "str", node.type.kind = TypeKind::str_; break;
                case NodeKind::identifier:
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
            if (symbol_tree.depth == 0) node.kind = NodeKind::global_var_declaration;

            if (var.value) analyze(*var.value.value());
            if (node.type.kind == TypeKind::Undetermined)
                report_error(node.source_token, "declaring a variable with deduced type requires an initializer.");
            if (node.type.kind == TypeKind::struct_) add_namespace(&node);
            if (node.type.kind == TypeKind::enum_)   add_namespace(&node);

            add_node(&node);
        }
        holds(FunctionDecl&, func) {
            symbol_tree.depth++;
            add_node(&node); // TODO: add parameters to the body's symbol_tree.depth
            symbol_tree.depth--;
            if (func.body) {
                func.body.value()->type = node.type;
                analyze(*func.body.value());
            }
        }
        holds(BlockScope&, scope) {
            // NOTE: consider taking the last node and passing that value to the scope (and returning it like rust)
            switch(node.kind) {
                case NodeKind::initializer_list:
                    break;
                case NodeKind::compound_statement:
                    symbol_tree.depth++;
                    for (auto& node : scope.nodes) analyze(*node);
                    symbol_tree.depth--;
                    break;
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }
        _ INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
    }
}

void Analyzer::report_binary_error(const ASTNode& node, const BinaryExpr& bin) {
    switch (node.kind) {
      case NodeKind::add:       case NodeKind::sub:
      case NodeKind::multiply:  case NodeKind::divide:
      case NodeKind::equal:     case NodeKind::not_equal:
      case NodeKind::less_than: case NodeKind::less_equals:
          report_error(node.source_token,
                       "invalid operands to binary expression '{}' and '{}'.",
                        bin.lhs->type.name, bin.rhs->type.name);
      case NodeKind::assignment:
          report_error(node.source_token,
                       "assigning to '{}'from incompatible type '{}'.",
                        bin.lhs->type.name, bin.rhs->type.name);
        default:
            INTERNAL_PANIC("expected binary node for error, got '{}'.", node.kind_name());
    }
}
