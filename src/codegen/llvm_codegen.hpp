#pragma once
#include "parser/ast_node.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <map>

struct Codegen {
  private:
    std::map<str, llvm::AllocaInst*> variable_env;
    unique_ptr<llvm::LLVMContext> llvm_context;
    unique_ptr<llvm::IRBuilder<>> ir_builder;
    unique_ptr<llvm::Module> llvm_module;
    std::stringstream file_stream;

  public:
    Codegen(const File& file) {
        llvm_context = std::make_unique<llvm::LLVMContext>();
        ir_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
        llvm_module = std::make_unique<llvm::Module>(file.path_name.string(), *llvm_context);
        file_stream << file.contents;
    }

    void codegen(BlockScope& file_scope);

    void print_llvm_ir() { llvm_module->print(llvm::outs(), nullptr); }

    [[nodiscard]] str llvm_ir_to_str() {
        std::string outstr;
        llvm::raw_string_ostream oss(outstr);
        llvm_module->print(oss, nullptr);
        return oss.str();
    }

  private:
    llvm::Value* codegen(const ASTNode& node);
    llvm::Value* codegen(const ASTNode& node, const PrimaryExpr& branch);
    llvm::Value* codegen(const ASTNode& node, const UnaryExpr& branch);
    llvm::Value* codegen(const ASTNode& node, const BinaryExpr& branch);
    llvm::Value* codegen(const ASTNode& node, const VariableDecl& branch);
    llvm::Value* codegen(const ASTNode& node, const FunctionDecl& branch);
    llvm::Value* codegen(const ASTNode& node, const BlockScope& branch);

    constexpr llvm::Type* fumo_to_llvm_type(const Type& fumo_type) {
        switch (fumo_type.kind) {
            // case TypeKind::_union:
            // case TypeKind::_enum:
            case TypeKind::_struct: {
                auto type = llvm::StructType::getTypeByName(*llvm_context, fumo_type.name);
                if (type == nullptr) INTERNAL_PANIC("couldn't get llvm::Type for '{}'", fumo_type.name);
                return type;
            }
            case TypeKind::_void:
                return llvm::Type::getVoidTy(*llvm_context);
            case TypeKind::_i8:
                return llvm::Type::getInt8Ty(*llvm_context);
            case TypeKind::_i32:
                return llvm::Type::getInt32Ty(*llvm_context);
            case TypeKind::_i64:
                return llvm::Type::getInt64Ty(*llvm_context);
            case TypeKind::_f32:
                return llvm::Type::getFloatTy(*llvm_context);
            case TypeKind::_f64:
                return llvm::Type::getDoubleTy(*llvm_context);
            case TypeKind::_bool:
                return llvm::Type::getInt1Ty(*llvm_context);
            case TypeKind::_str:
            default:
                INTERNAL_PANIC("couldn't get llvm::Type for '{}'", fumo_type.name);
        }
        return {};
    }
};
