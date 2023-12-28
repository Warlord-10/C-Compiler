// Lexical Tokens (LEXER PART)
#include "Base.h"
#include <stdio.h>


// Defining Global Variables
char* FILE_NAME;                                // File name which is to be compiled
int ERRORS = 0;
int DEBUG = 0;

Token* BASE_TOKEN;                              // Parent Node of the lexemes
AST_Node* BASE_AST_NODE;                        // Parent Node of the AST

GlobalTable* GLOBAL_TABLE_HEAD[GLOBAL_LEN];     // Head pointer to Global symbol table
TableData* GLOBAL_SYMBOL_TABLE;                  // Hash table 2.0

StackElement* FUNCTION_STACK;
StackElement* STACK_TOP;

// Gives a new empty token for the Lexer
Token* GiveTok(){
    Token* Tok = (Token*)malloc(sizeof(Token));
    Tok->next = NULL;
    Tok->character = 0;
    Tok->line = 0;
    return Tok;
}
void show_tokens(Token* ptr){
    while(ptr){
        printf("%s : [ln:%d, ch:%d]\n",ptr->value,ptr->line,ptr->character);
        ptr = ptr->next;
    }
}


// Gives a new empty node for the Parser
AST_Node* GiveNode(){
    AST_Node* Node = (AST_Node*)malloc(sizeof(AST_Node));
    Node->SIBLING = NULL;
    return Node;
}
void TakeNode(AST_Node** n){
    if((*n)->SIBLING != NULL){
        TakeNode(&((*n)->SIBLING));
    }
    free(*n);   // The pointer still exists but points to empty location
    *n = NULL;  // Pointer points to NULL
}


void gapfill(int indent){
    for(int i=0;i<indent;i++){
        printf(" ");
    }
}
void show_tree(int indent, AST_Node* ptr){
    if(ptr == NULL){
        return;
    }
    gapfill(indent);
    switch(ptr->TYPE){
        case PROGRAM_NODE:
            printf("Program [%s]:\n",ptr->NODE.Prog.head);
            show_tree(indent+4,ptr->NODE.Prog.child);
            break;
        case FUNCTION_DECL_NODE:
            printf("Function Decl [%s]:\n",ptr->NODE.Func.name);

            gapfill(indent+4);
            printf("Parameters:\n");
            show_tree(indent+8,ptr->NODE.Func.params);
            gapfill(indent+4);
            printf("-----------\n");

            show_tree(indent+4,ptr->NODE.Func.body);
            break;
        case FUNCTION_CALL_NODE:
            printf("Function Call [%s]:\n",ptr->NODE.Func_Call.name);
            show_tree(indent+4,ptr->NODE.Func.params);
            break;
        case BINOP_NODE:
            printf("Binary Operator [%d]:\n",ptr->NODE.BinOp.op);
            show_tree(indent+4,ptr->NODE.BinOp.left);
            show_tree(indent+4,ptr->NODE.BinOp.right);
            break;
        case RELOP_NODE:
            printf("Relative Operator [%d]:\n",ptr->NODE.RelOp.op);
            show_tree(indent+4,ptr->NODE.RelOp.left);
            show_tree(indent+4,ptr->NODE.RelOp.right);
            break;
        case RETURN_NODE:
            printf("Return:\n");
            show_tree(indent+4,ptr->NODE.Return.val);
            break;
        case UNOP_NODE:
            printf("Unary Operator [%d]:\n",ptr->NODE.UnOp.op);
            show_tree(indent+4,ptr->NODE.UnOp.val);
            break;
        case CONSTANT_NODE:
            printf("Constant [%d]\n",ptr->NODE.Const.Tok_Value);
            break;
        case VARIABLE_NODE:
            printf("Variable [%s]\n",ptr->NODE.Variable.Tok_Value);
            break;
        case ASSIGN_NODE:
            printf("Assignment:\n");
            show_tree(indent+4,ptr->NODE.AssignOp.var);
            if(ptr->NODE.AssignOp.expr != NULL){
                show_tree(indent+4,ptr->NODE.AssignOp.expr);
            }
            break;
        case IF_NODE:
            printf("If Condition:\n");
            show_tree(indent+4,ptr->NODE.IfBlock.cond_block);
            show_tree(indent+4,ptr->NODE.IfBlock.stmnt_block);
            show_tree(indent+4,ptr->NODE.IfBlock.else_block);
            break;
        case FOR_NODE:
            printf("For Loop:\n");
            show_tree(indent+4,ptr->NODE.ForLoop.init_block);
            show_tree(indent+4,ptr->NODE.ForLoop.cond_block);
            show_tree(indent+4,ptr->NODE.ForLoop.inc_block);
            show_tree(indent+4,ptr->NODE.ForLoop.body);
            break;
        case WHILE_NODE:
            printf("While Loop:\n");
            show_tree(indent+4,ptr->NODE.WhileLoop.body);
            show_tree(indent+4,ptr->NODE.WhileLoop.cond_block);
            break;
        case KEYWORD_NODE:
            printf("Keyword [%d]:\n",ptr->NODE.Keyword.kw);
            break;
        default:
            printf("anything\n");

    }
        show_tree(indent,ptr->SIBLING);
    if(ptr->SIBLING){
    }
}


