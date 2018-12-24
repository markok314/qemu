.include "RH850G3M_insts.s"
.include "gpr_init.s"

	
#----------------------Testing the SWITCH instruction-----
	mov 0x0000001a, r1
	mov 0xfee00000, r2
	mov 0x0fee0000, r3
	st.w r1, 0[r2]

	SWITCH r3
	
	mov 0xffffffff, r7
	#this should be jumped over

	mov 0x11111111, r7
	mov 0x22222222, r7
	
Lbl:	br Lbl
