.text
.include "gpr_init.s"

#--------tests for DIVQ-----------------------------	

	#few simple cases
	mov 5,r1	
	mov 2,r2
	divq r2,r1,r3
	divq r1,r2,r3

	mov 0xffffffff,r1	
	mov 2,r2
	divq r2,r1,r3
	divq r1,r2,r3
	
	mov 0xffffffff,r1	
	mov 0x55555555,r2
	divq r2,r1,r3
	divq r1,r2,r3

	#checking ov flag
	mov -0x80000000,r10
	mov -0x1,r11
	divq r11,r10,r6
	
	mov 0x80000000,r15
	mov 0xffffffff,r14
	divq r14,r15,r12
	
	#checking s flag
	mov 0x80000000,r15
	mov 0x5,r14
	divq r14,r15,r12

	mov -10,r9
	mov 5,r10
	divq r10,r9,r12
	
	mov -15,r6
	mov -5,r10
	divq r6,r10,r10
	
	mov 10,r9
	mov -5,r10
	divq r10,r9,r12
	
	#checking z flag
	mov 0x0,r1
	mov 0x6,r2
	divq r2,r1,r10

	mov 0x0,r1
	mov 0xfffffff,r2
	divq r2,r1,r10

	#checking with the same r2 and r3
	
	#few simple cases
	mov 5,r1	
	mov 2,r2
	divq r2,r1,r1
	divq r1,r2,r2

	mov 0xffffffff,r1	
	mov 2,r2
	divq r2,r1,r1
	divq r1,r2,r2
	
	mov 0xffffffff,r1	
	mov 0x55555555,r2
	divq r2,r1,r1
	divq r1,r2,r2

	#checking ov flag
	mov -0x80000000,r10
	mov -0x1,r11
	divq r11,r10,r10
	
	mov 0x80000000,r15
	mov 0xffffffff,r14
	divq r14,r15,r15
	
	#checking s flag
	mov 0x80000000,r15
	mov 0x5,r14
	divq r14,r15,r15

	mov -10,r9
	mov 5,r10
	divq r10,r9,r9
	
	mov -15,r6
	mov -5,r10
	divq r6,r10,r9
	
	mov 10,r9
	mov -5,r10
	divq r10,r9,r9
	
	#checking z flag
	mov 0x0,r1
	mov 0x6,r2
	divq r2,r1,r1

	mov 0x0,r1
	mov 0xfffffff,r2
	divq r2,r1,r1

	
#--------tests for DIVQU-----------------------------

	
	#few simple cases
	mov 5,r1	
	mov 2,r2
	divqu r2,r1,r3
	divqu r1,r2,r3

	mov 0xffffffff,r1	
	mov 2,r2
	divqu r2,r1,r3
	divqu r1,r2,r3
	
	mov 0xffffffff,r1	
	mov 0x55555555,r2
	divqu r2,r1,r3
	divqu r1,r2,r3

	#checking ov flag
	mov -0x80000000,r10
	mov -0x1,r11
	divqu r11,r10,r6
	
	mov 0x80000000,r15
	mov 0xffffffff,r14
	divqu r14,r15,r12
	
	#checking s flag
	mov 0x80000000,r15
	mov 0x5,r14
	divqu r14,r15,r12

	mov -10,r9
	mov 5,r10
	divqu r10,r9,r12
	
	mov -15,r6
	mov -5,r10
	divqu r6,r10,r10
	
	mov 10,r9
	mov -5,r10
	divqu r10,r9,r12
	
	#checking z flag
	mov 0x0,r1
	mov 0x6,r2
	divqu r2,r1,r10

	mov 0x0,r1
	mov 0xfffffff,r2
	divqu r2,r1,r10

	#checking with the same r2 and r3
	
	#few simple cases
	mov 5,r1	
	mov 2,r2
	divqu r2,r1,r1
	divqu r1,r2,r2

	mov 0xffffffff,r1	
	mov 2,r2
	divqu r2,r1,r1
	divqu r1,r2,r2
	
	mov 0xffffffff,r1	
	mov 0x55555555,r2
	divqu r2,r1,r1
	divqu r1,r2,r2

	#checking ov flag
	mov -0x80000000,r10
	mov -0x1,r11
	divqu r11,r10,r10
	
	mov 0x80000000,r15
	mov 0xffffffff,r14
	divqu r14,r15,r15
	
	#checking s flag
	mov 0x80000000,r15
	mov 0x5,r14
	divqu r14,r15,r15

	mov -10,r9
	mov 5,r10
	divqu r10,r9,r9
	
	mov -15,r6
	mov -5,r10
	divqu r6,r10,r9
	
	mov 10,r9
	mov -5,r10
	divqu r10,r9,r9
	
	#checking z flag
	mov 0x0,r1
	mov 0x6,r2
	divqu r2,r1,r1

	mov 0x0,r1
	mov 0xfffffff,r2
	divqu r2,r1,r1


	#few more tests for s flag, beacuse documentations
	# is little weird about that

	#msb of result is 1 (s flag in divu)
	mov 0xffffffff,r1
	mov 0x1,r2
	divqu r2,r1,r3

	mov 0xffffffff,r1
	mov 0x1,r2
	divqu r2,r1,r1
	
	#result is 1 (s flag in divhu)
	mov 0xffffffff,r1
	mov 0xffffffff,r2
	divqu r2,r1,r3

	mov 0xffffffff,r1
	mov 0xffffffff,r2
	divqu r2,r1,r1
	

	

