#include "codegen/llvm_codegen.hpp"

void Codegen::codegen(Scope& file_scope) {

    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), {}, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "main", llvm_module.get());

    llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", func);
    ir_builder->SetInsertPoint(bblock);

    for (auto& node : file_scope.nodes) codegen(*node);
}

llvm::Value* Codegen::codegen(const ASTNode& node) { 

    match(node) {
        holds(const Primary&,  v) return codegen(node, v);
        holds(const Unary&,    v) return codegen(node, v);
        holds(const Binary&,   v) return codegen(node, v);
        holds(const Variable&, v) return codegen(node, v);
        holds(const Function&, v) return codegen(node, v);
        holds(const Scope&,    v) return codegen(node, v);
        _ INTERNAL_PANIC("codegen not implemented for '{}'", node.kind_name());
    }
    std::unreachable();
}

llvm::Value* Codegen::codegen(const ASTNode& node, const Primary& primary) {

    switch (node.kind) {
        case NodeKind::integer:
            return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*llvm_context),
                                                std::get<int64_t>(primary.value));
        case NodeKind::floating_point:
            return llvm::ConstantInt::getSigned(llvm::Type::getFloatTy(*llvm_context),
                                                std::get<double>(primary.value));
        case NodeKind::identifier:
            // TODO: make a proper symbol table
            // return variable_env[std::get<str>(primary.value)];
        case NodeKind::str:
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node.kind_name());
    }
}

llvm::Value* Codegen::codegen(const ASTNode& node, const Unary& unary) {
    switch (node.kind) {
        case NodeKind::negate:
            return ir_builder->CreateNeg(codegen(*unary.expr));
        case NodeKind::logic_not:
            return ir_builder->CreateICmpEQ(codegen(*unary.expr),
                                            llvm::ConstantInt::getBool(*llvm_context, 0));
        case NodeKind::bitwise_not:
            return ir_builder->CreateNot(codegen(*unary.expr));
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node.kind_name());
    }
}

llvm::Value* Codegen::codegen(const ASTNode& node, const Binary& bin) {

    switch(node.kind){
        case NodeKind::add:
            return ir_builder->CreateAdd(codegen(*bin.lhs), codegen(*bin.rhs));
        case NodeKind::sub:
            return ir_builder->CreateSub(codegen(*bin.lhs), codegen(*bin.rhs));
        case NodeKind::multiply:
            return ir_builder->CreateMul(codegen(*bin.lhs), codegen(*bin.rhs));
        case NodeKind::divide:
            return ir_builder->CreateSDiv(codegen(*bin.lhs), codegen(*bin.rhs));
        case NodeKind::equal:
            return ir_builder->CreateICmpEQ(codegen(*bin.lhs), codegen(*bin.rhs));
        case NodeKind::not_equal:
            return ir_builder->CreateICmpNE(codegen(*bin.lhs), codegen(*bin.rhs));
        case NodeKind::less_than:
            return ir_builder->CreateICmpSLT(codegen(*bin.lhs), codegen(*bin.rhs));
        case NodeKind::less_equals:
            return ir_builder->CreateICmpSLE(codegen(*bin.lhs), codegen(*bin.rhs));
        case NodeKind::assignment:
            // FIXME: we shouldnt codegen a new alloca on assignment
            return ir_builder->CreateStore(codegen(*bin.rhs), codegen(*bin.lhs));
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node.kind_name());
    }
}

llvm::Value* Codegen::codegen(const ASTNode& node, const Variable& var) {
    // NOTE: type checker shouldn't allow "let x: void;" to exist
    auto type = fumo_to_llvm_type(node.type);
    llvm::AllocaInst* ptr = ir_builder->CreateAlloca(type, nullptr, var.name);
    if (var.value) ir_builder->CreateStore(codegen(*var.value.value()), ptr);
    return ptr;
}

llvm::Value* Codegen::codegen(const ASTNode& node, const Function& branch) {


    return {};
}
llvm::Value* Codegen::codegen(const ASTNode& node, const Scope& branch) { return {};}


