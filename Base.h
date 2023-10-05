// HAR HAR MAHADEV //


#include <stdlib.h>     // malloc, free
#include <string.h>     //string functions


#ifndef Base
#define Base


// Defines
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define STR_LEN 32
#define GLOBAL_LEN 32
#define HASH_LEN 16


// Lexer Enums
typedef enum token_type{
    // Basic Symbols
    L_PAREN = 0,    //  (
    R_PAREN,        //  )
    L_BRACE,        //  {
    R_BRACE,        //  }
    S_COLON,        //  ;
    LOG_NOT,        //  !
    QUES_MARK,      //  ?
    COLON,          //  :
    COMMA,          //  ,

    // Arithmetaic Operators 
    PLUS,           //  +
    MINUS,          //  -
    MULT,           //  *
    DIVIDE,         //  /
    ASN_OP,         //  =

    // Relational Operators
    LOG_OR,         //  ||
    LOG_AND,        //  &&
    EQ,             //  ==
    NE,             //  !=
    LT,             //  <
    LE,             //  <=
    GT,             //  >
    GE,             //  >=

    // Data types
    DATA_TYPE,
    DATA_TYPE_VOID,
    DATA_TYPE_INT,

    // Keywords
    KW_MAIN,
    KW_RETURN,
    KW_IF,
    KW_ELSE,
    KW_FOR,
    KW_WHILE,
    KW_BREAK,
    KW_CONTINUE,
    KW_DO,

    // Others
    PROG,           //  Program
    ID,             //  Identifier
    CONST           //  Constant Literal
} TOK_TYPE;


// Parser Enums
typedef enum node_type{
    ERROR_NODE = -1,
    PROGRAM_NODE = 0,
    FUNCTION_DECL_NODE,
    FUNCTION_CALL_NODE,
    IF_NODE,
    FOR_NODE,
    WHILE_NODE,
    DO_WHILE_NODE,
    UNOP_NODE,
    BINOP_NODE,
    RELOP_NODE,
    ASSIGN_NODE,
    RETURN_NODE,
    CONSTANT_NODE,
    VARIABLE_NODE,
    KEYWORD_NODE
} NODE_TYPE;



// Lexical Tokens (LEXER PART)
typedef struct Token{
    TOK_TYPE type;
    char value[32];
    int character,line;
    struct Token *next;
} Token;
Token* GiveTok(); 
void show_tokens(Token* ptr);



// AST Nodes (PARSER PART)
typedef struct AST_Node{
    NODE_TYPE TYPE;
    struct AST_Node* SIBLING;
    union TreeStructure{
        struct Prog{                    // Program
            char* head;
            struct AST_Node* child;
        }Prog;
        struct Func{                    // Function Declaration
            char* name;
            struct AST_Node* body;
            struct AST_Node* params;
        }Func;
        struct Func_Call{               // Function Call
            char* name;
            struct AST_Node* addr;
            struct AST_Node* args;
        }Func_Call;
        struct UnOp{                    // Unary Operator
            TOK_TYPE op;
            struct AST_Node* val;
        }UnOp;
        struct BinOp{                   // Binary Operator
            TOK_TYPE op;
            struct AST_Node* left;
            struct AST_Node* right; 
        }BinOp;
        struct RelOp{                   // Relational Operator
            TOK_TYPE op;
            struct AST_Node* left;
            struct AST_Node* right; 
        }RelOp;
        struct AssignOp{                // Assignment Operator
            struct AST_Node* var;
            struct AST_Node* expr;
        }AssignOp;
        struct Return{                  // Return Operator
            struct AST_Node* val;
        }Return;
        struct Const{                   // Constant Operator
            // data_type Data_Type;
            int Tok_Value;
        }Const;
        struct Variable{                // Variable Operator
            // data_type Data_Type;
            char* Tok_Value;                  
        }Variable;
        struct IfBlock{                 // If Statement
            struct AST_Node* cond_block;
            struct AST_Node* stmnt_block;
            struct AST_Node* else_block;
        }IfBlock;
        struct ForLoop{                 // For Loop
            struct AST_Node* init_block;
            struct AST_Node* cond_block;
            struct AST_Node* inc_block;
            struct AST_Node* body;
        }ForLoop;
        struct WhileLoop{               // While Loop
            struct AST_Node* cond_block;
            struct AST_Node* body;
        }WhileLoop;
        struct Keyword{                 // Keywords
            TOK_TYPE kw;
        }Keyword;
    }NODE;
}AST_Node;
AST_Node* GiveNode();
void TakeNode(AST_Node* n);
void show_tree(int indent, AST_Node* ptr);



// Hash Table
typedef struct HashTable{
    char* NAME;    // Name of the variable
    int OFFSET;     // Offset from the EBP register
    struct HashTable* CHAIN;    // pointer to the next chain table block
} HashTable;

typedef struct GlobalTable{
    char* NAME;    // name of the function
    HashTable** LOCAL_TABLE;    // pointer to local table
    AST_Node* ADDR;     // pointer to node
    struct GlobalTable* NEXT;   // pointer to next sibling
} GlobalTable;

int HashFunction(char* t, int type);  // the hash function

void PutGlobal(char* t, AST_Node* addr);    // insert into the global table
GlobalTable* GetGlobal(char* name);  // returns from the global table

void PutHash(HashTable** table, char* name);    // insert into hash table
void RemoveHash(char* name);    // removes the hash entry
HashTable* GetHash(HashTable** table, char* name);  // returns from global table

void ShowHash(HashTable** t);
void ShowGlobal();
// Errors



// Constants Declaration
extern Token* BASE_TOKEN;       // Parent Node of the lexemes
extern AST_Node* BASE_AST_NODE;     // Parent Node of the AST
extern GlobalTable* GLOBAL_TABLE_HEAD[GLOBAL_LEN];  // Pointer to Global Hash Table
extern char* FILE_NAME;     // Actual file name
extern int ERRORS;  // Number of errors 

void Set_Up();  // Initiatilizes the data


#endif


