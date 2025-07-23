#include "parser/ast_node.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <map>

struct Codegen {
  private:
    std::map<str, llvm::AllocaInst*> variable_table;
    unique_ptr<llvm::LLVMContext> llvm_context;
    unique_ptr<llvm::IRBuilder<>> ir_builder;
    unique_ptr<llvm::Module> llvm_module;

  public:
    Codegen(str name) {
        llvm_context = std::make_unique<llvm::LLVMContext>();
        ir_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
        llvm_module = std::make_unique<llvm::Module>(name, *llvm_context);
    }

    void codegen(vec<ASTNode>& AST);

    void print_llvm_ir() {
        llvm_module->print(llvm::outs(), nullptr);
    }

    [[nodiscard]] str llvm_ir_to_str() {
        std::string outstr;
        llvm::raw_string_ostream oss(outstr);
        llvm_module->print(oss, nullptr);
        return oss.str();
    }

  private:
    llvm::Value* codegen(ASTNode* node);
    llvm::Value* codegen(ASTNode* node, Primary& branch);
    llvm::Value* codegen(ASTNode* node, Unary& branch);
    llvm::Value* codegen(ASTNode* node, Binary& branch);
    llvm::Value* codegen(ASTNode* node, Variable& branch);
    llvm::Value* codegen(ASTNode* node, Function& branch);
    llvm::Value* codegen(ASTNode* node, Scope& branch);

    constexpr llvm::Type* fumo_to_llvm_type(const Type& fumo_type) {
        switch (fumo_type.kind) {
            // case TypeKind::_struct:
            // case TypeKind::_union:
            // case TypeKind::_enum:
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
            // case TypeKind::_str:
            default:
                INTERNAL_PANIC("can't convert '{}' to llvm::Type", fumo_type.name);
        }
        return {};
    }
};
