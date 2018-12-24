.include "RH850G3M_insts.s"
.include "gpr_init.s"

#-----------------tests for div------------------#
	
	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	div r2,r1,r3	
	div r1,r2,r4

	#simple tests with quotient 0	
	mov 0,r1
	mov 2,r2
	div r2,r1,r3	
	div r1,r2,r4
	#there is also z flag

	#simple tests with quotient 1	
	mov 10,r1
	mov 10,r2
	div r2,r1,r3	
	div r1,r2,r4
	#there is s flag 

	#simple test with reminder
	mov 11,r1
	mov 2,r2
	div r2,r1,r6
	div r1,r2,r7
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	div r2,r1,r3	
	div r1,r2,r4
	#here should be s flag

	mov 11,r1
	mov -2,r2
	div r2,r1,r6
	div r1,r2,r7
	#here should be s flag

	mov -10,r1
	mov -2,r2
	div r2,r1,r3	
	div r1,r2,r4
	#here shouldnt be s flag

	#trying to divide by zero
	mov 0xffff0000,r11
	mov 0x0,r12
	div r12,r11,r4
	div r11,r12,r5
	#result should be ov flag
	
	#overflow occurs here
	mov 0x80000000,r6
	mov -1,r7
	div r7,r6,r4
	div r6,r7,r6
	mov -0x80000000,r6
	mov -1,r7
	div r7,r6,r4
	div r6,r7,r6

	#trying dividing bigger numbers
	mov 0x12345678,r11
	mov 0x12345678,r12
	div r12,r11,r4

	mov 0xffffffff,r11
	mov 0x12345678,r12
	div r12,r11,r4
	
	#trying with the same r2 and r3

	# If general-purpose registers reg2 and reg3 are specified 
	# as being the same register, the operation result
	# quotient is not stored in reg2, so the flag is undefined.

	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	div r2,r1,r1	
	div r1,r2,r2
	#there is also z flag

	#simple test with reminder
	mov 11,r1
	mov 2,r2
	div r2,r1,r1
	div r1,r2,r2
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	div r2,r1,r1
	div r1,r2,r2
	#here should be s flag

	mov 11,r1
	mov -2,r2
	div r2,r1,r1
	div r1,r2,r2
	#here should be s flag

	mov -10,r1
	mov -2,r2
	div r2,r1,r1	
	div r1,r2,r2
	#here shouldnt be s flag

#-----------------tests for DIVH reg1, reg2------------------#

	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	divh r2,r1	
	divh r1,r2
	#there is also z flag
	
	#simple tests with quotient 0	
	mov 0,r1
	mov 2,r2
	divh r2,r1	
	divh r1,r2

	#simple tests with quotient 1	
	mov 10,r1
	mov 10,r2
	divh r2,r1	
	divh r1,r2

	#simple test with reminder
	mov 11,r1
	mov 2,r2
	divh r2,r1
	divh r1,r2
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	divh r2,r1	
	divh r1,r2
	#here should be s flag

	mov 11,r1
	mov -2,r2
	divh r2,r1
	divh r1,r2
	#here should be s flag

	mov -10,r1
	mov -2,r2
	divh r2,r1	
	divh r1,r2
	
	#trying to divide by zero
	mov 0xffff0000,r11
	mov 0x0,r12
	divh r12,r11
	divh r11,r12
	#result should be ov flag
	
	#overflow occurs here
	mov 0x80000000,r6
	mov -1,r7
	divh r7,r6
	divh r6,r7
	mov -0x80000000,r6
	mov -1,r7
	divh r7,r6
	divh r6,r7

	#trying dividing bigger numbers
	mov 0x12345678,r11
	mov 0x12345678,r12
	divh r12,r11

	mov 0xffffffff,r11
	mov 0x12345678,r12
	divh r12,r11

#-----------------tests for DIVH reg1, reg2, reg3------------------#

	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	divh r2,r1,r3	
	divh r1,r2,r4
	#there is also z flag

	
	#simple tests with quotient 0	
	mov 0,r1
	mov 2,r2
	divh r2,r1,r3	
	divh r1,r2,r4
	#there is also z flag

	#simple tests with quotient 1	
	mov 10,r1
	mov 10,r2
	divh r2,r1,r3	
	divh r1,r2,r4
	#there is s flag 


	#simple test with reminder
	mov 11,r1
	mov 2,r2
	divh r2,r1,r6
	divh r1,r2,r7
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	divh r2,r1,r3	
	divh r1,r2,r4
	#here should be s flag

	mov 11,r1
	mov -2,r2
	divh r2,r1,r6
	divh r1,r2,r7
	#here should be s flag

	mov -10,r1
	mov -2,r2
	divh r2,r1,r3	
	divh r1,r2,r4
	#here shouldnt be s flag

	#trying to divide by zero
	mov 0xffff0000,r11
	mov 0x0,r12
	divh r12,r11,r4
	divh r11,r12,r5
	#result should be ov flag
	
	#overflow occurs here
	mov 0x80000000,r6
	mov -1,r7
	divh r7,r6,r4
	divh r6,r7,r6
	mov -0x80000000,r6
	mov -1,r7
	divh r7,r6,r4
	divh r6,r7,r6

	#trying dividing bigger numbers
	mov 0x12345678,r11
	mov 0x12345678,r12
	divh r12,r11,r4

	mov 0xffffffff,r11
	mov 0x12345678,r12
	divh r12,r11,r4
	
	#trying with the same r2 and r3

	# If general-purpose registers reg2 and reg3 are specified 
	# as being the same register, the operation result
	# quotient is not stored in reg2, so the flag is undefined.

	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	divh r2,r1,r1	
	divh r1,r2,r2
	#there is also z flag

	#simple test with reminder
	mov 11,r1
	mov 2,r2
	divh r2,r1,r1
	divh r1,r2,r2
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	divh r2,r1,r1
	divh r1,r2,r2

	mov 11,r1
	mov -2,r2
	divh r2,r1,r1
	divh r1,r2,r2

	mov -10,r1
	mov -2,r2
	divh r2,r1,r1	
	divh r1,r2,r2

