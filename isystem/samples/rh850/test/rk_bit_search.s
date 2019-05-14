.include "RH850G3M_insts.s"

.include "gpr_init.s"



	mov 0xffff1234, r1
	mov 0x12005678, r2
	mov 0xff11ff00, r3
	mov 0xadac0000, r4
	mov 0x1f2e3d8c, r5
	mov 0xf3450112, r6
	mov 0xffffffff, r7
	mov 0xfffffffe, r9
	

#----------------------Testing the SCH0L instruction-----

	sch0l r1, r8		# expecting 0x11
	sch0l r2, r8		# expecting 0x1
	sch0l r3, r8		# expecting 0x9
	sch0l r4, r8		# expecting 0x2
	sch0l r5, r8		# expecting 0x1
	sch0l r6, r8		# expecting 0x5
	sch0l r7, r8		# expecting 0x0 and Z flag set
	sch0l r9, r8		# expecting 0x20 and CY flag set
	


#----------------------Testing the SCH1L instruction-----

	mov 0xffff1234, r1
	mov 0x12005678, r2
	mov 0x0001ff00, r3
	mov 0x000000f0, r4
	mov 0x0f2e3d8c, r5
	mov 0x00000002, r6
	mov 0x1, r10

	sch1l r1, r8		# expecting 0x1
	sch1l r2, r8		# expecting 0x4
	sch1l r3, r8		# expecting 0x10
	sch1l r4, r8		# expecting 0x19
	sch1l r5, r8		# expecting 0x5
	sch1l r6, r8		# expecting 0x1f
	sch1l r9, r8		# expecting 0x0 and Z flag set
	sch1l r10, r8		# expecting 0x20 and CY flag set


#----------------------Testing the SCH0R instruction-----

	mov 0xffff123f, r1
	mov 0x12005679, r2
	mov 0x0001ff00, r3
	mov 0x0000ffff, r4
	mov 0x0f1fffff, r5
	mov 0x00000003, r6
	mov 0xffffffff, r7
	mov 0x7fffffff, r9

	sch0r r1, r8		# expecting 0x7
	sch0r r2, r8		# expecting 0x2
	sch0r r3, r8		# expecting 0x1
	sch0r r4, r8		# expecting 0x11
	sch0r r5, r8		# expecting 0x16
	sch0r r6, r8		# expecting 0x3
	sch0r r7, r8		# expecting 0x0 and Z flag set
	sch0r r9, r8		# expecting 0x20 and CY flag set


#----------------------Testing the SCH1R instruction-----

	mov 0xffff123f, r1
	mov 0x12005000, r2
	mov 0x00010000, r3
	mov 0x80000000, r4
	mov 0x0f1ffd00, r5
	mov 0x00000000, r6

	sch1r r1, r8		# expecting 0x1
	sch1r r2, r8		# expecting 0xd
	sch1r r3, r8		# expecting 0x11
	sch1r r4, r8		# expecting 0x20 and CY flag set
	sch1r r5, r8		# expecting 0x9
	sch1r r6, r8		# expecting 0x0 and Z flag set



Lbl: br Lbl
