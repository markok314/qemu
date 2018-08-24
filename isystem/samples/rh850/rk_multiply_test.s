   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:


	mov 0x5, r1
	mov -0x7, r2
	mov 0xabcd, r3
	mov 0xff55, r4
	mov 0x12345678, r5
	mov 0xffffffff, r6

;---------------------------------mul------

	mul r1, r2, r7
	mov -0x7, r2
	mul r3, r2, r8
	mul r3, r4, r9
	mul r5, r6, r10
	mov -0x7, r2
	mov 0xff55, r4
	mov 0xffffffff, r6
	mul r6, r2, r11
	mul r10, r8, r8
	mul r8, r9, r11
	mul 0x0, r3, r4
	mul -0x5, r8, r9
	mul 0xf, r9, r9

;----------------------------------mulh----

	mov 0x1111abcd, r1
	mov 0x2222ff55, r2
	mov 0x12345678, r3
	mov 0xffffffff, r4
	mulh r1, r2
	mulh r2, r5
	mulh r5, r1
	mulh 0xd, r4
	mulh r8, r4
	mulh -0x1, r9

;---------------------------------mulhi----

	mulhi 0x7fff, r1, r2
	mulhi -0xff, r4, r5
	mulhi 0x0, r10, r7
	mulhi 0x1, r10, r10

;---------------------------------mulu-----

	mov 0xffffffff, r4
	mov 0x12345678, r5
	mulu r1, r5, r6
	mulu r3, r4, r5
	mulu 0xff, r4, r7
	mulu 0xffff, r7, r9

;------------------------------------------


   end
