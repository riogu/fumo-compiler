#include "codegen/llvm_codegen.hpp"

void Codegen::verify_user_main() {
    auto* user_main = llvm_module->getFunction("fumo.user_main");

    if (user_main && !user_main->isDeclaration() && !user_main->empty()) {
        user_main->setLinkage(llvm::GlobalValue::InternalLinkage);
        
        llvm::BasicBlock* back = &user_main->back();
        if (auto* term = user_main->back().getTerminator(); !term) {
            ir_builder->SetInsertPoint(back);
            ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());

            if (user_main->getReturnType()->isVoidTy()) 
                ir_builder->CreateRetVoid();
            else if (user_main->getReturnType()->isIntegerTy(32))
                ir_builder->CreateRet(ir_builder->getInt32(0));
            else {
                report_error(symbol_tree.function_decls["fumo.user_main"]->source_token,
                             "main has invalid return type. (NOTE: use void or i32).");
            }

        } else if (!llvm::isa<llvm::ReturnInst>(term)) {
            internal_panic("main ended with '{}', a non 'ret' terminator instruction.", term->getOpcodeName());
        }
    }
}
void Codegen::create_libc_main() {

    std::vector<llvm::Type*> main_args {};
    main_args.push_back(llvm::Type::getInt32Ty(*llvm_context));    // int argc
    main_args.push_back(llvm::PointerType::get(*llvm_context, 0)); // char** argv

    llvm::FunctionType* main_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*llvm_context), main_args, false);
    llvm::Function* main_func = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, 
                                                      "main", llvm_module.get());
    
    main_func->setDSOLocal(false);
    main_func->addFnAttr("used");
    
    auto arg_iter = main_func->arg_begin();
    llvm::Value* argc = &(*arg_iter);     argc->setName("argc");
    llvm::Value* argv = &(*(++arg_iter)); argv->setName("argv");
    
    llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(*llvm_context, "", main_func);
    ir_builder->SetInsertPoint(entry_block);
    
    llvm::Function* fumo_init = llvm_module->getFunction("fumo.init");
    ir_builder->CreateCall(fumo_init);
    
    llvm::Function* user_main = llvm_module->getFunction("fumo.user_main");
    if (!user_main) {
        ir_builder->CreateRet(ir_builder->getInt32(0));
        return;
    }

    llvm::Value* result;

    if (user_main->arg_size() == 0) {
        result = ir_builder->CreateCall(user_main);
    } else if (user_main->arg_size() == 2) {

        const auto& user_main_node_func = get<FunctionDecl>(symbol_tree.function_decls["fumo.user_main"]);

        const auto& argc_node = user_main_node_func.parameters[0];
        const auto& argv_node = user_main_node_func.parameters[1];

        // NOTE: maybe using other names shouldn't be an error. theres no particular harm in allowing it
        if (!(get_id(get<VariableDecl>(argc_node)).mangled_name != "argc"
           && get_id(get<VariableDecl>(argv_node)).mangled_name != "argv"
           && argc_node->type.kind == Type::i32_ && argv_node->type.kind == Type::str_)) {
            report_error(symbol_tree.function_decls["main"]->source_token,
                         "main has wrong types for arguments. NOTE: use main(i32 argc, str* argv) or main()");
        }

        result = ir_builder->CreateCall(user_main, {argc, argv});

    } else {
        report_error(symbol_tree.function_decls["fumo.user_main"]->source_token,
                     "main has wrong arguments. NOTE: use main(i32 argc, *str) or main()");
    }

    if (user_main->getReturnType()->isVoidTy())
        ir_builder->CreateRet(ir_builder->getInt32(0));
    else
        ir_builder->CreateRet(result);
}

void Codegen::clear_metadata() {
    for (auto& func : *llvm_module) {
        func.setSubprogram(nullptr);
        for (auto& BB : func) {
            for (auto& inst : BB) {
                inst.setDebugLoc(llvm::DebugLoc());
                inst.dropUnknownNonDebugMetadata();
            }
        }
    }
}

llvm::Function* Codegen::get_or_create_runtime_error_function() {
    static llvm::Function* null_error_fn = nullptr;
    
    if (!null_error_fn) {
        // void fumo.runtime_error(const char* message)
        llvm::Type* void_type = llvm::Type::getVoidTy(*llvm_context);
        llvm::FunctionType* fn_type = llvm::FunctionType::get(void_type, {ir_builder->getPtrTy()}, false);
        
        null_error_fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, 
                                              "fumo.runtime_error", llvm_module.get());
        
        llvm::BasicBlock* entry_bb = llvm::BasicBlock::Create(*llvm_context, "", null_error_fn);
        llvm::IRBuilder<> temp_builder(entry_bb);
        
        llvm::Value* message = null_error_fn->arg_begin();
        
        llvm::Function* printf_fn = get_or_create_printf();
        temp_builder.CreateCall(printf_fn, {temp_builder.CreateGlobalString("%s"), message});
        
        llvm::Function* exit_fn = get_or_create_exit();
        temp_builder.CreateCall(exit_fn, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 1)});
        temp_builder.CreateUnreachable();
    }
    
    return null_error_fn;
}
llvm::Function* Codegen::get_or_create_printf() {
    llvm::Function* printf_fn = llvm_module->getFunction("printf");
    if (!printf_fn) {
        llvm::Type* int_type = llvm::Type::getInt32Ty(*llvm_context);
        llvm::FunctionType* printf_type = llvm::FunctionType::get(int_type, {ir_builder->getPtrTy()}, true);
        printf_fn = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, "printf", llvm_module.get());
    }
    return printf_fn;
}

llvm::Function* Codegen::get_or_create_exit() {
    llvm::Function* exit_fn = llvm_module->getFunction("exit");
    if (!exit_fn) {
        llvm::Type* void_type = llvm::Type::getVoidTy(*llvm_context);
        llvm::Type* int_type = llvm::Type::getInt32Ty(*llvm_context);
        llvm::FunctionType* exit_type = llvm::FunctionType::get(void_type, {int_type}, false);
        exit_fn = llvm::Function::Create(exit_type, llvm::Function::ExternalLinkage, "exit", llvm_module.get());
    }
    return exit_fn;
}
