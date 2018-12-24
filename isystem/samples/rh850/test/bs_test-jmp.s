.include "RH850G3M_insts.s"

.text

#-----JMP------------------------------ OK

			mov 8, R3	# 0, 	R3 = 8
	        jmp [R3]	# 2,	JMP 8
	        mov 1, r4	# 4,	.
	        mov 2, r4	# 6,	.
	        mov 2, R3	# 8,	R3 = 2
	        add 4, R3	# 10,	R3 = 6		R3 = 14
			add 4, R3	# 12,	R3 = 10		R3 = 18
	        jmp [R3]	# 14,	JMP 10		JMP 18
			mov 3, r4	# 16,	.
			mov 36, R3	# 18,	R3 = 36
	        jmp -4[R3]	# 24,	JMP 32
	        mov 1, r4	# 30,	.
			mov 0x1000002e, R3	# 32, R3 = 0x1000002e
	        jmp 0xf0000000[R3]	# 38, JMP 46
	        mov 5, r4	# 44	.
			mov 6, r4	# 46	.
