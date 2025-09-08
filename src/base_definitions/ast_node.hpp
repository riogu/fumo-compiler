#pragma once
#include "base_definitions/tokens.hpp"
#include "base_definitions/types.hpp"
#include "utils/match_construct.hpp"
#include <llvm/IR/Value.h>

struct ASTNode; 
// NOTE: identifiers always hold declarations, but we distinguish the codegen for them
// based on if they are declaration_name or if they are one of the unsolved_names

struct Identifier {
    enum Kind {
        #define Identifier_kinds                                         \
        unknown_name,           /*                                   */  \
        declaration_name,       /*                                   */  \
        type_name,              /*                                   */  \
        var_name,               /*                                   */  \
        member_var_name,        /*                                   */  \
        func_call_name,         /*                                   */  \
        member_func_call_name   /*                                   */  \

        Identifier_kinds
    } kind;
    str name = "";
    enum Qualifier { unqualified, qualified } qualifier = unqualified;
    Opt<ASTNode*> declaration {}; // identifiers are solved to this
    str mangled_name = name;
    
    int scope_counts = 0;
    bool is_assigned_to = false;
};

struct PrimaryExpr {
    enum Kind {
        #define PrimaryExpr_kinds                                        \
        /* empty_expr,*/         /* ;                                 */ \
        integer,                 /* i32 | i64 | i8                    */ \
        floating_point,          /*                                   */ \
        str                      /*                                   */ \

        PrimaryExpr_kinds
    } kind;
    Literal value; // NOTE: not an identifier
};
struct UnaryExpr {
    enum Kind {
        #define UnaryExpr_kinds                                          \
        negate,                  /* unary -                           */ \
        logic_not,               /* !                                 */ \
        bitwise_not,             /* ~                                 */ \
        return_statement,        /* return                            */ \
        dereference,             /* return                            */ \
        address_of               /* return                            */ \

        UnaryExpr_kinds
    } kind;
    ASTNode* expr;
};

struct BinaryExpr {
    enum Kind {
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
    enum Kind {
        #define PostfixExpr_kinds                                        \
        postfix_expr             /*                                   */ \
    
        PostfixExpr_kinds
    } kind;
    vec<ASTNode*> nodes {};
    // bool is_assigned_to = false;
};

struct VariableDecl {
    enum Kind {
        #define VariableDecl_kinds                                       \
        global_var_declaration,  /*                                   */ \
        variable_declaration,    /*                                   */ \
        parameter                /*                                   */ \

        VariableDecl_kinds
    } kind;
    ASTNode* identifier;
};

struct FunctionDecl {
    enum Kind {
        #define FunctionDecl_kinds                                       \
        function_declaration,    /*                                   */ \
        member_func_declaration  /*                                   */ \

        FunctionDecl_kinds
    } kind;
    ASTNode* identifier;
    vec<ASTNode*> parameters {}; // if its empty we have no params
    Opt<ASTNode*> body {}; // compound statement {...}
    bool body_should_move = false;
};

struct FunctionCall {
    enum Kind {
        #define FunctionCall_kinds                                       \
        function_call,           /*                                   */ \
        member_function_call     /*                                   */ \
        
        FunctionCall_kinds
    } kind;
    ASTNode* identifier;
    vec<ASTNode*> argument_list {}; // if its empty we have no args
};

struct BlockScope {
    enum Kind {
        #define BlockScope_kinds                                         \
        compound_statement,      /* {...}                             */ \
        initializer_list         /*                                   */ \

        BlockScope_kinds
    } kind;
    vec<ASTNode*> nodes {};
};

struct NamespaceDecl {
    enum Kind {
        #define NamespaceDecl_kinds                                      \
        translation_unit,        /*                                   */ \
        namespace_declaration    /*                                   */ \

