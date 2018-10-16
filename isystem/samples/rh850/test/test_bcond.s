.text
.include "gpr_init.s"
	
#SAT/CARRY/OVERFLOW/SIGNED/ZERO

#--------------------------Testing BGE instruction----

	ldsr r0, PSW
	bge 0x2
	mov 0x1, r1
	
	#set on OV
	mov 0x4,r2
	ldsr r2, PSW
	bge 0x2
	mov 0x2, r1
	
#--------------------------Testing BGT instruction----

	ldsr r0, PSW
	bgt 0x2
	mov 0x3, r1
	
	#set on Z
	mov 0x1,r2
	ldsr r2, PSW
	bgt 0x2
	mov 0x4, r1
	
#--------------------------Testing BLE instruction----
	
	ldsr r0, PSW
	ble 0x2
	mov 0x5, r1
	
	#set on S
	mov 0x2,r2
	ldsr r2, PSW
	ble 0x2
	mov 0x6, r1

#--------------------------Testing BLT instruction----

	ldsr r0, PSW
	blt 0x2
	mov 0x7, r1
	
	#set on S
	mov 0x2,r2
	ldsr r2, PSW
	blt 0x2
	mov 0x8, r1

#--------------------------Testing BH instruction----

	ldsr r0, PSW
	bh 0x2
	mov 0x9, r1
	
	#set on CY
	mov 0x8,r2
	ldsr r2, PSW
	bh 0x2
	mov 0xa, r1

#--------------------------Testing BL instruction----
	
	ldsr r0, PSW
	bl 0x2
	mov 0xb, r1
	
	#set on CY
	mov 0x8,r2
	ldsr r2, PSW
	bl 0x2
	mov 0xc, r1

#--------------------------Testing BNH instruction----

	ldsr r0, PSW
	bnh 0x2
	mov 0xd, r1
	
	#set on CY
	mov 0x8,r2
	ldsr r2, PSW
	bnh 0x2
	mov 0xe, r1

#--------------------------Testing BNL instruction----
	
	ldsr r0, PSW
	bnl 0x2
	mov 0x10, r1
	
	#set on CY
	mov 0x8,r2
	ldsr r2, PSW
	bnl 0x2
	mov 0x11, r1
	
#--------------------------Testing BE instruction----
	
	ldsr r0, PSW
	be 0x2
	mov 0x12, r1
	
	#set on Z
	mov 0x1,r2
	ldsr r2, PSW
	be 0x2
	mov 0x13, r1
	
#--------------------------Testing BNE instruction----
	
	ldsr r0, PSW
	bne 0x2
	mov 0x14, r1
	
	#set on Z
	mov 0x1,r2
	ldsr r2, PSW
	bne 0x2
	mov 0x15, r1
	
#--------------------------Testing BC instruction----
	
	ldsr r0, PSW
	bc 0x2
	mov 0x16, r1
	
	#set on CY
	mov 0x8,r2
	ldsr r2, PSW
	bc 0x2
	mov 0x17, r1

#--------------------------Testing BF instruction----

	ldsr r0, PSW
	bf 0x2
	mov 0x18, r1
	
	#set on Z
	mov 0x1,r2
	ldsr r2, PSW
	bf 0x2
	mov 0x19, r1
	
#--------------------------Testing BN instruction----

	ldsr r0, PSW
	bn 0x2
	mov 0x1a, r1
	
	#set on S
	mov 0x2,r2
	ldsr r2, PSW
	bn 0x2
	mov 0x1b, r1

#--------------------------Testing BNC instruction----
	
	ldsr r0, PSW
	bnc 0x2
	mov 0x1c, r1
	
	#set on CY
	mov 0x8,r2
	ldsr r2, PSW
	bnc 0x2
	mov 0x1d, r1
	
#--------------------------Testing BNV instruction----
	
	ldsr r0, PSW
	bnv 0x2
	mov 0x1e, r1
	
	#set on OV
	mov 0x4,r2
	ldsr r2, PSW
	bnv 0x2
	mov 0x1f, r1	

#--------------------------Testing BNZ instruction----
	
	ldsr r0, PSW
	bnz 0x2
	mov 0x20, r1
	
	#set on Z
	mov 0x1,r2
	ldsr r2, PSW
	bnz 0x2
	mov 0x21, r1	

#--------------------------Testing BP instruction----
	
	ldsr r0, PSW
	bp 0x2
	mov 0x22, r1
	
	#set on S
	mov 0x2,r2
	ldsr r2, PSW
	bp 0x2
	mov 0x23, r1		
	
#--------------------------Testing BR instruction----
	
	ldsr r0, PSW
	br 0x2
	mov 0x24, r1
	
#--------------------------Testing BSA instruction----

	ldsr r0, PSW
	bsa 0x2
	mov 0x25, r1
	
	#set on SAT 
	mov 0x1f,r2
	ldsr r2, PSW
	bsa 0x2
	mov 0x26, r1	

#--------------------------Testing BT instruction----

	ldsr r0, PSW
	bt 0x2
	mov 0x27, r1
	
	#set on Z 
	mov 0x1,r2
	ldsr r2, PSW
	bt 0x2
	mov 0x28, r1

#--------------------------Testing BV instruction----
	
	ldsr r0, PSW
	bv 0x2
	mov 0x29, r1
	
	#set on OV
	mov 0x4,r2
	ldsr r2, PSW
	bv 0x2
	mov 0x2a, r1

#--------------------------Testing BZ instruction----

	ldsr r0, PSW
	bz 0x2
	mov 0x2b, r1
	
	#set on Z
	mov 0x1,r2
	ldsr r2, PSW
	bz 0x2
	mov 0x2c, r1
