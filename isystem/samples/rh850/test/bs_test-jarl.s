.include "RH850G3M_insts.s"
.include "gpr_init.s"

#-----JARL(1)-----32bit------------------------- OK

	        mov -1, R4
	        JARL lab1a, R2
lab1b:      add 1, R4
			bne out1
	        JARL lab1b, R2
lab1a:		jmp [R2]
out1:		nop

#-----JARL(2)---48bit--------------------------- OK

	        mov -1, R4
	        #JARL_ lab2a-.text, R2  ne dela
	        JARL_ 16, R2
lab2b:      add 1, R4
	        bne	out2
	        # JARL_ lab2b-.text, R2 ne dela
	        JARL_ -4, R2
lab2a:		jmp [R2]
out2:		nop

#-----JARL(3)----32bit-------------------------- OK

			movea lab3a-.text, R0, R3
	        mov -1, R4
	        JARL_RR R3, R2
lab3b:		add 1, R4
	        bne	out3
	        movea lab3b-.text, R0, R3
	        JARL_RR R3, R2
lab3a:		jmp [R2]
out3:		nop

Lbl:		BR Lbl

