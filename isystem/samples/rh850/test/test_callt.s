.include "RH850G3M_insts.s"
.include "gpr_init.s"

	
#----------------------Testing the CALLT instruction-----
start:
	mov 0xff,r10
	ldsr r10,PSW

	mov  hilo(callTable),r11
	ldsr r11, SR_CTBP
	
	CALLT 1
    
    mov  0x12345, r1

Lbl:	br Lbl

callTable:
    .hword 0xffff
    .hword subrutine - callTable
    .hword 0xe2b3

subrutine:	
		mov 0x0,r10
		ldsr r10,PSW   # modify psw reg to test restore

		CTRET          

    # should never get here
    br  start

