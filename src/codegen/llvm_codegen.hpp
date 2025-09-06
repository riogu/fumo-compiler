#pragma once
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Target/TargetMachine.h>
#include <map>
#include "base_definitions/ast_node.hpp"
#include "base_definitions/symbol_table_stack.hpp"

struct Codegen {
  private:
    std::map<str, llvm::AllocaInst*> variable_env;
    unique_ptr<llvm::LLVMContext> llvm_context;
    unique_ptr<llvm::IRBuilder<>> ir_builder;
    unique_ptr<llvm::Module> llvm_module; // if the module isnt destroyed before the context we get segv
    std::stringstream file_stream;
    SymbolTableStack symbol_tree {};
    ASTNode* file_root_node;

  public:
    Codegen(const File& file, SymbolTableStack& symbol_tree) : symbol_tree(std::move(symbol_tree)) {
        llvm_context = std::make_unique<llvm::LLVMContext>();
        ir_builder   = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
        llvm_module  = std::make_unique<llvm::Module>(file.output_name.string(), *llvm_context);
        llvm_module->setSourceFileName(file.path_name.string());
        file_stream << file.contents;
    }

    void codegen_file(ASTNode* file_scope);
    void compile_module(llvm::OptimizationLevel opt_level = llvm::OptimizationLevel::O0);

    [[nodiscard]] str llvm_ir_to_str() {
        std::string outstr;
        llvm::raw_string_ostream oss(outstr);
        llvm_module->print(oss, nullptr);
        return oss.str();
    }

  private:
    Opt<llvm::Value*> codegen( ASTNode& node);
    void register_declaration(std::string_view name, const ASTNode& node);
    llvm::Function* create_main();

    constexpr llvm::Type* fumo_to_llvm_type(const Type& fumo_type) {
        switch (fumo_type.kind) {
            // case TypeKind::_union:
            // case TypeKind::_enum:
            case Type::struct_: {
                auto type = llvm::StructType::getTypeByName(*llvm_context, get_name(fumo_type));
                if (type == nullptr) INTERNAL_PANIC("couldn't get llvm::Type for '{}'", get_id(fumo_type).mangled_name);
                return type;
            }
            case Type::Nothing:  return llvm::Type::getVoidTy(*llvm_context);
            case Type::i8_:      return llvm::Type::getInt8Ty(*llvm_context);
            case Type::i32_:     return llvm::Type::getInt32Ty(*llvm_context);
            case Type::i64_:     return llvm::Type::getInt64Ty(*llvm_context);
            case Type::f32_:     return llvm::Type::getFloatTy(*llvm_context);
            case Type::f64_:     return llvm::Type::getDoubleTy(*llvm_context);
            case Type::bool_:    return llvm::Type::getInt1Ty(*llvm_context);
            case Type::void_:    return llvm::Type::getVoidTy(*llvm_context);
            case Type::str_:   // TODO: add string types
            default:
                INTERNAL_PANIC("couldn't get llvm::Type for '{}'", get_id(fumo_type).mangled_name);
        }
        return {};
    }
};
