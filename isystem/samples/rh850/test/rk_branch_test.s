.include "RH850G3M_insts.s"
.include "gpr_init.s"


	mov hilo(Lbl), r1
	mov 0x0000baba, r2
	mov 0x400, r3
	mov 0x800, r4
	mov 0x5, r10
	mov 0x1, r11
	mov 0xbaba, r16
	
	mov 0x3, r15
	
	JARL_RR R4, R9
	

.org 0x00000400

	mov 0xffffffff, r6
	sub r11, r10		#5 iterations
	ble lab2		#loop to 0x800

	
	JARL_RR R1, R5
	
	
lab2:

	mov 0xabcdef, r18
	mov 0x5, r10
loop1:
	sub r11, r15
	bne loop1
	JARL_RR R3, R5 

.org 0x00000800

	mov 0xf, r20
	jmp [r16]
	

.org 0x0000baba

	mov 0xffffffff, r5
	
	
	JARL_RR R3, R5
	mov 0xf, r20




Lbl:	br Lbl
