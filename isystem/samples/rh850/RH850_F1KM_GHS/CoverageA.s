.text
.global _CoverageA
_CoverageA:
  add 0xFFFFFFF4,SP
  ST.W R31,4[SP]
  ST.W R8,8[SP]
  ST.W R7,12[SP]
  
  mov 2,R7
  mov 0x80000000,R8
  --#################
  jarl delay,R31
  --#################
  cmp 1,R7
  jarl testBSA_notTaken,R31
  jarl testBSA_both,R31  
  jarl testBN_notTaken,R31
  jarl testBN_both,R31
  jarl testBP_taken,R31
  jarl testBP_both,R31
  jarl testBNH_notTaken,R31
  jarl testBNH_both,R31
  jarl testBLT_notTaken,R31
  jarl testBLT_both,R31
  jarl testBLE_notTaken,R31
  jarl testBLE_both,R31
  jarl testBGT_taken,R31
  jarl testBGT_both,R31
  jarl testBGE_taken,R31
  jarl testBGE_both,R31
  jarl testBH_taken,R31
  jarl testBH_both,R31
  jarl testBNE_taken,R31
  jarl testBNE_both,R31
  
  jarl delay,R31
  cmp 2,R7
  jarl testBZ_taken,R31
  jarl testBZ_both,R31
  jarl testBNZ_notTaken,R31
  jarl testBNZ_both,R31
  jarl testBE_taken,R31
  jarl testBE_both,R31   
  jarl testBC_notTaken,R31
  jarl testBC_both,R31  
  jarl testBNC_taken,R31
  jarl testBNC_both,R31
  jarl testBL_notTaken,R31
  jarl testBL_both,R31
  jarl testBNH_taken,R31
  jarl testBNH_both,R31
  jarl testBNL_taken,R31
  jarl testBNL_both,R31
  jarl testBNE_notTaken,R31
  jarl testBNE_both,R31
  jarl testBV_notTaken,R31
  jarl testBV_both,R31   
  jarl testBNV_taken,R31
  jarl testBNV_both,R31
  
  jarl delay,R31
  cmp 3,R7
  jarl testBE_notTaken,R31
  jarl testBE_both,R31
  jarl testBC_taken,R31
  jarl testBC_both,R31   
  jarl testBNC_notTaken,R31
  jarl testBNC_both,R31 
  jarl testBL_taken,R31
  jarl testBL_both,R31
  jarl testBH_notTaken,R31
  jarl testBH_both,R31
  jarl testBNL_notTaken,R31
  jarl testBNL_both,R31   
  jarl testBN_taken,R31  
  jarl testBN_both,R31
  jarl testBP_notTaken,R31  
  jarl testBP_both,R31
  jarl testBZ_notTaken,R31
  jarl testBZ_both,R31
  jarl testBNZ_taken,R31
  jarl testBNZ_both,R31
  jarl testBLT_taken,R31
  jarl testBLT_both,R31
  jarl testBLE_taken,R31
  jarl testBLE_both,R31
  jarl testBGT_notTaken,R31
  jarl testBGT_both,R31
  jarl testBGE_notTaken,R31
  jarl testBGE_both,R31

  jarl delay,R31
  satadd 0x80000000,R8
  jarl testBSA_taken,R31
  jarl testBSA_both,R31  
  jarl testBV_taken,R31
  jarl testBV_both,R31   
  jarl testBNV_notTaken,R31
  jarl testBNV_both,R31    
  
  --#################
  jarl delay,R31
  --#################
  jr testJR --rt CoverageATestPoint2: id=JR, type=expression, covered=TRUE

dummy:
  JMP[R31]  --rt CoverageATestPoint2: id=JMPT, type=expression, covered=TRUE

dummyNotExecuted:
  JMP[R31]  --rt CoverageATestPoint2: id=JMPD, type=expression, covered=FALSE 

--#################

testBL_taken:
  bl dummy --rt CoverageATestPoint2: id=blt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBL_notTaken:
  bl dummy --rt CoverageATestPoint2: id=bln, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBL_both:
  bl dummy --rt CoverageATestPoint2: id=blb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBNH_taken:
  bnh dummy --rt CoverageATestPoint2: id=bnht, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBNH_notTaken:
  bnh dummy --rt CoverageATestPoint2: id=bnhn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBNH_both:
  bnh dummy --rt CoverageATestPoint2: id=bnhb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
 
--#################

testBH_taken:
  bh dummy --rt CoverageATestPoint2: id=bht, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBH_notTaken:
  bh dummy --rt CoverageATestPoint2: id=bhn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBH_both:
  bh dummy --rt CoverageATestPoint2: id=bhb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
   
--#################

