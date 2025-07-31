#include "base_definitions/ast_node.hpp"

[[nodiscard]] std::string ASTNode::to_str(int64_t depth) const {
    depth++;
    std::string result = std::format("{} ", kind_name());

    match(*this) {
        holds(Identifier, const& id) result += std::format("{}{}{}", gray("⟮"), id.name, gray("⟯"));
       
        holds(PrimaryExpr, const& prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:
                    result += std::format("{}{}{}", gray("⟮"), std::get<int64_t>(prim.value), gray("⟯"));
                    break;
                case PrimaryExpr::floating_point:
                    result += std::format("{}{}{}", gray("⟮"), std::get<double>(prim.value), gray("⟯"));
                    break;
                case PrimaryExpr::str:
                // case PrimaryExpr::identifier: 
                    result += std::format("{}{}{}", gray("⟮"), std::get<str>(prim.value), gray("⟯"));
                    break;
            }
        }

        holds(UnaryExpr, const& unary) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
            depth--;
            result += std::format(" {} {}", gray("::="), unary.expr->to_str(depth));
        }
        holds(BinaryExpr, const& bin) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
            result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), bin.lhs->to_str(depth));
            result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), bin.rhs->to_str(depth));
        }
        holds(PostfixExpr, const& postfix) {
            // TODO: improve printing for postfix expressions here 
            switch (postfix.kind) {
                case PostfixExpr::member_access:
                case PostfixExpr::deref_member_access:
                case PostfixExpr::function_call: 
                    result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
                    result += std::format("\n{} {} {}", std::string(depth * 2, ' '), gray("↳"), postfix.lhs->to_str(depth));
                    result += std::format("\n{} {} {}", std::string(depth * 2, ' '), gray("↳"), postfix.rhs->to_str(depth));
                    break;
            }
        }
        holds(VariableDecl, const& var) {
            result += std::format("{} {}", gray("=>"), get_id(var).mangled_name);
            str ptr_str; for (int i = 0; i < type.ptr_count; i++) ptr_str += "*";
            result += gray(": ") + yellow(get_name(type)) + purple_blue(ptr_str);
        }
        holds(FunctionDecl, const& func) {
            std::string temp = purple_blue("fn ") + blue(get_id(func).mangled_name) + gray("(");
            for(size_t i = 0; i < func.parameters.size(); i++) {
                ASTNode* var = func.parameters.at(i);
                auto& param = get<VariableDecl>(var);
                str ptr_str; for (int i = 0; i < var->type.ptr_count; i++) ptr_str += "*";
                temp += get_id(param).name + gray(": ") + yellow(get_name(var->type)) + gray(ptr_str);;
                if (i != func.parameters.size() - 1) temp += gray(", ");
            }
            temp += gray(")");
            str ptr_str; for (int i = 0; i < type.ptr_count; i++) ptr_str += "*";
            temp += gray(" -> ") + yellow(get_name(type)) + gray(ptr_str);

            result += std::format("{} {}", gray("=>"), temp);
            if (func.body) result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"),
                                                 func.body.value()->to_str(depth)); 
        }
        holds(BlockScope, const& scope) {
            switch (scope.kind) {
                case BlockScope::compound_statement:
                case BlockScope::initializer_list:
                    result += gray("{");
                    depth++;
                    for(auto& node: scope.nodes) 
                        result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), node->to_str(depth));
                    depth--;
                    result += std::format("\n{}{}", std::string(depth * 2, ' '), gray("}"));
                    break;
                case BlockScope::argument_list: 
                    result += gray("(");
                    depth++;
                    for(auto& node: scope.nodes) 
                        result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), node->to_str(depth));
                    depth--;
                    result += std::format("\n{}{}", std::string(depth * 2, ' '), gray(")"));
                    break;
            }
        }
        holds(NamespaceDecl, const& namespace_decl) {
            result += gray("=> ") + purple_blue("namespace ") + yellow(get_id(namespace_decl).mangled_name);
            result += std::format("\n{}{} ", std::string(depth * 2, ' '), gray("↳"));
            result += purple_blue("definition ") + gray("{");
            depth++;
            depth++;
            for(auto& node: namespace_decl.nodes) 
                result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), node->to_str(depth));
            depth--;
            result += std::format("\n{}{}", std::string(depth * 2, ' '), gray("}"));
        }
        holds(TypeDecl, const& type_decl) {
            result += gray("=> ") + purple_blue("struct ") + yellow(get_id(type_decl).mangled_name);
            if (type_decl.definition_body) {
                result += std::format("\n{}{} ", std::string(depth * 2, ' '), gray("↳"));
                result += purple_blue("definition ") + gray("{");
                depth++;
                depth++;
                for(auto& node: type_decl.definition_body.value()) 
                    result += std::format("\n{}{} {}", std::string(depth * 2, ' '), gray("↳"), node->to_str(depth));
                depth--;
                result += std::format("\n{}{}", std::string(depth * 2, ' '), gray("}"));
            }
        }
        _default { PANIC(std::format("couldn't print node of kind: {}.", kind_name())); }
    }
    return result;
}

#define nd_kind(v_) case v.v_: return std::format("\033[38;2;142;163;217m{}\033[0m",#v_);
#define branch_kind_name(Branch) holds(Branch, const& v) switch(v.kind) { map_macro(nd_kind, Branch##_kinds) } 

[[nodiscard]] std::string ASTNode::kind_name() const {
    match(*this) {
        branch_kind_name(Identifier);
        branch_kind_name(PrimaryExpr);
        branch_kind_name(UnaryExpr);
        branch_kind_name(BinaryExpr);
        branch_kind_name(PostfixExpr);
        branch_kind_name(VariableDecl);
        branch_kind_name(FunctionDecl);
        branch_kind_name(BlockScope);
        branch_kind_name(NamespaceDecl);
        branch_kind_name(TypeDecl);
        _default { PANIC(std::format("couldn't get kind name for node {}.", source_token.to_str())); }
    }
    std::unreachable();
}

#undef nd_kind
#undef branch_kind_name

