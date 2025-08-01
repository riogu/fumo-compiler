#include "base_definitions/ast_node.hpp"

[[nodiscard]] std::string ASTNode::to_str(int64_t depth) const {
    depth++;
    std::string result = std::format("{} ", yellow(branch_name()) + gray("::") +  enum_green(kind_name()));

    match(*this) {
        holds(Identifier, const& id) {
            if (id.declaration) 
                result += green("✓ SOLVED");
            else
                result += red("❌NOT SOLVED");
            result += std::format(" {}{}{}", gray("⟮"), white_gray(id.mangled_name), gray("⟯"));
        }
        holds(PrimaryExpr, const& prim) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
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
            result += std::format("{} {}", gray("=>"), white_gray(get_id(var).mangled_name));
            str ptr_str; for (int i = 0; i < type.ptr_count; i++) ptr_str += "*";
            result += gray(": ") + yellow(get_id(type).mangled_name) + purple_blue(ptr_str);
        }
        holds(FunctionDecl, const& func) {
            std::string temp = purple_blue("fn ") + blue(get_id(func).mangled_name) + gray("(");
            for(size_t i = 0; i < func.parameters.size(); i++) {
                ASTNode* param = func.parameters.at(i);
                auto& var = get<VariableDecl>(param);

                str ptr_str; for (int i = 0; i < param->type.ptr_count; i++) ptr_str += "*";

                temp += white_gray(get_name(var)) + gray(": ") + yellow(get_id(param->type).mangled_name) + gray(ptr_str);

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
        _default { PANIC(std::format("couldn't print node of kind: {}.", yellow(branch_name()) + gray("::") + enum_green(kind_name()))); }
    }
    return result;
}

#define nd_kind(v_) case v.v_: return #v_;
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

#define branch_kind_name(Branch) holds(Branch, const& v) return #Branch;

[[nodiscard]] std::string ASTNode::branch_name() const {
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

#undef branch_kind_name
