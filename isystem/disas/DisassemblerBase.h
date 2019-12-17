/*
   Copyright (c) 2018 iSYSTEM Labs d.o.o.
*/

// DisassemblerBase.h: interface for the CDisassemblerBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "idisassemble.h"

struct SLookupItem
{
  LPCSTR m_pszName;
  DWORD m_dwValue;
};


class CDisassemblerBase  
{
public:
  CDisassemblerBase(BOOL m_bDefaultBigEndian);
  virtual ~CDisassemblerBase();
  // quick disassemble - calc rnInsType, rdwNextAddress
  virtual int QuickDisasm(ADDROFFS dwAddress, const BYTE * pbyBuf, 
                          int nNumAvailBytes, int & rnInsType, 
                          ADDROFFS & rdwNextAddress, int & rnNumCycles);
  // string disassembly
  virtual int Disasm(const BYTE * pbyBuf, jstring & rjstrDasm, int nNumAvailBytes)=0;
  // TRUE if subroutine call or block instruction
  virtual BOOL IsStepOverInstr(const BYTE * pbyBuf)=0;
  // specify instruction align, max instr bytes
  virtual void GetInfo(IDisassemble::CInfo & rInfo)=0;
  // obtain required regs for disassembly
  void Update1();
  // new configuration
  virtual void SetConfig(const IDisassemble::CConfig & rConfig);
  // called before QuickDisasm, Disasm or IsStepOverInstr. A chance to calc endiannes, thumb etc.
  virtual void SetParameters(const IDisassemble::CParameters * pParameters, IDisassemble::CAnalyze * pAnalyze);

  // flags
  virtual BOOL SetsCallFlag() const { return m_bSetsCallFlag; }
  bool SetsExtraCycles() const { return m_flSetsExtraCycles; }

  BYTE GetMinimumInstructionLength() const { return m_byMinimumInstructionLength; }
  friend class IMemoryProperty;

  BOOL GetMemoryProp(int nProp, int nMemArea, ADDROFFS dwAddress)
  {
    if (NULL != m_pIMemoryProperty)
    {
      if (m_pIMemoryProperty->GetMemoryProp(nProp, nMemArea, dwAddress))
      {
        return TRUE;
      }
    }
    return GetMemoryProp1(nProp, nMemArea, dwAddress);
  };

protected:
  virtual int QuickDisasm32(DWORD dwAddress, const BYTE * pbyBuf, int nNumAvailBytes, int & rnInsType, DWORD & rdwNextAddress, int & rnNumCycles);
  // obtain required regs for disassembly
  virtual void Update() {};

  void SetExtraCycles(BYTE byExtraCycles);

  IMemoryProperty * m_pIMemoryProperty;
  void VerifyLookupMap(int nSize, const SLookupItem * pLookupItems);
  LPCTSTR GetLookupMapItem(DWORD dwValue, int nLookupSize, const SLookupItem * pLookupItems);

  inline DWORD GetInstr4(const BYTE * pbyBuf) { return m_bBigEndian ? MSB2DWORD(pbyBuf) : LSB2DWORD(pbyBuf); };
  inline WORD GetInstr2(const BYTE * pbyBuf) { return m_bBigEndian ? MSB2WORD(pbyBuf) : LSB2WORD(pbyBuf); };
  inline BOOL  BitIsSet(DWORD dwInst, BYTE byFrom) { return ISMASK(dwInst, 1<<byFrom); };

  enum EAddressToSymbolFlags
  {
    atsNone      = 0x00000000,
    atsAll       = 0x00000001, // compatible with old TRUE
    atsConstants = 0x00000002,
    atsTarget    = 0x00000004,
  };
  jstring StandardAdrToSymbol(ADDROFFS dwAddress, int nMemoryArea, int nNumHexBytes, DWORD dwATS = atsAll);
  jstring AdrMaskToSymbol(ADDROFFS dwAddress, WORD wMemoryAreaMask);
  jstring AdrMaskToSymbol(ADDROFFS dwAddress, WORD wMemoryAreaMask, int nNumHexBytes, DWORD dwATS = atsAll);
  IDisassemble::CConfig m_Config;
  const IDisassemble::CParameters * m_pParameters;
  
