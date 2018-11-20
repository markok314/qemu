.include "RH850G3M_insts.s"

.text

		mov 4, R3			# SIZE=4
		LDSR_ID R3, 11, 1	# SCCFG
		mov 30, R4			# 48-bit !
		LDSR_ID R4, 12, 1	# SCBP=30
		SYSCALL 0			# adr=SCBP+0*4=30, pc = scbp + mem(30) = 30 + 16 = 46
L1:		SYSCALL 1			# adr=SCBP+1*4=34, pc = scbp + mem(34) = 30 + 18 = 48
L2:		SYSCALL 2			# adr=SCBP+2*4=38, pc = scbp + mem(38) = 30 + 20 = 50
L3:		br L4				# 28
VT:		.word 16,18,20,0	# 30
		br L1				# 46
		br L2				# 48
		br L3				# 50
L4:		br L4

# EIPC = 0x00008002, SCBP = 0x1E,
# eiic = 0, psw = 0 ... cpu_ID se ne ohrani ...

