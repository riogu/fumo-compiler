#pragma once
#include "type_system/token_definitions.hpp"
#include "type_system/type.hpp"
#include "utils/match_construct.hpp"
// clang-format off

enum struct NodeKind { 
#define all_node_kinds                                               \
    /* ----------------------------------------                   */ \
    /* simple expressions                                         */ \
    /* binary                                                     */ \
    /* empty_expr,*/              /* ;                            */ \
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
    /* ----------------------------------------                   */ \
    /* postfix                                                    */ \
    function_call,           /* function call                     */ \
    /* ----------------------------------------                   */ \
    /* primary                                                    */ \
    integer,                 /* i32 | int64_t                     */ \
    floating_point,          /*                                   */ \
    str,                     /*                                   */ \
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
    global_var_declaration,  /*                                   */ \
    variable_declaration,    /*                                   */ \
    function_declaration,    /*                                   */ \
    parameter

    all_node_kinds
};

struct ASTNode; 

struct PrimaryExpr {
    Literal value; // can also be an identifier
    Opt<ASTNode*> var_declaration {}; // identifiers map to this
};
struct UnaryExpr {
    ASTNode* expr;
};
struct BinaryExpr {
    ASTNode* lhs;
    ASTNode* rhs;
};
struct VariableDecl {
    std::string name;
    Opt<ASTNode*> value {};
};
struct FunctionDecl {
    std::string name;
    vec<ASTNode*> parameters {}; // if its empty we have no params
    Opt<ASTNode*> body {}; // scope
};

// compound-statement | initializer-list
struct BlockScope {
    vec<ASTNode*> nodes {};
};
// block scopes arent used for name lookups after semantic analysis
// once all identifiers are solved, these block scopes arent needed for name lookups
// they are only used for bodies of function calls/if statements and so on 
struct If {}; struct For {};


struct ASTNode {

    using NodeBranch = std::variant<PrimaryExpr, UnaryExpr, BinaryExpr, VariableDecl, FunctionDecl, BlockScope>;

    Token source_token; // token that originated this Node
    NodeKind kind;
    NodeBranch branch;
    Type type {};

    constexpr operator std::unique_ptr<ASTNode>()&& { 
        return std::make_unique<ASTNode>(std::move(*this));
    }

    #define nd_kind(v_) case NodeKind::v_: return std::format("\033[38;2;142;163;217m{}\033[0m",#v_);
    [[nodiscard]] constexpr str kind_name() const {
        switch (kind) {
            map_macro(nd_kind, all_node_kinds);
            default: PANIC(std::format("provided unknown NodeKind '{}'.", (int)kind));
        }
    }

    [[nodiscard]] constexpr str to_str(int64_t depth) const;


};

constexpr auto wrapped_type_seq(ASTNode& node) { return type_sequence(node.branch); };
constexpr auto wrapped_type_seq(const ASTNode& node) { return type_sequence(node.branch); };
inline size_t index_of(ASTNode& node) { return node.branch.index(); }
inline size_t index_of(const ASTNode& node) { return node.branch.index(); }
template<typename T> auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }
template<typename T> auto& get_elem(const ASTNode& node) { return std::get<T>(node.branch); }

#define gray(symbol) str("\033[38;2;134;149;179m") + str(symbol) + str("\033[0m")
#define yellow(symbol) str("\033[38;2;252;191;85m") + str(symbol) + str("\033[0m")
#define blue(symbol) str("\033[38;2;156;209;255m") + str(symbol) + str("\033[0m")
[[nodiscard]] constexpr str ASTNode::to_str(int64_t depth = 0) const {
    depth++;
    str result = std::format("{} ", kind_name());

    match(*this) {
        holds(PrimaryExpr) result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
        
        holds(const UnaryExpr&, unary) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
            depth--;
            result += std::format(" {} {}", gray("::="), unary.expr->to_str(depth));
        }
        holds(const BinaryExpr&, bin) {
            result += std::format("{}{}{}", gray("⟮"), source_token.to_str(), gray("⟯"));
            result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"), bin.lhs->to_str(depth));
            result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"), bin.rhs->to_str(depth));
        }
        holds(const VariableDecl&, var) {
            result += std::format("{} {}", gray("=>"), yellow(var.name));
            result += gray(": ") + yellow(type.name);
            if (var.value) {
                result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"), var.value.value()->to_str(depth));
            }
        }
        holds(const FunctionDecl&, func) {
            str temp = yellow("fn ") + blue(func.name) + gray("(");
            for(size_t i = 0; i < func.parameters.size(); i++) {
                ASTNode* var = func.parameters.at(i);
                match(*var) {
                    holds(const VariableDecl&, param) {
                        temp += param.name + gray(": ") + yellow(var->type.name);
                        if (param.value) temp += " = " + param.value.value()->to_str();
                    }
                    _ { INTERNAL_PANIC("function parameter must be a variable'{}'", kind_name()); }
                }
                if (i != func.parameters.size() - 1) temp += gray(", ");
            }
            temp += gray(")");
            temp += gray(" -> ") + yellow(type.name);
            result += std::format("{} {}", gray("=>"), temp);
            if (func.body) result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"),
                                                 func.body.value()->to_str(depth)); 
        }
        holds(const BlockScope&, scope) {
            result += gray("{");
            depth++;
            for(auto& node: scope.nodes) 
                result += std::format("\n{}{} {}", str(depth * 2, ' '), gray("↳"), node->to_str(depth));
            depth--;
            result += std::format("\n{}{}", str(depth * 2, ' '), gray("}"));
        }
        _ { PANIC(std::format("couldn't print node of kind: {}.", kind_name())); }
    }

    return result;
}
