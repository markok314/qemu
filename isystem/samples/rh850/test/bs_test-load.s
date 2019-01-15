.include "RH850G3M_insts.s"
.include "gpr_init.s"

		.data
var1:   .byte   63
var2:   .byte   0x34
var3:   .byte   0xc3
var4:   .byte   -10
		.align 2
var5:   .hword   0x1234
var6:   .hword   0xcdef
var6b:	.hword  -30
		.align 4
var7:   .word   0xcdef1111
var8:   .byte   0x12
var9:   .byte   0xcd
		.align 4
var10:   .word   0xabcdef
var11:	.byte 0x6
var12:	.byte 0x7
var13:	.byte 0x3
var14:	.byte 0x1

.text
		movea .data, r0, r1

#----------------------------------ld.b(1)--disp16--- OK

        ld.b var1[r1], r2
        ld.b var2[r1], r3
        ld.b var3[r1], r4
        ld.b var4[r1], r5

#----------------------------------ld.bu(1)--disp16--- OK

        ld.bu var1[r1], r2
        ld.bu var2[r1], r3
        ld.bu var3[r1], r4
        ld.bu var4[r1], r5

#----------------------------------ld.h(1)--disp16--- OK

        ld.h var1[r1], r2
        ld.h var5[r1], r3
        ld.h var6[r1], r4
        ld.h var6b[r1], r5


#----------------------------------ld.hu(1)--disp16--- OK

        ld.hu var1[r1], r2
        ld.hu var5[r1], r3
        ld.hu var6[r1], r4
        ld.hu var6b[r1], r5

#-----------------------------------ld.w-----------OK

        ld.w var1[r1], r2
        ld.w var5[r1], r3
        ld.w var7[r1], r4
        ld.w var6b[r1], r5

#-----------------------------------sld.b--------- OK

		mov r1, r30

        sld.b var1[r30], r2
        sld.b var4[r30], r3
        sld.b var8[r30], r4
        sld.b var9[r30], r5

#-----------------------------------sld.bu--------- OK
# disp has only 4 bits! (vars must be low addressed)

		mov r1, r30

        sld.bu var1[r30], r2
        sld.bu var2[r30], r3
        sld.bu var3[r30], r4
        sld.bu var4[r30], r5

#-----------------------------------sld.h--------- OK

		mov r1, r30

        sld.h var1[r30], r2
        sld.h var5[r30], r3
        sld.h var6[r30], r4
        sld.h var6b[r30], r5

#-----------------------------------sld.bu--------- OK
# disp has only 4 bits! (vars must be low addressed)

		mov r1, r30

        sld.hu var1[r30], r2
        sld.hu var3[r30], r3
        sld.hu var5[r30], r4
        sld.hu var6[r30], r5

#-----------------------------------sld.w--------- OK

		mov r1, r30

        sld.w var1[r30], r2
        sld.w var2[r30], r3
        sld.w var3[r30], r4
        sld.w var4[r30], r5

Lbl:	BR Lbl
