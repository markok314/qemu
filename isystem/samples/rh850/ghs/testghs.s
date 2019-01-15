.text
.global _main
_main:
        ld.b 0x1[r7],r1
        ld.b 0x5 [r6],r1
        jarl 0x4, r18
        sld.bu 3[ep], r23
        jarl 0x12346, lp
        jmp 0x12 [r6]
        ld.bu 0xf[r5],r6
        bgt 0x2
        
