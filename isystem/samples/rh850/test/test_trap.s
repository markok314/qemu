.include "RH850G3M_insts.s"
.include "gpr_init.s"

	
#----------------------Testing the TRAP instruction-----

	#all possible vectors for trap is from 00 to 1f

	mov 0x2000,r10
	LDSR_ID R10,2,1

	trap  [0x00]
	trap  [0x01]
	trap  [0x02]
	trap  [0x03]
	trap  [0x04]
	trap  [0x05]
	trap  [0x06]
	trap  [0x07]
	trap  [0x08]
	trap  [0x09]
	trap  [0x0a]
	trap  [0x0b]
	trap  [0x0c]
	trap  [0x0d]
	trap  [0x0e]
	trap  [0x0f]

	trap  [0x10]
	trap  [0x11]
	trap  [0x12]
	trap  [0x13]
	trap  [0x14]
	trap  [0x15]
	trap  [0x16]
	trap  [0x17]
	trap  [0x18]
	trap  [0x19]
	trap  [0x1a]
	trap  [0x1b]
	trap  [0x1c]
	trap  [0x1d]
	trap  [0x1e]
	trap  [0x1f]

	#NEED TO CHECK EIPC, EIPSW,EIIC,WHOLE PSW register
	#after we move to some location
	

	.org 0x2040
		STSR EIPC, r1
		STSR EIPSW, r2
		STSR EIIC, r3
		STSR PSW, r4
		feret 
	
	
Lbl:	br Lbl
