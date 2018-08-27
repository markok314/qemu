   name    timeOfAssembly
   PUBLIC      __iar_program_start

   section .text:CODE(2)

__iar_program_start:
	
	;ALL LD AND ST INSTRUCTIONS WITH SMALL IMM
	
	ST.B r1,0x40[r0]
	LD.B 0x40[r0],r2
	LD.BU 0x40[r0],r2 	 

	ST.H r1,0x40[r0]
	LD.H 0x40[r0],r2
	LD.HU 0x40[r0],r2
	
	ST.W r1,0x40[r2]
	LD.W 0x40[r2],r2
	
	ST.DW r16, 0x7a [r2]    ;48bit?
	LD.DW 0x40[r2],r2	;48bit?
	
	;ALL LD AND ST INSTRUCTIONS WITH LARGE (>16 bits) IMM
	
	;ST.B r1,0xffff8000[r0]
	;LD.B 0xffff8000[r0],r2
	;LD.BU 0xffff8000[r0],r2

	;ST.H r1,0xffff8000[r0]
	;LD.H 0xffff8000[r0],r2
	;LD.HU 0xffff8000[r0],r2
	
	;ST.W r1,0xffff8000[r2]
	;LD.W 0xffff8000[r2],r2
	
	;ST.DW r16, 0x7a [r2]
	;LD.DW 0x40[r2],r2
	

	;ALL SLD AND SST INSTRUCTIONS
	
	SST.B r1 , 0x20 [ep] 
	SLD.B 0x20 [ep] , r1 
	SLD.BU 0xe [ep] , r1

	SST.H r1 , 0x20 [ep]
	SLD.H 0x20 [ep] , r1
	SLD.HU 0xe [ep] , r1

	SST.W r1 , 0x20 [ep]
	SLD.W 0x20 [ep] , r1
	
	

	
	
   end
