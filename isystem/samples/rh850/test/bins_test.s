.include "RH850G3M_insts.s"

.include "gpr_init.s"

.text

	mov 0xffffffff, r1
	mov 0x11111111, r2
	mov 0x00000000, r3
	
	BINS R3, 0, 4, R2	# msb<16   lsb<16 
	BINS R4, 4, 4, R2	# msb<16   lsb<16 
	BINS R5, 8, 4, R2	# msb<16   lsb<16 
	BINS R6, 8, 8, R2	# msb<16   lsb<16 
                                          
	BINS R7, 8, 9, R1	# msb=16  lsb<16
                                          
	BINS R8, 12, 12, R1	# msb>16  lsb<16
                                          
	BINS R9, 15, 14, R1	# msb>=16  lsb<16

Lbl:	br Lbl
