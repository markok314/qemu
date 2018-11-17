.text

#----------------BGE----(S xor OV) = 0--------------------OK
/*
        mov 5, r2
        mov 6, r3
        add r2, r3
        bge lab2	# S=0, OV=0: branch
		nop
lab1:	mov 3, r4
		mov -4, r5
		add r5, r4
		bge lab1	# S=1, OV=0: no branch
		br lab3
		nop
lab2:	mov 0x70000000, r4
		mov 0x60000000, r5
		add r4, r5
		bge lab1	# S=1, OV=1: branch
		nop
		nop
lab3:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
        bge lab2	# S=0, OV=1: no branch
        nop
        mov 1, r1
Lbl:	br Lbl
*/

#----------------BGT----((S xor OV) or Z) = 0--------------------OK
/*
        mov 5, r2
        mov 6, r3
        add r2, r3
        bgt lab2	# S=0, OV=0: branch
		nop
lab1:	mov 3, r4
		mov -4, r5
		add r5, r4
		bgt lab1	# S=1, OV=0: no branch
		br lab3
		nop
lab2:	mov 0x70000000, r4
		mov 0x60000000, r5
		add r4, r5
		bgt lab1	# S=1, OV=1: branch
		nop
		nop
lab3:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
        bgt lab2	# S=0, OV=1: no branch
        nop
        mov 1, r1
        mov 5, r2
        mov -5, r3
        add r2, r3
		bgt lab3	# Z=1: no branch
        mov 4, r3
        add r2, r3
		bgt lab4	# Z=0: branch
		nop
		nop
lab4:	mov 1, r4
Lbl:	br Lbl
*/

#----------------BLE----((S xor OV) or Z) = 1--------------------OK
/*
        mov 5, r2
        mov -6, r3
        add r2, r3
        ble lab2	# S=1, OV=0: branch
		nop
lab1:	mov 3, r4
		mov 4, r5
		add r5, r4
		ble lab1	# S=0, OV=0: no branch
		br lab3
		nop
lab2:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
		ble lab1	# S=0, OV=1: branch
		nop
		nop
lab3:	mov 0x70000000, r4
		mov 0x60000000, r5
		add r4, r5
        ble lab2	# S=1, OV=1: no branch
        nop
        mov 1, r1
        mov 5, r2
        mov 6, r3
        add r2, r3
		ble lab3	# Z=0: no branch
        mov -5, r3
        add r2, r3
		ble lab4	# Z=1: branch
		nop
		nop
lab4:	mov 1, r4
Lbl:	br Lbl
*/

#----------------BLT----(S xor OV) = 1--------------------OK
/*
        mov 5, r2
        mov -8, r3
        add r2, r3
        blt lab2	# S=1, OV=0: branch
		nop
lab1:	mov 3, r4
		mov 4, r5
		add r5, r4
		blt lab1	# S=0, OV=0: no branch
		br lab3
		nop
lab2:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
		blt lab1	# S=0, OV=1: branch
		nop
		nop
lab3:	mov 0x70000000, r4
		mov 0x60000000, r5
		add r4, r5
        blt lab2	# S=1, OV=1: no branch
        nop
        mov 1, r1
Lbl:	br Lbl
*/

#----------------BH----(CY or Z) = 0--------------------OK
/*
        mov 5, r2
        mov -8, r3
        add r2, r3
        BH lab1			# CY=0, Z=0 -> branch
		nop
		nop
lab1:	mov 3, r4
		mov -3, r5
		add r5, r4
		BH lab1			# CY=0, Z=1 -> no branch
lab2:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
		BH lab1			# CY=1, Z=0 -> no branch
lab3:	mov 0x90000000, r4
		mov 0x70000000, r5
		add r4, r5
        BH lab2			# CY=1, Z=1 -> no branch
        mov 1, r1
Lbl:	br Lbl
*/

#----------------BL----CY = 1--------------------OK
# Note: BL is treated as BC (also has CY = 1)
/*
        mov 5, r2
        mov 8, r3
        add r2, r3
        BL Lbl			# CY=0 -> no branch
        mov 0xb0000000, r5
lab1:	mov 0x90000000, r4
		add r4, r5
		BL lab1			# CY=1 -> branch, CY=0 -> no branch
Lbl:	br Lbl
*/

