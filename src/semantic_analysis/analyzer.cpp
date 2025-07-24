#include "semantic_analysis/analyzer.hpp"

void Analyzer::semantic_analysis(Scope& file_scope) {
    for (auto& node : file_scope.nodes) analyze(*node);
}

void Analyzer::analyze(ASTNode& node) {
    match(node) {
        holds(Primary&, prim) {
            switch (node.kind) {
                case NodeKind::integer:
                case NodeKind::floating_point:
                case NodeKind::str:
                case NodeKind::identifier: {
                    // ASTNode* var_decl = sym_table.find_node(std::get<str>(prim.value));
                }
                default:
                    INTERNAL_PANIC("semantic analysis missing for '{}'", node.kind_name());
            }
        }
        holds(Unary&, un) {}
        holds(Binary&, bin) {}
        holds(Variable&, var) {
            if (sym_table.depth == 0) node.kind = NodeKind::global_var_declaration;
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
        _ INTERNAL_PANIC("semantic analysis missing for '{}'", node.kind_name());
    }
}
