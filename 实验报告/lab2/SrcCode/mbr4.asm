org 0x7c00
[bits 16]
xor ax, ax ; eax = 0
; 初始化段寄存器, 段地址全部设为0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax
; 初始化栈指针
mov sp, 0x7c00 ;line10
mov ax, 0xb800
mov gs, ax
;清屏
mov ah, 0x07 ; 黑底白字
mov al, ' '
mov ebx, 0
loop1:
	cmp ebx, 3200
	jge endloop1
	mov [gs:2*ebx], ax ;line20
	add ebx, 1
	jmp loop1
endloop1:;输出学号
mov ah, 0x70 ;白底黑字
mov al, '2'
mov [gs:72], ax
mov al, '1'
mov [gs:74], ax
mov al, '3'
mov [gs:76], ax
mov al, '0' ;line30
mov [gs:78], ax
mov al, '7'
mov [gs:80], ax
mov al, '3'
mov [gs:82], ax
mov al, '0'
mov [gs:84], ax
mov al, '3'
mov [gs:86], ax
;line40
mov bl, 2
mov bh, 2
mov cl, 1
mov ch, 1
loop:
	push bx
	mov dh, 0
	mov dl, bh
	imul dx, 80
	mov bh, 0
	add dx, bx
	pop bx
	mov [gs:2*edx], ax ;line 50

	add bl, cl
	cmp bl, 0
	jle branch1
	cmp bl, 79
	jge branch1
	jmp end1
	branch1
		mov edx, 0
		sub dl, cl
		mov cl, dl
	end1:

	add bh, ch
	cmp bh, 2
	jle branch2
	cmp bh, 24
	jge branch2
	jmp end2
	branch2
		mov edx, 0
		sub dl, ch
		mov ch, dl
	end2:

	add ah, 1
	add al, 1
	cmp al, 58
	jl end3
		mov al, 48
	end3:
	
	mov edx,0
	loop3:
		cmp edx, 11451
		jg endloop3
		pushad
			mov ah, 03h
			mov bx, 0
			int 10h
		popad
		add edx, 1
		jmp loop3
	endloop3:
jmp loop
times 510 - ($ - $$) db 0
db 0x55, 0xaa
