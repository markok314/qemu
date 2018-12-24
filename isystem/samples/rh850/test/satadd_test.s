.include "RH850G3M_insts.s"
.include "gpr_init.s"


# Testing of SATADD.
# This is a test combined form rk_sat_op_test.s and
# test_saturated.s to isolate and test saturated addition 
# operations only.





	mov 0x5f, r1
	mov -0x7, r2
	mov 0x7ffffff0, r3
	mov 0x80000000, r4
	mov 0x12345678, r5
	mov 0xffffffff, r6

#----------------------------------satadd---

	satadd r1, r5, r15	# pos+pos=pos; no flag is set	
	satadd r2, r5, r15	# neg+pos=pos; CY flag is set
	satadd r3, r2, r8	# pos+neg=pos; CY flag is set
	satadd r3, r4, r9	# pos+MIN=neg; S flag is set
	satadd r1, r2, r7	# pos+neg=pos; CY flag is set ##DOUBLE##

	satadd r4, r2, r10	# MIN+neg=MIN; SAT, OV, CY and S flags are set
	ldsr r0, psw		# clearing PSW register

	satadd r4, r5, r11	# MIN+pos=neg; S flag is set
	satadd r3, r6, r12	# pos+neg=pos; CY flag is set ##DOUBLE##
	satadd r1, r8		# pos+pos=MAX; SAT and OV flags are set
	satadd -0x5, r10	# neg+MIN=MIN; SAT, OV, CY and S flags are set 
	ldsr r0, psw		# clearing PSW register

	satadd r9, r10		# neg+MIN=MIN; SAT, OV, CY and S flags are set 
	ldsr r0, psw		# clearing PSW register

	satadd 0xf, r12		# pos+pos=pos; no flag is set
	satadd 0xf, r12		# pos+pos=MAX; SAT and OV flags are set
	ldsr r0, psw		# clearing PSW register

	satadd r2, r6		# neg+neg=neg; CY and S flags are set



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



Lbl:	br Lbl

