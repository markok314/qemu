.text

# This is a file used for initialization of general purpose registers.
# All registers are set to 0x0. Include this file in test programs by 
# adding ' .include "gpr_init.s" '.

	mov 0x0,r1
	mov 0x0,r2
	mov 0x0,r3
	mov 0x0,r4
	mov 0x0,r5
	mov 0x0,r6
	mov 0x0,r7
	mov 0x0,r8
	mov 0x0,r9
	mov 0x0,r10
	mov 0x0,r11
	mov 0x0,r12
	mov 0x0,r13
	mov 0x0,r14
	mov 0x0,r15
	mov 0x0,r16
	mov 0x0,r17
	mov 0x0,r18
	mov 0x0,r19
	mov 0x0,r20
	mov 0x0,r21
	mov 0x0,r22
	mov 0x0,r23
	mov 0x0,r24
	mov 0x0,r25
	mov 0x0,r26
	mov 0x0,r27
	mov 0x0,r28
	mov 0x0,r29
	mov 0x0,r30
	mov 0x5a,r31
    LDSR_ID R31, SR_ASID, 2
    LDSR_ID R31, SR_CTPC, 0
    LDSR_ID R31, SR_CTBP, 0
    LDSR_ID R31, SR_EIPC, 0
    LDSR_ID R31, SR_EIWR, 0
    LDSR_ID R31, SR_EBASE, 1
    LDSR_ID R31, SR_FEPC, 0
    LDSR_ID R31, SR_FEWR, 0
    LDSR_ID R31, SR_INTBP, 1
    LDSR_ID R31, SR_SCCFG, 1
    LDSR_ID R31, SR_SCBP, 1
    LDSR_ID R31, SR_MEA, 2
    LDSR_ID R31, SR_MEI, 2  # only some bits are undefined
    mov 0x0, r31
