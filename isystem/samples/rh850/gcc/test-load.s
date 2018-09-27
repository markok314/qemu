.include "../test/Hide\ tests/RH850G3M_insts.s"

var1:   .quad   0x12345678
var2:   .quad   0x0123456789abcdef
var3:   .quad   0xcdef1111
var4:   .quad   -10

.text
		movea .text, r0, r1

#----------------------------------ld.b(1)--disp16--- OK
/*
        ld.b var1[r1], r2
        ld.b var2[r1], r3
        ld.b var3[r1], r4
        ld.b var4[r1], r5

var1:   .byte   63
var2:   .byte   0x34
var3:   .byte   0xc3
var4:   .byte   -10
*/
#----------------------------------ld.b(2)-----disp23 // zaenkrat nimamo dostopa do visokih naslovov!
#
#		...

#----------------------------------ld.bu(1)--disp16--- OK
/*
        ld.bu var1[r1], r2
        ld.bu var2[r1], r3
        ld.bu var3[r1], r4
        ld.bu var4[r1], r5

var1:   .byte   63
var2:   .byte   0x34
var3:   .byte   0xc3
var4:   .byte   -10
*/

#----------------------------------ld.bu(2)-----disp23 // zaenkrat nimamo dostopa do visokih naslovov!
#
#		...

#----------------------------------ld.h(1)--disp16--- OK
/*
        ld.h var1[r1], r2
        ld.h var2[r1], r3
        ld.h var3[r1], r4
        ld.h var4[r1], r5

var1:   .hword   63
var2:   .hword   0x1234
var3:   .hword   0xcdef
var4:   .hword   -10
*/
#----------------------------------ld.hu(1)--disp16--- OK
/*
        ld.hu var1[r1], r2
        ld.hu var2[r1], r3
        ld.hu var3[r1], r4
        ld.hu var4[r1], r5

var1:   .hword   63
var2:   .hword   0x1234
var3:   .hword   0xcdef
var4:   .hword   -10
*/
#-----------------------------------ld.w-----------OK
/*
        ld.w var1[r1], r2
        ld.w var2[r1], r3
        ld.w var3[r1], r4
        ld.w var4[r1], r5

var1:   .word   63
var2:   .word   0x1234
var3:   .word   0xcdef1111
var4:   .word   -10
*/
#-----------------------------------ld.dw -> macro LD_DW---------

start = 0xfee00000
		mov start, r1
		movea 0, r1, r2

        LD_DW 0, r2, r4
        /*LD_DW 8, r2, r6
        LD_DW 16, r2, r8
        LD_DW 24, r2, r10*/


#-----------------------------------sld.b--------- OK
/*
		mov r1, r30

        sld.b var1[r30], r2
        sld.b var2[r30], r3
        sld.b var3[r30], r4
        sld.b var4[r30], r5

var1:   .byte   63
var2:   .byte   0x12
var3:   .byte   0xcd
var4:   .byte   -10
*/
#-----------------------------------sld.bu--------- OK
# disp has only 4 bits! (vars must be low addressed)
/*
var1:   .byte   63
var2:   .byte   0x12
var3:   .byte   0xcd
var4:   .byte   -10

		mov r1, r30

        sld.bu var1[r30], r2
        sld.bu var2[r30], r3
        sld.bu var3[r30], r4
        sld.bu var4[r30], r5
*/

#-----------------------------------sld.h--------- OK
/*
		mov r1, r30

        sld.h var1[r30], r2
        sld.h var2[r30], r3
        sld.h var3[r30], r4
        sld.h var4[r30], r5

var1:   .hword   63
var2:   .hword   0x1234
var3:   .hword   0xcdef
var4:   .hword   -10
*/

#-----------------------------------sld.bu--------- OK
# disp has only 4 bits! (vars must be low addressed)
/*
var1:   .hword   63
var2:   .hword   0x1234
var3:   .hword   0xcdef
var4:   .hword   -10

		mov r1, r30

        sld.hu var1[r30], r2
        sld.hu var2[r30], r3
        sld.hu var3[r30], r4
        sld.hu var4[r30], r5
*/

#-----------------------------------sld.w--------- OK
/*
		mov r1, r30

        sld.w var1[r30], r2
        sld.w var2[r30], r3
        sld.w var3[r30], r4
        sld.w var4[r30], r5

bv:		.hword  0
var1:   .word   63
var2:   .word   0x1234
var3:   .word   0xabcdef
var4:   .word   -10
*/
