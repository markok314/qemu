/*
   Copyright (c) 2018 iSYSTEM Labs d.o.o.
*/

// DisassemblerBase.cpp: implementation of the CDisassemblerBase class.
//
//////////////////////////////////////////////////////////////////////

#include "DisassemblerBase.h"
//#include "AsystMacros.h"
#include <string.h>	// for memset()

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/*
int CLookupItemCollection::Compare(DWORD Key1, DWORD Key2) const
{
  if (Key1<Key2)
    return -1;
  else if (Key1>Key2)
    return 1;
  return 0;
}

DWORD CLookupItemCollection::KeyOf(const SLookupItem* Item) const
{
  return Item->m_dwValue;
}
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDisassemblerBase::CDisassemblerBase(BOOL m_bDefaultBigEndian) :
m_bBigEndian(m_bDefaultBigEndian)
{
  m_bSetsCallFlag = FALSE;
  m_flSetsExtraCycles = false;
  m_pIMemoryProperty = NULL;
  ///ZERO(m_Config);
  m_byMinimumInstructionLength = 1;
  m_pParameters = new IDisassemble::CParameters;
}

CDisassemblerBase::~CDisassemblerBase()
{
  ///RELEASEINTERFACE(m_pIMemoryProperty);
}

void CDisassemblerBase::SetConfig(const IDisassemble::CConfig & rConfig)
{
  m_Config = rConfig;

  IDisassembleSink::CCallBackData CallBackData;
  CallBackData.m_eWhat = IDisassembleSink::CCallBackData::cbGetIMemoryProperty;
  m_Config.m_pIDisassembleSink->CallBack(CallBackData);
  m_pIMemoryProperty = CallBackData.m_GetIMemoryProperty.m_pIMemoryProperty;
}

void CDisassemblerBase::Update1()
{
  BOOL bCanAccessContext = m_bCanAccessContext;
  m_bCanAccessContext = TRUE;
  Update();
  m_bCanAccessContext = bCanAccessContext;
}

void CDisassemblerBase::SetParameters(const IDisassemble::CParameters * pParameters, IDisassemble::CAnalyze * pAnalyze)
{
  m_pParameters=pParameters;
  m_pAnalyze=pAnalyze;
  m_bCanAccessContext = !ISMASK(m_pParameters->m_wOptions, IDisassemble::CParameters::daNoContextAccess);
  m_bWantNextInstructionImm = ISMASK(IDisassemble::CParameters::daWantNextAddrImm, m_pParameters->m_wOptions);
  m_bWantNextInstructionAll = IDisassemble::CParameters::daWantNextAddrAll == (IDisassemble::CParameters::daWantNextAddrAll & m_pParameters->m_wOptions);
}

void CDisassemblerBase::SetExtraCycles(BYTE byExtraCycles)
{
  m_flSetsExtraCycles = true;
  m_pAnalyze->m_byExtraCycles = byExtraCycles;
}

jstring CDisassemblerBase::StandardAdrToSymbol(ADDROFFS dwAddress, int nMemoryArea, int nNumHexBytes, DWORD dwATS)
{
  return AdrMaskToSymbol(dwAddress, 1<<nMemoryArea, nNumHexBytes, dwATS);
}

jstring CDisassemblerBase::AdrMaskToSymbol(ADDROFFS dwAddress, WORD wMemoryAreaMask, int nNumHexBytes, DWORD dwATS)
{
  ADDROFFS_PRIMITIVE aP = PRIMITIVE_FROMADDROFFS(dwAddress);
  if (0==nNumHexBytes)
  {
	  nNumHexBytes=1;
	  if (aP>0x0000ffff) 
      nNumHexBytes=4;
	  else if (aP>0x000000ff) 
      nNumHexBytes=2;
  }

  jstring jstrHEX = StrF(aP); ///, nNumHexBytes);

  jstring jstrResult;
  if (0 != dwATS && ISMASK(m_pParameters->m_wOptions, IDisassemble::CParameters::daSymbols))
  {
 	  jstring string;
    if (atsAll != dwATS && !ISMASK(atsTarget, dwATS))
    {
      wMemoryAreaMask = 0;
    }
	  ///m_Config.m_pIDisassembleSink->ConvertAddressToSymbol(dwAddress, wMemoryAreaMask, string);
    	/// !!! manjka ConvertAddressToSymbol()
	  jstrResult=string;
  }
  if (jstrResult.empty())
    jstrResult=jstrHEX;
  else if (ISMASK(m_pParameters->m_wOptions, IDisassemble::CParameters::daSymAddr)) 
    jstrResult=jstrResult+" ("+jstrHEX+")";
  return jstrResult;
}

jstring CDisassemblerBase::AdrMaskToSymbol(ADDROFFS dwAddress, WORD wMemoryAreaMask)
{
  jstring jstrResult;
  if (ISMASK(m_pParameters->m_wOptions, IDisassemble::CParameters::daSymbols))
  {
	  jstring string;
	  ///m_Config.m_pIDisassembleSink->ConvertAddressToSymbol(dwAddress, wMemoryAreaMask, string);
	  jstrResult=string;
  }
  return jstrResult;
}

/*BOOL CDisassemblerBase::ReadMemory(int nMemArea, ADDROFFS dwAddress, int nCount, BYTE * pbyBuf)
{
  if (!CanAccessContext())
  {
    memset(pbyBuf, 0, nCount);
    return TRUE;
  }
  IDisassembleSink::CCallBackData CallBackData;

  CallBackData.m_eWhat=IDisassembleSink::CCallBackData::cbReadMemory;
  CallBackData.m_ReadMemory.m_adrAddress.m_byArea=nMemArea;
  /// m_adrAddress je tipa sAddress, ki je neka struktura s polji m_byArea, m_aAddress, m_byProcess, ...
  CallBackData.m_ReadMemory.m_adrAddress.m_aAddress=PRIMITIVE_FROMADDROFFS(dwAddress);
  CallBackData.m_ReadMemory.m_adrAddress.m_byProcess = 0;
  CallBackData.m_ReadMemory.m_aCount=nCount;
  CallBackData.m_ReadMemory.m_pbyBuf=pbyBuf;
  m_Config.m_pIDisassembleSink->CallBack(CallBackData);
  return CallBackData.m_ReadMemory.m_bSuccess;
}*/

