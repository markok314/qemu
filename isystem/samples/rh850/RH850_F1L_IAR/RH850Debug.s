  public _restoreRegisters
  public _storeRegisters
  public _testReadRegisters
  public _testWriteRegisters
  public _testStepInst_asm
  RSEG CODE:CODE(4)
                      
_storeRegisters:
  ST.W R6,4[SP]
  ST.W R7,8[SP]
  ST.W R8,12[SP]
  ST.W R9,16[SP] 
  JMP[R31]
  
_testReadRegisters:             
  mov 0x1,R6  ;rt TestReadRegister: name=R6, value=0x1 
  mov 0x2,R7  ;rt TestReadRegister: name=R7, value=0x2
  mov 0x3,R8  ;rt TestReadRegister: name=R8, value=0x3
  mov 0x4,R9  ;rt TestReadRegister: name=R9, value=0x4
  JMP[R31]
  
_testWriteRegisters:
  NOP
  ;rt TestWriteRegister: name = R6, value=0x4, sizeInBits=32
  ;rt TestWriteRegister: name = R7, value=0x3, sizeInBits=32
  ;rt TestWriteRegister: name = R8, value=0x2, sizeInBits=32
  ;rt TestWriteRegister: name = R9, value=0x1, sizeInBits=32
  JMP[R31]

_restoreRegisters:
  LD.W 4[SP],R6
  LD.W 8[SP],R7
  LD.W 12[SP],R8
  LD.W 16[SP],R9
  JMP[R31]

_testStepOverInst:
  nop
  JMP[R31]
  
_testStepInst_asm:
  ST.W R31,4[SP] ;rt TestPoint: id=testStepInstStart, symbol=testStepInst, value=0
  jarl _testStepOverInst,R31 ;rt TestPoint: id=testStepInst, symbol=testStepInst, value=0
  LD.W 4[SP],R31 ;rt TestPoint: id=testStepOverInst, symbol=testStepInst, value=0
  JMP[R31]
  
  END

