#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic_analysis/analyzer.hpp"
#include "codegen/llvm_codegen.hpp"
#include "utils/common_utils.hpp"

#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/InitLLVM.h>

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

opt<bool> output_IR   {"emit-llvm-ir", desc("Outputs a .ll file with the generated llvm IR"),         cat(fumo_category)};
opt<bool> output_AST  {"emit-ast",     desc("Outputs a .ast file with the generated debug AST"),      cat(fumo_category)};
opt<bool> output_ASM  {"emit-asm",     desc("Outputs a .asm file with the generated assembly"),       cat(fumo_category)};
opt<bool> output_OBJ  {"emit-obj",     desc("Outputs a .o object file"),                              cat(fumo_category)};

opt<bool> print_file  {"print-file",   desc("Print the inputed file contents to the terminal"),       cat(fumo_category)};
opt<bool> print_IR    {"print-ir",     desc("Print llvm-IR to the terminal"),                         cat(fumo_category)};
opt<bool> print_AST   {"print-ast",    desc("Prints the debug AST representation to the terminal"),   cat(fumo_category)};
opt<bool> print_ASM   {"print-asm",    desc("Prints the output ASM to the terminal"),                 cat(fumo_category)};

opt<bool> verbose     {"v",            desc("Show commands executed during compilation and linking"), cat(fumo_category)};

opt<str>  out_file    {"o",            desc("Output filename"), value_desc("filename"),               cat(fumo_category)};
list<str> input_files {"i",            desc("Input files"),     value_desc("filenames"), OneOrMore,   cat(fumo_category)};

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


auto main(int argc, char** argv) -> int {
    llvm::InitLLVM init(argc, argv);
    str file_name = "command-line-string.fm";
    str cmdline_str = "";
    // -O3 -emit=llvm-ir -print-ast -print-file -print-ir -v


    //--------------------------------------------------------------------------
    // Lexer and Command-line arguments parsing
    Lexer lexer {};
    vec<Token> tokens; File file;
    bool received_cmdline_str = false;

    if (argc == 2) {
        cmdline_str = argv[1];
        // cant delete llvm 'Generic Options' from their cl parser
        // so we will allow the user to pass them in
        received_cmdline_str = !(cmdline_str == "-h"                 || cmdline_str == "-help" 
                              || cmdline_str == "--help-list"        || cmdline_str == "--help-list"
                              || cmdline_str == "-help-list-hidden"  || cmdline_str == "-help-hidden"
                              || cmdline_str == "--help-list-hidden" || cmdline_str == "--help-hidden"
                              || cmdline_str == "-version");
    }
    if (received_cmdline_str) {
        // this is for testing the compiler. you can pass in a program like:
        // "fn main() -> i32 {let x = 231;}" 
        // as one string to the compiler (with no flags) and it will compile it. it uses the flags below
        output_IR = true; output_ASM = true; output_OBJ = true;
        // print_file = true; 
        // print_AST = true;
        // print_IR = true;
        verbose = true;

        { map_macro(was_opt_level, 0, 1, 2, 3); }
        auto [_tokens, _file] = lexer.tokenize_string(file_name, cmdline_str);
        tokens = _tokens;
        file = _file;
        file.output_name = "src/tests/command-line-string.out";
    } else {
        llvm::cl::HideUnrelatedOptions(fumo_category);
        llvm::cl::ParseCommandLineOptions(argc, argv, str(str("ᗜ") + gray("‿") + str("ᗜ Fumo Compiler\n")));

        file_name = input_files[0]; // only one file name

        { map_macro(was_opt_level, 0, 1, 2, 3); }

        auto [_tokens, _file] = lexer.tokenize_file(file_name);
        tokens = _tokens; file = _file;
    }
    //--------------------------------------------------------------------------
    // Parser
    Parser parser {file};
    auto file_root_node = parser.parse_tokens(tokens);
    //--------------------------------------------------------------------------
    // Semantic Analysis
    // NOTE: recursive structs will crash the AST printing until after semantic analysis
    Analyzer analyzer {file};
    analyzer.semantic_analysis(file_root_node);
    //--------------------------------------------------------------------------
    // Codegen
    if (out_file.getNumOccurrences()) file.output_name = out_file.getValue();
    // for (const auto& node : get<NamespaceDecl>(file_root_node).nodes) {
    //     std::cerr << "node found:\n  " + node->to_str() + "\n";
    // }
    Codegen codegen {file, analyzer.symbol_tree};
    codegen.codegen_file(file_root_node);
    codegen.compile_and_link_module(opt_level);
}
