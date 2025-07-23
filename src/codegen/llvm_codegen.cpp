#include "codegen/llvm_codegen.hpp"

void Codegen::codegen(vec<ASTNode>& AST) {

    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), {}, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "main", llvm_module.get());

    llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", func);
    ir_builder->SetInsertPoint(bblock);

    for (auto& node : AST) codegen(&node);
}

llvm::Value* Codegen::codegen(ASTNode* node) { 

    match(*node) {
        holds(Primary,  &v) return codegen(node, v);
        holds(Unary,    &v) return codegen(node, v);
        holds(Binary,   &v) return codegen(node, v);
        holds(Variable, &v) return codegen(node, v);
        holds(Function, &v) return codegen(node, v);
        holds(Scope,    &v) return codegen(node, v);
        _ INTERNAL_PANIC("codegen not implemented for '{}'", node->kind_name());
    }
    std::unreachable();
}

llvm::Value* Codegen::codegen(ASTNode* node, Primary& primary) {

    switch (node->kind) {
        case NodeKind::integer:
            return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*llvm_context),
                                                std::get<int64_t>(primary.value));
        case NodeKind::floating_point:
            return llvm::ConstantInt::getSigned(llvm::Type::getFloatTy(*llvm_context),
                                                std::get<double>(primary.value));
        case NodeKind::identifier:
            // TODO: make a proper symbol table
            return variable_table[std::get<str>(primary.value)];
        case NodeKind::str:
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node->kind_name());
    }
}

llvm::Value* Codegen::codegen(ASTNode* node, Unary& unary) {
    switch (node->kind) {
        case NodeKind::negate:
            return ir_builder->CreateNeg(codegen(unary.expr.get()));
        case NodeKind::logic_not:
            return ir_builder->CreateICmpEQ(codegen(unary.expr.get()),
                                            llvm::ConstantInt::getBool(*llvm_context, 0));
        case NodeKind::bitwise_not:
            return ir_builder->CreateNot(codegen(unary.expr.get()));
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node->kind_name());
    }
}

llvm::Value* Codegen::codegen(ASTNode* node, Binary& bin) {

    switch(node->kind){
        case NodeKind::add:
            return ir_builder->CreateAdd(codegen(bin.lhs.get()), codegen(bin.rhs.get()));
        case NodeKind::sub:
            return ir_builder->CreateSub(codegen(bin.lhs.get()), codegen(bin.rhs.get()));
        case NodeKind::multiply:
            return ir_builder->CreateMul(codegen(bin.lhs.get()), codegen(bin.rhs.get()));
        case NodeKind::divide:
            return ir_builder->CreateSDiv(codegen(bin.lhs.get()), codegen(bin.rhs.get()));
        case NodeKind::equal:
            return ir_builder->CreateICmpEQ(codegen(bin.lhs.get()), codegen(bin.rhs.get()));
        case NodeKind::not_equal:
            return ir_builder->CreateICmpNE(codegen(bin.lhs.get()), codegen(bin.rhs.get()));
        case NodeKind::less_than:
            return ir_builder->CreateICmpSLT(codegen(bin.lhs.get()), codegen(bin.rhs.get()));
        case NodeKind::less_equals:
            return ir_builder->CreateICmpSLE(codegen(bin.lhs.get()), codegen(bin.rhs.get()));
        case NodeKind::assignment:
            return ir_builder->CreateStore(codegen(bin.rhs.get()), codegen(bin.lhs.get()));
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node->kind_name());
    }
}

llvm::Value* Codegen::codegen(ASTNode* node, Variable& var) {
    // NOTE: type checker shouldn't allow "let x: void;" to exist
    llvm::AllocaInst* ptr = ir_builder->CreateAlloca(fumo_to_llvm_type(var.type));
    if (var.value) ir_builder->CreateStore(codegen(var.value.value().get()), ptr);
    return ptr;
}

llvm::Value* Codegen::codegen(ASTNode* node, Function&   branch) { return {};}
llvm::Value* Codegen::codegen(ASTNode* node, Scope&      branch) { return {};}


