.include "RH850G3M_insts.s"
.include "gpr_init.s"

#--------------------FETRAP------------offset=0x30

	mov 0x4000, r1
	LDSR_ID R1, 2, 1	# RBASE = 0x4000

	mov 0x6000, r1
	LDSR_ID R1, 3, 1	# EBASE = 0x6000

	fetrap 1

	fetrap 2

	fetrap 3

	fetrap 0xc



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

	mov 0x8000, r1
	LDSR_ID R1, 2, 1	# RBASE = 0x8000

	mov 0xa000, r1
	LDSR_ID R1, 3, 1	# EBASE = 0xa000

	rie


	# rie imm/imm What does this instruciton do on target?

	br Lbl

.org 0x4030
	stsr FEPC, r2
	stsr FEPSW, r3
	stsr FEIC, r4	
	stsr PSW, r5 	# test script only tests lower 5 bits
	feret

.org 0x8060
	# load sysRegs to gpRegs for testing
	stsr FEPC, r2
	stsr FEPSW, r3
	stsr FEIC, r4	
	stsr PSW, r5 	# test script only tests lower 5 bits
	feret

.org 0xb040
	br checkup3

.org 0xb050
	# load sysRegs to gpRegs for testing
	stsr EIPC, r2
	stsr EIPSW, r3
	stsr EIIC, r4	
	stsr PSW, r5 	# test script only tests lower 5 bits
	eiret

checkup:
	# load sysRegs to gpRegs for testing
	stsr FEPC, r2
	stsr FEPSW, r3
	stsr FEIC, r4	
	stsr PSW, r5 	# test script only tests lower 5 bits
	feret

checkup2:
	# load sysRegs to gpRegs for testing
	stsr FEPC, r2
	stsr FEPSW, r3
	stsr FEIC, r4	
	stsr PSW, r5 	# test script only tests lower 5 bits
	feret

checkup3:
	# load sysRegs to gpRegs for testing
	stsr EIPC, r2
	stsr EIPSW, r3
	stsr EIIC, r4	
	stsr PSW, r5 	# test script only tests lower 5 bits
	eiret
	

Lbl: br Lbl
