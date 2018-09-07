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

#----------------------Testing the SAR instruction-----

#----------------------Testing the SASF instruction-----

#----------------------Testing the SETF instruction-----

#----------------------Testing the SHL instruction-----

#----------------------Testing the SHR instruction-----



Lbl: br Lbl

