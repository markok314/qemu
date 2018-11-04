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
	ei
	di
	ei

#----------------------Testing the FETRAP instruction--------
	
	fetrap  0x01
	fetrap 	0x02
	fetrap 	0x03
	fetrap 	0x04
	fetrap 	0x05
	fetrap 	0x06
	fetrap 	0x07
	fetrap 	0x08
	fetrap 	0x09
	fetrap 	0x0a
	fetrap 	0x0b
	fetrap 	0x0c
	fetrap 	0x0d
	fetrap 	0x0e
	fetrap 	0x0f

#----------------------Testing the DISPOSE instruction--------
	
Lbl:	br Lbl
