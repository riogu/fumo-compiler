#include <cstdlib>
#include <format>
#include <fstream>
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
    constexpr bool fail = false,
                   pass = true;

int main() {

    constexpr std::array ast_syntax {
        #include "../tests/ast_syntax.fm"
    };
    constexpr std::array postfix {
        #include "../tests/postfix.fm"
    };
    constexpr std::array scope_basic_checks {
        #include "../tests/scope_basic_checks.fm"
    };
    constexpr std::array scope_name_lookup {
        #include "../tests/scope_name_lookup.fm"
    };
    constexpr std::array all_tests {
        #include "../tests/ast_syntax.fm"
        #include "../tests/postfix.fm"
        #include "../tests/scope_basic_checks.fm"
        #include "../tests/scope_name_lookup.fm"
    };
    std::print("{}",   "------------------------------------------------\n");
    for (const auto& [test, expected] : scope_name_lookup) {
        auto [output, status] = exec(std::format("./build/fumo-compiler 2>&1 \"{}\"", test).c_str());
        if ((expected == fail && WEXITSTATUS(status)) 
         || (expected == pass && !WEXITSTATUS(status))) {
            std::print("-> \033[38;2;88;154;143m✓ OK\033[0m:\n"
                       "{}\n"
                       "{}"
                       "------------------------------------------------\n"
                       ,test
                       ,output
                       );
        } else {
            std::print("-> \033[38;2;235;67;54m❌FAILED\033[0m:\n"
                       "{}\n"
                       "{}"
                       "------------------------------------------------\n"
                       ,test
                       ,output
                       );
        }
    }
}
