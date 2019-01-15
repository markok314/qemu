.include "RH850G3M_insts.s"
.include "gpr_init.s"

#(msb>16 lsb>16) BINS0
#(msb>16 lsb<16) BINS1
#(msb<16 lsb<16) BINS2


	mov 0xffff1234, r1
	mov 0x12005678, r2
	mov 0xff11ff00, r3
	mov 0xadac0000, r4
	mov 0x1f2e3d8c, r5
	mov 0x13450112, r6
	mov 0x0, r7
	

#----------------------Testing the BINS instruction-----

	BINS R1, 0x0, 0x4, R4		#BINS2
	BINS R1, 0x0, 0x10, R4		#BINS1
	BINS R1, 0x2, 0x4, R7		#BINS2
	BINS R1, 0xf, 0x2, R7		#BINS1
	BINS R3, 0x0, 0x14, R8		#BINS1
	BINS R1, 0x5, 0x10, R9		#BINS1
	BINS R6, 22, 8, R10		#BINS0
	BINS R7, 12, 5, R11		#BINS1
	BINS R5, 24, 8, R5		#BINS0
	BINS R0, 12, 12, R4		#BINS1
	BINS R3, 19, 0, R11		#BINS0
	BINS R2, 0, 32, R12		#BINS1

	mov 0x00ff00ff,r1
	mov 0x12345678,r2

	BINS R1, 8, 8, R2		#BINS2
	BINS R10, 4, 28, R10		#BINS1
	BINS R8, 16, 16, R9		#BINS0
	BINS R12, 4, 22, R1		#BINS1
	BINS R6, 22, 4, R8		#BINS0
	BINS R5, 1, 5, R20		#BINS2

	


Lbl: br Lbl

