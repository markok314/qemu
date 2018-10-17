.define

.data
var1:   .space  5
var2:   .byte   7
var3:   .byte   0
var4:   .byte   0,0,0,0,0,0,0

start = 0xfee00000

.text
        mov start, r1

.ifdef LD.B_2

#----------------------------------ld.b(2)--disp23---
        movea var1 - .data, r1, r2

		mov 12, r3
        st.b r3, 0[r2]
        ld.b 0[r2], r4

		mov 0x78, r3
        st.b r3, 0xffff[r2]
        ld.b 0xffff[r2], r5

		mov 0xab, r3
        st.b r3, 0x3ffff[r2]
        ld.b 0x3ffff[r2], r6

		mov -11, r3
        st.b r3, 0x5555[r2]
        ld.b 0x5555[r2], r7

#----------------------------------ld.bu(2)--disp23---


#----------------------------------ld.h(2)--disp23---


#----------------------------------ld.hu(2)--disp23---


#----------------------------------ld.w(2)--disp23---

