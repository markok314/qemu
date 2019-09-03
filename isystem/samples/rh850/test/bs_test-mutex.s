# This test fails, because test hardware does not match documentation - these
# instructions are implemented on multicore devices only.
# Register (R21 in code below) is not modified to contain 0 or 1 on STC.W, 
# but remains unchanged.


.include "RH850G3M_insts.s"

.data
lock_adr: .word		0, 0, 0

.include "gpr_init.s"


		mov     hilo(lock_adr), r20
		mov     0, r21
		mov     0x45, r3
        st.w    r3, 0[r20]		# [lock_adr] = 0x1f
        ld.w    0[r20], r4		# r4 = 0x1f

	 	LDL_W   R20, R21    # R21 = 0x1f, LLbit = 1
		STC_W   R21, R20    # STC.W fails (R21 = 0)

	 	LDL_W   R20, R21
		CLL
		STC_W   R21, R20		# STC.W suceeds (R21 = 1)

	 	LDL_W   R20, R21
	 	addi    4, r20, r20
		STC_W   R21, R20		# STC.W fails (due to different address)
	 	addi    -4, r20, r20

Lbl:	br Lbl