testBNL_taken:
  bnl dummy --rt CoverageATestPoint2: id=bnlt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBNL_notTaken:
  bnl dummy --rt CoverageATestPoint2: id=bnln, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBNL_both:
  bnl dummy --rt CoverageATestPoint2: id=bnlb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBE_taken:
  be dummy --rt CoverageATestPoint2: id=bet, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBE_notTaken:
  be dummy --rt CoverageATestPoint2: id=ben, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBE_both:
  be dummy --rt CoverageATestPoint2: id=beb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBNE_taken:
  bne dummy --rt CoverageATestPoint2: id=bnet, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBNE_notTaken:
  bne dummy --rt CoverageATestPoint2: id=bnen, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBNE_both:
  bne dummy --rt CoverageATestPoint2: id=bneb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBLT_taken:
  blt dummy --rt CoverageATestPoint2: id=bltt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBLT_notTaken:
  blt dummy --rt CoverageATestPoint2: id=bltn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBLT_both:
  blt dummy --rt CoverageATestPoint2: id=bltb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBLE_taken:
  ble dummy --rt CoverageATestPoint2: id=blet, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBLE_notTaken:
  ble dummy --rt CoverageATestPoint2: id=blen, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBLE_both:
  ble dummy --rt CoverageATestPoint2: id=bleb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBGT_taken:
  bgt dummy --rt CoverageATestPoint2: id=bgtt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBGT_notTaken:
  bgt dummy --rt CoverageATestPoint2: id=bgtn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBGT_both:
  bgt dummy --rt CoverageATestPoint2: id=bgtb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBGE_taken:
  bge dummy --rt CoverageATestPoint2: id=bget, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBGE_notTaken:
  bge dummy --rt CoverageATestPoint2: id=bgen, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBGE_both:
  bge dummy --rt CoverageATestPoint2: id=bgeb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--##################  

testBN_taken:
  bn dummy --rt CoverageATestPoint2: id=bnt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBN_notTaken:
  bn dummy --rt CoverageATestPoint2: id=bnn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBN_both:
  bn dummy --rt CoverageATestPoint2: id=bnb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 

--##################  

testBP_taken:
  bp dummy --rt CoverageATestPoint2: id=bpt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBP_notTaken:
  bp dummy --rt CoverageATestPoint2: id=bpn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBP_both:
  bp dummy --rt CoverageATestPoint2: id=bpb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 

--##################  

testBNZ_taken:
  bnz dummy --rt CoverageATestPoint2: id=bnzt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBNZ_notTaken:
  bnz dummy --rt CoverageATestPoint2: id=bnzn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBNZ_both:
  bnz dummy --rt CoverageATestPoint2: id=bnzb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBZ_taken:
  bz dummy --rt CoverageATestPoint2: id=bzt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBZ_notTaken:
  bz dummy --rt CoverageATestPoint2: id=bzn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBZ_both:
  bz dummy --rt CoverageATestPoint2: id=bzb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBC_taken:
  bc dummy --rt CoverageATestPoint2: id=bct, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBC_notTaken:
  bc dummy --rt CoverageATestPoint2: id=bcn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBC_both:
  bc dummy --rt CoverageATestPoint2: id=bcb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBNC_taken:
  bnc dummy --rt CoverageATestPoint2: id=bnct, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBNC_notTaken:
  bnc dummy --rt CoverageATestPoint2: id=bncn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBNC_both:
  bnc dummy --rt CoverageATestPoint2: id=bncb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBV_taken:
  bv dummy --rt CoverageATestPoint2: id=bvt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBV_notTaken:
  bv dummy --rt CoverageATestPoint2: id=bvn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBV_both:
  bv dummy --rt CoverageATestPoint2: id=bvb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBNV_taken:
  bnv dummy --rt CoverageATestPoint2: id=bnvt, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBNV_notTaken:
  bnv dummy --rt CoverageATestPoint2: id=bnvn, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBNV_both:
  bnv dummy --rt CoverageATestPoint2: id=bnvb, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################

testBSA_taken:
  bsa dummy --rt CoverageATestPoint2: id=bsat, type=condition, trueConditions=1, falseConditions=0, bothConditions=0
  JMP[R31]  
  
testBSA_notTaken:
  bsa dummy --rt CoverageATestPoint2: id=bsan, type=condition, trueConditions=0, falseConditions=1, bothConditions=0
  JMP[R31]  

testBSA_both:
  bsa dummy --rt CoverageATestPoint2: id=bsab, type=condition, trueConditions=0, falseConditions=0, bothConditions=1
  JMP[R31] 
  
--#################
testJR:
  LD.W 4[SP],R31
  LD.W 8[SP],R8
  LD.W 12[SP],R7
  add 0xC,SP
  nop
  JMP[R31]
       
delay:
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  JMP[R31]