  IDisassemble::CAnalyze * m_pAnalyze;
  void SetAnalyzeFlags(WORD wFlags) { if ( m_pAnalyze ) { m_pAnalyze->m_wFlags |= wFlags; } }
  void ClearAnalyzeFlags(WORD wFlags) { if ( m_pAnalyze ) { m_pAnalyze->m_wFlags &= ~wFlags; } }
  void FlagConditional()             { SetAnalyzeFlags(IDisassemble::CAnalyze::flConditional); }
  void FlagConditionFalse()          { SetAnalyzeFlags(IDisassemble::CAnalyze::flConditionFalse); }
  void FlagIndirect()                { SetAnalyzeFlags(IDisassemble::CAnalyze::flIndirect); }
  void FlagCall()                    { SetAnalyzeFlags(IDisassemble::CAnalyze::flCall); }

  BOOL ReadMemory(int nMemArea, ADDROFFS dwAddress, int nCount, BYTE * pbyBuf);
  QWORD GetRegister64(LPCSTR pszRegister);
  DWORD GetRegister32(LPCSTR pszRegister) { return (DWORD)GetRegister64(pszRegister); }
  WORD  GetRegister16(LPCSTR pszRegister) { return (WORD) GetRegister64(pszRegister); }
  BYTE  GetRegister8 (LPCSTR pszRegister) { return (BYTE) GetRegister64(pszRegister); }


  EEndian GetEndian(int nMemArea, ADDROFFS dwAddress)
  {
    if (NULL != m_pIMemoryProperty)
    {
      return m_pIMemoryProperty->GetEndian(nMemArea, dwAddress);
    }
    else
    {
      return GetEndian1(nMemArea, dwAddress);
    }
  }

  BOOL WantNextInstructionImm() { return m_bWantNextInstructionImm; };
  BOOL WantNextInstructionAll() { return m_bWantNextInstructionAll; };
  BOOL WantConditionOutcome()   { return ISMASK(m_pParameters->m_wOptions, IDisassemble::CParameters::daWantConditionOutcome); }

  bool m_flSetsExtraCycles; // indicates that this object will directly set CAnalyze::m_byExtraCycles
  BOOL m_bSetsCallFlag; // indicates whether this object sets the CAnalyze::flCall flag
  BOOL m_bBigEndian;

  BYTE m_byMinimumInstructionLength; // minimum instr length for the current mode

  void FormatOpCodeString(jstring & rstrOpCodeString, const jstring & rstrInstruction, const jstring & rstrOperand, int nLenInstruction);

  ICodeCache * GetCodeCache(int nMemArea);
private:
  EEndian GetEndian1(int nMemArea, ADDROFFS dwAddress);
  BOOL GetMemoryProp1(int nProp, int nMemArea, ADDROFFS dwAddress); 
  BOOL CanAccessContext() { return m_bCanAccessContext; };
  BOOL m_bCanAccessContext;
  BOOL m_bWantNextInstructionImm;
  BOOL m_bWantNextInstructionAll;
};

class CDisassemblerNULL: public CDisassemblerBase
{
public:
  CDisassemblerNULL(): CDisassemblerBase(FALSE) {};
  // quick disassemble - calc rnInsType, rdwNextAddress
  int QuickDisasm(ADDROFFS dwAddress, const BYTE * pbyBuf, int nNumAvailBytes,
		  	  int & rnInsType, ADDROFFS & rdwNextAddress, int & rnNumCycles) ///override
  { 
    return 0; 
  };
  // string disassembly
  virtual int Disasm(const BYTE * pbyBuf, jstring & rjstrDasm, int nNumAvailBytes) { return 0;} ;
  // TRUE if subroutine call or block instruction
  virtual BOOL IsStepOverInstr(const BYTE * pbyBuf) { return FALSE; };
  // specify instruction align, max instr bytes
  virtual void GetInfo(IDisassemble::CInfo & rInfo) {};
  // obtain required regs for disassembly
};
