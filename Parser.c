#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Base.h"
#include "Parser.h"


Token* CURR_TOK = NULL; // points to current token
Token* LAST_TOK = NULL;
Token CURR_FUNC;    // holds a copy of current function token


void Advance(){
    if (CURR_TOK != NULL) {
        LAST_TOK = CURR_TOK;
        CURR_TOK = CURR_TOK->next;
        free(LAST_TOK);
    }
}
void Move_on(){
    while(CURR_TOK->type == S_COLON){
        Advance();
    }
}
void Advance_print(){
    printf("CURR: %s\n",CURR_TOK->value);
}
Token* Seek(){
    return CURR_TOK->next;
}
void Error(char error_desc[],char error_type[]){
    ERRORS++;
    printf(RED "ERROR: [%s]\n" RESET,error_type);
    printf("%d|\t%s\n",LAST_TOK->line,error_desc);
    printf("'%s'\n",CURR_TOK->value);
    printf("\n");
    Move_on();
}


AST_Node* Function_Call(){
    if(DEBUG) printf("Function Call called\n");
    // curr tok is <id>
    AST_Node* Node;
    if(GetTableInstance(CURR_TOK->value, getStackTop()) == NULL){
        Error("Function not defined", "Runtime Error");
        return NULL;
    }
    Node = GiveNode();
    Node->TYPE = FUNCTION_CALL_NODE;
    Node->NODE.Func_Call.name = CURR_TOK->value;
    Node->NODE.Func_Call.addr = GetGlobal(CURR_TOK->value)->ADDR;
    Advance();  // curr tok is "("
    if(Seek()->type != R_PAREN){
        AST_Node* temp = Expression();
        AST_Node* iter = temp;
        Token* next = Seek();
        while(next->type == COMMA){
            Advance();  // curr tok is ","
            temp->SIBLING = Expression();
            temp = temp->SIBLING;
            next = Seek();
        }
        Node->NODE.Func_Call.args = iter;
    }
    Advance();
    if(CURR_TOK->type != R_PAREN){
        Error("Missing ')' .","Syntax Error");
        return NULL;
    }
    return Node;
}


AST_Node* Factor(){
    if(DEBUG) printf("Factor called\n");
    AST_Node* Node;
    if(CURR_TOK->type == L_PAREN){
        Node = Expression();
        Advance();  // curr tok ")"
        if(CURR_TOK->type != R_PAREN){
            return NULL; //error
        }
        return Node;
    }
    else if(CURR_TOK->type == MINUS){
        Node = GiveNode();
        Node->TYPE = UNOP_NODE;
        Node->NODE.UnOp.op = MINUS;
        Advance();
        Node->NODE.UnOp.val = Factor();
        return Node;
    }
    else if(CURR_TOK->type == LOG_NOT){
        Node = GiveNode();
        Node->TYPE = UNOP_NODE;
        Node->NODE.UnOp.op = LOG_NOT;
        Advance();
        Node->NODE.UnOp.val = Factor();
        return Node;
    }
    else if(CURR_TOK->type == CONST){
        Node = GiveNode();
        Node->TYPE = CONSTANT_NODE;
        Node->NODE.Const.Tok_Value = atoi(CURR_TOK->value);
        return Node;
    }
    else if(CURR_TOK->type == ID){
        if(Seek()->type == L_PAREN){
            Node = Function_Call();
            return Node;
        }
        else{
            if(GetTableInstance(CURR_TOK->value, getStackTop()) == NULL){
                Error("Token not defined","Runtime Error");
                return NULL;
            }
            Node = GiveNode();
            Node->TYPE = VARIABLE_NODE;
            Node->NODE.Variable.Tok_Value = malloc(strlen(CURR_TOK->value)+1);
            strcpy(Node->NODE.Variable.Tok_Value,CURR_TOK->value);
            return Node;
        }
    }
    else{
        return NULL; //error
    }
}

