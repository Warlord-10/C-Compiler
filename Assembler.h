

/*
# end string with 0h to depict null byte
# to get a newline use 0Ah



section .text
    .globl _start
_start:
    mov ebx, 123    ; move 123 to ebx
    mov eax, ebx    ; move ebx to eax
    add ebx, ecx    ; ebx += ecx
    sub ebx, edx    ; ebx -= edx
    mul ebx         ; eax *= ebx 
    div edx         ; eax /= edx


    mov eax, 4      ; sys_write system call
    mov ebx, 1      ; stdout file descriptor (write to stdout)
    mov ecx, msg    ; bytes to write (string to print)
    mov edx, len    ; number of bytes to write

    int 0x80        ; perform system call
    mov eax, 1      ; sys_exit system call
    mov ebx, 0      ; exit status is 0
    int 0x80        ; it performs interrupt and the type of interrupt will be decided by eax


    jmp skip        ; jump to skip label
skip:
    cmp ecx, 99     ; compare ecx to 99
    jl skip         ; jump if less than

section .data


*/


/*
    RETURN: 
        mov eax, value
        ret
    UNOP:
        mov eax, value
        neg eax
    BINOP:
        ADD:
            mov eax, value1
            push eax
            mov eax, value2
            pop ecx
            add eax, ecx
        SUB:
            mov ecx, value1
            push ecx
            mov ecx, value2
            pop eax
            sub eax, ecx
        MUL:
            mov eax, value1
            mov ebx, value2
            mul ebx
        DIV:
            mov eax, value1
            mov ebx, value2
            div ebx
    RELOP:
        
*/

#ifndef Assembler
#define Assembler

void Assemble(AST_Node *ptr);

void BUILD(AST_Node *ptr);

#endif
