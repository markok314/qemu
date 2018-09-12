.text
.include "gpr_init.s"

	#testing starts here

--------#simple test for cmov cccc,r1,r2,r3------------------

	mov 0x1,r18
	mov 0x2,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29

-------#Testing V and NV condition for cmov ccccc,r1,r2,r3--
	
	#OV flag is not set here

	mov 0x1,r1
	mov 0x2,r2
	cmov V,r1,r2,r3
	cmov NV,r1,r2,r4
	
	#other condition should work as before
	mov 0x3,r18
	mov 0x4,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29


	#OV flag is set here

	mov 0x7, r2
	mov 0x80000000, r6
	sub r2, r6

	mov 0x1,r1
	mov 0x2,r2
	cmov V,r1,r2,r3
	cmov NV,r1,r2,r4
	
	#other condition should work as before
	mov 0x5,r18
	mov 0x6,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29

	ldsr r0, psw
	#clearing all flags

-------#Testing C/L and NC/NL condition for cmov ccccc,r1,r2,r3--

	#CY flag is not set here

	mov 0x3,r1
	mov 0x4,r2
	cmov C,r1,r2,r3
	cmov L,r1,r2,r4
	cmov NC,r1,r2,r5
	cmov NL,r1,r2,r6

	#other condition should work as before
	mov 0x7,r18
	mov 0x8,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29

	#CY flag is set here

	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1
	
	mov 0x5,r1
	mov 0x6,r2
	cmov C,r1,r2,r3
	cmov L,r1,r2,r4
	cmov NC,r1,r2,r5
	cmov NL,r1,r2,r6

	#other condition should work as before
	mov 0x9,r18
	mov 0x10,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29
	
	ldsr r0, psw
	#clearing all flags
	
-------#Testing Z and NZ condition for cmov ccccc,r1,r2,r3--
	
	#Z flag is not set here

	mov 0x7,r1
	mov 0x8,r2
	cmov Z,r1,r2,r3
	cmov NZ,r1,r2,r4
	
	#other condition should work as before
	mov 0x11,r18
	mov 0x12,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29
	
	#Z flag is set here
	addi 0x0, r0, r1
	
	mov 0x9,r1
	mov 0x10,r2
	cmov Z,r1,r2,r3
	cmov NZ,r1,r2,r4
	
	#other condition should work as before
	mov 0x13,r18
	mov 0x14,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29

	ldsr r0, psw
	#clearing all flags

-------#Testing NH and H condition for cmov ccccc,r1,r2,r3--
	
	#CHECKING COMBINATIONS (CY or Z) = 1
	#CHECKING Z FLAG

	#Z flag is not set here

	mov 0x11,r1
	mov 0x12,r2
	cmov NH,r1,r2,r3
	cmov H,r1,r2,r4
	
	#other condition should work as before
	mov 0x15,r18
	mov 0x16,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29
	
	#Z flag is set here
	addi 0x0, r0, r1
	
	mov 0x13,r1
	mov 0x14,r2
	cmov NH,r1,r2,r3
	cmov H,r1,r2,r4
	
	#other condition should work as before
	mov 0x17,r18
	mov 0x18,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29

	ldsr r0, psw
	#clearing all flags
	
	#CHECKING COMBINATIONS (CY or Z) = 1
	#CHECKING CY FLAG
	
	#CY flag is not set here
	mov 0x15,r1
	mov 0x16,r2
	cmov NH,r1,r2,r3
	cmov H,r1,r2,r4
	
	#other condition should work as before
	mov 0x19,r18
	mov 0x20,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29

	#CY flag is set here
	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1

	mov 0x17,r1
	mov 0x18,r2
	cmov NH,r1,r2,r3
	cmov H,r1,r2,r4
	
	#other condition should work as before
	mov 0x21,r18
	mov 0x22,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29

	ldsr r0, psw
	#clearing all flags


--------#simple test for cmov cccc,imm,r2,r3------------------

	mov 0x1,r18
	mov 0x2,r19
	cmov V,r18,r19,r20
	cmov NV,r18,r19,r21
	cmov C,r18,r19,r22
	cmov L,r18,r19,r23
	cmov NC,r18,r19,r24
	cmov NL,r18,r19,r25
	cmov Z,r18,r19,r26
	cmov NZ,r18,r19,r27
	cmov NH,r18,r19,r28
	cmov H,r18,r19,r29

-------#Testing V and NV condition for cmov ccccc,imm,r2,r3--
	
	#OV flag is not set here

	mov 0x2,r2
	cmov V,0x1,r2,r3
	cmov NV,0x1,r2,r4
	
	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29


	#OV flag is set here

	mov 0x7, r2
	mov 0x80000000, r6
	sub r2, r6

	mov 0x3,r2
	cmov V,0x2,r2,r3
	cmov NV,0x2,r2,r4
	
	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29

	ldsr r0, psw
	#clearing all flags

-------#Testing C/L and NC/NL condition for cmov ccccc,imm,r2,r3--

	#CY flag is not set here

	mov 0x8,r2
	cmov C,0x7,r2,r3
	cmov L,0x7,r2,r4
	cmov NC,0x7,r2,r5
	cmov NL,0x7,r2,r6

	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29

	#CY flag is set here

	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1
	
	mov 0x6,r2
	cmov C,0x5,r2,r3
	cmov L,0x5,r2,r4
	cmov NC,0x5,r2,r5
	cmov NL,0x5,r2,r6

	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29
	
	ldsr r0, psw
	#clearing all flags
	
-------#Testing Z and NZ condition for cmov ccccc,imm,r2,r3--
	
	#Z flag is not set here

	mov 0x8,r2
	cmov Z,0x2,r2,r3
	cmov NZ,0x2,r2,r4
	
	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29
	
	#Z flag is set here
	addi 0x0, r0, r1
	
	mov 0x10,r2
	cmov Z,0x9,r2,r3
	cmov NZ,0x9,r2,r4
	
	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29

	ldsr r0, psw
	#clearing all flags

-------#Testing NH and H condition for cmov ccccc,imm,r2,r3--
	
	#CHECKING COMBINATIONS (CY or Z) = 1
	#CHECKING Z FLAG

	#Z flag is not set here

	mov 0x12,r2
	cmov NH,0x11,r2,r3
	cmov H,0x11,r2,r4
	
	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29
	
	#Z flag is set here
	addi 0x0, r0, r1
	
	mov 0x14,r2
	cmov NH,0x13,r2,r3
	cmov H,0x13,r2,r4
	
	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29

	ldsr r0, psw
	#clearing all flags
	
	#CHECKING COMBINATIONS (CY or Z) = 1
	#CHECKING CY FLAG
	
	#CY flag is not set here
	mov 0x16,r2
	cmov NH,0x15,r2,r3
	cmov H,0x15,r2,r4
	
	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29

	#CY flag is set here
	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1

	mov 0x18,r2
	cmov NH,0x17,r2,r3
	cmov H,0x17,r2,r4
	
	#other condition should work as before
	mov 0xb,r19
	cmov V,0x1,r19,r20
	cmov NV,0x2,r19,r21
	cmov C,0x3,r19,r22
	cmov L,0x4,r19,r23
	cmov NC,0x5,r19,r24
	cmov NL,0x6,r19,r25
	cmov Z,0x7,r19,r26
	cmov NZ,0x8,r19,r27
	cmov NH,0x9,r19,r28
	cmov H,0xa,r19,r29

	ldsr r0, psw
	#clearing all flags

Lbl: br Lbl
