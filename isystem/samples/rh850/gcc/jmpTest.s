.include "gpr_init.s"

.text


	mov 0x2, r1
	mov 0xc, r3
	mov 0xaabb, r4
	callt 0x2
	st.b r3, 0xab[r1]
	JARL [R4], R10
	mov 0x8, r5
	
	add r1, r5
	add r2, r5
	ROTL_i 0x8, R3, R7
	ROTL_r R2, R4, R5
	jmp r4
	
	add r3, r5
	add r4, r5	
	add r5, r5


.org 0xaabb

	
	ld.b 0xab[r1], r10
	mov 0x2, r3
	LD.DW 0x2, R1, R4

SELF:
   	br SELF
