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
    // ------------------------------------------------------
    // these tests cover up to namespaces and basic struct syntax
    constexpr std::array ast_syntax {
        #include "../tests/part1-tests/ast_syntax.fm"
    };
    constexpr std::array postfix {
        #include "../tests/part1-tests/postfix.fm"
    };
    constexpr std::array scope_basic_checks {
        #include "../tests/part1-tests/scope_basic_checks.fm"
    };
    constexpr std::array scope_name_lookup {
        #include "../tests/part1-tests/scope_name_lookup.fm"
    };
    // ------------------------------------------------------
    // tests for codegen
    constexpr std::array simple_llvm_codegen {
        #include "../tests/part2-tests/simple_llvm_codegen.fm"
    };
    constexpr std::array codegen_func_calls {
        #include "../tests/part2-tests/codegen_func_calls.fm"
    };
    // ------------------------------------------------------
    
    constexpr std::array all_tests {
        #include "../tests/part1-tests/ast_syntax.fm"
        #include "../tests/part1-tests/postfix.fm"
        #include "../tests/part1-tests/scope_basic_checks.fm"
        #include "../tests/part1-tests/scope_name_lookup.fm"
    };

    // NOTE: remember to remove fsanitize=address later from the cmakelists.txt, 
    // else llvm will make false positive leaks
    std::print("{}",   "------------------------------------------------\n");
    for (const auto& [test, expected] : simple_llvm_codegen) {
        auto [output, status] = exec(std::format("ASAN_OPTIONS=detect_leaks=0 ./build/fumo 2>&1 \"{}\"", test).c_str());
        if ((expected == fail && WEXITSTATUS(status)) 
         || (expected == pass && !WEXITSTATUS(status))) {
            std::print("-> \033[38;2;88;154;143m✓ OK\033[0m:\n"
                       // "{}"
                       "------------------------------------------------\n"
                       ,output
                       );
        } else {
            std::print("-> \033[38;2;235;67;54m❌FAILED\033[0m:\n"
                       "{}"
                       "------------------------------------------------\n"
                       ,output
                       );
        }
    }
}
