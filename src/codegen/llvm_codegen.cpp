#include "codegen/llvm_codegen.hpp"
#include "base_definitions/ast_node.hpp"

void Codegen::codegen_file(ASTNode* file_root_node) {
    this->file_root_node = file_root_node;

    // create_libc_functions();

    llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvm_context), {}, false);
    llvm::Function* fumo_init = llvm::Function::Create(func_type, llvm::Function::InternalLinkage,
                                                       "fumo.init", llvm_module.get());
    fumo_init->setDSOLocal(true);
    fumo_init->addFnAttr("used");

    llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", fumo_init);
    fumo_init_builder->SetInsertPoint(bblock);
    fumo_init_builder->SetCurrentDebugLocation(llvm::DebugLoc());
    // codegen
    // ---------------------------------------------------------------------------
    // forward declaration of all structs as opaque first
    for (const auto& [name, _] : symbol_tree.type_decls) llvm::StructType::create(*llvm_context, name);

    for (const auto& [_, node] : symbol_tree.all_declarations) register_declaration(*node);
    for (const auto& node : get<NamespaceDecl>(file_root_node).nodes) codegen_value(*node);
    // ---------------------------------------------------------------------------

    fumo_init_builder->SetInsertPoint(&fumo_init->back());
    fumo_init_builder->SetCurrentDebugLocation(llvm::DebugLoc());
    fumo_init_builder->CreateRetVoid();

    verify_user_main();

    auto* user_main = llvm_module->getFunction("fumo.user_main");
    if (user_main) { create_libc_main(); }
}
// returns the ADDRESS where a value is stored (for assignment, address-of)
Opt<llvm::Value*> Codegen::codegen_address(ASTNode& node) {
    match(node) {

        holds(Identifier, const& id) {
            switch (id.kind) {
                case Identifier::var_name: {
                    ASTNode* declaration = if_value(id.declaration)
                                           else_panic_error(node.source_token, "missing declaration somehow.");
                    if (!declaration->llvm_value) {
                        internal_panic("[Codegen] forgot to assign llvm::Value to declaration for '{}'.",
                                       get_id(get<VariableDecl>(declaration)).mangled_name);
                    }
                    // Return the alloca address directly (this IS the lvalue)
                    return declaration->llvm_value;
                }
                case Identifier::member_var_name: {
                    int member_index = get<VariableDecl>(id.declaration.value()).member_index.value();
                    auto* type = llvm::StructType::getTypeByName(*llvm_context, id.base_struct_name);
                    if(!type) report_error(node.source_token, "wrong type name '{}'", id.base_struct_name);
                    if (node.llvm_value) {
                        return ir_builder->CreateStructGEP(type, node.llvm_value, member_index);
                    }
                    else if (current_this_ptr) { // implicit 'this' usage
                        auto* this_ptr = ir_builder->CreateLoad(ir_builder->getPtrTy(), current_this_ptr.value());
                        return ir_builder->CreateStructGEP(type, this_ptr, member_index);
                    } else {
                        internal_error(node.source_token, "[Codegen] forgot to assign llvm::Value or curr_this_ptr in '{}'.",
                                       node.name());
                    }
                }

                default:
                    report_error(node.source_token, "identifier '{}' is not assignable.", id.mangled_name);
            }
        }

        holds(UnaryExpr, const& un) {
            if (un.kind != UnaryExpr::dereference) report_error(node.source_token, "result of unary expression is not assignable.");
            if (!un.expr.value()->type.ptr_count) internal_panic("somehow passed non ptr type to '{}'", node.name());
            // NOTE: might be able to use this to report errors on using function return values

            // pass the ptr to the original struct to member function calls that were dereferenced
            if (node.llvm_value && !un.expr.value()->llvm_value) un.expr.value()->llvm_value = node.llvm_value;
            // *ptr as lvalue - the pointer value itself is the address we want
            auto* address = if_value(codegen_value(*un.expr.value()))
                            else_panic("[Codegen] found null value in UnaryExpr '{}'.", node.name());
            // -------------------------------------------------------------------------------------------
            // check null pointer dereferences
            llvm::Function* parent_function = ir_builder->GetInsertBlock()->getParent();
            llvm::Function* null_error_fn = get_or_create_fumo_runtime_error();

            auto* trap_bb = llvm::BasicBlock::Create(*llvm_context, "null_trap", parent_function);
            auto* safe_bb = llvm::BasicBlock::Create(*llvm_context, "safe_deref", parent_function);
            // Check if pointer is null
            llvm::Constant* null_ptr_const = llvm::Constant::getNullValue(address->getType());
            llvm::Value* is_null = ir_builder->CreateICmpEQ(address, null_ptr_const, "is_null");
            ir_builder->CreateCondBr(is_null, trap_bb, safe_bb);
            // Trap block - executed on null pointer
            ir_builder->SetInsertPoint(trap_bb);

            str error_msg = make_runtime_error(node.source_token, "found null pointer dereference");
            llvm::Constant* error_str = ir_builder->CreateGlobalString(error_msg, ".str_error_msg");
            ir_builder->CreateCall(null_error_fn, {error_str});
            ir_builder->CreateUnreachable();
            // Safe dereference block
            ir_builder->SetInsertPoint(safe_bb);
            // -------------------------------------------------------------------------------------------
            return address;
            
        }

        holds(VariableDecl, const& var) {
            // variable declarations create storage and return the address
            switch (var.kind) {
                case VariableDecl::member_var_declaration: internal_panic("member variables don't need codegen.");
                case VariableDecl::global_var_declaration:
                    return node.llvm_value;  // global var address
                case VariableDecl::parameter:
                case VariableDecl::variable_declaration: { 
                    // NOTE: all declarations are ALWAYS 0 initialized even if not assigned
                    auto* type = fumo_to_llvm_type(node.type);
                    node.llvm_value = ir_builder->CreateAlloca(type, nullptr, get_id(var).mangled_name);
                    ir_builder->CreateStore(llvm::Constant::getNullValue(type), node.llvm_value);
                    return node.llvm_value;  // alloca address
                }
            }
        }

        // this case requires specific attention
        holds(PostfixExpr, const& postfix) { // this is the lvalue postfix (wants address)
            auto* curr_node = postfix.nodes[0];
            llvm::Value* curr_ptr;
            // NOTE: should work even with first member being function call
            // this requires more work | var->func().x = 123; // still not solved atm
            curr_ptr = if_value(codegen_address(*curr_node))
                       else_error(curr_node->source_token, "found no value in postfix expression (fix this).");
            for (std::size_t i = 1; i < postfix.nodes.size(); i++) {
                curr_node = postfix.nodes[i];
                curr_node->llvm_value = curr_ptr; // used later by the elements for codegen
                // if present, it wasnt wrapped by a dereference earlier
                if (is_branch<FunctionCall>(curr_node)) {
                    auto* ret_val = if_value(codegen_value(*curr_node))
                                    else_error(curr_node->source_token, "found no value in postfix expr (FIX).");
                    auto* prev_node = postfix.nodes[i - 1]; // trying to access pointer element with '.' syntax.
                    if (i < postfix.nodes.size() && prev_node->type.ptr_count) { 
                        report_error(prev_node->source_token, // not the last element and wasn't dereferenced
                                     "Member reference type '{}' is a pointer; did you mean to use '->'?",
                                     type_name(prev_node->type));
                    }
                    if (node.type.kind == Type::void_) return std::nullopt; 
                    llvm::Value* temp_alloca = ir_builder->CreateAlloca(ret_val->getType());
                    ir_builder->CreateStore(ret_val, temp_alloca);
                    curr_ptr = temp_alloca; // need to guarantee we always have an address

                } else {
                    // %curr_ptr is a pointer to the address of the struct member 
                    // %curr_ptr = getelementptr inbounds nuw %struct.foo, ptr %struct_ptr, i32 0, i32 index
                    curr_ptr = if_value(codegen_address(*curr_node))
                               else_error(curr_node->source_token, "found no value in postfix expression (FIX THIS).");
                }
            }
            return curr_ptr;
        }

        _default { report_error(node.source_token, "expression is not assignable."); }
    }
    return std::nullopt;
}

