.text
.include "gpr_init.s"

	mov 0x5, r1
	mov -0x7, r2
	mov 0xabcd, r3
	mov 0xff55, r4
	mov 0x12345678, r5
	mov 0xffffffff, r6

#--------------------------Testing MUL instruction------

	#-------tests with small unsigned values
	mul r3, r4, r7
	mul r1, r3, r8
	mul 0xf, r3, r9
	mul 0x0, r9, r9
	mul 0x2, r3, r4

	#-------tests with small signed values
	mul r1, r2, r7
	mov -0x7, r2
	mul r3, r2, r8
	mul r5, r6, r10
	mov -0x7, r2
	mov 0xff55, r4
	mov 0xffffffff, r6
	mul r6, r2, r11
	mul r10, r8, r9
	mul r8, r9, r11
	mul -0x5, r8, r9
	mul -0xf, r9, r9

	#-------tests with large values
	mov 0x98765432, r1
	mov 0x86420002, r2
	mov 0x55335, r3
	mov 0x82713, r4
	mov 0x7f472920, r6
	mov 0x7123f24b, r7

	mul r1, r2, r8
	mul r1, r3, r9
	mul r6, r7, r10
	mul r1, r8, r11


#-----------------------------Testing MULH instruction----

	mov 0x1111abcd, r1
	mov 0x2222ff55, r2
	mov 0x12345678, r3
	mov 0xffffffff, r4
	mov 0x12340000, r5
	mov 0xffff0000, r6
	mov 0x00011000, r7

	mulh r1, r2
	mulh r2, r5
	mulh r7, r1
	mulh 0xd, r4
	mulh r4, r7
	mulh -0x1, r3

#-----------------------------Testing MULHI instruction----

	mulhi 0x7fff, r1, r2
	mulhi 0x1234, r2, r3
	mulhi 0x1, r3, r3
	mulhi -0xff, r4, r5
	mulhi 0x0, r10, r7
	mulhi -0x1, r10, r10
	mulhi -0x7777, r2, r5

#------------------------------Testing MULU instruction-----

	mov 0xffffffff, r4
	mov 0x12345678, r5
	
	mulu 0x1ff, r4, r7
	mov 0xffffffff, r4
	mulu -0x1, r4, r7
	mulu r1, r5, r6
	mulu r3, r4, r5
	mulu 0xff, r4, r7
	mulu 0xffff, r7, r9

#------------------------------------------


Lbl: br Lbl
