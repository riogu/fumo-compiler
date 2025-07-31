#include "semantic_analysis/analyzer.hpp"

// struct gaming {let ahhh: i32;};
// let x: gaming;
// fn func() -> void {
//      struct gaming {let bbbb: f32;};
//      let x: gaming = 123123;
//      {
//          struct gaming {let vvvvv = 321;};
//          let x: gaming;
//          funcfunc();
//      }
// }
//
        // somefunc();
        // namespace huh {
        //    struct foo {
        //         fn somefunc() -> void;
        //
        //         fn another() -> void {
        //             somefunc();
        //             struct bar {};
        //             "huh::foo::another()::bar"
        //             {
        //             "huh::foo::another()::::somefunc"();
        //             somefunc();
        //             }
        //         }
        //    };
        // }
[[nodiscard]] Opt<ASTNode*> Analyzer::find_declaration(Identifier& id) {
    id.mangled_name = symbol_tree.curr_scope_name + id.name;

    // FIXME: replace for loops with reverse iteration through the scope_name_stack
    switch (id.kind) {

        case Identifier::unsolved_type_name:
            for (const auto& [name, node] : symbol_tree.type_decls) {
                if (name == id.mangled_name) return node;
            }
            break;

        // TODO: function calls should not get mangled by the local function ever
        case Identifier::unsolved_func_call_name:
            switch (symbol_tree.curr_scope_kind) {
                case ScopeKind::TypeBody: // search mangled names first
                    for (const auto& [name, node] : symbol_tree.function_decls) {
                        if (name == id.mangled_name) return node;
                    }
                    for (const auto& [name, node] : symbol_tree.function_decls) {
                        if (name == id.name) return node;
                    }
                    break;
                case ScopeKind::CompoundStatement:
                    for (const auto& [name, node] : symbol_tree.function_decls) {
                        if (name == id.mangled_name) return node;
                    }
                case ScopeKind::Namespace: INTERNAL_PANIC("function call can't be global.");
            }
            break;

        case Identifier::unsolved_var_name:
            for (const auto& [name, node] : symbol_tree.local_variable_decls) {
                if (name == id.mangled_name) return node;
            }
            for (const auto& [name, node] : symbol_tree.global_variable_decls) {
                if (name == id.mangled_name) return node;
            }
            break;

        case Identifier::declaration_name: return id.declaration;
        case Identifier::unknown_name:
            INTERNAL_PANIC("forgot to set identifier name kind for {}.", id.mangled_name);
    }

    return std::nullopt;
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
                         get_name(bin.lhs->type), node.source_token.to_str(),get_name(bin.rhs->type));
        case BinaryExpr::assignment:
            report_error(node.source_token,"assigning to '{}'from incompatible type '{}'.",
                         get_name(bin.lhs->type), get_name(bin.rhs->type));
        default:
            INTERNAL_PANIC("expected binary node for error, got '{}'.", node.kind_name());
    }
}
