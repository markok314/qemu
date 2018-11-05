.data

.macro BINS1 r1:req, pos:req, width:req, r2:req
   .byte 0xe0 | \r2
   .byte 0x07 | (\r1 << 3)
   .byte 0x90 | ((\pos & 0x7) << 1)
   .byte 0x00 | ((\pos & 0x8)) | (((\width + \pos - 0x1 )& 0xf) << 4)
.endm

.macro BINS2 r1:req, pos:req, width:req, r2:req
   .byte 0xe0 | \r2
   .byte 0x07 | (\r1 << 3)
   .byte 0xB0 | ((\pos & 0x7) << 1)
   .byte 0x00 | ((\pos & 0x8)) | (((\width + \pos - 0x1 )& 0xf) << 4)
.endm

.macro BINS3 r1:req, pos:req, width:req, r2:req
   .byte 0xe0 | \r2
   .byte 0x07 | (\r1 << 3)
   .byte 0xD0 | ((\pos & 0x7) << 1)
   .byte 0x00 | ((\pos & 0x8)) | (((\width + \pos - 0x1 )& 0xf) << 4)
.endm

.equ R0, 0
.equ R1, 1
.equ R2, 2
.equ R3, 3
.equ R4, 4
.equ R5, 5
.equ R6, 6
.equ R7, 7
.equ R8, 8
.equ R9, 9

.text

	mov 0x0,r1
	mov 0x0,r2
	mov 0x0,r3
	mov 0x0,r4
	mov 0x0,r5
	mov 0x0,r6
	mov 0x0,r7
	mov 0x0,r8
	mov 0x0,r9
	mov 0x0,r10
	mov 0x0,r11
	mov 0x0,r12
	mov 0x0,r13
	mov 0x0,r14
	mov 0x0,r15
	mov 0x0,r16
	mov 0x0,r17
	mov 0x0,r18
	mov 0x0,r19
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

# Individual testing of BINS  // BINS reg1, pos, width, reg2

	mov 0xffffffff, r1
	mov 0x11111111, r2
	mov 0x00000000, r3
	
	BINS3 R3, 0, 4, R2	# lsb<16 msb<16
	BINS3 R4, 4, 4, R2	# lsb<16 msb<16
	BINS3 R5, 8, 4, R2	# lsb<16 msb<16
	BINS2 R6, 8, 8, R2	# lsb<16 msb>=16

	BINS1 R7, 8, 8, R1	# lsb>=16 msb>=16

	BINS2 R8, 8, 8, R1	# lsb>=16 msb>=16

	BINS3 R9, 8, 8, R1	# lsb>=16 msb>=16



Lbl:	br Lbl
