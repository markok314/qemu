.include "RH850G3M_insts.s"
.include "gpr_init.s"
	
#----------------------Testing the MAC instruction-----

	#some simple tests for mac

	mov 0x5,r2
	mov 0x6,r3
	mov 0x1,r4
	mov 0xfff,r6

	mov -6,r12
	mov 1,r13
	mov 0xffffffff,r14
	mov 0xffff0000,r16

	mac r2,r3,r4,r6
	mac r12,r13,r14,r16
	
	mov 0xffffffff,r2
	mov -0xfffffff,r3
	mov 0xffffffff,r4
	mov 0xffff0000,r6

	mov 0xffffffff,r12
	mov 0xffffffff,r13
	mov 0xffffffff,r14
	mov 0xffff0000,r16

	mac r2,r3,r4,r6
	mac r12,r13,r14,r16
	mac r6,r4,r2,r4
	mac r16,r13,r14,r12

	#testing with 0 in regs

	mov 0x0,r2
	mov 0x0,r3
	mov 0xffffffff,r4
	mov 0xffff0000,r6

	mov 0xffffffff,r12
	mov 0x0,r13
	mov 0x0,r14
	mov 0xffff0000,r16

	mac r2,r3,r4,r6
	mac r12,r13,r14,r16

	#testing for signed numbers 

	mov -0x1fff,r2
	mov -0xfffffff,r3
	mov 0xffffffff,r4
	mov 0xffff0000,r6

	mov -0xf15,r12
	mov 0x534,r13
	mov 0xffffffff,r14
	mov 0xffff0000,r16

	mac r2,r3,r4,r6
	mac r12,r13,r14,r16
	mac r6,r4,r2,r4
	mac r16,r13,r14,r12

	#what if two regs in same?

	mac r2,r3,r4,r4
	mac r2,r3,r4,r4
	mac r12,r13,r14,r14
	mac r6,r4,r2,r2
	mac r16,r13,r14,r14

	#testing influence on r4 + 1 reg

	mov 0xffffffff,r2
	mov -0xfffffff,r3
	mov 0xffffffff,r4
	mov 0xffff0000,r6
	mov 0xffff0000,r7

	mov 0xffffffff,r12
	mov 0xffffffff,r13
	mov 0xffffffff,r14
	mov 0xffff0000,r16
	mov 0x0000ffff,r17

	mac r2,r3,r4,r6
	mac r12,r13,r14,r16

#----------------------Testing the MACU instruction-----

	#some simple tests for macu

	mov 0x5,r2
	mov 0x6,r3
	mov 0x1,r4
	mov 0xfff,r6

	#what if we have negative(signed numbers in macu?
	mov -6,r12
	mov 1,r13
	mov 0xffffffff,r14
	mov 0xffff0000,r16

	macu r2,r3,r4,r6
	macu r12,r13,r14,r16
	
	mov 0xffffffff,r2
	mov -0xfffffff,r3
	mov 0xffffffff,r4
	mov 0xffff0000,r6

	mov 0xffffffff,r12
	mov 0xffffffff,r13
	mov 0xffffffff,r14
	mov 0xffff0000,r16

	macu r2,r3,r4,r6
	macu r12,r13,r14,r16
	macu r6,r4,r2,r4
	macu r16,r13,r14,r12

	#testing with 0 in regs

	mov 0x0,r2
	mov 0x0,r3
	mov 0xffffffff,r4
	mov 0xffff0000,r6

	mov 0xffffffff,r12
	mov 0x0,r13
	mov 0x0,r14
	mov 0xffff0000,r16

	macu r2,r3,r4,r6
	macu r12,r13,r14,r16

	#testing for signed numbers 

	mov -0x1fff,r2
	mov -0xfffffff,r3
	mov 0xffffffff,r4
	mov 0xffff0000,r6

	mov -0xf15,r12
	mov 0x534,r13
	mov 0xffffffff,r14
	mov 0xffff0000,r16

	macu r2,r3,r4,r6
	macu r12,r13,r14,r16
	macu r6,r4,r2,r4
	macu r16,r13,r14,r12

	#what if two regs in same?

	macu r2,r3,r4,r4
	macu r2,r3,r4,r4
	macu r12,r13,r14,r14
	macu r6,r4,r2,r2
	macu r16,r13,r14,r14

	#testing influence on r4 + 1 reg

	mov 0xffffffff,r2
	mov 0xfffffff,r3
	mov 0xffffffff,r4
	mov 0xffff0000,r6
	mov 0xffff0000,r7

	mov 0xffffffff,r12
	mov 0xffffffff,r13
	mov 0xffffffff,r14
	mov 0xffff0000,r16
	mov 0x0000ffff,r17

	macu r2,r3,r4,r6
	macu r12,r13,r14,r16



Lbl:	br Lbl
