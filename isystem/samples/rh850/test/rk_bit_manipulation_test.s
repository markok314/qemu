.include "RH850G3M_insts.s"
.include "gpr_init.s"


	## These instructions are only affecting 
	## the zero flag (ZF), expected ZF values 
	## are written in the comments by each instruction.


#-------------------------- Testing CLR1 instruction --------

	mov 0x4, r1
	mov 0x5, r2
	mov 0x7, r3
	mov 0xffffffff, r7	
	mov 0xffffffff, r8
	mov 0xfee00004, r11
	mov 0xfee00000, r12

	st.b r7, 0[r12]
	st.b r8, 0[r11]

	clr1 0x7, 0[r12]	#ZF=0
	clr1 0x0, 0[r12]	#ZF=0
	clr1 0x0, 0[r12]	#ZF=1
	clr1 0x1, 0[r12]	#ZF=0
	clr1 0x2, 0[r12]	#ZF=0
	clr1 0x3, 0[r12]	#ZF=0

	clr1 r1, [r11]		#ZF=0
	clr1 r0, [r11]		#ZF=0
	clr1 r3, [r11]		#ZF=0
	clr1 r2, [r11]		#ZF=0
	clr1 r2, [r11]		#ZF=1

	ld.b 0[r12], r13
	ld.b 0[r11], r14

	# Is it OK if the returned byte is sign-extended???
	
#-------------------------- Testing NOT1 instruction --------

	mov 0xffffffff, r1
	mov 0x00000000, r2
	mov 0xfee00000, r4
	mov 0xfee00004, r5
	mov 0x2, r6
	mov 0x4, r7
	mov 0x5, r8
	mov 0x6, r9

	st.b r1, 0[r4]
	st.b r2, 0[r5]
	
	not1 0x0, 0[r4]		#ZF=0
	not1 0x3, 0[r4]		#ZF=0
	not1 0x7, 0[r4]		#ZF=0
	not1 0x5, 0[r4]		#ZF=0
	not1 0x3, 0[r4]		#ZF=1
	not1 0x7, 0[r4]		#ZF=1

	not1 r6, [r5]
	not1 r7, [r5]
	not1 r8, [r5]
	not1 r7, [r5]
	not1 r6, [r5]
	not1 r9, [r5]
	not1 r0, [r5]

	ld.b 0[r4], r15
	ld.b 0[r5], r16

	# Is it OK if the returned byte is sign-extended???

#-------------------------- Testing SET1 instruction --------

	mov 0x00000000, r1
	mov 0x00000000, r2
	mov 0xfee00000, r4
	mov 0xfee00004, r5
	mov 0x1, r6
	mov 0x3, r7
	mov 0x5, r8
	mov 0x6, r9
	mov 0x7, r10

	st.b r1, 0[r4]
	st.b r2, 0[r5]
	
	set1 0x2, 0[r4]		#ZF=1
	set1 0x1, 0[r4]		#ZF=1
	set1 0x5, 0[r4]		#ZF=1
	set1 0x7, 0[r4]		#ZF=1
	set1 0x1, 0[r4]		#ZF=0
	set1 0x0, 0[r4]		#ZF=1
	set1 0x3, 0[r4]		#ZF=1
	set1 0x2, 0[r4]		#ZF=0

	set1 r6, [r5]		#ZF=1
	set1 r7, [r5]		#ZF=1
	set1 r8, [r5]		#ZF=1
	set1 r7, [r5]		#ZF=0
	set1 r10, [r5]		#ZF=1
	set1 r9, [r5]		#ZF=1
	set1 r0, [r5]		#ZF=1
	set1 r6, [r5]		#ZF=0

	ld.b 0[r4], r15
	ld.b 0[r5], r16


#-------------------------- Testing TST1 instruction --------

	mov 0x0000005b, r1
	mov 0x000000c3, r2
	mov 0xfee00000, r4
	mov 0xfee00004, r5
	mov 0x1, r6
	mov 0x3, r7
	mov 0x5, r8
	mov 0x6, r9
	mov 0x7, r10

	st.b r1, 0[r4]
	st.b r2, 0[r5]
	
	tst1 0x2, 0[r4]		#ZF=1
	tst1 0x1, 0[r4]		#ZF=0
	tst1 0x5, 0[r4]		#ZF=1
	tst1 0x7, 0[r4]		#ZF=1
	tst1 0x0, 0[r4]		#ZF=0
	tst1 0x3, 0[r4]		#ZF=0

	tst1 r6, [r5]		#ZF=0
	tst1 r7, [r5]		#ZF=1
	tst1 r8, [r5]		#ZF=1
	tst1 r10, [r5]		#ZF=0
	tst1 r9, [r5]		#ZF=0
	tst1 r0, [r5]		#ZF=0

	ld.b 0[r4], r15
	ld.b 0[r5], r16



Lbl: br Lbl