AST_Node* Term(){
    if(DEBUG) printf("Term called\n");
    AST_Node* left_term = Factor();

    Token* next = Seek();
    while(next->type == MULT || next->type == DIVIDE){
        Advance();  // curr tok is operator
        AST_Node* Node = GiveNode();
        Node->TYPE = BINOP_NODE;
        Node->NODE.BinOp.op = next->type;

        Advance();
        AST_Node* right_term = Factor();

        // Constant Folding
        if(left_term->TYPE == CONSTANT_NODE && right_term->TYPE == CONSTANT_NODE){
            if(next->type == MULT){
                left_term->NODE.Const.Tok_Value *= right_term->NODE.Const.Tok_Value;
            }
            else{
                left_term->NODE.Const.Tok_Value /= right_term->NODE.Const.Tok_Value;                
            }
            free(right_term);
            free(Node);
        }
        else{
            Node->NODE.BinOp.right = right_term;
            Node->NODE.BinOp.left = left_term;
            left_term = Node; 
        }
        next = Seek();
    }
    return left_term;
}

AST_Node* Additive_Exp(){
    if(DEBUG) printf("Additive_Exp called\n");
    AST_Node* left_term = Term();

    Token* next = Seek();
    while(next->type == PLUS || next->type == MINUS){
        Advance();
        AST_Node* Node = GiveNode();
        Node->TYPE = BINOP_NODE;
        Node->NODE.BinOp.op = next->type;

        Advance();
        AST_Node* right_term = Term();

        // Constant Folding
        if(left_term->TYPE == CONSTANT_NODE && right_term->TYPE == CONSTANT_NODE){
            if(next->type == PLUS){
                left_term->NODE.Const.Tok_Value += right_term->NODE.Const.Tok_Value;
            }
            else{
                left_term->NODE.Const.Tok_Value -= right_term->NODE.Const.Tok_Value;                
            }
            free(right_term);
            free(Node);
        }
        else{
            Node->NODE.BinOp.right = right_term;
            Node->NODE.BinOp.left = left_term;
            left_term = Node; 
        }
        next = Seek();
    }
    return left_term;
}

AST_Node* Relational_Exp(){
    if(DEBUG) printf("Relational_Exp called\n");
    AST_Node* left_term = Additive_Exp();

    Token* next = Seek();
    while(next->type == LT || next->type == LE || next->type == GT || next->type == GE){
        Advance();
        AST_Node* Node = GiveNode();
        Node->TYPE = RELOP_NODE;
        Node->NODE.RelOp.op = next->type;

        Advance();
        AST_Node* right_term = Additive_Exp();

        // Constant Folding
        if(left_term->TYPE == CONSTANT_NODE && right_term->TYPE == CONSTANT_NODE){
            if(next->type == LT){
                left_term->NODE.Const.Tok_Value = 
                    left_term->NODE.Const.Tok_Value < right_term->NODE.Const.Tok_Value;
            }
            else if(next->type == LE){
                left_term->NODE.Const.Tok_Value = 
                    left_term->NODE.Const.Tok_Value <= right_term->NODE.Const.Tok_Value;                
            }
            else if(next->type == GT){
                left_term->NODE.Const.Tok_Value = 
                    left_term->NODE.Const.Tok_Value > right_term->NODE.Const.Tok_Value;                
            }
            else if(next->type == GE){
                left_term->NODE.Const.Tok_Value = 
                    left_term->NODE.Const.Tok_Value >= right_term->NODE.Const.Tok_Value;                
            }
            free(right_term);
            free(Node);
        }
        else{
            Node->NODE.RelOp.right = right_term;
            Node->NODE.RelOp.left = left_term;
            left_term = Node; 
        }
        next = Seek();
    }
    return left_term;
}

AST_Node* Equality_Exp(){
    if(DEBUG) printf("Equality_Exp called\n");
    AST_Node* left_term = Relational_Exp();

    Token* next = Seek();
    while(next->type == EQ || next->type == NE){
        Advance();
        AST_Node* Node = GiveNode();
        Node->TYPE = RELOP_NODE;
        Node->NODE.RelOp.op = next->type;

        Advance();
        AST_Node* right_term = Relational_Exp();

        // Constant Folding
        if(left_term->TYPE == CONSTANT_NODE && right_term->TYPE == CONSTANT_NODE){
            if(next->type == EQ){
                left_term->NODE.Const.Tok_Value = 
                    left_term->NODE.Const.Tok_Value == right_term->NODE.Const.Tok_Value;
            }
            else{
                left_term->NODE.Const.Tok_Value = 
                    left_term->NODE.Const.Tok_Value != right_term->NODE.Const.Tok_Value;
            }
            free(right_term);
            free(Node);
        }
        else{
            Node->NODE.RelOp.right = right_term;
            Node->NODE.RelOp.left = left_term;
            left_term = Node; 
        }
        next = Seek();
    }
    return left_term;
}

