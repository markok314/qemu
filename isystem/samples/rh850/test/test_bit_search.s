.text
.include "gpr_init.s"

	#test for SCH0L
	mov 0xffffefff,r1
	sch0l r1,r2
	#store position of 0 in r2
	mov 0x00000000,r1
	sch0l r1,r2
	#store 1 in r2
	mov 0xffffffff,r1
	sch0l r1,r2
	#store 0 in r2, Z flag 1
	mov 0xfffffffe,r1
	sch0l r1,r2
	#store 32 in r2, CY flag 1

	#test for SCH0R
	mov 0xffffefff,r1
	sch0r r1,r2
	#store position of 0 in r2
	mov 0xffffffff,r1
	sch0r r1,r2
	#store 0 in r2, Z flag 1
	mov 0x7fffffff,r1
	sch0r r1,r2
	#store 1 in r2, CY flag 1

	#test for SCH1L
	mov 0x00008000,r1
	sch0l r1,r2
	#store position of 1 in r2
	mov 0x00000000,r1
	sch0l r1,r2
	#store position 0 in r2, Z flag 1
	mov 0x00000001,r1
	sch0l r1,r2
	#store position 32 in r2, CY flag 1

	#test for SCH1R
	mov 0x00008000,r1
	sch0l r1,r2
	#store position of 1 in r2
	mov 0x00000000,r1
	sch0l r1,r2
	#store position 0 in r2, Z flag 1
	mov 0x80000000,r1
	sch0l r1,r2
	#store position 1 in r2, CY flag 1
	
	Lbl: br Lbl
	

	