#-----------------tests for DIVHU reg1, reg2, reg3------------------#

	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	divhu r2,r1,r3	
	divhu r1,r2,r4
	#there is also z flag

	#simple tests with quotient 0	
	mov 0,r1
	mov 2,r2
	divhu r2,r1,r3	
	divhu r1,r2,r4
	#there is also z flag

	#simple tests with quotient 1	
	mov 10,r1
	mov 10,r2
	divhu r2,r1,r3	
	divhu r1,r2,r4
	#there is s flag 

	#simple test with reminder
	mov 11,r1
	mov 2,r2
	divhu r2,r1,r6
	divhu r1,r2,r7
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	divhu r2,r1,r3	
	divhu r1,r2,r4
	#here shouldnt be s flag

	mov 11,r1
	mov -2,r2
	divhu r2,r1,r6
	divhu r1,r2,r7
	#here shouldnt be s flag

	mov -10,r1
	mov -2,r2
	divhu r2,r1,r3	
	divhu r1,r2,r4
	#here shouldnt be s flag

	#trying to divide by zero
	mov 0xffff0000,r11
	mov 0x0,r12
	divhu r12,r11,r4
	divhu r11,r12,r5
	#result should be ov flag
	
	#overflow occurs here
	mov 0x80000000,r6
	mov -1,r7
	divhu r7,r6,r4
	divhu r6,r7,r6
	mov -0x80000000,r6
	mov -1,r7
	divhu r7,r6,r4
	divhu r6,r7,r6

	#trying dividing bigger numbers
	mov 0x12345678,r11
	mov 0x12345678,r12
	divhu r12,r11,r4

	mov 0xffffffff,r11
	mov 0x12345678,r12
	divhu r12,r11,r4
	
	#trying with the same r2 and r3

	# If general-purpose registers reg2 and reg3 are specified 
	# as being the same register, the operation result
	# quotient is not stored in reg2, so the flag is undefined.

	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	divhu r2,r1,r1	
	divhu r1,r2,r2
	#there is also z flag

	#simple test with reminder
	mov 11,r1
	mov 2,r2
	divhu r2,r1,r1
	divhu r1,r2,r2
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	divhu r2,r1,r1
	divhu r1,r2,r2

	mov 11,r1
	mov -2,r2
	divhu r2,r1,r1
	divhu r1,r2,r2

	mov -10,r1
	mov -2,r2
	divhu r2,r1,r1	
	divhu r1,r2,r2

#-----------------tests for DIVU reg1, reg2, reg3------------------#

	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	divu r2,r1,r3	
	divu r1,r2,r4

	#simple tests with quotient 0	
	mov 0,r1
	mov 2,r2
	divu r2,r1,r3	
	divu r1,r2,r4
	#there is also z flag

	#simple tests with quotient 1	
	mov 10,r1
	mov 10,r2
	divu r2,r1,r3	
	divu r1,r2,r4
	#there is no s flag 

	mov 0xffffffff,r1
	mov 0x1,r2
	divu r2,r1,r3
	divu r1,r2,r4
	#here should be s flag

	#simple test with reminder
	mov 11,r1
	mov 2,r2
	divu r2,r1,r6
	divu r1,r2,r7
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	divu r2,r1,r3	
	divu r1,r2,r4
	#here should be no s flag

	mov 11,r1
	mov -2,r2
	divu r2,r1,r6
	divu r1,r2,r7
	#here should be no s flag

	mov -10,r1
	mov -2,r2
	divu r2,r1,r3	
	divu r1,r2,r4
	#here shouldnt be s flag

	#trying to divide by zero
	mov 0xffff0000,r11
	mov 0x0,r12
	divu r12,r11,r4
	divu r11,r12,r5
	#result should be ov flag
	
	#overflow occurs here
	mov 0x80000000,r6
	mov -1,r7
	divu r7,r6,r4
	divu r6,r7,r6
	mov -0x80000000,r6
	mov -1,r7
	divu r7,r6,r4
	divu r6,r7,r6

	#trying dividing bigger numbers
	mov 0x12345678,r11
	mov 0x12345678,r12
	divu r12,r11,r4

	mov 0xffffffff,r11
	mov 0x12345678,r12
	divu r12,r11,r4
	
	#trying with the same r2 and r3

	# If general-purpose registers reg2 and reg3 are specified 
	# as being the same register, the operation result
	# quotient is not stored in reg2, so the flag is undefined.

	#simple tests with no reminder	
	mov 10,r1
	mov 2,r2
	divu r2,r1,r1	
	divu r1,r2,r2

	#simple test with reminder
	mov 11,r1
	mov 2,r2
	divu r2,r1,r1
	divu r1,r2,r2
	
	#simple test with negative 
	mov -10,r1
	mov 2,r2
	divu r2,r1,r1
	divu r1,r2,r2
	#here shouldt be s flag

	mov 11,r1
	mov -2,r2
	divu r2,r1,r1
	divu r1,r2,r2
	#here shouldnt be s flag

	mov -10,r1
	mov -2,r2
	divu r2,r1,r1	
	divu r1,r2,r2
	#here shouldnt be s flag

	mov 0xffffffff,r1
	mov 0x1,r2
	divu r2,r1,r1
	divu r1,r2,r2

Lbl:	br Lbl
	#here should be s flag
	
	
	
