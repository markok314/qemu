.include "RH850G3M_insts.s"
.include "gpr_init.s"

	
#----------------------Testing the TRAP instruction-----

	#all possible vectors for trap is from 00 to 1f
	#we checked only 00 and 10 vectors, beacuse others have same offset!

	#if PSW.EBV = 0 then RBASE else EBASE

    br start # skip handlers, which must be at fixed addresses (RBASE is always 0)

# handlers for PSW.EBV = 0, when RBASE is used, because RBASE is read-only set to 0.
  	.org 0x40
	br check1
back1:
	eiret
 
	.org 0x50
	br check2
back2:
	eiret 


start:
    # check if QEMU sets/reads PSW the same way as real device
    # bit 30 = 0 to stay in supervisor mode
	mov 0xBFFFFFFF, r5
	LDSR r5, PSW
	STSR PSW, r6    # only bits 0-7 are rea ==> r6 = 0xff

	# RBASE IS NOT changed to 0x2000, because it is read-only!
	mov 0x2000,r10
	LDSR_ID R10,2,1
    STSR_ID 2, R19, 1
	
    # reset PSW.EBV, remain in supervisor mode
    mov 0x0000, r1
    LDSR r1, PSW   

	mov 0x3000,r10
	LDSR_ID R10,3,1
	#EBASE changed to 0x3000

    trap  [0x00]    # 0x40
	STSR PSW, r4

	trap  [0x01]    # 0x40
	STSR PSW, r3

	trap  [0x10]    # 0x50
	STSR PSW, r6

	trap  [0x11]    #0x50
	
	# change PSW.EBV = 1
	STSR PSW, r20
	mov 0x8000,r21
	or r20,r21
	LDSR r21,PSW

    trap  [0x00]    # 0x3040
	STSR PSW, r4

	trap  [0x01]    # 0x3040
	STSR PSW, r3

	trap  [0x10]    # 0x3050
	STSR PSW, r6

	trap  [0x11]    #0x3050
	
	Lbl: br Lbl

	# NEED TO CHECK EIPC, EIPSW, EIIC, PSW after we move to some location:
check1:
	mov 0x1,r30			# šele po tem gre psw na 0x60 ...(?)
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


# handlers for TRAP when EBASE is used
	.org 0x3040
		br check3
back3:
		eiret 
	
	.org 0x3050
		br check4
back4:
		eiret 
		
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
