# Complete Fumo Language BNF Grammar

## Top Level
```bnf
<translation-unit> ::= <declaration>*

<declaration> ::= <variable-declaration>
                | <function-declaration>  
                | <struct-declaration>
                | <namespace-declaration>
```

## Statements
```bnf
<statement> ::= <expression-statement>
              | <if-statement>
              | <while-statement>
              | <jump-statement>
              | <compound-statement>
              | <variable-declaration>

<compound-statement> ::= "{" <statement>* "}"

<if-statement> ::= "if" "("? <expression> ")"? <compound-statement> 
                   ( "else" ( <if-statement> | <compound-statement> ) )?

<while-statement> ::= "while" <expression> <compound-statement>

<jump-statement> ::= "return" <expression>? ";"
                   | "continue" ";"
                   | "break" ";"

<expression-statement> ::= <expression> ";"
```

## Declarations
```bnf
<variable-declaration> ::= "let" <identifier> ( ":" <type-specifier> )? ( "=" <initializer> )? ";"

<function-declaration> ::= "fn" ( "static" )? <identifier> "(" <parameter-list>? ")" "->" <type-specifier> <compound-statement>?

<struct-declaration> ::= "struct" <identifier> "{" <struct-member>* "}"

<namespace-declaration> ::= "namespace" <identifier> "{" <declaration>* "}"

<struct-member> ::= <variable-declaration>
                  | <function-declaration>

<parameter-list> ::= <parameter> ( "," <parameter> )*

<parameter> ::= <identifier> ":" <type-specifier>
```

## Types
```bnf
<type-specifier> ::= <primitive-type>
                   | <pointer-type>
                   | <qualified-type>

<primitive-type> ::= "void" | "i8" | "i16" | "i32" | "i64" | "u8" | "u16" | "u32" | "u64" 
                   | "f32" | "f64" | "bool" | "char"

<pointer-type> ::= <type-specifier> "*"

<qualified-type> ::= <qualified-identifier>

<type-qualifier> ::= "const"
```

## Expressions
```bnf
<expression> ::= <assignment-expression>

<assignment-expression> ::= <logical-or-expression>
                          | <logical-or-expression> "=" <assignment-expression>

<logical-or-expression> ::= <logical-and-expression> 
                          | <logical-or-expression> "||" <logical-and-expression>

<logical-and-expression> ::= <equality-expression>
                           | <logical-and-expression> "&&" <equality-expression>

<equality-expression> ::= <relational-expression>
                        | <equality-expression> "==" <relational-expression>
                        | <equality-expression> "!=" <relational-expression>

<relational-expression> ::= <additive-expression>
                          | <relational-expression> "<" <additive-expression>
                          | <relational-expression> ">" <additive-expression>
                          | <relational-expression> "<=" <additive-expression>
                          | <relational-expression> ">=" <additive-expression>

<additive-expression> ::= <multiplicative-expression>
                        | <additive-expression> "+" <multiplicative-expression>
                        | <additive-expression> "-" <multiplicative-expression>

<multiplicative-expression> ::= <unary-expression>
                              | <multiplicative-expression> "*" <unary-expression>
                              | <multiplicative-expression> "/" <unary-expression>
                              | <multiplicative-expression> "%" <unary-expression>

<unary-expression> ::= <postfix-expression>
                     | <unary-operator> <unary-expression>

<unary-operator> ::= "&" | "*" | "+" | "-" | "!" | "~"

<postfix-expression> ::= <primary-expression>
                       | <postfix-expression> "(" <argument-list>? ")"
                       | <postfix-expression> "." <identifier>
                       | <postfix-expression> "->" <identifier>

<primary-expression> ::= <identifier>
                       | <qualified-identifier>
                       | <literal>
                       | "(" <expression> ")"
                       | <initializer-list>

<argument-list> ::= <assignment-expression> ( "," <assignment-expression> )*
```

## Initializers
```bnf
<initializer> ::= <assignment-expression>
                | <initializer-list>

<initializer-list> ::= <qualified-identifier> "{" <initializer-element-list>? "}"

<initializer-element-list> ::= <initializer> ( "," <initializer> )* ","?
```

## Identifiers and Literals
```bnf
<qualified-identifier> ::= <identifier> ( "::" <identifier> )*

<identifier> ::= [a-zA-Z_][a-zA-Z0-9_]*

<literal> ::= <integer-literal>
            | <floating-literal>
            | <character-literal>
            | <string-literal>
            | <boolean-literal>
            | <null-literal>

<integer-literal> ::= [0-9]+

<floating-literal> ::= [0-9]+ "." [0-9]+

<character-literal> ::= "'" <character> "'"

<string-literal> ::= "\"" <string-character>* "\""

<boolean-literal> ::= "true" | "false"

<null-literal> ::= "null"
```
