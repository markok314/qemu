.include "RH850G3M_insts.s"

.text

#-----JR disp22------------------------------ OK
/*
			mov 1, R3
	        mov 2, R3
	        JR 4
	        mov 3, R3
	        mov 4, R3
	        mov 5, R3
	        mov 6, R3
	        mov 7, R3
	        JR -8
*/

#-----JR disp32-------------------------------

			mov 1, R3
	        mov 2, R3
	        JR_ 4
	        mov 3, R3
	        mov 4, R3
	        mov 5, R3
	        mov 6, R3
	        mov 7, R3
	        JR_ -8
