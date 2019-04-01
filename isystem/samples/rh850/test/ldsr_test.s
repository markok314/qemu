.text
.include "RH850G3M_insts.s"
.include "gpr_init.s"

	LDSR_ID R1,0,0
	ldsr r1,0

	LDSR_ID R2,1,0
	ldsr r2,1

	LDSR_ID R3,2,0
	ldsr r3,2

	LDSR_ID R4,3,0
	ldsr r4,3

	LDSR_ID R5,5,0
	ldsr r5,5

	LDSR_ID R6,6,0
	ldsr r6,6

	LDSR_ID R7,7,0
	ldsr r7,7

	LDSR_ID R8,8,0
	ldsr r8,8

	LDSR_ID R9,9,0
	ldsr r9,9

	LDSR_ID R1,10,0
	ldsr r1,10

	LDSR_ID R2,11,0
	ldsr r2,11

	LDSR_ID R3,13,0
	ldsr r3,13

	LDSR_ID R4,14,0
	ldsr r4,14

	LDSR_ID R5,16,0
	ldsr r5,16

	LDSR_ID R6,17,0	
	ldsr r6,17

	LDSR_ID R7,20,0
	ldsr r7,20

	LDSR_ID R8,28,0
	ldsr r8,28

	LDSR_ID R9,29,0
	ldsr r9,29

	LDSR_ID R1,31,0
	ldsr r1,31

	LDSR_ID R2,0,1
	LDSR_ID R3,2,1
	LDSR_ID R4,3,1
	LDSR_ID R5,4,1
	LDSR_ID R6,5,1
	LDSR_ID R7,6,1
	LDSR_ID R8,11,1
	LDSR_ID R9,12,1
	LDSR_ID R1,0,2
	LDSR_ID R2,6,2
	LDSR_ID R3,7,2
	LDSR_ID R4,8,2
