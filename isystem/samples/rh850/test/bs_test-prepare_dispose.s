#.include "RH850G3M_insts.s"

.data
stack:		.word 0, 0, 0, 0, 0 	# last 0 has address stack+16

start = 0xfee00000

.text

        mov start, r1
        movea stack - .data, r1, r2
		addi 16, r2, r2		# r2 = stack+16
		mov r2, sp			# sp = stack+16
		mov 6, r20			#
		mov 7, r30
		PREPARE 0x401, 1	# r20 and r30 are copied to stack+12 and stack+8,
								# sp = sp -2*4 - 1*4 = sp - 12 = stack+4
		ld.w 4[sp], r4
		ld.w 8[sp], r5

		#DISPOSE imm5, list12

