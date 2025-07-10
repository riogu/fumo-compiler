#include <format>

// clang-format off

#define t std::tuple

auto main() -> int {
    // teststring + expected output (only for arithmetic for now)
    constexpr std::array tests {
        // t("var = 69 + 21 + (3 + 3 = 2) * (-2 - 3 / ~3) + 3 + 30;\nepic = 213;", 0),
        // t(" 123123 + ~213213* 3123;", 0),
        // t("gamer = 69420;gamer = ~1231 + 21312 * 3213 / (1231230 + 2130 + 2 * 3 - 45 + 3123 + 10);\n",0),
        // t("var = 69 + (3  + 2 = 3);", 0),
        // t("-(-(-(-(-(-(-(-(-(1)))))))));", 0),
        // t("(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-1))))))))))))))))))))))))))))))))))));",0),
        // t("wow = 1 + epic - 3 * (2  + 69 - 2) / 3;", 0),
        t("3 + 3 * 123 - (1 + 2);"),
    };

  [&tests]<std::size_t... i>(std::index_sequence<i...>) {
        (..., std::system(std::format("./build/c-compiler \"{}\"", std::get<0>(tests[i])).c_str()));
    }(std::make_index_sequence<tests.size()>());

}
