#include <cstdlib>
#include <format>
#include <string>
#include <vector>

int main() {
    constexpr std::array tests {
        "var = 69 + 21 + (3 + 3 = 2) * (-2 - 3 / ~3) + 3 + 30;\nepic = 213;",
        // FIXME:               ^ fix this in the analysis step
        " 123123 + ~213213* 3123;",
        "gamer = 69420; gamer = ~1231 + 21312 * 3213 / (1231230 + 2130 + 2 * 3 - 45 + 3123 + 10);\n",
        "var = 69 + (3  + 2 = 3);",
        "-(-(-(-(-(-(-(-(-(1)))))))));",
        "(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-(-1))))))))))))))))))))))))))))))))))));",
    };

    for (const auto& test : tests)
        std::system(std::format("./build/c-compiler \"{}\"", test).c_str());
}
