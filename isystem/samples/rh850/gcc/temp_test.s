.text

	mov 0x1, r1
	mov 0xff567546, r2
	mov 0xf, r4
	mov 0xffff, r5
	mov 0x1, r6

	bnz 0xa


	addi 0x1, r1, r1

	movhi 0xff, r2, r3

	dispose 0x1, 0x011f

	
	dispose 0x5, 0x041a, [r5]

	jmp [r6]

	# LDL.W [r5], r2   <- this doesnt work!!

	mac r1, r2, r4, r6
	macu r1, r2, r4, r6



#  gen_set_pc_im
#  use_goto_tb
