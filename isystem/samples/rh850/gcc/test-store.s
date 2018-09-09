.data
var1:   .byte   12
var2:   .byte   0x34
var3:   .byte   0xee
var4:   .byte   -9

start = 0xfee00000

.text
        mov start, r1
        movea var1 - .data, r1, r2
        mov 12, r3
        st.b r3, 0[r2]
        ld.b 0[r2], r4
