.include "RH850G3M_insts.s"
.include "gpr_init.s"

#------------------------------------------

	mov 0xfedf0000, r1
	mov 0xfedf0024, r2
	mov 0xfedf0060, r3
	mov 0xfedf00a0, r4

#----------------------------STORE INSTRUCTIONS---------------

	mov 0x1234, r5
	mov 0x8468, r6
	mov 0xdedababa, r7
	mov 0xcece1010, r8
	mov 0xcafebabe, r9
					#st.b r2, disp16[r1]
					#st.b r2, disp23[r1]
	st.b r5, 0x0[r1]
	st.b r6, 0x2214[r1]
	st.b r7, 0xffff[r1]
	st.b r8, 0x1234[r1]
	ST_B R7, 0xfff0 R1
	ST_B R5, 0xaaaa R1
	ST_B R6, 0xbbbb R1
					#st.dw r2, disp23[r1]
	ST_DW R6, 0x23f0 R2
	ST_DW R8, 0x23fc R2
					#st.h r2, disp16[r1]
					#st.h r2, disp23[r1]
	st.h r3, 0x0[r3]
	st.h r5, 0xa[r3]
	st.h r6, 0xa0[r3]
	st.h r8, 0x700e[r3]

	ST_H R3, 0xf000 R3
	ST_H R5, 0xf0f0 R3
	ST_H R6, 0xf00e R3
	ST_H R8, 0xf0fe R3
					#st.w r2, disp16[r1]
					#st.w r2, disp23[r1]
	st.w r7, 0x0[r4]
	st.w r8, 0x4[r4]
	st.w r6, 0x8[r4]

	ST_W R5, 0x2348 R4
	ST_W R6, 0x4444 R4
	ST_W R8, 0xa0 R4

#----------------------------LOAD INSTRUCTIONS---------------

					#ld.b disp16[r1], r2
					#ld.b disp23[r1], r2
	ld.b 0x0[r1], r10
	ld.b 0x2214[r1], r11
	ld.b 0x0[r2], r12
					#ld.bu disp16[r1], r2
					#ld.bu disp23[r1], r2
	ld.bu 0x0[r2], r13
	ld.bu 0x1234[r1], r16
	LD_BU 0xffff R1, R17
	LD_B 0xaaaa R1, R17
	LD_B 0xbbbb R1, R17
					#ld.dw disp23[r1], r2
	LD_DW 0x23f0 R2, R14
	LD_DW 0x23fc R2, R16
					#ld.h disp16[r1], r2
					#ld.h disp23[r1], r2
	ld.h 0x0[r3], r18
	ld.h 0xa[r3], r19
	ld.h 0xa0[r3], r20
	ld.h 0x700e[r3], r21

	LD_H 0xf000 R3, R22
	LD_H 0xf0f0 R3, R23
	LD_H 0xf00e R3, R24
	LD_H 0xf0fe R3, R25
					#ld.hu disp16[r1], r2
					#ld.hu disp23[r1], r2
	ld.hu 0x0[r3], r18
	ld.hu 0xa[r3], r19
	ld.hu 0xa0[r3], r20
	ld.hu 0xf00e[r3], r21

	LD_HU 0xf000 R3, R22
	LD_HU 0xf0f0 R3, R23
	LD_HU 0xf00e R3, R24
	LD_HU 0xf0fe R3, R25
					#ld.w disp16[r1], r2
					#ld.w disp23[r1], r2
	ld.w 0x0[r4], r26
	ld.w 0x4[r4], r27
	ld.w 0x8[r4], r28

	LD_W 0x2348 R4, R5
	LD_W 0x4444 R4, R6
	LD_W 0xa0 R4, R7

#----------------------SHORT STORE INSTRUCTIONS---------------

					#sst.b r4, 0x7f[ep]

					#sst.h r5, 0xff[ep]

					#sst.w r5, 0xff[ep]
	mov 0xfee00060, r30

	mov 0x12034644, r1
	mov 0x11237000, r2
	mov 0x00001524, r4
	mov 0x0ee00080, r5
	mov 0x00003456, r6
	mov 0xffffff00, r7
	mov 0x000e0024, r8
	mov 0x0aaa1023, r9
	mov 0x0012048b, r10


	sst.b r1, 0x0[ep]
	sst.b r1, 0x1[ep]
	sst.h r1, 0x2[ep]

	sst.b r5, 0x4[ep]		#signed
	sst.h r2, 0x8[ep]		#signed
	sst.w r7, 0xc[ep]		#signed

	sst.w r9, 0xfc[ep]		

#----------------------SHORT LOAD INSTRUCTIONS---------------

					#sld.b disp7[ep], r1
					#sld.bu disp8[ep], r1
					#sld.h disp8[ep], r3
					#sld.hu disp8[ep], r1
					#sld.w disp8[ep], r4
	sld.w 0x0[ep], r10

	sld.b 0x4[ep], r5
	sld.bu 0x4[ep], r6
	sld.h 0x4[ep], r7
	sld.h 0x0[ep], r8
	sld.hu 0x0[ep], r9
	sld.h 0x8[ep], r10		#this should be sign extended
	sld.hu 0x8[ep], r11
	sld.w 0x0[ep], r12
	sld.w 0xc[ep], r13
	sld.w 0xfc[ep], r14

Lbl: br Lbl
