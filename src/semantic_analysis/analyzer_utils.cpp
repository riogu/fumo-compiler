#include "semantic_analysis/analyzer.hpp"
#include <ranges>
#define find_value(key, map) (const auto& iter = map.find(key); iter != map.end())
#define find_in_each_map(map)                                         \
    if find_value(scope.name + id.name, map) {                        \
        return id.mangled_name = scope.name + id.name, iter->second;  \
    }
#define search_maps(...)                                              \
for (const auto& scope : scope_stack | std::views::reverse) {         \
    map_macro(find_in_each_map, __VA_ARGS__);                         \
}

// NOTE: change the implementation so we dont keep all locals to the end of the program

[[nodiscard]] Opt<ASTNode*> SymbolTableStack::find_declaration(Identifier& id) {
    switch (id.kind) {
        case Identifier::type_name:
            for (const auto& scope : scope_stack | std ::views ::reverse) {
                if (const auto& iter = type_decls.find(scope.name + id.name); iter != type_decls.end()) {
                    // std::cerr << scope.name + id.name + " | ";
                    return id.mangled_name = scope.name + id.name, iter->second;
                };
            };
            break;
        case Identifier::func_call_name:
            switch (curr_scope_kind) {
                case(ScopeKind::Namespace, ScopeKind::FunctionBody, ScopeKind::CompoundStatement)
                    search_maps(function_decls); break;
                case(ScopeKind::TypeBody, ScopeKind::MemberFuncBody, ScopeKind::MemberCompoundStatement)
                    if (id.qualifier == Identifier::qualified) {
                        search_maps(function_decls);
                        break;
                    }
                    search_maps(member_func_decls, function_decls); break;
            }
            break;
        case Identifier::var_name:
            switch (curr_scope_kind) {
                case(ScopeKind::MemberFuncBody, ScopeKind::MemberCompoundStatement)
                    if (id.qualifier == Identifier::qualified) {
                        search_maps(local_variable_decls, global_variable_decls);
                        break;
                    }
                    search_maps(member_variable_decls, local_variable_decls, global_variable_decls); break;
                case(ScopeKind::TypeBody)
                    search_maps(member_variable_decls, global_variable_decls); break;
                case(ScopeKind::FunctionBody, ScopeKind::CompoundStatement)
                    search_maps(local_variable_decls, global_variable_decls);  break;
                case(ScopeKind::Namespace) 
                    search_maps(global_variable_decls); break;
            }
            break;

        case Identifier::member_var_name:
            for (const auto& scope : scope_stack | std ::views ::reverse) {
                if find_value(id.mangled_name, member_variable_decls) {}
            }
            break;
        case Identifier::member_func_call_name:
            for (const auto& scope : scope_stack | std ::views ::reverse) {
                if find_value(id.mangled_name, member_variable_decls) {}
            }
            break;

        case Identifier::declaration_name: 
            INTERNAL_PANIC("declaration '{}' shouldn't search for itself", id.mangled_name);
        case Identifier::unknown_name:
            INTERNAL_PANIC("forgot to set identifier name kind for {}.", id.mangled_name);
    }

    return std::nullopt;
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

[[nodiscard]] bool Analyzer::is_compatible_t(const Type& a, const Type& b) {
    INTERNAL_PANIC("{} not implemented.", __FUNCTION__); 
}
[[nodiscard]] bool Analyzer::is_arithmetic_t(const Type& a) {
    INTERNAL_PANIC("{} not implemented.", __FUNCTION__); 
}
