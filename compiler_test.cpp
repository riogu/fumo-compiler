#include <format>
#include <memory>
#include <print>
#include <string>

// clang-format off
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

#define t std::pair

auto main() -> int {
    // teststring + expected output (only for arithmetic for now)
    constexpr std::array tests {
        // t("auto gamer = 123123;", ""),
        // t("var = 69 + 21 + (3 + 3 = 2) * (-2 - 3 / ~3) + 3 + 30;\nepic = 213;", ""),
        // t("123123 + ~213213* 3123;", "69"),
        // t("gamer = 69420;gamer = ~1231 + 21312 * 3213 / (1231230 + 2130 + 2 * 3 - 45 + 3123 + 10);","0"),
        // t("-(-(-(-(-(-(-(-(-(1)))))))));", "0"),
        // t("var = 69 + (3  + 2 = 3);", ""),
        // t("(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-1))))))))))))))))))))))))))))))))))));","0"),
        // t("wow = 1 + epic - 3 * (2  + 69 - 2) / 3;", "0"),
        // t("x = 3 + 3 * 123 - (321 + 213); x;", "0"),
        // t("let var: int = (-1) * 3- - 8 * - 1 +2;", "69")
        t(
            "let var: let = {69213 * 2 , {2, {323213, 123123}}};\n"
          // "fn somevar();"
          "var = {1232};\n"
          "let var: i32 = 1232;"
          ,"")
    };
    std::print("{}","\n------------------------------------------------\n");
    for (const auto& [test, expected] : tests) {
        auto output = exec(std::format("./build/fumo-compiler \"{}\"", test).c_str());
        if (output != expected) {
            std::print("> \033[38;2;235;67;54m❌FAILED\033[0m:\n"
                       "{}\n"
                       "\nEXPECTED:\n"
                       "{}\n"
                       "\nGOT:\n"
                       "{}"
                       "\n------------------------------------------------\n"
                       ,test, expected, output);
        }
        else {
            std::print("> \033[38;2;88;154;143m✓ OK\033[0m:\n"
                       "{}"
                       "\n------------------------------------------------\n"
                       , test);
        }
    }

}
