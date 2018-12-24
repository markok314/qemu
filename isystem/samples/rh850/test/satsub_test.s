.include "RH850G3M_insts.s"
.include "gpr_init.s"

# Testing of SATSUB, SATSUBI, SATSUBR.
# This is a test combined form rk_sat_op_test.s and
# test_saturated.s to isolate and test saturated subtraction 
# operations only.



#---------------------------------satsub---

	mov 0x5f, r1
	mov -0x7, r2
	mov 0x7ffffff0, r3
	mov 0x80000000, r4
	mov 0x12345678, r5
	mov 0xffffffff, r6
	mov 0x00444444, r7
	
	satsub r5, r3, r8	# pos-pos=pos; no flag is set
	satsub r4, r7, r8	# pos-neg=MAX; SAT, CY and OV flags are set
	ldsr r0, psw		# clearing PSW register

	satsub r2, r1, r9	# pos-neg=pos; CY flag is set
	satsub r3, r1, r10	# pos-pos=neg; CY and S flags are set
	satsub r7, r10		# neg-pos=MIN; SAT, CY and S flags are set
	ldsr r0, psw		# clearing PSW register

	satsub r1, r2, r11	# neg-pos=neg; S flag is set
	satsub r4, r2, r11	# neg-neg=pos; no flag is set


	mov 0x7fffffff, r3	
	mov 0xc2463987, r6

	satsub r2, r6		# neg-neg=neg; S flag is set
	satsub r4, r3, r16	# MAX-MIN=MAX; SAT, CY and OV flags are set
	ldsr r0, psw

	satsub r3, r4, r17	# MIN-MAX=MIN; SAT, OV and S flags are set

	ldsr r0, psw
#----------------------------------satsubi------

	mov 0x5f, r1
	mov -0x7, r2
	mov 0x7ffffff0, r3
	mov 0x80000005, r4
	mov 0x12345678, r5
	mov 0xffffffff, r6
	mov 0xcdefcdef, r7
	mov 0xfff43256, r8


	satsubi 0x7fff, r3, r10		# pos-pos=pos; CY adn OV flags are set
	satsubi 0x1234, r1, r11		# pos-pos=neg; CY and S flags are set
	satsubi -0x55, r5, r12		# pos-neg=pos; no flag is set 
	satsubi -0xff, r3, r13		# pos-neg=MAX; SAT flag is set
	ldsr r0, psw			# clearing PSW register

	satsubi -0x7, r8, r14		# neg-neg=neg; S flag is set
	satsubi -0x7777, r2, r15	# neg-neg=pos; no flag is set
	satsubi 0x4323, r7, r16		# neg-pos=neg; CY and S flags are set
	satsubi 0xadd, r4, r17		# neg-pos=MIN; SAT, CY and S flags are set

	ldsr r0, psw			# clearing PSW register

#-----------------------------------satsubr-----

	mov 0x7f, r1
	mov -0x9, r2
	mov 0x7ffffff2, r3
	mov 0x00099999, r4
	mov 0x12344678, r5
	mov 0xfff93dff, r6
	mov 0xcdefcdef, r7
	mov 0xff521256, r8
	mov r7, r10

	satsubr r3, r4		# pos-pos=pos; CY and OV flags are set
	satsubr r1, r5		# pos-pos=neg; CY and S flags are set
	satsubr r4, r2		# pos-neg=pos; no flag is set
	satsubr r3, r7		# pos-neg=MAX; SAT flag is set
	ldsr r0, psw		# clearing PSW register

	mov -0x9, r2
	mov r8, r9

	satsubr r10, r2		# neg-neg=neg; S flag is set
	satsubr r6, r8		# neg-neg=pos; no flag is set
	satsubr r9, r1		# neg-pos=neg; CY and S flags are set
	satsubr r5, r4		# neg-pos=MIN; SAT and S flags are set

	ldsr r0, psw		# clearing PSW register
#------------------------------------------





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
