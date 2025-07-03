#pragma once
#include "lexer/token_definitions.hpp"
#include <iostream>
#include <memory>

// clang-format off

enum struct NodeKind {
    // binary
    add,                     // +                          
    subtract,                // -                      
    multiply,                // *                      
    divide,                  // /
    equal,                   // ==
    not_equal,               // !=                     
    less_than,               // <                      
    greater_than,            // <=                     
    assignment,              // =
    // unary
    expression_statement,    // Expression statement
    negate,                  // unary -
    // variable
    variable,                // Variable
    // value
    integer,                 // Integer
};


struct ASTNode;
struct Unary; struct Binary; struct If; struct For; struct Variable; 
struct Value; struct Function; struct Member; struct Scope;

using NodeBranch = std::variant<Unary, Binary, If, For, Variable, Value, Function, Member, Scope>;

struct Unary {};
struct Binary {
    std::unique_ptr<ASTNode> lhs;
    std::unique_ptr<ASTNode> rhs;
};
struct If {};
struct For {};
struct Variable {};
struct Value {};
struct Function {};
struct Member {};
struct Scope {};

struct ASTNode {

    NodeKind kind;
    Token token; // token that originated this Node
    NodeBranch branch;

};


template<typename T, typename Head, typename... Tail>
constexpr int type_index(int index = 0) {
    if constexpr (std::is_same_v<T, Head>) return index;
    else
        return type_index<T, Tail...>(index + 1);
}

template<typename... Types>
struct type_sequence {
    constexpr type_sequence(const std::variant<Types...>&) {}
    constexpr type_sequence(std::unique_ptr<std::variant<Types...>>&) {}

    template<typename T>
    constexpr static int idx = type_index<T, Types...>();
};

constexpr auto wrapped_type_seq(const ASTNode& node) { return type_sequence(node.branch); };
constexpr auto wrapped_type_seq(const std::shared_ptr<ASTNode>& node) { return type_sequence(node->branch); };
constexpr auto wrapped_type_seq(const std::unique_ptr<ASTNode>& node) { return type_sequence(node->branch); };


inline size_t index_of(std::shared_ptr<ASTNode>& node) { return node->branch.index(); }
inline size_t index_of(std::unique_ptr<ASTNode>& node) { return node->branch.index(); }
inline size_t index_of(ASTNode& node) { return node.branch.index(); }
template<typename  T> auto& get_elem(std::unique_ptr<ASTNode>& node) { return std::get<T>(node->branch); }
template<typename  T> auto& get_elem(std::shared_ptr<ASTNode>& node) { return std::get<T>(node->branch); }
template<typename  T> auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }


#define match(v)                                                                                    \
{                                                                                                   \
    auto& __v__ = v;                                                                                \
    bool was_default = false;                                                                       \
    switch (auto __t_seq__ = wrapped_type_seq(v); index_of(v)) {                                   


#define holds(T, name)                                                                              \
    break;                                                                                          \
}                                                                                                   \
    case __t_seq__.idx<std::remove_pointer<std ::remove_cvref<T>::type>::type>: {                   \
        T name = get_elem<std::remove_pointer<std ::remove_cvref<decltype(name)>::type>::type>(__v__); 

#define _                                                                                           \
        break;                                                                                      \
    }                                                                                               \
        default:                                                                                    \
            was_default = true;                                                                     \
}                                                                                                   \
    if (was_default)

// clang-format on

namespace Parser {
Result<Vec<ASTNode>, str> parse_tokens(Vec<Token>& tokens);
} // namespace Parser


inline void func() {
    ASTNode v = {.branch = Binary {
                     .lhs = std::make_unique<ASTNode>(
                         ASTNode {.kind = NodeKind::add,
                                  .branch = Binary {std::make_unique<ASTNode>(),
                                                    std::make_unique<ASTNode>()}}),
                     .rhs = std::make_unique<ASTNode>(),
                 }};

    match(v) {
        holds(Binary, &bin_var) { // have to take by reference
            match(bin_var.lhs) {
                holds(Binary, &mul) {
                    std::cout << "mul was: " << mul.lhs << '\n';
                }
                _ {}
            }
        }
        _ std::cerr << "type unknown.\n";
    }
}
