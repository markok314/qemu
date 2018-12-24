.include "RH850G3M_insts.s"
.include "gpr_init.s"

.text
	mov 0xffe01000, r5
	mov 0xffe10000, r6

	LDL_W R6, R4
	LDL_W R5, R9
	
Lbl:
	br Lbl


