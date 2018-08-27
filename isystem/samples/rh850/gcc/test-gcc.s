.data
lbl1: .word 57
lbl2: .byte 89

.text

   # prepare r20, 0
   mov     r6, r20
   movea   0x0014, r0, r7

   # this works as expected
   mov     .data, r0
   movea   lbl2 - .data, r0, r7

   movea   lblX, r0, r7 # does not compile correctly with GCC, linker
                        # adds imm16 also to op code: 38 3e 18 00 instead 
                        # of 02 3e 18 00
   mov     10, r6
   add     r6, r7
lblX:
   mov     r7, r10

