.include "RH850G3M_insts.s"
.include "gpr_init.s"


#--------tests for bins (missing, implement macro for bins)

	
#--------tests for BSH-----------------------------

	#simple test for bsh

	mov 0x12345678,r1
	bsh r1,r2
	bsh r2,r1
	
	mov 0x7fffffff,r6
	bsh r9,r6
	bsh r6,r9

	#harder test for bsh

	#testing for z flag

	mov 0x00000000,r16
	bsh r16,r17
	bsh r17,r16
	
	#testing for s flag

	mov 0xffffffff,r5
	bsh r5,r1
			
	#testing for cy flag but not z flag

	mov 0x1234ff00,r12
	bsh r12,r2

	#bsh should test this:
	#Result: 0xffff8071 is this cy flag??

	mov 0xffff8071,r14
	bsh r14,r17
	
#--------tests for BSW-----------------------------

	#simple test for bsw
	mov 0x12345678,r6
	bsw r6,r10
	bsw r10,r6
	
	mov 0x80000000,r8
	bsw r8,r10
	
	#harder test for bsw
	#checking for z flag
	mov 0x00000000,r7
	bsw r7,r10
	
	#checking for s flag
	
	mov 0x0011000f,r21
	bsw r21, r30
	
	#checking for cy flag
	mov 0x000000f0,r29
	bsw r29,r5
	
	#tests for cmov are in special file

#--------tests for HSH-----------------------------
	ldsr r0, psw
	#clearing all flags

	#simple test for HSH

	mov 0x5,r7
	mov 0xfff11,r9
	hsh r7,r10
	hsh r9,r11
	
	ldsr r0, psw
	#clearing all flags

	#testing cy flag
	mov 0xffff0000,r1
	mov 0xff000000,r2
	mov 0xffffff00,r3
	mov 0xffffffff,r4
	hsh r1,r5
	ldsr r0, psw
	hsh r2,r6
	ldsr r0, psw
	hsh r3,r7
	ldsr r0, psw
	hsh r4,r8

	#testing s flag

	ldsr r0, psw
	mov 0xfffffff0,r10
	mov 0x0fffffff,r11
	mov 0xf0ffffff,r12
	mov 0x7fffffff,r13
	hsh r10,r14
	ldsr r0, psw
	hsh r11,r15
	ldsr r0, psw
	hsh r12,r16
	ldsr r0, psw
	hsh r13,r17
	ldsr r0, psw
	
	#testing z flag
	mov 0xffff0000,r1
	mov 0xff000000,r2
	mov 0xffffff00,r3
	mov 0xffffffff,r4
	hsh r1,r5
	ldsr r0, psw
	hsh r2,r6
	ldsr r0, psw
	hsh r3,r7
	ldsr r0, psw
	hsh r4,r8

#--------tests for HSW-----------------------------
	
	ldsr r0, psw

	#simple tests for hsw

	mov 0xfa8e,r1
	mov 0x05,r2
	mov 0x7000fff0,r3
	mov 0xfefefefe,r4
	
	hsw r1,r5
	ldsr r0, psw
	hsw r2,r6
	ldsr r0, psw
	hsw r3,r7
	ldsr r0, psw
	hsw r4,r8
	ldsr r0, psw

	#testing cy flag

	mov 0xffff0001,r7
	mov 0x0001fef0,r8
	mov 0x11110000,r9
	mov 0x0000fe01,r10
	mov 0x00000000,r11
	
	hsw r7,r12
	ldsr r0, psw
	hsw r8,r13
	ldsr r0, psw
	hsw r9,r14
	ldsr r0, psw
	hsw r10,r15
	ldsr r0, psw
	hsw r11,r16

	#testing s flag
	mov 0xffff8000,r1
	mov 0x0101f011,r2
	mov 0xffff0111,r3
	mov 0x00001000,r4
	mov 0xffff7fff,r5
	
	hsw r1,r6
	ldsr r0, psw
	hsw r2,r7
	ldsr r0, psw
	hsw r3,r8
	ldsr r0, psw
	hsw r4,r9
	ldsr r0, psw
	hsw r5,r10
	ldsr r0, psw

	#testing z flag
	mov 0x00000001,r25
	mov 0xffff0000,r26
	mov 0x0000ffff,r27
	mov 0x00000000,r28
	mov 0x80000000,r29
	
	hsw r25,r15
	ldsr r0, psw
	hsw r26,r16
	ldsr r0, psw
	hsw r27,r17
	ldsr r0, psw
	hsw r28,r18
	ldsr r0, psw
	hsw r29,r19

