.text
.include "RH850G3M_insts.s"
	
	mov 0x00ff00ff,r1
	mov 0x12345678,r2

	BINS1 R1,8,8,R2
