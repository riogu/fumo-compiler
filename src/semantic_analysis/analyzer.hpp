#pragma once
#include <ranges>
#include <llvm/IR/Function.h>
#include "base_definitions/ast_node.hpp"
#include "base_definitions/symbol_table_stack.hpp"
#include "utils/common_utils.hpp"

enum struct ControlFlowResult;

struct Analyzer {
    Analyzer(const File& file) { file_stream << file.contents; }
    void semantic_analysis(ASTNode* file_scope);

    SymbolTableStack symbol_tree {};
    vec<unique_ptr<ASTNode>> extra_nodes {}; // used for member functions to allocate a parameter
    Opt<Identifier*> curr_postfix_id {};
    ASTNode* root_node;

  private:
    std::stringstream file_stream;

    void analyze(ASTNode& node);
    void report_binary_error(const ASTNode& node, const BinaryExpr& bin);
    void check_initializer_lists(const ASTNode& node, BinaryExpr& bin);

    void analyze_function_control_flow(ASTNode& node);
    ControlFlowResult control_flow_analysis(ASTNode& node);

    ASTNode* create_main_node(Token& token) {
        // NOTE: not being used anymore
        auto id = push(ASTNode {token, Identifier {Identifier::declaration_name, "main"}});
        FunctionDecl function {FunctionDecl::function_declaration, id};

        auto* main_node = push(ASTNode {token, function});
        main_node->type = Type {push(ASTNode {token, Identifier {Identifier::type_name, "i32"}}), Type::i32_};

        return main_node;
    }
    void add_declaration(ASTNode& node);
    vec<Scope> iterate_qualified_names(FunctionDecl& func, ASTNode& node);
    [[nodiscard]] Opt<ASTNode*> get_or_create_generic_declaration(ASTNode& node);

    ASTNode* push(const ASTNode& node) {
        extra_nodes.push_back(std::make_unique<ASTNode>(node));
        return extra_nodes.back().get();
    }
    void determine_type(ASTNode& node);

    // let var: U = T{}; // should error
    // let var: Vec[U] = Vec[T]{}; // should error
    [[nodiscard]] bool is_compatible_or_generic_t(const Type& a, const Type& b) {
        auto& a_id = get_id(a);
        auto& b_id = get_id(b);
        if (!(a_id.kind == Identifier::generic_type_name || b_id.kind == Identifier::generic_type_name)
         && !(a_id.kind == Identifier::generic_wrapper_type_name || b_id.kind == Identifier::generic_wrapper_type_name)) {
            // we dont want to allow:
            // let a: any* = {};
            // let var: T* = a;
            // let var: Vec[T]* = a;
            // cant assign an any* to a generic.
            // only allow compatibility with 'any*' if it wasnt a generic type
            if ((a.kind == Type::any_ || b.kind == Type::any_) && a.ptr_count == b.ptr_count) return true;
        }

        if (is_arithmetic_t(a) && is_arithmetic_t(b) && a.ptr_count == b.ptr_count) return true;

        if (a.kind == b.kind) {
            if (type_name(a) == type_name(b)) {
                return true;
            }
        } else {
            if ((type_name(a) == "null*" && b.ptr_count) || (type_name(b) == "null*" && a.ptr_count)) {
                // allow compatibility between any pointer to null
                return true;
            }
        }
        return false;
    }

    void push_generic_context(vec<ASTNode*> generic_identifiers) {
        for (auto* node : generic_identifiers) {
            str name = get<Identifier>(node).mangled_name; // T, U, etc. from struct Foo[T, U], and so on
            const auto& [node_iterator, was_inserted] = symbol_tree.curr_generic_context.insert({name, node});
            if (!was_inserted) { // dont allow reusing generic type name identifiers
                report_error(node->source_token, "declaration of '{}' shadows previous generic parameter.", name);
            }
        }
    }

    void pop_generic_context(vec<ASTNode*> generic_identifiers) {
        for (auto* node : generic_identifiers) {
            str name = get<Identifier>(node).mangled_name; // T, U, etc. from struct Foo[T, U], and so on
            if find_value(name, symbol_tree.curr_generic_context) {
                symbol_tree.curr_generic_context.erase(iter);
            } else {
                // this shouldn't happen if push/pop are balanced
                report_error(node->source_token, "attempting to pop non-existent generic parameter '{}'.", name);
            }
        }
    }
};


