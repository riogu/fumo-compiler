#include "lexer.hpp"

#define symbol_case(v_) case static_cast<int>(Symbol::v_): return true;

[[nodiscard]] bool Lexer::identifier_ended() {
    switch (file_stream.peek()) {
        map_macro(symbol_case, all_symbols);
        default:
            return false;
    }
}

#define check_keyword(v_) if(identifier == #v_) return true;

[[nodiscard]] bool Lexer::is_keyword(const Str identifier) {
    map_macro(check_keyword, keywords);
    return false;
}

#undef symbol_case
#undef check_keyword

[[nodiscard]] Str Lexer::peek_line() {
    std::string line;
    int len = file_stream.tellg();
    std::getline(file_stream, line);
    file_stream.seekg(len, std::ios_base::beg);
    return line;
}

i64 Lexer::get_curr() {
    __FUMO_LINE_OFFSET__++;
    curr = file_stream.get();
    return curr;
}
