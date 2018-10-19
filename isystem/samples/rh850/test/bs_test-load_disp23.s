.include "RH850G3M_insts.s"

start = 0xfee00000

.text
        mov start, R1

#----------------------------------ld.b(2)--disp23---
# macro LD_B, since v850-elf-gcc.exe obviously doesn't support 48-bit load insts (ignores
#   bits above 16 of disp23)

        movea 0, R1, R2

		mov 12, R3
        ST_B R3, 0x7fff, R2
        LD_B 0x7fff, R2, R4

		mov 0x78, R3
        ST_B R3, 0xffff, R2
        LD_B 0xffff, R2, R5

		mov 0xab, R3
        ST_B R3, 0x3ffff, R2
        LD_B 0x3ffff, R2, R6

		mov -11, R3
        ST_B R3, 0x7ffff, R2
        LD_B 0x7ffff, R2, R7


# Does not work for disp23 longer than 16 bits (probably due to RAM size being 64K)

#----------------------------------ld.bu(2)--disp23---


#----------------------------------ld.h(2)--disp23---


#----------------------------------ld.hu(2)--disp23---


#----------------------------------ld.w(2)--disp23---

