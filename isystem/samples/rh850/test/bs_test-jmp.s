.include "RH850G3M_insts.s"
.include "gpr_init.s"

#-----JMP------------------------------ OK

			movea lab1-.text, R0, R3
	        jmp [R3]
	        nop
lab1:       mov -1, R4
lab2:       nop
			add 1, R4
			bne lab3
			movea lab2-.text, R0, R3
	        jmp [R3]
lab3:		nop
			movea lab4-.text, R0, R3
			addi 1000, R3, R3
	        jmp -1000[R3]
	        nop
lab4:		nop

Lbl:		BR Lbl

