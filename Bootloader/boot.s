[org 0x7C00]
use16

xor ax, ax

mov es, ax
mov ds, ax

cli
mov bp, 0x7C00
mov ss, ax
mov sp, bp
sti

mov ax, 0x0100
mov es, ax
xor bx, bx

mov ah, 0x02
mov al, 0x04
mov ch, 0x00
mov cl, 0x02
mov dh, 0x00
mov dl, 0x80
int 0x13
jc failed

mov ax, 0x0900
mov es, ax
xor bx, bx

mov ah, 0x02
mov al, 0x06
mov ch, 0x00
mov cl, 0x06
mov dh, 0x00
mov dl, 0x80
int 0x13
jc failed

mov ax, 0x0D00
mov es, ax
xor bx, bx

mov ah, 0x02
mov al, 0x02
mov ch, 0x00
mov cl, 0x0A
mov dh, 0x00
mov dl, 0x80
int 0x13
jc failed

mov ax, 0x0F00
mov es, ax
xor bx, bx

mov ah, 0x02
mov al, 0x10
mov ch, 0x00
mov cl, 0x0C
mov dh, 0x00
mov dl, 0x80
int 0x13
jc failed

mov ax, 0x03
int 0x10

jmp 0x0:0x1000

jmp $

print:
	mov ah, 0x0e
.ploop:
	mov al, [si]
	cmp al, 0x0
	je .end

	int 0x10
	inc si
	jmp .ploop
.end:
	ret

failed:
	mov si, failed_msg
	call print

	cli
	hlt

failed_msg: db 'Failed to read from disk', 0x0

times 510 - ($ - $$) db 0
dw 0xAA55
