#pragma once
#include "base_definitions/tokens.hpp"
#include "base_definitions/types.hpp"
#include "utils/match_construct.hpp"
#include <format>
#include <llvm/IR/Value.h>
enum struct ScopeKind { Namespace, TypeBody, CompoundStatement, FunctionBody, MemberFuncBody, MemberCompoundStatement};
struct Scope {
    str name;
    ScopeKind kind;
    str isolated_name;
    int inner_scope_count = 0;
};
struct ASTNode; 
// NOTE: identifiers always hold declarations, but we distinguish the codegen for them
// based on if they are declaration_name or if they are one of the unsolved_names

struct Identifier {
    enum Kind {
        #define Identifier_kinds                                         \
        unknown_name,                  /*                            */  \
        member_var_name,               /*                            */  \
        var_name,                      /*                            */  \
        type_name,                     /*                            */  \
        func_call_name,                /*                            */  \
        declaration_name,              /*                            */  \
        member_func_call_name,         /*                            */  \
        generic_wrapper_type_name,     /*                            */  \
        generic_type_name              /*                            */  \

        Identifier_kinds
    } kind;
    str name = "";

    enum Qualifier { unqualified, qualified } qualifier = unqualified;
    Opt<ASTNode*> declaration {}; // identifiers are solved to this
    str mangled_name = name;
    int scope_counts = 0;

    // generic identifiers only need to eventually become a flat instantiation of them
    // given:
    // node[T] { ... }
    // foo::bar[]() { ... }
    // it becomes
    // node[i32] { ... }
    // foo::bar[i32] { ... }
    // and thats it, then its easy to link usage to the instantiations
    // we want a "get or create" approach to template usage
    bool is_generic_wrapper() { return generic_identifiers.size(); }
    // we make a generic formatted name like:
    //   -> "foo::bar[{}]()" (this is stored normally in the 'name' variable)
    // without anything inside "[{}]"
    // that is what identifies uniquely each generic function
    // then, we use runtime_format() to apply the typenames to this formatted string
    // NOTE: this is used both for parameters in the declaration, and for arguments in the instantiation
    vec<ASTNode*> generic_identifiers {};
    str generic_alias = ""; // for keeping the original alias type names ('T', 'V', etc.)
    // (might be unused later)
    // each of these is an identifier
    // these are filled into the identifier on instantiation
    // (make a method to pass these into the non_generic_name)
    // we need to recursively solve identifiers one by one and fill out the final formatted string
    str base_struct_name = ""; // only used by member_var_name and member_func_call_name

};
str runtime_format(const str& fmt, auto&&... args) {
    return std::vformat(fmt, std::make_format_args(args...));
}

struct PrimaryExpr {
    enum Kind {
        #define PrimaryExpr_kinds                                        \
        /* empty_expr,*/         /* ;                                 */ \
        integer,                 /* i32 | i64 | i8                    */ \
        floating_point,          /*                                   */ \
        str,                     /*                                   */ \
        bool_,                   /*                                   */ \
        char_literal,            /*                                   */ \
        void_                    /*                                   */ \

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
    // made it optional to allow empty returns (otherwise it always has a value)
    Opt<ASTNode*> expr {};
};

struct BinaryExpr {
    enum Kind {
        #define BinaryExpr_kinds                                         \
        add,                     /* +                                 */ \
        sub,                     /* -                                 */ \
        multiply,                /* *                                 */ \
        divide,                  /* /                                 */ \
        modulus,                 /* /                                 */ \
        equal,                   /* ==                                */ \
        not_equal,               /* !=                                */ \
        less_than,               /* < | >                             */ \
        less_equals,             /* <= | >=                           */ \
        logical_and,             /* &&                                */ \
        logical_or,              /* ||                                */ \
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
    vec<int> member_indexes {};
};

struct VariableDecl {
    enum Kind {
        #define VariableDecl_kinds                                       \
        global_var_declaration,  /*                                   */ \
        variable_declaration,    /*                                   */ \
        member_var_declaration,  /*                                   */ \
        parameter                /*                                   */ \