QWORD CDisassemblerBase::GetRegister64(LPCSTR pszRegister)
{
  if (!CanAccessContext())
  {
    return 0;
  }
  IDisassembleSink::CCallBackData CallBackData;

  CallBackData.m_eWhat  = IDisassembleSink::CCallBackData::cbGetRegister;
  CallBackData.m_GetRegister.m_pszRegister  = pszRegister;
  m_Config.m_pIDisassembleSink->CallBack(CallBackData);
  return CallBackData.m_GetRegister.m_qwValue;
}

/*
BOOL CDisassemblerBase::GetMemoryProp1(int nProp, int nMemArea, ADDROFFS dwAddress)
{
  IDisassembleSink::CCallBackData CallBackData;

  CallBackData.m_eWhat=IDisassembleSink::CCallBackData::cbGetMemoryProperty;
  CallBackData.m_GetMemoryProperty.m_nProp=nProp;
  CallBackData.m_GetMemoryProperty.m_adrAddress.m_byArea=nMemArea;
  CallBackData.m_GetMemoryProperty.m_adrAddress.m_aAddress=PRIMITIVE_FROMADDROFFS(dwAddress);
  CallBackData.m_GetMemoryProperty.m_adrAddress.m_byProcess = 0;
  m_Config.m_pIDisassembleSink->CallBack(CallBackData);
  return CallBackData.m_GetMemoryProperty.m_bResult;
}

EEndian CDisassemblerBase::GetEndian1(int nMemArea, ADDROFFS dwAddress)
{
  IDisassembleSink::CCallBackData CallBackData;

  CallBackData.m_eWhat=IDisassembleSink::CCallBackData::cbGetEndian;
  CallBackData.m_GetEndian.m_adrAddress.m_byArea    = nMemArea;
  CallBackData.m_GetEndian.m_adrAddress.m_aAddress = PRIMITIVE_FROMADDROFFS(dwAddress);
  CallBackData.m_GetEndian.m_adrAddress.m_byProcess = 0;
  m_Config.m_pIDisassembleSink->CallBack(CallBackData);
  return CallBackData.m_GetEndian.m_eEndian;
}

ICodeCache * CDisassemblerBase::GetCodeCache(int nMemArea)
{
  IDisassembleSink::CCallBackData CallBackData;

  CallBackData.m_eWhat = IDisassembleSink::CCallBackData::cbGetCodeCache;
  CallBackData.m_GetCodeCache.m_nMemArea = nMemArea;
  m_Config.m_pIDisassembleSink->CallBack(CallBackData);
  return CallBackData.m_GetCodeCache.m_pICodeCache;
}
*/

