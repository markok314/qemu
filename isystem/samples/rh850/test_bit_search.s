
   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:

	;reseting all registers to 0
	mov 0x0,r1
	mov 0x0,r2
	mov 0x0,r3
	mov 0x0,r4
	mov 0x0,r5
	mov 0x0,r6
	mov 0x0,r7
	mov 0x0,r8
	mov 0x0,r9
	mov 0x0,r10
	mov 0x0,r11
	mov 0x0,r12
	mov 0x0,r13
	mov 0x0,r14
	mov 0x0,r15
	mov 0x0,r16
	mov 0x0,r17
	mov 0x0,r18
	mov 0x0,r19
	mov 0x0,r20
	mov 0x0,r21
	mov 0x0,r22
	mov 0x0,r23
	mov 0x0,r24
	mov 0x0,r25
	mov 0x0,r26
	mov 0x0,r27
	mov 0x0,r28
	mov 0x0,r29
	mov 0x0,r30
	mov 0x0,r31

	;testing starts here

	;test for SCH0L
	mov 0xffffefff,r1
	sch0l r1,r2
	;store position of 0 in r2
	mov 0x00000000,r1
	sch0l r1,r2
	;store 1 in r2
	mov 0xffffffff,r1
	sch0l r1,r2
	;store 0 in r2, Z flag 1
	mov 0xfffffffe,r1
	sch0l r1,r2
	;store 32 in r2, CY flag 1

	;test for SCH0R
	mov 0xffffefff,r1
	sch0r r1,r2
	;store position of 0 in r2
	mov 0xffffffff,r1
	sch0r r1,r2
	;store 0 in r2, Z flag 1
	mov 0x7fffffff,r1
	sch0r r1,r2
	;store 1 in r2, CY flag 1

	;test for SCH1L
	mov 0x00008000,r1
	sch0l r1,r2
	;store position of 1 in r2
	mov 0x00000000,r1
	sch0l r1,r2
	;store position 0 in r2, Z flag 1
	mov 0x00000001,r1
	sch0l r1,r2
	;store position 32 in r2, CY flag 1

	;test for SCH1R
	mov 0x00008000,r1
	sch0l r1,r2
	;store position of 1 in r2
	mov 0x00000000,r1
	sch0l r1,r2
	;store position 0 in r2, Z flag 1
	mov 0x80000000,r1
	sch0l r1,r2
	;store position 1 in r2, CY flag 1
	
	
	

	
