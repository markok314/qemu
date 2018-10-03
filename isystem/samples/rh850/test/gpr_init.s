.data

# implement instructions missing in V850 (added for E3 - RH850) as macros

.macro LDL_W r1:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07
    .byte 0x78
    .byte 0x03 | (\r3 << 3)
.endm

.macro ROTL_r r1:req, r2:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07 | (\r2 << 3)
    .byte 0xc6
    .byte 0x0 | (\r3 << 3)
.endm

.macro ROTL_i p:req, r2:req, r3:req
    .byte 0xe0 | \p
    .byte 0x07 | (\r2 << 3)
    .byte 0xc4
    .byte 0x0 | (\r3 << 3)
.endm

.macro JARL r1:req, r3:req
    .byte 0xe0 | \r1
    .byte 0xc7
    .byte 0x60
    .byte 0x01 | (\r3 << 3)
.endm

.macro LD_DW disp23:req, r1:req, r3:req
    .byte 0xa0 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7e) << 4) | 0x09 | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro ST_DW r3:req, disp23:req, r1:req
    .byte 0xa0 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7e) << 4) | 0x0f | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro CACHE cacheop:req,r1:req
   .byte 0xe0 | \r1
   .byte 0xe7 | (((\cacheop & 0x60)) >> 2)
   .byte 0x60
   .byte 0x01 | ((\cacheop & 0x1f) << 3)
.endm

.macro PUSHSP rh:req, rt:req
	.byte 0xe0 | \rh
	.byte 0x47
	.byte 0x60
	.byte 0x01 | (\rt << 3)
.endm

.macro POPSP rh:req, rt:req
	.byte 0xe0 | \rh
	.byte 0x67
	.byte 0x60
	.byte 0x01 | (\rt << 3)
.endm

.macro STSR_ID regid:req, r1:req, selid:req
   .byte 0xe0 | \regid
   .byte 0x07 | ((\r1) << 3)
   .byte 0x40 
   .byte 0x00 | ((\selid) << 3)
.endm

.macro LDSR_ID r1:req, regid:req ,selid:req
   .byte 0xe0 | \r1
   .byte 0x07 | ((\regid) << 3)
   .byte 0x20 
   .byte 0x00 | ((\selid) << 3)
.endm

.equ R0, 0
.equ R1, 1
.equ R2, 2
.equ R3, 3
.equ R4, 4
.equ R5, 5
.equ R6, 6
.equ R7, 7
.equ R8, 8
.equ R9, 9
.equ R10, 10
.equ R11, 11
.equ R12, 12
.equ R13, 13
.equ R14, 14
.equ R15, 15
.equ R16, 16
.equ R17, 17
.equ R18, 18
.equ R19, 19
.equ R20, 20
.equ R21, 21
.equ R22, 22
.equ R23, 23
.equ R24, 24
.equ R25, 25
.equ R26, 26
.equ R27, 27
.equ R28, 28
.equ R29, 29
.equ R30, 30
.equ R31, 31


.text

# Here we initialize general purpose registers. 
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
	mov 0x0,r31
