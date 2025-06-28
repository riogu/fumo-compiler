#include "lexer/lexer.hpp"
#include <cpptrace/from_current.hpp>
#include <iostream>

#define fn auto
using i32 = int;
using i64 = int64_t;
#define TRY CPPTRACE_TRY
#define CATCH(param) CPPTRACE_CATCH(param)

fn main() -> i32 {
    fs::path test = "src/tests/testfile.c";
    if (Lexer lexer; auto tokens = lexer.tokenize_file(test)) {
        for (auto& token : tokens.value())
            std::cout << token.to_str() << "\t-> " << token.type_name() << "\n";
    } else
        PANIC(tokens.error());
}

// std::cout << token_to_str(token) << " (" << tokentype_name(token.type) << ") | ";
// std::cout << token_to_str(token);
