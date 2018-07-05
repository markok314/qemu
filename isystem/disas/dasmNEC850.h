/*
   Copyright (c) 2018 iSYSTEM Labs d.o.o.
*/

#ifndef __DasmNEC850_h__
#define __DasmNEC850_h__

#include "DisassemblerBase.h"
///#include "AsystMacros.h"

typedef uint32_t u32;

typedef const BYTE *PCBYTE;

class CDisassemblerNEC850: public CDisassemblerBase  
{
public:
  CDisassemblerNEC850();
  int QuickDisasm32(DWORD dwAddress, const BYTE * pbyBuf, int nNumAvailBytes, int & rnInsType,
		  	  DWORD & rdwNextAddress, int & rnNumCycles); /// override;
  //virtual
  int Disasm(const BYTE * pbyBuf, jstring & rjstrDasm, int nNumAvailBytes);
  virtual BOOL IsStepOverInstr(const BYTE * pbyBuf);
  virtual void GetInfo(IDisassemble::CInfo & rInfo);
private:
  enum
  {
	  necRegReg,						// 1
	  necImmReg,						// 2
	  necCondBranch,				// 3
	  nec16bitLdSt,					// 4
	  necJump,							// 5
	  nec3Op,								// 6
	  nec32LdSt,						// 7
	  necBitManipulation,		// 8
	  necExtended1,					// 9
	  necExtended2,					// 10
	  necExtended3,					// 11
	  necExtended4,					// 12
	  necStackManipulation,	// 13

	  necError
  };
  void NecGetInst(PCBYTE& Buff, DWORD& dwInst, DWORD& dwInst1);
//   u32 NecExtractBits(const u32 dwInst, BYTE byFrom, BYTE byTo) const;
  BOOL NecBitIsSet(const u32 dwInst, BYTE byFrom) const;
  jstring NecGetCondCodeStr(const u32 dwCode);
	jstring NecGetFPCondCodeStr(const u32 dwCode);
  jstring NecGetReg(u32 dwReg);
  jstring NecGetSIMDReg(u32 dwReg);
  jstring NecGetSysReg(u32 dwSysReg, u32 dwSelID);
  jstring NecGetImm9(const u32 dwInst);
  jstring NecGetImm5(const u32 dwInst);
	jstring NecGetImm5_11(const u32 dwInst);
	jstring NecGetImm5_18(const u32 dwInst);
	jstring NecGetImm4(const u32 dwInst);
  jstring NecGetImm32(const u32 dwImm);
  jstring NecGetImm6(const u32 dwInst);
  jstring NecGetDisp22(const u32 dwInst);
	jstring NecGetDisp23(const u32 dwInst, const u32 dwInst1);
	jstring NecGetDisp23Z0(const u32 dwInst, const u32 dwInst1);
	jstring NecGetDisp32(const u32 dwInst, const u32 dwInst1);
  jstring NecGetDisp5(const u32 dwInst);
  jstring NecGetDisp9(const u32 dwInst);
  jstring NecGetDisp15(const u32 dwInst);
  jstring NecGetImmN16(const u32 dwInst);
  jstring NecGetDisp16(const u32 dwInst);
  jstring NecGetDisp17(const u32 dwInst);


  jstring GetBitManOperand(const u32 dwInst);
  jstring NecGetRegListItem(jstring strList, u32 dwReg);
  jstring NecGetRegList(const u32 dwInst);
  jstring NecGetPrepareOp(const u32 dwInst, const u32 dwInst1, int& iSize);
  jstring GetPrefetchStr(const u32 dwInst);
  jstring GetCacheOpInst(const u32 dwInst);
  int DissNEC850(BOOL bQD, const u32 dwInst, const u32 dwInst1, int& InsType, u32& dwNextAddress, jstring& Instruction, jstring& Operand);
  int DisassmNEC850All(BOOL bQD, PCBYTE Buff, int Bytes, int& InsType, u32& dwNextAddress, int& NumCycles, jstring & rjstrDasm);
  DWORD GetNumRegList(DWORD dwInst) const;

  DWORD m_dwExtra;
};


#endif
