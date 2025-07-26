#include "parser/ast_node.hpp"


[[nodiscard]]  std::string ASTNode::to_str(int64_t depth) const {
    depth++;
    std::string result = std::format("{} ", kind_name());

    match(*this) {
        holds(PrimaryExpr) result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
        
        holds(const UnaryExpr&, unary) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
            depth--;
            result += std::format(" {} {}", gray("::="), unary.expr->to_str(depth));
        }
        holds(const BinaryExpr&, bin) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
            result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), bin.lhs->to_str(depth));
            result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), bin.rhs->to_str(depth));
        }
        holds(const VariableDecl&, var) {
            result += std::format("{} {}", gray("=>"), yellow(var.name));
            result += gray(": ") + yellow(type.name);
            if (var.value) {
                result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), var.value.value()->to_str(depth));
            }
        }
        holds(const FunctionDecl&, func) {
            std::string temp = yellow("fn ") + blue(func.name) + gray("(");
            for(size_t i = 0; i < func.parameters.size(); i++) {
                ASTNode* var = func.parameters.at(i);
                match(*var) {
                    holds(const VariableDecl&, param) {
                        temp += param.name + gray(": ") + yellow(var->type.name);
                        if (param.value) temp += " = " + param.value.value()->to_str();
                    }
                    _ { INTERNAL_PANIC("function parameter must be a variable'{}'", kind_name()); }
                }
                if (i != func.parameters.size() - 1) temp += gray(", ");
            }
            temp += gray(")");
            temp += gray(" -> ") + yellow(type.name);
            result += std::format("{} {}", gray("=>"), temp);
            if (func.body) result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"),
                                                 func.body.value()->to_str(depth)); 
        }
        holds(const BlockScope&, scope) {
            result += gray("{");
            depth++;
            for(auto& node: scope.nodes) 
                result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), node->to_str(depth));
            depth--;
            result += std::format("\n{}{}", std::string(depth * 2, ' '), gray("}"));
        }
        _ { PANIC(std::format("couldn't print node of kind: {}.", kind_name())); }
    }

    return result;
}

#define nd_kind(v_) case ASTNode::v_: return std::format("\033[38;2;142;163;217m{}\033[0m",#v_);
[[nodiscard]] std::string ASTNode::kind_name() const {
    switch (kind) {
        map_macro(nd_kind, all_node_kinds);
        default: PANIC(std::format("provided unknown ASTNode '{}'.", (int)kind));
    }
}
