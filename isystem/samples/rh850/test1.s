   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:

   ; prepare r20, 0
   mov     r6, r20
   movea   0x0014, r0, r7
   mov     10, r6
   add     r6, r7
   mov     r7, r10
   ldl.w  [r1], r3

   end
