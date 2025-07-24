#include "semantic_analysis/analyzer.hpp"
#include "parser/ast_node.hpp"

void Analyzer::semantic_analysis(Scope& file_scope) {
    for (auto& node : file_scope.nodes) analyze(*node);
}

void Analyzer::analyze(ASTNode& node) {
    match(node) {
        holds(Primary&, prim) {
            switch (node.kind) {
                case NodeKind::integer:
                    node.type.name = "i32", node.type.kind = TypeKind::_i32; break;
                case NodeKind::floating_point:
                    node.type.name = "f64", node.type.kind = TypeKind::_f64; break;
                case NodeKind::str:
                    node.type.name = "str", node.type.kind = TypeKind::_str; break;
                case NodeKind::identifier:
                    prim.var_declaration = sym_table.find_node(std::get<str>(prim.value));
                    node.type = prim.var_declaration.value()->type;
                    break;
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
            }
        }
        holds(Unary&, un) {
            analyze(*un.expr);
            node.type = un.expr->type;
        }
        holds(Binary&, bin) {
            analyze(*bin.lhs);
            analyze(*bin.rhs);
            if (!is_compatible_t(bin.lhs->type, bin.rhs->type)) report_binary_error(node, bin);
        }
        holds(Variable&, var) {
            if (sym_table.depth == 0) node.kind = NodeKind::global_var_declaration;
            // TODO: resolve Type::_undetermined by finding the symbol associated to it
        }
        holds(Function&, func) {
            if (func.body) {
                sym_table.depth++;
                // add parameters to the body's depth
                sym_table.depth--;
                analyze(*func.body.value());
            }
        }
        holds(Scope&, scope) {
            sym_table.depth++;
            for (auto& node : scope.nodes) analyze(*node);
            sym_table.depth--;
        }
        _ INTERNAL_PANIC("semantic analysis missing for '{}'.", node.kind_name());
    }
}
