.text
.include "gpr_init.s"

# explanation of comments
# 0 - the conditions are not satisfied
# 1 - the conditions are satisfied



#-----------------------------Testing the ADF instruction-----

	mov 0x1, r1
	mov 0x7, r2
	mov 0x1234, r3
	mov 0x7fffffff, r4
	mov -0x1, r5
	mov 0x80000000, r6
	mov 0x8000000f, r7

#-------# Testing ADF with no flag set

	cmp r1, r2		
				# Adf result is either 0x8 or 0x9
				# so it does not set any flag

	adf V, r1, r2, r8	# 0
	adf NV, r1, r2, r9	# 1
	adf C, r1, r2, r10	# 0
	adf NC, r1, r2, r11	# 1 
	adf Z, r1, r2, r12	# 0 
	adf NZ, r1, r2, r13	# 1
	adf NH, r1, r2, r14	# 0 
	adf H, r1, r2, r15	# 1
	adf S, r1, r2, r16	# 0
	adf NS, r1, r2, r17	# 1
	adf T, r1, r2, r18	# 1
	adf LT, r1, r2, r19	# 0
	adf GE, r1, r2, r20	# 1
	adf LE, r1, r2, r21	# 0
	adf GT, r1, r2, r22	# 1

#-------# Testing ADF with CY and S flags set

	cmp r2, r1		
				# Adf result is either 0x123b or 0x123c
				# so it does not set any flag 

				# Because flags are being cleared in each step,
				# additional compare instructions are added.

	adf v, r3, r2, r8	# 0
	cmp r2, r1
	adf nv, r3, r2, r9	# 1
	cmp r2, r1
	adf c, r3, r2, r10	# 1
	cmp r2, r1
	adf nc, r3, r2, r11	# 0
	cmp r2, r1
	adf z, r3, r2, r12	# 0
	cmp r2, r1
	adf nz, r3, r2, r13	# 1	
	cmp r2, r1
	adf nh, r3, r2, r14	# 1
	cmp r2, r1
	adf h, r3, r2, r15	# 0
	cmp r2, r1
	adf s, r3, r2, r16	# 1
	cmp r2, r1
	adf ns, r3, r2, r17	# 0
	cmp r2, r1
	adf t, r3, r2, r18	# 1
	cmp r2, r1
	adf lt, r3, r2, r19	# 1
	cmp r2, r1
	adf ge, r3, r2, r20	# 0
	cmp r2, r1
	adf le, r3, r2, r21	# 1
	cmp r2, r1
	adf gt, r3, r2, r22	# 0

	ldsr r0, psw		# Clearing the PSW register

#-------# Testing ADF with OV and S flags set
	
	add r4, r3		
				# Adf result is either 0x80000007 or 0x80000008
				# so OV and S flags stay set with each instruction.

	adf V, r4, r2, r8	# 1
	adf NV, r4, r2, r9	# 0
	adf C, r4, r2, r10	# 0
	adf NC, r4, r2, r11	# 1
	adf Z, r4, r2, r12	# 0
	adf NZ, r4, r2, r13	# 1
	adf NH, r4, r2, r14	# 0
	adf H, r4, r2, r15	# 1
	adf S, r4, r2, r16	# 1
	adf NS, r4, r2, r17	# 0
	adf T, r4, r2, r18	# 1
	adf LT, r4, r2, r19	# 0
	adf GE, r4, r2, r20	# 1
	adf LE, r4, r2, r21	# 0
	adf GT, r4, r2, r22	# 1

	ldsr r0, psw		# Clearing the PSW register

#-------# Testing ADF with Z flag set
	
	addi 0x0, r0, r1	
				# Adf result is either 0x0 or 0x1
				# so Z flag is being set/reset, 
				# depending on condition

	adf V, r1, r0, r8	# 0
	adf NV, r1, r0, r9	# 1
	adf C, r1, r0, r10	# 0
	adf NC, r1, r0, r11	# 1
	adf Z, r1, r0, r12	# 0
	adf NZ, r1, r0, r13	# 0
	adf NH, r1, r0, r14	# 1
	adf H, r1, r0, r15	# 1
	adf S, r1, r0, r16	# 0
	adf NS, r1, r0, r17	# 1
	adf T, r1, r0, r18	# 1
	adf LT, r1, r0, r19	# 0
	adf GE, r1, r0, r20	# 1
	adf LE, r1, r0, r21	# 0
	adf GT, r1, r0, r22	# 0
				
				# Additional block is added for unreached scenarios

	adf Z, r1, r0, r12	# 1
	adf NZ, r1, r0, r13	# 1
	adf GT, r1, r0, r22	# 1
	adf NH, r1, r0, r21	# 0
	adf H, r1, r0, r15	# 1
	adf LT, r1, r0, r19	# 0

	ldsr r0, psw		# Clearing the PSW register

