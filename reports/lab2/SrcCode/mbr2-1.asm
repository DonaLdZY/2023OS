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

;设置光标位置
mov ah, 0x02
mov bh, 0
mov dh, 8
mov dl, 8
int 10h

;读取光标位置
mov ah, 0x03
mov bx, 0
int 10h

;输出光标位置
mov bl, 70H ;颜色，白底黑字
mov cx, 1

mov al, '('
mov ah, 0x0E
int 10h

mov al, dh
add al, 48
mov ah, 0x0E
int 10h

mov al, ','
mov ah, 0x0E
int 10h

mov al, dl
add al, 48
mov ah, 0x0E
int 10h

mov al, ')'
mov ah, 0x0E
int 10h

;重设光标位置
mov ah, 0x02
mov bh, 0
mov dh, 8
mov dl, 8
int 10h

jmp $ ; 死循环
times 510 - ($ - $$) db 0
db 0x55, 0xaa
