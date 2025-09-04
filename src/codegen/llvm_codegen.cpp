#include "codegen/llvm_codegen.hpp"

void Codegen::codegen_file(ASTNode* file_root_node) {

    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), {}, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "main", llvm_module.get());

    llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", func);
    ir_builder->SetInsertPoint(bblock);

    // for (const auto& [_, node] : symbol_tree.all_declarations) codegen(*node);
    for (const auto& node : get<NamespaceDecl>(file_root_node).nodes) codegen(*node);
}
/*
fn func() -> i32 {
    let x = 213;
}
*/
llvm::Value* Codegen::codegen(const ASTNode& node) {
    // NOTE: should always check for a possible nullptr on each codegen

    match(node) {

        holds(Identifier, const& id) {
            switch (id.kind) {
                case Identifier::var_name:
                case Identifier::member_var_name:
                    break;
                case Identifier::func_call_name:
                case Identifier::type_name:
                case Identifier::declaration_name:
                case Identifier::member_func_call_name:
                case Identifier::unknown_name:
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
            }

        }

        holds(PrimaryExpr, const& prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:
                    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*llvm_context),
                                                        std::get<int64_t>(prim.value));
                case PrimaryExpr::floating_point:
                    return llvm::ConstantInt::getSigned(llvm::Type::getFloatTy(*llvm_context),
                                                        std::get<double>(prim.value));
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
            llvm::Value* lhs_val = codegen(*bin.lhs);
            llvm::Value* rhs_val = codegen(*bin.rhs);
            if (lhs_val == nullptr || rhs_val == nullptr) {
                INTERNAL_PANIC("[Codegen] found nullptr in binary operand for '{}'.", node.name());
            }
            switch(bin.kind) {
                case BinaryExpr::add:
                    return ir_builder->CreateAdd(lhs_val, rhs_val);
                case BinaryExpr::sub:
                    return ir_builder->CreateSub(lhs_val, rhs_val);
                case BinaryExpr::multiply:
                    return ir_builder->CreateMul(lhs_val, rhs_val);
                case BinaryExpr::divide:
                    return ir_builder->CreateSDiv(lhs_val, rhs_val);
                case BinaryExpr::equal:
                    return ir_builder->CreateICmpEQ(lhs_val, rhs_val);
                case BinaryExpr::not_equal:
                    return ir_builder->CreateICmpNE(lhs_val, rhs_val);
                case BinaryExpr::less_than:
                    return ir_builder->CreateICmpSLT(lhs_val, rhs_val);
                case BinaryExpr::less_equals:
                    return ir_builder->CreateICmpSLE(lhs_val, rhs_val);
                case BinaryExpr::assignment:
                    // FIXME: we shouldnt codegen a new alloca on assignment
                    // should get the ptr from the current var environment instead
                    return ir_builder->CreateStore(rhs_val, lhs_val);
                default:
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
            }
        }

        holds(PostfixExpr, const& postfix) {
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
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

        holds(NamespaceDecl) {
            INTERNAL_PANIC("namespaces shouldn't be codegen'd, found '{}'.", node.name());
        }

        _default INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
    }
    std::unreachable();
}
