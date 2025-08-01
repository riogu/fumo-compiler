#include "semantic_analysis/analyzer.hpp"

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
                         get_name(bin.lhs->type), node.source_token.to_str(),get_name(bin.rhs->type));
        case BinaryExpr::assignment:
            report_error(node.source_token,"assigning to '{}' from incompatible type '{}'.",
                         get_name(bin.lhs->type), get_name(bin.rhs->type));
        default:
            INTERNAL_PANIC("expected binary node for error, got '{}'.", node.kind_name());
    }
}
