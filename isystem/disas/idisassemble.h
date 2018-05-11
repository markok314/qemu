/*
 * idisassemble.h
 *
 *  Created on: May 11, 2018
 *      Author: isystem
 */

#ifndef ISYSTEM_DISAS_IDISASSEMBLE_H_
#define ISYSTEM_DISAS_IDISASSEMBLE_H_

#include <stdint.h>


typedef uint8_t BYTE;
typedef int16_t WORD;
typedef uint64_t ADDROFFS;

/** Contains CPU family and variant. */
struct CCPUInfo
{
  /** Defines CPU Family IDs. */
  enum ECPUFamily // Flags2
  {
    cpu_8051            =  1,
    cpu_PowerPC         = 18,
    cpu_ARM             = 22,
    cpu_V850            = 25,
    cpu_TPU             = 34,
    cpu_TriCore         = 37,
    cpu_PCP             = 38,
    cpu_GTM             = 40,
    cpu_RX              = 41,
    cpu_SPT             = 42,

    cpu_Num,
    cpu_Generic = 0xFFFF
  };
  uint16_t m_wCPU = cpu_ARM;  ///< Contains CPU family.

  enum E8051
  {
    v8051_8051,
    v8051_8051MX,

    v8051_Num
  };
  enum EPowerPC
  {
    vPowerPC_4xx,
    vPowerPC_5xx,
    vPowerPC_6xx,
    vPowerPC_7xx,
    vPowerPC_8xx,
    vPowerPC_G5,
    vPowerPC_G3,

    vPowerPC_Num
  };
  enum EARMVariant
  {
    vARM_v3,
    vARM_v4T,
    vARM_v5TE,
    vARM_v5TEJ,
    vARM_v6,
    vARM_v7A,             // Cortex A  architecture version
    vARM_v7R,             // Cortex R  architecture version
    vARM_v7M,             // Cortex M3 architecture version
    vARM_v6M,             // Cortex M0 and M1 architecture version
    vARM_v8A,
    vARM_v8R,
    vARM_v8M,

    vARM_Num
  };
//  enum EARMSubVariant
    // encoding allows 4 sub-sub variants
    static const uint16_t svARM_CortexMask  = 0xF000;

    static const uint16_t svARM_CortexM     = 0x1000;
    static const uint16_t svARM_CortexM0    = svARM_CortexM | (0  << 2);
    static const uint16_t svARM_CortexM1    = svARM_CortexM | (1  << 2);
    static const uint16_t svARM_CortexM3    = svARM_CortexM | (3  << 2);
    static const uint16_t svARM_CortexM4    = svARM_CortexM | (4  << 2);
    static const uint16_t svARM_CortexM7    = svARM_CortexM | (7  << 2);
    static const uint16_t svARM_CortexM33   = svARM_CortexM | (33 << 2);

    static const uint16_t svARM_CortexR     = 0x2000;
    static const uint16_t svARM_CortexR4    = svARM_CortexR | (4  << 2);
    static const uint16_t svARM_CortexR5    = svARM_CortexR | (5  << 2);
    static const uint16_t svARM_CortexR7    = svARM_CortexR | (7  << 2);
    static const uint16_t svARM_CortexR52   = svARM_CortexR | (52 << 2);

    static const uint16_t svARM_CortexA     = 0x3000;
    static const uint16_t svARM_CortexA5    = svARM_CortexA | (5  << 2);
    static const uint16_t svARM_CortexA7    = svARM_CortexA | (7  << 2);
    static const uint16_t svARM_CortexA8    = svARM_CortexA | (8  << 2);
    static const uint16_t svARM_CortexA9    = svARM_CortexA | (9  << 2);
    static const uint16_t svARM_CortexA15   = svARM_CortexA | (15 << 2);
    static const uint16_t svARM_CortexA17   = svARM_CortexA | (17 << 2);
    static const uint16_t svARM_CortexA32   = svARM_CortexA | (32 << 2);
    static const uint16_t svARM_CortexA35   = svARM_CortexA | (35 << 2);
    static const uint16_t svARM_CortexA53   = svARM_CortexA | (53 << 2);
    static const uint16_t svARM_CortexA57   = svARM_CortexA | (57 << 2);
    static const uint16_t svARM_CortexA72   = svARM_CortexA | (72 << 2);
    static const uint16_t svARM_CortexA75   = svARM_CortexA | (75 << 2);

  enum EARMState
  {
/// v8
    sARMv8_EL0    = 0x0000,
    sARMv8_EL1    = 0x0001,
    sARMv8_EL2    = 0x0002,
    sARMv8_EL3    = 0x0003,
    sARMv8_ELMask = 0x0003,

    sARMv8_Secure = 0x0004,
/// v7
    sARMv7_PL0    = sARMv8_EL0,
    sARMv7_PL1    = sARMv8_EL1,
    sARMv7_PL2    = sARMv8_EL2,
    sARMv7_ELMask = sARMv8_ELMask,

    sARMv7_Secure = sARMv8_Secure,
  };

  enum EV850
  {
    vV850_V850,
    vV850_V850E3v5,

    vV850_Num
  };
  enum ETPU
  {
    veTPU,

    vTPU_Num
  };
  enum EGTM
  {
    veGTM,

    vGTM_Num
  };
  enum ETriCore
  {
    vTriCore13,
    vTriCore16,
    vTriCore161,
    vTriCore162P,

    vTriCore_Num
  };
  enum EPCP
  {
    vPCP,

    vPCP_Num
  };
  enum ERX
  {
    vRX,

    vRX_Num
  };
  enum ESPT
  {
    vSPT_v1,
    vSPT_v2,
    vSPT_v2_5,

    vSPT_Num
  };
  uint16_t m_wVariant     = 0;  ///< Contains CPU variant ID.

  uint16_t m_wSubVariant  = 0;  ///< Contains CPU subvariant ID.
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

  int32_t SetConfig(const CConfig * pConfig) = 0;
  struct CInfo
  {
    struct CInstrInfo
    {
      BYTE m_byMaxInstrMAUs;     // max number of MAUs in an instruction
      BYTE m_byInstructionAlign; // alignment of instructions for this mode
    } m_InstrInfo[mempISANum];
    BYTE m_byMinInstructionAlign; // min alignment of instructions for any mode
  };
  int32_t GetInfo(CInfo * pInfo) = 0;
  int32_t Update() = 0;
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
  int32_t Disassemble(const CParameters * pParameters, LPTSTR pszDasm, WORD * pwLength) PURE;
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
  int32_t Analyze(const CParameters * pParameters, CAnalyze * pAnalyze) PURE;
};

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


#endif /* ISYSTEM_DISAS_IDISASSEMBLE_H_ */
