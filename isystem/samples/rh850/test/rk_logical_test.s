.text
.include "gpr_init.s"
.include "RH850G3M_insts.s"
	
	mov 0x1, r1
	mov 0xf0, r2
	mov 0x7800, r3
	mov 0xffff, r4
	mov 0xf0f0f0f0, r5
	
	mov 0x12354, r6
	mov 0x945bf110, r7
	mov 0x2389, r8
	mov r6, r9
	mov r7, r10
	mov r8, r11


#--------------------------Testing AND instruction----

	and r1, r6	# Z flag is set
	and r2, r8
	and r3, r7
	and r4, r9
	and r5, r10	# S flag is set
	and r3, r11 
	and r0, r10	# Z flag is set

#--------------------------Testing ANDI instruction----

	mov 0xffffffff, r1
	mov 0x12abcd, r2
	mov 0x421, r3
	mov 0x87654321, r4
	mov 0x456689, r5
	
	andi 0xffff, r1, r6
	andi 0xf000, r2, r7
	andi 0x0017, r3, r8
	andi 0x0ff0, r4, r9
	andi 0xcccc, r5, r10
	andi 0x0, r7, r7	# Z flag is set
	


#--------------------------Testing NOT instruction----

	not r1, r6	# Z flag is set
	not r2, r7	# S flag is set
	not r3, r8	# S flag is set
	not r4, r9
	not r5, r10	# S flag is set
	not r0, r11	# S flag is set

#--------------------------Testing OR instruction----

	or r0, r1	# S flag is set
	or r1, r1	# S flag is set
	or r1, r6	# S flag is set
	or r3, r2
	or r9, r3
	or r0, r12	# Z flag is set
	or r8, r9	# S flag is set
	or r4, r5	# S flag is set

#--------------------------Testing ORI instruction----

	mov 0xffffffff, r1
	mov 0x12abcd, r2
	mov 0x421, r3
	mov 0x87654321, r4
	mov 0x456689, r5

	ori 0x4623, r2, r6
	ori 0x2, r4, r7		# S flag is set
	ori 0x200, r3, r8
	ori 0xffff, r3, r9
	ori 0x780, r4, r10	# S flag is set
	ori 0xf000, r3, r11
	ori 0x0, r0, r15	# Z flag is set

#--------------------------Testing TST instruction----

	tst r0, r0	# Z flag is set
	tst r0, r1	# Z flag is set
	tst r2, r3
	tst r5, r4
	tst r2, r6
	tst r1, r1	# S flag is set

#--------------------------Testing XOR instruction----

	mov 0xffffffff, r1
	mov 0x12abcd, r2
	mov 0x421, r3
	mov 0x87654321, r4
	mov 0x456689, r5
	mov 0xffff0000, r6
	
	xor r1, r4
	xor r2, r3
	xor r4, r3
	xor r1, r1	# Z flag is set
	xor r5, r2
	xor r0, r7	# S flag is set

#--------------------------Testing XORI instruction----

	xori 0xf000, r8, r12
	xori 0x0, r14, r14	# Z flag is set
	xori 0xffff, r7, r9	# S flag is set
	xori 0xabcd, r11, r11
	xori 0xffff, r6, r1	# S flag is set
	xori 0x00ff, r1, r1


Lbl: br Lbl
