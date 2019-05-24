.include "RH850G3M_insts.s"


.include "gpr_init.s"

.equ START, 0xfedd8000
.equ DISP22, 0x200020
.equ DISP23, 0x7ffff8
.equ TEST_ADDR_FOR_NEG_DISP, START + 8
.equ TEST_ADDR_BELOW_RAM, START + 8 - DISP22

#----------------------------------ld.b(2)--disp23--- OK
# macro LD_B, since v850-elf-gcc.exe doesn't support 48-bit load insts (ignores
#   bits above 16 of disp23)

        # test for pos. and neg. disp, pos value
		mov 0x5a, R3
        mov TEST_ADDR_BELOW_RAM, R1
        ST_B R3, DISP22, R1
        LD_B DISP22, R1, R4

		mov 0x78, R3
        mov TEST_ADDR_FOR_NEG_DISP, R2
        ST_B R3, DISP23, R2
        LD_B DISP23, R2, R5

        # test for pos. and neg. disp, neg value
		mov 0xa5, R3
        mov TEST_ADDR_BELOW_RAM, R1
        ST_B R3, DISP22, R1
        LD_B DISP22, R1, R4

		mov 0x87, R3
        mov TEST_ADDR_FOR_NEG_DISP, R2
        ST_B R3, DISP23, R2
        LD_B DISP23, R2, R5

#
#----------------------------------ld.bu(2)--disp23--- OK
# macro LD_BU, since v850-elf-gcc.exe doesn't support 48-bit load insts (ignores
#   bits above 16 of disp23)

        # test for pos. and neg. disp, pos value
		mov     0x5a, R3
        mov     TEST_ADDR_BELOW_RAM, R1
        ST_B    R3, DISP22, R1
        LD_BU   DISP22, R1, R4

		mov     0x78, R3
        mov     TEST_ADDR_FOR_NEG_DISP, R2
        ST_B    R3, DISP23, R2
        LD_BU   DISP23, R2, R5

        # test for pos. and neg. disp, neg value
		mov     0xa5, R3
        mov     TEST_ADDR_BELOW_RAM, R1
        ST_B    R3, DISP22, R1
        LD_BU   DISP22, R1, R4

		mov     0x87, R3
        mov     TEST_ADDR_FOR_NEG_DISP, R2
        ST_B    R3, DISP23, R2
        LD_BU   DISP23, R2, R5


#----------------------------------ld.h(2)--disp23--- OK
# macro LD_H

        # test for pos. and neg. disp, pos value
		mov     0x5a45, R3
        mov     TEST_ADDR_BELOW_RAM, R1
        ST_H    R3, DISP22, R1
        LD_H    DISP22, R1, R4

		mov     0x78a0, R3
        mov     TEST_ADDR_FOR_NEG_DISP, R2
        ST_H    R3, DISP23, R2
        LD_H    DISP23, R2, R5

        # test for pos. and neg. disp, neg value
		mov     0xa534, R3
        mov     TEST_ADDR_BELOW_RAM, R1
        ST_H    R3, DISP22, R1
        LD_H    DISP22, R1, R4

		mov     0x87fe, R3
        mov     TEST_ADDR_FOR_NEG_DISP, R2
        ST_H    R3, DISP23, R2
        LD_H    DISP23, R2, R5



#----------------------------------ld.hu(2)--disp23--- OK
# macro LD_HU

        # test for pos. and neg. disp, pos value
		mov     0x5a45, R3
        mov     TEST_ADDR_BELOW_RAM, R1
        ST_H    R3, DISP22, R1
        LD_HU   DISP22, R1, R4

		mov     0x78a0, R3
        mov     TEST_ADDR_FOR_NEG_DISP, R2
        ST_H    R3, DISP23, R2
        LD_HU   DISP23, R2, R5

        # test for pos. and neg. disp, neg value
		mov     0xa534, R3
        mov     TEST_ADDR_BELOW_RAM, R1
        ST_H    R3, DISP22, R1
        LD_HU   DISP22, R1, R4

		mov     0x87fe, R3
        mov     TEST_ADDR_FOR_NEG_DISP, R2
        ST_H    R3, DISP23, R2
        LD_HU   DISP23, R2, R5


#----------------------------------ld.w(2)--disp23--- OK
# macro LD_W

        # test for pos. and neg. disp, pos value
		mov     0x5a45, R3
        mov     TEST_ADDR_BELOW_RAM, R1
        ST_W    R3, DISP22, R1
        LD_W    DISP22, R1, R4

		mov     0x78a0, R3
        mov     TEST_ADDR_FOR_NEG_DISP, R2
        ST_W    R3, DISP23, R2
        LD_W    DISP23, R2, R5

        # test for pos. and neg. disp, neg value
		mov     0xa534, R3
        mov     TEST_ADDR_BELOW_RAM, R1
        ST_W    R3, DISP22, R1
        LD_W    DISP22, R1, R4

		mov     0x87fe, R3
        mov     TEST_ADDR_FOR_NEG_DISP, R2
        ST_W    R3, DISP23, R2
        LD_W    DISP23, R2, R5


Lbl: 	BR Lbl

