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


;------------------------------------------

	satadd r3, r1, r7
	satadd r3, r2, r8
	satadd r3, r4, r9
	satadd r4, r2, r10
	satadd r4, r5, r11
	satadd r3, r6, r12

;------------------------------------------
;------------------------------------------
;------------------------------------------
;------------------------------------------



   end
