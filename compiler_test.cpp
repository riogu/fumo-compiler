#include <format>

// clang-format off

#define t std::tuple

auto main() -> int {
    // teststring + expected output (only for arithmetic for now)
    constexpr std::array tests {
        t("var = 69 + 21 + (3 + 3 = 2) * (-2 - 3 / ~3) + 3 + 30;\nepic = 213;", 0),
        t(" 123123 + ~213213* 3123;", 0),
        t("gamer = 69420;gamer = ~1231 + 21312 * 3213 / (1231230 + 2130 + 2 * 3 - 45 + 3123 + 10);\n",0),
        t("var = 69 + (3  + 2 = 3);", 0),
        t("-(-(-(-(-(-(-(-(-(1)))))))));", 0),
        t("(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-1))))))))))))))))))))))))))))))))))));",0),
        t("wow = 1 + epic - 3 * (2  + 69 - 2) / 3;", 0),
        t("x = 3 + 3 * 123 - (321 + 213); x;", 0),
        t("var = (-1) * 3- - 8 * - 1 +2;", 5)
    };
    for (const auto& [test, expected]: tests) 
        std::system(std::format("./build/c-compiler \"{}\"", test).c_str());
}
