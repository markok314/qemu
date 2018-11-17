.include "RH850G3M_insts.s"

.text

#-----JARL(1)------------------------------ OK
/*
			mov 1, R3
	        mov 2, R3
	        JARL 6, R2
	        mov 3, R3
	        mov 4, R3
	        mov 5, R3
	        mov 6, R3
	        mov 7, R3
	        JARL -8, R2
*/

#-----JARL(2)------------------------------ OK
/*
			mov 1, R3
	        mov 2, R3
	        JARL_ 6, R2
	        mov 3, R3
	        mov 4, R3
	        mov 5, R3
	        mov 6, R3
	        mov 7, R3
	        JARL_ -8, R2
*/

#-----JARL(3)------------------------------ OK

			mov 1, R3
	        mov 10, R3
	        JARL_RR [R3], R2
	        mov 3, R3
	        mov 4, R3
	        mov 5, R3
	        mov 6, R3
	        mov 16, R3
	        JARL_RR [R3], R2

