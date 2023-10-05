#include <stdio.h>
#include <stdlib.h>     //malloc, free
#include <ctype.h>      //isdigit, isalpha
#include <string.h> 
#include "Base.h"
#include "Lexer.h"


// It creates string tokens
void Create_token(char x[],Token* t){
    if(strcmp(x,"return")==0){
        t->type = KW_RETURN;
    }
    else if(strcmp(x,"main")==0){
        t->type = KW_MAIN;
        strcpy(t->value,x);
    }
    else if(strcmp(x,"if")==0){
        t->type = KW_IF;
    }
    else if(strcmp(x,"else")==0){
        t->type = KW_ELSE;
    }
    else if(strcmp(x,"for")==0){
        t->type = KW_FOR;
    }
    else if(strcmp(x,"while")==0){
        t->type = KW_WHILE;
    }
    else if(strcmp(x,"break")==0){
        t->type = KW_BREAK;
    }
    else if(strcmp(x,"continue")==0){
        t->type = KW_CONTINUE;
    }
    else if(strcmp(x,"int")==0){
        t->type = DATA_TYPE;
    }
    else if(strcmp(x,"void")==0){
        t->type = DATA_TYPE;
    }
    else{
        t->type = ID;
        strcpy(t->value,x);
    }
}


// Actual Tokenizer
void TOKENIZER(){

    FILE *file_to_read;
    file_to_read = fopen(FILE_NAME,"r");

    Token* Tok = BASE_TOKEN;
    char ch;    // holds the character
    char curr_word[32] = "";    // current word formed
    short int int_flag = 0, char_flag = 0;  // flags for string and integers
    short int single_line_comment = 0, multi_line_comment = 0;  // comment flags
    int ch_no=0, ln_no=1, word_length=0;    // char_no, line_no and word length


    while(!feof(file_to_read)){
        ch = fgetc(file_to_read);
        ch_no++;
        //printf("%c ",ch);

        if(single_line_comment == 1){
            while(ch != '\n'){
                ch = fgetc(file_to_read);
            }
            printf("%c",ch);
            single_line_comment = 0;
        }
        if(multi_line_comment == 1){
            printf("%s: ",curr_word);
            if(ch == '*'){
                if(fgetc(file_to_read) == '/'){
                    multi_line_comment = 0;
                }
            }
            continue;
            
        }


        //updating the current word
        if(isalpha(ch)){
            strncat(curr_word,&ch,1);
            char_flag = 1;
            if(word_length == 0){
                word_length = ch_no;
            }
        }
        else if(isdigit(ch)){
            strncat(curr_word,&ch,1);
            int_flag = 1;
            if(word_length == 0){
                word_length = ch_no;
            }
        }
        // this block will only execute when the lexer will read an operator
        else{
            // first evaluate the word which formed before the occurance of operator
            Token* t = GiveTok();
            t->character = word_length;
            word_length = 0;
            t->line = ln_no;
            Tok->next = t;
            if(int_flag==1 && char_flag==0){
                t->type = CONST;    // Constant Integer Token
                strcpy(t->value,curr_word);
                Tok = Tok->next;
            }
            else if(char_flag==1 || int_flag==1){
                Create_token(curr_word,t);  // Identifier Token
                Tok = Tok->next;
            }
            else{
                Tok->next = NULL;
                free(t);
            }
            strcpy(curr_word,"");
            char_flag = 0;
            int_flag = 0;

            // now evaluate the operator itself
            Token* tt = GiveTok();
            tt->line = ln_no;
            tt->character = ch_no;
            Tok->next = tt;

            switch(ch){
                case '(':
                    tt->type = L_PAREN;
                    strcpy(tt->value,"(");
                    Tok = Tok->next;
                    break;
                case ')':
                    tt->type = R_PAREN;
                    strcpy(tt->value,")");
                    Tok = Tok->next;
                    break;
                case '{':
                    tt->type = L_BRACE;
                    strcpy(tt->value,"{");
                    Tok = Tok->next;
                    break;
                case '}':
                    tt->type = R_BRACE;
                    strcpy(tt->value,"}");
                    Tok = Tok->next;
                    break;
                case ';':
                    tt->type = S_COLON;
                    strcpy(tt->value,";");
                    Tok = Tok->next;
                    break;
                case ',':
                    tt->type = COMMA;
                    strcpy(tt->value,",");
                    Tok = Tok->next;
                    break;
                case '&':
                    fgetc(file_to_read);
                    tt->type = LOG_AND;
                    strcpy(tt->value,"&&");
                    Tok = Tok->next;
                    break;
                case '|':
                    fgetc(file_to_read);
                    tt->type = LOG_OR;
                    strcpy(tt->value,"||");
                    Tok = Tok->next;
                    break;
                case '=':
                    if(fgetc(file_to_read) == '='){
                        tt->type = EQ;
                        strcpy(tt->value,"==");
                    }
                    else{
                        fseek(file_to_read,-1,SEEK_CUR);
                        tt->type = ASN_OP;
                        strcpy(tt->value,"=");
                    }
                    Tok = Tok->next;
                    break;
                case '!':
                    if(fgetc(file_to_read) == '='){
                        tt->type = NE;
                        strcpy(tt->value,"!=");
                    }
                    else{
                        fseek(file_to_read,-1,SEEK_CUR);
                        tt->type = LOG_NOT;
                        strcpy(tt->value,"!");
                    }
                    Tok = Tok->next;
                    break;
                case '>':
                    if(fgetc(file_to_read) == '='){
                        tt->type = GE;
                        strcpy(tt->value,">=");
                    }
                    else{
                        fseek(file_to_read,-1,SEEK_CUR);
                        tt->type = GT;
                        strcpy(tt->value,">");
                    }
                    Tok = Tok->next;
                    break;
                case '<':
                    if(fgetc(file_to_read) == '='){
                        tt->type = LE;
                        strcpy(tt->value,"<=");
                    }
                    else{
                        fseek(file_to_read,-1,SEEK_CUR);
                        tt->type = LT;
                        strcpy(tt->value,"<");
                    }
                    Tok = Tok->next;
                    break;
                case '+':
                    tt->type = PLUS;
                    strcpy(tt->value,"+");
                    Tok = Tok->next;
                    break;
                case '-':
                    tt->type = MINUS;
                    strcpy(tt->value,"-");
                    Tok = Tok->next;
                    break;
                case '*':
                    tt->type = MULT;
                    strcpy(tt->value,"*");
                    Tok = Tok->next;
                    break;
                case '/':
                    char temp = fgetc(file_to_read);
                    if(temp == '/'){
                        single_line_comment = 1;
                        Tok->next = NULL;
                        free(tt);
                    }
                    else if(temp == '*'){
                        multi_line_comment = 1;
                        Tok->next = NULL;
                        free(tt);
                    }
                    else{
                        fseek(file_to_read,-1,SEEK_CUR);
                        tt->type = DIVIDE;
                        strcpy(tt->value,"/");
                        Tok = Tok->next;
                    }
                    break;
                case '?':
                    tt->type = QUES_MARK;
                    strcpy(tt->value,"?");
                    Tok = Tok->next;
                    break;
                case ':':
                    tt->type = COLON;
                    strcpy(tt->value,":");
                    Tok = Tok->next;
                    break;
                case '\n':
                    ln_no++;
                    ch_no = 0;
                    Tok->next = NULL;
                    free(tt);
                    break;
                case ' ':
                default:
                    Tok->next = NULL;
                    free(tt);
                    break;
            }
        }  
    }
    fclose(file_to_read);
    printf(GRN "[LEXER ENDS] \n\n" RESET);
}