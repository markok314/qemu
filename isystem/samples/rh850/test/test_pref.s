.include "RH850G3M_insts.s"

# This test proves only that QEMU recognizes the instructions and that it has no effect
# on CPU state. Cache manipulation has no meaning in QEMU anyway.
.text
	PREF 00000,[R1]
	PREF 00000,[R2]
Lbl: 
    br Lbl

	
