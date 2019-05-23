.include "RH850G3M_insts.s"

.data
stack:		.word 0, 0, 0, 0, 0 	# last 0 has address stack+16
start = 0xfee00000

.include "gpr_init.s"

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

		DISPOSE 1, 0x401
		nop

		mov 1, r20
		mov 2, r21
		mov 3, r22
		mov 4, r23
		mov 5, r24
		mov 6, r25
		mov 7, r26
		mov 8, r27
		mov 9, r28
		mov 10, r29
		mov 11, r30
		mov 12, r31
		PREPARE 0x401, 0x10
		DISPOSE 0x10, 0x401
		nop

Lbl:	BR Lbl


