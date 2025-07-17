#pragma once
#include "lexer/token_definitions.hpp"
#include "parser_errors.hpp"
#include <memory>
#include <vector>
// clang-format off

enum struct NodeKind { 
#define all_node_kinds                                                  \
    /* ----------------------------------------                   */    \
    /* simple expressions                                         */    \
    /* binary                                                     */    \
    empty_expr,              /* ;                                 */    \
    add,                     /* +                                 */    \
    sub,                     /* -                                 */    \
    multiply,                /* *                                 */    \
    divide,                  /* /                                 */    \
    equal,                   /* ==                                */    \
    not_equal,               /* !=                                */    \
    less_than,               /* < | >                             */    \
    less_equals,             /* <= | >=                           */    \
    assignment,              /* =                                 */    \
    /* unary                                                      */    \
    negate,                  /* unary -                           */    \
    logic_not,               /* !                                 */    \
    bitwise_not,             /* ~                                 */    \
    expression_statement,    /* expression statement              */    \
    /* ----------------------------------------                   */    \
    literal,                 /* int | float | string              */    \
    identifier,              /* (variable | function) name        */    \
                             /* | (enum |struct |union) member    */    \
    function_call,           /* function call                     */    \
    /* ----------------------------------------                   */    \
    /* statements                                                 */    \
    /* if, for, while, compound-statement, etc                    */    \
    compound_statement,      /* {...}                             */    \
    return_statement,        /* return                            */    \
    /* ----------------------------------------                   */    \
    /* top levels                                                 */    \
    expression,              /* expression                        */    \
    statement,               /* statement                         */    \
    variable_declaration,    /* variable                          */    \
    function_declaration,    /* function                          */    \
    initializer_list         /* function                          */

    all_node_kinds
};

template<typename T>
using unique_ptr = std::unique_ptr<T>;
struct ASTNode;
struct Unary; struct Binary; struct If; struct For; struct Variable; struct Primary; struct Function; struct Member; struct Scope; struct InitializerList;

using NodeBranch = std::variant<Unary, Binary, If, For, Variable, Primary, Function, Member, Scope, InitializerList>;

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
struct If {};struct For {};struct Member {};

struct Variable {
    std::string identifier;
    Opt<unique_ptr<ASTNode>> initializer;
};
struct InitializerList {
    Vec<unique_ptr<ASTNode>> nodes;
};
struct Scope {};struct Function {};

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

    [[nodiscard]] Vec<unique_ptr<ASTNode>> parse_tokens(Vec<Token>& tokens);
    std::stringstream file_stream;

  private:
    Vec<Token> tokens;
    std::vector<Token>::iterator curr_tkn;
    std::vector<Token>::iterator prev_tkn;

    // based on BNF for C99 with modifications (notes/current_bnf.md)
    // expressions
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
    // declarations
    [[nodiscard]] unique_ptr<ASTNode> variable_declaration();
    [[nodiscard]] unique_ptr<ASTNode> function_declaration();
    // misc
    [[nodiscard]] unique_ptr<ASTNode> declaration_specifier();
    [[nodiscard]] unique_ptr<ASTNode> initializer_list();
    [[nodiscard]] unique_ptr<ASTNode> initializer();


//  #define token_is(tok) (std::print("is_tkn '{}' == '{}' ?\n", curr_tkn->to_str(), #tok), is_tkn(tkn(tok)))
    #define token_is_str(tok) (is_tkn(str_to_tkn_type(tok)))
    #define token_is(tok, ...) (is_tkn(tkn(tok)) __VA_OPT__(&& is_tkn_keyword(#__VA_ARGS__)))
    constexpr bool is_tkn(const TokenType& type) {
        return curr_tkn != tokens.end()
               && ((curr_tkn)->type == type) ? ({ // std::print("consumed: '{}'\n", curr_tkn->to_str());
                                                  prev_tkn = curr_tkn; curr_tkn++; true; })
                                             : false;
    }

    #define peek_token_str(tok) peek_is_tkn(str_to_tkn_type(tok))
    #define peek_token(tok) peek_is_tkn(tkn(tok))
    constexpr bool peek_is_tkn(const TokenType& type) {
        return curr_tkn != tokens.end() && ((curr_tkn)->type == type);
                                           
    }

    constexpr bool is_tkn_keyword(std::string_view keyword) {
        return std::get<str>(prev_tkn->value.value()) == keyword;
    }

    #define expect_token(tok) consume_tkn_or_error(tkn(tok), #tok)
    #define expect_token_str(tok) consume_tkn_or_error(str_to_tkn_type(tok), tok)
    void consume_tkn_or_error(const TokenType& type, std::string_view repr) {
        if (!is_tkn(type)) {
            prev_tkn->line_offset++;
            report_error(prev_tkn, "expected '{}'.", repr);
        }
    }
};


[[nodiscard]] constexpr str ASTNode::to_str(i64 depth = 0) {
    depth++;
    str result = std::format("{} ", kind_name());

    match(*this) {
        holds(Binary, &bin) {
            result += std::format("\033[38;2;134;149;179m⟮\033[0m{}\033[38;2;134;149;179m⟯\033[0m", source_token.to_str());
            result += std::format("\n{}\033[38;2;134;149;179m↳\033[0m {}", str(depth * 2, ' '), bin.lhs->to_str(depth));
            result += std::format("\n{}\033[38;2;134;149;179m↳\033[0m {}", str(depth * 2, ' '), bin.rhs->to_str(depth));
        }
        holds(Unary, &unary) {
            result += std::format("\033[38;2;134;149;179m⟮\033[0m{}\033[38;2;134;149;179m⟯\033[0m", source_token.to_str());
            depth--;
            result += std::format(" \033[38;2;134;149;179m::=\033[0m {}", unary.expr->to_str(depth));
        }
        holds(Primary, &primary) {
            result += std::format("\033[38;2;134;149;179m⟮\033[0m{}\033[38;2;134;149;179m⟯\033[0m", source_token.to_str());
        }
        holds(InitializerList, &init_list) {
            depth++;
            result += "\033[38;2;134;149;179m{\033[0m";
            for(const auto& node: init_list.nodes) {
                result += std::format("\n{}\033[38;2;134;149;179m↳\033[0m {}", str(depth * 2, ' '), node->to_str(depth));
            }
            depth--;
            result += std::format("\n{}{}", str(depth * 2, ' '), "\033[38;2;134;149;179m}\033[0m");
        }
        // result += std::format(" \033[38;2;134;149;179m=>\033[0m '{}'", this->source_token.to_str());
        _ {
            PANIC(std::format("couldn't print node of kind: {}.", kind_name()));
        }
    }

    return result;
}
