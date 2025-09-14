#pragma once
#include "base_definitions/ast_node.hpp"



struct Parser {
    Parser(const File & file) { file_stream << file.contents; }
    [[nodiscard]] ASTNode* parse_tokens(vec<Token>& tokens);

  private:
    vec<unique_ptr<ASTNode>> all_nodes {};
    vec<Token> tokens;
    std::vector<Token>::iterator curr_tkn;
    std::vector<Token>::iterator prev_tkn;
    std::stringstream file_stream;
    vec<ASTNode*> AST;

    ASTNode* push(const ASTNode& node) {
        all_nodes.push_back(std::make_unique<ASTNode>(node));
        return all_nodes.back().get();
    }

    // implemention of the FumoLang BNF (language_specification/fumo_bnf.md)
    // --------------------------------------------------------------
    // expressions
    [[nodiscard]] ASTNode* statement();
    [[nodiscard]] ASTNode* expression_statement();
    [[nodiscard]] Opt<ASTNode*> if_statement();
    [[nodiscard]] ASTNode* expression();
    [[nodiscard]] ASTNode* assignment();
    [[nodiscard]] ASTNode* initializer();
    [[nodiscard]] ASTNode* initializer_list();
    [[nodiscard]] Opt<ASTNode*> equality();
    [[nodiscard]] Opt<ASTNode*> relational();
    [[nodiscard]] Opt<ASTNode*> add();
    [[nodiscard]] Opt<ASTNode*> multiply();
    [[nodiscard]] Opt<ASTNode*> unary();
    [[nodiscard]] Opt<ASTNode*> postfix();
    [[nodiscard]] vec<ASTNode*> argument_list();
    [[nodiscard]] Opt<ASTNode*> primary();
    [[nodiscard]] ASTNode* identifier(Identifier::Kind id_kind = Identifier::unknown_name,
                                      Opt<ASTNode*> declaration = std::nullopt);
    // --------------------------------------------------------------
    // declarations
    [[nodiscard]] Type declaration_specifier();

    [[nodiscard]] ASTNode* namespace_declaration();
    [[nodiscard]] ASTNode* struct_declaration();
    [[nodiscard]] ASTNode* enum_declaration();
    [[nodiscard]] vec<ASTNode*> named_scope_definition();
    // variable
    [[nodiscard]] ASTNode* variable_declaration();
    // function
    [[nodiscard]] ASTNode* function_declaration();
    [[nodiscard]] ASTNode* compound_statement();
    [[nodiscard]] std::pair<vec<ASTNode*>, bool> parameter_list();
    // --------------------------------------------------------------

//  #define token_is(tok) (std::print("is_tkn '{}' == '{}' ?\n", curr_tkn->to_str(), #tok), is_tkn(tkn(tok)))
    #define token_is_str(tok) is_tkn(str_to_tkn_type(tok))
    #define token_is(tok) is_tkn(tkn(tok))
    constexpr bool is_tkn(const TokenType& type) {
        return curr_tkn != tokens.end() && ((curr_tkn)->type == type) ? prev_tkn = curr_tkn++, true : false;
    }

    #define peek_token_str(tok) peek_is_tkn(str_to_tkn_type(tok))
    #define peek_token(tok) peek_is_tkn(tkn(tok))

    constexpr bool peek_is_tkn(const TokenType& type) {
        return curr_tkn != tokens.end() && ((curr_tkn)->type == type);
    }

    #define peek_keyword(keyword) peek_tkn_keyword(#keyword)
    constexpr bool peek_tkn_keyword(str_view keyword) {
        return (curr_tkn->type == TokenType::keyword && std::get<str>(curr_tkn->literal.value()) == keyword);
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
    #define expect_token_str(tok) consume_tkn_or_error(str_to_tkn_type(tok), tok)
    void consume_tkn_or_error(const TokenType& type, std::string_view repr) {
        if (!is_tkn(type)) report_error((*curr_tkn), "expected '{}'.", repr);
    }
};
