
.include "RH850G3M_insts.s"

.include "gpr_init.s"

		mov 4, R3		        #  sys call table size = 4
        br lxx
        mov 0xbabe, R5
lxx:
		LDSR_ID R3, SR_SCCFG, 1	
		mov 0x100, R4	        # 48-bit !
		LDSR_ID R4, SR_SCBP, 1	# SCBP=0x100 (Syscall base pointer)
		SYSCALL 0			# adr=SCBP+0*4=0x100, pc = scbp + mem(0x100) = 0x100 + 0x20 = 0x120
L1:		SYSCALL 1			# adr=SCBP+1*4=0xfee00204, pc = scbp + mem(0xffe00104) = 0xffe00100 + 0x28 = 0xffe00128
L2:		SYSCALL 2			# adr=SCBP+2*4=0xfee00208, pc = scbp + mem(0xffe00108) = 0xffe00100 + 0x30 = 0xffe00130
        SYSCALL 5           # vector greater than SIZE should branch to vector 0
        SYSCALL 250
        br .


		.org 0x100

vec0:	.word 0x20
vec1:	.word 0x60	# vec1=0x104
vec2:	.word 0x1050	# vec2=0x108
vecs:	.word 0, 0, 0, 0, 0	#vecs=0x10C (,110, 114, 118, 11C)

        .org 0x120
		mov 0x20, R5
		eiret
        .org 0x160
		mov 0x21, R5
		eiret
        .org 0x1150
		mov 0x22, R5
		eiret


