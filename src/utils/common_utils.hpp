#pragma once
#include <string>
#include <optional>
#include <filesystem>
#include <iostream>
#include <vector>

// #include <expected>
// template<typename Ok, typename Err>
// using Result = std::expected<Ok, Err>;
// template<typename T>
// using Err = std::unexpected<T>;
//
namespace fs = std::filesystem; 
using str = std::string;
template<typename  T> using vec = std::vector<T>;
template<typename T> using Opt = std::optional<T>;
template<typename T>
using unique_ptr = std::unique_ptr<T>;

using i64 = int64_t;

struct File {
    fs::path path_name;
    std::string contents;
};

#define INTERNAL_PANIC(fmt, ...) PANIC(std::format("internal fumo error: " fmt __VA_OPT__(, __VA_ARGS__)))

// print nice errors
#define report_error(tok, ...)                                                                  \
{                                                                                               \
    file_stream.seekg(file_stream.beg);                                                         \
    std::string line;                                                                           \
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
}

// NOTE: this is for resetting the stream in case you want many errors
/*if (tok.line_number != 1) {                                                                 \*/
/*    file_stream.seekg(tok.file_offset, std::ios_base::beg);                                 \*/
/*    while(file_stream.peek() != '\n') {                                                     \*/
/*        long pos = file_stream.tellg();                                                     \*/
/*        file_stream.seekg(pos-1);                                                           \*/
/*    }                                                                                       \*/
/*    file_stream.get();                                                                      \*/
/*}                                                                                           \*/
