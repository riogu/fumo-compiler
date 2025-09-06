#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic_analysis/analyzer.hpp"
#include "codegen/llvm_codegen.hpp"
#include "utils/common_utils.hpp"
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/InitLLVM.h>

llvm::cl::OptionCategory compiler_category("Fumo Compiler Options",
                                           "  Options for controlling the compilation process, such as opt levels");

llvm::cl::opt<bool> O0("O0", llvm::cl::desc("No optimizations"),         llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> O1("O1", llvm::cl::desc("Few optimizations"),        llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> O2("O2", llvm::cl::desc("Default optimizations"),    llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> O3("O3", llvm::cl::desc("Aggressive optimizations"), llvm::cl::cat(compiler_category));

llvm::cl::opt<bool> output_IR ("emit=llvm-ir",llvm::cl::desc("Outputs a .ll file with the generated llvm IR"),
                               llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> output_AST("emit=ast",    llvm::cl::desc("Outputs a .ast file with the generated debug AST"),
                               llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> output_ASM("emit=asm",    llvm::cl::desc("Outputs a .asm file with the generated assembly"),
                               llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> output_OBJ("emit=obj",    llvm::cl::desc("Outputs a .o object file"),
                               llvm::cl::cat(compiler_category));

llvm::cl::opt<bool> print_file("print-file",  llvm::cl::desc("Print the inputed file contents to the terminal"),
                               llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> print_IR  ("print-ir",    llvm::cl::desc("Print llvm-IR to the terminal"),
                               llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> print_AST ("print-ast",   llvm::cl::desc("Prints the debug AST representation to the terminal"),
                               llvm::cl::cat(compiler_category));
llvm::cl::opt<bool> print_ASM ("print-asm",   llvm::cl::desc("Prints the output ASM to the terminal"),
                               llvm::cl::cat(compiler_category));

llvm::cl::opt<std::string> output_filename("o", llvm::cl::desc("Output filename"), llvm::cl::value_desc("filename"), 
                                           llvm::cl::cat(compiler_category));
llvm::cl::list<std::string> input_filenames("i", llvm::cl::desc("Input files"), llvm::cl::value_desc("filenames"),
                                           llvm::cl::OneOrMore, llvm::cl::cat(compiler_category));


llvm::OptimizationLevel opt_level = llvm::OptimizationLevel::O2;
#define was_opt_level(level) if (O##level.getNumOccurrences()) opt_level = llvm::OptimizationLevel::O##level; else 

auto main(int argc, char** argv) -> int {
    llvm::InitLLVM init(argc, argv);
    str file_name = "command-line-string.fm";
    str cmdline_str = "";

    //--------------------------------------------------------------------------
    // Lexer
    Lexer lexer {};
    vec<Token> tokens; File file;
    bool received_cmdline_str = false;

    if (argc == 2) {
        cmdline_str = argv[1];
        received_cmdline_str = !(cmdline_str == "-h" || cmdline_str == "-help" || cmdline_str == "--help");
    }
    if (received_cmdline_str) {
        print_file = true;
        print_IR = true;
        output_IR = true;
        // print_AST = true;
        auto [_tokens, _file] = lexer.tokenize_string(file_name, cmdline_str);
        tokens = _tokens;
        file = _file;
        file.output_name = "tests/command-line-string.fm";
    } else {
        llvm::cl::HideUnrelatedOptions(compiler_category);
        llvm::cl::ParseCommandLineOptions(argc, argv, std::string(str("ᗜ") + gray("‿") + str("ᗜ Fumo Compiler\n")));

        file_name = input_filenames[0];

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
    Analyzer analyzer {file};
    analyzer.semantic_analysis(file_root_node);
    //--------------------------------------------------------------------------
    // Codegen
    if (output_filename.getNumOccurrences()) file.output_name = output_filename.getValue();

    Codegen codegen {file, analyzer.symbol_tree};
    codegen.codegen_file(file_root_node);

    codegen.compile_module(opt_level);
}
