   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:
	mov 0x12345678,r1
	mov 0x0,r2
	mov 0x11111111,r5
	ST.W r1,0x20[r2]
	LD.W 0x20[r2],r5
	
	;mov 0x77654321,r4
	;ST.H r4,0x20[r0]
	;LD.H 0x20[r0],r5
	

	;ST.B r1,0x40[r0]
	;LD.B 0x40[r0],r2
	;LD.H 0x40[r0],r3
	;LD.W 0x40[r0],r4
	
	;ST.W r5,0x80[r0]
	;ST.H r1,0x40[r0]
	;LD.B 0x80[r0],r6
	;LD.H 0x40[r0],r7
	;LD.W 0x80[r0],r8

	;ST.W r1,0x20[r0]
	;LD.B 0x20[r0],r11
	;LD.H 0x20[r0],r12
	;LD.W 0x20[r0],r13
	
   end
