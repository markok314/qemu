

.data
lbl1: .word 0x41414141 
lbl2: .byte 0x30

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

   mov     0x44332211, r2
   mov     hilo(data_start_addr), r1
   st.w    r2, 0[r1]

   mov     0xcafe, r5
   mov     hilo(SELF), r6
   jmp     r6
   mov     0xbabe, r5
SELF:
   br      SELF

   # prepare r20, 0
   mov     r6, r20
   movea   0x0014, r0, r7

   mov     hilo(data_start_addr), r1
   movea   lbl2 - .data, r1, r7
   ld.w    0[r1], r6

   movea   lblX, r0, r7 # does not compile correctly with GCC, linker
                        # adds imm16 also to op code: 38 3e 18 00 instead 
                        # of 02 3e 18 00
   mov     10 | 2, r6
   add     r6, r7
lblX:
   mov     r7, r10
   LDL_W   R1, R3

.bss
LB_BSS_A: .byte 0x99
LB_BSS_B: .byte 0x9a
