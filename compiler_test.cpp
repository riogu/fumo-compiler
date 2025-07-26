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
constexpr std::string fail = "fail", pass = "pass";
auto main() -> int {
    // teststring + expected output (only for arithmetic for now)
    constexpr std::array ast_syntax_tests {
        t("let var = 69 + 21 + (3 + 3 = 2) * (-2 - 3 / ~3) + 3 + 30;\nlet epic = 213;", "fail"),
        t("123123 + ~213213* 3123;", "pass"),
        t("let gamer = 69420;let gamer = ~1231 + 21312 * 3213 / (1231230 + 2130 + 2 * 3 - 45 + 3123 + 10);","pass"),
        t("(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-1))))))))))))))))))))))))))))))))))));","pass"),
        t("let var: i64 = {69213 * 2 , {2, {323213, 123123}}};\n", "pass"),
        t("fn func_name(a: i32, b: struct gaming{}) -> const i32* {}", "fail"),
        t("fn another_f() -> const i32****;\n"
          "fn func_name(a: i32, b: f64) -> const i32* {\n"
          "    let var: i32 = 213123;\n"
          "    {\n"
          "       let x: MyStruct;\n"
          "       x = {123 + 20};\n"
          "    }\n"
          "    var = 213;\n"
          "}\n"
          // "fn main() -> i32 {\n    let var = 1;\n    return 0;\n}\n"
          ,"pass"),
        t("let var;", "fail"),
        // t("fn main() -> i32 {\n    let var: i32 = (-1) * ~3 - -8 * -1 + 2;\n}", "pass"),
        // t(" 1 * 3 - 8 * 1 + 69;","pass"),
        // t("~1 + !0.0f;~-0; 1 + !(~3 - 4 * 3 + 9);","pass"),
    };
    constexpr std::array llvm_tests {
        // t("let x: cool = 12321; let y: i32; let x: f64;\n"
        //   "x = 12; fn func() -> void {}"
        //   , "pass"),
        // t("let x: i32 = 123123;                                     \n"
        //   "fn another_func(x: i32) -> void;                         \n"
        //   "fn func_name(a: i32, b: f64) -> const i32* {             \n"
        //   "    x = 69420;                                           \n"
        //   "    a = 69420;                                           \n"
        //   "    let x = 1111111;                                     \n"
        //   "    {                                                    \n"
        //   "       let x: f64;                                       \n"
        //   "       x = 12.0f;                                        \n"
        //   "    }                                                    \n"
        //   "    x = 213;                                             \n"
        //   "}                                                        \n"
        //     ,"pass"),
        // t("let x: i32 = 123123;                                     \n"
        //   "fn another_func(x: i32) -> void;                         \n"
        //   "fn func_name(a: i32, b: f64) -> const i32* {             \n"
        //   "    x = 69420;                                           \n"
        //   "    a = 69420;                                           \n"
        //   "    let x = 1111111;                                     \n"
        //   "    {                                                    \n"
        //   "       let x: f64;                                       \n"
        //   "       x = 12.0f;                                        \n"
        //   "    }                                                    \n"
        //   "    somevar = 69420;                                     \n"
        //   "    x = 213;                                             \n"
        //   "}                                                        \n"
        //     ,"pass"),
        t("fn func_name(a: i32, b: f64) -> const i32* {                 \n"
          "    let x:i32 = 1111111;                                     \n"
          "    let x:i32 = 1111;                                        \n"
          "}                                                            \n"
            ,"fail"),
    };

    std::print("{}",   "\n------------------------------------------------\n");
    for (const auto& [test, expected] : llvm_tests) {
        auto [output, status] = exec(std::format("./build/fumo-compiler \"{}\"", test).c_str());
        if ((expected == fail && WEXITSTATUS(status)) 
         || (expected == pass && !WEXITSTATUS(status))) {
            std::print("> \033[38;2;88;154;143m✓ OK\033[0m:\n"
                       "{}"
                       "\n------------------------------------------------\n"
                       ,test);
        } else {
            std::print("> \033[38;2;235;67;54m❌FAILED\033[0m:\n"
                       "{}"
                       "\n------------------------------------------------\n"
                       ,test);
        }
    }
}
