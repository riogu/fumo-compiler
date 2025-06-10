#pragma once
// clang-format off
#define cases(symbols) map_macro(symbols##_case, symbols)

#define singular_case(tkn)                                              \
case (int)(Symbol::tkn): {                                              \
        tokens.push_back({.type = TokenType::tkn, add_token_info});     \
        break;                                                          \
}
#define has_equals_case(tkn)                                            \
case (int)Symbol::tkn: {                                                \
    tokens.push_back(                                                   \
        file_stream.peek() == '='                                       \
            ? Token {.type = TokenType::tkn##_equals,add_token_info}    \
            : Token {.type = TokenType::tkn, add_token_info});          \
        break;                                                          \
}
#define has_double_and_equals_case(tkn)                                 \
case (int)Symbol::tkn: {                                                \
    tokens.push_back(                                                   \
        file_stream.peek() == (int)Symbol::tkn                          \
            ? ({get_curr();                                             \
                Token {.type = TokenType::tkn##_##tkn,add_token_info};})\
            : file_stream.peek() == '=' ?                               \
              ({get_curr();                                             \
              Token {.type =TokenType::tkn##_equals,add_token_info};})  \
            : Token {.type =TokenType::tkn, add_token_info});           \
    break;                                                              \
}


#define has_triple_case(tkn) // TODO:
#define ignore_case(tkn)                                            \
case static_cast<int>(Symbol::tkn): {                               \
    break;                                                          \
}

#define add_token_info                  \
.value = std::nullopt,                  \
.line_number = __FUMO_LINE_NUM__,       \
.line_offset = __FUMO_LINE_OFFSET__

