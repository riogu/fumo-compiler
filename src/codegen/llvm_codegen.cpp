#include "codegen/llvm_codegen.hpp"

void Codegen::codegen(ASTNode* file_root_node) {

    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), {}, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "main", llvm_module.get());

    llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", func);
    ir_builder->SetInsertPoint(bblock);

    for (auto& node : get<NamespaceDecl>(file_root_node).nodes) codegen(*node);
}

llvm::Value* Codegen::codegen(const ASTNode& node) { 
    match(node) {
        holds(PrimaryExpr,  const& v) return codegen(node, v);
        holds(UnaryExpr,    const& v) return codegen(node, v);
        holds(BinaryExpr,   const& v) return codegen(node, v);
        holds(VariableDecl, const& v) return codegen(node, v);
        holds(FunctionDecl, const& v) return codegen(node, v);
        holds(BlockScope,   const& v) return codegen(node, v);
        _default INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
    }
    std::unreachable();
}

llvm::Value* Codegen::codegen(const ASTNode& node, const PrimaryExpr& primary) {
    switch (primary.kind) {
        case PrimaryExpr::integer:
            return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*llvm_context),
                                                std::get<int64_t>(primary.value));
        case PrimaryExpr::floating_point:
            return llvm::ConstantInt::getSigned(llvm::Type::getFloatTy(*llvm_context),
                                                std::get<double>(primary.value));
        // case PrimaryExpr::identifier:
            // TODO: make a proper symbol table
            // return variable_env[std::get<str>(primary.value)];
        case PrimaryExpr::str:
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node.kind_name());
    }
}

llvm::Value* Codegen::codegen(const ASTNode& node, const UnaryExpr& unary) {
    switch (unary.kind) {
        case UnaryExpr::negate:
            return ir_builder->CreateNeg(codegen(*unary.expr));
        case UnaryExpr::logic_not:
            return ir_builder->CreateICmpEQ(codegen(*unary.expr),
                                            llvm::ConstantInt::getBool(*llvm_context, 0));
        case UnaryExpr::bitwise_not:
            return ir_builder->CreateNot(codegen(*unary.expr));
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
    }
}

llvm::Value* Codegen::codegen(const ASTNode& node, const BinaryExpr& bin) {

    switch(bin.kind) {
        case BinaryExpr::add:
            return ir_builder->CreateAdd(codegen(*bin.lhs), codegen(*bin.rhs));
        case BinaryExpr::sub:
            return ir_builder->CreateSub(codegen(*bin.lhs), codegen(*bin.rhs));
        case BinaryExpr::multiply:
            return ir_builder->CreateMul(codegen(*bin.lhs), codegen(*bin.rhs));
        case BinaryExpr::divide:
            return ir_builder->CreateSDiv(codegen(*bin.lhs), codegen(*bin.rhs));
        case BinaryExpr::equal:
            return ir_builder->CreateICmpEQ(codegen(*bin.lhs), codegen(*bin.rhs));
        case BinaryExpr::not_equal:
            return ir_builder->CreateICmpNE(codegen(*bin.lhs), codegen(*bin.rhs));
        case BinaryExpr::less_than:
            return ir_builder->CreateICmpSLT(codegen(*bin.lhs), codegen(*bin.rhs));
        case BinaryExpr::less_equals:
            return ir_builder->CreateICmpSLE(codegen(*bin.lhs), codegen(*bin.rhs));
        case BinaryExpr::assignment:
            // FIXME: we shouldnt codegen a new alloca on assignment
            // should get the ptr from the current var environment instead
            return ir_builder->CreateStore(codegen(*bin.rhs), codegen(*bin.lhs));
        default:
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
    }
}

llvm::Value* Codegen::codegen(const ASTNode& node, const VariableDecl& var) {
    // NOTE: type checker shouldn't allow "let x: void;" to exist
    auto type = fumo_to_llvm_type(node.type);
    llvm::AllocaInst* ptr = ir_builder->CreateAlloca(type, nullptr, get_id(var).name);
    // if (var.assignment) ir_builder->CreateStore(codegen(*var.assignment.value()), ptr);
    return ptr;
}

llvm::Value* Codegen::codegen(const ASTNode& node, const FunctionDecl& branch) {
    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
}
llvm::Value* Codegen::codegen(const ASTNode& node, const BlockScope& branch) {
    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
}