#----------------BNH----(CY or Z) = 1--------------------OK
/*
        mov 0xa0000000, r2
        mov 0x60000000, r3
        add r2, r3
        BNH lab2			# CY=1, Z=1 -> branch
lab1:	mov 0, r4
		mov 0, r5
		add r5, r4
		BNH lab3			# CY=0, Z=1 -> branch
lab2:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
		BNH lab1			# CY=1, Z=0 -> branch
lab3:	mov 0x90000, r4
		mov 0x700, r5
		add r4, r5
        BNH lab2			# CY=0, Z=0 -> no branch
        nop
Lbl:	br Lbl
*/

#----------------BNL----CY = 0--------------------OK
# Treated as BNC
/*
        mov 0xb0000000, r5
        mov 0x90000000, r4
        add r4, r5
        BNL Lbl			# CY=1 -> no branch
        mov 0x90000000, r3
lab1:	mov 0x40000000, r2
		add r2, r3
		BNL lab1		# CY=0 -> branch, CY=1 -> no branch
		nop
Lbl:	br Lbl
*/

#----------------BE----Z = 1--------------------OK
# Treated as BZ
/*
        mov 1234, r5
        mov -123, r4
        add r4, r5
        BE Lbl			# Z=0 -> no branch
        mov 2345, r3
lab1:	mov -2345, r2
		add r2, r3
		BE lab1		# Z=1 -> branch, Z=0 -> no branch
Lbl:	br Lbl
*/

#----------------BNE----Z = 0--------------------OK
/*
        mov 1234, r5
        mov -1234, r4
        add r4, r5
        BNE Lbl			# Z=1 -> no branch
        mov 1000, r3
lab1:	mov -500, r2
		add r2, r3
		BNE lab1			# Z=0 -> branch, Z=1 -> no branch
Lbl:	br Lbl
*/

#----------------BC = BL

#----------------BF = BNE

#----------------BN----S = 1--------------------OK
/*
        mov 1234, r5
        mov -234, r4
        add r4, r5
        BN Lbl			# S=0 -> no branch
        mov -1000, r3
lab1:	mov 700, r2
		add r2, r3
		BN lab1		# S=1 -> branch, S=0 -> no branch
Lbl:	br Lbl
*/

#----------------BNC = BNL

#----------------BNV----OV = 0--------------------OK
/*
        mov 0x70000000, r5
        mov 0x20000000, r4
        add r4, r5
        BNV Lbl			# OV=1 -> no branch
        mov 0x50000000, r3
lab1:	mov 0x20000000, r2
		add r2, r3
		BNV lab1		# OV=0 -> branch, OV=1 -> no branch
Lbl:	br Lbl
*/

#----------------BNZ = BF = BNE

#----------------BP----S = 0--------------------OK
/*
        mov -1234, r5
        mov 234, r4
        add r4, r5
        BP Lbl			# S=1 -> no branch
        mov 1000, r3
lab1:	mov -700, r2
		add r2, r3
		BP lab1		# S=0 -> branch, S=1 -> no branch
Lbl:	br Lbl
*/

#----------------BR-----always-------------------OK
/*
        mov -1234, r5
        mov 234, r4
        add r4, r5
        BR lab2
        mov 1000, r3
lab1:	mov -700, r2
		add r2, r3
		BR lab3
lab2:	BR lab1
lab3:	mov 1, r1
*/

#----------------BSA-----SAT = 1-------------------OK
/*
        mov 0x30000000, r5
        mov 0x30000000, r4
        satadd r4, r5
        BSA lab1			# SAT=0 -> no branch
        mov 0x40000000, r3
lab1:	mov 0x50000000, r2
		satadd r2, r3
		BSA lab1			# SAT=1 -> branch
*/

#----------------BT = BE = BZ

#----------------BV-----OV = 1-------------------OK
/*
        mov 0x40000000, r5
        mov 0x20000000, r4
        add r4, r5
        BV lab1			# OV=0 -> no branch
        mov 0x60000000, r3
lab1:	mov 0x40000000, r2
		add r2, r3
		BV lab1			# OV=1 -> branch
		nop
*/
#----------------BZ = BT


# Note: Bcond(2) with disp17 has not been tested
