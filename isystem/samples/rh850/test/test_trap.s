.include "RH850G3M_insts.s"
.include "gpr_init.s"

	
#----------------------Testing the TRAP instruction-----

	#all possible vectors for trap is from 00 to 1f
	#we checked only 00 and 10 vectors, beacuse others have same offset!

	#if PSW.EBV = 0 then RBASE else EBASE

	mov 0x2000,r10
	LDSR_ID R10,2,1
	#RBASE changed to 0x2000
	
	mov 0x2000,r10
	LDSR_ID R10,2,1
	#EBASE changed to 0x3000

	
	#change PSW.EBV = 0
	STSR PSW, r20
	mov 0xFFFFBFFF,r21
	and r20,r21
	LDSR r20,PSW
		

	trap  [0x00]
	STSR PSW, r4
	trap  [0x01]
	STSR PSW, r3

	trap  [0x10]
	STSR PSW, r6
	trap  [0x11]
	
	#change PSW.EBV = 1
	STSR PSW, r20
	mov 0x8000,r21
	or r20,r21
	LDSR r20,PSW

	#NEED TO CHECK EIPC, EIPSW,EIIC,WHOLE PSW register
	#after we move to some location

	Lbl: br Lbl

	.org 0x2040
		br check1
		back1:
		eiret
 
	.org 0x2050
		br check2
		back2:
		eiret 
	
	.org 0x3040
		br check3
		back3:
		eiret 
	
	.org 0x3050
		br check4
		back4:
		eiret 
		
check1:
	mov 0x1,r30
	STSR EIPC, r1
	STSR EIPSW, r2
	STSR EIIC, r3
	STSR PSW, r4
	br back1

check2:
	mov 0x2,r30
	STSR EIPC, r1
	STSR EIPSW, r2
	STSR EIIC, r3
	STSR PSW, r4
	br back2

check3:
	mov 0x3,r30
	STSR EIPC, r1
	STSR EIPSW, r2
	STSR EIIC, r3
	STSR PSW, r4
	br back3

check4:
	mov 0x4,r30
	STSR EIPC, r1
	STSR EIPSW, r2
	STSR EIIC, r3
	STSR PSW, r4
	br back4
