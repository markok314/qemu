.text
.include "gpr_init.s"


	#tests for bins (something is wrong with bins)

	#simple tests
	
	#mov 0xff00ff00,r1
	#bins r1,2,4,r2
	
	#test for bsh
	#simple test for bsh
	mov 0x12345678,r1
	bsh r1,r2
	bsh r2,r1
	
	mov 0x7fffffff,r6
	bsh r9,r6
	bsh r6,r9

	#harder test for bsh
	#testing for z flag
	mov 0x00000000,r16
	bsh r16,r17
	bsh r17,r16
	
	#testing for s flag
	mov 0xffffffff,r5
	bsh r5,r1
			
	#testing for cy flag but not z flag
	mov 0x1234ff00,r12
	bsh r12,r2

	#bsh should test this:
	#Result: 0xffff8071 is this cy flag??

	#test for bsw
	#simple test for bsw
	mov 0x12345678,r6
	bsw r6,r10
	bsw r10,r6
	
	mov 0x80000000,r8
	bsw r8,r10
	
	#harder test for bsw
	#checking for z flag
	mov 0x00000000,r7
	bsw r7,r10
	
	#checking for s flag
	
	mov 0x0011000f,r21
	bsw r21, r30
	
	#checking for cy flag
	mov 0x000000f0,r29
	bsw r29,r5
	
	#tests for cmov
	
	
	
	
	
	
	
	
	
Lbl: br Lbl