AST_Node* Logical_AndExp(){
    if(DEBUG) printf("Logical_AndExp called\n");
    AST_Node* left_term = Equality_Exp();

    Token* next = Seek();
    while(next->type == LOG_AND){
        Advance();
        AST_Node* Node = GiveNode();
        Node->TYPE = RELOP_NODE;
        Node->NODE.RelOp.op = next->type;

        Advance();
        AST_Node* right_term = Equality_Exp();

        // Constant Folding
        if(left_term->TYPE == CONSTANT_NODE && right_term->TYPE == CONSTANT_NODE){
            left_term->NODE.Const.Tok_Value = 
                left_term->NODE.Const.Tok_Value && right_term->NODE.Const.Tok_Value;
            free(right_term);
            free(Node);
        }
        else{
            Node->NODE.RelOp.right = right_term;
            Node->NODE.RelOp.left = left_term;
            left_term = Node; 
        }
        next = Seek();
    }
    return left_term;
}

AST_Node* Logical_OrExp(){
    if(DEBUG) printf("Logical_OrExp called\n");
    AST_Node* left_term = Logical_AndExp();

    Token* next = Seek();
    while(next->type == LOG_OR){
        Advance();
        AST_Node* Node = GiveNode();
        Node->TYPE = RELOP_NODE;
        Node->NODE.RelOp.op = next->type;

        Advance();
        AST_Node* right_term = Logical_AndExp();

        // Constant Folding
        if(left_term->TYPE == CONSTANT_NODE && right_term->TYPE == CONSTANT_NODE){
            left_term->NODE.Const.Tok_Value = 
                left_term->NODE.Const.Tok_Value || right_term->NODE.Const.Tok_Value;
            free(right_term);
            free(Node);
        }
        else{
            Node->NODE.RelOp.right = right_term;
            Node->NODE.RelOp.left = left_term;
            left_term = Node; 
        }
        next = Seek();
    }
    return left_term;
}

AST_Node* Conditional_Exp(){
    if(DEBUG) printf("Conditional_Exp called\n");
    AST_Node* left_term = Logical_OrExp();
    if(Seek()->type == QUES_MARK){
        AST_Node* right_term = GiveNode();
        right_term->TYPE = IF_NODE;
        Advance(); // curr tok is ?
        right_term->NODE.IfBlock.cond_block = left_term;
        right_term->NODE.IfBlock.stmnt_block = Expression();
        Advance(); // curr tok is :
        Advance(); // initiator
        right_term->NODE.IfBlock.else_block = Conditional_Exp();
        left_term = right_term;
    }
    return left_term;
}

AST_Node* Expression(){
    if(DEBUG) printf("Expression called\n");
    AST_Node* left_term = NULL;
    
    Advance(); // curr tok is <id> or a number but after, if its a "=", then go into if block
    if(Seek()->type == ASN_OP){
        if(CURR_TOK->type != ID){
            Error("Can't assign to constants","Syntax Error");
            return NULL;
        }
        // if(GetHash(GetGlobal(CURR_FUNC.value)->LOCAL_TABLE,CURR_TOK->value) == NULL){
        //     Error("Token not declared","Runtime Error");
        //     return NULL;
        // }

        if(GetTableInstance(CURR_TOK->value, getStackTop()) == NULL){
            Error("Token not declared. ","Runtime Error");
            return NULL;
        }

        left_term = GiveNode();
        left_term->TYPE = ASSIGN_NODE;
        left_term->NODE.AssignOp.var = GiveNode();
        left_term->NODE.AssignOp.var->TYPE = VARIABLE_NODE;
        left_term->NODE.AssignOp.var->NODE.Variable.Tok_Value = malloc(strlen(CURR_TOK->value)+1);
        strcpy(left_term->NODE.AssignOp.var->NODE.Variable.Tok_Value,CURR_TOK->value);
        
        Advance(); // curr tok is =
        left_term->NODE.AssignOp.expr = Expression();
    }
    else{
        left_term = Conditional_Exp();
    }
    return left_term;
    
}

