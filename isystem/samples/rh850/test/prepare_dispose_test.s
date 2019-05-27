.include "RH850G3M_insts.s"
.include "gpr_init.s"

#---------------- PREPARE -------------------

	mov hilo(.data + 0x100), r3	# stack pointer

	mov 0x324, r20
	mov 0x124a, r21
	mov 0xfffc, r22
	mov 0x1, r23
	mov 0xcbd, r24
	mov 0x74c, r25
	mov 0x99924, r26
	mov 0x1f2112a, r27
	mov 0xabbbc, r28
	mov 0x45784, r29
	mov 0xec212, r30
	mov 0xa344c, r31


    prepare {r21, r22, r24, r27, r28, r29}, 0
	
	
#----------------Clearing regs-------------------

	mov 0x0,r20
	mov 0x0,r21
	mov 0x0,r22
	mov 0x0,r23
	mov 0x0,r24
	mov 0x0,r25
	mov 0x0,r26
	mov 0x0,r27
	mov 0x0,r28
	mov 0x0,r29
	mov 0x0,r30
	mov 0x0,r31

#---------------- DISPOSE -------------------

	
	dispose 0x0, {r21, r22, r24, r27, r28, r29}



#---------------- PREPARE -------------------


	mov 0x324, r20
	mov 0x124a, r21
	mov 0xfffc, r22
	mov 0x1, r23
	mov 0xcbd, r24
	mov 0x74c, r25
	mov 0x99924, r26
	mov 0x1f2112a, r27
	mov 0xabbbc, r28
	mov 0x45784, r29
	mov 0xec212, r30
	mov 0xa344c, r31



	prepare {r22, r23, r26, r27, r28}, 0x18, 1
	
	
#----------------Clearing regs-------------------

	mov 0x0,r20
	mov 0x0,r21
	mov 0x0,r22
	mov 0x0,r23
	mov 0x0,r24
	mov 0x0,r25
	mov 0x0,r26
	mov 0x0,r27
	mov 0x0,r28
	mov 0x0,r29
	mov 0x0,r30
	mov 0x0,r31

#---------------- DISPOSE -------------------

	mov hilo(Lbl1), r5
	dispose 0x18, {r22, r23, r25, r28, r30}, [r5]

    mov     2, r6
Lbl1:

#---------------- PREPARE -------------------

	mov 0x324, r20
	mov 0x124a, r21
	mov 0xfffc, r22
	mov 0x1, r23
	mov 0xcbd, r24
	mov 0x74c, r25
	mov 0x99924, r26
	mov 0x1f2112a, r27
	mov 0xabbbc, r28
	mov 0x45784, r29
	mov 0xec212, r30
	mov 0xa344c, r31



	prepare {r0}, 0x5
	
	
#----------------Clearing regs-------------------

	mov 0x0,r20
	mov 0x0,r21
	mov 0x0,r22
	mov 0x0,r23
	mov 0x0,r24
	mov 0x0,r25
	mov 0x0,r26
	mov 0x0,r27
	mov 0x0,r28
	mov 0x0,r29
	mov 0x0,r30
	mov 0x0,r31

#---------------- DISPOSE -------------------

	
	dispose 0x5, {r20}



#---------------- PREPARE -------------------

	mov 0x324, r20
	mov 0x124a, r21
	mov 0xfffc, r22
	mov 0x1, r23
	mov 0xcbd, r24
	mov 0x74c, r25
	mov 0x99924, r26
	mov 0x1f2112a, r27
	mov 0xabbbc, r28
	mov 0x45784, r29
	mov 0xec212, r30
	mov 0xa344c, r31



	prepare {r20-r31}, 0xa
	
	
#----------------Clearing regs-------------------

	mov 0x0,r20
	mov 0x0,r21
	mov 0x0,r22
	mov 0x0,r23
	mov 0x0,r24
	mov 0x0,r25
	mov 0x0,r26
	mov 0x0,r27
	mov 0x0,r28
	mov 0x0,r29
	mov 0x0,r30
	mov 0x0,r31

#---------------- DISPOSE -------------------

	
	dispose 0xa, {r20-r31}

#------------------------------------------------






Lbl: br Lbl
