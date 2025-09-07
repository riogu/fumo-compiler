#pragma once
#include <llvm/IR/Function.h>
#include "base_definitions/ast_node.hpp"
#include "base_definitions/symbol_table_stack.hpp"

struct Analyzer {
    Analyzer(const File& file) { file_stream << file.contents; }
    void semantic_analysis(ASTNode* file_scope);

    SymbolTableStack symbol_tree {};
    
    vec<unique_ptr<ASTNode>> extra_nodes {}; // used for member functions to allocate a parameter

  private:
    std::stringstream file_stream;

    void analyze(ASTNode& node);
    void report_binary_error(const ASTNode& node, const BinaryExpr& bin);

    ASTNode* create_main_node(Token& token) {
        // NOTE: not being used anymore
        auto id = push(ASTNode {token, Identifier {Identifier::declaration_name, "main"}});
        FunctionDecl function {FunctionDecl::function_declaration, id};

        auto* main_node = push(ASTNode {token, function});
        main_node->type = Type {push(ASTNode {token, Identifier {Identifier::type_name, "i32"}}), Type::i32_};

        return main_node;
    }
    void add_declaration(ASTNode& node);
    void iterate_qualified_names(FunctionDecl& func);

    ASTNode* push(const ASTNode& node) {
        extra_nodes.push_back(std::make_unique<ASTNode>(node));
        return extra_nodes.back().get();
    }
    void determine_type(ASTNode& node);

};

