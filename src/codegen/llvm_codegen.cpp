#include "codegen/llvm_codegen.hpp"

void Codegen::codegen_file(ASTNode* file_root_node) {
    this->file_root_node = file_root_node;

    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvm_context), {}, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::InternalLinkage,
                                                  "fumo._start", llvm_module.get());
    func->setLinkage(llvm::GlobalValue::ExternalLinkage);
    func->setDSOLocal(false);
    func->addFnAttr(llvm::Attribute::NoInline); // Prevent inlining   
    func->addFnAttr("used");

    llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", func);
    ir_builder->SetInsertPoint(bblock);

    for (const auto& [name, node] : symbol_tree.all_declarations) register_declaration(name, *node);
    for (const auto& node : get<NamespaceDecl>(file_root_node).nodes) codegen(*node);

    auto* main = llvm_module->getFunction("main");
    if (main && !main->isDeclaration()) {
        // main = create_main();
        ir_builder->SetInsertPoint(&main->back(), main->back().end());

        auto* term = main->back().getTerminator();
        if (!(term && llvm::isa<llvm::ReturnInst>(term))) {
            ir_builder->CreateRet(ir_builder->getInt32(0));
        }

        ir_builder->SetInsertPoint(&func->back(), func->back().end());
        ir_builder->CreateRetVoid();

        ir_builder->SetInsertPoint(main->getEntryBlock().begin());
        ir_builder->CreateCall(func);

    }
    else {
        std::cerr << "forgot to create a main function for this module."
                     " [currently only supports single file compilation]" << std::endl;
        std::exit(1);
    }
    
}

