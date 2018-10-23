
.include "gpr_init.s"


	mov 0xffff0000, r1
	mov 0x00005678, r2
	mov 0xff000f00, r3
	mov 0xadac0000, r4
	mov 0x1f2e3d8c, r5
	mov 0xf3450112, r6
	mov 0xffffffff, r7	
	mov 0x80000000, r11
	mov 0xffffffff, r12
	mov 0x4, r8
	mov -0x4, r9
	


#----------------------Testing the DIV instruction-----

	div r8, r1, r10
	div r9, r2, r10
	div r1, r3, r10
	div r5, r7, r10
	div r3, r9, r10
	div r10, r2, r10
	div r5, r2, r10
	div r7, r5, r1		#values should be undefined
	div r11, r12, r2	#which value should be 0x80000000?
	div r6, r3, r3		#flags should be undefined?


#----------------------Testing the DIVH instruction-----

	mov 0xff123000, r1
	mov 0x42005678, r2
	mov 0xff055f00, r3
	mov 0x11240a00, r4
	mov 0xcf2e3d8c, r5
	mov 0x134a000f, r6
	mov 0xffffffff, r7
	mov 0x80000000, r11
	mov 0xffffffff, r12
	mov 0x4, r8
	mov -0x4, r9
	mov 0x0, r15


	divh r5, r1, r13
	divh r9, r4, r13
	divh r3, r2, r13
	divh r5, r7, r13
	divh r1, r8, r8
	divh r13, r2, r13
	divh r5, r9, r13
	divh r7, r5, r1		
	divh r11, r12, r2	
	divh r4, r3, r3		

	divh r5, r3
	divh r4, r4
	divh r3, r2
	divh r6, r1
	divh r1, r12
	divh r13, r2
	divh r5, r15
	divh r7, r11
	divh r1, r7
	divh r4, r3

#----------------------Testing the DIVHU instruction-----

	mov 0xff123000, r1
	mov 0x42085678, r2
	mov 0xff055f00, r3
	mov 0x11248a00, r4
	mov 0xcf2e3d8c, r5
	mov 0x134a000f, r6
	mov 0xffffffff, r7
	mov 0x80001000, r11
	mov 0xffffffff, r12
	mov 0x4, r8
	mov -0x4, r9


	divhu r5, r1, r13
	divhu r9, r4, r13
	divhu r3, r2, r13
	divhu r5, r7, r13
	divhu r15, r2, r8
	divhu r13, r13, r13
	divhu r5, r9, r13
	divhu r7, r5, r5		
	divhu r11, r12, r2	
	divhu r4, r3, r3		



#----------------------Testing the DIVU instruction-----

	mov 0xff123000, r1
	mov 0x42085678, r2
	mov 0xff055f00, r3
	mov 0x11248a00, r4
	mov 0x134a030f, r5
	mov 0x23455f00, r6
	mov 0x11248a00, r7
	mov 0xcf2e3d8c, r8
	mov 0xffffffff, r9
	mov 0x80001000, r11
	mov 0xffffffff, r12
	mov 0x4, r8
	mov -0x4, r9

	divu r1, r8, r8
	divu r13, r2, r13
	divu r5, r9, r13
	divu r7, r5, r1		
	divu r11, r12, r2	
	divu r4, r3, r3	
	divu r15, r3, r1
	divu r4, r9, r4
	divu r2, r2, r8
	divu r6, r1, r6
	divu r1, r12, r4





#----------------------Testing the DIVQ instruction-----


	mov 0xba234560, r1
	mov 0x24900fff, r2
	mov 0x000c1300, r3
	mov 0xff122000, r4
	mov 0xcccccc12, r5
	mov 0x01900129, r6
	mov 0x11248a00, r7
	mov 0xcf2e3d8c, r8
	mov 0xffffffff, r9
	mov 0x80001000, r10

	divq r1, r8, r8
	divq r13, r2, r13
	divq r5, r9, r13
	divq r7, r5, r1		
	divq r11, r12, r2	
	divq r4, r3, r3	
	divq r15, r3, r1
	divq r4, r9, r4
	divq r2, r2, r8
	divq r6, r1, r6
	divq r1, r12, r4
	divq r10, r11, r11
	




#----------------------Testing the DIVQU instruction-----

	mov 0xba234560, r1
	mov 0x24900fff, r2
	mov 0x000c1300, r3
	mov 0xff122000, r4
	mov 0xcccccc12, r5
	mov 0x01900129, r6
	mov 0x11248a00, r7
	mov 0xcf2e3d8c, r8
	mov 0xffffffff, r9
	mov 0x80001000, r10

	divqu r1, r8, r8
	divqu r13, r2, r13
	divqu r5, r9, r13
	divqu r7, r5, r1		
	divqu r11, r12, r2	
	divqu r4, r3, r3	
	divqu r15, r3, r1
	divqu r4, r9, r4
	divqu r2, r2, r8
	divqu r15, r6, r6
	divqu r1, r12, r4


Lbl: br Lbl
