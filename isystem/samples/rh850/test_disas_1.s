   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:
; The following instructions are not recognized by IAR assembler, so they
; are coded in hex numbers.
	; CLL
        DB 0xff, 0xff, 0x60, 0xf1 
	; JMP 0x12 [r6]
        DB 0xff, 0x06, 0x4a, 00, 00, 00
	; DISPOSE 0x10, r20-r31
	; DISPOSE 0x7, r2-r6, [r11]
        ; PREPARE R31,124
	; PREPARE r24-r26, 0x8
	; PREPARE r30-r31, 0xf, 0x04
	LD.B 0x1[r7],r1
	LD.B 0x5 [r6],r1
	LD.BU 0xf  [r5],r2
	LD.BU 0x12 [r2],r3
	LD.DW 0x1f [r15],r4
	LD.H 0x26 [r1],r8
	LD.H 0x46 [r1],r13
	LD.HU 0xf2 [r3],r21
	LD.HU 0x42 [r6],r2
	LD.W 0x6 [r4],r4
	LD.W 0x46 [r6],r6
	SLD.B 0x7e [ep],r8
	SLD.BU 0x6 [ep],r10
	SLD.H 0x2 [ep],r12
	SLD.HU 0x6 [ep],r14
	SLD.W 0x4 [ep],r16
	ST.B r2,0x2 [r1]
	ST.B r27, 0xfe [r2]
	ST.DW r16, 0x7a [r3]
	ST.H r14, 0x4c [r9]
	ST.H r12, 0xe6 [r11]
	ST.W r19, 0x6e [r13]
	ST.W r17, 0x12 [r17]
	SST.B r12, 0x14 [ep]
	SST.H r10, 0x4e [ep]
	SST.W r9, 0x4 [ep]
	MUL r8, r5, r7
	MUL 0x13, r2, r3
	MULH r1, r3
	MULH 0x6, r16
	MULHI 0x13, r14, r15
	MULU r1, r21, r12
	MULU 0x48, r17, r15
	MAC r15, r1, r2, r5
	MACU r1, r15, r14, r12
	ADD r21, r1
	ADD 0x4, r1
	ADDI 0x1ff, r6, r2
	CMP r1, r3
	CMP 0xf, r5
	MOV r15, r8
	MOV 0x13, r13
	MOV 0x23, r21
	MOVEA 0xff, r11, r16
	MOVHI 0xf1, r8, r5
	SUB r1, r4
	SUBR r2, r13
	ADF V, r16, r21, r18
	SBF NV, r4, r2, r6
	SATADD r4, r29
	SATADD 0xf, r30
	SATADD r12, r16, r5
	SATSUB r3, r2
	SATSUB r1, r5, r7
	SATSUBI 0xf6, r5, r3
	SATSUBR r2, r1
	AND r6, r16
	ANDI 0x14, r1, r17
	NOT r1, r17
	OR r18, r5
	ORI 0xf1, r4, r2
	TST r6, r8
	TST1 1, 0xe1 [r7]
	TST1 r2, [r8]
	XOR r6, r16
	XORI 0xf8, r11, r2
	BINS r11, 5, 6, r15
	BSH r11, r4
	BSW r1, r6
	CMOV V, r5, r2, r1
	CMOV NV, 0x5, r6, r6
	HSH r1, r6
	HSW r7, r3
	ROTL 0x4, r1, r8
	ROTL r18, r11, r9
	SAR r12, r7
	SAR 0x3, r9
	SAR r17, r1, r7
	SASF NH, r1
	SETF NC, r1
	SHL r5, r2
	SHL 0x6, r3
	SHL r4, r5, r8
	SHR r11, r13
	SHR 0x5, r21
	SHR r2, r6, r9
	SXB r1
	SXH r17
	ZXB r14
	ZXH r12
	SCH0L r11, r9
	SCH0R r8, r7
	SCH1L r6, r5
	SCH1R r4, r3
	DIV r2, r1, r6
	DIVH r7, r8
	DIVH r9, r11, r11
	DIVHU r12, r13, r14
	DIVU r15, r16, r17
	DIVQ r18, r19, r21
	DIVQU r22, r23, r2
	BGE 0x4
	BGT 0x4
	JARL 0xc, r18
	JARL 0xc4, r24
	JARL [r27], r21
	JMP [r6]
	; JMP 0x12 [r6]
	JR 0x68
	JR 0xfe
	LOOP r6, 0xde
	CLR1 4, 0x12 [r3]
	CLR1 r16, [r2]
	NOT1 3, 0x34 [r4]
	NOT1 r15, [r1]
	SET1 2, 0x34 [r5]
	SET1 r1, [r6]
	TST1 5, 0xa [r4] 
	CALLT 0x9
	CAXI [r10], r9, r8
	; CLL
	CTRET
	DI
	; DISPOSE 0x10, r20-r31
	; DISPOSE 0x7, r2-r6, [r11]
	EI
	EIRET
	FERET
	FETRAP 5
	HALT
	LDSR r14, 0xf, 0xa
	LDSR r15, 0x1c
	LDL.W [r16], r3
	NOP
	POPSP r3-r5
        ; PREPARE R31,124
	; PREPARE r24-r26, 0x8
	; PREPARE r30-r31, 0xf, 0x04
	PUSHSP r2-r5
	RIE 0x09, 0xf
	SNOOZE
	STSR 0x3f, r5, 0x6
	STSR 0xa, r1
	STC.W r3, [r2]
	SWITCH r4
	SYNCE
	SYNCI
	SYNCM
	SYNCP
	SYSCALL 15
	TRAP 12
   end
