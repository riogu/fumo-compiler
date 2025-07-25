#pragma once
#include "type_system/token_definitions.hpp"
#include "type_system/type.hpp"
#include "utils/match_construct.hpp"

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
                             /* | (enum | struct | union) member  */ \
    /* ----------------------------------------                   */ \
    /* primary                                                    */ \
    integer,                 /* i32 | int64_t                     */ \
    floating_point,          /*                                   */ \
    str,                     /*                                   */ \
    identifier,              /* (variable | function) name        */ \
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
    parameter,               /*                                   */ \
    struct_declaration,      /*                                   */ \
    enum_declaration        /*                                   */ \

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
// block scopes arent used for name lookups after semantic analysis
struct BlockScope {
    vec<ASTNode*> nodes {};
};

struct StructDecl {};
struct EnumDecl {};
struct NamespaceDecl {};

// struct IfStmt {}; struct ForStmt {};
struct ASTNode {

    using NodeBranch = std::variant<PrimaryExpr, UnaryExpr, BinaryExpr,
                                    VariableDecl, FunctionDecl, BlockScope,
                                    StructDecl, EnumDecl, NamespaceDecl>;

    Token source_token; // token that originated this Node
    NodeKind kind;
    NodeBranch branch;
    Type type {};

    constexpr operator std::unique_ptr<ASTNode>()&& { return std::make_unique<ASTNode>(std::move(*this)); }

    [[nodiscard]] str to_str(int64_t depth = 0) const;
    [[nodiscard]] str kind_name() const;

};

constexpr auto wrapped_type_seq(ASTNode& node) { return type_sequence(node.branch); };
constexpr auto wrapped_type_seq(const ASTNode& node) { return type_sequence(node.branch); };
inline size_t index_of(ASTNode& node) { return node.branch.index(); }
inline size_t index_of(const ASTNode& node) { return node.branch.index(); }
template<typename T> constexpr auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }
template<typename T> constexpr auto& get_elem(const ASTNode& node) { return std::get<T>(node.branch); }
