#include "semantic_analysis/analyzer.hpp"

void Analyzer::determine_type(Type& type) {
    INTERNAL_PANIC("{} not implemented.", __FUNCTION__);
}
[[nodiscard]] ASTNode* Analyzer::find_function_decl(std::string_view var_name) {
    INTERNAL_PANIC("{} not implemented.", __FUNCTION__);
}
[[nodiscard]] ASTNode* Analyzer::find_variable_decl(std::string_view var_name) {
    INTERNAL_PANIC("{} not implemented.", __FUNCTION__);
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
