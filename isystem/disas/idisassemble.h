/*
 * idisassemble.h
 *
 *  Created on: May 11, 2018
 *      Author: isystem
 */

#pragma once

#include "disas_common.h"


class IDisassembleSink
{
public:
  IDisassembleSink();
  virtual int ConvertAddressToSymbol (ADDROFFS aAddress, DWORD dwMemAreaMask, LPTSTR pszName) = 0; // address to symbol call back
  struct CCallBackData
  {
    enum EWhat
    {
      cbReadMemory,
      cbGetRegister,
      cbGetMemoryProperty,
      cbGetEndian,
      cbGetCodeCache,
      cbGetIMemoryProperty,
    } m_eWhat;
    struct CReadMemory
    {
      SAddress    m_adrAddress;
      ADDROFFS    m_aCount;
      BYTE *      m_pbyBuf;
      BOOL        m_bSuccess;
    } m_ReadMemory;
    struct CGetRegister
    {
      LPCTSTR m_pszRegister;
      QWORD   m_qwValue;
      BOOL    m_bSuccess;
    } m_GetRegister;
    struct CGetMemoryProperty
    {
      int      m_nProp;      // property to examine
      SAddress m_adrAddress; // address to examine
      BOOL     m_bResult;    // is the property selected
    } m_GetMemoryProperty;
    struct CGetEndian
    {
      SAddress m_adrAddress; // address to examine
      EEndian  m_eEndian;    // result
    } m_GetEndian;
    struct SGetCodeCache
    {
      int m_nMemArea;      // memory area
      ICodeCache * m_pICodeCache; // pointer to code cache interface
    } m_GetCodeCache;
    struct SGetIMemoryProperty
    {
      IMemoryProperty * m_pIMemoryProperty;
    } m_GetIMemoryProperty;
  };
  virtual int CallBack (CCallBackData & rCallBackData) = 0;  // call back
};


    struct CParameters
    {
        enum { MAX_DASM_LEN = 232 };
        enum EOptions
        {
        daSymAddr         = 0x0100,
        daSymbols         = 0x0200,
        daWantConditionOutcome = 0x0400,// request info on whether the condition passes. requires context access
        daWantNextAddrImm = 0x1000,  // when set then next address must be established for instructions encoding the target immediately
        daWantNextAddrAll = 0x3000,  // when set then next address must be established for all instructions
        daConditionPassed = 0x4000,  // conditional instruction executed - use only in conjunction with daNoContextAccess
        daNoContextAccess = 0x8000,  // register and memory context is not available
        
        daNumMAUsBeforeBuf= 0x000F,  // mask for number of MAUs available before m_pbyBuf

        daFormatMask      = 0x0030,
        daFormatPadd      = 0x0000,  // formated op-code string is padded with spaces between op-code and arguments
        daFormatTab       = 0x0010,  // formated op-code string uses TAB to separate op-code and arguments
        };
        WORD   m_wOptions;  // disassembly options
        WORD   m_wNumMAUs;  // number of MAUs available in m_pbyBuf
        ADDROFFS m_aAddress; 
        const BYTE * m_pbyBuf;  // pointer to where the dis. buffer is located
    };

    
class IDisassemble
{
public:
  struct CConfig
  {
    CCPUInfo m_CPUInfo;
    BYTE     m_byAddressLength;             // how many bytes of an address should be considered to form an address string
    IDisassembleSink * m_pIDisassembleSink; // Disassemble call back interface
    enum EFlags
    {
      flUseRegisterAliases = 0x00000001,    // use alternate register names
    };
    uint32_t    m_dwFlags;
  };

  virtual int32_t SetConfig(const CConfig * pConfig);// = 0;
  struct CInfo
  {
    struct CInstrInfo
    {
      BYTE m_byMaxInstrMAUs;     // max number of MAUs in an instruction
      BYTE m_byInstructionAlign; // alignment of instructions for this mode
    } m_InstrInfo[mempISANum];
    BYTE m_byMinInstructionAlign; // min alignment of instructions for any mode
  };
  virtual int32_t GetInfo(CInfo * pInfo);// = 0;
  virtual int32_t Update();// = 0;
  struct CParameters
  {
    CParameters(): m_wOptions(0), m_aAddress(0) {}
    enum { MAX_DASM_LEN = 232 };
    enum EOptions
    {
      daSymAddr         = 0x0100,
      daSymbols         = 0x0200,
      daWantConditionOutcome = 0x0400,// request info on whether the condition passes. requires context access
      daWantNextAddrImm = 0x1000,  // when set then next address must be established for instructions encoding the target immediately
      daWantNextAddrAll = 0x3000,  // when set then next address must be established for all instructions
      daConditionPassed = 0x4000,  // conditional instruction executed - use only in conjunction with daNoContextAccess
      daNoContextAccess = 0x8000,  // register and memory context is not available

      daNumMAUsBeforeBuf= 0x000F,  // mask for number of MAUs available before m_pbyBuf

