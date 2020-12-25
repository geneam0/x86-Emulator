.model tiny
.code
org 100h
start:
; BEGIN BODY
	; Example 3: AND, TEST, OR, XOR, NOT

	; AND
	and al, ah
	and ah, al
	and ax, ax
	and cx, dx
	and al, [0]
	and cl, [100] 
	and dl, [69]

	; TEST
	test ah, al
	test ax, ax
	test cx, dx
	test al, [0]
	test cl, [100]
	
	; OR
	or ah, al
	or ax, ax
	or cx, dx
	or al, [0]
	or cl, [100]
	or dl, [69]

	; XOR
	xor ah, al
	xor ax, ax
	xor cx, dx
	xor al, [0]
	xor cl, [100]
	xor dl, [69]

	; NOT
	not al
	not dl
	not bh
	not ax
	not bx

; END BODY
end start