#include "semantic_analysis/analyzer.hpp"
#include <ranges>

#define find_value(key, map) (const auto& iter = map.find(key); iter != map.end())
#define find_in_each_map(map)                                                       \
    if find_value(scope.name + id.name, map) {                                      \
        return id.mangled_name = scope.name + id.name, iter->second;                \
    }
#define search_maps(...)                                                            \
for (const auto& scope : scope_stack | std::views::reverse) {                       \
    map_macro(find_in_each_map, __VA_ARGS__);                                       \
}

// NOTE: change the implementation so we dont keep all locals to the end of the program

[[nodiscard]] Opt<ASTNode*> SymbolTableStack::find_declaration(Identifier& id) {
    switch (id.kind) {
        case Identifier::type_name:
            search_maps(type_decls);
            break;
        case Identifier::func_call_name:
            switch (curr_scope_kind) {
                case ScopeKind::Namespace:
                case ScopeKind::FunctionBody:
                case ScopeKind::CompoundStatement:

                    for (const auto& scope : scope_stack | std::views ::reverse) {
                        if (const auto& iter = function_decls.find(scope.name + id.name);
                            iter != function_decls.end()) {
                            return id.mangled_name = scope.name + id.name, iter->second;
                        };
                    };
                    break;
                case ScopeKind::TypeBody:
                case ScopeKind::MemberFuncBody:
                case ScopeKind::MemberCompoundStatement:
                    if (id.qualifier == Identifier::qualified) {
                        search_maps(function_decls);
                        break;
                    }
                    search_maps(member_function_decls, function_decls);
                    break;
            }
            break;
        case Identifier::var_name:
            switch (curr_scope_kind) {
                case ScopeKind::MemberFuncBody:
                case ScopeKind::MemberCompoundStatement:
                    if (id.qualifier == Identifier::qualified) {
                        search_maps(local_variable_decls, global_variable_decls);
                        break;
                    }
                    search_maps(member_variable_decls, local_variable_decls, global_variable_decls);
                    break;
                case ScopeKind::TypeBody:
                    search_maps(member_variable_decls, global_variable_decls);
                    break;
                case ScopeKind::FunctionBody:
                case ScopeKind::CompoundStatement:
                    search_maps(local_variable_decls, global_variable_decls);
                    break;
                case ScopeKind::Namespace:
                    search_maps(global_variable_decls);
                    break;
            }
            break;
        case Identifier::member_var_name:
            if find_value (id.mangled_name, member_variable_decls) return iter->second;
            break;
        case Identifier::member_func_call_name:
            if find_value (id.mangled_name, member_function_decls) return iter->second;
            break;


        case Identifier::declaration_name:
            INTERNAL_PANIC("declaration '{}' shouldn't search for itself", id.mangled_name);
        case Identifier::unknown_name:
            INTERNAL_PANIC("forgot to set identifier name kind for {}.", id.mangled_name);

    }
    return std::nullopt;
}

[[nodiscard]] ScopeKind SymbolTableStack::find_scope_kind(const str& name) {
    if find_value (name, namespace_decls) return ScopeKind::Namespace;
    if find_value (name, type_decls) return ScopeKind::TypeBody;
    INTERNAL_PANIC("couldnt find scope kind for '{}'.", name);
}
void Analyzer::iterate_qualified_names(FunctionDecl & func) {
    // NOTE: bad code to iterate through each qualified scope in an out-of-line definition of a function
    auto& id = get_id(func);
    if (id.qualifier == Identifier::qualified) {
        str temp = id.name;
        str unqualified_name = "";
        while (temp.back() != ':') unqualified_name = temp.back() + unqualified_name, temp.pop_back();
        temp.pop_back(), temp.pop_back();

        ScopeKind scope_kind;
        if find_value (temp, symbol_tree.type_decls) {
            func.kind = FunctionDecl::member_func_declaration;
            scope_kind = ScopeKind::MemberFuncBody;
        } else {
            func.kind = FunctionDecl::function_declaration;
            scope_kind = ScopeKind::FunctionBody;
        }

        str full_name = "";
        full_name.push_back(*temp.begin());
        str isolated_name = full_name;
        temp.erase(0, 1);
        while (1) {
            while (!temp.empty() && *temp.begin() != ':') {
                full_name += *temp.begin();
                isolated_name += *temp.begin();
                temp.erase(temp.begin());
            }

            ScopeKind kind = symbol_tree.find_scope_kind(full_name);
            full_name += *temp.begin(), temp.erase(0, 1);
            full_name += *temp.begin(), temp.erase(0, 1);

            symbol_tree.push_scope(isolated_name, kind);
            isolated_name = "";
            if (temp.empty()) break;
        }
        symbol_tree.push_scope(unqualified_name, scope_kind);

        return;
    } 
    switch (symbol_tree.curr_scope_kind) {
        case ScopeKind::TypeBody:
            func.kind = FunctionDecl::member_func_declaration;
            symbol_tree.push_scope(id.name, ScopeKind::MemberFuncBody);
            break;
        default:
            func.kind = FunctionDecl::function_declaration;
            symbol_tree.push_scope(id.name, ScopeKind::FunctionBody);
            break;
    }

}

void Analyzer::report_binary_error(const ASTNode& node, const BinaryExpr& bin) {
    switch (bin.kind) {
        case(BinaryExpr::add,   BinaryExpr::sub,       BinaryExpr::multiply,  BinaryExpr::divide,
             BinaryExpr::equal, BinaryExpr::not_equal, BinaryExpr::less_than, BinaryExpr::less_equals) {
            report_error(node.source_token, "invalid operands to binary expression: '{}' {} '{}'.",
                         get_name(bin.lhs->type), node.source_token.to_str(), get_name(bin.rhs->type));
        } 
        case BinaryExpr::assignment:
            report_error(node.source_token, "assigning to '{}' from incompatible type '{}'.",
                         get_name(bin.lhs->type), get_name(bin.rhs->type));
        default: INTERNAL_PANIC("expected binary node for error, got '{}'.", node.kind_name());
    }
}

[[nodiscard]] bool Analyzer::is_compatible_t(const Type& a, const Type& b) {
    return ((is_arithmetic_t(a) && is_arithmetic_t(b)) || (a.kind == b.kind && get_name(a) == get_name(b)));
}
[[nodiscard]] bool Analyzer::is_arithmetic_t(const Type& type) {
    return (type.kind == Type::i8_  || type.kind == Type::i32_ || type.kind == Type::i64_
         || type.kind == Type::f32_ || type.kind == Type::f64_ || type.kind == Type::bool_);
}
// namespace foo {
//    struct foo {
//         struct gaming {
//            let xee: i32 = 213; 
//            fn fff() -> i32 {return xee;} 
//            fn some() -> i32 {return fff();} 
//         };
//         fn some_func() -> void;
//     };
// }
// fn foo::foo::some_func() -> void {
//     let var: bar = {};
//     let eee = gaming {212322};
//     let e_ptr: gaming* = &eee;
//     let y = var.func(e_ptr->xee).some();
// }
