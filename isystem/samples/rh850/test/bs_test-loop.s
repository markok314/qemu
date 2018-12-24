.include "RH850G3M_insts.s"

#-------LOOP---------------------------------------- OK

	mov 1, R4
	mov 3, R1
	mov 5, R2
	add R1, R2
	sub R4, R1
	loop R1, 6
	mov 4, R2
	add R2, R5
	sub R4, R2
	loop R2, 4
	mov 3, R2
	mov 4, R2

Lbl:	br Lbl
