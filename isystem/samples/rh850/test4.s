   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:

   mov r0, r7 ;na r7 damo 0
   mov r0, r8 ;na r8 damo 0
   add 10, r7 ;r7 pristejemo 10
   
   cmp r7, r8 ;primerjamo r7 in r10
   BE pristej ;ce sta r7 in r8 enaka skoci na pristej
   add 10, r8 ;r8 pristejmo 10
  
   pristej:
    NOP
   
   xor r8,r7 ;na r7 shranimo r7 xor r8 rez=0
   and r8,r7 ;na r7 shranimo r7 and r8 rez=0
  
   ;koncni rezultat je r7=0 r8=10
   end

  