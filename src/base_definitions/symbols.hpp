#pragma once
// clang-format off
#include "utils/map-macro.hpp"


#define punctuators                     \
    less,                               \
    greater,                            \
    exclamation,                        \
    plus,                               \
    minus,                              \
    asterisk,                           \
    ampersand,                          \
    negate,                             \
    percent,                            \
    hashtag,                            \
    hat,                                \
    bar,                                \
    division,                           \
    equals,                             \
    question_mark,                      \
    colon,                              \
    dot,                                \
    open_bracket,                       \
    close_bracket,                      \
    open_paren,                         \
    close_paren,                        \
    open_brace,                         \
    close_brace,                        \
    semicolon,                          \
    comma,                              \
    equals_equals,                      \
    greater_equals,                     \
    less_equals,                        \
    exclamation_equals,                 \
    plus_equals,                        \
    minus_equals,                       \
    asterisk_equals,                    \
    division_equals,                    \
    percent_equals,                     \
    ampersand_equals,                   \
    hat_equals,                         \
    bar_equals,                         \
    bar_bar,                            \
    minus_minus,                        \
    plus_plus,                          \
    hashtag_hashtag,                    \
    ampersand_ampersand,                \
    greater_greater,                    \
    less_less,                          \
    less_less_equals,                   \
    greater_greater_equals,             \
    dot_dot_dot,                        \
    minus_greater,                      \
    colon_colon,                        \
    quotation_mark
#define builtin_types                   \
    i8               ,                  \
    u8               ,                  \
    u16              ,                  \
    i16              ,                  \
    i32              ,                  \
    u32              ,                  \
    i64              ,                  \
    u64              ,                  \
    f32              ,                  \
    f64              ,                  \
    void             ,                  \
    Any              ,                  \
    bool
#define keywords                        \
    namespace        ,                  \
    let              ,                  \
    fn               ,                  \
    const            ,                  \
    volatile         ,                  \
    static           ,                  \
    extern           ,                  \
    continue         ,                  \
    else             ,                  \
    for              ,                  \
    if               ,                  \
    inline           ,                  \
    return           ,                  \
    sizeof           ,                  \
    switch           ,                  \
    default          ,                  \
    case             ,                  \
    break            ,                  \
    while            ,                  \
    enum             ,                  \
    true             ,                  \
    false            ,                  \
    struct           ,                  \
    null             ,                  \
    builtin_types
#define literals                        \
    identifier,                         \
    keyword,                            \
    builtin_type,                       \
    integer,                            \
    floating_point,                     \
    char_literal,                       \
    string

#define all_tokens punctuators, literals, is_EOF
//----------------------------------------------------------------
// for printing
#define symbol_reprs__  \
"<"  ,                  \
">"  ,                  \
"!"  ,                  \
"+"  ,                  \
"-"  ,                  \
"*"  ,                  \
"&"  ,                  \
"~"  ,                  \
"%"  ,                  \
"#"  ,                  \
"^"  ,                  \
"|"  ,                  \
"/"  ,                  \
"="  ,                  \
"?"  ,                  \
":"  ,                  \
"."  ,                  \
"["  ,                  \
"]"  ,                  \
"("  ,                  \
")"  ,                  \
"{"  ,                  \
"}"  ,                  \
";"  ,                  \
","  ,                  \
"==" ,                  \
">=" ,                  \
"<=" ,                  \
"!=" ,                  \
"+=" ,                  \
"-=" ,                  \
"*=" ,                  \
"/=" ,                  \
"%=" ,                  \
"&=" ,                  \
"^=" ,                  \
"|=" ,                  \
"||" ,                  \
"--" ,                  \
"++" ,                  \
"##" ,                  \
"&&" ,                  \
">>" ,                  \
"<<" ,                  \
"<<=",                  \
">>=",                  \
"...",                  \
"->" ,                  \
"::" ,                  \
"\"" 

#define add_parens(S) (S)
#define symbol_reprs_ map_macro(add_parens, symbol_reprs__)
#define punctuators_ map_macro(add_parens, punctuators)


//-----------------------------------------------------------------------------
// for switch case usage in lexer
enum struct Symbol {
// has singular
    question_mark   =     '?',
    colon           =     ':',
    open_bracket    =     '[',
    close_bracket   =     ']',
    open_paren      =     '(',
    close_paren     =     ')',
    open_brace      =     '{',
    close_brace     =     '}',
    semicolon       =     ';',
    comma           =     ',',
    negate          =     '~',
// has equals
    exclamation     =     '!',
    asterisk        =     '*',
    percent         =     '%',
    hat             =     '^',
// has double and equals
    bar             =     '|',
    ampersand       =     '&',
    plus            =     '+',
    minus           =     '-',
    equals          =     '=',
// has triple
    dot             =     '.',
    greater         =     '>',
    less            =     '<',
    division        =     '/',
    hashtag         =     '#',
// ignore
    space           =     ' ',
    tab             =     '\t',
    r_thing         =     '\r',
    alert           =     '\a',
    newline         =     '\n',
};

#define singular                    \
    question_mark   ,               \
    open_bracket    ,               \
    close_bracket   ,               \
    open_paren      ,               \
    close_paren     ,               \
    open_brace      ,               \
    close_brace     ,               \
    semicolon       ,               \
    negate          ,               \
    comma
#define has_equals                  \
    exclamation     ,               \
    asterisk        ,               \
    percent         ,               \
    hat
#define has_double_and_equals       \
    ampersand       ,               \
    bar,                            \
    plus            ,               \
    equals
#define has_triple                  \
    colon           ,               \
    minus           ,               \
    dot             ,               \
    greater         ,               \
    division        ,               \
    less            ,               \
    hashtag         ,               \
    newline
#define ignore                      \
    space           ,               \
    tab             ,               \
    r_thing         ,               \
    alert          

#define all_symbols singular, has_equals, has_double_and_equals, has_triple, ignore

