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

llvm::Function* Codegen::get_or_create_fumo_runtime_error() {
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
void Codegen::create_libc_functions() {
    get_or_create_printf();
    get_or_create_puts();
    get_or_create_strlen();
    get_or_create_strcmp();
    get_or_create_strcpy();
    get_or_create_strcat();
    get_or_create_malloc();
    get_or_create_free();
    get_or_create_exit();
    get_or_create_abort();
    get_or_create_memcpy();
    get_or_create_memset();
}
// printf - int printf(const char *s, ...)
llvm::Function* Codegen::get_or_create_printf() {
    llvm::Function* printf_fn = llvm_module->getFunction("printf");
    if (!printf_fn) {
        llvm::Type* int_type = llvm::Type::getInt32Ty(*llvm_context);
        llvm::FunctionType* printf_type = llvm::FunctionType::get(int_type, {ir_builder->getPtrTy()}, true);
        printf_fn = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, "printf", llvm_module.get());
    }
    return printf_fn;
}

// puts - int puts(const char *s)
llvm::Function* Codegen::get_or_create_puts() {
    llvm::Function* puts_fn = llvm_module->getFunction("puts");
    if (!puts_fn) {
        llvm::Type* int_type = llvm::Type::getInt32Ty(*llvm_context);
        llvm::FunctionType* puts_type = llvm::FunctionType::get(int_type, {ir_builder->getPtrTy()}, false);
        puts_fn = llvm::Function::Create(puts_type, llvm::Function::ExternalLinkage, "puts", llvm_module.get());
    }
    return puts_fn;
}

// strlen - size_t strlen(const char *s)
llvm::Function* Codegen::get_or_create_strlen() {
    llvm::Function* strlen_fn = llvm_module->getFunction("strlen");
    if (!strlen_fn) {
        llvm::Type* size_t_type = llvm::Type::getInt64Ty(*llvm_context); // size_t is typically 64-bit
        llvm::FunctionType* strlen_type = llvm::FunctionType::get(size_t_type, {ir_builder->getPtrTy()}, false);
        strlen_fn = llvm::Function::Create(strlen_type, llvm::Function::ExternalLinkage, "strlen", llvm_module.get());
    }
    return strlen_fn;
}

// strcmp - int strcmp(const char *s1, const char *s2)
llvm::Function* Codegen::get_or_create_strcmp() {
    llvm::Function* strcmp_fn = llvm_module->getFunction("strcmp");
    if (!strcmp_fn) {
        llvm::Type* int_type = llvm::Type::getInt32Ty(*llvm_context);
        llvm::FunctionType* strcmp_type = llvm::FunctionType::get(int_type, {ir_builder->getPtrTy(), ir_builder->getPtrTy()}, false);
        strcmp_fn = llvm::Function::Create(strcmp_type, llvm::Function::ExternalLinkage, "strcmp", llvm_module.get());
    }
    return strcmp_fn;
}

// strcpy - char *strcpy(char *dest, const char *src)
llvm::Function* Codegen::get_or_create_strcpy() {
    llvm::Function* strcpy_fn = llvm_module->getFunction("strcpy");
    if (!strcpy_fn) {
        llvm::Type* ptr_type = ir_builder->getPtrTy();
        llvm::FunctionType* strcpy_type = llvm::FunctionType::get(ptr_type, {ptr_type, ptr_type}, false);
        strcpy_fn = llvm::Function::Create(strcpy_type, llvm::Function::ExternalLinkage, "strcpy", llvm_module.get());
    }
    return strcpy_fn;
}

// strcat - char *strcat(char *dest, const char *src)
llvm::Function* Codegen::get_or_create_strcat() {
    llvm::Function* strcat_fn = llvm_module->getFunction("strcat");
    if (!strcat_fn) {
        llvm::Type* ptr_type = ir_builder->getPtrTy();
        llvm::FunctionType* strcat_type = llvm::FunctionType::get(ptr_type, {ptr_type, ptr_type}, false);
        strcat_fn = llvm::Function::Create(strcat_type, llvm::Function::ExternalLinkage, "strcat", llvm_module.get());
    }
    return strcat_fn;
}

