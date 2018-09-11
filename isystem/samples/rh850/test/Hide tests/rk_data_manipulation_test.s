.include "RH850G3M_insts.s"
.text
.include "gpr_init.s"


	mov 0xffff1234, r1
	mov 0x12005678, r2
	mov 0xff11ff00, r3
	mov 0xadac0000, r4
	mov 0x1f2e3d8c, r5
	mov 0x13450112, r6
	mov 0x0, r7
	

#----------------------Testing the BINS instruction-----

# Gcc does not recognize this instruction
# This should be tested later


#----------------------Testing the BSH instruction-----

	bsh r1, r8	# S flag is set
	bsh r3, r9	# byte of zeros found; CY flag is set
	bsh r2, r10	
	bsh r4, r11	# CY, S and Z flags are set
	bsh r5, r12
	bsh r6, r13	# OK if byte of zeros are found?

#----------------------Testing the BSW instruction-----

	bsw r1, r8
	bsw r3, r9	# byte of zeros found; CY flag is set
	bsw r2, r10	# byte of zeros found; CY flag is set
	bsw r7, r11	# CY and Z flags are set
	bsw r5, r12	# S flag is set
	bsw r6, r13	# OK if byte of zeros are found?

#----------------------Testing the CMOV instruction-----

#----------------------Testing the HSH instruction-----

	mov 0xffffffff, r1
	mov 0x12345678, r2
	mov 0x12340000, r3
	mov 0x0, r4

	hsh r1, r5	# S flag is set
	hsh r2, r6
	hsh r3, r7	# CY flag is set
	hsh r4, r8	# CY and Z flags are set

#----------------------Testing the HSW instruction-----

	mov 0xfff12fff, r1
	mov 0x12345678, r2
	mov 0x12341000, r3
	mov 0x0, r4
	mov 0x0ffff000, r5

	hsw r1, r9	
	hsw r2, r6
	hsw r3, r7	
	hsw r4, r8	# CY and Z flags are set
	hsw r5, r10	# CY and S flags are set
	hsw r9, r1	# S flag is set

#----------------------Testing the ROTL instruction-----

	mov 0x80000000, r1
	mov 0x7fffffff, r2
	mov 0x00234567, r3
	mov 0x02345678, r4
	mov 0x1, r5
	mov 0xa, r6

	rotl r5, r1, r7		# 0th bit is 1; CY flag is set
	rotl r5, r2, r8		# S flag is set
	rotl r0, r1, r9		# S flag is set
	rotl r0, r2, r10	# 0th bit is 1; CY flag is set
	rotl r6, r3, r11	# S flag is set
	rotl r6, r10, r12	# CY and S flags are set
	rotl r6, r0, r13	# Z flag is set
	rotl r6, r7, r14	# no flag is set

	# also test the 'rotl imm5, r2, r3' instruction, once it is implemented in RH850G3M_insts.s

#----------------------Testing the SAR instruction-----

	sar r5, r1, r7		# CY and S flag are set
	sar r5, r2, r8
	sar r0, r1, r9		# S flag is set
	sar r0, r2, r10		
	sar 0x6, r4			
	sar 0xa, r1		# CY and S flag are set
	sar 0x4, r6		# Z flag is set
	sar r6, r3

#----------------------Testing the SASF instruction-----

			# Testing with no flag set
	sasf V, r1	
	sasf S, r2
	sasf NV, r3
	sasf NS, r4
	sasf C, r5
	sasf T, r6
	sasf NC, r1
	sasf SA, r2
	sasf Z, r3
	sasf LT, r4
	sasf NZ, r5
	sasf GE, r6
	sasf NH, r1
	sasf LE, r2
	sasf H, r3
	sasf GT, r4

	cmp r1, r2	# Testing with CY, OV and S flags set

	sasf V, r1
	sasf S, r2
	sasf NV, r3
	sasf NS, r4
	sasf C, r5
	sasf T, r6
	sasf NC, r1
	sasf SA, r2
	sasf Z, r3
	sasf LT, r4
	sasf NZ, r5
	sasf GE, r6
	sasf NH, r1
	sasf LE, r2
	sasf H, r3
	sasf GT, r4

	cmp r2, r1	# Testing with CY and S flags set

	sasf V, r1
	sasf S, r2
	sasf NV, r3
	sasf NS, r4
	sasf C, r5
	sasf T, r6
	sasf NC, r1
	sasf SA, r2
	sasf Z, r3
	sasf LT, r4
	sasf NZ, r5
	sasf GE, r6
	sasf NH, r1
	sasf LE, r2
	sasf H, r3
	sasf GT, r4

	cmp r3, r3	# Testing with Z flag set

	sasf V, r1
	sasf S, r2
	sasf NV, r3
	sasf NS, r4
	sasf C, r5
	sasf T, r6
	sasf NC, r1
	sasf SA, r2
	sasf Z, r3
	sasf LT, r4
	sasf NZ, r5
	sasf GE, r6
	sasf NH, r1
	sasf LE, r2
	sasf H, r3
	sasf GT, r4
	
	mov 0x7fffffff, r10
	satadd 0x1, r10		# Testing with SA flag set

	sasf V, r1
	sasf S, r2
	sasf NV, r3
	sasf NS, r4
	sasf C, r5
	sasf T, r6
	sasf NC, r1
	sasf SA, r2
	sasf Z, r3
	sasf LT, r4
	sasf NZ, r5
	sasf GE, r6
	sasf NH, r1
	sasf LE, r2
	sasf H, r3
	sasf GT, r4

	mov 0x80000000, r10
	sub r6, r10		# Testing with OV flag set

	sasf V, r1
	sasf S, r2
	sasf NV, r3
	sasf NS, r4
	sasf C, r5
	sasf T, r6
	sasf NC, r1
	sasf SA, r2
	sasf Z, r3
	sasf LT, r4
	sasf NZ, r5
	sasf GE, r6
	sasf NH, r1
	sasf LE, r2
	sasf H, r3
	sasf GT, r4

