.include "RH850G3M_insts.s"
.include "gpr_init.s"

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

	rie		# stores this PC as return addr


	# rie imm/imm What does this instruciton do on target?

	br Lbl

.org 0x1060
	feret

.org 0x4030
	feret

.org 0x6030
	feret



.org 0xb040
	eiret

.org 0xb050
	eiret

.org 0xd040
	eiret

.org 0xd050
	eiret		



	

Lbl: br Lbl
