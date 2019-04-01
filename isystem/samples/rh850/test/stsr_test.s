.text
.include "RH850G3M_insts.s"
.include "gpr_init.s"

	STSR_ID 0,R1,0
	stsr 0,r1
	STSR_ID 1,R2,0
	stsr 1,r1
	STSR_ID 2,R3,0
	stsr 2,r1
	STSR_ID 3,R4,0
	stsr 3,r1
	STSR_ID 5,R5,0
	stsr 5,r1
#	STSR_ID 6,R6,0
#	stsr 6,r1
#	STSR_ID 7,R7,0
#	stsr 7,r1
#	STSR_ID 8,R8,0
#	stsr 8,r1
#	STSR_ID 9,R9,0
#	stsr 9,r1
#	STSR_ID 10,R1,0
#	stsr 10,r1
#	STSR_ID 11,R2,0
#	stsr 11,r1
	STSR_ID 13,R3,0
	stsr 13,r1
	STSR_ID 14,R4,0
	stsr 14,r1
	STSR_ID 16,R5,0
	stsr 16,r1
	STSR_ID 17,R6,0	
	stsr 17,r1
	STSR_ID 20,R7,0
	stsr 20,r1
	STSR_ID 28,R8,0
	stsr 28,r1
	STSR_ID 29,R9,0
	stsr 29,r1
	STSR_ID 31,R1,0
	stsr 31,r1
	STSR_ID 0,R2,1
	STSR_ID 2,R3,1
	STSR_ID 3,R4,1
	STSR_ID 4,R5,1
	STSR_ID 5,R6,1
	STSR_ID 6,R7,1
	STSR_ID 11,R8,1
	STSR_ID 12,R9,1
	STSR_ID 0,R1,2
	STSR_ID 6,R2,2
	STSR_ID 7,R3,2
	STSR_ID 8,R4,2

lbl1:
    br lbl1

