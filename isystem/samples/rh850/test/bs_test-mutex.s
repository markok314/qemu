.include "RH850G3M_insts.s"

lock_adr: .word		0, 0, 0

start = 0xfee00000

.text
		mov start, R1
		movea lock_adr - .data, R1, R20
		mov 0x1f, R3
        st.w R3, 0[R20]
        ld.w 0[R20], R4

	 	LDL_W R20, R21
        st.w R3, 0[R20]
		STC_W R21, R20		# STC.W fails (R21 = 0)

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

