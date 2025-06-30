
Current structure for the AST parser in BNF format

<program> ::= <function>

---
- Statements


<compound-statement> ::= 
<expression> ::= 

---
- Misc definitions
<standard-types> ::= void | char | int | long | etc... (dont need to type these here)
<type-specifier> ::= <standard-types> 
                   | <struct-specifier>
                   | <enum-specifier>

<pointer> ::= " * " {<type-qualifier>}* {<pointer>}?

NOTE: haven't added "..." yet
<parameter-list> ::= <parameter>
                   | <paremeter-list> "," <parameter>

---
- Declarations

-> A declaration is used to introduce identifiers into the program and specify their meaning/properties

<declaration> ::= <variable-declaration> ";"
                | <function-declaration> ";"

<variable-declaration> ::= {<declaration-specifier>}+ <declarator-list> {"=" <initializer>}?

<function-declaration> ::= {<declaration-specifier>}+ <ptr-and-declarator> "(" {<parameter-list>}? ")"
<function-definition> ::= <function-declaration> <compound-statement>


<declarator> ::= <identifier>
               | <declarator> \[ {<constant-expression>}? \]
               | <declarator> "(" {<parameter-list>}? ")"

<declaration-specifier> ::= <type-qualifier> 
                          | <type-specifier>
                          | <storage-specifier>

<ptr-and-declarator> ::= {<pointer>}? <declarator> 

<declarator-list> ::= <ptr-and-declarator>
                    | <ptr-and-declarator>, <declarator-list>


++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```c
int a = 1, *p = NULL, f(void), (*pf)(double);
// the type specifier is "int"

// declarator "a" defines an object of type int
//   initializer "=1" provides its initial value

// declarator "*p" defines an object of type pointer to int
//   initializer "=NULL" provides its initial value

// declarator "f(void)" declares a function
//   taking void and returning int

// declarator "(*pf)(double)" defines an object of type pointer to function 
//   taking double and returning int

struct C
{
    int member; // "int" is the type specifier
                // "member" is the declarator
} obj, *pObj = &obj;
// "struct C { int member; }" is the type specifier

// declarator "obj" defines an object of type struct C

// declarator "*pObj" declares a pointer to C,

// initializer "= &obj" provides the initial value for that pointer
```
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

---
- Parameters 

-> a parameter is considered a declaration

<parameter-list> ::= <parameter>
                   | <parameter-list> "," <parameter> 
<parameter> ::= <type-specifier> <id> 