        VariableDecl_kinds
    } kind;
    ASTNode* identifier;

    Opt<int> member_index {}; // used only by member_var_declaration
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
    bool is_variadic = false;
    bool is_static = false;
    bool body_should_move = false;
    vec<Scope> scopes;
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
    bool body_should_move = false;
};

struct IfStmt {
    enum Kind {
        #define IfStmt_kinds                                             \
        if_statement,            /*                                   */ \
        else_statement,          /*                                   */ \
        else_if_statement        /*                                   */ \

        IfStmt_kinds
    } kind;
    Opt<ASTNode*> condition {};
    ASTNode* body; // compound statement {...}
    Opt<ASTNode*> else_stmt {}; // is else, or else if 
};
struct WhileStmt {
    enum Kind {
        #define WhileStmt_kinds                                         \
        while_loop              /*                                   */ \

       WhileStmt_kinds 
    } kind = while_loop;
    ASTNode* condition;
    ASTNode* body; // compound statement {...}

};
#define NodeKinds  Identifier,   FunctionCall, IfStmt, WhileStmt,       \
                   PrimaryExpr,  UnaryExpr, BinaryExpr, PostfixExpr,    \
                   VariableDecl, FunctionDecl, TypeDecl,                \
                   BlockScope,   NamespaceDecl                          \

struct ASTNode {
    using NodeBranch = std::variant<Identifier, FunctionCall, IfStmt, WhileStmt,
                                    PrimaryExpr,  UnaryExpr, BinaryExpr, PostfixExpr,
                                    VariableDecl, FunctionDecl, TypeDecl,
                                    BlockScope,   NamespaceDecl>;

    Token source_token; // token that originated this Node
    NodeBranch branch;
    Type type {};

    // can be the if statement's "end.if" block, 
    // can be the stack pointer of a declaration that an identifier is tied to, etc.
    // this member is used to keep necessary shared state between nodes in the codegen,
    // whenever an llvm:Value ties various members together.
    llvm::Value* llvm_value = nullptr; 

    [[nodiscard]] std::string to_str(int64_t depth = 0) const;
    [[nodiscard]] std::string kind_name() const;
    [[nodiscard]] std::string branch_name() const;
    [[nodiscard]] std::string name() const { return yellow(branch_name()) + gray("::") + enum_green(kind_name()); }
};

#define get_name(branch_) get_id(branch_).name
#define get_id(branch_)                                                             \
(*({ if (!branch_.identifier) [[unlikely]] {                                        \
        internal_panic("forgot to initialize an Identifier for '{}'.", #branch_);   \
     }                                                                              \
    &get<Identifier>(branch_.identifier);                                           \
   }))

template<typename Branch>
constexpr Branch& get(ASTNode* node) {
    if(!node) [[unlikely]] internal_panic("node was uninitialized (FIX THIS).");
    if (std::holds_alternative<Branch>(node->branch)) return std::get<Branch>(node->branch);
    else [[unlikely]]
        internal_panic("node didn't hold this branch, held '{}'", node->name());
}
template<typename... Branches>
constexpr bool is_branch(const ASTNode* node) {
    return (std::holds_alternative<Branches>(node->branch) || ...);
}

template<typename Branch>
constexpr Branch* get_if(ASTNode* node) {
    if (std::holds_alternative<Branch>(node->branch)) return {&std::get<Branch>(node->branch)};
    else
        return nullptr;
}

#define if_value(...)                           \
({                                              \
    auto* temp = __VA_ARGS__.value_or(nullptr);
#define else_error(...)                         \
    if (!temp) report_error(__VA_ARGS__);       \
    temp;                                       \
});
#define else_panic(...)                         \
    if (!temp) internal_panic(__VA_ARGS__);     \
    temp;                                       \
});
#define else_panic_error(...)                   \
    if (!temp) internal_error(__VA_ARGS__);     \
    temp;                                       \
});