// malloc - void *malloc(size_t size)
llvm::Function* Codegen::get_or_create_malloc() {
    llvm::Function* malloc_fn = llvm_module->getFunction("malloc");
    if (!malloc_fn) {
        llvm::Type* ptr_type = ir_builder->getPtrTy();
        llvm::Type* size_t_type = llvm::Type::getInt64Ty(*llvm_context);
        llvm::FunctionType* malloc_type = llvm::FunctionType::get(ptr_type, {size_t_type}, false);
        malloc_fn = llvm::Function::Create(malloc_type, llvm::Function::ExternalLinkage, "malloc", llvm_module.get());
    }
    return malloc_fn;
}

// free - void free(void *ptr)
llvm::Function* Codegen::get_or_create_free() {
    llvm::Function* free_fn = llvm_module->getFunction("free");
    if (!free_fn) {
        llvm::Type* void_type = llvm::Type::getVoidTy(*llvm_context);
        llvm::Type* ptr_type = ir_builder->getPtrTy();
        llvm::FunctionType* free_type = llvm::FunctionType::get(void_type, {ptr_type}, false);
        free_fn = llvm::Function::Create(free_type, llvm::Function::ExternalLinkage, "free", llvm_module.get());
    }
    return free_fn;
}

// exit - void exit(int status)
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

// abort - void abort(void)
llvm::Function* Codegen::get_or_create_abort() {
    llvm::Function* abort_fn = llvm_module->getFunction("abort");
    if (!abort_fn) {
        llvm::Type* void_type = llvm::Type::getVoidTy(*llvm_context);
        llvm::FunctionType* abort_type = llvm::FunctionType::get(void_type, {}, false);
        abort_fn = llvm::Function::Create(abort_type, llvm::Function::ExternalLinkage, "abort", llvm_module.get());
    }
    return abort_fn;
}

// memcpy - void *memcpy(void *dest, const void *src, size_t n)
llvm::Function* Codegen::get_or_create_memcpy() {
    llvm::Function* memcpy_fn = llvm_module->getFunction("memcpy");
    if (!memcpy_fn) {
        llvm::Type* ptr_type = ir_builder->getPtrTy();
        llvm::Type* size_t_type = llvm::Type::getInt64Ty(*llvm_context);
        llvm::FunctionType* memcpy_type = llvm::FunctionType::get(ptr_type, {ptr_type, ptr_type, size_t_type}, false);
        memcpy_fn = llvm::Function::Create(memcpy_type, llvm::Function::ExternalLinkage, "memcpy", llvm_module.get());
    }
    return memcpy_fn;
}

// memset - void *memset(void *s, int c, size_t n)
llvm::Function* Codegen::get_or_create_memset() {
    llvm::Function* memset_fn = llvm_module->getFunction("memset");
    if (!memset_fn) {
        llvm::Type* ptr_type = ir_builder->getPtrTy();
        llvm::Type* int_type = llvm::Type::getInt32Ty(*llvm_context);
        llvm::Type* size_t_type = llvm::Type::getInt64Ty(*llvm_context);
        llvm::FunctionType* memset_type = llvm::FunctionType::get(ptr_type, {ptr_type, int_type, size_t_type}, false);
        memset_fn = llvm::Function::Create(memset_type, llvm::Function::ExternalLinkage, "memset", llvm_module.get());
    }
    return memset_fn;
}

