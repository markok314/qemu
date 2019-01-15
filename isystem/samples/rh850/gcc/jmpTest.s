.include "gpr_init.s"

.text


	mov 0xfee00022, r1
	#mov 0xfff00000, r5
	#add r5, r12
	#mov 0xabc, r3
	#mov 0xaabb, r4
	#st.h r4, 0x0[r1]
	#st.b r3, 0xab[r1]
	#mov 0xaabb, r2
	#caxi r1, r2, r5		#put 0xaabb in r2
	#JARL [R4], R10
	mov 0x8, r31
	st.w r31, 0x0[r1]
	mov 0xfee00022, r3
	ld.w 0x0[r1], r1
	mov 0x1, r20
	mov 0x2, r21
	mov 0x3, r22
	mov 0x4, r23
	mov 0x5, r24
	mov 0x6, r25
	mov 0xa, r5
	mov 0xb, r6
	mov 0xc, r7
	PUSHSP R5 R7
	#prepare 0xfff, 0x0

	mov 0x0, r5
	mov 0x0, r6
	mov 0x0, r7

	mov 0xfff, r20
	mov 0xffff, r21
	mov 0xfffff, r22
	mov 0xffff, r23
	mov 0xfff, r24
	mov 0xff, r25
	POPSP R5 R7



	dispose 0x0, 0xfff


	#ld.w 0x0[r1], r20
	#add r1, r5
	#add r2, r5
	#ROTL_i 0x8, R3, R7
	#ROTL_r R2, R4, R5
	#jmp r4
	
	#add r3, r5
	#add r4, r5	
	#add r5, r5


#.org 0xaabb

	
	#ld.bu 0xab[r1], r10
	#mov 0x2, r3

	

SELF:
   	br SELF
