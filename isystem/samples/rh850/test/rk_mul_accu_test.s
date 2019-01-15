.include "RH850G3M_insts.s"
.include "gpr_init.s"


	mov 0x11224, r1
	mov 0x5b32, r2
	mov -0x2346, r3
	mov 0x736fb24a, r4
	mov 0xaab43111, r5 	

	mov 0x43, r6
	mov -0x2345, r7
	mov -0xff, r8
	mov 0x5423, r9


#--------------------------Testing MAC instruction------

	mac r1, r2, r6, r10
	mac r1, r3, r2, r12
	mac r2, r3, r6, r14
	mac r3, r4, r8, r16
	mac r3, r5, r8, r18
	mac r5, r4, r6, r20 
	mov 0x123, r7
	mac r17, r19, r6, r22

#--------------------------Testing MACU instruction------

	#  Testing on same values
	macu r1, r2, r6, r10
	macu r1, r3, r2, r12
	macu r2, r3, r6, r14
	macu r3, r4, r8, r16
	macu r3, r5, r8, r18
	macu r5, r4, r6, r20
	macu r17, r19, r6, r22

#--------------------------Additional combined tests------

	mac r2, r3, r6, r10
	macu r2, r3, r6, r10
	mac r17, r13, r10, r12
	macu r14, r8, r2, r14
	macu r18, r23, r24, r24


Lbl: br Lbl
