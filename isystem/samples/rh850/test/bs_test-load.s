.include "RH850G3M_insts.s"

		.data
var1:   .byte   63
var2:   .byte   0x34
var3:   .byte   0xc3
var4:   .byte   -10
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

start = 0xfee00000

.include "gpr_init.s"

		mov start, r1

#----------------------------------ld.b(1)--disp16--- OK

		movea var1-.data, r0, r1
        ld.b 0[r1], r2
        ld.b 1[r1], r3
        ld.b 2[r1], r4
        ld.b 3[r1], r5

#----------------------------------ld.bu(1)--disp16--- OK

        ld.bu 0[r1], r2
        ld.bu 1[r1], r3
        ld.bu 2[r1], r4
        ld.bu 3[r1], r5

#----------------------------------ld.h(1)--disp16--- OK

        ld.h 0[r1], r2
        ld.h 4[r1], r3
        ld.h 6[r1], r4
        ld.h 8[r1], r5

		BR Lbl

#----------------------------------ld.hu(1)--disp16--- OK

        ld.hu 0[r1], r2
        ld.hu 4[r1], r3
        ld.hu 6[r1], r4
        ld.hu 8[r1], r5

#-----------------------------------ld.w-----------OK

        ld.w 0[r1], r2
        ld.w 4[r1], r3
        ld.w 12[r1], r4
        ld.w 8[r1], r5

#-----------------------------------sld.b--------- OK

		mov r1, r30

        sld.b 0[r30], r2
        sld.b 3[r30], r3
        sld.b 14[r30], r4
        sld.b 15[r30], r5

#-----------------------------------sld.bu--------- OK
# disp has only 4 bits! (vars must be low addressed)

		mov r1, r30

        sld.bu 0[r30], r2
        sld.bu 1[r30], r3
        sld.bu 2[r30], r4
        sld.bu 3[r30], r5

#-----------------------------------sld.h--------- OK

		mov r1, r30

        sld.h 0[r30], r2
        sld.h 2[r30], r3
        sld.h 4[r30], r4
        sld.h 6[r30], r5

#-----------------------------------sld.bu--------- OK
# disp has only 4 bits! (vars must be low addressed)

		mov r1, r30

        sld.hu 0[r30], r2
        sld.hu 2[r30], r3
        sld.hu 4[r30], r4
        sld.hu 8[r30], r5

#-----------------------------------sld.w--------- OK

		mov r1, r30

        sld.w 12[r30], r2
        sld.w 16[r30], r3

Lbl:	BR Lbl

