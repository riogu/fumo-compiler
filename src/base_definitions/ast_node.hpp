#pragma once
#include "base_definitions/tokens.hpp"
#include "base_definitions/types.hpp"
#include "utils/match_construct.hpp"

struct ASTNode; 

// these ugly macros are just for printing the enums later in the debug

struct PrimaryExpr {
    enum {
        #define PrimaryExpr_kinds                                        \
        /* empty_expr,*/         /* ;                                 */ \
        integer,                 /* i32 | i64 | i8                    */ \
        floating_point,          /*                                   */ \
        str,                     /*                                   */ \
        identifier               /* (variable | function) name        */ \

        PrimaryExpr_kinds
    } kind;
    Literal value;                    // can also be an identifier
    Opt<ASTNode*> var_declaration {}; // identifiers map to this
};
struct UnaryExpr {
    enum {
        #define UnaryExpr_kinds                                          \
        negate,                  /* unary -                           */ \
        logic_not,               /* !                                 */ \
        bitwise_not,             /* ~                                 */ \
        return_statement         /* return                            */ \

        UnaryExpr_kinds
    } kind;
    ASTNode* expr;
};

struct BinaryExpr {
    enum {
        #define BinaryExpr_kinds                                         \
        add,                     /* +                                 */ \
        sub,                     /* -                                 */ \
        multiply,                /* *                                 */ \
        divide,                  /* /                                 */ \
        equal,                   /* ==                                */ \
        not_equal,               /* !=                                */ \
        less_than,               /* < | >                             */ \
        less_equals,             /* <= | >=                           */ \
        assignment               /* =                                 */ \

        BinaryExpr_kinds
    } kind;
    ASTNode* lhs;
    ASTNode* rhs;
};

struct PostfixExpr {
    enum {
        #define PostfixExpr_kinds                                        \
        member_access,           /*                                   */ \
        deref_member_access,     /*                                   */ \
        function_call            /*                                   */ \
    
        PostfixExpr_kinds
    } kind;
    ASTNode* lhs;
    ASTNode* rhs;
};


struct VariableDecl {
    enum {
        #define VariableDecl_kinds                                       \
        global_var_declaration,  /*                                   */ \
        variable_declaration,    /*                                   */ \
        parameter                /*                                   */ \

        VariableDecl_kinds
    } kind;
};
struct FunctionDecl {
    enum {
        #define FunctionDecl_kinds                                       \
        function_declaration     /*                                   */ \
    
        FunctionDecl_kinds
    } kind;
    vec<ASTNode*> parameters {}; // if its empty we have no params
    Opt<ASTNode*> body {}; // compound statement {...}
};

struct BlockScope {
    enum {
        #define BlockScope_kinds                                         \
        compound_statement,      /* {...}                             */ \
        initializer_list,        /*                                   */ \
        argument_list            /*                                   */ \

        BlockScope_kinds
    } kind;
    vec<ASTNode*> nodes {};
};

struct NamespaceDecl {
    enum {
        #define NamespaceDecl_kinds                                      \
        translation_unit,        /*                                   */ \
        namespace_declaration    /*                                   */ \

        NamespaceDecl_kinds
    } kind;
    vec<ASTNode*> nodes {};
};

// a struct/enum introduces a named scope when defined
struct TypeDecl {
    enum {
        #define TypeDecl_kinds                                           \
        struct_declaration        /*                                  */ \
        /* enum_declaration */    /*                                  */ \

        TypeDecl_kinds
    } kind;
    Opt<vec<ASTNode*>> definition_body {};
};

struct ASTNode {

    using NodeBranch = std::variant<PrimaryExpr, UnaryExpr, BinaryExpr, PostfixExpr,
                                    VariableDecl, FunctionDecl, TypeDecl,
                                    BlockScope, NamespaceDecl>;

    Token source_token; // token that originated this Node
    NodeBranch branch;
    str name = "";
    Type type {};
    str mangled_name = "";

    [[nodiscard]] std::string to_str(int64_t depth = 0) const;
    [[nodiscard]] std::string kind_name() const;

};

constexpr auto wrapped_type_seq(ASTNode& node) { return type_sequence(node.branch); };
constexpr auto wrapped_type_seq(const ASTNode& node) { return type_sequence(node.branch); };
inline size_t index_of(ASTNode& node) { return node.branch.index(); }
inline size_t index_of(const ASTNode& node) { return node.branch.index(); }
template<typename T> constexpr auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }
template<typename T> constexpr auto& get_elem(const ASTNode& node) { return std::get<T>(node.branch); }

