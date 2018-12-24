.include "RH850G3M_insts.s"
.include "gpr_init.s"

	#testing starts here

	#tests for and
	#simple tests for and
	mov 0xffff,r1
	mov 0xff11,r2
	and r1,r2
	and r2,r1
		
	mov 0xffffffff,r6
	and r1,r6
	#harder tests for and
	mov 0xffffffff,r7
	mov 0xf1111111,r8
	and r8,r7
	#checking s flag

	mov 0xffffffff,r8
	mov 0x00000000,r10
	and r8,r10
	and r10,r8
	#checking z flag
	
	#test for andi
	mov 0xff,r1
	andi 0x11ff,r1,r8		
	andi 0xffff,r1,r10
	mov 0xffffffff,r5
	andi 0x0,r5,r10
	andi 0xffff,r10,r12
	#checking zero flag
	
	mov 0xffffffff,r12
	andi 0xffff,r12,r20

	#test for not
	#simple test for not
	mov 0x1f,r1
	not r1,r2
	not r2,r3
	
	mov 0xffffffff,r10
	not r10,r11
	not r11,r12
	
	mov 0x00000000,r16
	not r16,r17
	not r17,r15

	#harder test for not
	mov 0x00000000,r15
	not r15,r14
	#checking s flag

	mov 0xffffffff,r1
	not r1,r2
	#checking z flag

	#tests for or
	#simple test for or
	mov 0x666,r1
	mov 0x777,r2
	or r1,r2
	or r2,r1

	mov 0x0,r10
	mov 0xfff,r12
	or r10,r12
	or r12,r10
	
	mov 0xffff,r17
	mov 0x0000ffff,r18
	or r17,r18
	or r18,r17

	mov 0x00ff00ff,r2
	mov 0x77777777,r29
	or r2,r29
	or r29,r2
	
	#harder test for or
	mov 0xf0000000,r9
	mov 0x0000000f,r12
	or r9,r12
	#checking for s flag
	
	mov 0x00ff00ff,r30
	mov 0xff00ff00,r2
	or r2,r30
	#checking for z flag
	
	mov 0x0,r2
	mov 0x0,r13
	or r2,r13
	or r13,r2	
	#another test for z flag

	#test for ori
	#simple tests for ori
	mov 0x5,r7
	ori 0xf,r7,r10
	ori 0xffff,r7,r12
	ori 0x0,r7,r13
	
	mov 0xff00ff00,r7
	ori 0xf,r7,r10
	ori 0x55,r7,r12
	ori 0x0,r7,r13
	
	#harder test for ori
	mov 0x0,r9
	ori 0x0,r9,r10
	ori 0x0,r10,r29
	#checking for z flag

	mov 0xff00ff00,r9
	ori 0x0,r9,r4
	ori 0xffff,r9,r10
	#checking for z flag
	
	mov 0xffffffff,r29
	ori 0xff,r29,r1
	ori 0xffff,r29,r10
	#another checking for z flag
	
	#tests for tst
	#simple test for tst
	mov 0xff,r1
	mov 0xff,r2
	tst r1,r2
	mov 0x005,r9
	mov 0x005,r15
	tst r9,r15
	tst r1,r9
	tst r2,r15
	
	#harder test for tst
	mov 0xffffffff,r10
	mov 0xf0000000,r5
	tst r10,r5
	tst r5,r10
	#checking for s flag
	
	mov 0xff00ff00,r9
	mov 0x00ff00ff,r15
	tst r9,r15
	tst r15,r9
	#checking for z flag
	
	mov 0x0,r9
	mov 0xffffffff,r24
	tst r9,r24
	tst r24,r9
	tst r26,r9
	#another test for z flag

	#tests for xor
	#simple test for xor
	mov 0xf,r1
	mov 0x7,r6
	xor r1,r6
	mov 0xffff,r5
	mov 0x0000,r6
	xor r5,r6
	mov 0x6,r22
	mov 0x4,r23
	xor r22,r23
	xor r23,r22
	
	#harder test for xor	
	mov 0xffffffff,r29
	mov 0x0fffffff,r5
	xor r29,r5
	#checking for z flag

	mov 0xffffffff,r28
	mov 0x0,r17
	xor r28,r17
	#another checking for z flag

	mov 0xffffffff,r16
	mov 0xffffffff,r8
	xor r8,r16
	#checking for s flag
	
	mov 0x0,r15
	mov 0x0,r11
	xor r11,r15
	xor r15,r11
	#another checking for z flag
	
	#test for xori
	#simple test for xori
	
	mov 0x5,r6
	xori 0x00ff,r6,r1
	xori 0x5,r6,r18
	xori 0xffff,r18,r5
	xori 0xffff,r6,r5
	xori 0x6,r6,r9
	
	#harder test for xori

	mov 0xffffffff,r9
	xori 0xffff,r9,r11
	xori 0x0,r9,r12
	#checking for s flag
	
	mov 0xffff,r7
	mov 0x0,r8
	xori 0xffff,r7,r15
	xori 0x0,r8,r16
	#testing for z flag
		
	Lbl: br Lbl
	
	
	
	
	
	