#--------tests for ROTL (something is wrong with rotl)
		
#--------tests for SAR-----------------------------
	
	ldsr r0, psw
	
	#simple test for sar

	mov 0x5,r1
	mov 0xff0000ff,r2
	sar r1,r2,r3
	ldsr r0, psw
	sar r1,r2
	ldsr r0, psw
	sar 0x1,r1
	ldsr r0, psw

	mov 0x9,r7
	mov 0xff001100,r10
	sar r7,r10,r12
	ldsr r0, psw
	sar r7,r10
	ldsr r0, psw
	sar 0x6,r10
	ldsr r0, psw

 	mov 0x1f,r5
	mov 0xffffffff,r6
	sar r5,r6,r7
	sar r5,r6
	sar 0x1f,r6

	mov 0x1f,r5
	mov 0xffffffff,r6
	sar r5,r6,r7
	ldsr r0, psw
	sar r5,r6
	ldsr r0, psw
	sar 0x1f,r6
	ldsr r0, psw
	
	mov 0x0,r5
	mov 0xffffffff,r6
	sar r5,r6,r7
	ldsr r0, psw
	sar r5,r6
	ldsr r0, psw
	sar 0x0,r6
	ldsr r0, psw

	#testing cy flag
	
	#sar r1,r2
	
	mov 0xfffffff0,r1
	mov 0xfffffff1,r2
	mov 0x0000000f,r3
	mov 0x00000007,r4
	mov 0x0000000f,r5
	mov 0x00000007,r6
	mov 0xffffffff,r7
			
	mov 0x1,r11
	mov 0x1,r12
	mov 0x3,r13
	mov 0x3,r14
	mov 0x4,r15
	mov 0x5,r16
	mov 0xf,r14
	
	sar r11,r1
	sar r12,r2
	sar r13,r3
	sar r14,r4	
	sar r15,r5	
	sar r16,r6	
	sar r17,r7
	ldsr r0, psw

	#sar r1,r2,r3
	
	mov 0xfffffff0,r1
	mov 0xfffffff1,r2
	mov 0x0000000f,r3
	mov 0x00000007,r4
	mov 0x0000000f,r5
	mov 0x00000007,r6
	mov 0xffffffff,r7
			
	mov 0x1,r11
	mov 0x1,r12
	mov 0x3,r13
	mov 0x3,r14
	mov 0x4,r15
	mov 0x5,r16
	mov 0xf,r14
	
	sar r11,r1,r21
	sar r12,r2,r22
	sar r13,r3,r23
	sar r14,r4,r24	
	sar r15,r5,r25	
	sar r16,r6,r26	
	sar r17,r7,r27
	ldsr r0, psw
	
	#sar imm,r2
	
	mov 0xfffffff0,r1
	mov 0xfffffff1,r2
	mov 0x0000000f,r3
	mov 0x00000007,r4
	mov 0x0000000f,r5
	mov 0x00000007,r6
	mov 0xffffffff,r7
			
	mov 0x1,r11
	mov 0x1,r12
	mov 0x3,r13
	mov 0x3,r14
	mov 0x4,r15
	mov 0x5,r16
	mov 0xf,r14
	
	sar 0x1,r1
	sar 0x1,r2
	sar 0x3,r3
	sar 0x3,r4	
	sar 0x4,r5	
	sar 0x4,r6	
	sar 0xf,r7
	ldsr r0, psw
	
	#testing s flag
	
	mov 0xffffffff,r1
	mov 0x0,r2
	sar r2,r1
	ldsr r0, psw
	sar r2,r1,r3
	ldsr r0, psw
	sar 0x0,r1
	ldsr r0, psw

	#testing z flag
	
	mov 0x00000001,r1
	mov 0x0000000f,r2
	mov 0x0000000f,r3
	mov 0x0000000f,r4
	mov 0x0000ffff,r5
	mov 0x00000000,r6
	
	mov 0x1,r11
	mov 0x2,r12
	mov 0x3,r13
	mov 0x4,r14
	mov 0x1f,r15
	mov 0xe,r16
	
	sar r11,r1
	sar r12,r2
	sar r13,r3
	sar r14,r4
	sar r15,r5
	sar r16,r6
	ldsr r0, psw

	mov 0x1,r11
	mov 0x2,r12
	mov 0x3,r13
	mov 0x4,r14
	mov 0x1f,r15
	mov 0xe,r16
	
	sar 0x1,r1
	sar 0x2,r2
	sar 0x3,r3
	sar 0x4,r4
	sar 0x1f,r5
	sar 0xe,r6
	ldsr r0, psw
	
	mov 0x1,r11
	mov 0x2,r12
	mov 0x3,r13
	mov 0x4,r14
	mov 0x1f,r15
	mov 0xe,r16
	
	sar r11,r1,r21
	sar r12,r2,r22
	sar r13,r3,r23
	sar r14,r4,r24
	sar r15,r5,r25
	sar r16,r6,r26
	ldsr r0, psw

