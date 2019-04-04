.include "RH850G3M_insts.s"
.include "gpr_init.s"

	
#----------------------Testing the CALLT instruction-----

	# check value of CTBP on bluebox, to generate valid address
	# DONE CTBP can be modified with ldsr command
start:
	mov 0xff,r10
	ldsr r10,PSW
	#modify psw reg to 0xff	

	mov  hilo(callTable),r11
	ldsr r11,20
	#20 is CTBP
	
	CALLT 1
    
    mov  0x12345, r1
    br Lbl
callTable:
    .hword 0xffff
    .hword subrutine - callTable
    .hword 0xe2b3

	subrutine:	

		#check registers with stsr regID,reg,sellID
		stsr CTPC,r1
		stsr CTPSW,r2
		#here we should get only PSW(4:0) TEST THAT

		mov 0x1111,r10
		ldsr r10,PSW
		#modify psw reg to 0xff	

		CTRET
		#with that we should return to back after CALLT

    # should never get here
    br  start

Lbl:	br Lbl
