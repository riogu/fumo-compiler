#include "codegen/llvm_codegen.hpp"
#include <llvm/Support/Casting.h>

void Codegen::codegen_file(ASTNode* file_root_node) {
    this->file_root_node = file_root_node;

    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvm_context), {}, false);
    llvm::Function* fumo_init = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                                                       "fumo.init", llvm_module.get());
    fumo_init->setLinkage(llvm::GlobalValue::ExternalLinkage);
    fumo_init->setDSOLocal(false);
    // fumo_init->addFnAttr(llvm::Attribute::NoInline);
    fumo_init->addFnAttr("used");

    llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", fumo_init);
    fumo_init_builder->SetInsertPoint(bblock);
    fumo_init_builder->SetCurrentDebugLocation(llvm::DebugLoc());

    // codegen
    // ---------------------------------------------------------------------------
    for (const auto& [name, node] : symbol_tree.all_declarations) register_declaration(name, *node);
    for (const auto& node : get<NamespaceDecl>(file_root_node).nodes) codegen_rvalue(*node);
    // ---------------------------------------------------------------------------

    fumo_init_builder->SetInsertPoint(&fumo_init->back());
    fumo_init_builder->SetCurrentDebugLocation(llvm::DebugLoc());
    fumo_init_builder->CreateRetVoid();

    verify_user_main();
    create_libc_main();
}

// returns the ACTUAL VALUE (for expressions, function args, etc)
Opt<llvm::Value*> Codegen::codegen_rvalue(ASTNode& node) {
    
    match(node) {
        holds(Identifier, const& id) {
            switch (id.kind) {
                case Identifier::var_name: {
                    ASTNode* declaration = id.declaration.value();
                    if (!declaration->llvm_value) {
                        INTERNAL_PANIC("[Codegen Error] forgot to assign llvm::Value to declaration for '{}'.",
                                       get_id(get<VariableDecl>(declaration)).mangled_name);
                    }
                    // Load the value from the alloca
                    return ir_builder->CreateLoad(fumo_to_llvm_type(node.type), declaration->llvm_value);
                }
                case Identifier::member_var_name:
                    // TODO: this needs GEP from the base address of the struct + load
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
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
                    return llvm::ConstantFP::get(llvm::Type::getFloatTy(*llvm_context),
                                                 std::get<double>(prim.value));
                case PrimaryExpr::str:
                default:
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.kind_name());
            }
        }

        holds(UnaryExpr, const& un) {
            // &expr - get the lvalue (address) of the expression
            if (un.kind == UnaryExpr::address_of) return codegen_lvalue(*un.expr);

            auto* val = codegen_rvalue(*un.expr).value_or(nullptr);
            if (!val) INTERNAL_PANIC("[Codegen] found null value in UnaryExpr '{}'.", node.name());

            switch (un.kind) {
                case UnaryExpr::dereference: // *ptr - load the value from the pointer
                    if (!un.expr->type.ptr_count) INTERNAL_PANIC("somehow passed non ptr type to '{}'", node.name());
                    return ir_builder->CreateLoad(fumo_to_llvm_type(node.type), val);
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

            if (bin.kind == BinaryExpr::assignment) {
                auto* lhs_addr = codegen_lvalue(*bin.lhs).value_or(nullptr);
                auto* rhs_val  = codegen_rvalue(*bin.rhs).value_or(nullptr);
                
                if (!lhs_addr || !rhs_val) {
                    INTERNAL_PANIC("[Codegen] found null value in assignment for '{}'.", node.name());
                }
                
                // global initialization goes in fumo init
                if (auto var = get_if<VariableDecl>(bin.lhs)) {
                    if (var->kind == VariableDecl::global_var_declaration) {
                        return fumo_init_builder->CreateStore(rhs_val, lhs_addr);
                    }
                }

                return ir_builder->CreateStore(rhs_val, lhs_addr);
            }

            auto* lhs_val = codegen_rvalue(*bin.lhs).value_or(nullptr);
            auto* rhs_val = codegen_rvalue(*bin.rhs).value_or(nullptr);

            if (!lhs_val || !rhs_val) {
                INTERNAL_PANIC("[Codegen] found null value in binary operand for '{}'.", node.name());
            }

            switch (bin.kind) {
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
                default:
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
            }
        }

        holds(PostfixExpr, const& postfix) {
            // TODO: finish postfix expr codegen
            // for now we make function calls work without other postfixes
            if (postfix.nodes.size() != 1) {
                INTERNAL_PANIC("codegen only implemented for function calls");
            }
            return codegen_rvalue(*postfix.nodes[0]);
        }

        holds(VariableDecl, const& var) {
            // variable declarations return their alloca address (used during initialization)
            return codegen_lvalue(node);
        }

        holds(FunctionDecl, const& func) {
            // NOTE: we delete forward declarations and only keep the first occurence
            auto* llvm_func = llvm_module->getFunction(get_id(func).mangled_name);
            if (func.body) {
                llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", llvm_func);
                ir_builder->SetInsertPoint(bblock);
                ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());

                for (int i = 0; i < func.parameters.size(); i++) {
                    auto* param = func.parameters[i];
                    codegen_lvalue(*param);
                    
                    // some notes because this is confusing to me:
                    // formal arguments (parameters), when used in the function body,
                    // have the values of the function call args
                    auto* formal_arg = llvm_func->args().begin() + i;
                    ir_builder->CreateStore(formal_arg, param->llvm_value);
                }

                codegen_rvalue(*func.body.value());

                if (node.type.kind == Type::void_) ir_builder->CreateRetVoid();
            }
            return llvm_func;
        }

        holds(FunctionCall, const& func_call) {
            // Function calls: all arguments are rvalues
            if (auto* llvm_func = llvm_module->getFunction(get_id(func_call).mangled_name)) {
                vec<llvm::Value*> arg_values {};
                for (auto* arg : func_call.argument_list) {
                    // All function arguments are rvalues - no special handling needed!
                    if (auto* val = codegen_rvalue(*arg).value_or(nullptr)) {
                        arg_values.push_back(val);
                    } else {
                        INTERNAL_PANIC("[Codegen] couldn't codegen function argument '{}'.", arg->name());
                    }
                }
                return ir_builder->CreateCall(llvm_func, arg_values);
            } else {
                INTERNAL_PANIC("[Codegen] {}'s declaration didn't have a llvm::Function.", node.name());
            }
        }

        holds(BlockScope, const& scope) {
            for (auto* node : scope.nodes) codegen_rvalue(*node);
        }

        holds(TypeDecl, const& type_decl) {
            if (type_decl.definition_body) {
                for (auto* node : type_decl.definition_body.value()) codegen_rvalue(*node);
            }
            // NOTE: type decls also codegens its own member function decls
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
        }

        holds(NamespaceDecl) {
            INTERNAL_PANIC("namespaces shouldn't be codegen'd, found '{}'.", node.name());
        }

        _default {
            INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
        }
    }
    
    return std::nullopt;
}

