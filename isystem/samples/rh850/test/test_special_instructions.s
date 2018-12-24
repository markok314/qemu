.include "RH850G3M_insts.s"
.include "gpr_init.s"

	
#----------------------Testing the CAXI instruction-----
	
	#fixed store memory address	
	mov 0xfee000a0, r10
	mov 0xfee000a4, r11
	mov 0xfee000a8, r12
	mov 0xfee000ac, r13
	
	#data 
	mov 0x0, r1
	mov 0x80000005, r2
	mov 1, r3
	mov 0xffffffff, r4

	#store 
	st.w r1, 0x0[r10]
	st.w r2, 0x0[r11]
	st.w r3, 0x0[r12]
	st.w r4, 0x0[r13]

	#SIMPLE TEST FOR CAXI

	mov 10,r15
	mov 12,r25
	caxi [r10],r15,r25
	
	#check what is on adr
	ld.w 0x0[r10], r5

	#TESTING S FLAG 

	mov 0xf,r15
	mov 15,r25
	caxi [r13],r15,r25

	#check what is on adr
	ld.w 0x0[r13], r5

	#TESTING Z FLAG

	mov 0x0,r15
	mov 19,r25
	caxi [r10],r15,r25

	#check what is on adr
	ld.w 0x0[r10], r5
	
	#TESTING OV FLAG
	
	mov 0x80000000,r15
	mov 22,r25
	caxi [r12],r15,r25

	#check what is on adr
	ld.w 0x0[r12], r5	

#----------------------Testing the EI and DI instruction-----

	#those two just put 1 or 0 to PSW.ID
	stsr PSW,r1
	
	#PSW.ID is 6b it of PSW register
	# because we check only first 5, we check it with storing 
	# whole psw reg to gpr 
	
	ei
	stsr PSW,r1

	di
	stsr PSW,r1

	ei
	stsr PSW,r1

#----------------------Testing the FETRAP instruction--------
	
	fetrap  0x01
	STSR PSW, r5
	fetrap 	0x02
	STSR PSW, r6
	fetrap 	0x03
	STSR PSW, r7
	fetrap 	0x04
	STSR PSW, r8
	fetrap 	0x05
	STSR PSW, r9
	fetrap 	0x06
	STSR PSW, r10
	fetrap 	0x07
	STSR PSW, r11
	fetrap 	0x08
	STSR PSW, r12
	fetrap 	0x09
	STSR PSW, r13
	fetrap 	0x0a
	STSR PSW, r14
	fetrap 	0x0b
	STSR PSW, r15
	fetrap 	0x0c
	STSR PSW, r16
	fetrap 	0x0d
	STSR PSW, r17
	fetrap 	0x0e
	STSR PSW, r18
	fetrap 	0x0f
	STSR PSW, r19

	#we need to check  FEPC, FEPSW,FEIC and whole PSW reg
	#we return from trap using FERET, just check whole PSW reg then!
	#FEPC SR2, 0
	#FEPSW SR3, 0
	#FEIC SR14, 0

	#trap jumps to RBASE + offset (0x30) if PSW.EBV = 0
	# or to EBASE + offset(0x30) if PSW.EBV = 1
	# SR3, 1 EBASE
	# SR2, 1 RBASE


	#example for one checking adn returning back
	#TODO: check to which address blubox jump
	.data 0x2000
	return1:	
		STSR 2, r1
		STSR 3, r2
		STSR 14, r3
		STSR PSW, r4
		feret
		feret
	
	
Lbl:	br Lbl
