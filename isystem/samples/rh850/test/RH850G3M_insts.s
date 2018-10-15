.data

# implement instructions missing in V850 (added for E3 - RH850) as macros

.macro LDL_W r1:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07
    .byte 0x78
    .byte 0x03 | (\r3 << 3)
.endm

.macro STC_W r3:req, r1:req
    .byte 0xe0 | \r1
    .byte 0x07
    .byte 0x7a
    .byte 0x03 | (\r3 << 3)
.endm

.macro CLL
    .hword 0xffff
    .byte 0x60
    .byte 0xf1
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

.macro LOOP r1:req, disp16:req
    .byte 0xe0 | \r1
    .byte 0x06
   	.hword \disp16 | 0x1
.endm

.macro CACHE cacheop:req,r1:req
   .byte 0xe0 | \r1
   .byte 0xe7 | (((\cacheop & 0x60)) >> 2)
   .byte 0x60
   .byte 0x01 | ((\cacheop & 0x1f) << 3)
.endm

.macro BINS1 r1:req, pos:req, width:req, r2:req
   .byte 0xe0 | \r2
   .byte 0x07 | (\r1 << 3)
   .byte 0x90 | ((\pos & 0x7) << 1)
   .byte 0x00 | ((\pos & 0x8)) | (((\width + \pos - 0x1 )& 0xf) << 4)
.endm

.macro BINS2 r1:req, pos:req, width:req, r2:req
   .byte 0xe0 | \r2
   .byte 0x07 | (\r1 << 3)
   .byte 0xB0 | ((\pos & 0x7) << 1)
   .byte 0x00 | ((\pos & 0x8)) | (((\width + \pos - 0x1 )& 0xf) << 4)
.endm

.macro BINS3 r1:req, pos:req, width:req, r2:req
   .byte 0xe0 | \r2
   .byte 0x07 | (\r1 << 3)
   .byte 0xD0 | ((\pos & 0x7) << 1)
   .byte 0x00 | ((\pos & 0x8)) | (((\width + \pos - 0x1 )& 0xf) << 4)
.endm

.macro PREF prefop:req,r1
   .byte 0xe0 | \r1
   .byte 0xdf 
   .byte 0x60
   .byte 0x01 | (\prefop << 3)
.endm

.macro SNOOZE
    .byte 0xe0
    .byte 0x0f
    .byte 0x20
    .byte 0x01
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

