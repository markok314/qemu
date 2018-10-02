.text
.include "gpr_init.s"
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

			
	Lbl: br Lbl