// returns the ACTUAL VALUE (for expressions, function args, etc)
Opt<llvm::Value*> Codegen::codegen_value(ASTNode& node) {
    
    match(node) {
        holds(Identifier, const& id) {
            switch (id.kind) {
                // Load the value from the alloca
                case Identifier::var_name: {
                    auto* address = codegen_address(node).value();
                    return ir_builder->CreateLoad(fumo_to_llvm_type(node.type), address);
                }
                case Identifier::member_var_name: {
                    auto* address = codegen_address(node).value();
                    return ir_builder->CreateLoad(fumo_to_llvm_type(node.type), address);
                }
                case Identifier::func_call_name:
                case Identifier::type_name:
                case Identifier::declaration_name:
                case Identifier::member_func_call_name:
                case Identifier::unknown_name:
                    internal_panic("shouldn't codegen this identifier: '{}'.", node.name());
            }
        }

        holds(PrimaryExpr, const& prim) {
            switch (prim.kind) {
                case PrimaryExpr::integer:
                    return ir_builder->getInt32(std::get<int64_t>(prim.value));
                case PrimaryExpr::floating_point:
                    return llvm::ConstantFP::get(llvm::Type::getFloatTy(*llvm_context),
                                                 std::get<double>(prim.value));
                case PrimaryExpr::str: {
                    auto* global_str = ir_builder->CreateGlobalString(std::get<str>(prim.value), ".str");
                    return ir_builder->CreateConstGEP2_32(global_str->getType(), global_str, 0, 0);
                }
                case PrimaryExpr::bool_:
                    return ir_builder->getInt1(std::get<bool>(prim.value));
                case PrimaryExpr::void_:
                    return llvm::Constant::getNullValue(ir_builder->getPtrTy());
                case PrimaryExpr::char_literal:
                    return ir_builder->getInt8(std::get<int64_t>(prim.value));
                default:
                    internal_panic("codegen not implemented for '{}'", node.name());
            }
        }

        holds(UnaryExpr, const& un) {
            // &expr - get the lvalue (address) of the expression
            if (un.kind == UnaryExpr::address_of) return codegen_address(*un.expr.value());

            if (un.kind == UnaryExpr::dereference) {
                auto* address = codegen_address(node).value();
                return ir_builder->CreateLoad(fumo_to_llvm_type(node.type), address);
            }
            if (un.kind == UnaryExpr::return_statement) {
                if (node.type.kind == Type::void_ && !node.type.ptr_count) return ir_builder->CreateRetVoid();

                auto* val = if_value(codegen_value(*un.expr.value()))
                            else_panic("[Codegen] found null value in UnaryExpr '{}'.", node.name());
                if (ir_builder->getCurrentFunctionReturnType() == llvm::Type::getVoidTy(*llvm_context) && un.expr) {
                    report_error(node.source_token, "void function shouldn't return a value.");
                }
                if (ir_builder->getCurrentFunctionReturnType() != val->getType()) { 
                    // check if we can promote the returned value to the function return,
                    // otherwise we report an error
                    auto func_name = ir_builder->GetInsertBlock()->getParent()->getName().data();
                    Type func_type = symbol_tree.all_declarations[func_name]->type;

                    if (is_arithmetic_t(func_type) && is_arithmetic_t(node.type)) {
                        val = convert_arithmetic_t(val, ir_builder->getCurrentFunctionReturnType());
                    } else {
                        report_error(node.source_token, "function return type '{}' does not match return value's type '{}'.",
                                     type_name(func_type),
                                     type_name(node.type));
                    }
                }
                return ir_builder->CreateRet(val);

            }

            auto* val = if_value(codegen_value(*un.expr.value()))
                        else_panic("[Codegen] found null value in UnaryExpr '{}'.", node.name());

            switch (un.kind) {
                case UnaryExpr::negate:
                    if (val->getType()->isIntegerTy()) return ir_builder->CreateNeg(val);
                    else if (val->getType()->isFloatingPointTy()) return ir_builder->CreateFNeg(val);
                    else {
                        report_error(node.source_token, "cannot apply '-' to value of type '{}'", type_name(un.expr.value()->type));
                    }
                case UnaryExpr::logic_not: 
                    if (val->getType()->isIntegerTy()) {
                        if (val->getType()->isIntegerTy(1)) return ir_builder->CreateNot(val);
                        else {
                            return ir_builder->CreateICmpEQ(val, llvm::ConstantInt::get(val->getType(), 0));
                        }
                    } else if (val->getType()->isPointerTy()) { // compare with nullptr
                        return ir_builder->CreateICmpEQ(val, llvm::ConstantPointerNull::get(ir_builder->getPtrTy()));

                    } else if (val->getType()->isFloatingPointTy()) {
                        return ir_builder->CreateFCmpOEQ(val, llvm::ConstantFP::get(val->getType(), 0.0));

                    } else {
                        report_error(node.source_token, "cannot apply '!' to value of type '{}'", type_name(un.expr.value()->type));
                    }
                case UnaryExpr::bitwise_not: 
                    if (val->getType()->isIntegerTy()) return ir_builder->CreateNot(val);
                    else {
                        report_error(node.source_token, "cannot apply '~' to value of type '{}'", type_name(un.expr.value()->type));
                    }

                default:
                    internal_panic("codegen not implemented for '{}'", node.name());
            }
        }

        holds(BinaryExpr, const& bin) {
            switch (bin.kind) {
                case BinaryExpr::assignment: {
                    auto* old_block = ir_builder->GetInsertBlock();

                    if (auto* var = get_if<VariableDecl>(bin.lhs)) {
                        if (var->kind == VariableDecl::global_var_declaration) {
                            ir_builder->SetInsertPointPastAllocas(llvm_module->getFunction("fumo.init"));
                            ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());
                        }
                    }
                    auto* lhs_addr = if_value(codegen_address(*bin.lhs))
                                     else_error(node.source_token, "cannot assign to expression.");
                                             
                    auto* rhs_val  = if_value(codegen_value(*bin.rhs))
                                     else_panic("[Codegen] found null value in rhs of assignment for '{}'.", node.name());

                    ir_builder->CreateStore(rhs_val, lhs_addr);

                    if (auto* var = get_if<VariableDecl>(bin.lhs)) { // put the ir_builder back outside fumo.init
                        if (var->kind == VariableDecl::global_var_declaration) ir_builder->SetInsertPoint(old_block);
                    }
                    return rhs_val;
                }
                case BinaryExpr::logical_and: {
                    llvm::Function* curr_func = ir_builder->GetInsertBlock()->getParent();
                    auto* rhs_block = llvm::BasicBlock::Create(*llvm_context, "logic_and.rhs", curr_func);
                    auto* end_block = llvm::BasicBlock::Create(*llvm_context, "logic_and.end", curr_func);

                    auto* lhs_val = if_value(codegen_value(*bin.lhs))
                                    else_panic_error(bin.rhs->source_token, "can't get rvalue for argument of '{}'.", node.name());
                    auto* lhs_block = ir_builder->GetInsertBlock(); // AFTER the lhs codegen
                    lhs_val = llvm_value_to_bool(lhs_val);
                    // short circuit the lhs (if true, try rhs, else end)
                    ir_builder->CreateCondBr(lhs_val, rhs_block, end_block);

                    // evaluate rhs (only if the lhs was false do we get here)
                    ir_builder->SetInsertPoint(rhs_block);
                    ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());

                    auto* rhs_val = if_value(codegen_value(*bin.rhs))
                                    else_panic_error(bin.rhs->source_token, "can't get rvalue for argument of '{}'.", node.name());
                    llvm::Value* rhs_bool = llvm_value_to_bool(rhs_val);

                    auto* rhs_block_after = ir_builder->GetInsertBlock(); // block after RHS evaluation (might have changed!)
                    ir_builder->CreateBr(end_block);

                    ir_builder->SetInsertPoint(end_block);
                    ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());
                    // phi nodes select the final value based on which block we came from
                    // so if we came from the rhs, we use that as the final value (only executed if lhs was true)
                    // (basically, its how we make temporaries have multiple values while still in SSA form)
                    llvm::PHINode* phi = ir_builder->CreatePHI(ir_builder->getInt1Ty(), 2, "phi.result");
                    phi->addIncoming(ir_builder->getFalse(), lhs_block); // came from lhs (so we had false)
                    phi->addIncoming(rhs_bool, rhs_block_after); // came from rhs
                    return phi;
                }
                case BinaryExpr::logical_or: {
                    llvm::Function* curr_func = ir_builder->GetInsertBlock()->getParent();
                    auto* rhs_block = llvm::BasicBlock::Create(*llvm_context, "logic_or.rhs", curr_func);
                    auto* end_block = llvm::BasicBlock::Create(*llvm_context, "logic_or.end", curr_func);

                    auto* lhs_val = if_value(codegen_value(*bin.lhs))
                                    else_panic_error(bin.rhs->source_token, "can't get rvalue for argument of '{}'.", node.name());
                    auto* lhs_block = ir_builder->GetInsertBlock();

                    lhs_val = llvm_value_to_bool(lhs_val); // if true, go to the end, else continue
                    ir_builder->CreateCondBr(lhs_val, end_block, rhs_block);

                    ir_builder->SetInsertPoint(rhs_block);
                    ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());

                    auto* rhs_val = if_value(codegen_value(*bin.rhs))
                                    else_panic_error(bin.rhs->source_token, "can't get rvalue for argument of '{}'.", node.name());
                    llvm::Value* rhs_bool = llvm_value_to_bool(rhs_val);

                    auto* rhs_block_after = ir_builder->GetInsertBlock(); // curr block might have changed
                    ir_builder->CreateBr(end_block);

                    ir_builder->SetInsertPoint(end_block);
                    ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());
                    llvm::PHINode* phi = ir_builder->CreatePHI(ir_builder->getInt1Ty(), 2, "phi.result");
                    phi->addIncoming(ir_builder->getTrue(), lhs_block); // lhs was true
                    phi->addIncoming(rhs_bool, rhs_block_after);
                    return phi;
                }
                default: ;
                // the rest of the cases work the same
                // implemented below
            }
            auto* lhs_val = codegen_value(*bin.lhs).value_or(nullptr);
            auto* rhs_val = codegen_value(*bin.rhs).value_or(nullptr);
            if (!lhs_val || !rhs_val) internal_panic("[Codegen] found null value in binary operand for '{}'.", node.name());
            //-------------------------------------------------------------------------------------------
            // pointer arithmetic cases
            if (bin.kind == BinaryExpr::add) {
                if (is_ptr_t(bin.lhs->type)) {
                    Type pointed_type = node.type; pointed_type.ptr_count--; // get pointed-to type
                    return ir_builder->CreateGEP(fumo_to_llvm_type(pointed_type), lhs_val, rhs_val);
                }
            }
            if (bin.kind == BinaryExpr::sub) {
                if (is_ptr_t(bin.lhs->type)) {
                    Type pointed_type = node.type; pointed_type.ptr_count--; // get pointed-to type
                    return ir_builder->CreateGEP(fumo_to_llvm_type(pointed_type), lhs_val, ir_builder->CreateNeg(rhs_val));
                }
            }
            if (bin.kind == BinaryExpr::equal) {
                if (is_ptr_t(bin.lhs->type) && is_ptr_t(bin.rhs->type)) return ir_builder->CreateICmpEQ(lhs_val, rhs_val);
            }
            if (bin.kind == BinaryExpr::not_equal) {
                if (is_ptr_t(bin.lhs->type) && is_ptr_t(bin.rhs->type)) return ir_builder->CreateICmpNE(lhs_val, rhs_val);
            }
            //-------------------------------------------------------------------------------------------
            // normal binary operator cases
            TypePromotion promo = promote_operands(lhs_val, rhs_val, is_signed_t(bin.lhs->type), is_signed_t(bin.rhs->type));
            lhs_val = promo.promoted_lhs; rhs_val = promo.promoted_rhs;
            auto* op_type = promo.common_type;
            switch (bin.kind) {
                case BinaryExpr::add: 
                    if (op_type->isIntegerTy()) return ir_builder->CreateAdd(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy())   return ir_builder->CreateFAdd(lhs_val, rhs_val);
                    break;
                case BinaryExpr::sub:
                    if (op_type->isIntegerTy()) return ir_builder->CreateSub(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy())   return ir_builder->CreateFSub(lhs_val, rhs_val);
                    break;
                case BinaryExpr::modulus:
                    if (op_type->isIntegerTy()) return ir_builder->CreateSRem(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy()) return ir_builder->CreateFRem(lhs_val, rhs_val);
                    break;
                case BinaryExpr::multiply:
                    if (op_type->isIntegerTy()) return ir_builder->CreateMul(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy())   return ir_builder->CreateFMul(lhs_val, rhs_val);
                    break;
                case BinaryExpr::divide:
                    if (op_type->isIntegerTy()) return ir_builder->CreateSDiv(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy())   return ir_builder->CreateFDiv(lhs_val, rhs_val);
                    break;
                case BinaryExpr::equal:
                    if (op_type->isIntegerTy()) return ir_builder->CreateICmpEQ(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy())   return ir_builder->CreateFCmpOEQ(lhs_val, rhs_val);
                    break;
                case BinaryExpr::not_equal:
                    if (op_type->isIntegerTy()) return ir_builder->CreateICmpNE(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy())   return ir_builder->CreateFCmpONE(lhs_val, rhs_val);
                    break;
                case BinaryExpr::less_than:
                    if (op_type->isIntegerTy()) return ir_builder->CreateICmpSLT(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy())   return ir_builder->CreateFCmpOLT(lhs_val, rhs_val);
                    break;
                case BinaryExpr::less_equals:
                    if (op_type->isIntegerTy()) return ir_builder->CreateICmpSLE(lhs_val, rhs_val);
                    if (op_type->isFloatingPointTy())   return ir_builder->CreateFCmpOLE(lhs_val, rhs_val);
                    break;
                default: internal_panic("codegen not implemented for '{}'", node.name());
            }
            internal_error(node.source_token, "invalid types in '{}', found '{}' and '{}'.",
                           node.name(), type_name(bin.lhs->type), type_name(bin.rhs->type));
        }

        // let x = myvar.member.other->foo().e;
        // myvar.member.other->foo()->e = 123;
        holds(PostfixExpr, const& postfix) {
            auto* val = if_value(codegen_address(node))
                        else_error(node.source_token, "expression is not assignable.");
            return ir_builder->CreateLoad(fumo_to_llvm_type(node.type), val);
        }

        holds(VariableDecl, const& var) {
            // done this way just so we can call the same function in codegen_file()
            if (var.kind != VariableDecl::global_var_declaration) {
                internal_panic("[Codegen] can't call codegen_value() on '{}' other than global variables.", node.name());
            }
            return codegen_address(node);
        }

        holds(FunctionDecl, const& func) {
            // NOTE: we move the body of the function to the first occurence
            // all others are forward declarations
            auto* llvm_func = llvm_module->getFunction(get_id(func).mangled_name);
            if (func.body) {
                llvm::BasicBlock* bblock = llvm::BasicBlock::Create(*llvm_context, "", llvm_func);
                ir_builder->SetInsertPoint(bblock);
                ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());

                int this_arg_offset = 0;
                if (func.kind == FunctionDecl::member_func_declaration  // adding 'this' pointer
                    && !node.type.qualifiers.contains(Type::static_)) { // except static member functions
                    this_arg_offset = 1;
                    current_this_ptr = ir_builder->CreateAlloca(ir_builder->getPtrTy(), nullptr, "this");
                    // copy the parameter VALUE into the local ADDRESS
                    ir_builder->CreateStore(llvm_func->getArg(0), current_this_ptr.value());
                    //                      source value          destination address
                }
                for (int i = 0; i < func.parameters.size(); i++) {
                    auto* param = func.parameters[i];
                    codegen_address(*param);
                    // some notes because this is confusing to me:
                    // formal arguments (parameters), when used in the function body,
                    // have the values of the function call args
                    auto* formal_arg = llvm_func->getArg(i + this_arg_offset);
                    ir_builder->CreateStore(formal_arg, param->llvm_value);
                }

                codegen_value(*func.body.value());

                if (!ir_builder->GetInsertBlock()->getTerminator()) {
                    if (node.type.kind == Type::void_) ir_builder->CreateRetVoid();
                }

                current_this_ptr = std::nullopt;
                if (get_id(func).mangled_name != "fumo.user_main") { // main function has special handling
                    for (auto& block : *llvm_func) { // cleanup untermined unreachable blocks
                        if (!block.getTerminator()) { // main has an implicit return 0; for users
                            llvm::IRBuilder<> builder(&block);
                            builder.CreateUnreachable();
                        }
                    }
                }
            }
            return llvm_func;
        }

        holds(FunctionCall, const& func_call) {
            // Function calls: all arguments are rvalues
            // TODO: add 'this' pointer if its a member function (in llvm_value member)
            if (auto* llvm_func = llvm_module->getFunction(get_id(func_call).mangled_name)) {
                vec<llvm::Value*> arg_values {};
                if (func_call.kind == FunctionCall::member_function_call &&
                    !node.type.qualifiers.contains(Type::static_)) { // dont add 'this' for static functions
                    if (node.llvm_value) {
                        arg_values.push_back(node.llvm_value);
                    } else if (current_this_ptr) {
                        // this means we are callin a member function from another member function
                        // so we must pass forward the "this" argument
                        node.llvm_value = ir_builder->CreateLoad(ir_builder->getPtrTy(), current_this_ptr.value());
                        arg_values.push_back(node.llvm_value);
                    } else {
                        if (!node.llvm_value) internal_error(node.source_token, "didn't set llvm::Value for '{}'", node.name());
                        if(!current_this_ptr.value())  internal_error(node.source_token, "didn't set curr_this_ptr '{}'", node.name());
                    }
                }
                auto& func_decl = get<FunctionDecl>(symbol_tree.all_declarations[get_id(func_call).mangled_name]);
                for (int i = 0; i < func_call.argument_list.size(); i++) {
                    auto* arg = func_call.argument_list[i];
                    // All function arguments are rvalues
                    auto* val = if_value(codegen_value(*arg))
                                else_panic("[Codegen] couldn't codegen function argument '{}'.", arg->name());
                    // we need to promote integers correctly for every situation, function args, initializer lists, etc...
                    // might be worth doing it in semantic analysis instead of here
                    if (i < func_decl.parameters.size()) {
                        auto* param = func_decl.parameters[i];
                        if (is_arithmetic_t(arg->type) && is_arithmetic_t(param->type)) {
                                val = convert_arithmetic_t(val, fumo_to_llvm_type(param->type));
                        }
                    }
                    arg_values.push_back(val);
                }
                return ir_builder->CreateCall(llvm_func, arg_values);
            } else {
                internal_panic("[Codegen] {}'s declaration didn't have a llvm::Function.", node.name());
            }
        }

        holds(BlockScope, const& scope) {
            // NOTE: might need more codegen
            switch (scope.kind) {
                case BlockScope::compound_statement:
                    for (auto* node : scope.nodes) {
                        // var->member(); // this is valid (ignoring return value)
                        // ONLY happens if we have a postfix by itself (not an assignment)
                        if (is_branch<PostfixExpr>(node) || is_branch<VariableDecl>(node)) {
                             codegen_address(*node);
                        } else {
                            codegen_value(*node);
                        }
                    }
                    break;
                case BlockScope::initializer_list: 
                    if (fumo_to_llvm_type(node.type)->isStructTy()) { // wont be a pointer to struct
                        auto* struct_type = fumo_to_llvm_type(node.type);
                        // fill with 0s before initialization
                        auto* struct_ptr = ir_builder->CreateAlloca(struct_type);
                        ir_builder->CreateStore(llvm::Constant::getNullValue(struct_type), struct_ptr);
                        //-------------------------------------------------------------------------
                        // filling initializer list with default values
                        TypeDecl type_decl;
                        if find_value (get_id(node.type).mangled_name, symbol_tree.type_decls) {
                            type_decl = get<TypeDecl>(iter->second);
                        } else internal_error(node.source_token, "somehow type declaration was wrong.");

                        for (auto* member_node : type_decl.definition_body.value()) {
                            if (auto* assign = get_if<BinaryExpr>(member_node)) {
                                auto* rvalue = if_value(codegen_value(*assign->rhs))
                                               else_error(member_node->source_token,
                                                          "[Codegen] can't get rvalue for argument of '{}'.", node.name());
                                int member_index = get<VariableDecl>(assign->lhs).member_index.value();
                                auto* field_ptr = ir_builder->CreateStructGEP(struct_type, struct_ptr, member_index);
                                ir_builder->CreateStore(rvalue, field_ptr);
                            }
                        }
                        //-------------------------------------------------------------------------
                        // the values the user passed in
                        // NOTE: adding {.member = 40} syntax is easy for the codegen
                        const auto& member_nodes = type_decl.definition_body.value();
                        for (std::size_t i = 0; i < scope.nodes.size(); ++i) {
                            auto* arg = scope.nodes[i];
                            auto* val = if_value(codegen_value(*arg))
                                           else_panic("[Codegen] can't get rvalue for argument of '{}'.", node.name());
                            //-------------------------------------------------------------------------
                            // numeric promotion (probably remove once we add casting)
                            if (i < member_nodes.size()) {
                                auto* member = member_nodes[i];
                                if (is_arithmetic_t(arg->type) && is_arithmetic_t(member->type)) {
                                        val = convert_arithmetic_t(val, fumo_to_llvm_type(member->type));
                                }
                            }
                            //-------------------------------------------------------------------------
                            // LLVM treats each struct ptr as an array, so we must derefence it at '0'
                            // getelementptr inbounds nuw %struct.foo, ptr %struct_ptr, i32 0, i32 i
                            // ptr %struct_ptr -> this is "an array" of structs with 1 element
                            auto* field_ptr = ir_builder->CreateStructGEP(struct_type, struct_ptr, i);
                            ir_builder->CreateStore(val, field_ptr); // Store to field i
                        }
                        // we expect an rvalue, so we must load the value (not return the alloca)
                        return ir_builder->CreateLoad(struct_type, struct_ptr);

                    } else if (scope.nodes.size() == 0) { 
                        // empty list is always null initialized (other than structs with default members)
                        return llvm::Constant::getNullValue(fumo_to_llvm_type(node.type));

                    } else if (scope.nodes.size() == 1) { // allows init list syntax for i32 and other types
                        auto* rvalue = if_value(codegen_value(*scope.nodes[0]))
                                       else_panic("[Codegen] can't get rvalue for '{}'.", node.name());
                        return rvalue;

                    } else {
                        report_error(node.source_token, "non-struct initializer lists can only have 0 or 1 arguments.");
                    }
                    break;
            }
        }

        holds(TypeDecl, const& type_decl) {
            if (type_decl.definition_body) {
                for (auto* node : type_decl.definition_body.value()) {
                    // only codegen member functions
                    if (is_branch<FunctionDecl, TypeDecl>(node)) codegen_value(*node);
                }
            }
        }

        holds(NamespaceDecl, &nmspace) {
            for (auto* node : nmspace.nodes) codegen_value(*node);
        }

        holds(IfStmt, &if_stmt) { // NOTE: we use node.llvm_value to store the final merging "end.if" block
            llvm::Function* curr_func = ir_builder->GetInsertBlock()->getParent();

            switch (if_stmt.kind) {
                case IfStmt::if_statement: 
                case IfStmt::else_if_statement: {
                    // ----------------------------------------------------------------------
                    // if statement condition
                    auto* cond = if_value(if_stmt.condition) else_panic("missing condition in '{}'.", node.name());
                    auto* cond_val = if_value(codegen_value(*cond))
                                     else_panic_error(cond->source_token, "can't get rvalue for argument of '{}'.", node.name());
                    cond_val = llvm_value_to_bool(cond_val);
                    // ----------------------------------------------------------------------
                    // create basic blocks for if statement

                    auto* if_true_block = llvm::BasicBlock::Create(*llvm_context, "begin.if", curr_func);

                    llvm::BasicBlock* else_block = nullptr;
                    if (auto* else_stmt = if_stmt.else_stmt.value_or(nullptr)) {
                        str block_name = get<IfStmt>(else_stmt).kind == IfStmt::else_statement ? "else" : "else.if";
                        else_block = llvm::BasicBlock::Create(*llvm_context, block_name, curr_func);
                    } 
                    if (if_stmt.kind == IfStmt::if_statement) {
                        node.llvm_value = llvm::BasicBlock::Create(*llvm_context, "end.if", // make end block
                                                                   ir_builder->GetInsertBlock()->getParent());
                    }
                    // ----------------------------------------------------------------------
                    if (else_block) {
                        ir_builder->CreateCondBr(cond_val, if_true_block, else_block);
                    }
                    else { // there was no else, jump to after the if statement
                        ir_builder->CreateCondBr(cond_val, if_true_block, llvm::cast<llvm::BasicBlock>(node.llvm_value));
                    }
                    // ----------------------------------------------------------------------
                    // if body
                    ir_builder->SetInsertPoint(if_true_block);
                    ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());
                    codegen_value(*if_stmt.body);

                    if (!ir_builder->GetInsertBlock()->getTerminator()) {
                        // only if there was no return or similar by the user
                        ir_builder->CreateBr(llvm::cast<llvm::BasicBlock>(node.llvm_value));
                    }

                    if (auto* else_stmt = if_stmt.else_stmt.value_or(nullptr)) {
                        else_stmt->llvm_value = node.llvm_value; // pass on the end.if branch

                        ir_builder->SetInsertPoint(else_block);
                        ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());
                        codegen_value(*else_stmt); // simply codegen the else statement

                        if (!else_block->getTerminator()) {
                            ir_builder->SetInsertPoint(else_block);
                            ir_builder->CreateBr(llvm::cast<llvm::BasicBlock>(node.llvm_value));
                        }
                    } // node.llvm_value stores the final merge basic block
                    // needs to be moved to the end for correctness (checking final return later)
                    auto* end_block = llvm::cast<llvm::BasicBlock>(node.llvm_value);
                    end_block->moveAfter(&curr_func->back());

                    ir_builder->SetInsertPoint(end_block);
                    ir_builder->SetCurrentDebugLocation(llvm::DebugLoc());
                    break;
                }
                case IfStmt::else_statement: 
                    codegen_value(*if_stmt.body);

                    if (!ir_builder->GetInsertBlock()->getTerminator()) {
                        // only if there was no return or similar by the user
                        ir_builder->CreateBr(llvm::cast<llvm::BasicBlock>(node.llvm_value));
                    }
                    break;
            }
        }
        holds(WhileStmt, &while_stmt) {
            auto* curr_func = ir_builder->GetInsertBlock()->getParent();
            auto* cond_block = llvm::BasicBlock::Create(*llvm_context, "while.cond", curr_func);
            auto* body_block = llvm::BasicBlock::Create(*llvm_context, "while.body", curr_func);
            auto* end_block  = llvm::BasicBlock::Create(*llvm_context, "while.end", curr_func);
    
            // start at the condition block (so we can loop back to it)
            ir_builder->CreateBr(cond_block);

            ir_builder->SetInsertPoint(cond_block); // evalute condition inside the cond block
            auto* cond_val = if_value(codegen_value(*while_stmt.condition))
                             else_panic_error(node.source_token, "couldn't generate while loop condition");
            cond_val = llvm_value_to_bool(cond_val);

            ir_builder->CreateCondBr(cond_val, body_block, end_block);

            ir_builder->SetInsertPoint(body_block);
            codegen_value(*while_stmt.body);

            if (!ir_builder->GetInsertBlock()->getTerminator()) {
                ir_builder->CreateBr(cond_block); // jump back to condition of we reach the end
            }

            ir_builder->SetInsertPoint(end_block);
        }
        _default {
            report_error(node.source_token, "expression for '{}' is not an rvalue.", node.name());
        }
    }
    
    return std::nullopt;
}

