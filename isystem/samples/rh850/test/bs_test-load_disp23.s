.include "RH850G3M_insts.s"

start = 0xfee00000

.include "gpr_init.s"

#----------------------------------ld.b(2)--disp23--- OK
# macro LD_B, since v850-elf-gcc.exe doesn't support 48-bit load insts (ignores
#   bits above 16 of disp23)

        mov 0xfeb00000, R1

		mov 12, R3
        ST_B R3, 0x301000, R1
        LD_B 0x301000, R1, R4

        mov 0xff10a000, R2

		mov 0x78, R3
        ST_B R3, 0x4ffff0, R2
        LD_B 0x4ffff0, R2, R5

        mov 0xfee07000, R2

		mov -11, R3
        ST_B R3, 0x7ff000, R2
        LD_B 0x7ff000, R2, R6

		mov 0xab, R3
        ST_B R3, 0x1000, R2
        LD_B 0x1000, R2, R7


#----------------------------------ld.bu(2)--disp23--- OK
# macro LD_BU, since v850-elf-gcc.exe doesn't support 48-bit load insts (ignores
#   bits above 16 of disp23)

        mov 0xfeb00000, R1

		mov 12, R3
        ST_B R3, 0x301000, R1
        LD_BU 0x301000, R1, R4

        mov 0xff10a000, R2

		mov 0x78, R3
        ST_B R3, 0x4ffff0, R2
        LD_BU 0x4ffff0, R2, R5

        mov 0xfee07000, R2

		mov -11, R3
        ST_B R3, 0x7ff000, R2
        LD_BU 0x7ff000, R2, R6

		mov 0xab, R3
        ST_B R3, 0x1000, R2
        LD_BU 0x1000, R2, R7


#----------------------------------ld.h(2)--disp23--- OK
# macro LD_H

        mov 0xfeb00000, R1

		mov 0x1234, R3
        ST_H R3, 0x301000, R1
        LD_H 0x301000, R1, R4

        mov 0xff10a000, R2

		mov 0x56789, R3
        ST_H R3, 0x4ffff0, R2
        LD_H 0x4ffff0, R2, R5

        mov 0xfee07000, R2

		mov -11, R3
        ST_H R3, 0x7ff000, R2
        LD_H 0x7ff000, R2, R6

		mov 0xabcd, R3
        ST_H R3, 0x1000, R2
        LD_H 0x1000, R2, R7


#----------------------------------ld.hu(2)--disp23--- OK
# macro LD_HU

        mov 0xfeb00000, R1

		mov 0x1234, R3
        ST_H R3, 0x301000, R1
        LD_HU 0x301000, R1, R4

        mov 0xff10a000, R2

		mov 0x56789, R3
        ST_H R3, 0x4ffff0, R2
        LD_HU 0x4ffff0, R2, R5

        mov 0xfee07000, R2

		mov -11, R3
        ST_H R3, 0x7ff000, R2
        LD_HU 0x7ff000, R2, R6

		mov 0xabcd, R3
        ST_H R3, 0x1000, R2
        LD_HU 0x1000, R2, R7

#----------------------------------ld.w(2)--disp23--- OK
# macro LD_W

        mov 0xfeb00000, R1

		mov 0x123456, R3
        ST_W R3, 0x305000, R1
        LD_W 0x305000, R1, R4

        mov 0xff10a000, R2

		mov 0x56789, R3
        ST_W R3, 0x4ffff0, R2
        LD_W 0x4ffff0, R2, R5

        mov 0xfee07000, R2

		mov -11, R3
        ST_W R3, 0x7ff000, R2
        LD_W 0x7ff000, R2, R6

		mov 0xabcdef00, R3
        ST_W R3, 0x7FFA24, R2
        LD_W 0x7FFA24, R2, R7

Lbl: 	BR Lbl

