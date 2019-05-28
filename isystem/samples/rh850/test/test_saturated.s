.include "RH850G3M_insts.s"
.include "gpr_init.s"

	#simple tests for satadd
	mov 120,r1
	mov 23,r2
	satadd r1,r2
	satadd 15,r2
	satadd r1,r2,r3
	
	#harder tests for satadd positive

	mov 0x7fffffff,r1
	mov 0x5,r2
	satadd r1,r2
	satadd 0x5,r1
	satadd r3,r2,r1
	#adding a little to max positive value
	#result should be saturated to 7fffffff 
	
	mov 5,r2
	satadd r1,r2
	satadd 1,r2
	satadd r1,r2,r3
	#add a negative number to max positive value
	#result should be below 7fffffff

	mov 0x80000000,r1
	mov 0xfffffffe,r2
	satadd r1,r2
	satadd 0x5,r1
	satadd r3,r2,r1
	#add a little negative number to max negative value
	
	#harder tests for satadd negative

	mov 0xffffffff,r1
	mov 0x80000000,r2
	satadd r1,r2
	satadd r1,r3
	satadd 5,r1
	satadd r1,r2,r4
	satadd r1,r3,r4
	#add a max negative number to min negative number
	
	mov 0x7fffffff,r2
	satadd r2,r1
	satadd 4,r2
	satadd r2,r1,r3
	#add a lot to a little positive value	

	#simple tests for satsub
	mov 120,r1
	mov 23,r2
	satsub r1,r2
	satsub r1,r2,r3

	#harder tests for satadd negative
	mov 0x7fffffff,r1
	mov 0xffffffff,r2
	satsub r2,r1,r3 
	satsub r2,r1
	#sub a negative number from max positive
	
	mov 0x80000000,r1
	mov 0xffffffff,r2
	satsub r2,r1,r3
	satsub r2,r1
	#sub a 1 from max negative number
	
	mov 0x80000000,r1
	mov 0x00000005,r2
	satsub r2,r1,r3
	satsub r2,r1
	#sub positive number from max negative value
	
	#simple test for satsubi
	mov 0x5,r1
	satsubi 0x5,r1,r2
	satsubi 40,r1,r2
	satsubi 0x7777,r1,r2

	#harder test for satsubi

	mov 0x7fffffff,r1
	satsubi 0x5,r1,r2
	#sub a little negative value from max positive value
	
	mov 0x7fffffff,r1
	satsubi 0x1,r1,r2
	#sub a little positive value from max positive value
	
	mov 0x80000000,r1
	satsubi 0x1,r1,r2
	#sub a little positive value from max negative value
	
	mov 0x80000000,r1
	satsubi 4,r1,r2
	#sub a little negative value from max negative value
	
	
	#simple tests for satsubr
	mov 120,r1
	mov 23,r2
	satsubr r1,r2

	#harder tests for satsubr negative
	mov 0x7fffffff,r1
	mov 0xffffffff,r2
	satsubr r1,r2
	#sub a max positive from  negative number 
	
	mov 0x80000000,r1
	mov 0xffffffff,r2
	satsubr r1,r2
	#sub a max negative number from  1 
	
	mov 0x80000000,r1
	mov 0x00000005,r2
	satsubr r1,r2
	#sub max negative value from  positive number 
	satsubr r1,r2
	satsubr r1,r2
	satsubr r1,r2
	Lbl: br Lbl
	

	
