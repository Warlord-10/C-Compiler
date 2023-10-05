
/*
<program> ::= { <function> }

<function> ::= "int" <id> "(" [ "int" <id> { "," "int" <id> } ] ")" [ "{" { <block-item> } "}" ] | ";" 

<block-item> ::= <statement> 
            | <declaration>

<declaration> ::= "int" <id> [ = <exp> ] ";"

<statement> ::= "return" <exp> ";"
            | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
            | "{" { <block-item> } "}
            | "for" "(" [exp] ";" [exp] ";" [exp] ")" <statement>
            | "for" "(" <declaration> [exp] ";" [exp] ")" <statement>
            | "while" "(" <exp> ")" <statement>
            | "do" <statement> "while" "(" <exp> ")" ";"
            | "break" ";"
            | "continue" ";"
            | <exp> ";"

<exp> ::= <id> "=" <exp> 
            | <conditional-exp>
            
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }

<factor> ::= <function-call> 
            | "(" <exp> ")" 
            | <unary_op> <factor> 
            | <int> 
            | <id>

<function-call> ::= id "(" [ <exp> { "," <exp> } ] ")"
<unary_op> ::= "!"
            | "-" 


General Rule:
    []: optional
    {}: 0 or more no. of times 
*/




#ifndef Parser
#define Parser

void Advance();
void Advance_print();
Token* Seek();
void Move_on();
void Error(char error_desc[],char error_type[]);



AST_Node* Factor();
AST_Node* Term();
AST_Node* Additive_Exp();
AST_Node* Relational_Exp();
AST_Node* Equality_Exp();
AST_Node* Logical_AndExp();
AST_Node* Logical_OrExp();
AST_Node* Conditional_Exp();
AST_Node* Expression();
AST_Node* Statement();
AST_Node* Declaration();
AST_Node* Block();
AST_Node* Function();

void PARSER();

#endif

/*

<program> ::= <function>

<function> ::= "int" <id> "(" [ <param-list> ] ")" "{" { <block-item> } "}"

<param-list> ::= <param> { "," <param> }
<param> ::= "int" <id>

<block-item> ::= <statement> 
            | <declaration>

<declaration> ::= "int" <id> [ "=" <exp> ] ";"
                | "int" <id> "[" <int> "]" [ = "{" { <exp> } "}" ] ";"

<statement> ::= "return" <exp> ";"
            | <exp> ";"
            | "if" "(" <exp> ")" <statement> [ "else" <statement> ]

<exp> ::= <id> "=" <exp> 
            | <conditional-exp>
            | <function-call>

<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }

<factor> ::= "(" <exp> ")" 
            | <unary_op> <factor> 
            | <int> 
            | <id>
            | <array-access>
            | <array>

<array> ::= "{" { <exp> } "}"

<array-access> ::= <id> "[" <exp> "]"

<function-call> ::= <id> "(" [ <arg-list> ] ")"
<arg-list> ::= <exp> { "," <exp> }
<unary_op> ::= "!"

*/





/*
OFFICIAL C GRAMMAR:
<program> ::= <external-declaration> 
                | <program> <external-declaration>

<external-declaration> ::= <function-definition> 
                | <declaration>

<function-definition> ::= <type-specifier> <declarator> <declaration-list> <compound-statement>

<type-specifier> ::= "void" 
                | "char" 
                | "short" 
                | "int" 
                | "long" 
                | "float" 
                | "double" 
                | <struct-or-union-specifier> 
                | <enum-specifier> 
                | <typedef-name>

<struct-or-union-specifier> ::= <struct-or-union> <identifier> "{" <struct-declaration-list> "}" 
                | <struct-or-union> <identifier>

<struct-or-union> ::= "struct" 
                | "union"

<struct-declaration-list> ::= <struct-declaration> 
                | <struct-declaration-list> <struct-declaration>

<struct-declaration> ::= <specifier-qualifier-list> <struct-declarator-list> ";"

<specifier-qualifier-list> ::= <type-specifier> 
                | <type-specifier> <specifier-qualifier-list> 
                | <type-qualifier> 
                | <type-qualifier> <specifier-qualifier-list>

<type-qualifier> ::= "const" 
                | "volatile"

<struct-declarator-list> ::= <struct-declarator> 
                | <struct-declarator-list> "," <struct-declarator>

<struct-declarator> ::= <declarator> 
                | ":" <constant-expression> 
                | <declarator> ":" <constant-expression>

<enum-specifier> ::= "enum" <identifier> "{" <enumerator-list> "}" 
                | "enum" <identifier>

<enumerator-list> ::= <enumerator> 
                | <enumerator-list> "," <enumerator>

<enumerator> ::= <identifier> 
                | <identifier> "=" <constant-expression>

<declarator> ::= <pointer> <direct-declarator> 
                | <direct-declarator>

<direct-declarator> ::= <identifier> 
                | "(" <declarator> ")" 
                | <direct-declarator> "[" <constant-expression> "]" 
                | <direct-declarator> "[" "]" 
                | <direct-declarator> "(" <parameter-type-list> ")" 
                | <direct-declarator> "(" <identifier-list> ")" 
                | <direct-declarator> "(" ")"

<pointer> ::= "*" 
                | "*" <type-qualifier-list> 
                | "*" <pointer> 
                | "*" <type-qualifier-list> <pointer>

<type-qualifier-list> ::= <type-qualifier> 
                | <type-qualifier-list> <type-qualifier>

<parameter-type-list> ::= <parameter-list> 
                | <parameter-list> "," "..."

<parameter-list> ::= <parameter-declaration> 
                | <parameter-list> "," <parameter-declaration>

<parameter-declaration> ::= <declaration-specifiers> <declarator> 
                | <declaration-specifiers> <abstract-declarator> 
                | <declaration-specifiers>

<identifier-list> ::= <identifier> 
                | <identifier-list> "," <identifier>

<declaration> ::= <declaration-specifiers> <init-declarator-list> ";"

<declaration-specifiers> ::= <storage-class-specifier> 
                | <storage-class-specifier> <declaration-specifiers> 
                | <type-specifier> 
                | <type-specifier> <declaration-specifiers> 
                | <type-qualifier> 
                | <type-qualifier> <declaration-specifiers>

<init-declarator-list> ::= <init-declarator> 
                | <init-declarator-list> "," <init-declarator>

<init-declarator> ::= <declarator> |

*/