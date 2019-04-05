.data

# implement instructions missing in V850 (added for E3 - RH850) as macros

.macro BINS r2:req, pos:req, width:req, r1:req
   .byte 0xe0 | \r2
   .byte 0x07 | (\r1 << 3)
   .if \pos>=0x10
   .byte 0x90 | ((\pos & 0x7) << 1)
   .else
   .if (\width + \pos - 0x1)>=0x10
   .byte 0xB0 | ((\pos & 0x7) << 1)
   .else
   .byte 0xD0 | ((\pos & 0x7) << 1)
   .endif
   .endif
   .byte 0x00 | ((\pos & 0x8)) | (((\width + \pos - 0x1 )& 0xf) << 4)
.endm


.macro CACHE cacheop:req,r1:req
   .byte 0xe0 | \r1
   .byte 0xe7 | (((\cacheop & 0x60)) >> 2)
   .byte 0x60
   .byte 0x01 | ((\cacheop & 0x1f) << 3)
.endm

.macro CLL
    .hword 0xffff
    .byte 0x60
    .byte 0xf1
.endm

.macro JARL_ disp32:req, r1:req
    .byte 0xe0 | \r1
    .byte 0x02
    .word \disp32 & 0xfffffffe
.endm

.macro JARL_RR r1:req, r3:req
    .byte 0xe0 | \r1
    .byte 0xc7
    .byte 0x60
    .byte 0x01 | (\r3 << 3)
.endm

.macro JR_ disp32:req
    .hword 0x02e0
    .word \disp32
.endm

.macro LD_B disp23:req, r1:req, r3:req
    .byte 0x80 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7f) << 4) | 0x05 | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro LD_BU disp23:req, r1:req, r3:req
    .byte 0xa0 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7f) << 4) | 0x05 | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro LD_DW disp23:req, r1:req, r3:req
    .byte 0xa0 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7e) << 4) | 0x09 | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

#.macro LD.DW p:req, r1:req, r3:req
#    .byte 0xa0 | \r1
#    .byte 0x07
#    .byte 0x09 | ((\p & 0x7) << 5)
#    .byte ((\p & 0x38) >> 3) | (\r3 << 3)
#    .byte ((\p & 0x3fc0) >> 6)
#    .byte ((\p & 0x3fc000) >> 14)
#.endm

.macro LD_H disp23:req, r1:req, r3:req
    .byte 0x80 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7e) << 4) | 0x07 | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro LD_HU disp23:req, r1:req, r3:req
    .byte 0xa0 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7e) << 4) | 0x07 | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro LDL_W r1:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07
    .byte 0x78
    .byte 0x03 | (\r3 << 3)
.endm

.macro LDSR_ID r1:req, regid:req ,selid:req
   .byte 0xe0 | \r1
   .byte 0x07 | ((\regid) << 3)
   .byte 0x20
   .byte 0x00 | ((\selid) << 3)
.endm

.macro LD_W disp23:req, r1:req, r3:req
    .byte 0x80 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7e) << 4) | 0x09 | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro LOOP r1:req, disp16:req
    .byte 0xe0 | \r1
    .byte 0x06
   	.hword \disp16 | 0x1
.endm

.macro POPSP rh:req, rt:req
	.byte 0xe0 | \rh
	.byte 0x67
	.byte 0x60
	.byte 0x01 | (\rt << 3)
.endm

.macro PREF prefop:req,r1
   .byte 0xe0 | \r1
   .byte 0xdf
   .byte 0x60
   .byte 0x01 | (\prefop << 3)
.endm

.macro PUSHSP rh:req, rt:req
	.byte 0xe0 | \rh
	.byte 0x47
	.byte 0x60
	.byte 0x01 | (\rt << 3)
.endm

.macro ROTL_i p:req, r2:req, r3:req
    .byte 0xe0 | \p
    .byte 0x07 | (\r2 << 3)
    .byte 0xc4
    .byte 0x0 | (\r3 << 3)
.endm

.macro ROTL_r r1:req, r2:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07 | (\r2 << 3)
    .byte 0xc6
    .byte 0x0 | (\r3 << 3)
.endm

.macro SNOOZE
    .byte 0xe0
    .byte 0x0f
    .byte 0x20
    .byte 0x01
.endm

.macro ST_B r3:req, disp23:req, r1:req
    .byte 0x80 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7f) << 4) | 0x0d | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro ST_DW r3:req, disp23:req, r1:req
    .byte 0xa0 | \r1
    .byte 0x07
   	.hword lo((\disp23 & 0x7e) << 4) | 0x0f | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

#.macro ST.DW r3:req, p:req, r1:req
#    .byte 0xa0 | \r1
#    .byte 0x07
#    .byte 0x09 | ((\p & 0x7) << 5)
#    .byte ((\p & 0x38) >> 3) | (\r3 << 3)
#    .byte ((\p & 0x3fc0) >> 6)
#    .byte ((\p & 0x3fc000) >> 14)
#.endm

.macro ST_H r3:req, disp23:req, r1:req
    .hword 0x07a0 | \r1
   	.hword lo((\disp23 & 0x7e) << 4) | 0x0d | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
.endm

.macro STC_W r3:req, r1:req
    .byte 0xe0 | \r1
    .byte 0x07
    .byte 0x7a
    .byte 0x03 | (\r3 << 3)
.endm

.macro STSR_ID regid:req, r1:req, selid:req
   .byte 0xe0 | \regid
   .byte 0x07 | ((\r1) << 3)
   .byte 0x40
   .byte 0x00 | ((\selid) << 3)
.endm

.macro ST_W r3:req, disp23:req, r1:req
    .hword 0x0780 | \r1
   	.hword lo((\disp23 & 0x7e) << 4) | 0x0f | (\r3 << 11)
   	.hword lo(\disp23 >> 7)
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

# selID = 0
.equ SR_EIPC, 0
.equ SR_EIPSW, 1
.equ SR_FEPC, 2
.equ SR_FEPSW, 3
.equ SR_PSW, 5

# selID = 1
.equ SR_RBASE, 2
.equ SR_EBASE, 3
.equ SR_SCBP, 12

