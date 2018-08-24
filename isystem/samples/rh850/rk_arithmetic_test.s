   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:
;----------------------------------add-----

	mov 0x2, r1
	mov 0x80000000, r2
	mov 0x7fffffff, r3
	
	mov r2, r4
	mov r3, r6
	
	add r0, r5 	;Z flag
	add r1, r3	;S flag, OV flag
	add r2, r3	;CY flag, S flag
	
	add -0x5, r6
	
	movea 0x0, r1, r7
	movea 0x1111, r6, r8

;----------------------------------addi----

	mov 0x2, r1
	mov 0x80000000, r2
	mov 0x7fff0000, r3

	movea 0x7fff, r3, r5
	addi -0xff, r5, r5
	addi 0x7fff, r5, r6	
	addi 0x7fff, r6, r6	;OV, S
	addi 0x0, r9, r6	;Z
	
	mov 0xfffffff0, r4
	addi 0x10, r4, r7	;CY, Z?


;----------------------------------cmp-----

	mov 0x7fffffff, r3
	mov -0xa, r4	

	cmp r0, r13		;CY?
	cmp r1, r0		;CY, S
	cmp 0x2, r0		;CY, S
	cmp 0x0, r14		;CY, Z
	cmp r3, r4		;OV
	cmp r4, r3		;CY, OV, S
	cmp -0xf, r4
	mov -0xf, r4
	cmp -0xa, r4		;CY, S

;---------------------------movea--movhi----

	mov 0x0000ffff, r1
	mov 0x11110000, r2
	movhi 0xffff, r1, r3
	movhi 0x1111, r2, r4
	movea 0x3333, r4, r4
	mov 0xffffffff, r5
	mov 0x22223333, r6
	cmp r3, r5
	cmp r4, r6

;-----------------------------------sub----

	sub r1, r5
	sub r2, r6
	sub r4, r1
	sub r5, r5
	sub r5, r4

	mov -0xc, r6
	movhi 0x1, r3, r3
	movhi 0x7fff, r3, r3
	sub r3, r6		;OV

;----------------------------------subi----

	subr r5, r4
	subr r3, r4
	subr r2, r3
	subr r7, r8
	mov 0x6, r1
	mov 0x1245, r2
	subr r1, r2

;------------------------------------------


   end
