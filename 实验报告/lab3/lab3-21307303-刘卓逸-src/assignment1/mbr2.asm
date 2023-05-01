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

mov ax, 1                ; 逻辑扇区号第0~15位
mov cx, 0                ; 逻辑扇区号第16~31位
mov bx, 0x7e00           ; bootloader的加载地址
call asm_read_hard_disk  ; 读取硬盘

jmp 0x0000:0x7e00        ; 跳转到bootloader

jmp $ ; 死循环

asm_read_hard_disk:  
;ch 柱面C / cl 扇区S /  dh 磁头H / dl 驱动器
;CS=0 HS=0 SS=1 PS=63 PH=18
	and eax, 0x0000ffff
	and ecx, 0x00001fff
	;mov ch, 0x00 ; test 柱面号  1/(63*18)+0=0
	push eax
	push ecx
	mov edx, 0
	mov ecx, 18*63
	idiv ecx
	pop ecx
	mov ch,al
	pop eax
	;mov dh, 0x00 ; test 磁头 1/63-(0-0)*18+0=0
	push eax
	push ecx
	mov edx, 0
	mov ecx, 63
	idiv ecx
	pop ecx
	push ecx
	shr ecx, 8
	sub ecx, 0
	imul ecx, ecx, 18
	sub eax, ecx
	pop ecx
	add eax, 0
	mov dh, al
	pop eax
	;mov cl, 0x02 ; test 扇区 1-(0-0)*63*18+(0-0)*63+1=2
	push eax
	push ecx
		shr ecx, 8
		imul ecx, ecx, 63*18
		sub eax, ecx
	pop ecx
	push edx
		shr edx, 8
		imul edx, edx, 63
		sub eax, edx
	pop edx
	add eax,1
	mov cl, al
	pop eax
	mov dl, 0x80 ; 硬盘
	mov ah, 0x02 ; 中断功能:读扇区
	mov al, 0x05 ; 读5个扇区
	int 13H
   ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa
