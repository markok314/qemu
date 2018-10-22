.text

#--------------------------Testing CLR1 bit#3, disp16 [reg1] instruction-----------

	mov 0xfee00000,r1
	mov 0xfee00020,r2

	mov 0xffffffff,r2
	mov 0xffffff55,r3
	
	ST.W r2, 0x0 [r1]
	ST.W r3, 0x0 [r2]
	#store test regs into ram

	LD.W 0x0 [r1] , r5
	LD.W 0x0 [r2] , r6

	#simple tests with chanching bits to 0
	CLR1 0x0, 0x0 [r1]
	LD.W 0x0 [r1] , r11
	
	CLR1 0x2, 0x0 [r1]
	LD.W 0x0 [r1] , r12
	
	CLR1 0x7, 0x0 [r1]
	LD.W 0x0 [r1] , r12

	
	#simple tests with chanching bits to 1
	#checking z flag
	
	CLR1 0x0, 0x0 [r2]
	LD.W 0x0 [r1] , r11
	
	CLR1 0x1, 0x0 [r2]
	LD.W 0x0 [r1] , r12
	
	CLR1 0x2, 0x0 [r2]
	LD.W 0x0 [r1] , r13
	
	CLR1 0x3, 0x0 [r2]
	LD.W 0x0 [r1] , r14

	CLR1 0x4, 0x0 [r2]
	LD.W 0x0 [r1] , r15

	CLR1 0x5, 0x0 [r2]
	LD.W 0x0 [r1] , r16
	
	CLR1 0x6, 0x0 [r2]
	LD.W 0x0 [r1] , r17
	
	CLR1 0x7, 0x0 [r1]
	LD.W 0x0 [r1] , r18

	
#--------------------------Testing CLR1 reg2, [reg1] instruction-----------
	
	mov 0x0,r20
	mov 0x1,r21
	mov 0x2,r22
	mov 0x3,r23
	mov 0x4,r24
	mov 0x5,r25
	mov 0x6,r26
	mov 0x7,r27

	mov 0xfee00000,r1
	mov 0xfee00020,r2

	mov 0xffffffff,r2
	mov 0xffffff55,r3
	
	ST.W r2, 0x0 [r1]
	ST.W r3, 0x0 [r2]
	#store test regs into ram

	LD.W 0x0 [r1] , r5
	LD.W 0x0 [r2] , r6

	#simple tests with chanching bits to 0
	CLR1 r20, [r1]
	LD.W 0x0 [r1] , r11
	
	CLR1 r22, [r1]
	LD.W 0x0 [r1] , r12
	
	CLR1 r27, [r1]
	LD.W 0x0 [r1] , r12

	
	#simple tests with chanching bits to 1
	#checking z flag
	
	CLR1 r20, [r2]
	LD.W 0x0 [r1] , r11
	
	CLR1 r21,[r2]
	LD.W 0x0 [r1] , r12
	
	CLR1 r22, [r2]
	LD.W 0x0 [r1] , r13
	
	CLR1 r23, [r2]
	LD.W 0x0 [r1] , r14

	CLR1 r24, [r2]
	LD.W 0x0 [r1] , r15

	CLR1 r25, [r2]
	LD.W 0x0 [r1] , r16
	
	CLR1 r26, [r2]
	LD.W 0x0 [r1] , r17
	
	CLR1 r27, [r1]
	LD.W 0x0 [r1] , r18

#--------------------------Testing NOT1  bit#3, disp16 [reg1] instruction-----------

	mov 0xfee00000,r1
	mov 0xfee00020,r2

	mov 0xffffffee,r2
	mov 0xffffff77,r3
	
	ST.W r2, 0x0 [r1]
	ST.W r3, 0x0 [r2]
	#store test regs into ram

	LD.W 0x0 [r1] , r5
	LD.W 0x0 [r2] , r6

	#simple tests with chanching bits to 0
	NOT1 0x0, 0x0 [r1]
	LD.W 0x0 [r1] , r11
	
	NOT1 0x2, 0x0 [r1]
	LD.W 0x0 [r1] , r12
	
	NOT1 0x7, 0x0 [r1]
	LD.W 0x0 [r1] , r12

	
	#simple tests with chanching bits to 1
	#checking z flag
	
	NOT1 0x0, 0x0 [r2]
	LD.W 0x0 [r1] , r11
	
	NOT1 0x1, 0x0 [r2]
	LD.W 0x0 [r1] , r12
	
	NOT1 0x2, 0x0 [r2]
	LD.W 0x0 [r1] , r13
	
	NOT1 0x3, 0x0 [r2]
	LD.W 0x0 [r1] , r14

	NOT1 0x4, 0x0 [r2]
	LD.W 0x0 [r1] , r15

	NOT1 0x5, 0x0 [r2]
	LD.W 0x0 [r1] , r16
	
	NOT1 0x6, 0x0 [r2]
	LD.W 0x0 [r1] , r17
	
	NOT1 0x7, 0x0 [r1]
	LD.W 0x0 [r1] , r18

	
#--------------------------Testing NOT1 reg2, [reg1] instruction-----------
	
	mov 0x0,r20
	mov 0x1,r21
	mov 0x2,r22
	mov 0x3,r23
	mov 0x4,r24
	mov 0x5,r25
	mov 0x6,r26
	mov 0x7,r27

	mov 0xfee00000,r1
	mov 0xfee00020,r2

	mov 0xffffffff,r2
	mov 0xffffff55,r3
	
	ST.W r2, 0x0 [r1]
	ST.W r3, 0x0 [r2]
	#store test regs into ram

	LD.W 0x0 [r1] , r5
	LD.W 0x0 [r2] , r6

	#simple tests with chanching bits to 0
	NOT1 r20, [r1]
	LD.W 0x0 [r1] , r11
	
	NOT1 r22, [r1]
	LD.W 0x0 [r1] , r12
	
	NOT1 r27, [r1]
	LD.W 0x0 [r1] , r12

	
	#simple tests with chanching bits to 1
	#checking z flag
	
	NOT1 r20, [r2]
	LD.W 0x0 [r1] , r11
	
	NOT1 r21,[r2]
	LD.W 0x0 [r1] , r12
	
	NOT1 r22, [r2]
	LD.W 0x0 [r1] , r13
	
	NOT1 r23, [r2]
	LD.W 0x0 [r1] , r14

	NOT1 r24, [r2]
	LD.W 0x0 [r1] , r15

	NOT1 r25, [r2]
	LD.W 0x0 [r1] , r16
	
	NOT1 r26, [r2]
	LD.W 0x0 [r1] , r17
	
	NOT1 r27, [r1]
	LD.W 0x0 [r1] , r18


#--------------------------Testing SET1  bit#3, disp16 [reg1] instruction-----------

	mov 0xfee00000,r1
	mov 0xfee00020,r2

	mov 0xffffff33,r2
	mov 0xffffff11,r3
	
	ST.W r2, 0x0 [r1]
	ST.W r3, 0x0 [r2]
	#store test regs into ram

	LD.W 0x0 [r1] , r5
	LD.W 0x0 [r2] , r6

	SET1 0x0, 0x0 [r1]
	LD.W 0x0 [r1] , r11
	
	SET1 0x2, 0x0 [r1]
	LD.W 0x0 [r1] , r12
	
	SET1 0x7, 0x0 [r1]
	LD.W 0x0 [r1] , r12

	#checking z flag
	
	SET1 0x0, 0x0 [r2]
	LD.W 0x0 [r1] , r11
	
	SET1 0x1, 0x0 [r2]
	LD.W 0x0 [r1] , r12
	
	SET1 0x2, 0x0 [r2]
	LD.W 0x0 [r1] , r13
	
	SET1 0x3, 0x0 [r2]
	LD.W 0x0 [r1] , r14

	SET1 0x4, 0x0 [r2]
	LD.W 0x0 [r1] , r15

	SET1 0x5, 0x0 [r2]
	LD.W 0x0 [r1] , r16
	
	SET1 0x6, 0x0 [r2]
	LD.W 0x0 [r1] , r17
	
	SET1 0x7, 0x0 [r1]
	LD.W 0x0 [r1] , r18

	
#--------------------------Testing SET1 reg2, [reg1] instruction-----------
	
	mov 0x0,r20
	mov 0x1,r21
	mov 0x2,r22
	mov 0x3,r23
	mov 0x4,r24
	mov 0x5,r25
	mov 0x6,r26
	mov 0x7,r27

	mov 0xfee00000,r1
	mov 0xfee00020,r2

	mov 0xffffffff,r2
	mov 0xffffff55,r3
	
	ST.W r2, 0x0 [r1]
	ST.W r3, 0x0 [r2]
	#store test regs into ram

	LD.W 0x0 [r1] , r5
	LD.W 0x0 [r2] , r6

	SET1 r20, [r1]
	LD.W 0x0 [r1] , r11
	
	SET1 r22, [r1]
	LD.W 0x0 [r1] , r12
	
	SET1 r27, [r1]
	LD.W 0x0 [r1] , r12

	
	#checking z flag
	
	SET1 r20, [r2]
	LD.W 0x0 [r1] , r11
	
	SET1 r21,[r2]
	LD.W 0x0 [r1] , r12
	
	SET1 r22, [r2]
	LD.W 0x0 [r1] , r13
	
	SET1 r23, [r2]
	LD.W 0x0 [r1] , r14

	SET1 r24, [r2]
	LD.W 0x0 [r1] , r15

	SET1 r25, [r2]
	LD.W 0x0 [r1] , r16
	
	SET1 r26, [r2]
	LD.W 0x0 [r1] , r17
	
	SET1 r27, [r1]
	LD.W 0x0 [r1] , r18

#--------------------------Testing TST1 bit#3, disp16 [reg1] instruction-----------
#what happens if we want more then just one bit?
	
	mov 0xfee00000,r1
	mov 0xfee00020,r2

	mov 0xffffff33,r2
	mov 0xffffff11,r3
	
	ST.W r2, 0x0 [r1]
	ST.W r3, 0x0 [r2]
	#store test regs into ram

	LD.W 0x0 [r1] , r5
	LD.W 0x0 [r2] , r6

	TST1 0x0, 0x0 [r1]
	
	TST1 0x2, 0x0 [r1]
	
	TST1 0x7, 0x0 [r1]

	
	#checking z flag
	
	TST1 0x0, 0x0 [r2]
	
	TST1 0x1, 0x0 [r2]
	
	TST1 0x2, 0x0 [r2]
	
	TST1 0x3, 0x0 [r2]

	TST1 0x4, 0x0 [r2]

	TST1 0x5, 0x0 [r2]
	
	TST1 0x6, 0x0 [r2]
	
	TST1 0x7, 0x0 [r1]

	
#--------------------------Testing TST1 reg2, [reg1] instruction-----------
	
	mov 0x0,r20
	mov 0x1,r21
	mov 0x2,r22
	mov 0x3,r23
	mov 0x4,r24
	mov 0x5,r25
	mov 0x6,r26
	mov 0x7,r27

	mov 0xfee00000,r1
	mov 0xfee00020,r2

	mov 0xffffffff,r2
	mov 0xffffff55,r3
	
	ST.W r2, 0x0 [r1]
	ST.W r3, 0x0 [r2]
	#store test regs into ram

	LD.W 0x0 [r1] , r5
	LD.W 0x0 [r2] , r6

	TST1 r20, [r1]
	
	TST1 r22, [r1]
	
	TST1 r27, [r1]

	#checking z flag
	
	TST1 r20, [r2]
	
	TST1 r21,[r2]
	
	TST1 r22, [r2]
	
	TST1 r23, [r2]

	TST1 r24, [r2]

	TST1 r25, [r2]
	
	TST1 r26, [r2]
	
	TST1 r27, [r1]
