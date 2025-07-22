#include "codegen/llvm_codegen.hpp"

llvm::Value* Codegen::codegen(ASTNode* node, Primary&    branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Unary&      branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Binary&     branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Variable&   branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Function&   branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Scope&      branch) { return {};}

void Codegen::codegen(vec<ASTNode>& AST) {
    for (auto& node : AST) codegen(&node);
}

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