// The hash function
int HashFunction(char* t, int type){
    int i=0, ans=0;
    while(*(t+i)){
        ans += *(t+i);
        i++;
    }
    return ans%type;
}

// Inserts Function data into the global table
void PutGlobal(char* t, AST_Node* addr){
    GlobalTable* b = (GlobalTable*)malloc(sizeof(GlobalTable)); // allocates memory
    b->NAME = malloc(strlen(t)+1);  // allocates memory for name
    strcpy(b->NAME,t);
    b->NEXT = NULL;
    b->ADDR = addr; // pointer to AST node

    b->LOCAL_TABLE = malloc(HASH_LEN * sizeof(HashTable*)); // allocates an array for local table
    for(int i=0;i<HASH_LEN;i++){
        b->LOCAL_TABLE[i] = NULL;
    }

    //index pointer holds the address of table elements(pointers), as we want to point the pointer to the object
    GlobalTable** index_pointer = GLOBAL_TABLE_HEAD+HashFunction(t, GLOBAL_LEN);
    if(*index_pointer){     // if its pointing to a table block
        GlobalTable* temp = *index_pointer;
        while(temp->NEXT){
            temp = temp->NEXT;
        }
        temp->NEXT = b;
    }
    else{
        *index_pointer = b;
    }
}

// Returns the address of Local Hash Table
GlobalTable* GetGlobal(char* name){
    GlobalTable** index_pointer = GLOBAL_TABLE_HEAD+HashFunction(name, GLOBAL_LEN);
    if(*index_pointer){
        GlobalTable* temp = *index_pointer;
        while(temp){
            if(strcmp(temp->NAME,name) == 0){ 
                return temp;
            }
            temp = temp->NEXT;
        }
    }
    return NULL;
}

// Enters into the local hash table
void PutHash(HashTable** table, char* name){
    HashTable* Hash = (HashTable*)malloc(sizeof(HashTable));
    Hash->NAME = malloc(strlen(name)+1);
    strcpy(Hash->NAME,name);
    Hash->CHAIN = NULL;
    Hash->OFFSET = 0;

    //HashTable** HASH_TABLE = table; 
    HashTable** table_pointer = table+HashFunction(name, HASH_LEN);  // points to the index of hashtable
    //printf("Hash: %d, Name: %s, No: %d\n",Hash,Hash->NAME->value,HashFunction(name));
    //printf("table_pointer: %d\n",table_pointer);

    if(*table_pointer){     // checks if the index points to the object
        HashTable* temp = *table_pointer;
        while(temp->CHAIN){
            temp = temp->CHAIN;
        }
        temp->CHAIN = Hash;
    }
    else{
        *table_pointer = Hash;
    }
    //printf("ptr: %d\n\n",*table_pointer);
    //for(int i=0;i<HASH_LEN;i++){
    //    printf("loc addr: %d, %d->%d\n",i,(table+i),*(table+i));
    //}
}

// Gets the data from local hash table
HashTable* GetHash(HashTable** table, char* name){
    HashTable** table_pointer = table+HashFunction(name, HASH_LEN);
    if(*table_pointer){
        HashTable* temp = *table_pointer;
        while(temp){
            if(strcmp(temp->NAME,name) == 0){
                return temp;
            }
            temp = temp->CHAIN;
        }
    }
    else{
        return NULL;
    }
}

// removes the Hash 
void RemoveHash(char* t){

}

// show local table
void ShowHash(HashTable** table){
    for(int i=0;i<HASH_LEN;i++){
        printf("loc addr: %d, %d->%d, ",i,(table+i),*(table+i));
        if(*(table+i)){
            printf("<%s>",(*(table+i))->NAME);
            HashTable* temp = *(table+i);
            while(temp->CHAIN){
                printf("-><%s>",temp->NAME);
                temp = temp->CHAIN;
            }
        }
        printf("\n");
    }
    printf("\n");
}

// show global table
void ShowGlobal(){
    GlobalTable* table_pointer; // points to the object, pointed by the global hash index
    for(int i=0;i<GLOBAL_LEN;i++){
        table_pointer = *(GLOBAL_TABLE_HEAD+i);
        if(table_pointer){
            // If the global_table_head points to a valid object
            printf("\n<Global Index: %d>: {%d}->[%d]: <%s>\n",i,GLOBAL_TABLE_HEAD+i,table_pointer,table_pointer->NAME);
            ShowHash(table_pointer->LOCAL_TABLE);
        }
    }
}




// Hash 2.0
TableData* createTableInstance() {
    TableData* temp = (TableData*)malloc(sizeof(TableData));
    if (temp != NULL) {
        // Initialize members as needed
        temp->NAME = NULL;  
        temp->ADDR = NULL;
        temp->NEXT = NULL;

        // Initialize TABLE array elements to NULL
        for (int i = 0; i < GLOBAL_LEN; ++i) {
            temp->TABLE[i] = NULL;
        }
    }
    return temp;
}

