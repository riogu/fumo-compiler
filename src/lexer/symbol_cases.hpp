#pragma once
// clang-format off
#define cases(symbols) map_macro(symbols##_case, symbols)

#define singular_case(v_)                                           \
case static_cast<int>(Symbol::v_): {                                \
        tokens.push_back({.type = TokenType::v_,                    \
                          add_token_info});                         \
        break;                                                      \
}
#define has_equals_case(v_)                                         \
case (int)Symbol::v_: {                                             \
    tokens.push_back(                                               \
        file_stream.peek() == '='                                   \
            ? Token {.type = TokenType::v_##_equals,                \
                          add_token_info}                           \
            : Token {.type = TokenType::v_,                         \
                          add_token_info});                         \
        break;                                                      \
}
#define has_double_and_equals_case(v_)
#define has_triple_case(v_)
#define ignore_case(v_)                                             \
case static_cast<int>(Symbol::v_): {                                \
    break;                                                          \
}

#define add_token_info                  \
.value = std::nullopt,                  \
.line_number = __FUMO_LINE_NUM__,       \
.line_offset = __FUMO_LINE_OFFSET__