TypePromotion Codegen::promote_operands(llvm::Value* lhs, llvm::Value* rhs, bool lhs_signed, bool rhs_signed) {
    llvm::Type* lhs_type = lhs->getType();
    llvm::Type* rhs_type = rhs->getType();
    
    // If types already match, no promotion needed
    if (lhs_type == rhs_type) {
        return {lhs_type, lhs, rhs, lhs_signed && rhs_signed};
    }
    
    // Integer promotion rules
    if (lhs_type->isIntegerTy() && rhs_type->isIntegerTy()) {
        unsigned lhs_bits = lhs_type->getIntegerBitWidth();
        unsigned rhs_bits = rhs_type->getIntegerBitWidth();
        
        llvm::Type* target_type;
        bool result_signed;
        
        if (lhs_bits > rhs_bits) {
            target_type = lhs_type;
            result_signed = lhs_signed;
        } else if (rhs_bits > lhs_bits) {
            target_type = rhs_type;
            result_signed = rhs_signed;
        } else {
            // Same width - unsigned wins if either is unsigned
            target_type = lhs_type; // They're the same width
            result_signed = lhs_signed && rhs_signed;
        }
        
        // Promote operands
        llvm::Value* new_lhs = lhs;
        llvm::Value* new_rhs = rhs;
        
        if (lhs_type != target_type) {
            if (lhs_signed) {
                new_lhs = ir_builder->CreateSExt(lhs, target_type);
            } else {
                new_lhs = ir_builder->CreateZExt(lhs, target_type);
            }
        }
        
        if (rhs_type != target_type) {
            if (rhs_signed) {
                new_rhs = ir_builder->CreateSExt(rhs, target_type);
            } else {
                new_rhs = ir_builder->CreateZExt(rhs, target_type);
            }
        }
        
        return {target_type, new_lhs, new_rhs, result_signed};
    }
    
    // Float promotion (int -> float, float -> double)
    if (lhs_type->isFloatingPointTy() || rhs_type->isFloatingPointTy()) {
        llvm::Type* target_type;
        
        // Determine target floating point type
        if (lhs_type->isDoubleTy() || rhs_type->isDoubleTy()) {
            target_type = ir_builder->getDoubleTy();
        } else {
            target_type = ir_builder->getFloatTy();
        }
        
        llvm::Value* new_lhs = lhs;
        llvm::Value* new_rhs = rhs;
        
        // Convert lhs if needed
        if (lhs_type->isIntegerTy()) {
            if (lhs_signed) {
                new_lhs = ir_builder->CreateSIToFP(lhs, target_type);
            } else {
                new_lhs = ir_builder->CreateUIToFP(lhs, target_type);
            }
        } else if (lhs_type->isFloatTy() && target_type->isDoubleTy()) {
            new_lhs = ir_builder->CreateFPExt(lhs, target_type);
        }
        
        // Convert rhs if needed
        if (rhs_type->isIntegerTy()) {
            if (rhs_signed) {
                new_rhs = ir_builder->CreateSIToFP(rhs, target_type);
            } else {
                new_rhs = ir_builder->CreateUIToFP(rhs, target_type);
            }
        } else if (rhs_type->isFloatTy() && target_type->isDoubleTy()) {
            new_rhs = ir_builder->CreateFPExt(rhs, target_type);
        }
        
        return {target_type, new_lhs, new_rhs, true}; // floats are "signed"
    }
    
    // No valid promotion
    internal_panic("cannot find common type for promotion of operands.");
}
llvm::Value* Codegen::convert_int_type(llvm::Value* value, llvm::Type* target_type, bool is_signed) {
    llvm::Type* source_type = value->getType();
    
    if (source_type == target_type) {
        return value;
    }
    if (source_type->isIntegerTy() && target_type->isIntegerTy()) {
        unsigned source_bits = source_type->getIntegerBitWidth();
        unsigned target_bits = target_type->getIntegerBitWidth();
        
        if (source_bits > target_bits) {
            return ir_builder->CreateTrunc(value, target_type);
        } else if (source_bits < target_bits) {
            return is_signed ? ir_builder->CreateSExt(value, target_type) 
                             : ir_builder->CreateZExt(value, target_type);
        }
    }
    return value;
}
