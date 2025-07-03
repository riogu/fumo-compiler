#include "lexer/lexer.hpp"
#include <cpptrace/from_current.hpp>
#include <iostream>

#define fn auto
using i32 = int;
using i64 = int64_t;
#define TRY CPPTRACE_TRY
#define CATCH(param) CPPTRACE_CATCH(param)

fn main(int argc, char* argv[]) -> i32 {


    bool huh;
    bool wow = huh = 2;

    fs::path test = (argc > 1) ? argv[1] : "src/tests/testfile.c";

    // TODO: write a script to run all the tests for the compiler (up to current stage)

    if (Lexer lexer; auto tokens = lexer.tokenize_file(test)) {
        for (auto& token : tokens.value())
            std::cout << token.to_str() << "\t-> " << token.type_name() << "\n";
    } else
        PANIC(tokens.error());
}
