#include <format>

// clang-format off

#define t std::tuple

auto main() -> int {
    // teststring + expected output (only for arithmetic for now)
    constexpr std::array tests {
        t("a = 10; b = 5; return a + 3;"),
    };
  [&tests]<std::size_t... i>(std::index_sequence<i...>) {
        (..., std::system(std::format("./build/c-compiler \"{}\"", std::get<0>(tests[i])).c_str()));
    }(std::make_index_sequence<tests.size()>());

}
