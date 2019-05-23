.include "RH850G3M_insts.s"

.data
lock_adr: .word		0, 0, 0
start = 0xfee00000

.include "gpr_init.s"


		mov start, R1
		movea lock_adr - .data, R1, R20
		mov 0, R21
		mov 0x1f, R3		# R3 = 0x1f
        st.w R3, 0[R20]		# M[lock_adr] = 0x1f
        ld.w 0[R20], R4		# R4 = 0x1f

	 	LDL_W [R20], R21	# R21 = 0x1f, LLbit = 1
        #st.w R3, 0[R20]	# Does st.w clear LLbit?
		CLL
		STC_W R21, [R20]	# STC.W fails (R21 = 0)

	 	LDL_W R20, R21
	 	mov 6, R2
		STC_W R21, R20		# STC.W suceeds (R21 = 1)

	 	LDL_W R20, R21
	 	addi 4, R20, R20
		STC_W R21, R20		# STC.W fails (due to different address)
	 	addi -4, R20, R20

	 	LDL_W R20, R21
	 	CLL
		STC_W R21, R20		# STC.W fails (due to CLL)

Lbl:	BR Lbl

