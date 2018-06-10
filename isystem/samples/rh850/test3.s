   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:

   mov r0, r7 ;na r7 damo 0
   mov r0, r8 ;na r8 damo 0
   add 4, r7 ;r7 pristejemo 4
   add 2, r8 ;r8 pristejemo 2
   mulh r8,r7 ;r7 = r8*r7=8
   
   NOP
   NOP
   
   ;r7 = 8 r8 = 2
   satadd r8, r7 ;r7 = 10
   mov r0, r9 ;na r9 damo 0
   add 2, r9 ;r9 pristejemo 2
   shl r9,r8 ; r8 shiftama za r9, r8 = 8
   satsub r8,r7 ;r7 = r7 - r8 = 2
   
   ;koncni rezultat je r7 = 2, r8 = 8, r9 = 2
  
   end