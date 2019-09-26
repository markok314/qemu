.include "RH850G3M_insts.s"
.include "gpr_init.s"

	#testing starts here

	#simple tests for adf

	mov 0x5,r9
	mov 0x6,r10
	adf v,r9,r15,r4
	adf nv,r9,r15,r5
	adf c,r9,r10,r6
	adf l,r9,r10,r7
	adf nc,r9,r15,r8
	adf nl,r9,r15,r2
	adf z,r9,r15,r10
	adf nz,r9,r15,r11
	adf nh,r9,r15,r12
	adf h,r9,r15,r13
	adf s,r9,r15,r14
	adf n,r9,r15,r1
	adf ns,r9,r15,r16
	adf p,r9,r15,r17
	adf t,r9,r15,r18
	adf lt,r9,r15,r19
	adf ge,r9,r15,r20
	adf le,r9,r15,r21
	adf gt,r9,r15,r22
	
	mov 0x7fffffff,r1
	mov 0x1,r2
	adf v,r1,r2,r4
	adf nv,r1,r2,r5
	adf c,r1,r2,r6
	adf l,r1,r2,r7
	adf nc,r1,r2,r8
	adf nl,r1,r2,r9
	adf z,r1,r2,r10
	adf nz,r1,r2,r11
	adf nh,r1,r2,r12
	adf h,r1,r2,r13
	adf s,r1,r2,r14
	adf n,r1,r2,r15
	adf ns,r1,r2,r16
	adf p,r1,r2,r17
	adf t,r1,r2,r18
	adf lt,r1,r2,r19
	adf ge,r1,r2,r20
	adf le,r1,r2,r21
	adf gt,r1,r2,r22
	
	mov 0x81111111,r1
	mov 0x0,r2
	adf v,r1,r2,r4
	adf nv,r1,r2,r5
	adf c,r1,r2,r6
	adf l,r1,r2,r7
	adf nc,r1,r2,r8
	adf nl,r1,r2,r9
	adf z,r1,r2,r10
	adf nz,r1,r2,r11
	adf nh,r1,r2,r12
	adf h,r1,r2,r13
	adf s,r1,r2,r14
	adf n,r1,r2,r15
	adf ns,r1,r2,r16
	adf p,r1,r2,r17
	adf t,r1,r2,r18
	adf lt,r1,r2,r19
	adf ge,r1,r2,r20
	adf le,r1,r2,r21
	adf gt,r1,r2,r22
	#testing s flag
	
	mov 0x80000001,r1
	mov 0x80000000,r2
	adf v,r1,r2,r4
	adf nv,r1,r2,r5
	adf c,r1,r2,r6
	adf l,r1,r2,r7
	adf nc,r1,r2,r8
	adf nl,r1,r2,r9
	adf z,r1,r2,r10
	adf nz,r1,r2,r11
	adf nh,r1,r2,r12
	adf h,r1,r2,r13
	adf s,r1,r2,r14
	adf n,r1,r2,r15
	adf ns,r1,r2,r16
	adf p,r1,r2,r17
	adf t,r1,r2,r18
	adf lt,r1,r2,r19
	adf ge,r1,r2,r20
	adf le,r1,r2,r21
	adf gt,r1,r2,r22
	#testing z flag
	
	#testing all conditions

	#checking v and nv condition

	#testing for v = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf v,r1,r2,r3
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf nv,r1,r2,r4

	#testing for v = 0
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf v,r1,r2,r5
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf nv,r1,r2,r6
	
	#checking c/l and nc/nl	
	
	#testing for c/l = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#c should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf c,r1,r2,r7
	
	#testing for c/l = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#c should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf l,r1,r2,r8

	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#c should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf nc,r1,r2,r9

	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#c should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf nl,r1,r2,r10

	#testing for nc/nl = 0
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#c should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf c,r1,r2,r11
	
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#c should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf l,r1,r2,r12
	
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#c should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf nc,r1,r2,r13

	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#c should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf nl,r1,r2,r14

	#checking nh and h flag
	
	#test (z or cy) = 0
	mov 0x5,r1
	mov 0xfff,r2
	or r1,r2
	#z flag should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf z,r1,r2,r16
	adf nz,r1,r2,r17

	#test (z or cy) = 1
	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flag should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf z,r1,r2,r18
	adf nz,r1,r2,r19

	#checking z and nz flag

	#test z = 0
	mov 0x5,r1
	mov 0xfff,r2
	or r1,r2
	#z flag should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf z,r1,r2,r20
	adf nz,r1,r2,r21

	#test z = 1
	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flag should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf z,r1,r2,r22
	adf nz,r1,r2,r23

	#checking s/n and ns/p condition 
	
	#test s/n = 0
	mov 0x2,r1
	mov 0x1,r2
	or r1,r2
	#flag S should be 0
	mov 0x1,r1
	mov 0x2,r2
	adf s,r1,r2,r24
	adf n,r1,r2,r25
	adf ns,r1,r2,r26
	adf p,r1,r2,r27
	
	#test ns/p = 0
	mov 0xffffffff,r1
	mov 0x05,r2
	or r1,r2
	#flag S should be 1
	mov 0x1,r1
	mov 0x2,r2
	adf s,r1,r2,r28
	adf n,r1,r2,r29
	adf ns,r1,r2,r1
	adf p,r1,r2,r2
	
	#checking lt and ge condition
	
	#lt should be 0, ge should be 1
	mov 0x5,r1
	mov 0x2,r2
	add r1,r2
	adf lt,r1,r2,r9
	mov 0x5,r1
	mov 0x2,r2
	add r1,r2
	adf gt,r1,r2,r10

	#lt should be 1, ge should be 0
	mov 0xffffffff,r2
	mov 0xffffffff,r3
	or r2,r3
	#lt should be 1
	adf lt,r1,r2,r9
	mov 0xffffffff,r2
	mov 0xffffffff,r3
	or r2,r3
	adf gt,r1,r2,r10

	#checking for le and gt condtition

	#le shoudl be 1
	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flah should be 1
	mov 0x6,r1
	mov 0x3,r2
	adf le,r1,r2,r6

	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flah should be 1
	mov 0x6,r1
	mov 0x3,r2
	adf gt,r1,r2,r7

	#gt should be 1
	mov 0x12,r1
	mov 0x4,r2
	add r1,r2
	adf le,r1,r2,r8
	adf gt,r1,r2,r9

    mov 5, r1
    mov 0xffffffff, r2
    mov 6, r4
    mov 8, r5
    sub r5,r4   # sets S bit bu not OV bit, which makes conditon lt true
    adf lt,r1,r2,r9

    # test for OV bit
    mov 0x7ffffffe, r1
    mov 0x1, r2
    adf nz,r1,r2,r9
    adf z,r1,r2,r9


	#simple tests for sbf

	mov 0x5,r9
	mov 0x8,r10
	sbf v,r9,r15,r4
	sbf nv,r9,r15,r5
	sbf c,r9,r10,r6
	sbf l,r9,r10,r7
	sbf nc,r9,r15,r8
	sbf nl,r9,r15,r2
	sbf z,r9,r15,r10
	sbf nz,r9,r15,r11
	sbf nh,r9,r15,r12
	sbf h,r9,r15,r13
	sbf s,r9,r15,r14
	sbf n,r9,r15,r1
	sbf ns,r9,r15,r16
	sbf p,r9,r15,r17
	sbf t,r9,r15,r18
	sbf lt,r9,r15,r19
	sbf ge,r9,r15,r20
	sbf le,r9,r15,r21
	sbf gt,r9,r15,r22
	
	mov 0x80000005,r1
	mov 0x7fffffff,r2
	sbf v,r1,r2,r4
	sbf nv,r1,r2,r5
	sbf c,r1,r2,r6
	sbf l,r1,r2,r7
	sbf nc,r1,r2,r8
	sbf nl,r1,r2,r9
	sbf z,r1,r2,r10
	sbf nz,r1,r2,r11
	sbf nh,r1,r2,r12
	sbf h,r1,r2,r13
	sbf s,r1,r2,r14
	sbf n,r1,r2,r15
	sbf ns,r1,r2,r16
	sbf p,r1,r2,r17
	sbf t,r1,r2,r18
	sbf lt,r1,r2,r19
	sbf ge,r1,r2,r20
	sbf le,r1,r2,r21
	sbf gt,r1,r2,r22

	mov 0x80000000,r1
	mov 0x80000000,r2
	sbf v,r1,r2,r4
	sbf nv,r1,r2,r5
	sbf c,r1,r2,r6
	sbf l,r1,r2,r7
	sbf nc,r1,r2,r8
	sbf nl,r1,r2,r9
	sbf z,r1,r2,r10
	sbf nz,r1,r2,r11
	sbf nh,r1,r2,r12
	sbf h,r1,r2,r13
	sbf s,r1,r2,r14
	sbf n,r1,r2,r15
	sbf ns,r1,r2,r16
	sbf p,r1,r2,r17
	sbf t,r1,r2,r18
	sbf lt,r1,r2,r19
	sbf ge,r1,r2,r20
	sbf le,r1,r2,r21
	sbf gt,r1,r2,r22
	#testing s flag
	
	mov 0x80000000,r1
	mov 0x80000000,r2
	sbf v,r1,r2,r4
	sbf nv,r1,r2,r5
	sbf c,r1,r2,r6
	sbf l,r1,r2,r7
	sbf nc,r1,r2,r8
	sbf nl,r1,r2,r9
	sbf z,r1,r2,r10
	sbf nz,r1,r2,r11
	sbf nh,r1,r2,r12
	sbf h,r1,r2,r13
	sbf s,r1,r2,r14
	sbf n,r1,r2,r15
	sbf ns,r1,r2,r16
	sbf p,r1,r2,r17
	sbf t,r1,r2,r18
	sbf lt,r1,r2,r19
	sbf ge,r1,r2,r20
	sbf le,r1,r2,r21
	sbf gt,r1,r2,r22
	#testing z flag
	
	#testing all conditions

	#checking v and nv condition

	#testing for v = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf v,r1,r2,r3

	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#v should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf nv,r1,r2,r4

	#testing for v = 0
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf v,r1,r2,r5

	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#v should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf nv,r1,r2,r6
	
	#checking c/l and nc/nl	
	
	#testing for c/l = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#c should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf c,r1,r2,r7
	
	#testing for c/l = 1
	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#c should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf l,r1,r2,r8

	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#c should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf nc,r1,r2,r9

	mov 0x80000000,r1
	mov 0x80000000,r2
	add r1,r2
	#c should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf nl,r1,r2,r10

	#testing for nc/nl = 0
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#c should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf c,r1,r2,r11
	
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#c should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf l,r1,r2,r12
	
	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#c should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf nc,r1,r2,r13

	mov 0x12,r1
	mov 0x3,r2
	add r1,r2
	#c should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf nl,r1,r2,r14

	#checking nh and h flag
	
	#test (z or cy) = 0
	mov 0x5,r1
	mov 0xfff,r2
	or r1,r2
	#z flag should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf z,r1,r2,r16

	mov 0x5,r1
	mov 0xfff,r2
	or r1,r2
	#z flag should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf nz,r1,r2,r17

	#test (z or cy) = 1
	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flag should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf z,r1,r2,r18
	#test (z or cy) = 1
	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flag should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf nz,r1,r2,r19
	
	#checking z and nz flag

	#test z = 0
	mov 0x5,r1
	mov 0xfff,r2
	or r1,r2
	#z flag should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf z,r1,r2,r20
	
	#test z = 0
	mov 0x5,r1
	mov 0xfff,r2
	or r1,r2
	#z flag should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf nz,r1,r2,r21

	#test z = 1
	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flag should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf z,r1,r2,r22
	#test z = 1
	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flag should be 1
	mov 0x1,r1
	sbf nz,r1,r2,r23

	#checking s/n and ns/p condition 
	
	#test s/n = 0
	mov 0x2,r1
	mov 0x1,r2
	or r1,r2
	#flag S should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf s,r1,r2,r24
	
	#test s/n = 0
	mov 0x2,r1
	mov 0x1,r2
	or r1,r2
	#flag S should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf n,r1,r2,r25
	
	#test s/n = 0
	mov 0x2,r1
	mov 0x1,r2
	or r1,r2
	#flag S should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf ns,r1,r2,r26

	#test s/n = 0
	mov 0x2,r1
	mov 0x1,r2
	or r1,r2
	#flag S should be 0
	mov 0x1,r1
	mov 0x2,r2
	sbf p,r1,r2,r27
	
	#test ns/p = 0
	mov 0xffffffff,r1
	mov 0x05,r2
	or r1,r2
	#flag S should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf s,r1,r2,r28
	
	#test ns/p = 0
	mov 0xffffffff,r1
	mov 0x05,r2
	or r1,r2
	#flag S should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf n,r1,r2,r29
	
	#test ns/p = 0
	mov 0xffffffff,r1
	mov 0x05,r2
	or r1,r2
	#flag S should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf ns,r1,r2,r1
	
	#test ns/p = 0
	mov 0xffffffff,r1
	mov 0x05,r2
	or r1,r2
	#flag S should be 1
	mov 0x1,r1
	mov 0x2,r2
	sbf p,r1,r2,r2
	
	#checking lt and ge condition
	
	#lt should be 0, ge should be 1
	mov 0x5,r1
	mov 0x2,r2
	add r1,r2
	sbf lt,r1,r2,r9
	mov 0x5,r1
	mov 0x2,r2
	add r1,r2
	sbf gt,r1,r2,r10

	#lt should be 1, ge should be 0
	mov 0xffffffff,r2
	mov 0xffffffff,r3
	or r2,r3
	#lt should be 1
	sbf lt,r1,r2,r9
	mov 0xffffffff,r2
	mov 0xffffffff,r3
	or r2,r3
	sbf gt,r1,r2,r10

	#checking for le and gt condtition

	#le shoudl be 1
	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flah should be 1
	mov 0x6,r1
	mov 0x3,r2
	sbf le,r1,r2,r6

	mov 0x0,r1
	mov 0x0,r2
	or r1,r2
	#z flah should be 1
	mov 0x6,r1
	mov 0x3,r2
	sbf gt,r1,r2,r7

	#gt should be 1
	mov 0x12,r1
	mov 0x4,r2
	add r1,r2
	sbf le,r1,r2,r8
	#gt should be 1
	mov 0x12,r1
	mov 0x4,r2
	add r1,r2
	sbf gt,r1,r2,r9

Lbl: 	br Lbl


