.include "RH850G3M_insts.s"
.text
	
    jr   initCore    # init code was moved, because otherwise it occupies 
                     # exception address 0x60

#----------------------Testing the TRAP instruction-----

	#all possible vectors for trap is from 00 to 1f
	#we checked only 00 and 10 vectors, beacuse others have same offset!

	#if PSW.EBV = 0 then RBASE else EBASE

# handlers for PSW.EBV = 0, when RBASE is used, because RBASE is read-only set to 0.
  	.org 0x40
	mov 0x1,r30			# šele po tem gre psw na 0x60 ...(?)
	eiret
 
	.org 0x50
	mov 0x2,r30			# šele po tem gre psw na 0x60 ...(?)
	eiret 


    .org 0x78   # breakpoint is set to this address in targetcontrol.py
mainProg:
	# RBASE IS NOT changed to 0x2000, because it is read-only!
	mov 0x2000,r10
	LDSR_ID R10, SR_RBASE, 1
    STSR_ID SR_RBASE, R19, 1
	
    # reset PSW.EBV, remain in supervisor mode
    mov 0x00ff, r1  # set also ID bit, because winIDEA handles it in single stepping
    ldsr r1, SR_PSW 

	mov 0x3000,r10
	LDSR_ID R10, SR_EBASE, 1

    trap  [0x00]    # 0x40

	trap  [0x01]    # 0x40

	trap  [0x10]    # 0x50

	trap  [0x11]    #0x50
	
	# change PSW.EBV = 1
	mov 0x8ff,r21
	LDSR r21,PSW

    trap  [0x00]    # 0x3040

	trap  [0x01]    # 0x3040

	trap  [0x10]    # 0x3050

	trap  [0x11]    #0x3050
	
	Lbl: br Lbl


initCore:
    .include "gpr_init.s"
    jr    mainProg


# handlers for TRAP when EBASE is used
	.org 0x3040
	mov 0x3,r30
	eiret 
	
	.org 0x3050
	mov 0x4,r30
	eiret 

