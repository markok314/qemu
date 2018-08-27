   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:


	mov 0x5f, r1
	mov -0x7, r2
	mov 0x7ffffff0, r3
	mov 0x80000000, r4
	mov 0x12345678, r5
	mov 0xffffffff, r6

;----------------------------------satadd---

	satadd r1, r5, r15
	satadd r2, r5, r15
	satadd r3, r2, r8
	satadd r3, r4, r9
	satadd r3, r1, r7
	satadd r1, r2, r7
	satadd r1, r5, r7

	satadd r4, r2, r10
	ldsr r0, psw	;clearing PSW register
	satadd r4, r5, r11
	satadd r3, r6, r12
	satadd r1, r8
	satadd -0x5, r10
	satadd r11, r15
	ldsr r0, psw
	satadd r11, r15
	satadd r9, r10
	satadd 0xf, r12
	satadd 0xf, r12
	ldsr r0, psw
;---------------------------------satsub---

	mov 0x5f, r1
	mov -0x7, r2
	mov 0x7ffffff0, r3
	mov 0x80000000, r4
	mov 0x12345678, r5
	mov 0xffffffff, r6
	mov 0x00444444, r7
	mov 0xff233566, r8

	satsub r2, r1, r9
	satsub r3, r1, r10
	satsub r7, r10
	ldsr r0, psw
	satsub r1, r2, r11
	satsub r4, r2, r11
	satsub r3, r6
	ldsr r0, psw
	mov 0x7fffffff, r3
	satsub r4, r3, r16

	ldsr r0, psw
;------------------------------------------

	mov 0x5f, r1
	mov -0x7, r2
	mov 0x7ffffff0, r3
	mov 0x80000000, r4
	mov 0x12345678, r5
	mov 0xffffffff, r6
	mov 0x00444444, r7
	mov 0xff233566, r8


	

	
	ldsr r0, psw

;------------------------------------------
;------------------------------------------



   end
