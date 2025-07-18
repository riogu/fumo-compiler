#pragma once
#include "parser/ast_node.hpp"
#include "parser/parser_errors.hpp"

struct Parser {
    std::stringstream file_stream;
    [[nodiscard]] Vec<unique_ptr<ASTNode>> parse_tokens(Vec<Token>& tokens);

  private:
    Vec<Token> tokens;
    std::vector<Token>::iterator curr_tkn;
    std::vector<Token>::iterator prev_tkn;
    // based on BNF for C99 with modifications (notes/current_bnf.md)
    // --------------------------------------------------------------
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
    [[nodiscard]] unique_ptr<ASTNode> initializer_list();
    [[nodiscard]] unique_ptr<ASTNode> initializer();
    [[nodiscard]] unique_ptr<ASTNode> function_declaration();
    [[nodiscard]] Vec<Variable> parameter_list();
    [[nodiscard]] Type declaration_specifier();
    [[nodiscard]] Struct struct_declaration();
    [[nodiscard]] Enum enum_declaration();
    [[nodiscard]] unique_ptr<ASTNode> compound_statement();
    // --------------------------------------------------------------

//  #define token_is(tok) (std::print("is_tkn '{}' == '{}' ?\n", curr_tkn->to_str(), #tok), is_tkn(tkn(tok)))
    #define token_is_str(tok) is_tkn(str_to_tkn_type(tok))
    #define token_is(tok) is_tkn(tkn(tok))
    constexpr bool is_tkn(const TokenType& type) {
        return curr_tkn != tokens.end()
               && ((curr_tkn)->type == type) ? ({ // std::print("consumed: '{}'\n", curr_tkn->to_str());
                                                  prev_tkn = curr_tkn++; true; })
                                             : false;
    }

    #define peek_token_str(tok) peek_is_tkn(str_to_tkn_type(tok))
    #define peek_token(tok) peek_is_tkn(tkn(tok))

    constexpr bool peek_is_tkn(const TokenType& type) {
        return curr_tkn != tokens.end() && ((curr_tkn)->type == type);
    }
    #define token_is_keyword(keyword) is_tkn_keyword(#keyword)
    constexpr bool is_tkn_keyword(std::string_view keyword) {
        if (curr_tkn->type == TokenType::keyword && std::get<str>(curr_tkn->literal.value()) == keyword) {
            prev_tkn = curr_tkn++; 
            return true; 
        }
        return false;
    }

    #define expect_token(tok) consume_tkn_or_error(tkn(tok), #tok)
    #define expect_token_str(tok) consume_tkn_or_error(str_to_tkn_type(tok), #tok)
    void consume_tkn_or_error(const TokenType& type, std::string_view repr) {
        if (!is_tkn(type)) {
            prev_tkn->line_offset++;
            report_error(prev_tkn, "expected '{}'.", repr);
        }
    }
};
