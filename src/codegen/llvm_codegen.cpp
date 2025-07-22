#include "codegen/llvm_codegen.hpp"

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

llvm::Value* Codegen::codegen(ASTNode* node, Unary&      branch) { return {}; }

llvm::Value* Codegen::codegen(ASTNode* node, Binary& branch) {
    switch(node->kind){
        case NodeKind::add:
            return ir_builder->CreateAdd(codegen(branch.lhs.get()), codegen(branch.rhs.get()));
        case NodeKind::sub:
            return ir_builder->CreateSub(codegen(branch.lhs.get()), codegen(branch.rhs.get()));
        case NodeKind::multiply:
            return ir_builder->CreateMul(codegen(branch.lhs.get()), codegen(branch.rhs.get()));
        case NodeKind::divide:
            return ir_builder->CreateSDiv(codegen(branch.lhs.get()), codegen(branch.rhs.get()));
        case NodeKind::equal:
            return ir_builder->CreateICmpEQ(codegen(branch.lhs.get()), codegen(branch.rhs.get()));
        case NodeKind::not_equal:
            return ir_builder->CreateICmpNE(codegen(branch.lhs.get()), codegen(branch.rhs.get()));
        case NodeKind::less_than:
            return ir_builder->CreateICmpSLT(codegen(branch.lhs.get()), codegen(branch.rhs.get()));
        case NodeKind::less_equals:
            return ir_builder->CreateICmpSLE(codegen(branch.lhs.get()), codegen(branch.rhs.get()));
        case NodeKind::assignment:
            return ir_builder->CreateStore(codegen(branch.rhs.get()), codegen(branch.lhs.get()));
        default:
            PANIC("codegen not implemented for '" + node->kind_name() + "'.");
    }
}
llvm::Value* Codegen::codegen(ASTNode* node, Variable&   branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Function&   branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Scope&      branch) { return {};}


llvm::Value* Codegen::codegen(ASTNode* node) { 
    match(*node) {
        holds(Primary,  &v) return codegen(node, v);
        holds(Unary,    &v) return codegen(node, v);
        holds(Binary,   &v) return codegen(node, v);
        holds(Variable, &v) return codegen(node, v);
        holds(Function, &v) return codegen(node, v);
        holds(Scope,    &v) return codegen(node, v);
        _ PANIC("missing codegen implementation for node: " + node->kind_name());
    }
    std::unreachable();
}

void Codegen::codegen(vec<ASTNode>& AST) {
    for (auto& node : AST) codegen(&node);
}

/*


*/