        NamespaceDecl_kinds
    } kind;
    ASTNode* identifier;
    vec<ASTNode*> nodes {};
};

// a struct/enum Kind introduces a named scope when defined
struct TypeDecl {
    enum Kind {
        #define TypeDecl_kinds                                           \
        struct_declaration        /*                                  */ \
        /* enum_declaration */    /*                                  */ \

        TypeDecl_kinds
    } kind;
    Opt<vec<ASTNode*>> definition_body {};
};

struct ASTNode {

    using NodeBranch = std::variant<Identifier, FunctionCall,
                                    PrimaryExpr,  UnaryExpr,    BinaryExpr, PostfixExpr,
                                    VariableDecl, FunctionDecl, TypeDecl,
                                    BlockScope,   NamespaceDecl>;

    Token source_token; // token that originated this Node
    NodeBranch branch;
    Type type {};

    llvm::Value* llvm_value {};
    bool is_llvm_ptr = false; // used to know if we are an address to be assigned to
                              // (aka, we are the target of a 'store' instruction)

    [[nodiscard]] std::string to_str(int64_t depth = 0) const;
    [[nodiscard]] std::string kind_name() const;
    [[nodiscard]] std::string branch_name() const;
    [[nodiscard]] std::string name() const { return yellow(branch_name()) + gray("::") + enum_green(kind_name()); }
};

#define get_name(branch_) get_id(branch_).name
#define get_id(branch_)                                                             \
(*({ if (!branch_.identifier) [[unlikely]] {                                        \
        INTERNAL_PANIC("forgot to initialize an Identifier for '{}'.", #branch_);   \
     }                                                                              \
    &get<Identifier>(branch_.identifier);                                           \
   }))

template<typename Branch>
constexpr Branch& get(ASTNode* node) {
    if(!node) [[unlikely]] INTERNAL_PANIC("node was uninitialized (FIX THIS).");
    if (std::holds_alternative<Branch>(node->branch)) return std::get<Branch>(node->branch);
    else [[unlikely]]
        INTERNAL_PANIC("node didn't hold this branch, held '{}'", node->name());
}
template<typename... Branches>
constexpr bool is_branch(const ASTNode* node) {
    return (std::holds_alternative<Branches>(node->branch) || ...);
}
#define or_error(...) ; if(!held) report_error(__VA_ARGS__); branch_;});

template<typename Branch>
constexpr Branch* get_if(ASTNode* node) {
    if (std::holds_alternative<Branch>(node->branch)) return {&std::get<Branch>(node->branch)};
    else
        return nullptr;
}

constexpr auto wrapped_type_seq(ASTNode& node) { return type_sequence(node.branch); };
constexpr auto wrapped_type_seq(const ASTNode& node) { return type_sequence(node.branch); };
constexpr std::size_t index_of(ASTNode& node) { return node.branch.index(); }
constexpr std::size_t index_of(const ASTNode& node) { return node.branch.index(); }
template<typename T> constexpr auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }
template<typename T> constexpr auto& get_elem(const ASTNode& node) { return std::get<T>(node.branch); }


[[nodiscard]] constexpr str type_name(Type type) {
    str temp = get_id(type).mangled_name;
    while (type.ptr_count--) temp += "*";
    return temp;
}
// Type checking functions
[[nodiscard]] constexpr bool is_arithmetic_t(const Type& type) {
    return (type.kind == Type::i8_  || type.kind == Type::i32_ || type.kind == Type::i64_
         || type.kind == Type::f32_ || type.kind == Type::f64_ || type.kind == Type::bool_);
}
[[nodiscard]] constexpr bool is_compatible_t(const Type& a, const Type& b) {
    return ((is_arithmetic_t(a) && is_arithmetic_t(b) && a.ptr_count == b.ptr_count)
         || (a.kind == b.kind && type_name(a) == type_name(b)));
}
[[nodiscard]] constexpr bool is_same_t(const Type& a, const Type& b) {
    return (a.kind == b.kind && type_name(a) == type_name(b));
}
