.model tiny
.code
org 100h
start:
; BEGIN BODY
	; Example 2: ADD, SUB, INC, DEC, NEG, CMP

	; ADD
	add al, ah
	add ah, al
	add ax, ax
	add cx, dx
	add al, [0]
	add cl, [100]
	add dl, [69]

	; SUB
	sub al, ah
	sub ah, al
	sub ax, ax
	sub cx, dx
	sub al, 0
	sub cl, 110
	sub dl, 49
	
	; INC DEC
	inc ax
	inc cx
	dec ax
	dec sp

	; NEG
	neg ax 
	neg bx
	neg al
	neg bl

	; CMP
	cmp al, ah
	cmp ch, bh
	cmp ax, dx
	cmp dx, dx
	cmp al, 5
	cmp dh, 5

; END BODY
end start