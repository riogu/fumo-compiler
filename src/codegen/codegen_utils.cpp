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
            INTERNAL_PANIC("main ended with '{}', a non 'ret' terminator instruction.", term->getOpcodeName());
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
    
    llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(*llvm_context, "entry", main_func);
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
