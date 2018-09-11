.data
# implement instructions missing in V850 (added for E3 - RH850) as macros
.macro LDL_W r1:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07
    .byte 0x78
    .byte 0x03 | (\r3 << 3)
.endm

# how to add rotl imm5, r2, r3 ?


.macro ROTL r1:req, r2:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07 | (\r2 << 3)
    .byte 0xc6
    .byte 0x0 | (\r3 << 3)
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

.equ r0, 0
.equ r1, 1
.equ r2, 2
.equ r3, 3
.equ r4, 4
.equ r5, 5
.equ r6, 6
.equ r7, 7
.equ r8, 8
.equ r9, 9
.equ r10, 10
.equ r11, 11
.equ r12, 12
.equ r13, 13
.equ r14, 14
