.data
lbl1: .word 57
lbl2: .byte 89
start = 0xfe000000

# implement instructions missing in V850 (added for E3 - RH850) as macros
.macro LDL_W r1:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07
    .byte 0x78
    .byte 0x03 | (\r3 << 3)
.endm

.equ R1, 1
.equ R3, 3

.text

   # prepare r20, 0
   mov     r6, r20
   movea   0x0014, r0, r7

   # this works as expected
   mov     start, r1
   movea   lbl2 - .data, r1, r7

   movea   lblX, r0, r7 # does not compile correctly with GCC, linker
                        # adds imm16 also to op code: 38 3e 18 00 instead 
                        # of 02 3e 18 00
   mov     10 | 2, r6
   add     r6, r7
lblX:
   mov     r7, r10
   LDL_W   R1, R3

