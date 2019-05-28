.include "RH850G3M_insts.s"
.text
        jr   initCore    # init code was moved, because otherwise it occupies 
                         # exception address 0x60

        .org  0x60
        STSR  SR_FEPC, R10
        add   4, r10  # inc return address, otherwise CPU gets locked on RIE instruction
        LDSR  R10, SR_FEPC
        mov     0xefc, r8
        feret

        .org 0x78   # breakpoint is set to this address in targetcontrol.py
mainProg: 
        nop

        # RBASE is const set to 0 in hardware, so jump to 0x60 will be made (PSW.EBV == 0)
        rie
        nop    # RIE above may be 16 or 32-bit

        # set PSW.EBV bit, to use EBASE
        # STSR_ID SR_PSW, R2, 0  # this causes error in test, because of debug bits set in PSW
        # ori     0x8000, r2, r2
        mov     0x8020, r2   # ignore state of other bits in PSW
        ldsr    R2, SR_PSW

		mov 0x1000, r1
		LDSR_ID R1, SR_EBASE, 1	# EBASE = 0x2000

		rie
        nop
        mov     0x35c, r6
Lbl:	BR Lbl

initCore:
        .include "gpr_init.s"

        jr    mainProg

		.org 0x1060
        STSR  SR_FEPC, R10
        add   4, r10  # inc return address, otherwise CPU gets locked on RIE instruction
        LDSR  R10, SR_FEPC
        mov     0xabc, r8
		feret

/*
	FEPC ← PC (return PC)i  čudno, da pride nazaj na isti ukaz ...? That's by design!
	FEPSW ← PSW
	FEIC ← exception cause code (00000060H)
	PSW.UM ← 0
	PSW.NP ← 1
	PSW.EP ← 1
	PSW.ID ← 1
	PC ← exception handler address (offset address 60H)
*/