void PutTableInstance(char* name, AST_Node* nodeAddr, TableData* tableAddr){
    printf("Putting Table: %s, %d, %d \n", name, nodeAddr, tableAddr);
    TableData* dataToInsert = createTableInstance();
    dataToInsert->NAME = malloc(strlen(name)+1);
    
    strcpy(dataToInsert->NAME, name);
    dataToInsert->ADDR = nodeAddr;
    dataToInsert->NEXT = NULL;
    int offset = 0;

    // Data is inserted into the program(global) table
    if(tableAddr == NULL){
        offset = HashFunction(name, GLOBAL_LEN);
        // If there's no data at this index, add data directly
        if(GLOBAL_SYMBOL_TABLE->TABLE[offset] == NULL) GLOBAL_SYMBOL_TABLE->TABLE[offset] = dataToInsert;
        else{
            // If there's already data at this index, add data to the end of the linked list
            TableData* temp = GLOBAL_SYMBOL_TABLE->TABLE[offset];
            while(temp->NEXT){
                temp = temp->NEXT;
            }
            temp->NEXT = dataToInsert;
        }
    }
    // Data is inserted inside a scope table.
    else{
        offset = HashFunction(name, GLOBAL_LEN);
        if(tableAddr->TABLE[offset] == NULL) tableAddr->TABLE[offset] = dataToInsert;
        else{
            TableData* temp = tableAddr->TABLE[offset];
            while(temp->NEXT){
                temp = temp->NEXT;
            }
            temp->NEXT = dataToInsert;
        }
    }
}

TableData* GetTableInstance(char* name, TableData* addr){
    int offset = 0; 
    TableData* temp;
    if(addr == NULL){  
        offset = HashFunction(name, GLOBAL_LEN);  
        temp = GLOBAL_SYMBOL_TABLE->TABLE[offset];
    }
    else{
        offset = HashFunction(name, GLOBAL_LEN);
        temp = addr->TABLE[offset];
    }
    while(temp){
        if(strcmp(name, temp->NAME) == 0) return temp;
        temp = temp->NEXT;
    }
    return NULL;
}


// Function Stack
int pushToStack(AST_Node* x){
    printf("pushing=>");
    StackElement* temp = (StackElement*)malloc(sizeof(StackElement));
    temp->NODE = x;
    temp->NEXT = NULL;
    temp->PREV = STACK_TOP;

    STACK_TOP->NEXT = temp;
    STACK_TOP = temp;
    return 0;
}
void popFromStack(){
    printf("popping=>");
    StackElement* temp = STACK_TOP;
    if(temp->NODE->TYPE != PROGRAM_NODE){
        STACK_TOP = STACK_TOP->PREV;
        STACK_TOP->NEXT = NULL;
        free(temp);
    }
}
TableData* getStackTop(){
    switch(STACK_TOP->NODE->TYPE){
        case PROGRAM_NODE:
            return STACK_TOP->NODE->NODE.Prog.table;
            break;
        case FUNCTION_DECL_NODE:
            return STACK_TOP->NODE->NODE.Func.table;
            break;
        case WHILE_NODE:
            return STACK_TOP->NODE->NODE.WhileLoop.table;
            break;
        case FOR_NODE:
            return STACK_TOP->NODE->NODE.ForLoop.table;
            break;
        case IF_NODE:
            return STACK_TOP->NODE->NODE.IfBlock.table;
            break;
    }
}


// Initialises the BASE_TOKEN and BASE_AST_NODE
void Set_Up(){

    // Assigning initial values to Global variables
    BASE_TOKEN = GiveTok();       // Parent Node of the lexemes
    BASE_AST_NODE = GiveNode();     // Parent Node of the AST
    GLOBAL_SYMBOL_TABLE = createTableInstance(); // The Global Symbol Table
    FUNCTION_STACK = (StackElement*)malloc(sizeof(StackElement));    // The primary Stack Element
    FILE_NAME = "test.txt";

    //Initialising lexer array
    BASE_TOKEN->type = PROG;
    strcpy(BASE_TOKEN->value,"test");
    
    //initialising parse tree
    BASE_AST_NODE->TYPE = PROGRAM_NODE;
    BASE_AST_NODE->NODE.Prog.head = malloc(strlen(BASE_TOKEN->value)+1);
    strcpy(BASE_AST_NODE->NODE.Prog.head,BASE_TOKEN->value);
    BASE_AST_NODE->NODE.Prog.table = GLOBAL_SYMBOL_TABLE;

    // Initialising the global symbol table
    GLOBAL_SYMBOL_TABLE->NAME = "MAIN";
    GLOBAL_SYMBOL_TABLE->ADDR = BASE_AST_NODE;
    GLOBAL_SYMBOL_TABLE->NEXT = NULL;

    //Initialising the function stack;
    FUNCTION_STACK->NODE = BASE_AST_NODE;
    FUNCTION_STACK->NEXT = NULL;
    FUNCTION_STACK->PREV = NULL;

    // Pointer, pointing to the top of the stack
    STACK_TOP = FUNCTION_STACK;

    printf("asas: %d \n", FUNCTION_STACK);
    printf("asfasfds: %d \n", STACK_TOP);
}