// adds function prototypes and forward declarations of types
void Codegen::register_declaration(ASTNode& node) {

    match(node) {
        holds(TypeDecl, const& type_decl) {
            // TODO: continue this codegen
            auto* struct_type = llvm::cast<llvm::StructType>(fumo_to_llvm_type(node.type));

            if (type_decl.definition_body) {
                vec<llvm::Type*> member_types {};
                for (auto* node : type_decl.definition_body.value()) {
                    if (!is_branch<FunctionDecl, TypeDecl>(node)) member_types.push_back(fumo_to_llvm_type(node->type));
                }
                if (auto error = struct_type->setBodyOrError(member_types)) {
                    // Handle the error
                    std::string error_msg;
                    llvm::raw_string_ostream stream(error_msg);
                    stream << error;
                    report_error(node.source_token, "failed to set struct body: {}", error_msg);
                }
            }
        }

        holds(FunctionDecl, const& func) {

            auto* found_func = llvm_module->getFunction(get_id(func).mangled_name);
            if (found_func) return;

            vec<llvm::Type*> param_types {};
            if (func.kind == FunctionDecl::member_func_declaration &&  // adding 'this' pointer
                !node.type.qualifiers.contains(Type::static_)) {       // but not to static functions
                param_types.push_back(llvm::PointerType::getUnqual(*llvm_context));
            }
            for (const auto& param : func.parameters) {
                param_types.push_back(fumo_to_llvm_type(param->type));
            }
            llvm::FunctionType* func_type = llvm::FunctionType::get(fumo_to_llvm_type(node.type), param_types, func.is_variadic);

            llvm::Function* llvm_func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                                                               get_id(func).mangled_name, llvm_module.get());
            node.llvm_value = llvm_func;
        }

        holds(VariableDecl, const& var) {
            // we only recieve globals here
            if (var.kind != VariableDecl::global_var_declaration) {
                internal_panic("should only register globals, found local variable '{}'.",
                               get_id(var).mangled_name);
            }
            auto* global_var = new llvm::GlobalVariable(fumo_to_llvm_type(node.type), false,
                                                        llvm::GlobalValue::ExternalLinkage,
                                                        llvm::Constant::getNullValue(fumo_to_llvm_type(node.type)),
                                                        get_id(var).mangled_name);
            llvm_module->insertGlobalVariable(global_var);
            node.llvm_value = global_var;
        }

        _default internal_panic("added wrong declaration '{}' to 'symbol_table.all_declarations', ", node.name());
    }
}