// returns the ADDRESS where a value is stored (for assignment, address-of)
Opt<llvm::Value*> Codegen::codegen_lvalue(ASTNode& node) {
    match(node) {

        holds(Identifier, const& id) {
            switch (id.kind) {
                case Identifier::var_name: {
                    ASTNode* declaration = id.declaration.value();
                    if (!declaration->llvm_value) {
                        INTERNAL_PANIC("[Codegen Error] forgot to assign llvm::Value to declaration for '{}'.",
                                       get_id(get<VariableDecl>(declaration)).mangled_name);
                    }
                    // Return the alloca address directly (this IS the lvalue)
                    return declaration->llvm_value;
                }
                case Identifier::member_var_name:
                    // TODO: this needs GEP from the base address of the struct
                    INTERNAL_PANIC("codegen not implemented for '{}'", node.name());
                    break;
                default:
                    report_error(node.source_token, "Cannot assign to identifier '{}'", id.mangled_name);
            }
        }

        holds(UnaryExpr, const& un) {
            // *ptr as lvalue - the pointer value itself is the address we want
            if (un.kind == UnaryExpr::dereference) {
                return codegen_rvalue(*un.expr);
            } else {
                report_error(node.source_token, "expression is not assignable.");
            }
        }

        holds(VariableDecl, const& var) {
            // Variable declarations create storage and return the address
            switch (var.kind) {
                case VariableDecl::global_var_declaration:
                    return node.llvm_value;  // Global variable address
                case VariableDecl::parameter:
                case VariableDecl::variable_declaration: {
                    auto* type = fumo_to_llvm_type(node.type);
                    node.llvm_value = ir_builder->CreateAlloca(type, nullptr, get_id(var).mangled_name);
                    return node.llvm_value;  // Return alloca address
                }
            }
        }

        // TODO: Add array indexing, member access, etc.
        // holds(PostfixExpr with array_index): return GEP address
        // holds(PostfixExpr with member_access): return GEP address

        _default {
            report_error(node.source_token, "expression is not assignable.");
        }
    }
    return std::nullopt;
}

// adds function prototypes and forward declarations of types
void Codegen::register_declaration(std::string_view name, ASTNode& node) {

    match(node) {
        holds(TypeDecl, const& type_decl) {
            // TODO: continue this codegen
            llvm::StructType::create(*llvm_context, name);
        }
        holds(FunctionDecl, const& func) {

            vec<llvm::Type*> param_types {};

            for (const auto& param : func.parameters) {
                param_types.push_back(fumo_to_llvm_type(param->type));
                // codegen(*param); NOTE: removing this from here
            }
            llvm::FunctionType* func_type = llvm::FunctionType::get(fumo_to_llvm_type(node.type), param_types, false);

            llvm::Function* llvm_func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                                                               get_id(func).mangled_name, llvm_module.get());
            node.llvm_value = llvm_func;
        }
        holds(VariableDecl, const& var) {
            // we only recieve globals here
            if (var.kind != VariableDecl::global_var_declaration) {
                INTERNAL_PANIC("should only register globals, found local variable '{}'.",
                               get_id(var).mangled_name);
            }
            auto* global_var = new llvm::GlobalVariable(fumo_to_llvm_type(node.type), false,
                                                        llvm::GlobalValue::ExternalLinkage,
                                                        llvm::Constant::getNullValue(fumo_to_llvm_type(node.type)),
                                                        get_id(var).mangled_name);
            llvm_module->insertGlobalVariable(global_var);
            node.llvm_value = global_var;
        }
        _default INTERNAL_PANIC("added wrong declaration '{}' to 'symbol_table.all_declarations', ", node.name());
    }
}

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

