#include "parser/parser.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

struct Codegen {

    unique_ptr<llvm::LLVMContext> llvm_context;
    unique_ptr<llvm::IRBuilder<>> ir_builder;
    unique_ptr<llvm::Module> llvm_module;

    void codegen(const Vec<unique_ptr<ASTNode>>& AST);
};
