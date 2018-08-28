   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:
;----------------------------------adf-----

	mov 0x1, r1
	mov 0x7, r2
	mov 0x1234, r3
	mov 0x7fffffff, r4
	mov -0x1, r5
	mov 0x80000000, r6
	mov 0x8000000f, r7
;------------------------------------------
	cmp r1, r2		; no flag is set

	adf V, r1, r2, r8
	adf NV, r1, r2, r9
	adf C, r1, r2, r10
	adf NC, r1, r2, r11
	adf Z, r1, r2, r12
	adf NZ, r1, r2, r13	
	adf NH, r1, r2, r14
	adf H, r1, r2, r15
	adf S, r1, r2, r16
	adf NS, r1, r2, r17
	adf T, r1, r2, r18
	adf LT, r1, r2, r19
	adf GE, r1, r2, r20
	adf LE, r1, r2, r21
	adf GT, r1, r2, r22
;------------------------------------------
	cmp r2, r1		; CY and S flags are set

	adf V, r3, r2, r8
	cmp r2, r1
	adf NV, r3, r2, r9
	cmp r2, r1
	adf C, r3, r2, r10
	cmp r2, r1
	adf NC, r3, r2, r11
	cmp r2, r1
	adf Z, r3, r2, r12
	cmp r2, r1
	adf NZ, r3, r2, r13	
	cmp r2, r1
	adf NH, r3, r2, r14
	cmp r2, r1
	adf H, r3, r2, r15
	cmp r2, r1
	adf S, r3, r2, r16
	cmp r2, r1
	adf NS, r3, r2, r17
	cmp r2, r1
	adf T, r3, r2, r18
	cmp r2, r1
	adf LT, r3, r2, r19
	cmp r2, r1
	adf GE, r3, r2, r20
	cmp r2, r1
	adf LE, r3, r2, r21
	cmp r2, r1
	adf GT, r3, r2, r22
;------------------------------------------
	ldsr r0, psw
	add r4, r3	; OV and S flags are set and stay
			; set until the ldsr instruction

	adf V, r4, r2, r8
	adf NV, r4, r2, r9
	adf C, r4, r2, r10
	adf NC, r4, r2, r11
	adf Z, r4, r2, r12
	adf NZ, r4, r2, r13	
	adf NH, r4, r2, r14
	adf H, r4, r2, r15
	adf S, r4, r2, r16
	adf NS, r4, r2, r17
	adf T, r4, r2, r18
	adf LT, r4, r2, r19
	adf GE, r4, r2, r20
	adf LE, r4, r2, r21
	adf GT, r4, r2, r22
;------------------------------------------
	ldsr r0, psw
	addi 0x0, r0, r1	; Z flag is set 

	adf V, r1, r0, r8	; Z flag is being set/reset 
				; throughout this part, 
				; depending on condition
	adf NV, r1, r0, r9
	adf C, r1, r0, r10
	adf NC, r1, r0, r11
	adf Z, r1, r0, r12
	adf NZ, r1, r0, r13	
	adf NH, r1, r0, r14
	adf H, r1, r0, r15
	adf S, r1, r0, r16
	adf NS, r1, r0, r17
	adf T, r1, r0, r18
	adf LT, r1, r0, r19
	adf GE, r1, r0, r20
	adf LE, r1, r0, r21
	adf GT, r1, r0, r22
;------------------------------------------
	ldsr r0, psw
	addi 0x0, r3, r1	; S flag is set and stays set
				; until the ldsr instruction
	adf V, r1, r0, r8
	adf NV, r1, r0, r9
	adf C, r1, r0, r10
	adf NC, r1, r0, r11
	adf Z, r1, r0, r12
	adf NZ, r1, r0, r13	
	adf NH, r1, r0, r14
	adf H, r1, r0, r15
	adf S, r1, r0, r16
	adf NS, r1, r0, r17
	adf T, r1, r0, r18
	adf LT, r1, r0, r19
	adf GE, r1, r0, r20
	adf LE, r1, r0, r21
	adf GT, r1, r0, r22

;------------------------------------------
	ldsr r0, psw
	sub r2, r6		; OV flag is set
	mov 0x80000000, r1		
	mov -0x2, r3

	adf V, r1, r3, r8
	adf NV, r2, r3, r9
	adf C, r5, r2, r10
	adf NC, r5, r2, r11
	adf Z, r5, r2, r12
	adf NZ, r5, r2, r13	
	adf NH, r5, r2, r14
	adf H, r5, r2, r15
	adf S, r5, r2, r16
	adf NS, r5, r2, r17
	adf T, r5, r2, r18
	adf LT, r5, r2, r19
	adf GE, r5, r2, r20
	adf LE, r5, r2, r21
	adf GT, r5, r2, r22
	
	
