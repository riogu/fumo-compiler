#include "codegen/llvm_codegen.hpp"
#include <llvm/IR/Instructions.h>

llvm::Value* Codegen::codegen(ASTNode* node, Primary& branch) {
    switch (node->kind) {
        case NodeKind::integer:
            return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*llvm_context),
                                                std::get<int>(branch.value));
        case NodeKind::floating_point:
            return llvm::ConstantInt::getSigned(llvm::Type::getFloatTy(*llvm_context),
                                                std::get<double>(branch.value));
        case NodeKind::identifier:
            return variable_table[std::get<str>(branch.value)];
        case NodeKind::str:
        default:
            PANIC("codegen not implemented for '" + node->kind_name() + "'.");
            
    }
}

llvm::Value* Codegen::codegen(ASTNode* node, Unary&      branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Binary&     branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Variable&   branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Function&   branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Scope&      branch) { return {};}


llvm::Value* Codegen::codegen(ASTNode* node) { 
    match(*node) {
        holds(Primary,  &v) codegen(node, v);
        holds(Unary,    &v) codegen(node, v);
        holds(Binary,   &v) codegen(node, v);
        holds(Variable, &v) codegen(node, v);
        holds(Function, &v) codegen(node, v);
        holds(Scope,    &v) codegen(node, v);
        _ PANIC("missing codegen implementation for node: " + node->kind_name());
    }
    std::unreachable();
}

void Codegen::codegen(vec<ASTNode>& AST) {
    for (auto& node : AST) codegen(&node);
}

/*


*/
