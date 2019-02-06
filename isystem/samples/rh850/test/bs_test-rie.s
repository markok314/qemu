.include "RH850G3M_insts.s"

.include "gpr_init.s"

		#mov 0x1000, r1
		#LDSR_ID R1, 2, 1	# RBASE = 0x1000

		mov 0x0, r1
		LDSR_ID R1, 2, 1	# RBASE = 0x0 (kot na čipu)

		mov 0x2000, r1
		LDSR_ID R1, 3, 1	# EBASE = 0x2000

		rie

		.org 0x60
Lbl:	BR Lbl

#		.org 0x1060	# ne deluje na čipu, očitno je reset vektor RBASE = 0
#Lbl:	BR Lbl
		feret

/*
	FEPC ← PC (return PC)		čudno, da pride nazaj na isti ukaz ...?
	FEPSW ← PSW
	FEIC ← exception cause code (00000060H)
	PSW.UM ← 0
	PSW.NP ← 1
	PSW.EP ← 1
	PSW.ID ← 1
	PC ← exception handler address (offset address 60H)
*/
