#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic_analysis/analyzer.hpp"
#include "codegen/llvm_codegen.hpp"
#include "utils/common_utils.hpp"
#include <csignal>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/InitLLVM.h>
#include <print>
#include "llvm/Support/Signals.h"

struct foo {
    int x;
    void func() {}

    static foo make() {
        return foo {};
    }
};


extern "C" const char* __asan_default_options() { return "detect_leaks=0"; }
    
llvm::OptimizationLevel opt_level = llvm::OptimizationLevel::O2;
#define was_opt_level(level) if (O##level.getNumOccurrences()) opt_level = llvm::OptimizationLevel::O##level; else 

using namespace llvm::cl;
OptionCategory fumo_category("Fumo Compiler Options",
                             "  Options for controlling the compilation process, such as opt levels");

opt<bool> O0          {"O0",           desc("No optimizations"),                                      cat(fumo_category)};
opt<bool> O1          {"O1",           desc("Few optimizations"),                                     cat(fumo_category)};
opt<bool> O2          {"O2",           desc("Default optimizations"),                                 cat(fumo_category)};
opt<bool> O3          {"O3",           desc("Aggressive optimizations"),                              cat(fumo_category)};

opt<bool> output_IR_O0{"emit-ir-O0",   desc("Outputs a .ll file with the generated llvm IR"),         cat(fumo_category)};
opt<bool> output_IR   {"emit-ir",      desc("Outputs a .ll file with the generated llvm IR"),         cat(fumo_category)};
opt<bool> output_AST  {"emit-ast",     desc("Outputs a .ast file with the generated debug AST"),      cat(fumo_category)};
opt<bool> output_ASM  {"emit-asm",     desc("Outputs a .asm file with the generated assembly"),       cat(fumo_category)};
opt<bool> output_OBJ  {"emit-obj",     desc("Outputs a .o object file"),                              cat(fumo_category)};

opt<bool> print_file  {"print-file",   desc("Print the inputed file contents to the terminal"),       cat(fumo_category)};
opt<bool> print_IR    {"print-ir",     desc("Print llvm-IR to the terminal"),                         cat(fumo_category)};
opt<bool> print_AST   {"print-ast",    desc("Prints the debug AST representation to the terminal"),   cat(fumo_category)};
opt<bool> print_ASM   {"print-asm",    desc("Prints the output ASM to the terminal"),                 cat(fumo_category)};

opt<bool> verbose     {"v",            desc("Show commands executed during compilation and linking"), cat(fumo_category)};

opt<str>  out_file    {"o",            desc("Output filename"), value_desc("filename"),               cat(fumo_category)};
list<str> input_files {Positional,    desc("Input source files"), value_desc("filenames"), OneOrMore, cat(fumo_category)};

// Linking control
opt<bool> no_link     {"c",            desc("Compile only, do not link (produces .o files)"),         cat(fumo_category)};
opt<bool> static_link {"static",       desc("Create a statically linked executable"),                 cat(fumo_category)};
opt<bool> strip_syms  {"s",            desc("Strip symbol table from executable"),                    cat(fumo_category)};
// Linker selection
opt<str>  linker_name  {"linker",      desc("Specify which linker to use (auto/gcc/clang)"), 
                       value_desc("linker"), init("auto"),                                            cat(fumo_category)};
list<str> libraries   {"l",            desc("Link with library"), 
                       value_desc("library"), ZeroOrMore,                                             cat(fumo_category)};
list<str> lib_paths   {"L",            desc("Add directory to library search path"), 
                       value_desc("directory"), ZeroOrMore,                                           cat(fumo_category)};

void custom_handler(int sig) {
    // print stack trace without bug report message
    // this removes the "PLEASE submit a bug report" message specifically
    llvm::sys::PrintStackTrace(llvm::errs());
    std::exit(1);
}

// #define fn auto
// #define i64 int64_t
//
// fn fib(i64 n) -> i64 {
//     //1
//     if (n < 2) {
//         return 0;
//     } else {
//         //2
//         if(n > 2) return fib(n - 1) + fib(n - 2);
//         //3
//         else return 0;
//     }
// }
// fn e() -> int {
//     i64 var;
//     scanf("%ld", &var);
//     printf("%ld\n", fib(var));
//     return 0;
// }
// while loops
// control flow analysis
// void*
// casting
// var as i32*;
auto main(int argc, char** argv) -> int {
    // TODO: warn users if they provide a non existent source file
    llvm::InitLLVM init(argc, argv); 
    signal(SIGSEGV, custom_handler); signal(SIGABRT, custom_handler);
    signal(SIGFPE,  custom_handler); signal(SIGILL,  custom_handler);

    //--------------------------------------------------------------------------
    // Lexer and Command-line arguments parsing
    vec<str> obj_files = {};

    llvm::cl::HideUnrelatedOptions(fumo_category);
    llvm::cl::ParseCommandLineOptions(argc, argv, str(str("ᗜ") + gray("‿") + str("ᗜ Fumo Compiler\n")));

    for (const auto& file_name : input_files) {
        if (!std::filesystem::exists(file_name)) std::print("Error: file '{}' not found\n", file_name), std::exit(1); 

        { map_macro(was_opt_level, 0, 1, 2, 3); }

        Lexer lexer {};
        auto [tokens, file] = lexer.tokenize_file(file_name);
        //--------------------------------------------------------------------------
        // Parser
        Parser parser {file};
        auto file_root_node = parser.parse_tokens(tokens);
        //--------------------------------------------------------------------------
        // Semantic Analysis
        Analyzer analyzer {file};
        analyzer.semantic_analysis(file_root_node);
        //--------------------------------------------------------------------------
        // recursive structs will crash the AST printing until after semantic analysis
        // Codegen
        // for (const auto& node : get<NamespaceDecl>(file_root_node).nodes) {
        //     std::cerr << "node found:\n  " + node->to_str() + "\n";
        // }
        Codegen codegen {file, analyzer.symbol_tree};
        codegen.codegen_file(file_root_node);
        obj_files.push_back(codegen.compile_file(opt_level));
    }
    link_object_files(obj_files, out_file.getNumOccurrences()? out_file.getValue() : "fumo.out");
}
