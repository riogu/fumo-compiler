#include "semantic_analysis/analyzer.hpp"

[[nodiscard]] ASTNode* Analyzer::find_declaration(ASTNode& node) {
    INTERNAL_PANIC("{} not implemented.", __FUNCTION__); 
    match(node) {
        holds(Identifier&, id) {
            for (int i = symbol_tree.symbols_to_nodes_stack.size(); i != 0; i--) {
                for (const auto& [name, node] : symbol_tree.symbols_to_nodes_stack.at(i)) {
                    if (name == id.name) return node;
                }
            }
            report_error(node.source_token, "use of undeclared identifier '{}'", id.name);
        }
        _default{}
    }
    report_error(node.source_token, "use of undeclared identifier.");
}



[[nodiscard]] bool Analyzer::is_compatible_t(const Type& a, const Type& b) {
    INTERNAL_PANIC("{} not implemented.", __FUNCTION__); 
}
[[nodiscard]] bool Analyzer::is_arithmetic_t(const Type& a) {
    INTERNAL_PANIC("{} not implemented.", __FUNCTION__); 
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
