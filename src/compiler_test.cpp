#include <cstdlib>
#include <format>
#include <print>
#include <string>

std::pair<std::string, int> exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    int status = pclose(pipe);
    return {result, status};
}
#define t std::pair
constexpr bool fail = false, pass = true;

int main() {
    
    constexpr std::array all_tests {
        #include "../tests/early-ast-tests/ast_syntax.fm"
        #include "../tests/early-ast-tests/postfix.fm"
        #include "../tests/early-ast-tests/scope_basic_checks.fm"
        #include "../tests/early-ast-tests/scope_name_lookup.fm"

        #include "../tests/codegen-tests/simple_llvm_codegen.fm"
        #include "../tests/codegen-tests/codegen_func_calls.fm"
    };
    // ------------------------------------------------------
    // these tests cover up to namespaces and basic struct syntax
    constexpr std::array ast_syntax {
        #include "../tests/early-ast-tests/ast_syntax.fm"
    };
    constexpr std::array postfix {
        #include "../tests/early-ast-tests/postfix.fm"
    };
    constexpr std::array scope_basic_checks {
        #include "../tests/early-ast-tests/scope_basic_checks.fm"
    };
    constexpr std::array scope_name_lookup {
        #include "../tests/early-ast-tests/scope_name_lookup.fm"
    };
    // ------------------------------------------------------
    // tests for codegen
    constexpr std::array simple_llvm_codegen {
        #include "../tests/codegen-tests/simple_llvm_codegen.fm"
    };
    constexpr std::array codegen_func_calls {
        #include "../tests/codegen-tests/codegen_func_calls.fm"
    };
    constexpr std::array codegen_structs {
        #include "../tests/codegen-tests/codegen_structs.fm"
    };
    // ------------------------------------------------------

    std::print("{}",   "------------------------------------------------\n");
    for (const auto& [test, expected] : codegen_structs) {
        auto [output, status] = exec(std::format("ASAN_OPTIONS=detect_leaks=0 ./build/fumo 2>&1 \"{}\"", test).c_str());
        if ((expected == fail && WEXITSTATUS(status)) 
         || (expected == pass && !WEXITSTATUS(status))) {
            std::print("-> \033[38;2;88;154;143m✓ OK\033[0m:\n"
                       "{}\n"
                       "{}"
                       "------------------------------------------------\n"
                       ,output
                       ,test
                       );
        } else {
            std::print("-> \033[38;2;235;67;54m❌FAILED\033[0m:\n"
                       "{}\n"
                       "{}"
                       "------------------------------------------------\n"
                       ,output
                       ,test
                       );
        }
    }
}
