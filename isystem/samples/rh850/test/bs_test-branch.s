

.text

	        mov 5, r2
	        mov 6, r3
	        add r2, r3
	        bge lab2
lab1:    	mov 0xfffffffa, r4
        	mov 0x7, r5
        	add r4, r5
        	bge lab3
lab2:       mov 3, r4
        	mov -4, r5
        	add r5, r4
#	        bge lab1
lab3:		add r4, r2
	        bge lab2
	        add r5, r3
#	        bge lab1
	        mov 1, r1





