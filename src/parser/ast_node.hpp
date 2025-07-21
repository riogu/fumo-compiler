#pragma once
#include "lexer/token_definitions.hpp"
#include "parser/type.hpp"
#include <llvm/IR/Value.h>
// clang-format off

enum struct NodeKind { 
#define all_node_kinds                                               \
    /* ----------------------------------------                   */ \
    /* simple expressions                                         */ \
    /* binary                                                     */ \
    empty_expr,              /* ;                                 */ \
    add,                     /* +                                 */ \
    sub,                     /* -                                 */ \
    multiply,                /* *                                 */ \
    divide,                  /* /                                 */ \
    equal,                   /* ==                                */ \
    not_equal,               /* !=                                */ \
    less_than,               /* < | >                             */ \
    less_equals,             /* <= | >=                           */ \
    assignment,              /* =                                 */ \
    /* unary                                                      */ \
    negate,                  /* unary -                           */ \
    logic_not,               /* !                                 */ \
    bitwise_not,             /* ~                                 */ \
    expression_statement,    /* expression statement              */ \
    function_call,           /* function call                     */ \
    /* ----------------------------------------                   */ \
    /* primary                                                    */ \
    literal,                 /* int | float | string              */ \
    identifier,              /* (variable | function) name        */ \
                             /* | (enum | struct | union) member  */ \
    /* ----------------------------------------                   */ \
    /* statements                                                 */ \
    /* if, for, while, compound-statement, etc                    */ \
    return_statement,        /* return                            */ \
    /* ----------------------------------------                   */ \
    /* scopes                                                     */ \
    compound_statement,      /* {...}                             */ \
    initializer_list,        /*                                   */ \
    translation_unit,        /*                                   */ \
    /* ----------------------------------------                   */ \
    /* top levels                                                 */ \
    expression,              /*                                   */ \
    statement,               /*                                   */ \
    variable_declaration,    /*                                   */ \
    function_declaration

    all_node_kinds
};

struct ASTNode; struct Unary; struct Binary; struct If; struct For; struct Variable; struct Primary;
struct Function; struct Scope; 

using NodeBranch = std::variant<Primary, Unary, Binary, Variable, Function, Scope>;

struct Primary {
    Literal value; // can also be an identifier
};
struct Unary {
    unique_ptr<ASTNode> expr;
};
struct Binary {
    unique_ptr<ASTNode> lhs;
    unique_ptr<ASTNode> rhs;
};
struct  Variable {
    Type type;
    std::string name;
    Opt<unique_ptr<ASTNode>> value;
};
struct Function {
    Type type;
    std::string name;
    vec<Variable> parameters; // if its empty we have no params
    Opt<unique_ptr<ASTNode>> body; // scope
};
// compound-statement | initializer-list | translation-unit | struct-body
struct Scope { 
    vec<ASTNode> nodes;
};

struct If {};
struct For {};

struct ASTNode {
    Token source_token; // token that originated this Node
    NodeKind kind;
    NodeBranch branch;

    constexpr operator std::unique_ptr<ASTNode>()&& { 
        return std::make_unique<ASTNode>(std::move(*this));
    }

    #define nd_kind(v_) case NodeKind::v_: return std::format("\033[38;2;142;163;217m{}\033[0m",#v_);
    [[nodiscard]] constexpr str kind_name() {
        switch (kind) {
            map_macro(nd_kind, all_node_kinds);
            default: PANIC(std::format("provided unknown NodeKind '{}'.", (int)kind));
        }
    }

    [[nodiscard]] constexpr str to_str(i64 depth);


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
inline size_t index_of(ASTNode& node) { return node.branch.index(); }
inline size_t index_of(const ASTNode& node) { return node.branch.index(); }
inline size_t index_of(std::shared_ptr<ASTNode>& node) { return node->branch.index(); }
inline size_t index_of(std::unique_ptr<ASTNode>& node) { return node->branch.index(); }
template<typename T> auto& get_elem(std::unique_ptr<ASTNode>& node) { return std::get<T>(node->branch); }
template<typename T> auto& get_elem(std::shared_ptr<ASTNode>& node) { return std::get<T>(node->branch); }
template<typename T> auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }
template<typename T> auto& get_elem(const ASTNode& node) { return std::get<T>(node.branch); }


#define match(v)                                                                                    \
{                                                                                                   \
    auto& v___ = v;                                                                                 \
    bool was_default = false;                                                                       \
    switch (auto t_seq___ = wrapped_type_seq(v); index_of(v)) {                                   

#define holds(T, ...)                                                                               \
    break;                                                                                          \
}                                                                                                   \
    case t_seq___.idx<std::remove_pointer<std ::remove_cvref<T>::type>::type>: {                    \
        __VA_OPT__(T __VA_ARGS__ =                                                                  \
            get_elem<std::remove_pointer                                                            \
                        <std::remove_cvref<decltype(__VA_ARGS__)>::type>::type>(v___);)

#define _                                                                                           \
        break;                                                                                      \
    }                                                                                               \
        default:                                                                                    \
            was_default = true;                                                                     \
}                                                                                                   \
    if (was_default)


#define gray(symbol) str("\033[38;2;134;149;179m") + str(symbol) + str("\033[0m")
#define yellow(symbol) str("\033[38;2;252;191;85m") + str(symbol) + str("\033[0m")
#define blue(symbol) str("\033[38;2;156;209;255m") + str(symbol) + str("\033[0m")
[[nodiscard]] constexpr str ASTNode::to_str(i64 depth = 0) {
    depth++;
    str result = std::format("{} ", kind_name());

    match(*this) {
        holds(Binary, &bin) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
            result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"), bin.lhs->to_str(depth));
            result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"), bin.rhs->to_str(depth));
        }
        holds(Unary, &unary) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
            depth--;
            result += std::format(" {} {}", gray("::="), unary.expr->to_str(depth));
        }
        holds(Primary, &primary) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
        }
        holds(Variable, &var) {
            result += std::format("{} {}", gray("=>"), yellow(var.name));
            result += gray(": ") + yellow(var.type.name);
            if (var.value) {
                result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"), var.value.value()->to_str(depth));
            }
        }
        holds(Function, &func) {
            str temp = yellow("fn ") + blue(func.name) + gray("(");
            for(size_t i = 0; i < func.parameters.size(); i++) {
                const auto& param = func.parameters.at(i);
                temp += param.name + gray(": ") + yellow(param.type.name);
                if(i != func.parameters.size() - 1) temp += gray(", ");
            }
            temp += gray(")");
            temp += gray(" -> ") + yellow(func.type.name);
            result += std::format("{} {}", gray("=>"), temp);
            if (func.body) result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"),
                                                 func.body.value()->to_str(depth)); 
        }
        holds(Scope, &scope) {
            result += gray("{");
            depth++;
            for(auto& node: scope.nodes) 
                result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"), node.to_str(depth));
            depth--;
            result += std::format("\n{}{}", str(depth * 2, ' '), gray("}"));
        }
        _ { PANIC(std::format("couldn't print node of kind: {}.", kind_name())); }
    }

    return result;
}
