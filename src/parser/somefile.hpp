#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>

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

#define _                                                                                           \
        break;                                                                                      \
    }                                                                                               \
        default:                                                                                    \
            was_default = true;                                                                     \
}                                                                                                   \
    if (was_default)

#define holds(T, name)                                                                              \
break;}                                                                                             \
    case __t_seq__.idx<std::remove_pointer<std ::remove_cvref<T>::type>::type>: {                   \
    T name = get_elem<std::remove_pointer<std ::remove_cvref<decltype(name)>::type>::type>(__v__); 

#define match(v)                                                                                    \
{                                                                                                   \
    auto& __v__ = v;                                                                                \
    bool was_default = false;                                                                       \
    switch (auto __t_seq__ = type_sequence(v); index_of(v)) {                                   


struct Add; struct Sub;struct Mul;struct Div; struct Var;struct Func;
using ASTNode = std::variant<Add, Sub, Mul, Div, Var, Func>;

struct Add{ std::unique_ptr<ASTNode> lhs; std::unique_ptr<ASTNode> rhs;};
struct Sub{ int lhs, rhs;};
struct Mul{int lhs;};
struct Div{};
struct Var{};
struct Func{};

inline size_t index_of(std::shared_ptr<ASTNode>& node) { return node->index(); }
inline size_t index_of(std::unique_ptr<ASTNode>& node) { return node->index(); }
inline size_t index_of(ASTNode& node) { return node.index(); }
template<typename  T> auto& get_elem(std::unique_ptr<ASTNode>& node) { return std::get<T>(*node); }
template<typename  T> auto& get_elem(std::shared_ptr<ASTNode>& node) { return std::get<T>(*node); }
template<typename  T> auto& get_elem(ASTNode& node) { return std::get<T>(node); }


inline void func() {
    ASTNode v = Add {
        .lhs = std::make_unique<ASTNode>(Sub {1, 2}),
        .rhs = std::make_unique<ASTNode>(Sub {2, 4}),
    };

    match(v) {
        holds(Add, &add_var) {
            match(add_var.lhs) {
                holds(Mul, mul) {
                    std::cout << "mul was: " << mul.lhs << '\n';
                }
                holds(Sub, sub) std::cout << "inner was sub: "<< sub.lhs; {}
                _ {}
            }
        }
        holds(Sub, sub) std::cout << "was sub";

        holds(Mul, mul) std::cout << "was mul.\n";

        _ std::cerr << "type unknown.\n";
    }
}
