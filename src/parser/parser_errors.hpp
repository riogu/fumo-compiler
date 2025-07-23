#pragma once
#include <iostream>
// print nice errors
#define report_error(tok, ...)                                                          \
{                                                                                       \
    file_stream.seekg(file_stream.beg);                                                 \
    std::string line;                                                                   \
    if (tok.line_number != 1) {                                                         \
        file_stream.seekg(tok.file_offset, std::ios_base::beg);                         \
        while(file_stream.peek() != '\n') {                                             \
            long pos = file_stream.tellg();                                             \
            file_stream.seekg(pos-1);                                                   \
        }                                                                               \
        file_stream.get();                                                              \
    }                                                                                   \
    std::getline(file_stream, line);                                                    \
                                                                                        \
    std::cerr << std::format("\n  | error in file '{}' at line {}:\n  | {}\n  |{}{}\n", \
                             tok.file_name,                                             \
                             tok.line_number,                                           \
                             line,                                                      \
                             std::string(tok.line_offset, ' ') + "^ ",                  \
                             std::format(__VA_ARGS__));                                 \
    std::abort();                                                                       \
}
