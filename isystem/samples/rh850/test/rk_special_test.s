.include "RH850G3M_insts.s"

.data 
var1:    .word 0
var2:    .word 0
var3:    .word 0
var4:    .word 0

.include "gpr_init.s"


#
# Testing instructions:
# CAXI, CALL, CTRET, LDSR, STSR
#

#--------------------- Tests for CAXI --------- OK

	mov 0x5, r4
	mov 0x5, r5
	mov 0xa, r6
	mov 0xc, r7

	mov hilo(var1), r8
	mov hilo(var2), r9
	mov hilo(var3), r10
	mov hilo(var4), r11

	ST_W R4, 0x0, R8	# Mem(r8) = 0x5
	ST_W R5, 0x0, R9	# Mem(r9) = 0x5
	ST_W R7, 0x0, R10	# Mem(r10) = 0xc
	ST_W R6, 0x0, R11	# Mem(r11) = 0xa

	mov 0xabcd, r12
	mov 0x1234, r13
	
	caxi [r8], r4, r12	# if Mem(r8)==r5 then Mem(r8)<-r12 else Mem(r8)<-Mem(r8); r12<-Mem(r8)
						# je, Mem(r5) <- r10(=0xabcde)
	LD_W  0x0, R8, R16

	caxi [r9], r5, r13	# je
	LD_W 0x0, R9, R17

	caxi [r10], r6, r14	# ni
	LD_W 0x0, R10, R18

	caxi [r11], r7, r15	# ni
	LD_W 0x0, R11, R19
	

#--------------------

label1: 

	nop
	ei			# quick test for EI instruction, PSW changes
	nop
	di			# quick test for DI instruction, PSW changes

#--------------------Test for LDSR + STSR -----------


	mov 0xabcd, r1
	mov 0xbade, r2
	mov 0x1234, r3
	mov 0x11111111, r4
	mov 0xffffffff, r5
	mov 0x26467, r6
	mov 0x31fadeff, r7
	mov 0x00f1134, r8
	mov 0xcf53, r9


	ldsr r0, eipc
	ldsr r0, eipsw
	ldsr r1, FEPC
	ldsr r2, FEPSW
    # floating point regs are not available, loading causes exception on hardware
	# ldsr r3, FPSR
	# ldsr r3, FPEPC
	# ldsr r4, FPST
	# ldsr r5, FPCC
	# ldsr r6, FPCFG
	# ldsr r7, FPEC
	ldsr r9, EIIC
	ldsr r2, FEIC
	ldsr r1, CTPC
	ldsr r4, CTPSW
	ldsr r8, CTBP
	ldsr r7, EIWR
	ldsr r6, FEWR
	ldsr r4, BSEL


	LDSR_ID R1, SR_MCFG0, 1
	LDSR_ID R2, SR_RBASE, 1
	LDSR_ID R3, SR_EBASE, 1
	mov 0x01111111, r4   # it seems real proc limits valid values to available FLASH/RAM. Not impl. in QEMU at the moment.
	LDSR_ID R4, SR_INTBP, 1
	mov 0x11111111, r4
	LDSR_ID R6, SR_MCTL, 1
	LDSR_ID R5, SR_PID, 1
	LDSR_ID R8, SR_SCCFG, 1
	LDSR_ID R9, SR_SCBP, 1
	LDSR_ID R4, SR_HTCFG0, 2
	LDSR_ID R5, SR_MEA, 2
	LDSR_ID R1, SR_ASID, 2
	LDSR_ID R6, SR_MEI, 2

	
	stsr FEPC, r1
    # floating point regs are not avail
	# stsr FPSR, r3
	# stsr FPEPC, r4
	# stsr FPST, r6
	# stsr FPCC, r4
	# stsr FPCFG, r5
	# stsr FPEC, r7
	stsr EIIC, r8
	stsr FEIC, r9
	stsr CTPC, r10
	stsr CTPSW, r11
	stsr CTBP, r12
	stsr EIWR, r13
	stsr FEWR, r14
	stsr BSEL, r15

	STSR_ID SR_MCFG0, R1, 1
	STSR_ID SR_RBASE, R4 1
	STSR_ID SR_EBASE, R2, 1
	STSR_ID SR_INTBP, R7, 1
	STSR_ID SR_MCTL, R4, 1
	STSR_ID SR_PID,  R9, 1
	STSR_ID SR_SCCFG, R8, 1
	STSR_ID SR_SCBP, R5, 1
	STSR_ID SR_HTCFG0, R3, 2
	STSR_ID SR_MEA,  R11, 2
	STSR_ID SR_ASID, R13 2
	STSR_ID SR_MEI,  R14, 2

Lbl1:
    br  Lbl1

