   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:
	MOV 0x1,r1
	MOV 0x2,r2
	MOV 0x3,r3
	MOV 0x4,r4
	MOV 0x5,r5
	MOV 0x6,r6
	ZXH r1
	ZXB r2
	SXH r3
	SXB r4
	SHR r1,r2
	SHR 0xf,r2
	SHR r1,r2,r3
	SHR r4,r5
	SHR 0xf,r4
	SHR r4,r5,r6
	MOV 0x7,r7
	MOV 0x8,r8
	MOV 0x9,r9
	SAR r7,r8
	SAR 0xf,r7
	SAR r7,r8,r9
	MOV 0xA,r10
	MOV 0xB,r11
	MOV 0xC,r12
	ROTL 0x1,r10,r11
	ROTL r10,r11,r12
	MOV 0xff00ff00,r13
	MOV 0x0,r14
	HSW r14,r13
	HSH r14,r13
	BSW r14,r13
	BSH r14,r13
	;CMOV
	;SASF
	;SETF
	;BINS
	end
