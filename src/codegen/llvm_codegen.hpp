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
    str output_name = "tests/command-line-string.out";
    vec<str> object_files;
    bool static_link = false;
    bool strip_symbols = false;
    bool verbose = false;
    vec<str> libraries;     // -l flags
    vec<str> library_paths; // -L flags
};
extern llvm::cl::opt<bool> output_IR, output_AST, output_ASM, output_OBJ, 
                           print_file, print_IR, print_AST, print_ASM,
                           verbose, no_link, static_link, strip_syms;
extern llvm::cl::opt<str>  linker_name;
extern llvm::cl::list<str> libraries, lib_paths;

struct Codegen {
  private:
    std::map<str, llvm::AllocaInst*> variable_env;
    unique_ptr<llvm::LLVMContext> llvm_context;
    unique_ptr<llvm::IRBuilder<>> ir_builder;
    std::unique_ptr<llvm::IRBuilder<>> fumo_init_builder; 
    unique_ptr<llvm::Module> llvm_module; // if the module isnt destroyed before the context we get segv
    std::stringstream file_stream;
    SymbolTableStack symbol_tree {};
    ASTNode* file_root_node;
    File file;

  public:
    Codegen(const File& file, SymbolTableStack& symbol_tree) : symbol_tree(std::move(symbol_tree)), file(file) {
        llvm_context = std::make_unique<llvm::LLVMContext>();
        ir_builder   = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
        fumo_init_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
        llvm_module  = std::make_unique<llvm::Module>(file.output_name.string(), *llvm_context);
        llvm_module->setSourceFileName(file.path_name.string());
        file_stream << file.contents;
    }

    void codegen_file(ASTNode* file_scope);

    void compile_module(llvm::OptimizationLevel opt_level);
    void compile_and_link_module(llvm::OptimizationLevel opt_level);

    Result<void, str> link_executable(const LinkOptions& opts);
    Result<void, str> link_simple(const str& object_file, LinkerType linker_type = LinkerType::AUTO);
    LinkOptions build_link_options(const str& output_name, const vec<str>& object_files);

    [[nodiscard]] str llvm_ir_to_str() {
        std::string outstr;
        llvm::raw_string_ostream oss(outstr);
        llvm_module->print(oss, nullptr);
        return oss.str();
    }

  private:
    Opt<llvm::Value*> codegen(ASTNode& node);
    void register_declaration(std::string_view name, ASTNode& node);

    void clear_metadata();
    void create_libc_main();

    constexpr llvm::Type* fumo_to_llvm_type(const Type& fumo_type) {

        if (fumo_type.ptr_count) return llvm::PointerType::getUnqual(*llvm_context);

        switch (fumo_type.kind) {
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

    }
};

