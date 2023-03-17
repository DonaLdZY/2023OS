%include "head.include"

your_if:
	pushad
	mov eax, [a1]
	cmp eax, 12  
	jl branch1
	cmp eax, 24
	jl branch2
	jmp branch3
	branch1:
		mov edx, 0
		mov ebx, 2
		idiv ebx
		add eax, 1
		mov [if_flag], eax
	jmp endif
	branch2:
		mov ebx, 24
		sub ebx, eax
		imul eax, ebx
		mov [if_flag], eax
	jmp endif
	branch3:
		shl eax, 4
		mov [if_flag], eax 
	endif:
	popad

your_while:
	pushad
	loop:
	mov ecx, [while_flag]
	mov ebx, [a2]
	cmp ebx, 12
	jl endloop
		push ebx 
		push ecx
		call my_random
		pop ecx
		pop ebx
		sub ebx, 12
		mov [ecx+ebx], al
		add ebx, 11
		mov [a2], ebx
	jmp loop
	endloop:
	popad

%include "end.include"

your_function:
	pushad
	mov ebx, [your_string]
	mov eax, 0
	loop2:
	mov ecx, [ebx+eax]
	cmp cl, 0
	je endloop2
		pushad
			push ecx 
			call print_a_char
			pop ecx
		popad
		add eax, 1 
	jmp loop2
	endloop2:
	popad
