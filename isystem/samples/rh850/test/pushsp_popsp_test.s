.include "RH850G3M_insts.s"
.include "gpr_init.s"

#----------------Tests for PUSHSP - POPSP -------------------


	mov hilo(.data + 100), r3	# stack pointer

	mov 0x324, r20
	mov 0x124a, r21
	mov 0xfffc, r22
	mov 0x1, r23
	mov 0xcbd, r24
	mov 0x74c, r25
	mov 0x99924, r26
	mov 0x1f2112a, r27
	mov 0xabbbc, r28
	mov 0x45784, r29
	mov 0xec212, r30
	mov 0xa344c, r31

	PUSHSP R20 R30
	
	#Clearing regs

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

	POPSP R20 R30

#------------------------------------------------

	mov 0x324, r5
	mov 0x124a, r6
	mov 0xfffc, r7
	mov 0x1, r8
	mov 0xcbd, r9
	mov 0x74c, r10
	mov 0x99924, r11
	mov 0x1f2112a, r12
	mov 0xabbbc, r13
	mov 0x45784, r14
	mov 0xec212, r15
	mov 0xa344c, r16

	PUSHSP R5 R11
	
	#Clearing regs

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

	POPSP R5 R11

#------------------------------------------------

	mov 0x324, r5
	mov 0x124a, r6
	mov 0xfffc, r7
	mov 0x1, r8
	mov 0xcbd, r9
	mov 0x74c, r10
	mov 0x99924, r11
	mov 0x1f2112a, r12
	mov 0xabbbc, r13
	mov 0x45784, r14
	mov 0xec212, r15
	mov 0xa344c, r16

	PUSHSP R8 R16
	
	#Clearing regs

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

	POPSP R8 R16
	

#------------------------------------------------

	mov 0x324, r5
	mov 0x124a, r6
	mov 0xfffc, r7
	mov 0x1, r8
	mov 0xcbd, r9
	mov 0x74c, r10
	mov 0x99924, r11
	mov 0x1f2112a, r12
	mov 0xabbbc, r13
	mov 0x45784, r14
	mov 0xec212, r15
	mov 0xa344c, r16

	PUSHSP R8 R8

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

	POPSP R9 R9


Lbl: br Lbl
