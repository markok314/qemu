.include "RH850G3M_insts.s"
.include "gpr_init.s"

#-------LOOP---------------------------------------- OK

		nop
		nop
		mov 1, R4
		mov 3, R1
		mov 5, R2
		addi -1, R1, R3
		add R1, R2
		add 10, R10
		nop	
		loop R1, 2
		mov 3, R1
		nop
		add R1, R2
		nop
#Lbl1:	br Lbl1
		loop R1, 6	# r1=3-1=2 != 0, pc-=6, r1=1, pc-=6, r1=0, no loop
		mov 4, R2	# r2=4
		add 10, R10
		sub R4, R2	# r2=4-1=3, r2=1
		loop R2, 4	# r2=2 != 0 pc-=4, r2=0 == 0, no loop
		add 10, R10
		add 10, R10
Lbl:	br Lbl

