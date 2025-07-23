#include <string>
#include <optional>
#include <filesystem>
#include <vector>
// #include <expected>

using str = std::string;
// template<typename Ok, typename Err>
// using Result = std::expected<Ok, Err>;
// template<typename T>
// using Err = std::unexpected<T>;
//
namespace fs = std::filesystem; 

template<typename  T> using vec = std::vector<T>;
template<typename T> using Opt = std::optional<T>;
using i64 = int64_t;

#define INTERNAL_PANIC(fmt, ...) PANIC(std::format("fumo internal error: " fmt, __VA_ARGS__))
