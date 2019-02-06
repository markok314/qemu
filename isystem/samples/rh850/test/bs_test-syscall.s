.include "RH850G3M_insts.s"

.include "gpr_init.s"

		mov 4, R3			# SIZE=4
		LDSR_ID R3, 11, 1	# SCCFG (Syscall cfg. setting)
		mov 0x100, R4		# 48-bit !
		LDSR_ID R4, 12, 1	# SCBP=0x100 (Syscall base pointer)
		SYSCALL 0			# adr=SCBP+0*4=0x100, pc = scbp + mem(0x100) = 0x100 + 0x20 = 0x120
L1:		SYSCALL 1			# adr=SCBP+1*4=0xfee00204, pc = scbp + mem(0xffe00104) = 0xffe00100 + 0x28 = 0xffe00128
L2:		SYSCALL 2			# adr=SCBP+2*4=0xfee00208, pc = scbp + mem(0xffe00108) = 0xffe00100 + 0x30 = 0xffe00130
L3:		br Lbl				# 28
#VT:		.word 16,18,20,0	# 30

		.org 0x100
vec0:	.word 0x20
vec1:	.word 0x24	# vec1=0x104
vec2:	.word 0x38	# vec2=0x108
vecs:	.word 0, 0, 0, 0, 0	#vecs=0x10C (,110, 114, 118, 11C)
call0:	ori 5, R7, R8
		mov 2, R5
		BR Lbl
		eiret
call1:	BR Lbl
call2:	eiret

Lbl:	BR Lbl

# eiic = 0, psw = 0 ... cpu_ID se ne ohrani ...