#--------test for SASF is in special file

#--------tests for SETF is in special file

#--------tests for SHL-----------------------------

	#simple tests 
	mov 0x5,r1
	mov 0x7,r2
	shl r1,r2
	shl 0x10,r2
	shl r1,r2,r6
	mov 0xff,r1
	mov 0x14,r6
	shl r1,r6
	shl r1,r6,r10	
	shl 0x1f,r1
	
	ldsr r0, psw

	#checking for z flag
	mov 0xffffffff,r1
	mov 0x7fffffff,r2
	mov 0x0fffffff,r3
	mov 0xffffffff,r1
	
	mov 0x5,r11
	mov 0x1,r12
	mov 0x8,r13
	mov 0x0,r14
	
	shl r11,r1
	ldsr r0, psw
	shl r12,r2
	ldsr r0, psw
	shl r13,r3
	ldsr r0, psw
	shl r14,r4
	ldsr r0, psw

	mov 0xffffffff,r1
	mov 0x7fffffff,r2
	mov 0x0fffffff,r3
	mov 0xffffffff,r1

	shl 0x5,r1
	ldsr r0, psw
	shl 0x1,r2
	ldsr r0, psw
	shl 0x8,r3
	ldsr r0, psw
	shl 0x0,r4
	ldsr r0, psw

	mov 0xffffffff,r1
	mov 0x7fffffff,r2
	mov 0x0fffffff,r3
	mov 0xffffffff,r1

	shl r11,r1,r21
	ldsr r0, psw
	shl r12,r2,r22
	ldsr r0, psw
	shl r13,r3,r23
	ldsr r0, psw
	shl r14,r4,r24
	ldsr r0, psw

	
	#checking for s flag
	mov 0x5,r1
	mov 0x7,r2
	mov -0x1,r3
	mov -0x6,r4
	mov 0x7fffffff,r5
	mov 0xffffffff,r6
	
	mov 0x5,r11
	mov 0x1,r12
	mov 0xf,r13
	mov 0xe,r14
	mov 0x3,r15
	mov 0x0,r16
	
	shl r11,r1,r21
	ldsr r0, psw
	shl r12,r2,r22
	ldsr r0, psw
	shl r13,r3,r23
	ldsr r0, psw
	shl r14,r4,r24
	ldsr r0, psw
	shl r15,r5,r25
	ldsr r0, psw
	shl r16,r6,r26
	ldsr r0, psw

	shl r11,r1
	ldsr r0, psw
	shl r12,r2
	ldsr r0, psw
	shl r13,r3
	ldsr r0, psw
	shl r14,r4
	ldsr r0, psw
	shl r15,r5
	ldsr r0, psw
	shl r16,r6
	ldsr r0, psw
	
	mov 0x5,r1
	mov 0x7,r2
	mov -0x1,r3
	mov -0x6,r4
	mov 0x7fffffff,r5
	mov 0xffffffff,r6
	
	shl 0x5,r1
	ldsr r0, psw
	shl 0x1,r2
	ldsr r0, psw
	shl 0xf,r3
	ldsr r0, psw
	shl 0xe,r4
	ldsr r0, psw
	shl 0x3,r5
	ldsr r0, psw
	shl 0x0,r6
	ldsr r0, psw

	#checking for z flag
	mov 0x0,r1
	mov 0xff000000,r2
	mov 0xff000000,r2
	mov 0xff000000,r2
	
	mov 0xf,r11
	mov 0x7,r12
	mov 0x8,r13
	mov 0x9,r14
	
	shl r11,r1,r21
	ldsr r0, psw
	shl r12,r2,r22
	ldsr r0, psw
	shl r13,r3,r23
	ldsr r0, psw
	shl r14,r4,r24
	ldsr r0, psw

	shl r11,r1
	ldsr r0, psw
	shl r12,r2
	ldsr r0, psw
	shl r13,r3
	ldsr r0, psw
	shl r14,r4
	ldsr r0, psw

	mov 0x0,r1
	mov 0xff000000,r2
	mov 0xff000000,r2
	mov 0xff000000,r2
	
	shl 0xf,r1
	ldsr r0, psw
	shl 0x7,r2
	ldsr r0, psw
	shl 0x8,r3
	ldsr r0, psw
	shl 0x9,r4
	ldsr r0, psw	
	
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

