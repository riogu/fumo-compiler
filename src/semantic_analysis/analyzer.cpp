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
            if (symbol_tree.depth == 0) node.kind = ASTNode::global_var_declaration;

            if (var.value) analyze(*var.value.value());
            if (node.type.kind == Type::Undetermined)
                report_error(node.source_token, "declaring a variable with deduced type requires an initializer.");
            if (node.type.kind == Type::struct_) add_namespace(&node);
            if (node.type.kind == Type::enum_)   add_namespace(&node);

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
                case ASTNode::initializer_list:
                    break;
                case ASTNode::compound_statement:
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
      case ASTNode::add:       case ASTNode::sub:
      case ASTNode::multiply:  case ASTNode::divide:
      case ASTNode::equal:     case ASTNode::not_equal:
      case ASTNode::less_than: case ASTNode::less_equals:
          report_error(node.source_token,
                       "invalid operands to binary expression: '{}' {} '{}'.",
                        bin.lhs->type.name, node.source_token.to_str(), bin.rhs->type.name);
      case ASTNode::assignment:
          report_error(node.source_token,
                       "assigning to '{}'from incompatible type '{}'.",
                        bin.lhs->type.name, bin.rhs->type.name);
        default:
            INTERNAL_PANIC("expected binary node for error, got '{}'.", node.kind_name());
    }
}
