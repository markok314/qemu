.include "RH850G3M_insts.s"
.include "gpr_init.s"

	#sub max negative number 1 and adf 1
	#checking if this set some flags or not

        


	mov 0x7fffffff, r17
	mov 0x1, r18
	add r17, r18

	#testing for v = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0xffffffff,r1
	mov 0x7fffffff,r2
	sbf v,r1,r2,r3
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x80000000,r1
	mov 0xffffffff,r2
	adf nv,r1,r2,r4

	#testing for v = 0
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x80000000,r1
	mov 0xffffffff,r2
	adf v,r1,r2,r5
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x80000000,r1
	mov 0xffffffff,r2
	adf nv,r1,r2,r6

    mov 5, r1
    mov 0xffffffff, r2
    mov 6, r4
    mov 8, r5
    sub r5,r4
    sbf lt,r1,r2,r9

    mov 0x7ffffffe, r1
    mov 0x1, r2
    sbf nz,r1,r2,r9
    sbf z,r1,r2,r9

    mov 2, r1
    mov 3, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 3, r1
    mov 3, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 4, r1
    mov 3, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0, r1
    mov 3, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 1, r1
    mov 3, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0x7fffffff, r1   # OV
    mov 0xfffffffe, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0x7fffffff, r1   # OV
    mov 0xffffffff, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0x7ffffffe, r1   # OV
    mov 0xffffffff, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0x7fffffff, r1   # OV
    mov 0x0, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0xffffffff, r1   # OV
    mov 0xffffffff, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0x80000000, r1   # OV
    mov 0x0, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0x80000000, r1   # OV
    mov 0x80000000, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    mov 0x80000000, r1   # OV
    mov 0xffffffff, r2
    add r1, r5    # z = 0
    sbf nz,r1,r2,r9
    add r1, r5    # z = 0
    sbf z,r1,r2,r9

    # test all corner cases:
    # R1, R2
    # ------
    # 0, 0
    # 1, 0
    # 0, 1
    # 0, 2
    # 2, 0
    # 0, 7f
    # 7f, 0
    # 0, ff
    # ff, 0
    # 0, 80
    # 80, 0
    mov 1, r3
    mov 1, r4

    mov 0, r1
    mov 0, r2
    jarl testSBF, r6

    mov 0, r1
    mov 1, r2
    jarl testSBF, r6

    mov 1, r1
    mov 0, r2
    jarl testSBF, r6

    mov 0, r1
    mov 2, r2
    jarl testSBF, r6

    mov 2, r1
    mov 0, r2
    jarl testSBF, r6

    mov 0, r1
    mov 0x7fffffff, r2
    jarl testSBF, r6

    mov 0x7fffffff, r1
    mov 0, r2
    jarl testSBF, r6

    mov 0, r1
    mov 0xffffffff, r2
    jarl testSBF, r6

    mov 0xffffffff, r1
    mov 0, r2
    jarl testSBF, r6

    mov 0, r1
    mov 0x80000000, r2
    jarl testSBF, r6

    mov 0x80000000, r1
    mov 0, r2
    jarl testSBF, r6


    # 1
    mov 1, r1
    mov 1, r2
    jarl testSBF, r6

    mov 1, r1
    mov 2, r2
    jarl testSBF, r6

    mov 2, r1
    mov 1, r2
    jarl testSBF, r6

    mov 1, r1
    mov 0x7fffffff, r2
    jarl testSBF, r6

    mov 0x7fffffff, r1
    mov 1, r2
    jarl testSBF, r6

    mov 1, r1
    mov 0xffffffff, r2
    jarl testSBF, r6

    mov 0xffffffff, r1
    mov 1, r2
    jarl testSBF, r6

    mov 1, r1
    mov 0x80000000, r2
    jarl testSBF, r6

    mov 0x80000000, r1
    mov 1, r2
    jarl testSBF, r6

    # 2
    mov 2, r1
    mov 2, r2
    jarl testSBF, r6

    mov 2, r1
    mov 0x7fffffff, r2
    jarl testSBF, r6

    mov 0x7fffffff, r1
    mov 2, r2
    jarl testSBF, r6

    mov 2, r1
    mov 0xffffffff, r2
    jarl testSBF, r6

    mov 0xffffffff, r1
    mov 2, r2
    jarl testSBF, r6

    mov 2, r1
    mov 0x80000000, r2
    jarl testSBF, r6

    mov 0x80000000, r1
    mov 2, r2
    jarl testSBF, r6

    # 0x7fffffff
    mov 0x7fffffff, r1
    mov 0x7fffffff, r2
    jarl testSBF, r6

    mov 0x7fffffff, r1
    mov 0xffffffff, r2
    jarl testSBF, r6

    mov 0xffffffff, r1
    mov 0x7fffffff, r2
    jarl testSBF, r6

    mov 0x7fffffff, r1
    mov 0x80000000, r2
    jarl testSBF, r6

    mov 0x80000000, r1
    mov 0x7fffffff, r2
    jarl testSBF, r6


    # 0xffffffff
    mov 0xffffffff, r1
    mov 0xffffffff, r2
    jarl testSBF, r6

    mov 0xffffffff, r1
    mov 0x80000000, r2
    jarl testSBF, r6

    mov 0x80000000, r1
    mov 0xffffffff, r2
    jarl testSBF, r6

    # 0x80000000
    mov 0x80000000, r1
    mov 0x80000000, r2
    jarl testSBF, r6

    br  .

testSBF:
    add r3, r4
    sbf z, r1, r2, r5
    add r3, r4
    sbf nz, r1, r2, r5
    jmp [r6]

    
