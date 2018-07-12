   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:
; The following instructions are not recognized by IAR assembler, so they
; are coded in hex numbers.
	MOV 0x13, r13
	MOV 0x23, r21
	MOV r15, r8
   end
