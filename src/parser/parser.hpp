#pragma once
#include "lexer/token_definitions.hpp"
#include <iostream>
#include <memory>
#include <vector>

// clang-format off


#define report_error(...)                                                               \
{                                                                                       \
    std::ifstream file_stream = std::ifstream(curr_tkn->file_name);                     \
    std::string line;                                                                   \
    file_stream.seekg(curr_tkn->file_offset, std::ios_base::beg);                       \
    std::getline(file_stream, line);                                                    \
                                                                                        \
    std::cerr << std::format("\n  | error in file '{}' at line {}:\n  | {}\n  |{}{}\n", \
                             curr_tkn->file_name,                                       \
                             curr_tkn->line_number,                                     \
                             line,                                                      \
                             std::string(curr_tkn->line_offset, ' ') + "^ ",            \
                             __VA_ARGS__);                                              \
    exit(1);                                                                            \
}

enum struct NodeKind {
    // binary
    add,                     // +                          
    sub,                     // -                      
    multiply,                // *                      
    divide,                  // /
    equal,                   // ==
    not_equal,               // !=                     
    less_than,               // < | >
    less_equals,             // <= | >=
    assignment,              // =
    // unary
    expression_statement,    // Expression statement
    negate,                  // unary -
    logic_not,               // !
    bitwise_not,             // ~
    // variable
    variable,                // Variable
    // literal
    integer,                 // Integer
};

template<typename T>
using unique_ptr = std::unique_ptr<T>;
struct ASTNode;
// using ASTNode_ptr = std::unique_ptr<ASTNode>;
struct Unary; struct Binary; struct If; struct For; struct Variable; 
struct Literal; struct Function; struct Member; struct Scope;

using NodeBranch = std::variant<Unary, Binary, If, For, Variable, Literal, Function, Member, Scope>;

struct Literal {};
struct Unary {
    unique_ptr<ASTNode> expr;
};
struct Binary {
    unique_ptr<ASTNode> lhs;
    unique_ptr<ASTNode> rhs;
};
struct If {};
struct For {};
struct Member {};
struct Variable {};
struct Scope {};
struct Function {};

struct ASTNode {

    Token token; // token that originated this Node
    NodeKind kind;
    NodeBranch branch;

    constexpr operator std::unique_ptr<ASTNode>() { 
        return std::make_unique<ASTNode>(std::move(*this));
    }


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
template<typename T> auto& get_elem(std::unique_ptr<ASTNode>& node) { return std::get<T>(node->branch); }
template<typename T> auto& get_elem(std::shared_ptr<ASTNode>& node) { return std::get<T>(node->branch); }
template<typename T> auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }


#define match(v)                                                                                    \
{                                                                                                   \
    auto& __v__ = v;                                                                                \
    bool was_default = false;                                                                       \
    switch (auto __t_seq__ = wrapped_type_seq(v); index_of(v)) {                                   

#define holds(T, name)                                                                              \
    break;                                                                                          \
}                                                                                                   \
    case __t_seq__.idx<std::remove_pointer<std ::remove_cvref<T>::type>::type>: {                   \
        T name=get_elem<std::remove_pointer<std::remove_cvref<decltype(name)>::type>::type>(__v__);

#define _                                                                                           \
        break;                                                                                      \
    }                                                                                               \
        default:                                                                                    \
            was_default = true;                                                                     \
}                                                                                                   \
    if (was_default)

// clang-format on

struct Parser {
    Vec<Token> tokens;
    std::vector<Token>::iterator curr_tkn;
    Result<Vec<unique_ptr<ASTNode>>, str> parse_tokens(Vec<Token>& tokens);

    // based on BNF for C99 with modifications (notes/current_bnf.md)
    [[nodiscard]] unique_ptr<ASTNode> statement();
    [[nodiscard]] unique_ptr<ASTNode> expression_statement();
    [[nodiscard]] unique_ptr<ASTNode> expression();
    [[nodiscard]] unique_ptr<ASTNode> assignment();
    [[nodiscard]] unique_ptr<ASTNode> equality();
    [[nodiscard]] unique_ptr<ASTNode> relational();
    [[nodiscard]] unique_ptr<ASTNode> add();
    [[nodiscard]] unique_ptr<ASTNode> multiply();
    [[nodiscard]] unique_ptr<ASTNode> unary();
    [[nodiscard]] unique_ptr<ASTNode> primary();

    // print nice errors
    void report_parser_error(std::string_view error);
    // clang-format off


#define tkn_is(tok) consume_tkn(tkn(tok))
    constexpr bool consume_tkn(const TokenType& type) {
        return (curr_tkn->type == type) ? ({ curr_tkn++; true; }) : false;
    }

#define expect_tkn(tok) consume_tkn_or_error(tkn(tok), #tok)

    void consume_tkn_or_error(const TokenType& type, std::string_view repr) {
        if(!consume_tkn(type)) report_parser_error(std::format("expected '{}'", repr));
    }

    // clang-format on
};

inline void test_example_func() {
    ASTNode v = {
        .branch = Binary {
            .lhs = ASTNode {.kind = NodeKind::add,
                            .branch = Binary {ASTNode {}, std::make_unique<ASTNode>()}},
            .rhs = ASTNode {},
        }};

    // doesnt matter if its an ASTNode or a unique/shared_ptr, same API
    match(v) {
        holds(Binary, &bin_var) { // have to take by reference
            match(bin_var.lhs) {
                holds(Unary, &unaryvar) {}
                _ {}
            }
        }
        holds(If, &if_var) {
            switch (v.kind) {
                case NodeKind::add:
                case NodeKind::variable:
                case NodeKind::integer:
                    break;
                default:
                    break;
            }
        }
        _ std::cerr << "type unknown.\n";
    }
}