#-------# Testing ADF with S flag set
	
	addi 0x0, r3, r1	
				# Adf result is either 0x80001233 or 0x80001234
				# so S flag stays set with each instruction.

	adf V, r1, r0, r8	# 0
	adf NV, r1, r0, r9	# 1
	adf C, r1, r0, r10	# 0
	adf NC, r1, r0, r11	# 1
	adf Z, r1, r0, r12	# 0
	adf NZ, r1, r0, r13	# 1
	adf NH, r1, r0, r14	# 0
	adf H, r1, r0, r15	# 1
	adf S, r1, r0, r16	# 1
	adf NS, r1, r0, r17	# 0
	adf T, r1, r0, r18	# 1
	adf LT, r1, r0, r19	# 1
	adf GE, r1, r0, r20	# 0
	adf LE, r1, r0, r21	# 1
	adf GT, r1, r0, r22	# 0

	ldsr r0, psw		# Clearing the PSW register

#-------# Testing ADF with OV flag set
	
	sub r2, r6
	mov 0x80000000, r1
	mov -0x2, r3
				# Adf result is either 0x7ffffffe or 0x7fffffff
				# so OV and CY flags stay set with each instruction.

	adf V, r1, r3, r8	# 1
	adf NV, r1, r3, r9	# 0	
	adf C, r1, r3, r10	# 1
	adf NC, r1, r3, r11	# 0
	adf Z, r1, r3, r12	# 0
	adf NZ, r1, r3, r13	# 1
	adf NH, r1, r3, r14	# 1
	adf H, r1, r3, r15	# 0
	adf S, r1, r3, r16	# 0
	adf NS, r1, r3, r17	# 1
	adf T, r1, r3, r18	# 1
	adf LT, r1, r3, r19	# 1
	adf GE, r1, r3, r20	# 0
	adf LE, r1, r3, r21	# 1
	adf GT, r1, r3, r22	# 0

	ldsr r0, psw		# Clearing the PSW register




#----------------------------------Testing the SBF instruction------------

	mov 0x1, r1
	mov 0x7, r2
	mov 0x1234, r3
	mov 0x7fffffff, r4
	mov -0x1, r5
	mov 0x80000000, r6
	mov 0x8000000f, r7

#-------# Testing SBF with no flag set

	cmp r1, r2		
				# Sbf result is either 0x5 or 0x6 
				# so it does not set any flag

	sbf V, r1, r2, r8	# 0
	sbf NV, r1, r2, r9	# 1
	sbf C, r1, r2, r10	# 0
	sbf NC, r1, r2, r11	# 1
	sbf Z, r1, r2, r12	# 0
	sbf NZ, r1, r2, r13	# 1
	sbf NH, r1, r2, r14	# 0
	sbf H, r1, r2, r15	# 1
	sbf S, r1, r2, r16	# 0
	sbf NS, r1, r2, r17	# 1
	sbf T, r1, r2, r18	# 1
	sbf LT, r1, r2, r19	# 0
	sbf GE, r1, r2, r20	# 1
	sbf LE, r1, r2, r21	# 0
	sbf GT, r1, r2, r22	# 1

#-------# Testing SBF with CY and S flags set

	cmp r2, r1		
				# Sbf result is either 0xffffedd2 or 0xffffedd3 
				# so CY and S flags are set with each instruction

	sbf V, r3, r2, r8	# 1
	sbf NV, r3, r2, r9	# 0
	sbf C, r3, r2, r10	# 0
	sbf NC, r3, r2, r11	# 1
	sbf Z, r3, r2, r12	# 1
	sbf NZ, r3, r2, r13	# 0
	sbf NH, r3, r2, r14	# 0
	sbf H, r3, r2, r15	# 1
	sbf S, r3, r2, r16	# 0
	sbf NS, r3, r2, r17	# 1
	sbf T, r3, r2, r18	# 0
	sbf LT, r3, r2, r19	# 0
	sbf GE, r3, r2, r20	# 1
	sbf LE, r3, r2, r21	# 0
	sbf GT, r3, r2, r22	# 1

	ldsr r0, psw		# Clearing the PSW register

