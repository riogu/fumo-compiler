#include "parser/ast_node.hpp"
#include <llvm/IR/IRBuilder.h>
#include <map>

struct Codegen {

    unique_ptr<llvm::LLVMContext> llvm_context;
    unique_ptr<llvm::IRBuilder<>> ir_builder;
    unique_ptr<llvm::Module> llvm_module;
    std::map<str, llvm::Value*> symbol_table;

    void begin_codegen(vec<ASTNode>& AST);

    template<typename Branch>
    llvm::Value* codegen(ASTNode* node);
};
