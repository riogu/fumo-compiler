// works just like a stack of scopes
// we push a std::map when entering a new scope
// we pop the current std::map when leaving a scope

// all declarations are flattened and identifiers are changed to match them
// they are "global" but renamed internally
// for example:
//
//   struct    foo  { struct bar {};     } => struct "foo::bar"    {};
//   struct    foo  { fn func() -> void; } => fn     "foo::func"(this: foo*) -> void;
//
//   namespace foo  { struct bar {};     } => struct "foo::bar"    {};
//   namespace foo  { fn func() -> void; } => fn     "foo::func"() -> void;
//
//   fn f() -> void { struct bar {};     } => struct "foo()::bar"  {};
//   fn f() -> void { let bar: i32;      } => let    "foo()::bar": i32;
//
//   for generics:
//   struct  foo::bar[T, U, V]()      { ... }
//   struct "foo::bar[{}]()"          { ... }
//
//   fn  foo::bar[T]::thing[U]()      { ... }
//   fn "foo::bar[{}]::thing[{}]()"   { ... }
//
#pragma once

#include "base_definitions/ast_node.hpp"
#include "utils/common_utils.hpp"

#define find_value(key, map) (const auto& iter = map.find(key); iter != map.end())
#define each_case_label(the_case) case the_case:
#undef cases
#define case(...) map_macro(each_case_label, __VA_ARGS__)

#include <map>
#include <utility>

struct SymbolTableStack {
    // --------------------------------------------
    // NOTE: these are not used in codegen
    std::map<str, ASTNode*> local_variable_decls {}; 
    std::map<str, ASTNode*> namespace_decls {};
    // --------------------------------------------
    // for separating lookups (could be just a flag on each member but this is fine)
    // the flag is elsewhere and then we just pick a map to search. same result
    std::map<str, ASTNode*> global_variable_decls {};
    std::map<str, ASTNode*> type_decls {};
    std::map<str, ASTNode*> function_decls {};
    std::map<str, ASTNode*> member_function_decls {};
    std::map<str, ASTNode*> member_variable_decls {};
    // --------------------------------------------
    // change this to only include the top level declarations so we can recursively codegen them.
    std::map<str, ASTNode*> all_declarations {}; 
    // NOTE: doesn't include namespaces or local variables or member variables
    // --------------------------------------------
    vec<Scope> scope_stack {};

    str curr_scope_name = "";
    ScopeKind curr_scope_kind;

    void push_scope(str name, ScopeKind kind) {
        // std::cerr << name << '\n';
        switch (kind) {
            case ScopeKind::Namespace:    name += "::";   break;
            case ScopeKind::TypeBody:     name += "::";   break;
            case ScopeKind::MemberFuncBody:
            case ScopeKind::FunctionBody: name += "()::"; break;
            case ScopeKind::CompoundStatement: // += "0::"
            case ScopeKind::MemberCompoundStatement:
                if (!scope_stack.empty()) {
                    int& prev_scope_count = scope_stack.back().inner_scope_count;
                    name = std::to_string(prev_scope_count) + "::";
                    prev_scope_count++;
                }
                break;
        }
        curr_scope_kind = kind;
        curr_scope_name += name;
        scope_stack.push_back({curr_scope_name, kind, name});
    }
    void pop_scope() {
        curr_scope_name.resize(curr_scope_name.size() - scope_stack.back().isolated_name.size());
        scope_stack.pop_back();
        if (!scope_stack.empty()) [[likely]] curr_scope_kind = scope_stack.back().kind;
    }

    auto push_variable_decl(Identifier& identifier, ASTNode& node) {
        identifier.mangled_name = curr_scope_name + identifier.name;
        switch (curr_scope_kind) {
            case ScopeKind::Namespace:
                all_declarations.insert({identifier.mangled_name, &node});
                return global_variable_decls.insert({identifier.mangled_name, &node});
            case ScopeKind::TypeBody:
                return member_variable_decls.insert({identifier.mangled_name, &node});
            case ScopeKind::MemberFuncBody:
            case ScopeKind::FunctionBody: 
            case ScopeKind::CompoundStatement: 
            case ScopeKind::MemberCompoundStatement:
                return local_variable_decls.insert({identifier.mangled_name, &node});
        }
        std::unreachable();
    }
    auto push_type_decl(Identifier& identifier, ASTNode& node) {
        identifier.mangled_name = curr_scope_name + identifier.name;
        all_declarations.insert({identifier.mangled_name, &node});
        return type_decls.insert({identifier.mangled_name, &node});
    }
    auto push_namespace_decl(Identifier& identifier, ASTNode& node) {
        identifier.mangled_name = curr_scope_name + identifier.name;
        return namespace_decls.insert({identifier.mangled_name, &node});
    }
    auto push_function_decl(Identifier& identifier, ASTNode& node) {
        // NOTE: this function might need some changes if you get issues with identifiers

        identifier.mangled_name = curr_scope_name + identifier.name;
        all_declarations.insert({identifier.mangled_name, &node});
        switch (curr_scope_kind) {
            case ScopeKind::TypeBody: 
                if (node.type.qualifiers.contains(Type::static_)) {
                    return function_decls.insert({identifier.mangled_name, &node});
                }
                return member_function_decls.insert({identifier.mangled_name, &node});
            case ScopeKind::Namespace:
            case ScopeKind::FunctionBody:
            case ScopeKind::MemberFuncBody: 
            case ScopeKind::CompoundStatement:
            case ScopeKind::MemberCompoundStatement:
                if (get<FunctionDecl>(&node).kind == FunctionDecl::member_func_declaration) {
                    if (node.type.qualifiers.contains(Type::static_)) {
                        return function_decls.insert({identifier.mangled_name, &node});
                    } 
                    return member_function_decls.insert({identifier.mangled_name, &node});
                }
                return function_decls.insert({identifier.mangled_name, &node});
        }
        std::unreachable();
    }

    [[nodiscard]] Opt<ASTNode*> find_declaration(Identifier& id);
    [[nodiscard]] ScopeKind find_scope_kind(const str& name);
};
