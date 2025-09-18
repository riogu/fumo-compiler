#pragma once
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Target/TargetMachine.h>
#include "base_definitions/ast_node.hpp"
#include "base_definitions/symbol_table_stack.hpp"

enum struct LinkerType { AUTO, GCC, CLANG};

struct LinkOptions {
    LinkerType linker = LinkerType::AUTO;
    str output_name = "src/tests/command-line-string.out";
    vec<str> object_files;
    bool static_link = false;
    bool strip_symbols = false;
    bool verbose = false;
    vec<str> libraries;     // -l flags
    vec<str> library_paths; // -L flags
};
struct TypePromotion {
    llvm::Type* common_type;
    llvm::Value* promoted_lhs;
    llvm::Value* promoted_rhs;
    bool is_signed;
};

extern llvm::cl::opt<bool> output_IR, output_AST, output_ASM, output_OBJ, output_IR_O0, 
                           print_file, print_IR, print_AST, print_ASM,
                           verbose, no_link, static_link, strip_syms;
extern llvm::cl::opt<str>  linker_name;
extern llvm::cl::list<str> libraries, lib_paths;

enum struct ValueKind { no_value, address, value };

LinkOptions build_link_options(const str& output_name, const vec<str>& object_files);
Result<void, str> link_executable(const LinkOptions& opts);
void link_object_files(vec<str> obj_files, fs::path exec_name);

struct Codegen {
  private:
    std::map<str, llvm::AllocaInst*> variable_env;
    unique_ptr<llvm::LLVMContext> llvm_context;
    std::unique_ptr<llvm::Module> llvm_module; // if the module isnt destroyed before the context we get segv
    std::unique_ptr<llvm::IRBuilder<>> ir_builder;
    std::unique_ptr<llvm::IRBuilder<>> fumo_init_builder; 
    std::stringstream file_stream;
    SymbolTableStack symbol_tree {};
    ASTNode* file_root_node;
    File file;

    // used during the FunctionDecl context. is *NOT* used by the function call at all
    Opt<llvm::Value*> current_this_ptr {};

    // used to be able to assign to a function call if deferenced:
    // *func() = 123; 
    // valid as long as func() returns a pointer, and that pointer is dereferenced
    ValueKind prev_value_kind = ValueKind::no_value;

  public:
    Codegen(const File& file, SymbolTableStack& symbol_tree) : symbol_tree(std::move(symbol_tree)), file(file) {
        llvm_context = std::make_unique<llvm::LLVMContext>();
        ir_builder   = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
        fumo_init_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
        llvm_module  = std::make_unique<llvm::Module>(file.path_name.string(), *llvm_context);
        llvm_module->setSourceFileName(file.path_name.string());
        file_stream << file.contents;
    }

    void codegen_file(ASTNode* file_scope);

    void compile_module(llvm::OptimizationLevel opt_level);
    [[nodiscard]] fs::path compile_file(llvm::OptimizationLevel opt_level);

    [[nodiscard]] str llvm_ir_to_str() {
        std::string outstr;
        llvm::raw_string_ostream oss(outstr);
        llvm_module->print(oss, nullptr);
        return oss.str();
    }

  private:
    void register_declaration(ASTNode& node);
    Opt<llvm::Value*> codegen_address(ASTNode& node);
    Opt<llvm::Value*> codegen_value(ASTNode& node);

    void clear_metadata();
    void create_libc_main();
    void verify_user_main();
    void create_libc_functions();
    llvm::Value* node_to_bool(ASTNode& node);
    TypePromotion promote_operands(llvm::Value* lhs, llvm::Value* rhs, bool lhs_signed = true,
                                   bool rhs_signed = true);
    llvm::Value* convert_arithmetic_t(llvm::Value* value, llvm::Type* target_type, bool is_signed = true);

    llvm::Value* llvm_value_to_bool(llvm::Value* val) {
        // you can add optional types and that kinda thing here later 
        if (val->getType()->isIntegerTy(1)) return val;

        if (val->getType()->isIntegerTy()) {
            llvm::Value* zero = llvm::ConstantInt::get(val->getType(), 0);
            return ir_builder->CreateICmpNE(val, zero, "tobool");
        }
        if (val->getType()->isPointerTy()) {
            llvm::Value* null_ptr = llvm::ConstantPointerNull::get(ir_builder->getPtrTy());
            return ir_builder->CreateICmpNE(val, null_ptr, "tobool");
        }
        if (val->getType()->isFloatingPointTy()) {
            llvm::Value* zero_fp = llvm::ConstantFP::get(val->getType(), 0.0);
            return ir_builder->CreateFCmpONE(val, zero_fp, "tobool");
        }
        internal_panic("Cannot convert type to boolean");
    }

    llvm::Type* fumo_to_llvm_type(const Type& fumo_type) {

        if (fumo_type.ptr_count) return ir_builder->getPtrTy();

        switch (fumo_type.kind) {
            case Type::struct_: {
                auto type = llvm::StructType::getTypeByName(*llvm_context, type_name(fumo_type));
                if (type == nullptr) internal_panic("couldn't get llvm::Type for '{}'", type_name(fumo_type));
                return type;
            }
            case Type::u8_:      // same llvm type as i8, etc...
            case Type::i8_:      return llvm::Type::getInt8Ty(*llvm_context);   
            case Type::u16_:    
            case Type::i16_:     return llvm::Type::getInt16Ty(*llvm_context);   
            case Type::u32_:   
            case Type::i32_:     return llvm::Type::getInt32Ty(*llvm_context);  
            case Type::u64_:  
            case Type::i64_:     return llvm::Type::getInt64Ty(*llvm_context);  
            case Type::f32_:     return llvm::Type::getFloatTy(*llvm_context);  
            case Type::f64_:     return llvm::Type::getDoubleTy(*llvm_context); 
            case Type::bool_:    return llvm::Type::getInt1Ty(*llvm_context);   
            case Type::void_:    return llvm::Type::getVoidTy(*llvm_context);
            // case Type::str_:     return llvm::PointerType::getUnqual(*llvm_context);
                break;
            default:
                internal_panic("couldn't get llvm::Type for '{}'", type_name(fumo_type));
        }
    }

    llvm::Function* get_or_create_fumo_runtime_error();
    llvm::Function* get_or_create_printf();
    llvm::Function* get_or_create_puts();
    llvm::Function* get_or_create_strlen();
    llvm::Function* get_or_create_strcmp();
    llvm::Function* get_or_create_strcpy();
    llvm::Function* get_or_create_strcat();
    llvm::Function* get_or_create_malloc();
    llvm::Function* get_or_create_free();
    llvm::Function* get_or_create_exit();
    llvm::Function* get_or_create_abort();
    llvm::Function* get_or_create_memcpy();
    llvm::Function* get_or_create_memset();
};