Opt<llvm::Value*> Codegen::codegen(ASTNode& node) {
    // NOTE: should always check for a possible nullptr on each codegen

    match(node) {

        holds(Identifier, const& id) {
            switch (id.kind) {
                case Identifier::var_name: 
                    // NOTE: always return a pointer, have BinaryExpr::assignment deal with loading from the pointer
                    // i will need to keep some sort of state between the loads and all that for the codegen to work
                    // the current codegen is NOT working
                    if (!id.declaration.value()->llvm_value) {
                        INTERNAL_PANIC("[Codegen Error] forgot to assign llvm::Value to declaration for '{}'.",
                                       get_id(get<VariableDecl>(id.declaration.value())).mangled_name);
                    }
                    if (get<VariableDecl>(id.declaration.value()).kind == VariableDecl::global_var_declaration
                        || id.is_assigned_to) {
                        return id.declaration.value()->llvm_value;
                    }
                    return ir_builder->CreateLoad(fumo_to_llvm_type(id.declaration.value()->type),
                                                  id.declaration.value()->llvm_value);
                
                case Identifier::member_var_name:
                    // TODO: this needs GEP from the base address of the struct
                    break;
                case Identifier::func_call_name:
                case Identifier::type_name:
                case Identifier::declaration_name:
                case Identifier::member_func_call_name:
                case Identifier::unknown_name:
                    INTERNAL_PANIC("shouldn't codegen this identifier: '{}'.", node.name());
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

            llvm::Value* val;
            if (!(val = codegen(*un.expr).value_or(nullptr))) {
                INTERNAL_PANIC("[Codegen] found null value in UnaryExpr '{}'.", node.name());
            }

            switch (un.kind) {
                case UnaryExpr::negate:
                    return ir_builder->CreateNeg(val);
                case UnaryExpr::logic_not:
                    return ir_builder->CreateICmpEQ(val, llvm::ConstantInt::getBool(*llvm_context, 0));
                case UnaryExpr::bitwise_not:
                    return ir_builder->CreateNot(val);
                case UnaryExpr::return_statement:
                    return ir_builder->CreateRet(val);
                default:
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
            }
        }

        holds(BinaryExpr, const& bin) {

            llvm::Value* lhs_val = codegen(*bin.lhs).value_or(nullptr);
            llvm::Value* rhs_val = codegen(*bin.rhs).value_or(nullptr);
            if (!lhs_val || !rhs_val) {
                INTERNAL_PANIC("[Codegen] found null value in binary operand for '{}'.", node.name());
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
                    // NOTE: making global variables is very complicated and has a lot of implications,
                    // so for now we will make a simple _start function and use that,
                    // but it should be redone
                    //
                    // let var = 123 + 3213;
                    if (auto var = get_if<VariableDecl>(bin.lhs)) {
                        if (var->kind == VariableDecl::global_var_declaration) {
                            ir_builder->SetInsertPointPastAllocas(llvm_module->getFunction("fumo._start"));
                        }
                    }

                    return ir_builder->CreateStore(rhs_val, lhs_val);
                default:
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
            }
        }

        holds(PostfixExpr, const& postfix) {
            for (auto* node : postfix.nodes) codegen(*node);
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
        }
        holds(VariableDecl, const& var) {
            // NOTE: type checker shouldn't allow "let x: void;" to exist
            switch (var.kind) {
                case VariableDecl::global_var_declaration: {
                    // NOTE: we need to make a system that provides a Const "default zero'd value"
                    // to satisfy the llvm API and the constraint that globals need to have default values.
                    // so, we need a function that does that for user structs and for primitive types, and etc
                    // (in place of a call to llvm::ConstantInt
                    auto* global_var = new llvm::GlobalVariable(fumo_to_llvm_type(node.type),
                                                                false,
                                                                llvm::GlobalValue::ExternalLinkage,
                                                                llvm::Constant::getNullValue(fumo_to_llvm_type(node.type)),
                                                                get_id(var).mangled_name);
                    llvm_module->insertGlobalVariable(global_var);
                    node.llvm_value = llvm::cast<llvm::Value>(global_var);
                    return node.llvm_value;
                }
                case VariableDecl::variable_declaration: {
                    auto* type = fumo_to_llvm_type(node.type);
                    node.llvm_value = ir_builder->CreateAlloca(type, nullptr, get_id(var).mangled_name);
                    return node.llvm_value;
                }
                case VariableDecl::parameter:
                    break;
            }
        }

        holds(FunctionDecl, const& func) {
            // NOTE: we delete forward declarations and only keep the first occurence
            auto* llvm_func = llvm_module->getFunction(get_id(func).mangled_name);
            if (func.body) {
                llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", llvm_func);
                ir_builder->SetInsertPoint(bblock);
                codegen(*func.body.value());
                if (node.type.kind == Type::void_) ir_builder->CreateRetVoid();
            } 
            return llvm_func;
        }

        holds(FunctionCall) {
            // report_error(node.source_token, "function '{}' was never defined.", get_id(func).mangled_name);
            INTERNAL_PANIC("namespaces shouldn't be codegen'd, found '{}'.", node.name());
        }

        holds(BlockScope, const& scope) {
            for (auto* node : scope.nodes) codegen(*node);
        }

        holds(TypeDecl, const& type_decl) {

            if (type_decl.definition_body) {
                for (auto* node : type_decl.definition_body.value()) codegen(*node);
            }
            // NOTE: type decls also codegens its own member function decls
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
        }

        holds(NamespaceDecl) {
            INTERNAL_PANIC("namespaces shouldn't be codegen'd, found '{}'.", node.name());
        }


        _default INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
    }
    // NOTE: if our branch has nothing to return, it returns nullptr;
    return std::nullopt;
}

void Codegen::register_declaration(std::string_view name, const ASTNode& node) {
    // adds function prototypes and forward declarations of types

    match(node) {
        holds(TypeDecl, const& type_decl) {
            // TODO: continue this codegen
            llvm::StructType::create(*llvm_context, name);
        }
        holds(FunctionDecl, const& func) {

            vec<llvm::Type*> param_types {};

            for (const auto& param : func.parameters) {
                param_types.push_back(fumo_to_llvm_type(param->type));
                codegen(*param);
            }
            llvm::FunctionType* func_type = llvm::FunctionType::get(fumo_to_llvm_type(node.type), param_types, false);

            llvm::Function* llvm_func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                                                               get_id(func).mangled_name, llvm_module.get());
        }
        _default INTERNAL_PANIC("added wrong declaration '{}' to 'symbol_table.all_declarations', ", node.name());
    }
}

llvm::Function* Codegen::create_main() {
    std::vector<llvm::Type*> main_args {};
    // main_args.push_back(llvm::Type::getInt32Ty(*llvm_context));    // int argc
    // main_args.push_back(llvm::PointerType::get(*llvm_context, 0)); // char** argv

    llvm::FunctionType* main_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), {}, false);
    llvm::Function* main = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, 
                                                  "main", llvm_module.get());

    // auto args = main->arg_begin();
    // args->setName("argc");
    // (++args)->setName("argv");
    //
    main->setLinkage(llvm::GlobalValue::ExternalLinkage);
    main->addFnAttr("used");
    main->setDSOLocal(false);

    return main;
}