AST_Node* Statement(){
    if(DEBUG) printf("Statement called\n");
    AST_Node* Node = NULL;

    // return node
    if(Seek()->type == KW_RETURN){
        Advance(); // curr tok is return
        Node = GiveNode();
        Node->TYPE = RETURN_NODE;
        Node->NODE.Return.val = Expression();
        Advance(); // curr tok is ;
        if(CURR_TOK->type != S_COLON){
            Error("Missing ';' .","Syntax Error");
            return NULL; //error
        }
    }

    // If Else Node
    else if(Seek()->type == KW_IF){
        Advance(); // curr tok is if
        Node = GiveNode();
        Node->TYPE = IF_NODE;
        Node->NODE.IfBlock.else_block = NULL;
        Node->NODE.IfBlock.stmnt_block = NULL;

        Advance(); // curr tok is (
        if(CURR_TOK->type != L_PAREN){
            Error("Missing '(' .","Syntax Error");
            return NULL; //error
        }
        // Parsing the conditions
        Node->NODE.IfBlock.cond_block = Expression();
        
        Advance(); // curr tok is )
        if(CURR_TOK->type != R_PAREN){
            Error("Missing ')' .","Syntax Error");
            return NULL; //error
        }

        TableData* tempTable = createTableInstance();  // creating new table
        tempTable->ADDR = Node; // defining the ownership
        Node->NODE.IfBlock.table = tempTable;   // assigning the table to the node
        PutTableInstance("if-else", Node, getStackTop());   // putting into the topmost symbol table
        pushToStack(Node);

        // Parsing the statements
        Node->NODE.IfBlock.stmnt_block = Statement();
        
        if(Seek()->type == KW_ELSE){
            Advance(); // curr tok is else
            Node->NODE.IfBlock.else_block = Statement();
        }
        popFromStack();
    }

    // For loop 
    else if(Seek()->type == KW_FOR){
        Advance(); // curr tok is "for"
        Advance(); // curr tok is "("
        Node = GiveNode();
        Node->TYPE = FOR_NODE;
        Node->NODE.ForLoop.init_block = NULL;
        Node->NODE.ForLoop.cond_block = NULL;
        Node->NODE.ForLoop.inc_block = NULL;
        Node->NODE.ForLoop.body = NULL;

        TableData* tempTable = createTableInstance();  // creating new table
        tempTable->ADDR = Node; // defining the ownership
        Node->NODE.ForLoop.table = tempTable;   // assigning the table to the node
        PutTableInstance("for-loop", Node, getStackTop());   // putting into the topmost symbol table
        pushToStack(Node);

        if(Seek()->type != S_COLON){
            if(Seek()->type == DATA_TYPE){
                Node->NODE.ForLoop.init_block = Declaration();
                // Declaration ends with ";" parsed
            }
            else{
                Node->NODE.ForLoop.init_block = Expression();
                Advance(); // curr tok is ";"
            }       
        }
        else{
            Advance(); // curr tok is ";"
        }

        if(Seek()->type != S_COLON){
            Node->NODE.ForLoop.cond_block = Expression();
        }
        /*
        else{
            Node->NODE.ForLoop.cond_block = GiveNode();
            Node->NODE.ForLoop.cond_block->TYPE = CONSTANT_NODE;
            Node->NODE.ForLoop.cond_block->NODE.Const.Tok_Value = 1;
        }
        */
        Advance(); // curr tok is ";"
        if(Seek()->type != R_PAREN){
            Node->NODE.ForLoop.inc_block = Expression();
        }
        Advance(); // curr tok is ")"
        Node->NODE.ForLoop.body = Statement();
        popFromStack();
    }

    // While loop
    else if(Seek()->type == KW_WHILE){
        Advance(); // curr tok is while
        Advance(); // curr tok is "("
        Node = GiveNode();
        Node->TYPE = WHILE_NODE;
        Node->NODE.WhileLoop.body = NULL;
        Node->NODE.WhileLoop.cond_block = NULL;

        TableData* tempTable = createTableInstance();  // creating new table
        tempTable->ADDR = Node; // defining the ownership
        Node->NODE.WhileLoop.table = tempTable;   // assigning the table to the node
        PutTableInstance("while-loop", Node, getStackTop());   // putting into the topmost symbol table
        pushToStack(Node);

        Node->NODE.WhileLoop.cond_block = Expression();
        Advance(); // curr tok is ")"
        Node->NODE.WhileLoop.body = Statement();
        popFromStack();
    }

    // Block item
    else if(Seek()->type == L_BRACE){
        Advance(); // curr tok is "{"
        if(Seek()->type != R_BRACE){
            AST_Node* temp = Block();
            AST_Node* Body = temp;
            while(Seek()->type != R_BRACE){
                temp->SIBLING = Block();
                temp = temp->SIBLING;
            }
            Node = Body;
        }
        Advance();  // curr tok is "}"
        if(CURR_TOK->type != R_BRACE){
            Error("Missing '}' .","Syntax Error");
            return NULL; //error
        }
    }
    
    // Do While Loop
    else if(Seek()->type == KW_DO){
        Advance();  // curr tok is "do"
        Node = GiveNode();
        Node->TYPE = DO_WHILE_NODE;

        TableData* tempTable = createTableInstance();  // creating new table
        tempTable->ADDR = Node; // defining the ownership
        Node->NODE.WhileLoop.table = tempTable;   // assigning the table to the node
        PutTableInstance("do-while", Node, getStackTop());   // putting into the topmost symbol table
        pushToStack(Node);

        Node->NODE.WhileLoop.body = Statement();
        Advance();  // curr tok is "while"
        Advance();  // curr tok is "(" 
        Node->NODE.WhileLoop.cond_block = Expression();
        Advance();  // curr tok is ")"
        Advance();  // curr tok is ";"

        popFromStack();
        return Node;
    }

    // Break and Continue
    else if(Seek()->type == KW_BREAK || Seek()->type == KW_CONTINUE){
        Advance();  // curr tok is "break"/"continue"
        Node = GiveNode();
        Node->TYPE = KEYWORD_NODE;
        Node->NODE.Keyword.kw = CURR_TOK->type;
        Advance();  // curr tok is ";"
        return Node;
    }
    else{
        Node = Expression();
        Advance(); // curr tok is ;
        if(CURR_TOK->type != S_COLON){
            Error("Missing ';' .","Syntax Error");
            return NULL; //error
        }
    }
    return Node;
}

