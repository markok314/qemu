.text
.include "gpr_init.s"

	#testing starts here
	#testing when does processor set flag for cond operations

	#testing for v = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x7ffffff0,r1
	mov 0xf,r2
	adf v,r1,r2,r3
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x7fffff0,r1
	mov 0xf,r2
	adf nv,r1,r2,r4

	#testing for v = 0
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x7ffffff0,r1
	mov 0xf,r2
	adf v,r1,r2,r5
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x7fffff0,r1
	mov 0xf,r2
	adf nv,r1,r2,r6


	#sub max negative number 1 and adf 1
	#checking if this set some flags or not

	#testing for v = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x80000000,r1
	mov 0xffffffff,r2
	adf v,r1,r2,r3
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
	
	#max negative number and add 1 then sbf
	#checking if this set some flags or not

	#checking v and nv condition

	#testing for v = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x7fffffff,r1
	mov 0x1,r2
	sbf v,r1,r2,r3

	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x7fffffff,r1
	mov 0x1,r2
	sbf nv,r1,r2,r4

	#testing for v = 0
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x7fffffff,r1
	mov 0x1,r2
	sbf v,r1,r2,r5

	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x7fffffff,r1
	mov 0x1,r2
	sbf nv,r1,r2,r6

	
	Lbl: br Lbl
	
