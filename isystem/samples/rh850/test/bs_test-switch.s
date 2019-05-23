.include "RH850G3M_insts.s"
.include "gpr_init.s"


#------SWITCH---------------------------- OK

		mov 7, r1
		switch r1
		mov 1, r2
		mov 2, r2
		mov 4, r1
		switch r1
		mov 3, r2
		mov 4, r2
		br 6
LUT:	.hword	2, 0
		mov 5, r2
		mov 6, r2
		nop

Lbl:	BR Lbl
