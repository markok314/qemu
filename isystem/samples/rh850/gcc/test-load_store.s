.data
var1:   .byte   0x12
var2:   .byte   0x34
var3:   .byte   0x56
var4:   .byte   0x78

start = 0xfee00000

.text
        mov start, r1
        movea var1 - .data, r1, r2
        ld.b 0x0[r2], r4
        ld.b 0x1[r2], r5
        ld.b 0x2[r2], r6
