#pragma once
#include "lexer/token_definitions.hpp"
#include <iostream>
#include <memory>
#include <print>
#include <vector>

// clang-format off


// print nice errors
#define report_error(tok, ...)                                                          \
{                                                                                       \
    file_stream.seekg(file_stream.beg);                                                 \
    std::string line;                                                                   \
    if (tok->line_number != 1) {                                                        \
        file_stream.seekg(tok->file_offset, std::ios_base::beg);                        \
        while(file_stream.peek() != '\n') {                                             \
            long pos = file_stream.tellg();                                             \
            file_stream.seekg(pos-1);                                                   \
        }                                                                               \
        file_stream.get();                                                              \
    }                                                                                   \
    std::getline(file_stream, line);                                                    \
                                                                                        \
    std::cerr << std::format("\n  | error in file '{}' at line {}:\n  | {}\n  |{}{}\n", \
                             tok->file_name,                                            \
                             tok->line_number,                                          \
                             line,                                                      \
                             std::string(tok->line_offset+1, ' ') + "^ ",               \
                             std::format(__VA_ARGS__));                                 \
    exit(1);                                                                            \
}
#define all_node_kinds       \
    add,                     \
    sub,                     \
    multiply,                \
    divide,                  \
    equal,                   \
    not_equal,               \
    less_than,               \
    less_equals,             \
    assignment,              \
    negate,                  \
    logic_not,               \
    bitwise_not,             \
    expression_statement,    \
    literal,                 \
    identifier,              \
    function_call,           \
    compound_statement,      \
    expression,              \
    statement,               \
    variable_declaration,    \
    function_declaration

enum struct NodeKind { 
    // NOTE: remember to update the list above when you add a new NodeKind

    // ----------------------------------------
    // simple expressions
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
    negate,                  // unary -
    logic_not,               // !
    bitwise_not,             // ~
    expression_statement,    // expression statement
    // ----------------------------------------
    literal,                 // int | float | string
    identifier,              // (variable | function) name 
    //                          | (enum |struct |union) member
    function_call,           // function call
    // ----------------------------------------
    // statements 
    // if, for, while, compound-statement, etc
    compound_statement,      // {...}
    // ----------------------------------------
    // top levels
    expression,              // expression
    statement,               // statement
    variable_declaration,    // variable
    function_declaration,    // function
};

template<typename T>
using unique_ptr = std::unique_ptr<T>;
struct ASTNode;
// using ASTNode_ptr = std::unique_ptr<ASTNode>;
struct Unary; struct Binary; struct If; struct For; struct Variable; 
struct Primary; struct Function; struct Member; struct Scope;

using NodeBranch = std::variant<Unary, Binary, If, For, Variable, Primary, Function, Member, Scope>;

struct Primary {
    Literal literal; // taken from the tokens
};
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

    constexpr operator std::unique_ptr<ASTNode>()&& { 
        return std::make_unique<ASTNode>(std::move(*this));
    }
    

#define nd_kind(v_) case NodeKind::v_: return #v_;
    [[nodiscard]] constexpr str kind_name() {
        switch (kind) {
            map_macro(nd_kind, all_node_kinds);
            default: PANIC(fmt("provided unknown NodeKind '{}'.", (int)kind));
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
inline size_t index_of(std::shared_ptr<ASTNode>& node) { return node->branch.index(); }
inline size_t index_of(std::unique_ptr<ASTNode>& node) { return node->branch.index(); }
inline size_t index_of(ASTNode& node) { return node.branch.index(); }
template<typename T> auto& get_elem(std::unique_ptr<ASTNode>& node) { return std::get<T>(node->branch); }
template<typename T> auto& get_elem(std::shared_ptr<ASTNode>& node) { return std::get<T>(node->branch); }
template<typename T> auto& get_elem(ASTNode& node) { return std::get<T>(node.branch); }


#define match(v)                                                                                    \
{                                                                                                   \
    auto& v___ = v;                                                                                 \
    bool was_default = false;                                                                       \
    switch (auto t_seq___ = wrapped_type_seq(v); index_of(v)) {                                   

#define holds(T, name)                                                                              \
    break;                                                                                          \
}                                                                                                   \
    case t_seq___.idx<std::remove_pointer<std ::remove_cvref<T>::type>::type>: {                    \
        T name=get_elem<std::remove_pointer<std::remove_cvref<decltype(name)>::type>::type>(v___);

#define _                                                                                           \
        break;                                                                                      \
    }                                                                                               \
        default:                                                                                    \
            was_default = true;                                                                     \
}                                                                                                   \
    if (was_default)

struct Parser {
    Vec<Token> tokens;
    std::vector<Token>::iterator curr_tkn;
    Vec<unique_ptr<ASTNode>> parse_tokens(Vec<Token>& tokens);
    std::vector<Token>::iterator prev_tkn;
    std::stringstream file_stream;

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

#define tkn_is(tok) (std::print("is_tkn '{}' == '{}' ?\n", curr_tkn->to_str(), #tok), is_tkn(tkn(tok)))
    constexpr bool is_tkn(const TokenType& type) {
            return curr_tkn != tokens.end() && ((curr_tkn)->type == type) ? ({ std::print("consumed: '{}'\n", curr_tkn->to_str());
                                                                               prev_tkn = curr_tkn; curr_tkn++; true; })
                                                                          : false;
    }

#define expect_tkn(tok) consume_tkn_or_error(tkn(tok), #tok)
    void consume_tkn_or_error(const TokenType& type, std::string_view repr) {
        if (!is_tkn(type)) report_error(prev_tkn, "expected '{}'.", repr);
    }
};

// clang-format on

[[nodiscard]] constexpr str ASTNode::to_str(i64 depth = 0) {
    str result {};
    depth++;

    match(*this) {

        holds(Binary, &bin) {
            result += std::format("{}:\n{}| ", kind_name(), std::string(depth * 2, ' '));
            result += bin.lhs->to_str(depth);
            result += bin.rhs->to_str(depth);
        }

        holds(Unary, &unary) {
            result += std::format("{}:\n{}| ", kind_name(), std::string(depth * 2, ' '));
            result += unary.expr->to_str(depth);
        }

        holds(Primary, &primary) {
            result += std::format("{} => ", kind_name());
            result += std::format("'{}' \n{}| ",
                                  this->token.to_str(),
                                  std::string(depth * 2, ' '));
        }

        _ {
            PANIC(std::format("couldn't print node of kind: {}.", kind_name()));
        }
    }

    return result;
}
