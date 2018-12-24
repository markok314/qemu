.include "RH850G3M_insts.s"
.include "gpr_init.s"

	
#----------------------Testing the CALLT instruction-----

	# check value of CTBP on bluebox, to generate valid address
	# DONE CTBP can be modified with ldsr command

	mov 0xff,r10
	ldsr r10,PSW
	#modify psw reg to 0xff	

	mov 0x0,r11
	ldsr r11,20
	#20 is CTBP
	#we can modify CTBP register
	
	CALLT subrutine
	#test_callt.o:(.text+0x4e): warning: could not locate special linker symbol __ctbp
	#this outputs ./build.sh

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

Lbl:	br Lbl
