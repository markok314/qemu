.include "RH850G3M_insts.s"
.include "gpr_init.s"

    # check if QEMU sets/reads PSW the same way as real device
    # bit 30 = 0 to stay in supervisor mode
    # bits 16, 17, 18 are set to 0, because not all devices have coprocessor,
    #     and bits can't beset in HW, but can be set in QEMU
    mov 0xBFF8FFFF, r5
    LDSR r5, SR_PSW
    # works, but can not be tested with the current test suite, because
    # R6 contains also debug bits set, which are missing in QEMU
    # STSR SR_PSW, r6    # only bits 0-7 are read ==> r6 = 0xff

    mov 0x02ab34cd, r5
	LDSR_ID R5, SR_EIPC, 0
	ldsr r5, SR_EIPC
    STSR_ID SR_EIPC, R4, 0
    stsr SR_EIPC, r7

    # RBASE is always 0, can not be written to
    mov 0x02ab34cd, r15
	LDSR_ID R15, SR_RBASE, 1
    STSR_ID SR_RBASE, R13, 1

    # also always 0
    mov 0x02ab34cd, r15
	LDSR_ID R15, SR_BSEL, 0
    STSR_ID SR_BSEL, R13, 0

    # const, contains CPU identifier
    mov 0x02ab34cd, r15
	LDSR_ID R15, SR_HTCFG0, 2
    STSR_ID SR_HTCFG0, R14, 2

    mov 0xffffFFFF, r15
	LDSR_ID R15, SR_MEI, 2
    STSR_ID SR_MEI, R14, 2

    mov 0xffffFFFF, r15
	LDSR_ID R15, SR_INTBP, 1
    STSR_ID SR_INTBP, R14, 1

    mov 0xffffFFFF, r15
	LDSR_ID R15, SR_PID, 1
    STSR_ID SR_PID, R14, 1

    mov 0x02ab34cd, r15
	LDSR_ID R15, SR_EBASE, 1
    STSR_ID SR_EBASE, R14, 1

    mov 0xa5b4c5d6, r6
    LDSR_ID R6, SR_SCCFG, 1
    STSR_ID SR_SCCFG, R7, 1

    # if some bits above bit 24 are 1 (eg 0xa5), then SCBP bits 24:31 = 0xFD
    # this is currently not implemented in QEMU, as detailed doc is missing - 
    # it seems this value depends on RAM location.
    mov 0x00b4c5d7, r6
    LDSR_ID R6, SR_SCBP, 1
    STSR_ID SR_SCBP, R7, 1

    mov 0xa5b4c5d7, r6
    LDSR_ID R6, SR_MCFG0, 1
    STSR_ID SR_MCFG0, R7, 1

    mov 0xa5b4c5d7, r6
    LDSR_ID R6, SR_MCTL, 1
    STSR_ID SR_MCTL, R7, 1

lbl: br lbl
