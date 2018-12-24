
.include "RH850G3M_insts.s"
.include "gpr_init.s"

#----------------------------------mov-----

	mov 0x2, r1
	mov 0x80000000, r2	# this value is later referred to as "MIN"
	mov 0x7fffffff, r3	# this value is later referred to as "MAX"
	mov 0x0, r5
	mov 0x44, r6
	mov -0x20, r7
	mov -0xffff, r8

	mov r2, r4
	mov r3, r9

#----------------------------------add-----

	# Testing addition with positive and negative values
	
	add r0, r5 		# When r5=0, Z flag is set
	add r3, r1		# MAX+pos=neg(opverflow), OV and S flags are set

	add r2, r3		# MIN+MAX=-1, S flag is set
	add r7, r6		# neg+pos=pos, CY flag is set
	add r7, r8		# neg+neg=neg, CY and S flags are set
	
	mov 0x2, r5
	add -0x5, r5		# neg+pos=neg, CY and S flags are set
	
	add r2, r5		# MIN+neg=pos(overflow), CY and OV flags are set

	mov 0x2345, r1
	mov 0x123, r2
	add r1, r2		# pos+pos=pos, no flag is set
	
	
	movea 0x1111, r6, r8

#----------------------------------addi----

	# Testing addition with positive and negative immediate values

	mov 0x2, r1
	mov 0x80000000, r2
	mov 0x7fff0000, r3
	mov 0x0, r9
	movea 0x7fff, r3, r5

	addi -0xff, r5, r5	# neg+pos=pos, CY flag is set
	addi 0x7fff, r5, r6	# pos+pos=pos, no flag is set
	addi 0x7fff, r6, r6	# pos+pos=neg(overflow), OV and S flags are set
	addi 0x0, r9, r6	# when r9=0, Z flag is set
	addi -0x6, r2, r7	# neg+MIN=pos(overflow), CY and OV flags are set 
	addi -0xa, r1, r7	# neg+pos=neg, S flag is set
	addi -0xc, r7, r8	# neg+neg=neg, CY and S flags are set
	
	mov 0xfffffff0, r4
	addi 0x10, r4, r7	# pos+neg=0, CY and Z flags are set


#----------------------------------cmp-----

	mov 0x7fffffff, r3
	mov -0xa, r4	

	ldsr r0, psw

	cmp r0, r13		# when r13=0, values equal, CY and Z flags are set
	cmp r1, r0		# first operand larger, CY and S flags are set
	cmp 0x2, r0		# first operand larger, CY and S flags are set
	cmp 0x0, r14		# when r14=0, values equal, CY and Z flags are set
	cmp r3, r4		# first operand larger, OV flag is set
	cmp r4, r3		# second operand larger, CY, OV and S flags are set
	cmp -0xf, r4		# second operand larger, no flag is set
	mov -0xf, r4
	cmp -0xa, r4		# first operand larger, CY and S flags are set

#---------------------------movea--movhi----

	# Additional testing of move instructions

	mov 0x0000ffff, r1
	mov 0x11110000, r2
	movhi 0xffff, r1, r3
	movhi 0x1111, r2, r4
	movea 0x3333, r4, r4
	mov 0xffffffff, r5
	mov 0x22223333, r6
	mov 0x80000000, r7

#-----------------------------------sub----

	# Testing subtraction with positive and negative values

	sub r1, r5		# neg-pos=neg, S flag is set
	sub r2, r6		# pos-pos=pos, no flag is set
	sub r4, r1		# pos-pos=neg, CY and S flags are set
	sub r8, r8		# neg-neg=0, CY and Z flags are set

	mov 0x1, r8

	sub r5, r4		# pos-neg=pos, CY flag is set
	sub r7, r5		# neg-neg=pos, no flag is set
	sub r7, r8		# pos-neg=neg, CY, OV nad S flags are set
	sub r3, r7		# neg-neg=neg, CY and S flags are set
	sub r2, r7		# neg-pos=pos, OV flag is set

	mov -0xc, r6
	movhi 0x1, r3, r3
	movhi 0x7fff, r3, r3
	sub r3, r6		#OV

#----------------------------------subi----

	# Testing reverse subtraction with positive and negative values

	mov 0x2, r1
	mov 0x80000000, r2	
	mov 0x7fffffff, r3
	mov 0x1234, r4	
	mov -0xa, r5
	mov 0x44, r6
	mov -0x20, r7
	mov -0xffff, r8
	mov r8, r9

	subr r6, r1		# pos-pos=pos, no flag is set
	subr r1, r6		# pos-pos=neg, CY and S flags are set
	subr r4, r7		# pos-neg=pos, CY flag is set
	subr r3, r8		# pos-neg=neg, CY, OV and S flags are set
	subr r8, r6		# neg-neg=neg, CY and S flags are set
	subr r5, r8		# neg-neg=pos, no flag is set
	subr r9, r4		# neg-pos=neg, S flag is set
	subr r6, r8		# neg-pos=pos, OV flag is set

#------------------------------------------

Lbl: br Lbl

