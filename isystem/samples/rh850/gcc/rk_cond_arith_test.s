.text
.include "gpr_init.s"

#----------------------------------ADF-----

	mov 0x1, r1
	mov 0x7, r2
	mov 0x1234, r3
	mov 0x7fffffff, r4
	mov -0x1, r5
	mov 0x80000000, r6
	mov 0x8000000f, r7
#-------
	cmp r1, r2		# no flag is set

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
#-------
	cmp r2, r1		# CY and S flags are set

	adf v, r3, r2, r8
	cmp r2, r1
	adf nv, r3, r2, r9
	cmp r2, r1
	adf c, r3, r2, r10
	cmp r2, r1
	adf nc, r3, r2, r11
	cmp r2, r1
	adf z, r3, r2, r12
	cmp r2, r1
	adf nz, r3, r2, r13	
	cmp r2, r1
	adf nh, r3, r2, r14
	cmp r2, r1
	adf h, r3, r2, r15
	cmp r2, r1
	adf s, r3, r2, r16
	cmp r2, r1
	adf ns, r3, r2, r17
	cmp r2, r1
	adf t, r3, r2, r18
	cmp r2, r1
	adf lt, r3, r2, r19
	cmp r2, r1
	adf ge, r3, r2, r20
	cmp r2, r1
	adf le, r3, r2, r21
	cmp r2, r1
	adf gt, r3, r2, r22

#-------
	ldsr r0, psw
	add r4, r3		# OV and S flags are set and stay
				# set until the ldsr instruction

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
#-------
	ldsr r0, psw
	addi 0x0, r0, r1	# Z flag is set 

	adf V, r1, r0, r8	# Z flag is being set/reset 
				# throughout this part, 
				# depending on condition
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
#-------
	ldsr r0, psw
	addi 0x0, r3, r1	# S flag is set and stays set
				# until the ldsr instruction
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
#-------
	ldsr r0, psw
	sub r2, r6		# OV flag is set
	mov 0x80000000, r1
	mov -0x2, r3

	adf V, r1, r3, r8	# OV and CY flags stay set
	adf NV, r1, r3, r9	# until the ldsr instruction
	adf C, r1, r3, r10
	adf NC, r1, r3, r11
	adf Z, r1, r3, r12
	adf NZ, r1, r3, r13	
	adf NH, r1, r3, r14
	adf H, r1, r3, r15
	adf S, r1, r3, r16
	adf NS, r1, r3, r17
	adf T, r1, r3, r18
	adf LT, r1, r3, r19
	adf GE, r1, r3, r20
	adf LE, r1, r3, r21
	adf GT, r1, r3, r22

	ldsr r0, psw

#----------------------------------SBF---------------

	mov 0x1, r1
	mov 0x7, r2
	mov 0x1234, r3
	mov 0x7fffffff, r4
	mov -0x1, r5
	mov 0x80000000, r6
	mov 0x8000000f, r7

#-------
	cmp r1, r2		# no flag is set

	sbf V, r1, r2, r8
	sbf NV, r1, r2, r9
	sbf C, r1, r2, r10
	sbf NC, r1, r2, r11
	sbf Z, r1, r2, r12
	sbf NZ, r1, r2, r13	
	sbf NH, r1, r2, r14
	sbf H, r1, r2, r15
	sbf S, r1, r2, r16
	sbf NS, r1, r2, r17
	sbf T, r1, r2, r18
	sbf LT, r1, r2, r19
	sbf GE, r1, r2, r20
	sbf LE, r1, r2, r21
	sbf GT, r1, r2, r22
#-------
	cmp r2, r1		# CY and S flags are set and stay
				# set until the ldsr instruction

	sbf V, r3, r2, r8
	sbf NV, r3, r2, r9
	sbf C, r3, r2, r10
	sbf NC, r3, r2, r11
	sbf Z, r3, r2, r12
	sbf NZ, r3, r2, r13	
	sbf NH, r3, r2, r14
	sbf H, r3, r2, r15
	sbf S, r3, r2, r16
	sbf NS, r3, r2, r17
	sbf T, r3, r2, r18
	sbf LT, r3, r2, r19
	sbf GE, r3, r2, r20
	sbf LE, r3, r2, r21
	sbf GT, r3, r2, r22
#-------
	ldsr r0, psw
	add r4, r3		# OV and S flags are set 

	sbf V, r4, r2, r8	# CY and S flags stay set
	sbf NV, r4, r2, r9	# until the ldsr instruction
	sbf C, r4, r2, r10
	sbf NC, r4, r2, r11
	sbf Z, r4, r2, r12
	sbf NZ, r4, r2, r13	
	sbf NH, r4, r2, r14
	sbf H, r4, r2, r15
	sbf S, r4, r2, r16
	sbf NS, r4, r2, r17
	sbf T, r4, r2, r18
	sbf LT, r4, r2, r19
	sbf GE, r4, r2, r20
	sbf LE, r4, r2, r21
	sbf GT, r4, r2, r22
#-------
	ldsr r0, psw
	addi 0x0, r0, r1	# Z flag is set 

				# Z flag is being set/reset 
				# throughout this part, 
				# depending on condition,
				# CY flag stays set
	sbf V, r1, r0, r8
	sbf NV, r1, r0, r9
	sbf C, r1, r0, r10
	sbf NC, r1, r0, r11
	sbf Z, r1, r0, r12
	sbf NZ, r1, r0, r13	
	sbf NH, r1, r0, r14
	sbf H, r1, r0, r15
	sbf S, r1, r0, r16
	sbf NS, r1, r0, r17
	sbf T, r1, r0, r18
	sbf LT, r1, r0, r19
	sbf GE, r1, r0, r20
	sbf LE, r1, r0, r21
	sbf GT, r1, r0, r22
#-------
	ldsr r0, psw
	addi 0x0, r3, r1	# S flag is set and stays set
				# until the ldsr instruction
	sbf V, r0, r1, r8
	sbf NV, r0, r1, r9
	sbf C, r0, r1, r10
	sbf NC, r0, r1, r11
	sbf Z, r0, r1, r12
	sbf NZ, r0, r1, r13	
	sbf NH, r0, r1, r14
	sbf H, r0, r1, r15
	sbf S, r0, r1, r16
	sbf NS, r0, r1, r17
	sbf T, r0, r1, r18
	sbf LT, r0, r1, r19
	sbf GE, r0, r1, r20
	sbf LE, r0, r1, r21
	sbf GT, r0, r1, r22
#-------
	ldsr r0, psw
	sub r2, r6		# OV flag is set and stays set
	mov 0x80000000, r3	# until the ldsr instruction
	mov 0xff, r5

	sbf V, r5, r3, r8	
	sbf NV, r5, r3, r9	
	sbf C, r5, r3, r10
	sbf NC, r5, r3, r11
	sbf Z, r5, r3, r12
	sbf NZ, r5, r3, r13	
	sbf NH, r5, r3, r14
	sbf H, r5, r3, r15
	sbf S, r5, r3, r16
	sbf NS, r5, r3, r17
	sbf T, r5, r3, r18
	sbf LT, r5, r3, r19
	sbf GE, r5, r3, r20
	sbf LE, r5, r3, r21
	sbf GT, r5, r3, r22

	ldsr r0, psw

# problems occur if we are adding/subtracting to MAX/MIN value
# when this happens, unappropriate flags are set!!
	
	
