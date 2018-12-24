.include "RH850G3M_insts.s"
.include "gpr_init.s"

#--------tests for SHR-----------------------------

	#simple tests 
	mov 0x5,r1
	mov 0x7,r2
	shr r1,r2
	shr 0x10,r2
	shr r1,r2,r6
	mov 0xff,r1
	mov 0x14,r6
	shr r1,r6
	shr r1,r6,r10	
	shr 0x1f,r1
	
	ldsr r0, psw

	#testing cy flag
	mov 0x0000000f,r1
	mov 0x0000000f,r2
	mov 0x0000000f,r3
	mov 0x0,r4
	
	mov 0x3,r11
	mov 0x4,r12	
	mov 0x5,r13
	mov 0xff,r14
	
	shr r11,r1,r21
	shr r12,r2,r22
	shr r13,r3,r23
	shr r14,r4,r24
	ldsr r0, psw
	

	shr r11,r1
	shr r12,r2
	shr r13,r3
	shr r14,r4
	ldsr r0, psw
	
	mov 0x0000000f,r1
	mov 0x0000000f,r2
	mov 0x0000000f,r3
	mov 0x0,r4
	
	shr 0x3,r1
	shr 0x4,r2
	shr 0x5,r3
	shr 0x1f,r4
	ldsr r0, psw

	#testing s flag

	mov 0x5,r1
	mov 0x6,r2
	mov -0x9,r3
	mov -0xff,r4
	mov 0xffffffff,r5
	
	mov 0x3,r11
	mov 0x4,r12	
	mov 0x1,r13
	mov 0x4,r14
	mov 0x0,r15
	
	shr r11,r1,r21
	shr r12,r2,r22
	shr r13,r3,r23
	shr r14,r4,r24
	shr r15,r5,r25
	ldsr r0, psw
	
	shr r11,r1
	shr r12,r2
	shr r13,r3
	shr r14,r4
	shr r15,r5
	ldsr r0, psw
	
	mov 0x5,r1
	mov 0x6,r2
	mov -0x9,r3
	mov -0xff,r4
	mov 0xffffffff,r5
	
	shr 0x3,r1
	shr 0x4,r2
	shr 0x1,r3
	shr 0x4,r4
	shr 0x0,r5
	ldsr r0, psw

	#checking for z flag
	mov 0x0,r1
	mov 0x0000000f,r2
	mov 0x0000000f,r3
	mov 0x0000000f,r4
	mov 0x0000000f,r5

	mov 0x1f,r11
	mov 0x2,r12
	mov 0x3,r13
	mov 0x4,r14
	mov 0x5,r15
	
	shr r11,r1,r21
	shr r12,r2,r22
	shr r13,r3,r23
	shr r14,r4,r24
	shr r15,r5,r25
	ldsr r0, psw
	
	shr r11,r1
	shr r12,r2
	shr r13,r3
	shr r14,r4
	shr r15,r5
	ldsr r0, psw

	mov 0x0,r1
	mov 0x0000000f,r2
	mov 0x0000000f,r3
	mov 0x0000000f,r4
	mov 0x0000000f,r5

	shr 0x1f,r1
	shr 0x2,r2
	shr 0x3,r3
	shr 0x4,r4
	shr 0x5,r5
	ldsr r0, psw

Lbl: br Lbl

