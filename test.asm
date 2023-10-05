#include	'stdfunc.asm'

SECTION .text
global _start

case1:
	push ebp
	mov ebp, esp

	sub esp, 4
	mov DWORD PTR [ebp-4], 9
	sub esp, 4
	mov DWORD PTR [ebp-8], 8
	mov eax, DWORD PTR [ebp-8]
	mov ebx, 19
	cmp eax, ebx
	setl al
	cmp eax, 1
	jne _L1
_L1:
	sub esp, 4
	mov DWORD PTR [ebp-12], 190
	mov DWORD PTR [ebp-12], 289
	mov eax, DWORD PTR [ebp-8]
	mov ebx, 1
	add eax, ebx
	mov DWORD PTR [ebp-8], eax
	je _L1
	mov esp, ebp
	pop ebp
	ret

case2:
	push ebp
	mov ebp, esp

	sub esp, 4
	mov ecx, 
	mov DWORD PTR [ebp-4], ecx
	mov eax, DWORD PTR [ebp-4]
	mov ebx, 10
	cmp eax, ebx
	sete al
	cmp eax, 1
	jne _L2
	sub esp, 4
	mov DWORD PTR [ebp-8], 0
	mov eax, DWORD PTR [ebp-8]
	mov ebx, 10
	cmp eax, ebx
	setl al
	cmp eax, 1
	jne _L3
_L3:
	mov eax, DWORD PTR [ebp-4]
	mov ebx, 6
	add eax, ebx
	mov DWORD PTR [ebp-4], eax
	mov eax, DWORD PTR [ebp-8]
	mov ebx, 3
	cmp eax, ebx
	sete al
	cmp eax, 1
	jne _L4
_L4:
	mov eax, DWORD PTR [ebp-8]
	mov ebx, 1
	add eax, ebx
	mov DWORD PTR [ebp-8], eax
	je _L4
_L4:
	sub esp, 4
	mov eax, DWORD PTR [ebp-4]
	mov ebx, 90
	mov DWORD PTR [ebp-12], eax
DWORD PTR [ebp-12]	mov esp, ebp
	pop ebp
	ret

case3:
	push ebp
	mov ebp, esp

	sub esp, 4
	mov DWORD PTR [ebp-4], 10
	sub esp, 4
	mov eax, DWORD PTR [ebp-4]
	mov ebx, 10
	add eax, ebx
	mov DWORD PTR [ebp-8], eax
	mov eax, DWORD PTR [ebp-4]
	mov ebx, DWORD PTR [ebp-8]
	cmp eax, ebx
	sete al
	cmp eax, 1
	jne _L5
	mov DWORD PTR [ebp-4], 10
_L5:
	mov esp, ebp
	pop ebp
	ret

case4:
	push ebp
	mov ebp, esp

	mov esp, ebp
	pop ebp
	ret

case4:
	push ebp
	mov ebp, esp

	sub esp, 4
	mov DWORD PTR [ebp-4], eax
	mov esp, ebp
	pop ebp
	ret

;Code Generated