#include <cstdlib>
#include <format>
#include <print>
#include <string>

// clang-format off
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
constexpr std::string fail = "fail";
constexpr std::string pass = "pass";
auto main() -> int {
    // teststring + expected output (only for arithmetic for now)
    constexpr std::array tests {
        t("let var = 69 + 21 + (3 + 3 = 2) * (-2 - 3 / ~3) + 3 + 30;\nlet epic = 213;", "fail"),
        t("123123 + ~213213* 3123;", "pass"),
        t("let gamer = 69420;let gamer = ~1231 + 21312 * 3213 / (1231230 + 2130 + 2 * 3 - 45 + 3123 + 10);","pass"),
        t("(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-1))))))))))))))))))))))))))))))))))));","pass"),
        t("let var: int = (-1) * 3- - 8 * - 1 +2;", "pass"),
        t("let var: i64 = {69213 * 2 , {2, {323213, 123123}}};\n"
          "fn func_name(a: i32, b: f64) -> const i32* {}\n"
          "fn another_f() -> const i32****;\n"
          "var = {1232};\n"
          "let var;"
          "let var: i32 = 1232;"
          ,"pass"),
        t("struct gaming;", 
          "fail")
    };
    std::print("{}","\n------------------------------------------------\n");
    for (const auto& [test, expected] : tests) {
        auto [output, status] = exec(std::format("./build/fumo-compiler \"{}\"", test).c_str());
        if ((expected == fail && WEXITSTATUS(status)) 
         || (expected == pass && !WEXITSTATUS(status))) {
            std::print("> \033[38;2;88;154;143m✓ OK\033[0m:\n"
                       "{}"
                       "\n------------------------------------------------\n"
                       ,test);
        } else {
            std::print("> \033[38;2;235;67;54m❌FAILED\033[0m:\n"
                       "{}\n"
                       "\n------------------------------------------------\n"
                       ,test);
        }
    }

}
