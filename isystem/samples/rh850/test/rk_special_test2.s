.include "RH850G3M_insts.s"

.text   
        jr   initCore    # init code was moved, because otherwise it occupies 
                         # exception address 0x30

    .org 0x30
    mov     12345, r7
    feret

    .org 0x40            # TRAP, vector5 < 16
    mov     0x12345, r7
    eiret

    .org 0x50            # TRAP, vector5 >= 16
    mov     0xf12345, r7
    eiret

        .org 0x78   # breakpoint is set to this address in targetcontrol.py
mainProg: 
        nop
# Testing instructions:
# FETRAP, FERET, TRAP, EIRET, RIE

# -------------------- FETRAP ------------ offset = 0x30

	fetrap 1

	mov 0x6000, r1
	LDSR_ID R1, SR_EBASE, 1	# EBASE = 0x6000

    # set PSW.EBV bit, to use EBASE
    # STSR_ID SR_PSW, R2, 0  # this causes error in test, because of debug bits set in PSW
    # ori     0x8000, r2, r2
    mov     0x8020, r2   # ignore state of other bits in PSW
    mov     0x0020, r3   # ignore state of other bits in PSW
    LDSR_ID R2, SR_PSW, 0

	fetrap 0xc



# --------------------- TRAP -------------------- offset = 40/50

    LDSR_ID R3, SR_PSW, 0    # use RBASE
	trap 0x1	
	trap 0x11	

	mov 0xd000, r1
	LDSR_ID R1, SR_EBASE, 1	# EBASE = 0xd000
    LDSR_ID R2, SR_PSW, 0    # use EBASE

	trap 0x1f
	trap 0x0


Lbl: br Lbl

initCore:
    .include "gpr_init.s"
    jr  mainProg


.org 0x6030
	mov 9, R5
	feret

.org 0xd040
	mov 99, R5
	eiret

.org 0xd050
	eiret		



	

