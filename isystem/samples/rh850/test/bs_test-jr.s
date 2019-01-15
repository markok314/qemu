.include "RH850G3M_insts.s"
.include "gpr_init.s"


#-----JR disp22------------------------------ OK

			mov -1, R4
	        nop
	        JR lab1		# 32-bit command
	        nop
lab1:       nop
	        add 1, R4
	        bne lab2
	        nop
	        JR -8
lab2:		nop

#-----JR disp32-------------------------------

/*
It works, and PC is first incremented by 6 (which is OK),
but later it is somewhere decremented by 2 (or incremented again by 4),
for some reason I don't know ... ?!
*/
			mov -1, R4
	        nop
	        #JR_ lab3		# 48-bit command
			JR_ 8
	        nop
lab3:       nop
	        add 1, R4
	        bne lab4
	        nop
	        JR_ -8
lab4:		nop

Lbl:		BR Lbl
