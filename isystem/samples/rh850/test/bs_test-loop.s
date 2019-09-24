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
lbl_A:
		nop	
		LOOP R1, lo(. - lbl_A - 2)

		mov 3, R1
lbl_loop:
		nop
		add R1, R2
		nop
		LOOP R1, lo(. - lbl_loop - 2)  # r1=3-1=2 != 0, pc-=6, r1=1, pc-=6, r1=0, no loop

		mov 4, R2
lbl_B:
		add 10, R10
		sub R4, R2	
		LOOP R2, lo(. - lbl_B - 2)

		add 10, R10
# now test that loop resets PSW.S

        mov 3, r7
        addi 0xfff0, r7, r6

        mov 3, r7
        addi 0xffff, r7, r6

        mov 2, r22
lbl_S:
        mov 5, r9
        add 0x10, r9   # sets PSW.S, because result is negative (imm5 < 0)
        LOOP R22, lo(. - lbl_S - 2)

Lbl:	br Lbl

