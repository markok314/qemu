.include "RH850G3M_insts.s"
.include "gpr_init.s"

#----------------Tests for PREPARE-------------------

	mov 0xfee00100, r3	# stack pointer

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



	prepare 0x30f, 0x0
	
	
#----------------Clearing regs-------------------

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

#----------------Tests for DISPOSE-------------------

	
	dispose 0x0, 0x7

#------------------------------------------------






Lbl: br Lbl
