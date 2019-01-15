.include "RH850G3M_insts.s"
.include "gpr_init.s"

#--------tests for SASF-----------------------------

	#simple tests for sasf
	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19

-------#Testing V and NV condition for sasf---------

	#OV flag is not set here

	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19
	
	#OV flag is set here

	mov 0x7, r2
	mov 0x80000000, r6
	sub r2, r6
	
	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19
	
	ldsr r0, psw
	#clearing all flags
	
-------#Testing C/L and NC/NL condition for sasf----

	#CY flag is not set here

	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19

	#CY flag is set here
	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1
	
	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1
	
	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19

	ldsr r0, psw
	#clearing all flags
	
-------#Testing  Z and NZ condition for sasf----

	#Z flag is not set here

	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19

	#Z flag is set here
	addi 0x0, r0, r1
	
	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19

	ldsr r0, psw
	#clearing all flags

-------#Testing  NH and NH condition for sasf----
	#CHECKING COMBINATIONS (CY or Z) = 1
	#CHECKING Z FLAG

	#Z flag is not set here

	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19
	
	#Z flag is set here
	addi 0x0, r0, r1

	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19
	
	ldsr r0, psw
	#clearing all flags

	#CHECKING COMBINATIONS (CY or Z) = 1
	#CHECKING CY FLAG
	
	#CY flag is not set here

	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19

	#CY flag is set here
	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1

	mov 0x1,r10
	mov 0x2,r11
	mov 0x3,r12
	mov 0x1,r13
	mov 0x2,r14
	mov 0x3,r15
	mov 0x1,r16
	mov 0x2,r17
	mov 0x3,r18
	mov 0x1,r19
	
	sasf V,r10
	sasf NV,r11
	sasf C,r12
	sasf L,r13
	sasf NC,r14
	sasf NL,r15
	sasf Z,r16
	sasf NZ,r17
	sasf NH,r18
	sasf H,r19
	

#--------tests for SETF-----------------------------

	#simple tests for setf

	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19

-------#Testing V and NV condition for setf---------

	#OV flag is not set here

	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19
	
	#OV flag is set here

	mov 0x7, r2
	mov 0x80000000, r6
	sub r2, r6
	
	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19
	
	ldsr r0, psw
	#clearing all flags
	
-------#Testing C/L and NC/NL condition for setf----

	#CY flag is not set here

	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19

	#CY flag is set here
	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1
	
	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19

	ldsr r0, psw
	#clearing all flags
	
-------#Testing  Z and NZ condition for setf----

	#Z flag is not set here

	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19

	#Z flag is set here
	addi 0x0, r0, r1
	
	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19

	ldsr r0, psw
	#clearing all flags

-------#Testing  NH and NH condition for setf----
	#CHECKING COMBINATIONS (CY or Z) = 1
	#CHECKING Z FLAG

	#Z flag is not set here
	
	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19
	
	#Z flag is set here
	addi 0x0, r0, r1
	
	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19
	
	ldsr r0, psw
	#clearing all flags

	#CHECKING COMBINATIONS (CY or Z) = 1
	#CHECKING CY FLAG
	
	#CY flag is not set here
	
	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19

	#CY flag is set here
	mov 0x1, r1
	mov 0x7, r2
	cmp r2, r1
	
	setf V,r10
	setf NV,r11
	setf C,r12
	setf L,r13
	setf NC,r14
	setf NL,r15
	setf Z,r16
	setf NZ,r17
	setf NH,r18
	setf H,r19
Lbl: br Lbl