AST_Node* Declaration(){
    if(DEBUG) printf("Declaration called\n");
    AST_Node* Node;
    Advance(); // curr tok is int
    Advance(); // curr tok is id

    if(CURR_TOK->type != ID){
        Error("Missing identifier name. ","Syntax Error");
        return NULL; //error
    }
    if(GetTableInstance(CURR_TOK->value, getStackTop()) != NULL){
        Error("Variable already declared. ","Syntax Error");
        return NULL;
    }
    // if(GetHash(GetGlobal(CURR_FUNC.value)->LOCAL_TABLE,CURR_TOK->value)){
    //     Error("Variable already declared. ","Syntax Error");
    //     return NULL;
    // }
    Node = GiveNode();
    Node->TYPE = VARIABLE_NODE;
    Node->NODE.Variable.Tok_Value = malloc(strlen(CURR_TOK->value)+1);
    strcpy(Node->NODE.Variable.Tok_Value,CURR_TOK->value);
    
    // PutHash(GetGlobal(CURR_FUNC.value)->LOCAL_TABLE,CURR_TOK->value);
    PutTableInstance(CURR_TOK->value, Node, getStackTop());

    if(Seek()->type == ASN_OP){
        // if equal to is present assign the value
        Advance();  // curr tok is =
        AST_Node* new_node = GiveNode();
        new_node->TYPE = ASSIGN_NODE;
        new_node->NODE.AssignOp.var = Node;
        new_node->NODE.AssignOp.expr = Expression();
        Node = new_node;
    }
    else{
        // if equal to is not present declare it with 0
        AST_Node* new_node = GiveNode();
        new_node->TYPE = ASSIGN_NODE;
        new_node->NODE.AssignOp.var = Node;
        new_node->NODE.AssignOp.expr = GiveNode();
        new_node->NODE.AssignOp.expr->TYPE = CONSTANT_NODE;
        new_node->NODE.AssignOp.expr->NODE.Const.Tok_Value = 0;
        Node = new_node;
    }
    Advance(); // curr tok is ;
    if(CURR_TOK->type != S_COLON){
        Error("Missing ';' .","Syntax Error");
        return NULL; //error
    }
    return Node;
}

