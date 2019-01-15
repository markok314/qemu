
.include "RH850G3M_insts.s"
.include "gpr_init.s"


## Tests for branch instructions.
## If we reach label1, something has gone wrong.
## This will be indicated by value 0xffffffff in register R10.


	mov 0x1, r1
	mov 0x2, r2
	mov 0xffffffff, r3
	mov 0x80000000, r4
	mov 0x7fffffff, r5
	mov hilo(jarlBranch1), r6
	mov hilo(jarlBranch2), r8
	

	cmp r1, r1	# Z flag set

	bgt label1
	blt label1
	bl label1
	bne label1
	bc label1
	bf label1
	bn label1
	bsa label1
	bv label1

	bt branch1

return1:
	tst r3, r3	# S flag set

	bge label1
	bgt label1
	bl label1
	bnh label1
	be label1
	bc label1
	bp label1
	bt label1
	bsa label1
	bv label1
	bz label1

	ble branch2

return2:
	sub r1, r4	# OV flag set

	bge label1
	bgt label1
	bl label1
	bnh label1
	be label1
	bc label1
	bn label1
	bnv label1
	bt label1
	bsa label1
	bz label1

	bv branch3

return3:
	sub r3, r1	# CY flag
	
	ble label1
	blt label1
	bh label1
	bnl label1
	be label1
	bn label1
	bnc label1
	bt label1
	bsa label1
	bz label1
	bv label1

	bl branch4

return4:

	JARL_RR R6, R7
	mov 0x11111111, r24
	jmp 0x4[r8]
	


#--------------------------------------------------------------------------

	br Lbl


label1:				#this label shoud never be reached
	mov 0xffffffff, r10




branch1:
	mov 0x11111111, r20
	br return1

branch2:
	mov 0x11111111, r21
	br return2

branch3:
	mov 0x11111111, r22
	br return3

branch4:
	mov 0x11111111, r23
	br return4


jarlBranch1:
	jmp [r7]		# use link register to jump back
	mov 0xffffffff, r10

jarlBranch2:
	mov 0xffff, r10		# this should be skipped
	mov 0x11111111, r25

Lbl:	br Lbl
