#include "codegen/llvm_codegen.hpp"

void Codegen::codegen_file(ASTNode* file_root_node) {

    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), {}, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "main", llvm_module.get());

    llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", func);
    ir_builder->SetInsertPoint(bblock);

    for (const auto& [_, node] : symbol_tree.all_declarations) codegen(*node);
}

llvm::Value* Codegen::codegen(const ASTNode& node) {

    match(node) {

        holds(Identifier, const& id) {
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
        }

        holds(PrimaryExpr, const& prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:
                    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*llvm_context),
                                                        std::get<int64_t>(prim.value));
                case PrimaryExpr::floating_point:
                    return llvm::ConstantInt::getSigned(llvm::Type::getFloatTy(*llvm_context),
                                                        std::get<double>(prim.value));
                // case PrimaryExpr::identifier:
                    // TODO: make a proper symbol table
                    // return variable_env[std::get<str>(primary.value)];
                case PrimaryExpr::str:
                default:
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.kind_name());
                }
        }

        holds(UnaryExpr, const& un) {
            switch (un.kind) {
                case UnaryExpr::negate:
                    return ir_builder->CreateNeg(codegen(*un.expr));
                case UnaryExpr::logic_not:
                    return ir_builder->CreateICmpEQ(codegen(*un.expr),
                                                    llvm::ConstantInt::getBool(*llvm_context, 0));
                case UnaryExpr::bitwise_not:
                    return ir_builder->CreateNot(codegen(*un.expr));
                default:
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
            }
        }

        holds(BinaryExpr, const& bin) {
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

        holds(VariableDecl, const& var) {
            // NOTE: type checker shouldn't allow "let x: void;" to exist
            auto type = fumo_to_llvm_type(node.type);
            llvm::AllocaInst* ptr = ir_builder->CreateAlloca(type, nullptr, get_id(var).name);
            // if (var.assignment) ir_builder->CreateStore(codegen(*var.assignment.value()), ptr);
            return ptr;
        }

        holds(FunctionDecl, const& func) {

            vec<llvm::Type*> param_types {};
            for (const auto& param : func.parameters) {
                param_types.push_back(fumo_to_llvm_type(param->type));
                codegen(*param);
            }
            llvm::FunctionType* func_type = llvm::FunctionType::get(fumo_to_llvm_type(node.type), param_types, false);
            llvm::Function* llvm_func = llvm::Function::Create(func_type,
                                                          llvm::Function::ExternalLinkage,
                                                          get_id(func).mangled_name,
                                                          llvm_module.get());
            llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", llvm_func);

            ir_builder->SetInsertPoint(bblock);

            if (func.body) codegen(*func.body.value());

            return llvm_func;
        }

        holds(BlockScope, const& scope) {
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
        }

        holds(TypeDecl, const& type_decl) {
            // NOTE: type decls also codegens its own member function decls
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
        }

        holds(PostfixExpr, const& postfix) {
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
        }

        _default INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
    }
    std::unreachable();
}
