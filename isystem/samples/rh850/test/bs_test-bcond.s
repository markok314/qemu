.include "RH850G3M_insts.s"
.include "gpr_init.s"

#----------------BGE----(S xor OV) = 0--------------------OK

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
		nop

#----------------BGT----((S xor OV) or Z) = 0--------------------OK

        mov 5, r2
        mov 6, r3
        add r2, r3
        bgt lab5	# S=0, OV=0: branch
		nop
lab4:	mov 3, r4
		mov -4, r5
		add r5, r4
		bgt lab4	# S=1, OV=0: no branch
		br lab6
		nop
lab5:	mov 0x70000000, r4
		mov 0x60000000, r5
		add r4, r5
		bgt lab4	# S=1, OV=1: branch
		nop
		nop
lab6:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
        bgt lab5	# S=0, OV=1: no branch
        nop
        mov 1, r1
        mov 5, r2
        mov -5, r3
        add r2, r3
		bgt lab6	# Z=1: no branch
        mov 4, r3
        add r2, r3
		bgt lab7	# Z=0: branch
		nop
		nop
lab7:	mov 1, r4
		nop

#----------------BLE----((S xor OV) or Z) = 1--------------------OK

        mov 5, r2
        mov -6, r3
        add r2, r3
        ble lab9	# S=1, OV=0: branch
		nop
lab8:	mov 3, r4
		mov 4, r5
		add r5, r4
		ble lab8	# S=0, OV=0: no branch
		br lab10
		nop
lab9:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
		ble lab8	# S=0, OV=1: branch
		nop
		nop
lab10:	mov 0x70000000, r4
		mov 0x60000000, r5
		add r4, r5
        ble lab9	# S=1, OV=1: no branch
        nop
        mov 1, r1
        mov 5, r2
        mov 6, r3
        add r2, r3
		ble lab10	# Z=0: no branch
        mov -5, r3
        add r2, r3
		ble lab11	# Z=1: branch
		nop
		nop
lab11:	mov 1, r4
		nop

#----------------BLT----(S xor OV) = 1--------------------OK

        mov 5, r2
        mov -8, r3
        add r2, r3
        blt lab13	# S=1, OV=0: branch
		nop
lab12:	mov 3, r4
		mov 4, r5
		add r5, r4
		blt lab12	# S=0, OV=0: no branch
		br lab14
		nop
lab13:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
		blt lab12	# S=0, OV=1: branch
		nop
		nop
lab14:	mov 0x70000000, r4
		mov 0x60000000, r5
		add r4, r5
        blt lab13	# S=1, OV=1: no branch
        nop
        mov 1, r1
		nop

#----------------BH----(CY or Z) = 0--------------------OK

        mov 5, r2
        mov -8, r3
        add r2, r3
        bh lab15			# CY=0, Z=0 -> branch
		nop
		nop
lab15:	mov 3, r4
		mov -3, r5
		add r5, r4
		bh lab15			# CY=0, Z=1 -> no branch
lab16:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
		bh lab15			# CY=1, Z=0 -> no branch
lab17:	mov 0x90000000, r4
		mov 0x70000000, r5
		add r4, r5
        bh lab16			# CY=1, Z=1 -> no branch
        mov 1, r1
		nop

#----------------BL----CY = 1--------------------OK
# Note: BL is treated as BC (also has CY = 1)

        mov 5, r2
        mov 8, r3
        add r2, r3
        bl lab17			# CY=0 -> no branch
        mov 0xb0000000, r5
lab18:	mov 0x90000000, r4
		add r4, r5
		bl lab18			# CY=1 -> branch, CY=0 -> no branch
		nop

#----------------BNH----(CY or Z) = 1--------------------OK

        mov 0xa0000000, r2
        mov 0x60000000, r3
        add r2, r3
        bnh lab20			# CY=1, Z=1 -> branch
lab19:	mov 0, r4
		mov 0, r5
		add r5, r4
		bnh lab21			# CY=0, Z=1 -> branch
lab20:	mov 0x80abcdef, r4
		mov 0x8000abcd, r5
		add r4, r5
		bnh lab19			# CY=1, Z=0 -> branch
lab21:	mov 0x90000, r4
		mov 0x700, r5
		add r4, r5
        bnh lab20			# CY=0, Z=0 -> no branch
        nop

#----------------BNL----CY = 0--------------------OK
# Treated as BNC

        mov 0xb0000000, r5
        mov 0x90000000, r4
        add r4, r5
        bnl lab21			# CY=1 -> no branch
        mov 0x90000000, r3
lab22:	mov 0x40000000, r2
		add r2, r3
		bnl lab22		# CY=0 -> branch, CY=1 -> no branch
		nop

#----------------BE----Z = 1--------------------OK
# Treated as BZ

        mov 1234, r5
        mov -123, r4
        add r4, r5
        be lab22			# Z=0 -> no branch
        mov 2345, r3
lab23:	mov -2345, r2
		add r2, r3
		be lab23		# Z=1 -> branch, Z=0 -> no branch
		nop

#----------------BNE----Z = 0--------------------OK

        mov 1234, r5
        mov -1234, r4
        add r4, r5
        bne lab23			# Z=1 -> no branch
        mov 1000, r3
lab24:	mov -500, r2
		add r2, r3
		bne lab24			# Z=0 -> branch, Z=1 -> no branch
		nop

#----------------BC = BL

#----------------BF = BNE

#----------------BN----S = 1--------------------OK

        mov 1234, r5
        mov -234, r4
        add r4, r5
        bn lab24			# S=0 -> no branch
        mov -1000, r3
lab25:	mov 700, r2
		add r2, r3
		bn lab25		# S=1 -> branch, S=0 -> no branch

#----------------BNC = BNL

#----------------BNV----OV = 0--------------------OK

        mov 0x70000000, r5
        mov 0x20000000, r4
        add r4, r5
        bnv lab25			# OV=1 -> no branch
        mov 0x50000000, r3
lab26:	mov 0x20000000, r2
		add r2, r3
		bnv lab26		# OV=0 -> branch, OV=1 -> no branch
		nop

#----------------BNZ = BF = BNE

#----------------BP----S = 0--------------------OK

        mov -1234, r5
        mov 234, r4
        add r4, r5
        bp lab26			# S=1 -> no branch
        mov 1000, r3
lab27:	mov -700, r2
		add r2, r3
		bp lab27		# S=0 -> branch, S=1 -> no branch
		nop

#----------------BR-----always-------------------OK

        mov -1234, r5
        mov 234, r4
        add r4, r5
        br lab29
        mov 1000, r3
lab28:	mov -700, r2
		add r2, r3
		br lab30
lab29:	br lab28
lab30:	mov 1, r1
		nop

#----------------BSA-----SAT = 1-------------------OK

        mov 0x30000000, r5
        mov 0x30000000, r4
        satadd r4, r5
        bsa lab31			# SAT=0 -> no branch
        mov 0x40000000, r3
		mov 0x50000000, r2
		satadd r2, r3
		bsa lab31			# SAT=1 -> branch
		nop
        mov 0x1234, r11
lab31:	nop

#----------------BT = BE = BZ

#----------------BV-----OV = 1-------------------OK

        mov 0x40000000, r5
        mov 0x20000000, r4
        add r4, r5
        bv lab32			# OV=0 -> no branch
        mov 0x60000000, r3
		mov 0x40000000, r2
		add r2, r3
		bv lab32			# OV=1 -> branch
        mov 0x1234, r11
		nop
lab32:	nop
		nop

#----------------BZ = BT

Lbl:	br Lbl

# Note: Bcond(2) with disp17 has not been tested

