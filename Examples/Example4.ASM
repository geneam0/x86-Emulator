.model tiny
.code
org 100h
start:
; BEGIN BODY

	; Example 4: Shift and Rotate Operators, Multiplication and Division, Jump
	
	; Shift and Rotate Operators (SHL, SHR, SAR)
	SHL al, 2
	SHR cx, 1
	SAR dl, 1
	ROL bh, 1
	ROR ch, 1
	RCL dh, 1
	RCR bx, 1
	
	; Multiplication
	IMUL AL
	IMUL BH
	IMUL AX
	IMUL BX
	MUL AL
	MUL BH
	MUL AX
	MUL BX

	; Division
	IDIV AL
	IDIV BH
	IDIV AX
	IDIV BX
	DIV AL
	DIV BH
	DIV AX
	DIV BX

	; Jump
	
	;JMP lab
	JC lab
    JNC lab
    JZ lab
    JNZ lab
    JBE lab
    JNBE lab
    JS lab
    JNS lab
    JLE lab

	lab: 
	mov al, 100

; END BODY
end start