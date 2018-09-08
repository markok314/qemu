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
#-----------------------------------ld.dw----------- Seems that v850 doesn't have ld.dw

        ld.dw var1[r1], r2
        ld.dw var2[r1], r3
        ld.dw var3[r1], r4
        ld.dw var4[r1], r5

var1:   .quad   63
var2:   .quad   0x1234
var3:   .quad   0xcdef1111
var4:   .quad   -10
