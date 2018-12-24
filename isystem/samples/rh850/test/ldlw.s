.data


.macro LDL_W r1:req, r3:req
    .byte 0xe0 | \r1
    .byte 0x07
    .byte 0x78
    .byte 0x03 | (\r3 << 3)
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

.include "gpr_init.s"



	mov 0x12345679, r5
	mov 0x0000ffe1, r6

	LDL_W R6, R4
	LDL_W R5, R9
	



Lbl:	br Lbl


