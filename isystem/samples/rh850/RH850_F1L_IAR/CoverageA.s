  PUBLIC _CoverageA
  RSEG CODE:CODE(4)
_CoverageA:
  ST.W R31,4[SP]
  ST.W R8,8[SP]
  ST.W R7,12[SP]
  
  mov 2,R7
  mov 0,R8
  cmp 1,R7
  ;#################  
  jarl testJMP,R31
  ;#################
  jarl testBN_a,R31
  cmp 4,R7
  jarl testBN_b,R31
  ;#################
  jarl testBZ_a,R31
  cmp 2,R7
  jarl testBZ_b,R31
  ;#################
  jarl testBL_a,R31
  cmp -2,R7
  jarl testBL_b,R31
  ;#################
  jarl testBE_a,R31
  cmp 2,R7
  jarl testBE_b,R31
  ;#################
  jarl testBLT_a,R31
  DIVH R8,R7
  jarl testBLT_b,R31
  ;#################
  jr testJR

;##################  

testJMP
  JMP[R31]  ;rt CoverageATestPoint: id=JMP38, type=expression, covered=TRUE
  
;##################  

testBN_a
  bn testBN_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BN44, type=expression, covered=TRUE
testBN_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BN46, type=expression, covered=FALSE
  
testBN_b
  bn testBN_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BN50, type=expression, covered=FALSE
testBN_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BN52, type=expression, covered=TRUE
  
;#################

testBZ_a
  bz testBZ_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BZ58, type=expression, covered=TRUE
testBZ_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BZ60, type=expression, covered=FALSE
  
testBZ_b
  bz testBZ_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BZ64, type=expression, covered=FALSE
testBZ_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BZ66, type=expression, covered=TRUE

;#################

testBL_a
  bl testBL_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BL72, type=expression, covered=TRUE
testBL_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BL74, type=expression, covered=FALSE
  
testBL_b
  bl testBL_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BL78, type=expression, covered=FALSE
testBL_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BL80, type=expression, covered=TRUE
  
;#################

testBE_a
  be testBE_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BE86, type=expression, covered=TRUE
testBE_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BE88, type=expression, covered=FALSE
  
testBE_b
  be testBE_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BE92, type=expression, covered=FALSE
testBE_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BE94, type=expression, covered=TRUE

;#################

testBLT_a
  blt testBLT_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BLT100, type=expression, covered=TRUE
testBLT_a_1
  JMP[R31]  ;rt CoverageATestPoint: id=BLT102, type=expression, covered=FALSE
  
testBLT_b
  blt testBLT_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BLT106, type=expression, covered=FALSE
testBLT_b_1
  JMP[R31]  ;rt CoverageATestPoint: id=BLT108, type=expression, covered=TRUE
  
;#################
testJR
  LD.W 4[SP],R31
  LD.W 8[SP],R8
  LD.W 12[SP],R7
  nop
  JMP[R31]  ;rt CoverageATestPoint: id=JMP116, type=expression, covered=TRUE
  
  END       
