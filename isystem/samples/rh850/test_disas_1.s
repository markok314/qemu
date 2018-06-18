   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:
	LD.B disp16 [r7],r1
	LD.B disp23 [r6],r1
	LD.BU disp16 [r5],r2
	LD.BU disp23 [r2],r3
	LD.DW disp23 [r15],r5
	LD.H disp16 [r1],r8
	LD.H disp23[r1],r13
	LD.HU disp16 [r3],r21
	LD.HU disp23 [r6],r2
	LD.W disp16 [r4],r4
	LD.W disp23 [r6],r6
	SLD.B disp7 [ep],r8
	SLD.BU disp4 [ep],r10
	SLD.H disp8 [ep],r12
	SLD.HU disp5 [ep],r14
	SLD.W disp8 [ep],r16
	ST.B r2, disp16 [r1]
	ST.B r27, disp23 [r2]
	ST.DW r16, disp23 [r3]
	ST.H r14, disp16 [r9]
	ST.H r12, disp23 [r11]
	ST.W r19, disp16 [r13]
	ST.W r17, disp23 [r17]
	SST.B r12, disp7 [ep]
	SST.H r10, disp8 [ep]
	SST.W r9, disp8 [ep]
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
	ADF BE, r16, r21, r18
	SBF BE, r4, r2, r6
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
	TST1 1, disp16 [r7]
	TST1 r2, [r8]
	XOR r6, r16
	XORI 0xf8, r11, r2
	BINS r11, 5, 6, r15
	BSH r11, r4
	BSW r1, r6
	CMOV BE, r5, r2, r1
	CMOV BC, 0x5, r6, r6
	HSH r1, r6
	HSW r7, r3
	ROTL 0x4, r1, r8
	ROTL r18, r11, r9
	SAR r12, r7
	SAR 0x3, r9
	SAR r17, r1, r7
	SASF BE, r1
	SETF BE, r1
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
	Bcond disp9
	Bcond disp17
	JARL disp22, r18
	JARL disp32, r24
	JARL [r27], r21
	JMP [r6]
	JMP disp32 [r7]
	JR disp22
	JR disp32
	LOOP r6, disp16
	CLR1 4, disp16 [r3]
	CLR1 r16, [r2]
	NOT1 3, disp16 [r4]
	NOT1 r15, [r1]
	SET1 2, disp16 [r5]
	SET1 r1, [r6]
	TST1 5, disp16 [r4] 
	CALLT 0x9
	CAXI [r10], r9, r8
	CLL
	CTRET
	DI
	DISPOSE 0x41, list12
	DISPOSE 0x7, list12, [r11]
	EI
	EIRET
	FERET
	FETRAP vector4
	HALT
	LDSR r14, regID, selID 
	LDSR r15, regID 
	LDL.W [r16], r3
	NOP
	POPSP rh-rt
	PREPARE list12, 0x1
	PREPARE list12, 0xf, sp/imm"na"note ??? (str.275)
	PUSHSP rh-rt
	RIE 0x09, 0xf
	SNOOZE
	STSR regID, r5, selID
	STSR regID, r1
	STC.W r3, [r2]
	SWITCH r4
	SYNCE
	SYNCI
	SYNCM
	SYNCP
	SYSCALL vector8
	TRAP vector5
   end
