.include "RH850G3M_insts.s"
.include "gpr_init.s"

	#sub max negative number 1 and adf 1
	#checking if this set some flags or not

	mov 0x7fffffff, r17
	mov 0x1, r18
	add r17, r18

	#testing for v = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0xffffffff,r1
	mov 0x7fffffff,r2
	sbf v,r1,r2,r3
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x80000000,r1
	mov 0xffffffff,r2
	adf nv,r1,r2,r4

	#testing for v = 0
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x80000000,r1
	mov 0xffffffff,r2
	adf v,r1,r2,r5
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x80000000,r1
	mov 0xffffffff,r2
	adf nv,r1,r2,r6

	Lbl: br Lbl
