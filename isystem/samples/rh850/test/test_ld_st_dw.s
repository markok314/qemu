.include "RH850G3M_insts.s"

start = 0xfee00000

.text
		mov start, R1
		movea 0, R1, R2

#-----------------------------------ld.dw -> macro LD_DW---------OK
/*
		#LSB
		mov 0x123, R3
		st.w R3, 0[R2]
		# MSB
		mov 0xaabbc, R3
		st.w R3, 4[R2]
        LD_DW 0, R2, R4

		#LSB
		mov -250, R3
		st.w R3, 8[R2]
		# MSB
		mov 0x0aabbccd, R3
		st.w R3, 12[R2]
        LD_DW 8, R2, R6
*/
#-----------------------------------st.dw -> macro ST_DW---------OK

		mov 0x12345678, R8
		mov 0xabcdef, R9
        ST_DW R8, 16, R2
        LD_DW 16, R2, R4

lbl:    br lbl