constexpr auto wrapped_type_seq(ASTNode& node) { return type_sequence(node.branch); };
constexpr auto wrapped_type_seq(const ASTNode& node) { return type_sequence(node.branch); };
constexpr std::size_t index_of(ASTNode& node) { return node.branch.index(); }
constexpr std::size_t index_of(const ASTNode& node) { return node.branch.index(); }
template<typename T> constexpr auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }
template<typename T> constexpr auto& get_elem(const ASTNode& node) { return std::get<T>(node.branch); }


[[nodiscard]] constexpr str flatten_generic_id(ASTNode* node) {
    auto& id = get<Identifier>(node);
    
    if (!id.is_generic_wrapper()) {
        return id.mangled_name; // Simple case: just "T" or "foo::bar::thing"
    }
    
    // id.mangled_name might be: "foo[{}]::bar[{}]::thing"
    // We need to fill all the {} placeholders
    
    std::vector<str> string_args;
    for (auto* child_node : id.generic_identifiers) {
        string_args.push_back(flatten_generic_id(child_node)); // Recursive
    }
    
    // Manual format replacement since std::format is giving you trouble
    str result = id.mangled_name;
    size_t pos = 0;
    for (size_t i = 0; i < string_args.size(); ++i) {
        pos = result.find("{}", pos);
        if (pos != str::npos) {
            result.replace(pos, 2, string_args[i]); // Replace "{}" with actual type
            pos += string_args[i].length();
        }
    }
    
    return result;
}
[[nodiscard]] constexpr str type_name(const Type& type) {
    auto& id = get_id(type);
    str temp = id.mangled_name;
    
    if (id.is_generic_wrapper()) {
        std::vector<str> string_args;
        for (auto* node : id.generic_identifiers) {
            string_args.push_back(flatten_generic_id(node));
        }
        // Manual format replacement (same as flatten_generic_id)
        size_t pos = 0;
        for (size_t i = 0; i < string_args.size(); ++i) {
            pos = temp.find("{}", pos);
            if (pos != str::npos) {
                temp.replace(pos, 2, string_args[i]); // Replace "{}" with actual type
                pos += string_args[i].length();
            }
        }
    }
    int ptr_count = type.ptr_count;
    while (ptr_count--) temp += "*";
    std::cerr << "created type_name: " << temp << "\n";
    return temp;
}
// Type checking functions
[[nodiscard]] constexpr bool is_int_t(const Type& type) {
    return (type.kind == Type::u8_ || type.kind == Type::u32_ || type.kind == Type::u64_ ||
            type.kind == Type::i8_ || type.kind == Type::i32_ || type.kind == Type::i64_) && !type.ptr_count;
}

[[nodiscard]] constexpr bool is_signed_t(const Type& type) {
    return (type.kind == Type::f32_|| type.kind == Type::f64_ ||
            type.kind == Type::i8_ || type.kind == Type::i32_ || type.kind == Type::i64_) && !type.ptr_count;
}
[[nodiscard]] constexpr bool is_float_t(const Type& type) {
    return (type.kind == Type::f32_ || type.kind == Type::f64_) && !type.ptr_count;
}
[[nodiscard]] constexpr bool is_ptr_t(const Type& type) { return type.ptr_count; }
[[nodiscard]] constexpr bool is_arithmetic_t(const Type& type) {
    return (type.kind == Type::u8_  || type.kind == Type::u32_ || type.kind == Type::u64_ || type.kind == Type::char_ ||
            type.kind == Type::i8_  || type.kind == Type::i32_ || type.kind == Type::i64_ ||
            type.kind == Type::f32_ || type.kind == Type::f64_ || type.kind == Type::bool_) && !type.ptr_count;
}
[[nodiscard]] constexpr bool is_same_t(const Type& a, const Type& b) {
    return (a.kind == b.kind && type_name(a) == type_name(b));
}
[[nodiscard]] constexpr bool is_compatible_t(const Type& a, const Type& b) {

    if ((a.kind == Type::any_ || b.kind == Type::any_) && a.ptr_count == b.ptr_count) return true;

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

