include "RH850G3M_insts.s"
.include "gpr_init.s"

    # check if QEMU sets/reads PSW the same way as real device
    # bit 30 = 0 to stay in supervisor mode
    mov 0xBFFFFFFF, r5
    LDSR r5, SR_PSW
    STSR SR_PSW, r6    # only bits 0-7 are read ==> r6 = 0xff

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
	LDSR_ID R15, SR_BSEL, 1
    STSR_ID SR_BSEL, R13, 1

    # const, contains CPU identifier
    mov 0x02ab34cd, r15
	LDSR_ID R15, SR_HTCFG0, 1
    STSR_ID SR_HTCFG0, R14, 1

    mov 0xffffFFFF, r15
	LDSR_ID R15, SR_MEI, 1
    STSR_ID SR_MEI, R14, 1

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

    mov 0xa5b4c5d7, r6
    LDSR_ID R6, SR_SCBP, 1
    STSR_ID SR_SCBP, R7, 1

    mov 0xa5b4c5d7, r6
    LDSR_ID R6, SR_MCFG0, 1
    STSR_ID SR_MCFG0, R7, 1

    mov 0xa5b4c5d7, r6
    LDSR_ID R6, SR_MCTL, 1
    STSR_ID SR_MCTL, R7, 1

lbl: br lbl
