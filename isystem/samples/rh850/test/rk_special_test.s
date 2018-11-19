.include "RH850G3M_insts.s"
.include "gpr_init.s"

#------------------------------------------------

#	ldsr r0, psw 	# load r0 to PSW register
#	stsr psw, r2	# load PSW to r2


#--------------------- Tests for CAXI ---------

	mov 0x5, r1
	mov 0x5, r2	
	mov 0xa, r3
	mov 0xc, r4

	mov 0xfee0006c, r5
	mov 0xfee00080, r6
	mov 0xfee00084, r7
	mov 0xfee00088, r8

	ST_H R1, 0x0, R5
	ST_H R2, 0x0, R6
	ST_H R3, 0x0, R7
	ST_H R4, 0x0, R8

	
	caxi [r5], r2, r10
	caxi [r6], r1, r11
	caxi [r7], r4, r12
	caxi [r8], r3, r13
	
	

	
	LD_H 0x0, R5, R10

	ldsr r5, ctbp
	

	ST_H R0, 0x0, R5



	
#--------------------- Tests for CTRET ---------

	# preparing values for CTPC register
	
	mov hilo(ctretBr1), r5
	mov hilo(ctretBr2), r6
	mov hilo(ctretBr3), r7
	mov hilo(ctretBr4), r8

	# preparing values for CTPSW register
	
	mov 0xd, r9
	mov 0x1e, r10
	mov 0x1, r11
	mov 0x9, r12
	
	ldsr r5, ctpc		# loading ctretBr1 addr to ctpc
	ldsr r9, ctpsw		# loading 0xd to ctpsw
	ctret

ctReturn1:
	ldsr r6, ctpc		# loading ctretBr2 addr to ctpc
	ldsr r10, ctpsw		# loading 0x1e to ctpsw
	ctret

ctReturn2:
	ldsr r7, ctpc		# loading ctretBr3 addr to ctpc
	ldsr r11, ctpsw		# loading 0x1 to ctpsw
	ctret

ctReturn3:
	ldsr r8, ctpc		# loading ctretBr4 addr to ctpc
	ldsr r12, ctpsw		# loading 0x9 to ctpsw
	ctret

ctReturn4:
	mov hilo(label1), r10	# continue to next test

	ldsr r10, ctpc
	ldsr r0, ctpsw
	ctret
	


	

ctretBr1:
	mov 0xffffffff, r20
	br ctReturn1

ctretBr2:
	mov 0xffffffff, r21
	br ctReturn2
ctretBr3:
	mov 0xffffffff, r22
	br ctReturn3
ctretBr4:
	mov 0xffffffff, r23
	br ctReturn4

#------------------------------------------------


label1: 

	nop
	ei			# quick test for EI instruction
	stsr psw, r1	
	nop
	di			# quick test for DI instruction
	stsr psw, r1	


#--------------------- Tests for EIRET ---------
	

	# preparing values for EIPC register
	
	mov hilo(eiretBr1), r5
	mov hilo(eiretBr2), r6
	mov hilo(eiretBr3), r7
	mov hilo(eiretBr4), r8

	# preparing values for EIPSW register
	
	mov 0x1d, r9
	mov 0x8, r10
	mov 0x5, r11
	mov 0x11, r12

	ldsr r5, eipc		# loading eiretBr1 addr to eipc
	ldsr r9, eipsw		# loading 0x1d to eipsw
	eiret

eiReturn1:
	ldsr r6, eipc		# loading eiretBr2 addr to eipc
	ldsr r10, eipsw		# loading 0x8 to eipsw
	eiret

eiReturn2:
	ldsr r7, eipc		# loading eiretBr3 addr to eipc
	ldsr r11, eipsw		# loading 0x5 to eipsw
	eiret

eiReturn3:
	ldsr r8, eipc		# loading eiretBr4 addr to eipc
	ldsr r12, eipsw		# loading 0x11 to eipsw
	eiret

eiReturn4:
	mov hilo(label2), r10	# continue to next test

	ldsr r10, eipc
	ldsr r0, eipsw
	eiret



eiretBr1:
	mov 0xffffffff, r24
	br eiReturn1
eiretBr2:
	mov 0xffffffff, r25
	br eiReturn2
eiretBr3:
	mov 0xffffffff, r26
	br eiReturn3
eiretBr4:
	mov 0xffffffff, r27
	br eiReturn4


label2:

#--------------------- Tests for FERET ---------
	

	# preparing values for FEPC register
	
	mov hilo(feretBr1), r5
	mov hilo(feretBr2), r6
	mov hilo(feretBr3), r7
	mov hilo(feretBr4), r8

	# preparing values for FEPSW register
	
	mov 0x1d, r9
	mov 0x8, r10
	mov 0x5, r11
	mov 0x11, r12

	ldsr r5, fepc		# loading feretBr1 addr to fepc
	ldsr r9, fepsw		# loading 0x1d to fepsw
	feret

feReturn1:
	ldsr r6, fepc		# loading feretBr2 addr to fepc
	ldsr r10, fepsw		# loading 0x8 to fepsw
	feret

feReturn2:
	ldsr r7, fepc		# loading feretBr3 addr to fepc
	ldsr r11, fepsw		# loading 0x5 to fepsw
	feret

feReturn3:
	ldsr r8, fepc		# loading eiretBr4 addr to fepc
	ldsr r12, fepsw		# loading 0x11 to fepsw
	feret

feReturn4:
	mov hilo(label3), r10	# continue to next test

	ldsr r10, fepc
	ldsr r0, fepsw
	feret



feretBr1:
	mov 0xffffffff, r24
	br feReturn1
feretBr2:
	mov 0xffffffff, r25
	br feReturn2
feretBr3:
	mov 0xffffffff, r26
	br feReturn3
feretBr4:
	mov 0xffffffff, r27
	br feReturn4


label3:


Lbl: br Lbl
	



