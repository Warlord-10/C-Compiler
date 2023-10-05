#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include "Base.h"
#include "Assembler.h"

/*  
    basic approach:
        convert every node to equivalent assembly
        look for symbol table for the variable name, data_type and initial value

    use fprintf
    use fputc
*/

char ASM_FILE_NAME[32];
FILE *file_to_write;

AST_Node *CURR_FUNC2;
int pointer = 0;
int counter = 0;



void Assemble(AST_Node *ptr){
    if(!ptr){
        return;
    }
    switch(ptr->TYPE){
        case PROGRAM_NODE:
            /*
                #include stdfunc.asm
                SECTION .text
                global _start
            */
            fprintf(file_to_write,"#include\t'stdfunc.asm'\n\n");
            fprintf(file_to_write,"SECTION .text\nglobal _start\n");
            Assemble(ptr->NODE.Prog.child);
            fprintf(file_to_write,"\n;Code Generated");
            fclose(file_to_write);
            break;
            
        case FUNCTION_DECL_NODE:
            CURR_FUNC2 = ptr;
            pointer = 0;
            /*
                <func_name>:
                    push ebp
                    mov ebp, esp
                    <body>
                    mov esp, ebp
                    pop ebp
                    ret
            */
            fprintf(file_to_write,"\n%s:\n",ptr->NODE.Func.name);
            fprintf(file_to_write,"\tpush ebp\n\tmov ebp, esp\n\n");
            Assemble(ptr->NODE.Func.body);
            fprintf(file_to_write,"\tmov esp, ebp\n\tpop ebp\n");
            fprintf(file_to_write,"\tret\n");
            break;

        case ASSIGN_NODE:
            // remove the variable and const from switch and put it inside the assn and binop block
            // if the variable is declared then it can only be defined multiple times
            HashTable* temp = GetHash(GetGlobal(CURR_FUNC2->NODE.Func.name)->LOCAL_TABLE,
                                ptr->NODE.AssignOp.var->NODE.Variable.Tok_Value);
            if(temp->OFFSET == 0){  // address must me filled in hash table
                pointer+=4;
                temp->OFFSET = pointer;
                fprintf(file_to_write,"\tsub esp, 4\n");
            }
            if(ptr->NODE.AssignOp.expr->TYPE == CONSTANT_NODE){
                fprintf(file_to_write,"\tmov ");
                Assemble(ptr->NODE.AssignOp.var);
                fprintf(file_to_write,", ");
                Assemble(ptr->NODE.AssignOp.expr);
                fprintf(file_to_write,"\n");
            }
            else if(ptr->NODE.AssignOp.expr->TYPE == VARIABLE_NODE){
                fprintf(file_to_write,"\tmov ecx, ");
                Assemble(ptr->NODE.AssignOp.expr);
                fprintf(file_to_write,"\n\tmov ");
                Assemble(ptr->NODE.AssignOp.var);
                fprintf(file_to_write,", ecx\n");
            }
            else{
                Assemble(ptr->NODE.AssignOp.expr);
                fprintf(file_to_write,"\tmov ");
                Assemble(ptr->NODE.AssignOp.var);
                fprintf(file_to_write,", eax\n");
            }
            break;
            
        case BINOP_NODE: 
            // Final sum is always moved to eax register
            int op_node=0;
            // If the root have a binary operator in its left node
            if(ptr->NODE.BinOp.left->TYPE != CONSTANT_NODE && ptr->NODE.BinOp.left->TYPE != VARIABLE_NODE){
                op_node++;
                Assemble(ptr->NODE.BinOp.left);
                fprintf(file_to_write,"\tmov ecx, eax\n");
            }
            // If the root have a binary operator in its right node
            if(ptr->NODE.BinOp.right->TYPE != CONSTANT_NODE && ptr->NODE.BinOp.right->TYPE != VARIABLE_NODE){
                op_node++;
                Assemble(ptr->NODE.BinOp.right);
                fprintf(file_to_write,"\tmov ebx, eax\n");
            }

            // If the left and right node dont have any operator
            if(ptr->NODE.BinOp.left->TYPE == CONSTANT_NODE || ptr->NODE.BinOp.left->TYPE == VARIABLE_NODE){
                fprintf(file_to_write,"\tmov eax, ");
                Assemble(ptr->NODE.BinOp.left);
                fprintf(file_to_write,"\n");
            }
            if(ptr->NODE.BinOp.right->TYPE == CONSTANT_NODE || ptr->NODE.BinOp.right->TYPE == VARIABLE_NODE){
                fprintf(file_to_write,"\tmov ebx, ");
                Assemble(ptr->NODE.BinOp.right);
                fprintf(file_to_write,"\n");
            }
            // If the node have binary operator on both the child nodes;
            if(op_node == 2){
                op_node == 0;
                fprintf(file_to_write,"\tmov eax, ecx\n");
            }

            // compares the operator
            switch(ptr->NODE.BinOp.op){
                case PLUS:
                    fprintf(file_to_write,"\tadd eax, ebx\n");
                    break;           
                case MINUS:  
                    fprintf(file_to_write,"\tsub eax, ebx\n");        
                    break;           
                case MULT:
                    fprintf(file_to_write,"\timul eax, ebx\n");           
                    break;           
                case DIVIDE:
                    fprintf(file_to_write,"\tidiv eax, ebx\n");   
                    break;           
            }
            break;

        case RELOP_NODE:
            if(ptr->NODE.RelOp.left->TYPE == CONSTANT_NODE || ptr->NODE.RelOp.left->TYPE == VARIABLE_NODE){
                fprintf(file_to_write,"\tmov eax, ");
                Assemble(ptr->NODE.RelOp.left);
            }
            fprintf(file_to_write,"\n");

            if(ptr->NODE.RelOp.right->TYPE == CONSTANT_NODE || ptr->NODE.RelOp.right->TYPE == VARIABLE_NODE){
                fprintf(file_to_write,"\tmov ebx, ");
                Assemble(ptr->NODE.RelOp.right);
            }
            fprintf(file_to_write,"\n");
            switch (ptr->NODE.RelOp.op){
                case EQ:
                    fprintf(file_to_write, "\tcmp eax, ebx\n");
                    fprintf(file_to_write, "\tsete al\n");
                    break;
                case NE:
                    fprintf(file_to_write, "\tcmp eax, ebx\n");
                    fprintf(file_to_write, "\tsetne al\n");
                    break;
                case LT:
                    fprintf(file_to_write, "\tcmp eax, ebx\n");
                    fprintf(file_to_write, "\tsetl al\n");
                    break;
                case LE:
                    fprintf(file_to_write, "\tcmp eax, ebx\n");
                    fprintf(file_to_write, "\tsetle al\n");
                    break;
                case GT:
                    fprintf(file_to_write, "\tcmp eax, ebx\n");
                    fprintf(file_to_write, "\tsetg al\n");
                    break;
                case GE:
                    fprintf(file_to_write, "\tcmp eax, ebx\n");
                    fprintf(file_to_write, "\tsetge al\n");
                    break;
            }
            break;

        case IF_NODE:
            // Evaluate condition expression and jump to else_block if it evaluates to false
            counter++;
            Assemble(ptr->NODE.IfBlock.cond_block);
            fprintf(file_to_write, "\tcmp eax, 1\n");   
            fprintf(file_to_write, "\tjne _L%d\n",counter);

            // If condition is true, evaluate statements
            Assemble(ptr->NODE.IfBlock.stmnt_block);

            // If condition is false, evaluate else block
            if (ptr->NODE.IfBlock.else_block){
                fprintf(file_to_write,"\tjmp _L%d\n",counter+1);
                fprintf(file_to_write, "_L%d:\n",counter);
                Assemble(ptr->NODE.IfBlock.else_block);
                fprintf(file_to_write, "_L%d:\n",counter+1);
            }
            else{
                fprintf(file_to_write, "_L%d:\n",counter);
            }
            break;

        case WHILE_NODE:
            Assemble(ptr->NODE.WhileLoop.cond_block);
            break;

        case FOR_NODE:
            counter++;
            if(ptr->NODE.ForLoop.init_block){
                Assemble(ptr->NODE.ForLoop.init_block);
            }
            if(ptr->NODE.ForLoop.cond_block){
                Assemble(ptr->NODE.ForLoop.cond_block);
                fprintf(file_to_write, "\tcmp eax, 1\n");   
                fprintf(file_to_write, "\tjne _L%d\n",counter);
            }
            fprintf(file_to_write,"_L%d:\n",counter);
            Assemble(ptr->NODE.ForLoop.body);
            if(ptr->NODE.ForLoop.inc_block){
                Assemble(ptr->NODE.ForLoop.inc_block);
            }
            fprintf(file_to_write, "\tje _L%d\n",counter);
            break;


        case CONSTANT_NODE:
            fprintf(file_to_write,"%d",ptr->NODE.Const.Tok_Value);
            break;
        case RETURN_NODE:
            Assemble(ptr->NODE.Return.val);
            break;
        case VARIABLE_NODE:
            temp = GetHash(GetGlobal(CURR_FUNC2->NODE.Func.name)->LOCAL_TABLE,
                                ptr->NODE.Variable.Tok_Value);
            if(temp->OFFSET != 0){
                fprintf(file_to_write,"DWORD PTR [ebp-%d]",temp->OFFSET);
            }
            break;
        default:
            break;
    }
    if(ptr->SIBLING){
        Assemble(ptr->SIBLING);
    }
}

void BUILD(AST_Node *ptr){
    strcpy(ASM_FILE_NAME,ptr->NODE.Prog.head);
    strcat(ASM_FILE_NAME,".asm");
    //printf("File Name: %s\n",ASM_FILE_NAME);

    file_to_write = fopen(ASM_FILE_NAME, "w");
    Assemble(ptr);

    printf(GRN "[ASSEMBLY DONE]\n\n" RESET);
    //Assemble(CURR_NODE->SIBLING);
}