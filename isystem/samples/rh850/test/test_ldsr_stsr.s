.text
.include "RH850G3M_insts.s"
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

    mov 0x02ab34cd, r15
	LDSR_ID R15, SR_EBASE, 1
    STSR_ID SR_EBASE, R14, 1

lbl: br lbl