#-------# Testing SBF with OV and S flags set 
	
	add r4, r3

	mov r4, r2	
	mov -0x2, r4 	
				# Sbf result is either 0x80000000 or 0x80000001
				# so CY, OV and S flags stay set with each instruction.

	sbf V, r4, r2, r8	# 1
	sbf NV, r4, r2, r9	# 0
	sbf C, r4, r2, r10	# 1
	sbf NC, r4, r2, r11	# 0
	sbf Z, r4, r2, r12	# 0
	sbf NZ, r4, r2, r13	# 1
	sbf NH, r4, r2, r14	# 1
	sbf H, r4, r2, r15	# 0
	sbf S, r4, r2, r16	# 1
	sbf NS, r4, r2, r17	# 0
	sbf T, r4, r2, r18	# 1
	sbf LT, r4, r2, r19	# 0
	sbf GE, r4, r2, r20	# 1
	sbf LE, r4, r2, r21	# 0
	sbf GT, r4, r2, r22	# 1

	ldsr r0, psw		# Clearing the PSW register

#-------# Testing SBF with Z and CY flags set

	
	addi 0x0, r0, r1	

				# Sbf result is either 0x0 or 0xffffffff
				# so Z flag stays set, CY is being set if 
				# condition is met.

	sbf V, r1, r0, r8	# 0
	sbf NV, r1, r0, r9	# 1
	sbf C, r1, r0, r10	# 1
	sbf NC, r1, r0, r11	# 0
	sbf Z, r1, r0, r12	# 1
	sbf NZ, r1, r0, r13	# 1
	sbf NH, r1, r0, r14	# 1
	sbf H, r1, r0, r15	# 0
	sbf S, r1, r0, r16	# 0
	sbf NS, r1, r0, r17	# 1
	sbf T, r1, r0, r18	# 1
	sbf LT, r1, r0, r19	# 1
	sbf GE, r1, r0, r20	# 0
	sbf LE, r1, r0, r21	# 1
	sbf GT, r1, r0, r22	# 0

	sbf C, r1, r0, r10	# 0
	sbf NZ, r1, r0, r13	# 0
	sbf LT, r1, r0, r19	# 0
	sbf GE, r1, r0, r20	# 1

	ldsr r0, psw		# Clearing the PSW register

#--------# Testing SBF with S flag set

	ldsr r0, psw
	addi 0x0, r3, r1	
				# Sbf result is either 0x80001232 or 0x80001233
				# so S flag stays set with each instruction.

	sbf V, r0, r1, r8	# 0
	sbf NV, r0, r1, r9	# 1
	sbf C, r0, r1, r10	# 0
	sbf NC, r0, r1, r11	# 1
	sbf Z, r0, r1, r12	# 0
	sbf NZ, r0, r1, r13	# 1	
	sbf NH, r0, r1, r14	# 0
	sbf H, r0, r1, r15	# 1
	sbf S, r0, r1, r16	# 1
	sbf NS, r0, r1, r17	# 0
	sbf T, r0, r1, r18	# 1
	sbf LT, r0, r1, r19	# 1
	sbf GE, r0, r1, r20	# 0
	sbf LE, r0, r1, r21	# 1
	sbf GT, r0, r1, r22	# 0

	ldsr r0, psw		# Clearing the PSW register

#-------# Testing SBF with OV flag set

	sub r2, r6		
	mov 0x80000000, r3	
	mov 0xff, r5
				# Sbf result is either 0x7ffffff00 or 0x7fffff01
				# so OV flag stays set with each instruction.

	sbf V, r5, r3, r8	# 1
	sbf NV, r5, r3, r9	# 0	
	sbf C, r5, r3, r10	# 0
	sbf NC, r5, r3, r11	# 1
	sbf Z, r5, r3, r12	# 0
	sbf NZ, r5, r3, r13	# 1
	sbf NH, r5, r3, r14	# 0
	sbf H, r5, r3, r15	# 1
	sbf S, r5, r3, r16	# 0
	sbf NS, r5, r3, r17	# 1
	sbf T, r5, r3, r18	# 1
	sbf LT, r5, r3, r19	# 1
	sbf GE, r5, r3, r20	# 0
	sbf LE, r5, r3, r21	# 1
	sbf GT, r5, r3, r22	# 0

	ldsr r0, psw

# problems occur if we are adding/subtracting to MAX/MIN value
# when this happens, unappropriate flags are set!!

#-------# Testing edge cases for flag settings

	mov 0x7ffffff0, r1
	mov 0xf, r2
	cmp 0xf, r2
	adf Z r1, r2, r3
	cmp 0xf, r2
	adf NZ r1, r2, r3






	
	
