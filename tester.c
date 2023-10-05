#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Base.h"
#include "Lexer.h"
#include "Parser.h"
#include "Assembler.h"

/* 26/5/2023:   all good no bugs */
/* 27/5/2023:   added single and multiline comments
                added global error counter
                */
/* 28/5/2023:   added pointer to function in global table for function call*/
/* 4/6/2023:    added function declaration*/
/* 5/6/2023:    added unary operator and function call*/
/* 7/6/2023:    updated printer for function call and function decl
                added hash table support for parameters*/
/* 8/6/2023:    added do while loop, keywords */
/* future: add error_no variable, universal hash table*/

int main(){

    Set_Up();   // Inital call

    printf("\nCOMPILER\n");
    printf("BASE_TOKEN addr: %d \n", BASE_TOKEN);
    printf("BASE_AST_NODE addr: %d \n", BASE_AST_NODE);
    printf("GLOBAL_TABLE addr: %d \n\n", GLOBAL_TABLE_HEAD);

    //Settings:
    int lexer = 1;
    int parser = 1;
    int assembler = 1;
    int show_lexer = 1;
    int show_parser = 1;
    int show_table = 1;


    TOKENIZER();
    //show_tokens(BASE_TOKEN);

    PARSER();
    if(ERRORS == 0){
        show_tree(0,BASE_AST_NODE);
        //BUILD(BASE_AST_NODE);
        printf(GRN "\nProgram ended with %d errors.\n" RESET,ERRORS);
        ShowGlobal();
    }
    else{
        printf(RED "\nProgram terminated with %d error(s).\n" RESET,ERRORS);
    }
    
    printf("Press any key to exit....");
    getchar();
    return 0;
}

/*
clock_t t;
t = clock();
t = clock() - t;
double time_taken = ((double)t)/CLOCKS_PER_SEC;
printf("took %f seconds to execute \n", time_taken);

gcc -o Compiler.exe Base.c Lexer.c Parser.c Assembler.c tester.c
./Compiler.exe
*/