void CDisassemblerBase::VerifyLookupMap(int nSize, const SLookupItem * pLookupItems)
{
#ifdef _DEBUG
  // verify proper sorting
  DWORD dwLastValue=pLookupItems[0].m_dwValue;
  for (int I=1; I<nSize; I++)
  {
    ASSERT(pLookupItems[I].m_dwValue>dwLastValue);
    dwLastValue=pLookupItems[I].m_dwValue;
  }
#endif
}

static int s_LookupCompare(const void *elem1, const void *elem2)
{
  DWORD dwKey=*(DWORD*)elem1;
  DWORD dwElementValue=((const SLookupItem *)elem2)->m_dwValue;
  if (dwKey<dwElementValue)
    return -1;
  else if (dwKey>dwElementValue)
    return 1;
  else
    return 0;
}

#include <stdlib.h>

LPCTSTR CDisassemblerBase::GetLookupMapItem(DWORD dwValue, int nLookupSize, const SLookupItem * pLookupItems)
{
  const SLookupItem * pItem=(const SLookupItem *)bsearch(&dwValue, pLookupItems, nLookupSize, sizeof *pLookupItems, s_LookupCompare);
  return NULL==pItem ? NULL : pItem->m_pszName;
}


void CDisassemblerBase::FormatOpCodeString(jstring & rstrOpCodeString, const jstring & rstrInstruction, const jstring & rstrOperand, int nLenInstruction)
{
  rstrOpCodeString = rstrInstruction;
  switch (m_pParameters->m_wOptions & IDisassemble::CParameters::daFormatMask)
  {
    case IDisassemble::CParameters::daFormatPadd:
      {
        Padd(rstrOpCodeString, (unsigned)nLenInstruction > (rstrInstruction.size()+1)? nLenInstruction : rstrInstruction.size()+1 );
      }
      break;
    case IDisassemble::CParameters::daFormatTab:
      rstrOpCodeString += '\t';
      break;
    default:
      //ASSERT(0);
    	//throw(0);
    	break;
  }

  rstrOpCodeString += rstrOperand;
}

int CDisassemblerBase::QuickDisasm(ADDROFFS dwAddress, const BYTE * pbyBuf, int nNumAvailBytes, int & rnInsType, ADDROFFS & rdwNextAddress, int & rnNumCycles)
{
  DWORD dwNextAddress = DWORD_FROMADDROFFS(dwAddress);
  int nRet = QuickDisasm32(DWORD_FROMADDROFFS(dwAddress), pbyBuf, nNumAvailBytes, rnInsType, dwNextAddress, rnNumCycles);
  rdwNextAddress = dwNextAddress;
  return nRet;
}

int CDisassemblerBase::QuickDisasm32(DWORD dwAddress, const BYTE * pbyBuf, int nNumAvailBytes, int & rnInsType, DWORD & rdwNextAddress, int & rnNumCycles)
{
  ///ASSERT(0);
  return 0;
}
