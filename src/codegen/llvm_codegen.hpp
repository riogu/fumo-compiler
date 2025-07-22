#include "parser/ast_node.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <memory>
#include <map>


struct Codegen {
    std::map<str, llvm::AllocaInst*> variable_table;
    unique_ptr<llvm::LLVMContext> llvm_context;
    unique_ptr<llvm::IRBuilder<>> ir_builder;
    unique_ptr<llvm::Module> llvm_module;

    Codegen(str name) {
        llvm_context = std::make_unique<llvm::LLVMContext>();
        ir_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
        llvm_module = std::make_unique<llvm::Module>(name, *llvm_context);
    }

    void codegen(vec<ASTNode>& AST);
    llvm::Value* codegen(ASTNode* node);
    llvm::Value* codegen(ASTNode* node, Primary& branch);
    llvm::Value* codegen(ASTNode* node, Unary& branch);
    llvm::Value* codegen(ASTNode* node, Binary& branch);
    llvm::Value* codegen(ASTNode* node, Variable& branch);
    llvm::Value* codegen(ASTNode* node, Function& branch);
    llvm::Value* codegen(ASTNode* node, Scope& branch);


    void print_llvm_ir() { llvm_module->print(llvm::outs(), nullptr); }
    [[nodiscard]] str llvm_ir_to_str() {
        std::string outstr;
        llvm::raw_string_ostream oss(outstr);
        llvm_module->print(oss, nullptr);
        return oss.str();
    }
};
