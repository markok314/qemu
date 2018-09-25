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