#--------tests for SXB-----------------------------

	mov 0x5,r1
	sxb r1
	mov 0xffffffff,r2
	sxb r2
	mov -0x1,r3
	sxb r3
	mov -0xff,r4
	sxb r4
	mov -0x1ff,r5
	sxb r5
	mov 0xffffffff,r6
	sxb r6
	mov 0x80000005,r7
	sxb r7
	mov -0x6,r8
	sxb r8
	mov -0x7f,r9
	sxb r9
	mov 0x7f,r10
	sxb r10	

#--------tests for SXH-----------------------------

	mov 0x7fff,r1
	sxh r1 
	mov 0xffff,r2
	sxh r2 
	mov -0x7fff,r3
	sxh r3 
	mov -0xffff,r4
	sxh r4 
	mov 0x5,r5
	sxh r5 
	mov -0x6,r6
	sxh r6 
	mov 0xfffffffe,r7
	sxh r7 
	mov 0x80000000,r8
	sxh r8 
	mov 0x80000ff0,r9
	sxh r9 	
	mov 0x7fffffff,r10
	sxh r10 
	mov -0xff,r11
	sxh r11
	mov 0xff,r12
	sxh r12 
	
#--------tests for ZXB-----------------------------
	mov 0x5,r1
	zxb r1
	mov 0xffffffff,r2
	zxb r2
	mov -0x1,r3
	zxb r3
	mov -0xff,r4
	zxb r4
	mov -0x1ff,r5
	zxb r5
	mov 0xffffffff,r6
	zxb r6
	mov 0x80000005,r7
	zxb r7
	mov -0x6,r8
	zxb r8
	mov -0x7f,r9
	zxb r9
	mov 0x7f,r10
	zxb r10	

#--------tests for ZXH-----------------------------

	mov 0x7fff,r1
	zxh r1 
	mov 0xffff,r2
	zxh r2 
	mov -0x7fff,r3
	zxh r3 
	mov -0xffff,r4
	zxh r4 
	mov 0x5,r5
	zxh r5 
	mov -0x6,r6
	zxh r6 
	mov 0xfffffffe,r7
	zxh r7 
	mov 0x80000000,r8
	zxh r8 
	mov 0x80000ff0,r9
	zxh r9 	
	mov 0x7fffffff,r10
	zxh r10 
	mov -0xff,r11
	zxh r11
	mov 0xff,r12
	zxh r12
	
	
Lbl: br Lbl


