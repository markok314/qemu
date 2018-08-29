.data
lbl1: .word 57
lbl2: .byte 89

.text
	mov 0x7fffffff,r1
	mov 0x1,r2
	add r1,r2
	
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	
Lbl: 
	br Lbl

Tst:
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	

