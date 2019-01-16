.include "RH850G3M_insts.s"
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

	mov 0xfee0006c, r8
	mov 0xfee00080, r9
	mov 0xfee00084, r10
	mov 0xfee00088, r11

	ST_H R4, 0x0, R8	# Mem(r8) = 0x5
	ST_H R5, 0x0, R9	# Mem(r9) = 0x5
	ST_H R7, 0x0, R10	# Mem(r10) = 0xc
	ST_H R6, 0x0, R11	# Mem(r11) = 0xa

	mov 0xabcd, r12
	mov 0x1234, r13
	
	caxi [r8], r4, r12	# if Mem(r8)==r5 then Mem(r8)<-r12 else Mem(r8)<-Mem(r8); r12<-Mem(r8)
						# je, Mem(r5) <- r10(=0xabcde)
	LD_HU 0x0, R8, R16

	caxi [r9], r5, r13	# je
	LD_HU 0x0, R9, R17

	caxi [r10], r6, r14	# ni
	LD_HU 0x0, R10, R18

	caxi [r11], r7, r15	# ni
	LD_HU 0x0, R11, R19
	
#--------------------Test for CALLT+CTRET -----------

	mov 0x11, r1
	ldsr r1, psw 

	mov 0xfee00200, r1
	LDSR_ID R1, 20, 0	# CTBP = 0xfee00200

	mov 0x20, r2
	st.h r2, 0x0[r1]	# Mem(0xfee00200) <- 0x20

	# storing CTRET opcode
	mov 0xfee00220, r1
	mov 0x014407E0, r2
	st.w r2, 0x0[r1]	# Mem(0xfee00220) <- 0x014407E0

	callt 0x00	# adr <- 0xfee00200 + 2*imm = 0xfee00200 (iz tega pobere halfword 0x0020)
				# PC <- 0xfee00200 + 0x0020 = 0xfee00220

#--------------------

	mov 0x9, r1
	ldsr r1, psw 

	mov 0xfee00500, r1
	LDSR_ID R1, 20, 0	# CTBP = 0xfee00500

	mov 0xfee00520, r1
	mov 0x40, r2
	st.h r2, 0x0[r1]	# Mem(0xfee00520) <- 0x40

	# storing CTRET opcode
	mov 0xfee00540, r1
	mov 0x014407E0, r2
	st.w r2, 0x0[r1]	# Mem(0xfee00540) <- 0x014407E0

	callt 0x10	# adr <- CTBP + 2*imm = 0xfee00500 + 2*0x10 = 0xfee00520
				# PC <- CTBP + ld.h(adr) = 0xfee00500 + 0x40 = 0xfee00540

#--------------------

	mov 0xd, r1
	ldsr r1, psw 

	mov 0xfee00900, r1
	LDSR_ID R1, 20, 0	# CTBP = 0xfee00900

	mov 0xfee00906, r1
	mov 0x100, r2
	st.h r2, 0x0[r1]	# Mem(0xfee00906) <- 0x100

	# storing CTRET opcode
	mov 0xfee00a00, r1
	mov 0x014407E0, r2
	st.w r2, 0x0[r1]	# Mem(0xfee00a00) <- 0x014407E0

	callt 0x3	# adr <- 0xfee00906 (CTBP+imm)
			# PC <-0xfee00a00 (CTBP+load(adr))
	


label1: 

	nop
	ei			# quick test for EI instruction
	stsr psw, r1	
	nop
	di			# quick test for DI instruction
	stsr psw, r1	

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
	ldsr r3, FPSR
	ldsr r3, FPEPC
	ldsr r4, FPST
	ldsr r5, FPCC
	ldsr r6, FPCFG
	ldsr r7, FPEC
	ldsr r9, EIIC
	ldsr r2, FEIC
	ldsr r1, CTPC
	ldsr r4, CTPSW
	ldsr r8, CTBP
	ldsr r7, EIWR
	ldsr r6, FEWR
	ldsr r4, BSEL


	LDSR_ID R1, 0, 1
	LDSR_ID R2, 2, 1
	LDSR_ID R3, 3, 1
	LDSR_ID R4, 4, 1
	LDSR_ID R6, 5, 1
	LDSR_ID R5, 6, 1
	LDSR_ID R8, 11, 1
	LDSR_ID R9, 12, 1
	LDSR_ID R4, 0, 2
	LDSR_ID R5, 6, 2
	LDSR_ID R1, 7, 2
	LDSR_ID R6, 8, 2

	
	stsr FEPC, r1
	stsr FEPSW, r2
	stsr FPSR, r3
	stsr FPEPC, r4
	stsr FPST, r6
	stsr FPCC, r4
	stsr FPCFG, r5
	stsr FPEC, r7
	stsr EIIC, r8
	stsr FEIC, r9
	stsr CTPC, r10
	stsr CTPSW, r11
	stsr CTBP, r12
	stsr EIWR, r13
	stsr FEWR, r14
	stsr BSEL, r15

	STSR_ID 0, R1, 1
	STSR_ID 2, R4 1
	STSR_ID 3, R2, 1
	STSR_ID 4, R7, 1
	STSR_ID 5, R4, 1
	STSR_ID 6, R9, 1
	STSR_ID 11, R8, 1
	STSR_ID 12, R5, 1
	STSR_ID 0, R3, 2
	STSR_ID 6, R11, 2
	STSR_ID 7, R13 2
	STSR_ID 8, R14, 2


#--------------------------imported from rk_special_test2.s
#----------------------------------------------------
#----------------------------------------------------
#----------------------------------------------------

#
# Testing instructions:
# FETRAP, FERET, TRAP, EIRET, RIE
#

#--------------------FETRAP------------offset=0x30

	mov 0x4000, r1
	LDSR_ID R1, 2, 1	# RBASE = 0x4000

	mov 0x6000, r1
	LDSR_ID R1, 3, 1	# EBASE = 0x6000

	fetrap 1
	fetrap 2
	fetrap 3
	fetrap 0xc

.org 0x1060
	feret

.org 0x4030
	feret

.org 0x6030
	feret

#---------------------TRAP--------------------offset=40/50

	mov 0xb000, r1
	LDSR_ID R1, 2, 1	# RBASE = 0xb000

	mov 0xd000, r1
	LDSR_ID R1, 3, 1	# EBASE = 0xd000

	trap 0x11	

	trap 0x1f

	trap 0x0

	trap 0xc

	trap 0x7


#---------------------RIE--------------------offset=0x60

	mov 0x1000, r1
	LDSR_ID R1, 2, 1	# RBASE = 0x1000

	mov 0x2000, r1
	LDSR_ID R1, 3, 1	# EBASE = 0x2000

	#rie		# stores this PC as return addr

	# rie imm/imm What does this instruciton do on target?


.org 0xb040
	eiret

.org 0xb050
	eiret

.org 0xd040
	eiret

.org 0xd050
	eiret


