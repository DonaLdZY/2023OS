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
mov sp, 0x7c00
mov ax, 0xb800
mov gs, ax

loop:
	mov ah, 00h
	int 16h
	mov ah, 0Eh
	int 10h
jmp loop
times 510 - ($ - $$) db 0
db 0x55, 0xaa
