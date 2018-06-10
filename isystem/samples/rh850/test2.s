   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:

   mov r0, r7 ;na r7 damo 0
   mov r0, r8 ;na r8 damo 0
   add 1, r7 ;r7 pristejemo 1
   
   cmp r7, r8 ;primerjamo r7 in r8
   BNE pristej ;ce sta r7 in r8 razlicna skoci na pristej
   add 10, r8 ;r8 pristejmo 10
  
   pristej:
    NOP
   
   or r7, r8 ;na r8 gre r7 OR r8 r7=1, r8=1
   
   shl r7,r8 ;r8 shift levo za 1 r8=2
   add r8,r7 ; sestejemo registra v r7=3
   add 6, r7 ; r7 = 9
   
   divh r8,r7,r9 ; r7=r7/r8 = 4 r9 = r7/r8 = 1
   
   ;koncni rezultat je r7=4 r8=2 r9=1
   
   end

  