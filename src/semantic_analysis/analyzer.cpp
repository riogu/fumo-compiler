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
                    node.type.name = "i32", node.type.kind = TypeKind::_i32; break;
                case NodeKind::floating_point:
                    node.type.name = "f64", node.type.kind = TypeKind::_f64; break;
                case NodeKind::str:
                    node.type.name = "str", node.type.kind = TypeKind::_str; break;
                case NodeKind::identifier:
                    prim.var_declaration = symbol_tree.find_node(std::get<str>(prim.value));
                    node.type = prim.var_declaration.value()->type;
                    break;
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }
        holds(UnaryExpr&, un) {
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
            // TODO: resolve Type::_undetermined by finding the symbol associated to it
        }
        holds(FunctionDecl&, func) {
            if (func.body) {
                symbol_tree.depth++;
                // add parameters to the body's depth
                symbol_tree.depth--;
                analyze(*func.body.value());
            }
        }
        holds(BlockScope&, scope) {
            symbol_tree.depth++;
            for (auto& node : scope.nodes) analyze(*node);
            symbol_tree.depth--;
        }
        _ INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
    }
}