      daFormatMask      = 0x0030,
      daFormatPadd      = 0x0000,  // formated op-code string is padded with spaces between op-code and arguments
      daFormatTab       = 0x0010,  // formated op-code string uses TAB to separate op-code and arguments
    };
    WORD   m_wOptions; /// // disassemly options
    WORD   m_wNumMAUs;  // number of MAUs available in m_pbyBuf
    ADDROFFS m_aAddress; ///
    const BYTE * m_pbyBuf;  // pointer to where the dis. buffer is located
  };
  virtual int32_t Disassemble(const CParameters * pParameters, LPTSTR pszDasm, WORD * pwLength);// = 0;
  struct CAnalyze
  {
    enum EFlags
    {
      flWrite               = 0x0001,   // low WORD of m_dwExtra contains the size mem access in MAUs
      flRead                = 0x0002,   // low WORD of m_dwExtra contains the size mem access in MAUs
      flIndirect            = 0x0004,   // instruction is indirect jump(PC can not be determined from op-code only).
      flRelative            = 0x0008,   // the direct branch is using relative displacement
      flCall                = 0x0010,   // in conjunction with it_Jmp, this indicates that it's a subroutine call
      flConditional         = 0x0020,   // conditional instruction, sequential, jump,...
      flConditionFalse      = 0x0040,   // instruction condition is false - valid when daWantConditionPass requested
      flAltISA              = 0x0080,   // alternate instruction set
      // nexus trace
      flOCT_Mask            = 0x0F00,   // OCT message mask
      flOCT_DBM             = 0x0100,   // instruction generates a Nexus DBM
      flOCT_IBM             = 0x0200,   // instruction generates a Nexus IBM
      flOCT_HIST            = 0x0400,   // instruction generates entry into nexus HIST field
      flOCT_HIST2           = 0x0800,   // instruction generates two entries into nexus HIST field (flOCT_HIST must also be set)
      // cortex PTM
      flPTM_Waypoint        = 0x0100,   // this instruction is waypoint instruction (see "CoreSight Program Flow Trace Architecture Specification")
      flPTM_ReturnStack     = 0x0200,   // this instruction is bl instruction which should push return address to return stack
      // cortex ETM4
      flETM4_Atom           = 0x0400,   // this instruction generates ETM4 Atom packet
      flETM4_AtomW          = 0x0800,   // this instruction generates ETM4 Atom packet when data trace is on for writes
      flETM4_AtomR          = 0x1000,   // this instruction generates ETM4 Atom packet when data trace is on for reads
      flETM4_Wait           = 0x2000,   // entry into low power mode WFI or WFE
      // TPU
      fl_TPU_Flush          = 0x1000,   // pipe is flushed (DBM is not used for tpu -> reuse flag)
      fl_TPU_Flush_Valid    = 0x2000,   // this instruction has flush field valid
      // powerpc
      fl_PPC_mflr           = 0x1000,   // mflr instruction - used for detection of leaf functions in call stack
      // TriCore
      fl_TC_MCDS_Direct     = 0x0100,   // direct branch
      fl_TC_MCDS_Indirect   = 0x0200,   // indirect branch
      fl_TC_FastRet         = 0x0400,   // fast ret
    };
    WORD m_wFlags;
    BYTE m_byExMemPropMask; // mask of 1<<ExMemProperty that the instr starts
    BYTE m_byLength; // length of instruction
    BYTE m_byMemAccessSizeMAUs; // for flWrite and flRead
    BYTE m_byConditionPropagateCnt; // how many subsequent instructions are conditional due to current instruction.
    BYTE m_byMinimumInstructionLength; // minimum instruction length for this mode.
    BYTE m_byExtraCycles;      // number of cycles to execute - 1 (i.e. default = 0
    ADDROFFS m_aNextInstruction; // next instruction
    uint32_t m_dwInstructionType;  // loword(common types) hiword(special types)
  };
  virtual int32_t Analyze(const CParameters * pParameters, CAnalyze * pAnalyze);// = 0;
};


#define LOWORD(a) ((WORD)(a))
#define HIWORD(a) ((WORD)(((DWORD)(a) >> 16) & 0xFFFF))

#define MAKELONG(a, b)   ((unsigned long) (((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))

#define MAKE_INST_TYPE2(TYPE1, TYPE2)         MAKELONG(TYPE1, TYPE2)
#define SET_INST_TYPE(VAR, TYPE)          VAR=MAKELONG(TYPE, HIWORD(VAR))
#define SET_INST_TYPE2(VAR, TYPE1, TYPE2) VAR=MAKE_INST_TYPE2(TYPE1, TYPE2)
#define SET_INST_TYPE_CPU(VAR, TYPE)      VAR=MAKELONG(LOWORD(VAR), TYPE)

#define GET_INST_TYPE(VAL)     (LOWORD(VAL))
#define GET_INST_TYPE_CPU(VAL) (HIWORD(VAL))

// general instruction types
enum EINST_TYPE
{
  it_Unknown,
  it_Continue,
  it_Jmp,
  it_CondJmp,
  it_Ret,
  it_Reti,
  it_Retf,
  it_Trap,
  it_Step,
};
