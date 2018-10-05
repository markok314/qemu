.include "RH850G3M_insts.s"

.text

	mov 1, R1
	mov 5, R2
	add R1, R2
	add R2, R2
	mov 4, R3
	#jmp R3
	mov 5, R2
	LOOP R1, -8
	add R2, R2
	add R2, R2
	add R2, R2