#----------------------Testing the SETF instruction-----


	cmp r2, r1	# Testing with CY and S flags set

	setf V, r1
	setf S, r2
	setf NV, r3
	setf NS, r4
	setf C, r5
	setf T, r6
	setf NC, r1
	setf SA, r2
	setf Z, r3
	setf LT, r4
	setf NZ, r5
	setf GE, r6
	setf NH, r1
	setf LE, r2
	setf H, r3
	setf GT, r4

			# Testing with no flag set
	setf V, r1
	setf S, r2
	setf NV, r3
	setf NS, r4
	setf C, r5
	setf T, r6
	setf NC, r1
	setf SA, r2
	setf Z, r3
	setf LT, r4
	setf NZ, r5
	setf GE, r6
	setf NH, r1
	setf LE, r2
	setf H, r3
	setf GT, r4

	cmp r3, r3	# Testing with Z flag set

	setf V, r1
	setf S, r2
	setf NV, r3
	setf NS, r4
	setf C, r5
	setf T, r6
	setf NC, r1
	setf SA, r2
	setf Z, r3
	setf LT, r4
	setf NZ, r5
	setf GE, r6
	setf NH, r1
	setf LE, r2
	setf H, r3
	setf GT, r4
	
	mov 0x7fffffff, r10
	satadd 0x1, r10		# Testing with SA flag set

	setf V, r1
	setf S, r2
	setf NV, r3
	setf NS, r4
	setf C, r5
	setf T, r6
	setf NC, r1
	setf SA, r2
	setf Z, r3
	setf LT, r4
	setf NZ, r5
	setf GE, r6
	setf NH, r1
	setf LE, r2
	setf H, r3
	setf GT, r4

	mov 0x80000000, r10
	sub r6, r10		# Testing with OV flag set

	setf V, r1
	setf S, r2
	setf NV, r3
	setf NS, r4
	setf C, r5
	setf T, r6
	setf NC, r1
	setf SA, r2
	setf Z, r3
	setf LT, r4
	setf NZ, r5
	setf GE, r6
	setf NH, r1
	setf LE, r2
	setf H, r3
	setf GT, r4

	cmp r1, r2	# Testing with CY, OV and S flags set

	setf V, r1
	setf S, r2
	setf NV, r3
	setf NS, r4
	setf C, r5
	setf T, r6
	setf NC, r1
	setf SA, r2
	setf Z, r3
	setf LT, r4
	setf NZ, r5
	setf GE, r6
	setf NH, r1
	setf LE, r2
	setf H, r3
	setf GT, r4

#----------------------Testing the SHL instruction-----

	mov 0x80000000, r1
	mov 0x7fffffff, r2
	mov 0x0acdef00, r3
	mov 0x6, r4
	mov 0x1, r5


	shl r5, r1, r6		# CY and Z flags are set
	shl r5, r2, r7		# S flag is set
	shl r0, r1, r8		# S flag is set
	shl r0, r2, r8
	shl r4, r0, r9		# Z flag is set
	shl r4, r3		# S flag is set
	shl r3, r3		# CY flag is set


#----------------------Testing the SHR instruction-----

	mov 0x1, r1
	mov 0x2, r2
	
	shr 0x0, r1
	shr 0x1, r1
	shr 0x0, r2
	shr 0x2, r2


	ldsr r0, psw
Lbl: br Lbl

