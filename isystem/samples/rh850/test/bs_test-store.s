.data
var1:   .byte   0
var2:   .byte   0
var3:   .byte   0
var4:   .byte   0,0,0,0,0,0,0

start = 0xfee00000

.text
        mov start, r1

#----------------------------------st.b(1)--disp16---OK
/*
		mov 12, r3
        movea var1 - .data, r1, r2
        st.b r3, 0[r2]
        ld.bu 0[r2], r4

        mov 0x85, r3
        movea var2 - .data, r1, r2
        st.b r3, 0[r2]
        ld.bu 0[r2], r5

        mov -11, r3
        movea var3 - .data, r1, r2
        st.b r3, 0[r2]
        ld.bu 0[r2], r6

        mov 120, r3
        movea var4 - .data, r1, r2
        st.b r3, 0[r2]
        ld.bu 0[r2], r7
*/
#----------------------------------st.h(1)--disp16---OK
/*
        mov 12, r3
        movea var1 - .data, r1, r2
        st.h r3, 0[r2]
        ld.hu 0[r2], r4

        mov 0x4bcd, r3
        movea var2 - .data, r1, r2
        st.h r3, 0[r2]
        ld.hu 0[r2], r5

        mov -11, r3
        movea var3 - .data, r1, r2
        st.h r3, 0[r2]
        ld.hu 0[r2], r6

        mov 2047, r3
        movea var4 - .data, r1, r2
        st.h r3, 0[r2]
        ld.hu 0[r2], r7
*/
#----------------------------------st.w(1)--disp16---OK

        mov 12, r3
        movea var1 - .data, r1, r2
        st.w r3, 0[r2]
        ld.w 0[r2], r4

        mov 0x4321abcd, r3
        movea var2 - .data, r1, r2
        st.w r3, 0[r2]
        ld.w 0[r2], r5

        mov -11, r3
        movea var3 - .data, r1, r2
        st.w r3, 0[r2]
        ld.w 0[r2], r6

        mov 2047, r3
        movea var4 - .data, r1, r2
        st.w r3, 0[r2]
        ld.w 0[r2], r7

#----------------------------------st.dw(1)--disp16--- v850 doesn't have st.dw


#----------------------------------sst.b----------- OK
/*
        mov 12, r3
        movea var1 - .data, r1, r30
        sst.b r3, 0x7f[r30]
        sld.b 0x7f[r30], r4
		# sld.bu has only a 4-bit disp, therefore sld.b is applied (despite the sign-extension)

        mov -11, r3
        movea var2 - .data, r1, r30
        sst.b r3, 0x7f[r30]
        sld.b 0x7f[r30], r5

        mov 0x85, r3
        movea var3 - .data, r1, r30
        sst.b r3, 0xf[r30]
        sld.bu 0xf[r30], r6

        mov 120, r3
        movea var4 - .data, r1, r30
        sst.b r3, 0xf[r30]
        sld.bu 0xf[r30], r7
*/
#----------------------------------sst.h-----------OK
/*
        mov 12, r3
        movea var1 - .data, r1, r30
        sst.h r3, 0xfe[r30]
        # last bit must be 0, otherwise misaligned
        sld.h 0xfe[r30], r4

        mov 0x8577, r3
        movea var2 - .data, r1, r30
        sst.h r3, 0x7e[r30]
        sld.h 0x7e[r30], r5

        mov -11, r3
        movea var3 - .data, r1, r30
        sst.h r3, 0x1e[r30]
        sld.hu 0x1e[r30], r6

        mov 2047, r3
        movea var4 - .data, r1, r30
        sst.h r3, 0x1e[r30]
        sld.hu 0x1e[r30], r7
*/
#----------------------------------sst.w----------- OK
/*
        mov 0x123, r3
        movea var1 - .data, r1, r30
        sst.w r3, 0xfc[r30]
        sld.w 0xfc[r30], r4

        mov 0xabcd1234, r3
        movea var2 - .data, r1, r30
        sst.w r3, 0x74[r30]
        sld.w 0x74[r30], r5

        mov -11, r3
        movea var3 - .data, r1, r30
        sst.w r3, 0x18[r30]
        sld.w 0x18[r30], r6

        mov 2047, r3
        movea var4 - .data, r1, r30
        sst.w r3, 0x0[r30]
        sld.w 0x0[r30], r7
*/

