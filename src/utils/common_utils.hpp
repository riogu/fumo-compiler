#pragma once
#include <string>
#include <optional>
#include <filesystem>
#include <iostream>
#include <vector>

#include <expected>
template<typename Ok, typename Err>
using Result = std::expected<Ok, Err>;
template<typename T>
using Err = std::unexpected<T>;
//
namespace fs = std::filesystem; 
using str = std::string;
using str_view = std::string_view;
template<typename  T> using vec = std::vector<T>;
template<typename T> using Opt = std::optional<T>;
template<typename T>
using unique_ptr = std::unique_ptr<T>;

using i64 = int64_t;
inline constexpr std::nullopt_t None = std::nullopt;

struct File {
    fs::path path_name;
    std::string contents;
};

#define internal_panic(fmt, ...) PANIC(std::format("internal fumo error: " fmt "\n[INFO] please report this error to https://github.com/riogu/fumo-compiler/issues with this crash."\
                                                   __VA_OPT__(, __VA_ARGS__)))
#define internal_error(tok, ...)                                                                \
do {                                                                                            \
    file_stream.seekg(file_stream.beg);                                                         \
    std::string line;                                                                           \
    if (tok.line_number != 1) {                                                                 \
        file_stream.seekg(tok.file_offset, std::ios_base::beg);                                 \
        while(file_stream.peek() != '\n' && file_stream.peek() != EOF) {                        \
            long pos = file_stream.tellg();                                                     \
            file_stream.seekg(pos-1);                                                           \
        }                                                                                       \
        file_stream.get();                                                                      \
    }                                                                                           \
    std::getline(file_stream, line);                                                            \
                                                                                                \
    std::cerr << std::format("-> \033[38;2;235;67;54m[error]\033[0m in file '{}' at line {}:\n" \
                             "   | {}\n"                                                        \
                             "   |{}{}\n\n",                                                    \
                             tok.file_name,                                                     \
                             tok.line_number,                                                   \
                             line,                                                              \
                             std::string(tok.line_offset, ' ') + "^ ",                          \
                             std::format(__VA_ARGS__));                                         \
    internal_panic("reported above.");                                                          \
} while (false)

// print nice errors
#define report_error(tok, ...)                                                                  \
do {                                                                                            \
    file_stream.seekg(file_stream.beg);                                                         \
    std::string line;                                                                           \
    if (tok.line_number != 1) {                                                                 \
        file_stream.seekg(tok.file_offset, std::ios_base::beg);                                 \
        while(file_stream.peek() != '\n' && file_stream.peek() != EOF) {                        \
            long pos = file_stream.tellg();                                                     \
            file_stream.seekg(pos-1);                                                           \
        }                                                                                       \
        file_stream.get();                                                                      \
    }                                                                                           \
    std::getline(file_stream, line);                                                            \
                                                                                                \
    std::cerr << std::format("-> \033[38;2;235;67;54m[error]\033[0m in file '{}' at line {}:\n" \
                             "   | {}\n"                                                        \
                             "   |{}{}\n\n",                                                    \
                             tok.file_name,                                                     \
                             tok.line_number,                                                   \
                             line,                                                              \
                             std::string(tok.line_offset, ' ') + "^ ",                          \
                             std::format(__VA_ARGS__));                                         \
    std::exit(1);                                                                               \
} while (false)

#define make_runtime_error(tok, ...)                                                            \
({                                                                                              \
    file_stream.seekg(file_stream.beg);                                                         \
    std::string line;                                                                           \
    if (tok.line_number != 1) {                                                                 \
        file_stream.seekg(tok.file_offset, std::ios_base::beg);                                 \
        while(file_stream.peek() != '\n' && file_stream.peek() != EOF) {                        \
            long pos = file_stream.tellg();                                                     \
            file_stream.seekg(pos-1);                                                           \
        }                                                                                       \
        file_stream.get();                                                                      \
    }                                                                                           \
    std::getline(file_stream, line);                                                            \
                                                                                                \
    str output = std::format("-> \033[38;2;235;67;54m[runtime error]\033[0m in file '{}' at line {}:\n" \
                             "   | {}\n"                                                        \
                             "   |{}{}\n\n",                                                    \
                             tok.file_name,                                                     \
                             tok.line_number,                                                   \
                             line,                                                              \
                             std::string(tok.line_offset, ' ') + "^ ",                          \
                             std::format(__VA_ARGS__));                                         \
    output;                                                                                     \
})
