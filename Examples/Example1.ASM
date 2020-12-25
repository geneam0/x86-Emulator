.model tiny
.code
org 100h
start:
; BEGIN BODY

	; Example 1: NOP, MOV, XCHG

	; NOP
	nop

	; MOV register to register
	mov ah, al
	mov bh, bl
	mov cl, ah
	mov dl, ah
	mov ax, bx
	mov bx, ax
	mov bx, bx

	; MOV immediate to register (NOTE: Emulator cannot differentiate between these datatypes)
	mov al, [12]
	mov ch, [100]
	mov dl, [200]
	mov bl, [46]
	mov al, 97
	mov ch, 'a'
	mov dl, 61h
	mov bl, 01100001b

	; XCHG register to register
	xchg al, ah
	xchg al, bl
	xchg ax, bx
	xchg bx, ax
	xchg cx, cx

; END BODY
end start