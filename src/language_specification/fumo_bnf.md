Current structure for the AST parser in BNF format
- Top level

<translation-unit> ::= <declaration>
                     | <statement>

- Statements

<statement> ::= <jump-statement>
              | <expression-statement>

<jump-statement> ::= "return" {<expression>}? ";"
                   | "continue" ";"
                   | "break" ";"

<expression-statement> = <expression> ";"

---
---
---
- Declarations

<declaration> ::= "let"         <variable-declaration>  ";"
                | "fn"          <function-declaration>  ";"
                | "struct"      <struct-declaration>    ";"
                | "namespace"   <namespace-declaration> ";"
                | "enum"        <enum-declaration>      ";"

---

<struct-declaration>      ::= <named-scope-declaration>
<namespace-declaration>   ::= <named-scope-declaration>
<enum-declaration>        ::= [TODO]

<named-scope-declaration> ::= <identifier> "{" {<declaration>}* "}"

<variable-declaration>    ::= <declarator-list> {":"}? {<declaration-specifier>}+ {"=" <initializer>}?
<function-declaration>    ::=  <declarator> "(" {<parameter-list>}? ")" "->" {<declaration-specifier>}+ {<compound-statement>}?

---
<compound-statement> ::= { {<declaration>}* {<statement>}* {<compound-statement>}* }

<declarator> ::= <identifier>
               | <declarator> "(" {<parameter-list>}? ")"
               | <declarator> "\[" {<constant-expression>}? "]"

<declaration-specifier> ::= {<type-qualifier> | <type-specifier>}+ {<pointer>}*

<declarator-list> ::= <declarator>
                    | <declarator> "," <declarator-list>


<parameter-list> ::= <parameter>
                   | <parameter-list> "," <parameter> 

<parameter> ::= <declarator-specifier> <identifier> 

---
---
---
- Expressions

<expression> ::= <assignment> 

<assignment> ::= <equality> | {"=" <initializer>}?

<initializer> ::= "{" <initializer-list> "}"
                | <equality>

<initializer-list> ::= <initializer> {","}?
                     | <initializer> , <initializer-list>

<equality> ::= <relational> {"==" <relational> | "!=" <relational> }*

<relational> ::= <add> { <relational-op>  <add> }*

<add> ::=  <multiplication> { "+" <multiplication> | "-" <multiplication> }*

<multiplication> ::=  <unary> { \"*" <unary> |"/" <unary> }*

<unary> ::= <unary-op> <unary> 
          | <primary>

<primary> ::= "(" <equality> ")"
            | <identifier> 
            | <literal>


---
---
---
- Misc definitions
<pointer> ::= " * "
<unary-op> ::= - 
             | ! 
             | ~ 
             | +
<relational-op> ::= < 
                  | > 
                  | <=
                  | >=
<type-specifier> ::= void
                   | u8
                   | i32
                   | i64
                   | f32
                   | f64
                   | str
                   | bool
                   | <struct-specifier>
                   | <enum-specifier>
                   | <typedef-name>
<type-qualifier> ::= const
                   | volatile
                   | static
                   | extern