AST_Node* Block(){
    if(DEBUG) printf("Block called\n");
    AST_Node* Node = NULL;
    if(Seek()->type == DATA_TYPE){
        Node = Declaration();   
    }
    else{
        Node = Statement();
    }
    return Node;
}

AST_Node* Function(){
    if(DEBUG) printf("Function called\n");
    Advance();
    if(!CURR_TOK){
        return NULL;
    }

    AST_Node* Node = GiveNode();
    Node->NODE.Func.body = NULL;
    Node->NODE.Func.params = NULL;

    if(CURR_TOK->type != DATA_TYPE){
        Error("Invalid declaration of function, missing function type. ","Syntax Error");
        return NULL; //error
    }
    Advance();  // curr tok is func_name
    if(CURR_TOK->type != KW_MAIN && CURR_TOK->type != ID){
        Error("Invalid declaration of function, missing function name. ","Syntax Error");
        return NULL; //error
    }
    Node->TYPE = FUNCTION_DECL_NODE;
    Node->NODE.Func.name = malloc(strlen(CURR_TOK->value)+1);
    strcpy(Node->NODE.Func.name,CURR_TOK->value);

    // CURR_FUNC = *CURR_TOK;
    TableData* initTable = createTableInstance(); // creating a symbol table
    initTable->ADDR = Node; // defining the ownership
    Node->NODE.Func.table = initTable;  // assigning the symbol table to the new function
    PutTableInstance(CURR_TOK->value, Node, getStackTop()); // putting into the topmost symbol table
    pushToStack(Node);  // pushing the function into the stack
    // PutGlobal(CURR_FUNC.value,Node);

    // Parsing the parameters
    Advance();
    if(CURR_TOK->type != L_PAREN){
        Error("Missing '(' .","Syntax Error");
        return NULL; //error
    }
    if(Seek()->type == DATA_TYPE){
        Advance(); // curr tok is int
        Advance(); // curr tok is id

        AST_Node* iterator = GiveNode();
        iterator->NODE.Variable.Tok_Value = CURR_TOK->value;
        iterator->TYPE = VARIABLE_NODE;
        PutTableInstance(CURR_TOK->value, iterator, getStackTop());
        // PutHash(GetGlobal(CURR_FUNC.value)->LOCAL_TABLE,CURR_TOK->value);
        AST_Node* parameter = iterator;

        Token* Next = Seek();
        while(Next->type == COMMA){
            Advance(); // curr tok is ","
            Advance(); // curr tok is int
            Advance(); // curr tok is id
            AST_Node* temp = GiveNode();
            temp->NODE.Variable.Tok_Value = CURR_TOK->value;
            temp->TYPE = VARIABLE_NODE;
            PutTableInstance(CURR_TOK->value, temp, getStackTop());
            // PutHash(GetGlobal(CURR_FUNC.value)->LOCAL_TABLE,CURR_TOK->value);
            iterator->SIBLING = temp;
            iterator = iterator->SIBLING;
            Next = Seek();
        }
        Node->NODE.Func.params = parameter;
    }
    Advance();
    if(CURR_TOK->type != R_PAREN){
        Error("Missing ')' .","Syntax Error");
        return NULL; //error
    }

    // Parsing the function body
    if(Seek()->type != S_COLON){
        Advance();
        if(CURR_TOK->type != L_BRACE){
            Error("Missing '{' .","Syntax Error");
            return NULL; //error
        }
        // if not empty function
        if(Seek()->type != R_BRACE){
            AST_Node* temp = Block();
            AST_Node* Body = temp;
            while(Seek()->type != R_BRACE){
                temp->SIBLING = Block();
                temp = temp->SIBLING;
            }
            Node->NODE.Func.body = Body;
        }
        Advance();
        if(CURR_TOK->type != R_BRACE){
            Error("Missing '}' .","Syntax Error");
            return NULL; //error
        }
    }
    else{
        Advance(); // curr tok is ";"
    }
    popFromStack();
    Node->SIBLING = Function();
    return Node;
}



void PARSER(){
    CURR_TOK = BASE_TOKEN;     //Token array pointer
    printf("Starting to parse\n");
    
    BASE_AST_NODE->NODE.Prog.child = Function();
    printf(GRN "[PARSER ENDS]\n\n" RESET);
}