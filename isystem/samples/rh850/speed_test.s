.include "RH850G3M_insts.s"
.text   

# This test runs long loop to test performance.
# This should not be used for comparison with hardware as 
# it will last VERY long ...
    mov 0x0400000, r7
    mov 200, r6
    mov 1, r1

lbl:
    # ST_DW R0, 0, R6
    st.w R0, 0, R6
    addi 1, r6, r6
    cmp r6, r7
    bne lbl

    halt

