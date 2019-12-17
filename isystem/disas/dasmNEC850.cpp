/*
   Copyright (c) 2018 iSYSTEM Labs d.o.o.
*/

///#include "comtypes.h"
///#include "sets.h"
#include "dasmNEC850.h"
///#include "memoryareas.h"
///#include "..\kernel\SharedDefs_V850.h"
#include "wrap.h"

#define VECTOR_RELOC_BASE     0xFFFF0000
#define INVALID_INSTR_HANDLER 0x00000004
#define SET_STR(bCond, strVar, strValue) {if (!bCond) strVar = strValue;}

#define maPhysicalV850 32 ///???

#define NecAdrToSymbol(A) StandardAdrToSymbol(A, maPhysicalV850, 4)

CDisassemblerNEC850 *p = NULL;

void disasm_wrap(char *buf, size_t buflen, uint64_t pc, uint64_t inst, int insLen)
{
	jstring rjstrDasm;
	BYTE instr[8];

	if (!p)
		p = new CDisassemblerNEC850();

	for ( int i=0; i<8; i++)
		instr[i] = 0;
	for ( int i=0; i<insLen; i++)
		instr[i] = (inst >> (8*i)) & 0x0FF;

	p->Disasm( instr, rjstrDasm, buflen);
	//printf("%s\n", rjstrDasm.c_str());
	strcpy( buf, rjstrDasm.c_str());

}

void SignExtend(LONG & lDisp, uint8_t n)
{
	if ( lDisp & (1 << (n-1)) )	// sign
		lDisp += ((1 << (64 - n)) - 1) << n;
}


jstring Hex(LONG intValue, int numOfDigits)
{
	if(intValue<0){
		intValue = intValue^0xffffffff;
		intValue += 1;

		intValue = 0xffffffff-intValue;
		intValue++;
	}

	int remainder=0;
	char hexStr[12]="";
	char outStr[12]="";
	jstring output;

	int counter=0;
	while(intValue>0){
		remainder = intValue%16;

		switch(remainder){
		case 0:
					hexStr[counter]='0'; break;
		case 1:
					hexStr[counter]='1'; break;
		case 2:
					hexStr[counter]='2'; break;
		case 3:
					hexStr[counter]='3'; break;
		case 4:
					hexStr[counter]='4'; break;
		case 5:
					hexStr[counter]='5'; break;
		case 6:
					hexStr[counter]='6'; break;
		case 7:
					hexStr[counter]='7'; break;
		case 8:
					hexStr[counter]='8'; break;
		case 9:
					hexStr[counter]='9'; break;
		case 10:
					hexStr[counter]='a'; break;
		case 11:
					hexStr[counter]='b'; break;
		case 12:
					hexStr[counter]='c'; break;
		case 13:
					hexStr[counter]='d'; break;
		case 14:
					hexStr[counter]='e'; break;
		case 15:
					hexStr[counter]='f'; break;
		}
		intValue = intValue/16;
		counter++;
	}
	for(int i=0;i<numOfDigits-counter;i++){
		outStr[i] = '0';
	}

	for(int i=0;i<counter;i++){
		outStr[i+(numOfDigits-counter)] = hexStr[counter-1-i];
	}
	output = outStr;



	return output;
}

CDisassemblerNEC850::CDisassemblerNEC850(): CDisassemblerBase(FALSE)
{
  m_byMinimumInstructionLength = 2;
  m_bSetsCallFlag = TRUE;
}

//#define NecExtractBits(INST, FROM, TO) GETBITS_DWORD(INST, FROM, (TO - FROM))
#ifndef NecExtractBits
inline u32 NecExtractBits(const u32 & dwInst, BYTE byFrom, BYTE byTo) ///const
{
  u32 Mask, Code, ShiftedCode;

  ShiftedCode = dwInst >> byFrom;
  Mask = 0xffffffff >> (byFrom-byTo);
  Code = ShiftedCode & Mask;

  return Code;
}
#endif 

BOOL CDisassemblerNEC850::IsStepOverInstr(const BYTE * pbyBuf)
{
	BOOL bIsStepOver = FALSE;

	u32 dwInst  = 0;
	u32 dwInst1 =	0;
	NecGetInst(pbyBuf, dwInst, dwInst1);

	DWORD dwForm1 = NecExtractBits(dwInst, 5, 32);
  switch(dwForm1)
  {
    case 0x008003F: // TRAP
    case 0x7C1903F: // LDTC.PC reg2
		  bIsStepOver = TRUE; 
		  return bIsStepOver;
  }

	dwForm1 = NecExtractBits(dwInst,  4, 11);
	DWORD dwForm2 = NecExtractBits(dwInst, 16, 32);
	if ((dwForm1 == 0x7F) && (dwForm2 == 0x0000))
	{
		bIsStepOver = TRUE; // RIE = reserved instruction exception, jumps to 0x00000030
		return bIsStepOver;
	}

  dwForm1 = NecExtractBits(dwInst, 5, 27);
  if (dwForm1 == 0x0B063F) 
  {
    bIsStepOver = TRUE; // JARL
		return bIsStepOver;
  }

	dwForm1 = NecExtractBits(dwInst, 0, 16);
	if (dwForm1 == 0x0040)
	{
		bIsStepOver = TRUE; // RIE = reserved instruction exception, jumps to 0x00000030
		return bIsStepOver;
	}

	dwForm1 = NecExtractBits(dwInst, 6, 16);
	if (dwForm1 == 0x008)
	{
		bIsStepOver = TRUE; // CALLT
		return bIsStepOver;
  }

	dwForm1 = NecExtractBits(dwInst, 0, 11) | (NecExtractBits(dwInst, 15, 16) << 11);
  if (dwForm1 == 0x040)
	{
		bIsStepOver = TRUE; // FETRAP vector4 = FE level software exception, jumps to 0x00000030
		return bIsStepOver;
	}

	dwForm1 = NecExtractBits(dwInst,  6, 11);
	DWORD dwReg = NecExtractBits(dwInst,  11, 16);  // if 0 the instruction is JR - jump relative
	if ((dwForm1 == 0x1E) && !NecBitIsSet(dwInst, 16) && dwReg != 0)
	{
		bIsStepOver = TRUE; // JARL = jump with link register = call
		return bIsStepOver;
  }

	dwForm1 = NecExtractBits(dwInst, 5, 16);
	if ((dwForm1 == 0x017) && (NecExtractBits(dwInst, 0, 5) != 0x0))
	{
	  bIsStepOver = TRUE; // JARL = jump with link register = call
	  return bIsStepOver;
	}
  if ((dwForm1 == 0x037) &&	(NecBitIsSet(dwInst, 16)))
  {
	  bIsStepOver = TRUE; // LOOP
	  return bIsStepOver;
	}
	
  return bIsStepOver;
}

void CDisassemblerNEC850::GetInfo(IDisassemble::CInfo & rInfo)
{
  rInfo.m_InstrInfo[0].m_byInstructionAlign = 2; 
  rInfo.m_InstrInfo[0].m_byMaxInstrMAUs = 4; 
}

int CDisassemblerNEC850::Disasm(const BYTE * pbyBuf, jstring & rjstrDasm, int nNumAvailBytes)
{
	int iBytes = nNumAvailBytes;
	int iInsType = 0;
	u32 dwNextAddress = 0;
	int iNumCycles = 0;

	return DisassmNEC850All(FALSE, pbyBuf, iBytes, iInsType, dwNextAddress, iNumCycles, rjstrDasm);
}

int CDisassemblerNEC850::QuickDisasm32(DWORD dwAddress, const BYTE * pbyBuf, int nNumAvailBytes, int & rnInsType, DWORD & rdwNextAddress, int & rnNumCycles)
{
	jstring strDasm;

	return DisassmNEC850All(TRUE, pbyBuf, nNumAvailBytes, rnInsType, rdwNextAddress, rnNumCycles, strDasm);
}



void CDisassemblerNEC850::NecGetInst(PCBYTE& Buff, DWORD& dwInst, DWORD& dwInst1)
{
	dwInst = ((DWORD)((BYTE)(Buff[0])) | 
					 (DWORD)(((BYTE)(Buff[1])) << 8) | 
					 (DWORD)(((BYTE)(Buff[2])) << 16) | 
					 (DWORD)(((BYTE)(Buff[3])) << 24) );

	dwInst1 = ((DWORD)((BYTE)(Buff[4])) | 
					 (DWORD)(((BYTE)(Buff[5])) << 8) | 
					 (DWORD)(((BYTE)(Buff[6])) << 16) | 
					 (DWORD)(((BYTE)(Buff[7])) << 24) );
}

BOOL CDisassemblerNEC850::NecBitIsSet(const u32 dwInst, BYTE byFrom) const
{
	BYTE byTo = byFrom+1;
	u32 bit = NecExtractBits(dwInst, byFrom, byTo);
	return BOOL(bit);
}

jstring CDisassemblerNEC850::NecGetCondCodeStr(const u32 dwCode)
{
	jstring strCode;
	switch(dwCode)
	{
		case 0x0: strCode = "V";  break;
		case 0x1: strCode = "C";  break;
		case 0x2: strCode = "Z";  break;
		case 0x3: strCode = "NH"; break;
		case 0x4: strCode = "N";  break;
		case 0x5: strCode = "R";  break;
		case 0x6: strCode = "LT"; break;
		case 0x7: strCode = "LE"; break;
		case 0x8: strCode = "NV"; break;
		case 0x9: strCode = "NC"; break;
		case 0xA: strCode = "NZ"; break;
		case 0xB: strCode = "H";  break;
		case 0xC: strCode = "P";  break;
		case 0xD: strCode = "SA"; break;
		case 0xE: strCode = "GE"; break;
		case 0xF: strCode = "GT"; break;
	}
	return strCode;
}

jstring CDisassemblerNEC850::NecGetFPCondCodeStr(const u32 dwCode)
{
	jstring strCode;
	switch(dwCode)
	{
		case 0x0: strCode = "F";   break;
		case 0x1: strCode = "UN";  break;
		case 0x2: strCode = "EQ";  break;
		case 0x3: strCode = "UEQ"; break;
		case 0x4: strCode = "OLT"; break;
		case 0x5: strCode = "ULT"; break;
		case 0x6: strCode = "OLE"; break;
		case 0x7: strCode = "ULE"; break;
		case 0x8: strCode = "SF";  break;
		case 0x9: strCode = "NGLE";break;
		case 0xA: strCode = "SEQ"; break;
		case 0xB: strCode = "NGL"; break;
		case 0xC: strCode = "LT";  break;
		case 0xD: strCode = "NGE"; break;
		case 0xE: strCode = "LE";  break;
		case 0xF: strCode = "NGT"; break;
	}
	return strCode;
}

jstring CDisassemblerNEC850::NecGetReg(u32 dwReg)
{
	/*jstring strReg;
	switch(dwReg)
	{
	case 0x03: strReg = "SP"; break;
	case 0x04: strReg = "GP"; break;
	case 0x05: strReg = "TP"; break;
	case 0x1E: strReg = "EP"; break;
	case 0x1F: strReg = "LP"; break;
	default  : strReg = "R"+StrF(dwReg); break;
	}
	return strReg;*/
  return "R"+Long2Str(dwReg);
}

jstring CDisassemblerNEC850::NecGetSIMDReg(u32 dwReg)
{	
  return "VR"+Long2Str(dwReg);
}


jstring CDisassemblerNEC850::NecGetSysReg(u32 dwSysReg, u32 dwSelID)
{
	jstring strSysReg;
  switch(dwSelID) // V850E3v5 = RH850, additional opcode field
  {
  case 0: ///esidException: // 0 - default for V850E2v4 = Fx4 and older, a bit different than RH850
    switch(dwSysReg)
    {
    case 0x00:  strSysReg = "EIPC"; break;
    case 0x01:	strSysReg = "EIPSW"; break;
    case 0x02:	strSysReg = "FEPC"; break;
    case 0x03:	strSysReg = "FEPSW"; break;
    case 0x04:	strSysReg = "ECR"; break;
    case 0x05:	strSysReg = "PSW"; break;

    case 0x06:  strSysReg = "FPSR"; break;   // undisclosed to users?
    case 0x07:  strSysReg = "FPEPC"; break;   // undisclosed to users?
    case 0x08:  strSysReg = "FPST"; break;  // reserved for future use
    case 0x09:  strSysReg = "FPCC"; break;  // reserved for future use
    case 0x0A:  strSysReg = "FPCFG"; break; // reserved for future use
    case 0x0B:  strSysReg = "FPEC"; break;
    case 0x0C:  strSysReg = "SCBP"; break;
    case 0x0D:  strSysReg = "EIIC"; break;
    case 0x0E:  strSysReg = "FEIC"; break;
    case 0x0F:  strSysReg = "DBIC"; break;

    case 0x10:	strSysReg = "CTPC"; break;
    case 0x11:	strSysReg = "CTPSW"; break;   
    case 0x12:	strSysReg = "DBPC"; break;  
    case 0x13:	strSysReg = "DBPSW"; break;  
    case 0x14:	strSysReg = "CTBP"; break;  
    case 0x15:	strSysReg = "DIR"; break;  
    case 0x16:	strSysReg = "BPCn"; break;  
    case 0x17:	strSysReg = "ASID"; break;                              
    case 0x18:	strSysReg = "BPAVn"; break; 
    case 0x19:	strSysReg = "BPAMn"; break; 
    case 0x1A:	strSysReg = "BPDVn"; break; 
    case 0x1B:	strSysReg = "BPDMn"; break; 

    case 0x1C:  strSysReg = "EIWR"; break;
    case 0x1D:  strSysReg = "FEWR"; break;
    case 0x1E:  strSysReg = "DBWR"; break;
    case 0x1F:  strSysReg = "BSEL"; break;
    default: strSysReg = "SR"+Long2Str(dwSysReg); break;
    }
    break;
  case 1: ///esidConfiguration: // 1
    switch(dwSysReg)
    {
    case 0x00:  strSysReg = "MCFG0"; break;
    case 0x01:	strSysReg = "MCFG1"; break;
    case 0x02:	strSysReg = "RBASE"; break;
    case 0x03:	strSysReg = "EBASE"; break;
    case 0x04:	strSysReg = "INTBP"; break;
    case 0x05:	strSysReg = "MCTL"; break;
    case 0x06:  strSysReg = "PID"; break;
    case 0x07:  strSysReg = "FPIPR"; break;
    case 0x0A:  strSysReg = "TCSEL"; break;
    case 0x0B:  strSysReg = "SCCFG"; break;
    case 0x0C:  strSysReg = "SCBP"; break;
    case 0x0D:  strSysReg = "HVCCFG"; break;
    case 0x0E:  strSysReg = "HVCBP"; break;
    case 0x0F:  strSysReg = "VCSEL"; break;
    case 0x10:	strSysReg = "VMPRT0"; break;
    case 0x11:	strSysReg = "VMPRT1"; break;   
    case 0x12:	strSysReg = "VMPRT2"; break;  
    case 0x17:	strSysReg = "VMSCCTL"; break;                              
    case 0x18:	strSysReg = "VMSCTBL0"; break; 
    case 0x19:	strSysReg = "VMSCTBL1"; break; 
    case 0x1A:	strSysReg = "VMSCTBL2"; break; 
    case 0x1B:	strSysReg = "VMSCTBL3"; break; 
    default: strSysReg = "SR"+Long2Str(dwSysReg); break;
    }
    break;
  case 2: ///esidMemory: // 2
    switch(dwSysReg)
    {
    case 0x00:  strSysReg = "HTCFG0"; break;
    case 0x05:	strSysReg = "HTCTL"; break;
    case 0x06:  strSysReg = "MEA"; break;
    case 0x07:  strSysReg = "ASID"; break;
    case 0x08:  strSysReg = "MEI"; break;
    case 0x0A:  strSysReg = "ISPR"; break;
    case 0x0B:  strSysReg = "PMR"; break;
    case 0x0C:  strSysReg = "ICSR"; break;
    case 0x0D:  strSysReg = "INTCFG"; break;
    case 0x10:	strSysReg = "TLBSCH"; break;
    case 0x17:	strSysReg = "HTSCCTL"; break;                              
    case 0x18:	strSysReg = "HTSCTBL0"; break; 
    case 0x19:	strSysReg = "HTSCTBL1"; break; 
    case 0x1A:	strSysReg = "HTSCTBL2"; break; 
    case 0x1B:	strSysReg = "HTSCTBL3"; break; 
    case 0x1C:	strSysReg = "HTSCTBL4"; break; 
    case 0x1D:	strSysReg = "HTSCTBL5"; break; 
    case 0x1E:	strSysReg = "HTSCTBL6"; break; 
    case 0x1F:	strSysReg = "HTSCTBL7"; break; 
    default: strSysReg = "SR"+Long2Str(dwSysReg); break;
    }
    break;
  case 3: ///esidDebug: // 3
    switch(dwSysReg)
    {
    case 0x00:  strSysReg = "VMSCPTR"; break;
    case 0x01:	strSysReg = "HTSCPTR"; break;

    case 0x0F:  strSysReg = "DBIC"; break;

    case 0x11:	strSysReg = "DBCMC"; break;   
    case 0x12:	strSysReg = "DBPC"; break;  
    case 0x13:	strSysReg = "DBPSW"; break;  
    case 0x14:	strSysReg = "DIR0"; break;  
    case 0x15:	strSysReg = "DIR1"; break;  
    case 0x16:	strSysReg = "BPCn"; break;  
    case 0x18:	strSysReg = "BPAVn"; break; 
    case 0x19:	strSysReg = "BPAMn"; break; 
    case 0x1A:	strSysReg = "BPDVn"; break; 
    case 0x1B:	strSysReg = "BPDMn"; break; 

    case 0x1E:  strSysReg = "DBWR"; break;
    default: strSysReg = "SR"+Long2Str(dwSysReg); break;
    }
    break;
  case 4: ///esidMMU: //4
    switch(dwSysReg)
    {
      case 0x00: strSysReg = "TLBIDX"; break;
      case 0x04: strSysReg = "TELO0";  break;
      case 0x05: strSysReg = "TELO1";  break;
      case 0x06: strSysReg = "TEHI0";  break;
      case 0x07: strSysReg = "TEHI1";  break;
      case 0x0A: strSysReg = "TLBCFG"; break;
      case 0x0C: strSysReg = "BWERRL"; break;
      case 0x0D: strSysReg = "BWERRH"; break;
      case 0x0E: strSysReg = "BRERRL"; break;
      case 0x0F: strSysReg = "BRERRH"; break;
      case 0x10: strSysReg = "ICTAGL"; break;
      case 0x11: strSysReg = "ICTAGH"; break;
      case 0x12: strSysReg = "ICDATL"; break;
      case 0x13: strSysReg = "ICDATH"; break;
      case 0x14: strSysReg = "DCTAGL"; break;
      case 0x15: strSysReg = "DCTAGH"; break;
      case 0x16: strSysReg = "DCDATL"; break;
      case 0x17: strSysReg = "DCDATH"; break;
      case 0x18: strSysReg = "ICCTRL"; break;
      case 0x19: strSysReg = "DCCTRL"; break;
      case 0x1A: strSysReg = "ICCFG";  break;
      case 0x1B: strSysReg = "DCCFG";  break;
      case 0x1C: strSysReg = "ICERR";  break;
      case 0x1D: strSysReg = "DCERR";  break;
      default: strSysReg = "SR"+Long2Str(dwSysReg); break;
    }
    break;
  case 5: ///esidMPU0: //5
    switch(dwSysReg)
    {
      case 0x00: strSysReg = "MPM";    break;
      case 0x01: strSysReg = "MPRC";   break;
      case 0x04: strSysReg = "MPBRGN"; break;
      case 0x05: strSysReg = "MPTRGN"; break;
      case 0x08: strSysReg = "MCA";    break;
      case 0x09: strSysReg = "MCS";    break;
      case 0x0A: strSysReg = "MCC";    break;
      case 0x0C: strSysReg = "MCR";    break;
      case 0x14: strSysReg = "MPPRT0"; break;
      case 0x15: strSysReg = "MPPRT1"; break;
      case 0x16: strSysReg = "MPPRT2"; break;     
      default: strSysReg = "SR"+Long2Str(dwSysReg); break;
    }
    break;
  case 6: ///esidMPU1: //6
    switch(dwSysReg)
    {
      case 0x00: strSysReg = "MPLA0"; break;
      case 0x01: strSysReg = "MPUA0"; break;
      case 0x02: strSysReg = "MPAT0"; break;

      case 0x04: strSysReg = "MPLA1"; break;
      case 0x05: strSysReg = "MPUA1"; break;
      case 0x06: strSysReg = "MPAT1"; break;

      case 0x08: strSysReg = "MPLA2"; break;
      case 0x09: strSysReg = "MPUA2"; break;
      case 0x0A: strSysReg = "MPAT2"; break;

      case 0x0C: strSysReg = "MPLA3"; break;
      case 0x0D: strSysReg = "MPUA3"; break;
      case 0x0E: strSysReg = "MPAT3"; break;

      case 0x10: strSysReg = "MPLA4"; break;
      case 0x11: strSysReg = "MPUA4"; break;
      case 0x12: strSysReg = "MPAT4"; break;

      case 0x14: strSysReg = "MPLA5"; break;
      case 0x15: strSysReg = "MPUA5"; break;
      case 0x16: strSysReg = "MPAT5"; break;

      case 0x18: strSysReg = "MPLA6"; break;
      case 0x19: strSysReg = "MPUA6"; break;
      case 0x1A: strSysReg = "MPAT6"; break;

      case 0x1C: strSysReg = "MPLA7"; break;
      case 0x1D: strSysReg = "MPUA7"; break;
      case 0x1E: strSysReg = "MPAT7"; break;
      default: strSysReg = "SR" + Long2Str(dwSysReg); break;
      ///default: strSysReg = "SR" + ToString(dwSysReg); break;
    }
    break;
  case 7: ///esidMPU2: //7
    switch(dwSysReg)
    {
      case 0x00: strSysReg = "MPLA8"; break;
      case 0x01: strSysReg = "MPUA8"; break;
      case 0x02: strSysReg = "MPAT8"; break;

      case 0x04: strSysReg = "MPLA9"; break;
      case 0x05: strSysReg = "MPUA9"; break;
      case 0x06: strSysReg = "MPAT9"; break;

      case 0x08: strSysReg = "MPLA10"; break;
      case 0x09: strSysReg = "MPUA10"; break;
      case 0x0A: strSysReg = "MPAT10"; break;

      case 0x0C: strSysReg = "MPLA11"; break;
      case 0x0D: strSysReg = "MPUA11"; break;
      case 0x0E: strSysReg = "MPAT11"; break;

      case 0x10: strSysReg = "MPLA12"; break;
      case 0x11: strSysReg = "MPUA12"; break;
      case 0x12: strSysReg = "MPAT12"; break;

      case 0x14: strSysReg = "MPLA13"; break;
      case 0x15: strSysReg = "MPUA13"; break;
      case 0x16: strSysReg = "MPAT13"; break;

      case 0x18: strSysReg = "MPLA14"; break;
      case 0x19: strSysReg = "MPUA14"; break;
      case 0x1A: strSysReg = "MPAT14"; break;

      case 0x1C: strSysReg = "MPLA15"; break;
      case 0x1D: strSysReg = "MPUA15"; break;
      case 0x1E: strSysReg = "MPAT15"; break;
      default: strSysReg = "SR"+Long2Str(dwSysReg); break;
    }
    break;
  default:
    strSysReg = "SR"+Long2Str(dwSysReg);
    break;
  }
  return dwSelID != 0 ? strSysReg + "," + Long2Str(dwSelID) : strSysReg;
}


jstring CDisassemblerNEC850::NecGetImm9(const u32 dwInst)
{
	DWORD dwImm = NecExtractBits(dwInst, 0, 5) + (NecExtractBits(dwInst, 18, 22) << 5);	
	jstring strImm;
	//strImm = "#"+StrF(dwImm);
	strImm = "#"+Hex(dwImm, 4);
	return strImm;
}

jstring CDisassemblerNEC850::NecGetImm5_11(const u32 dwInst)
{
	DWORD dwImm = NecExtractBits(dwInst, 11, 16);	
	jstring strImm;
	//strImm = "#"+StrF(dwImm);
	strImm = "#"+Hex(dwImm, 4);
	return strImm;
}

jstring CDisassemblerNEC850::NecGetImm4(const u32 dwInst)
{
	DWORD dwImm = NecExtractBits(dwInst, 0, 4);	
	jstring strImm;
	//strImm = "#"+StrF(dwImm);
	strImm = "#"+Hex(dwImm, 4);
	return strImm;
}

jstring CDisassemblerNEC850::NecGetImm5_18(const u32 dwInst)
{
	DWORD dwImm = NecExtractBits(dwInst, 18, 22);
  dwImm = 32 - (dwImm << 1);	
	jstring strImm;
	//strImm = "#"+StrF(dwImm);
	strImm = "#"+Hex(dwImm, 4);
	return strImm;
}

jstring CDisassemblerNEC850::NecGetImm5(const u32 dwInst)
{
	DWORD dwImm = NecExtractBits(dwInst, 0, 5);	
	jstring strImm;
	//strImm = "#"+StrF(dwImm);
	strImm = "#"+Hex(dwImm, 4);
	return strImm;
}

jstring CDisassemblerNEC850::NecGetImm32(const u32 dwImm)
{
	jstring strImm;
	//strImm = "#"+StrF(dwImm);
	strImm = "#"+Hex(dwImm, 8);
	return strImm;
}

jstring CDisassemblerNEC850::NecGetImm6(const u32 dwInst)
{
	DWORD dwImm = NecExtractBits(dwInst, 0, 6);	
	jstring strImm;
	//strImm = "#"+StrF(dwImm);
	strImm = "#"+Hex(dwImm, 4);
	return strImm;
}

jstring CDisassemblerNEC850::NecGetDisp32(const u32 dwInst, const u32 dwInst1)
{
	LONG lDisp = NecExtractBits(dwInst, 16, 32) | (NecExtractBits(dwInst1, 0, 16) << 16);
	DWORD dwAddr = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;
	jstring Operand;
	Operand = NecAdrToSymbol(dwAddr);
	return Operand;
}

jstring CDisassemblerNEC850::NecGetDisp22(const u32 dwInst)
	{
	LONG lDisp = ( NecExtractBits(dwInst, 16, 32) )| NecExtractBits(dwInst, 0, 6) << 16  ;
	SignExtend(lDisp, 22);
	DWORD dwAddr = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;
	jstring Operand;
	Operand = Hex(dwAddr, 6);
	return Operand;
	}


jstring CDisassemblerNEC850::NecGetDisp5(const u32 dwInst)
{
	LONG lDisp = NecExtractBits(dwInst, 0, 5);
	SignExtend(lDisp, 5);
	jstring Operand;
	//Operand = StrF(lDisp);
	Operand = Hex(lDisp, 4);
	return Operand;
}

jstring CDisassemblerNEC850::NecGetDisp9(const u32 dwInst)
{
	LONG lDisp = (NecExtractBits(dwInst, 4, 7) | (NecExtractBits(dwInst, 11, 16) << 3)) << 1;
	SignExtend(lDisp, 9);
	DWORD dwAddr = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;
	jstring Operand;
	Operand = Hex(dwAddr, 4);
	return Operand;
}

jstring CDisassemblerNEC850::NecGetDisp15(const u32 dwInst)
{
	LONG lDisp = (NecExtractBits(dwInst, 17, 32) << 1);
	SignExtend(lDisp, 16);
	DWORD dwAddr = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) - (DWORD) lDisp;
	jstring Operand;
	Operand = NecAdrToSymbol(dwAddr);
	return Operand;
}

jstring CDisassemblerNEC850::NecGetImmN16(const u32 dwInst)
{
	LONG lDisp = NecExtractBits(dwInst, 16, 32);
	SignExtend(lDisp, 16);
	jstring Operand;
	//Operand = StrF(lDisp);
	Operand = Hex(lDisp, 4);
	return Operand;
}



jstring CDisassemblerNEC850::NecGetDisp16(const u32 dwInst)
{
	LONG lDisp = (LONG) dwInst;
	SignExtend(lDisp, 16);
	jstring Operand;
	//Operand = ToString(lDisp);  // decimal
  if (lDisp < 0)
	Operand = "-" + Hex(-lDisp, 4);
    //Operand = "-" + StrF(-lDisp);///
  else
	    ///Operand = StrF(lDisp, 2);
  //Operand = StrF(lDisp);
  Operand = Hex(lDisp, 4);
	return Operand;
}

jstring CDisassemblerNEC850::NecGetDisp17(const u32 dwInst)
{
  LONG lDisp = (LONG) (((NecExtractBits(dwInst, 17, 32) | (NecExtractBits(dwInst, 4, 5) << 15))) << 1);
  SignExtend(lDisp, 17);
  DWORD dwAddr = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;
  jstring Operand;
  Operand = NecAdrToSymbol(dwAddr);
  return Operand;
}


jstring CDisassemblerNEC850::NecGetDisp23(const u32 dwInst, const u32 dwInst1)
{
	LONG lDisp = (NecExtractBits(dwInst, 20, 27) | (NecExtractBits(dwInst1, 0, 16) << 7));
	SignExtend(lDisp, 23);
	//DWORD dwAddr = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;
	jstring Operand;
	//Operand = StrF(lDisp); ///, 3);
	Operand = Hex(lDisp, 6);
	return Operand;
}

jstring CDisassemblerNEC850::NecGetDisp23Z0(const u32 dwInst, const u32 dwInst1)
{
	LONG lDisp = ((NecExtractBits(dwInst, 21, 27) << 1) | (NecExtractBits(dwInst1, 0, 16) << 7));
	SignExtend(lDisp, 23);
	//DWORD dwAddr = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;
	jstring Operand;
	//Operand = StrF(lDisp); ///, 4);
	Operand = Hex(lDisp, 6);
	return Operand;
}

jstring CDisassemblerNEC850::GetBitManOperand(const u32 dwInst)
{
	jstring strBit = "#" + Hex(NecExtractBits(dwInst, 11, 14), 4);
	LONG lDisp = (LONG) NecExtractBits(dwInst, 16, 32);
	SignExtend(lDisp, 16);
	jstring Operand;

	Operand = strBit + jstring(",") + jstring(Hex(lDisp, 4)) + jstring("[") + NecGetReg(NecExtractBits(dwInst, 0, 5)) + jstring("]");
	return Operand;
}

jstring CDisassemblerNEC850::NecGetRegListItem(jstring strList, u32 dwReg)
{
  jstring strList1=NecGetReg(dwReg);
  if (strList.empty())
    return strList1;
  else
    return strList + "," + strList1;
}
jstring CDisassemblerNEC850::NecGetRegList(const u32 dwInst)
{
	jstring strList;
	if (NecBitIsSet(dwInst, 27)) strList = NecGetRegListItem(strList, 20);
	if (NecBitIsSet(dwInst, 26)) strList = NecGetRegListItem(strList, 21);
	if (NecBitIsSet(dwInst, 25)) strList = NecGetRegListItem(strList, 22);
	if (NecBitIsSet(dwInst, 24)) strList = NecGetRegListItem(strList, 23);

	if (NecBitIsSet(dwInst, 31)) strList = NecGetRegListItem(strList, 24);
	if (NecBitIsSet(dwInst, 30)) strList = NecGetRegListItem(strList, 25);
	if (NecBitIsSet(dwInst, 29)) strList = NecGetRegListItem(strList, 26);
	if (NecBitIsSet(dwInst, 28)) strList = NecGetRegListItem(strList, 27);

	if (NecBitIsSet(dwInst, 23)) strList = NecGetRegListItem(strList, 28);
	if (NecBitIsSet(dwInst, 22)) strList = NecGetRegListItem(strList, 29);
	if (NecBitIsSet(dwInst, 0))  strList = NecGetRegListItem(strList, 30);
	if (NecBitIsSet(dwInst, 21)) strList = NecGetRegListItem(strList, 31);

	return strList;
}

jstring CDisassemblerNEC850::NecGetPrepareOp(const u32 dwInst, const u32 dwInst1, int& iSize)
{
	jstring strOp;
	strOp = NecGetRegList(dwInst) + "," + jstring(Long2Str(NecExtractBits(dwInst, 1, 6)<<2));
	DWORD dwOpType = NecExtractBits(dwInst, 19, 21);
	LONG lOp = (LONG) NecExtractBits(dwInst1, 0, 16);
	SignExtend(lOp, 16);
	
	switch(dwOpType)
	{
		case 0x0:	strOp = strOp + ",sp"; iSize = 4; break;
		case 0x1: strOp = strOp + jstring(Long2Str(lOp)); iSize = 6; break;
		case 0x2:	strOp = strOp + jstring(Long2Str(NecExtractBits(dwInst1, 0, 16) << 16)); iSize = 6; break;
		case 0x3: strOp = strOp + jstring(Long2Str(dwInst1)); iSize = 8; break;
	}

	return strOp;
}

jstring CDisassemblerNEC850::GetPrefetchStr(const u32 dwInst)
{
  jstring strOp;
  
  switch(dwInst)
  {
    case 0x00: strOp = "PREFI"; break;
    case 0x04: strOp = "PREFD"; break;
    default: strOp = "Other";
  }

  return strOp;
}

jstring CDisassemblerNEC850::GetCacheOpInst(const u32 dwInst)
{
  jstring strOp;
  switch(dwInst)
  {
  case 0x00: strOp = "CHBII"; break;
  case 0x20: strOp = "CIBII"; break;
  case 0x40: strOp = "CFALI"; break;
  case 0x60: strOp = "CISTI"; break;
  case 0x61: strOp = "CILDI"; break;
  case 0x04: strOp = "CHBID"; break;
  case 0x06: strOp = "CHBIWBD"; break;
  case 0x07: strOp = "CHBWBD"; break;
  case 0x24: strOp = "CIBID"; break;
  case 0x26: strOp = "CIBIWBD"; break;
  case 0x27: strOp = "CIBWBD"; break;
  case 0x44: strOp = "CFALD"; break;
  case 0x64: strOp = "CISTD"; break;
  case 0x65: strOp = "CILDD"; break;
  case 0x7e: strOp = "CLL"; break;
  default: strOp = "??";
  }

  return strOp;
}


DWORD CDisassemblerNEC850::GetNumRegList(DWORD dwInst) const
{
  DWORD dwNum = 0;
  for (BYTE i = 21; i <= 31; i++)
    if (NecBitIsSet(dwInst, i))
      dwNum++;
  if (NecBitIsSet(dwInst, 0))
    dwNum++;
  return dwNum;
}



int CDisassemblerNEC850::DissNEC850(BOOL bQD, const u32 dwInst, const u32 dwInst1, int& InsType, u32& dwNextAddress, jstring& Instruction, jstring& Operand)
{
	SET_INST_TYPE(InsType , it_Continue);
	u32 dwForm1 = 0;
	u32 dwForm2 = 0;
    u32 dwForm3 = 0;

  dwForm1 = NecExtractBits(dwInst, 0, 32);
  {
    switch(dwForm1)
    {
    case 0x012007E0: SET_STR(bQD, Instruction, "HALT"); return 4; // HALT
    case 0x014007E0: SET_STR(bQD, Instruction, "RETI"); SET_INST_TYPE(InsType , it_Reti); return 4;
    case 0x014807E0: SET_STR(bQD, Instruction, "EIRET"); SET_INST_TYPE(InsType , it_Reti); return 4;
    case 0x014407E0: SET_STR(bQD, Instruction, "CTRET"); SET_INST_TYPE(InsType , it_Ret); return 4;
    case 0x014607E0: SET_STR(bQD, Instruction, "DBRET"); SET_INST_TYPE(InsType , it_Ret); return 4;
    case 0x016007E0: SET_STR(bQD, Instruction, "DI"); return 4;
    case 0x016087E0: SET_STR(bQD, Instruction, "EI"); return 4;
      // Added 6.11.2007
    case 0x037807E0: SET_STR(bQD, Instruction, "TRFF"); return 4; // TRFF

      // Added 13.12.2010
    case 0x014A07E0: SET_STR(bQD, Instruction, "FERET"); SET_INST_TYPE(InsType , it_Ret); return 4;
      // Added 18.7.2013
    case 0x01200FE0: SET_STR(bQD, Instruction, "SNOOZE"); return 4;
    case 0x013407E0: SET_STR(bQD, Instruction, "DST"); return 4;
    case 0x013207E0: SET_STR(bQD, Instruction, "EST"); return 4;

    case 0x896087E0: SET_STR(bQD, Instruction, "TLBAI"); return 4;
    case 0xE96087E0: SET_STR(bQD, Instruction, "TLBR"); return 4;
    case 0xC16087E0: SET_STR(bQD, Instruction, "TLBS"); return 4;
    case 0x816087E0: SET_STR(bQD, Instruction, "TLBVI"); return 4;
    case 0xE16087E0: SET_STR(bQD, Instruction, "TLBW"); return 4;

    // Added 15.10.2018
    case 0xF160FFFF: SET_STR(bQD, Instruction, "CLL"); return 4;

    }
  }

  dwForm1 = NecExtractBits(dwInst, 0, 17) | (NecExtractBits(dwInst, 20, 32) << 17);
  {
    switch(dwForm1)
    {
    case 0x008007E0:
    	SET_STR(bQD, Instruction, "TRFSR");
    	SET_STR(bQD, Operand, StrF(NecExtractBits(dwInst, 17, 20)));
    	return 4;
    }
  }

  dwForm1 = NecExtractBits(dwInst, 5, 32);
  {
    switch(dwForm1)
    {
    case 0x008003F:	
      {
        SET_STR(bQD, Instruction, "TRAP"); 
        SET_STR(bQD, Operand, "#" + StrF(NecExtractBits(dwInst, 0, 5)));
        SET_INST_TYPE(InsType , it_Trap);
        if (bQD)				
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
        return 4; //return necExtended2; // TRAP
      }
    case 0x008803F:	
      {
        SET_STR(bQD, Instruction, "HVTRAP"); 
        SET_STR(bQD, Operand, "#" + StrF(NecExtractBits(dwInst, 0, 5)));
        SET_INST_TYPE(InsType , it_Trap); 
        if (bQD)				
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
        return 4; //return necExtended2; // HVTRAP
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst,  0, 27);
  {
    // Added 6.11.2007
    switch(dwForm1)
    {
    case 0x37807E0: SET_STR(bQD, Instruction, "STFF"); SET_STR(bQD, Operand, "EFG,"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;
    case 0x37C07E0: SET_STR(bQD, Instruction, "STFC"); SET_STR(bQD, Operand, "ECT,"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;
    }
  }

  dwForm1 = NecExtractBits(dwInst, 0, 11);
  {
    dwForm2 = NecExtractBits(dwInst, 16, 32);
    switch(dwForm1)
    {
    case 0x7E0: 
      switch(dwForm2)
      {
      case 0x037A: SET_STR(bQD, Instruction, "LDFF"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 12, 16))+",EFG"); return 4;
      case 0x037E: SET_STR(bQD, Instruction, "LDFC"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 12, 16))+",ECT"); return 4;
        // Added 13.12.2010
      case 0x0BC8: SET_STR(bQD, Instruction, "STACCL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4;
      case 0x0BCA: SET_STR(bQD, Instruction, "STACCH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4;
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst,  5, 27) | (NecExtractBits(dwInst,  30, 32) << 22);
  {
    switch (dwForm1)
    {
    case 0x0B06BF: 
      {
        SET_STR(bQD, Instruction, "SYSCALL"); 
        SET_STR(bQD, Operand, StrF((NecExtractBits(dwInst,  0, 5) | (NecExtractBits(dwInst,  27, 30) << 5))));
        SET_INST_TYPE(InsType , it_Jmp); 
        if (bQD)				
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
        return 4;
      }
    case 0x4B06BF: 
      {
        SET_STR(bQD, Instruction, "HVCALL");  
        SET_STR(bQD, Operand, ToString((NecExtractBits(dwInst,  0, 5) | (NecExtractBits(dwInst,  27, 30) << 5))));
        SET_INST_TYPE(InsType , it_CondJmp); 
        if (bQD)				
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
        return 4;
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst,  5, 27);
  switch (dwForm1)
  {
    case 0x0B02FF: SET_STR(bQD, Instruction, "DBPUSH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))+"-"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;
    case 0x0B067F: SET_STR(bQD, Instruction, "DBTAG");  SET_STR(bQD, Operand, "#"+ToString((NecExtractBits(dwInst,  0, 5) | (NecExtractBits(dwInst,  27, 32) << 5)))); return 4;
    case 0x0B06FF: SET_STR(bQD, Instruction, "PREF");   SET_STR(bQD, Operand, GetPrefetchStr(NecExtractBits(dwInst, 27, 32))+",["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 4;
    case 0x0B033F: SET_STR(bQD, Instruction, "POPSP");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))+"-"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;
    case 0x0B023F: SET_STR(bQD, Instruction, "PUSHSP"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))+"-"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;
  }

  dwForm1 = NecExtractBits(dwInst,  4, 11);
  {
    dwForm2 = NecExtractBits(dwInst, 16, 32);
    switch(dwForm1)
    {
    case 0x7E:
      switch(dwForm2)
      {
      case 0x0200: 
        {
          SET_STR(bQD, Instruction, "SASF"); SET_STR(bQD, Operand, NecGetCondCodeStr(NecExtractBits(dwInst, 0, 4)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); 
          //if (0x5 != NecExtractBits(dwInst, 0, 4))
            //FlagConditional();
          return 4; // SASF
        }
      case 0x0000: 
        {
          SET_STR(bQD, Instruction, "SETF"); 
          SET_STR(bQD, Operand, NecGetCondCodeStr(NecExtractBits(dwInst, 0, 4)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); 
          //if (0x5 != NecExtractBits(dwInst, 0, 4))
            //FlagConditional();
          return 4; // SETF
        }
      }
    case 0x7F:
      {
        switch(dwForm2)
        {
        case 0x0000: 
          {
            SET_STR(bQD, Instruction, "RIE"); 
            SET_STR(bQD, Operand, NecGetImm5_11(dwInst) + "," + NecGetImm4(NecExtractBits(dwInst, 11, 16))); 
            SET_INST_TYPE(InsType, it_Jmp); 
            if (bQD)				
            {
              m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
              dwNextAddress = 0x00000030;					
            }
            return 4; 
          }
        }
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst, 5, 27);
  switch(dwForm1)
  {
    case 0x0B063F:
      {
        SET_STR(bQD, Instruction, "JARL"); 
        SET_STR(bQD, Operand, "["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"],"+NecGetReg(NecExtractBits(dwInst, 27, 32))); 
        SET_INST_TYPE(InsType, it_Jmp); 
        if (bQD)				
        {
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flIndirect;
        }
        return 4; //return necJump; // JARL
      }
    case 0x1BC03F:
      {
        SET_STR(bQD, Instruction, "LDL.W"); 
        SET_STR(bQD, Operand, "["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"],"+NecGetReg(NecExtractBits(dwInst, 27, 32))); 
        return 4;
      }
    case 0x1BD03F:
      {
        SET_STR(bQD, Instruction, "STC.W"); 
        SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 27, 32)) + ",["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); 
        // FlagConditional();
        return 4;
      }
  }
  
  dwForm1 = NecExtractBits(dwInst,  5, 11);
  {
    dwForm2 = NecExtractBits(dwInst, 16, 27);
    switch(dwForm1)
    {
    case 0x3F:
      DWORD dwSelID = NecExtractBits(dwInst, 27, 32);
      switch(dwForm2)
      {
      case 0x0020:
        {
          SET_STR(bQD, Instruction, "LDSR");
          DWORD dwRegID = NecExtractBits(dwInst, 11, 16);
          {            
            SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetSysReg(dwRegID, dwSelID)); 
            return 4; // LDSR
          }
          break;
        }
      case 0x0040: 
        {
          //SET_STR(bQD, Instruction, "STSR"); SET_STR(bQD, Operand, NecGetSysReg(NecExtractBits(dwInst, 0, 5), dwSelID) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // STSR

          SET_STR(bQD, Instruction, "STSR");
          DWORD dwReg   = NecExtractBits(dwInst, 11, 16);
          DWORD dwRegID = NecExtractBits(dwInst, 0, 5);
          {
            SET_STR(bQD, Operand, NecGetSysReg(dwRegID, dwSelID) + "," + NecGetReg(dwReg)); 
            return 4; // STSR
          }
          break;
        }
      case 0x0034: SET_STR(bQD, Instruction, "LDVC.SR"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetSysReg(NecExtractBits(dwInst, 11, 16), NecExtractBits(dwInst, 27, 32))); return 4;
      case 0x0054: SET_STR(bQD, Instruction, "STVC.SR"); SET_STR(bQD, Operand, NecGetSysReg(NecExtractBits(dwInst, 0, 5), NecExtractBits(dwInst, 27, 32)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4;

      case 0x0032:
        {
          dwForm2 = NecExtractBits(dwInst, 27, 32);
          switch(dwForm2)
          {
            case 0x00: SET_STR(bQD, Instruction, "LDTC.GR"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4;
            case 0x01: SET_STR(bQD, Instruction, "LDTC.VR"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetSIMDReg(NecExtractBits(dwInst, 11, 16))); return 4;
            case 0x1F: 
              {
                SET_STR(bQD, Instruction, "LDTC.PC"); 
                SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))); 
                SET_INST_TYPE(InsType , it_Jmp); 
                if (bQD)				
                  m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flIndirect;
                return 4;
              }
          }                   
          break;
        }

      case 0x0052:
        {
          dwForm2 = NecExtractBits(dwInst, 27, 32);
          switch(dwForm2)
          {
            case 0x00: SET_STR(bQD, Instruction, "STTC.GR"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))  + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4;
            case 0x01: SET_STR(bQD, Instruction, "STTC.VR"); SET_STR(bQD, Operand, NecGetSIMDReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4;
            case 0x1F: SET_STR(bQD, Instruction, "STTC.PC"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))); return 4;            
          }                   
          break;
        }

      case 0x0050:
        {
          DWORD dwRegID = NecExtractBits(dwInst, 11, 16);
          SET_STR(bQD, Instruction, "STTC.SR");
          SET_STR(bQD, Operand, NecGetSysReg(dwRegID, dwSelID) + "," + NecGetReg(dwRegID) + "," + ToString(dwSelID));
          return 4;
          break;
        }

      case 0x0C4: SET_STR(bQD, Instruction, "ROTL"); SET_STR(bQD, Operand, NecGetImm5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;            
      case 0x0C6: SET_STR(bQD, Instruction, "ROTL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;            


      case 0x0080: SET_STR(bQD, Instruction, "SHR");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // SHR
      case 0x00A0: SET_STR(bQD, Instruction, "SAR");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // SAR
      case 0x00C0: SET_STR(bQD, Instruction, "SHL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // SHL
      case 0x00E0: SET_STR(bQD, Instruction, "SET1"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + ",[" + NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 4; // SET1
      case 0x00E2: SET_STR(bQD, Instruction, "NOT1"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + ",[" + NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 4; // NOT1
      case 0x00E4: SET_STR(bQD, Instruction, "CLR1"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + ",[" + NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 4; // CLR1
      case 0x00E6: SET_STR(bQD, Instruction, "TST1"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + ",[" + NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 4; // TST1

        // Added 13.12.2010
      case 0x0BC0: SET_STR(bQD, Instruction, "MACACC");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // 
      case 0x0BC2: SET_STR(bQD, Instruction, "MACUACC"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // 
      case 0x0BC4: SET_STR(bQD, Instruction, "LDACC");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // 
      }
    }	
  }

  dwForm1 = NecExtractBits(dwInst,  0, 11);
  {
    dwForm2 = NecExtractBits(dwInst, 16, 27);
    switch(dwForm1)
    {
    case 0x7E0:
      switch(dwForm2)
      {
      case 0x340:	SET_STR(bQD, Instruction, "BSW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // BSW
      case 0x342:	SET_STR(bQD, Instruction, "BSH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // BSH
      case 0x344: SET_STR(bQD, Instruction, "HSW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSW

        // Added 13.12.2010
      case 0x346: SET_STR(bQD, Instruction, "HSH");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSH
      case 0x360: SET_STR(bQD, Instruction, "SCH0R"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSH
      case 0x362: SET_STR(bQD, Instruction, "SCH1R"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSH
      case 0x364: SET_STR(bQD, Instruction, "SCH0L"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSH
      case 0x366: SET_STR(bQD, Instruction, "SCH1L"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSH

        // Added 6.11.2007
      case 0x3F8: SET_STR(bQD, Instruction, "NEGF.S");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSW
      case 0x3FC: SET_STR(bQD, Instruction, "ABSF.S");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSW
        //case 0x360: SET_STR(bQD, Instruction, "CVT.SW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSW	
      case 0x368: SET_STR(bQD, Instruction, "TRNC.SW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSW
      case 0x370: SET_STR(bQD, Instruction, "CVT.WS");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // HSW	
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst,  5, 11);
  {
    dwForm2 = NecExtractBits(dwInst, 16, 27);
    switch(dwForm1)
    {
    case 0x3F:
      {
        BOOL bFound = FALSE;
        switch(dwForm2)
        {
        case 0x220: bFound = TRUE; SET_STR(bQD, Instruction, "MUL");    SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // MUL
        case 0x222: bFound = TRUE; SET_STR(bQD, Instruction, "MULU");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // MULU
        case 0x280: bFound = TRUE; SET_STR(bQD, Instruction, "DIVH");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // DIVH
        case 0x282: bFound = TRUE; SET_STR(bQD, Instruction, "DIVHU");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // DIVHU
        case 0x2C0: bFound = TRUE; SET_STR(bQD, Instruction, "DIV");    SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // DIV
        case 0x0EE: bFound = TRUE; SET_STR(bQD, Instruction, "CAXI");   SET_STR(bQD, Operand, "["+ NecGetReg(NecExtractBits(dwInst, 0, 5)) + "]," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // CAXI
        case 0x2C2: bFound = TRUE; SET_STR(bQD, Instruction, "DIVU");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // DIVU

          // Added 6.11.2007	
        case 0x3E8: bFound = TRUE; SET_STR(bQD, Instruction, "ADDF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // ADDF.S
        case 0x3E4: bFound = TRUE; SET_STR(bQD, Instruction, "SUBF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // SUBF.S
        case 0x3EC: bFound = TRUE; SET_STR(bQD, Instruction, "MULF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // MULF.S
        case 0x3E0: bFound = TRUE; SET_STR(bQD, Instruction, "DIVF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // DIVF.S
        case 0x3F0: bFound = TRUE; SET_STR(bQD, Instruction, "MINF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // MINF.S
        case 0x3F4: bFound = TRUE; SET_STR(bQD, Instruction, "MAXF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // MAXF.S

          // Added 13.12.2010	
        case 0x082: bFound = TRUE; SET_STR(bQD, Instruction, "SHR"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;
        case 0x0A2: bFound = TRUE; SET_STR(bQD, Instruction, "SAR"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; 
        case 0x0C2: bFound = TRUE; SET_STR(bQD, Instruction, "SHL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; 

        case 0x39A: bFound = TRUE; SET_STR(bQD, Instruction, "SATSUB"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; 
        case 0x3BA: bFound = TRUE; SET_STR(bQD, Instruction, "SATADD"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; 
        case 0x2FC: bFound = TRUE; SET_STR(bQD, Instruction, "DIVQ");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; 
        case 0x2FE: bFound = TRUE; SET_STR(bQD, Instruction, "DIVQU");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; 

        case 0x4E0: bFound = TRUE; SET_STR(bQD, Instruction, "FMAF.S");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;	//FMAF.S
        case 0x4E2: bFound = TRUE; SET_STR(bQD, Instruction, "FMSF.S");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;	//FMSF.S
        case 0x4E4: bFound = TRUE; SET_STR(bQD, Instruction, "FNMAF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;	//FNMAF.S
        case 0x4E6: bFound = TRUE; SET_STR(bQD, Instruction, "FNMSF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;	//FNMSF.S
        case 0x442:
		  {
			dwForm3 = NecExtractBits(dwInst, 0, 5);
            switch (dwForm3)
            {
              case(2): bFound = TRUE; SET_STR(bQD, Instruction, "CVTF.HS"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;	//CVTF.HS
              case(3): bFound = TRUE; SET_STR(bQD, Instruction, "CVTF.SH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4;	//CVTF.SH
            }
          }
        }
        if (!bFound){
        	//break;
			dwForm2 = NecExtractBits(dwInst, 21, 27);
			if ((dwForm2 == 0x19) & (!NecBitIsSet(dwInst, 16)))
			{
			  SET_STR(bQD, Instruction, "CMOV"); SET_STR(bQD, Operand, NecGetCondCodeStr(NecExtractBits(dwInst, 17, 21)) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32)));
			  //FlagConditional();
			  return 4;
			}
        }
        //return necExtended3;
      }
    }

    dwForm2 = NecExtractBits(dwInst, 16, 18) | (NecExtractBits(dwInst, 22, 27) << 2);
    switch(dwForm1)
    {
    case 0x3F:
      {
        switch (dwForm2)
        {
        case 0x24: SET_STR(bQD, Instruction, "MUL");  SET_STR(bQD, Operand, NecGetImm9(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // MUL
        case 0x26: SET_STR(bQD, Instruction, "MULU"); SET_STR(bQD, Operand, NecGetImm9(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; // MULU

        case 0x18: SET_STR(bQD, Instruction, "SDIVHN");  SET_STR(bQD, Operand, NecGetImm5_18(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; //
        case 0x1A: SET_STR(bQD, Instruction, "SDIVHUN"); SET_STR(bQD, Operand, NecGetImm5_18(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; //
        case 0x1C: SET_STR(bQD, Instruction, "SDIVN");   SET_STR(bQD, Operand, NecGetImm5_18(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; //
        case 0x1E: SET_STR(bQD, Instruction, "SDIVUN");  SET_STR(bQD, Operand, NecGetImm5_18(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; //
        case 0x28: SET_STR(bQD, Instruction, "DIVHN");   SET_STR(bQD, Operand, NecGetImm5_18(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; //
        case 0x2A: SET_STR(bQD, Instruction, "DIVHUN");  SET_STR(bQD, Operand, NecGetImm5_18(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; //
        case 0x2C: SET_STR(bQD, Instruction, "DIVN");    SET_STR(bQD, Operand, NecGetImm5_18(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; //
        case 0x2E: SET_STR(bQD, Instruction, "DIVUN");   SET_STR(bQD, Operand, NecGetImm5_18(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); return 4; //
        }
        /*
        if (((NecExtractBits(dwInst, 22, 27) == 0x9) && NecExtractBits(dwInst, 16, 18) == 0x0))
        {

        }
        if (((NecExtractBits(dwInst, 22, 27) == 0x9) && NecExtractBits(dwInst, 16, 18) == 0x2))
        {

        }
        */
        if (((NecExtractBits(dwInst, 21, 27) == 0x18) && (!NecBitIsSet(dwInst, 16))))
        {
          SET_STR(bQD, Instruction, "CMOV"); SET_STR(bQD, Operand, NecGetCondCodeStr(NecExtractBits(dwInst, 1, 5)) + "," + NecGetImm5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +	NecGetReg(NecExtractBits(dwInst, 27, 32))); 
          //FlagConditional();
          return 4; // CMOV
        }
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst, 5, 11) | (NecExtractBits(dwInst, 16, 17) << 6) | (NecExtractBits(dwInst, 20, 27) << 7);
  {
    dwForm2 = (NecExtractBits(dwInst, 27, 28) << 3) | NecExtractBits(dwInst, 17, 20);
    switch(dwForm1)
    {
      case 0x04BF:
      case 0x05BF:
      case 0x06BF: SET_STR(bQD, Instruction, "BINS"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + 
    		  ToString(dwForm2)+","+ToString(NecExtractBits(dwInst, 28, 32)+1-dwForm2)+","+
        NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4;
    }
  }

  dwForm1 = NecExtractBits(dwInst, 5, 11) | (NecExtractBits(dwInst, 16, 17) << 6) | (NecExtractBits(dwInst, 21, 27) << 7);
  {
    switch(dwForm1)
    {
    case 0x0EBF:
      {
        SET_STR(bQD, Instruction, "ADF"); 
        SET_STR(bQD, Operand, NecGetCondCodeStr(NecExtractBits(dwInst, 17, 21)) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); 
        //if (0x5 != NecExtractBits(dwInst, 17, 21))
          //FlagConditional();

        return 4;
      }
    case 0x0E3F:
      {
        SET_STR(bQD, Instruction, "SBF"); 
        SET_STR(bQD, Operand, NecGetCondCodeStr(NecExtractBits(dwInst, 17, 21)) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 32))); 
        //if (0x5 != NecExtractBits(dwInst, 17, 21))
          //FlagConditional();
        return 4;
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst, 0, 16);
  { 
    switch(dwForm1)
    {
    case 0x0000: SET_STR(bQD, Instruction, "NOP"); return 2;
    case 0xF840: SET_STR(bQD, Instruction, "DBTRAP"); return 2;
    case 0xF040: SET_STR(bQD, Instruction, "RMTRAP"); return 2;
    case 0xE840: SET_STR(bQD, Instruction, "DBCP"); return 2;
    case 0xE040: SET_STR(bQD, Instruction, "DBHVTRAP"); return 2;

      //case 0x0040: SET_STR(bQD, Instruction, "DBINT");  return 2;
    case 0x0040: 
      {
        SET_STR(bQD, Instruction, "RIE"); 
        SET_INST_TYPE(InsType, it_Jmp); 
        if (bQD)				
        {
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
          dwNextAddress = 0x00000030;					
        }
        return 2; 
      }
    case 0x001D: SET_STR(bQD, Instruction, "SYNCE");  return 2;
    case 0x001E: SET_STR(bQD, Instruction, "SYNCM");  return 2;
    case 0x001F: SET_STR(bQD, Instruction, "SYNCP");  return 2;
    case 0x001C: SET_STR(bQD, Instruction, "SYNCI");  return 2;
    case 0x02E0: 
      {
        SET_STR(bQD, Instruction, "JR"); 
        SET_STR(bQD, Operand, NecGetDisp32(dwInst, dwInst1)); 
        SET_INST_TYPE(InsType , it_Jmp);
        if (bQD)
        {
          m_pAnalyze->m_wFlags &= (0xFFFF ^ IDisassemble::CAnalyze::flIndirect);				
          long lDisp = NecExtractBits(dwInst, 16, 32) | (NecExtractBits(dwInst1, 0, 16) << 16);
          dwNextAddress = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;					
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flRelative;
        }
        return 6;
      }
    }
  }  

	dwForm1 = NecExtractBits(dwInst,  6, 16);
  {
    dwForm2 = NecExtractBits(dwInst, 16, 21);
    int iSize = 4;
    switch(dwForm1)
    {
    case 0x19:
      switch(dwForm2)
      {
      case 0x0: 
        SET_STR(bQD, Instruction, "DISPOSE"); 
        SET_STR(bQD, Operand, jstring(Long2Str(NecExtractBits(dwInst, 1, 6)<<2)) + "," + NecGetRegList(dwInst) ; 
        break;// DISPOSE
      default:	
        SET_STR(bQD, Instruction, "DISPOSE"); 
        SET_STR(bQD, Operand, jstring(Long2Str(NecExtractBits(dwInst, 1, 6)<<2)) + "," + NecGetRegList(dwInst) + ",[" + NecGetReg(NecExtractBits(dwInst, 16, 21)) + "]"));
        SET_INST_TYPE(InsType , it_Ret); 
        break;      
        //return necStackManipulation;
      }	
      m_dwExtra = GetNumRegList(dwInst);
      return 4;// DISPOSE
    case 0x1E:
      if (NecExtractBits(dwInst, 16, 21) == 0x01)
      {
        SET_STR(bQD, Instruction, "PREPARE"); 
        SET_STR(bQD, Operand, NecGetRegList(dwInst) + "," + jstring(Long2Str(NecExtractBits(dwInst, 1, 6)<<2))); 
        if (bQD)
          m_dwExtra = GetNumRegList(dwInst);
        return 4;	 //return necStackManipulation; // PREPARE
      }
      if (NecExtractBits(dwInst, 16, 19) == 0x3) //return necStackManipulation; // PREPARE
      {
        SET_STR(bQD, Instruction, "PREPARE"); 
        SET_STR(bQD, Operand, NecGetPrepareOp(dwInst, dwInst1, iSize)); 
        if (bQD)
        {
	        switch (NecExtractBits(dwInst, 19, 21))
	        {
		        case 0x0:	iSize = 4; break;
		        case 0x1: iSize = 6; break;
		        case 0x2:	iSize = 6; break;
		        case 0x3: iSize = 8; break;
            default:             break;
	        }
          m_dwExtra = GetNumRegList(dwInst);
        }
        return iSize;
      }
    default: break;
    }
  }

	dwForm1 = NecExtractBits(dwInst, 5, 21);
  {
    switch(dwForm1)
    {
    case 0x383C: SET_STR(bQD, Instruction, "LD.H");   SET_STR(bQD, Operand, NecGetDisp23Z0(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"],"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 6;
    case 0x483C: SET_STR(bQD, Instruction, "LD.W");   SET_STR(bQD, Operand, NecGetDisp23Z0(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"],"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 6;
    case 0x383D: SET_STR(bQD, Instruction, "LD.HU");  SET_STR(bQD, Operand, NecGetDisp23Z0(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"],"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 6;

    case 0x783C: SET_STR(bQD, Instruction, "ST.W");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 27, 32))+","+NecGetDisp23Z0(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 6;
    case 0x683D: SET_STR(bQD, Instruction, "ST.H");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 27, 32))+","+NecGetDisp23Z0(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 6;

    // Added 15.10.2013
    case 0x483D: SET_STR(bQD, Instruction, "LD.DW");  SET_STR(bQD, Operand, NecGetDisp23Z0(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"],"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 6;
    case 0x783D: SET_STR(bQD, Instruction, "ST.DW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 27, 32))+","+NecGetDisp23Z0(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 6;
    }	
  }

	dwForm1 = NecExtractBits(dwInst, 5, 20);
  {
    switch(dwForm1)
    {
    case 0x283C: SET_STR(bQD, Instruction, "LD.B");  SET_STR(bQD, Operand, NecGetDisp23(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"],"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 6;
    case 0x683C: SET_STR(bQD, Instruction, "ST.B");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 27, 32)) + "," + NecGetDisp23(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); return 6;
    case 0x283D: SET_STR(bQD, Instruction, "LD.BU"); SET_STR(bQD, Operand, NecGetDisp23(dwInst, dwInst1)+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"],"+NecGetReg(NecExtractBits(dwInst, 27, 32))); return 6;
    }
  }

	dwForm1 = NecExtractBits(dwInst, 5, 11) | (NecExtractBits(dwInst, 16, 17) << 6) | (NecExtractBits(dwInst, 21, 28) << 7);
  {
    switch(dwForm1)
    {
    case 0x0F3F: SET_STR(bQD, Instruction, "MAC");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))+ "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 28, 32) << 1) + "," + NecGetReg(NecExtractBits(dwInst, 17, 21) << 1)); return 4;
    case 0x0FBF: SET_STR(bQD, Instruction, "MACU"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))+ "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 28, 32) << 1) + "," + NecGetReg(NecExtractBits(dwInst, 17, 21) << 1)); return 4;
    }
  }

  dwForm1 = NecExtractBits(dwInst, 5, 11) | (NecExtractBits(dwInst, 13, 16) << 6) | (NecExtractBits(dwInst, 16, 27) << 9);
  switch(dwForm1)
  {
  case 0x2C1FF:	
    {
      SET_STR(bQD, Instruction, "CACHE"); 
      SET_STR(bQD, Operand, GetCacheOpInst(NecExtractBits(dwInst, 27, 32) | (NecExtractBits(dwInst, 13, 15) << 5))+",["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); 
      return 4;
    }
  }

	dwForm1 = NecExtractBits(dwInst, 0, 11) | (NecExtractBits(dwInst, 15, 16) << 11);
  {
    switch(dwForm1)
    {
    case 0x040:	
      {
        SET_STR(bQD, Instruction, "FETRAP"); 
        SET_STR(bQD, Operand, ToString(NecExtractBits(dwInst, 11, 15)));
        SET_INST_TYPE(InsType , it_Jmp);
        if (bQD)	
          dwNextAddress = 0x00000030;					
        return 2;
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst, 5, 17);
  if (0x83F == dwForm1)
  {
    SET_STR(bQD, Instruction, "B" + NecGetCondCodeStr(NecExtractBits(dwInst, 0, 4))); 
    SET_STR(bQD, Operand, NecGetDisp17(dwInst)); 
    SET_INST_TYPE(InsType , it_CondJmp); 
    if (bQD)			
    {	
      if (NecExtractBits(dwInst, 0, 4) == 5)
      {
        m_pAnalyze->m_wFlags &= (0xFFFF ^ IDisassemble::CAnalyze::flIndirect);
        SET_INST_TYPE(InsType, it_Jmp); 
      }
      else
      {
        FlagConditional();
        if (WantConditionOutcome())
        {
          if (false) // evaluate flags affecting the instruction execution here
          {
            FlagConditionFalse();
          }
        }
      }

      LONG lDisp = (((NecExtractBits(dwInst, 17, 32) | (NecExtractBits(dwInst, 4, 5) << 15))) << 1);
      SignExtend(lDisp, 17);
      dwNextAddress = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;					
      if (bQD)				
        m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flRelative;
    }
    //!! Dodaj m_pAnalyze->m_wFlags status tudi za pogojne skoke..
    return 4; //return necCondBranch; // Bcond
  }

	dwForm1 = NecExtractBits(dwInst, 5, 16);
  {
    switch(dwForm1)
    {
    case 0x002:	SET_STR(bQD, Instruction, "SWITCH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))); SET_INST_TYPE(InsType , it_Jmp); return 2;// SWITCH
    case 0x003:	SET_STR(bQD, Instruction, "JMP"); SET_STR(bQD, Operand, "["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]"); 
      SET_INST_TYPE(InsType , (NecExtractBits(dwInst, 0, 5)==31) ? it_Ret : it_Jmp); return 2;// JMP
    case 0x004:	SET_STR(bQD, Instruction, "ZXB"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))); return 2; // ZXB
    case 0x005:	SET_STR(bQD, Instruction, "SXB"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))); return 2; // SXB
    case 0x006:	SET_STR(bQD, Instruction, "ZXH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))); return 2; // ZXH
    case 0x007:	SET_STR(bQD, Instruction, "SXH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5))); return 2; // return necRegReg; //SXH
    case 0x031: SET_STR(bQD, Instruction, "MOV"); SET_STR(bQD, Operand, NecGetImm32(NecExtractBits(dwInst, 16, 32) | (NecExtractBits(dwInst1, 0, 16) << 16)) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5))); return 6; //return nec3Op; // MOV
    case 0x037:	if (!NecBitIsSet(dwInst, 16))
      {
        SET_STR(bQD, Instruction, "JMP"); 
        SET_STR(bQD, Operand, ToString((NecExtractBits(dwInst, 17, 32) << 1) | (NecExtractBits(dwInst1, 0, 16) << 16))+"["+NecGetReg(NecExtractBits(dwInst, 0, 5))+"]");
        SET_INST_TYPE(InsType , (NecExtractBits(dwInst, 0, 5)==31) ? it_Ret : it_Jmp); 
        return 6; // JMP
      }
      else
      {
        SET_STR(bQD, Instruction, "LOOP");         
        SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetDisp15(dwInst)); 
        SET_INST_TYPE(InsType , it_Jmp);
        if (bQD)
        {
          m_pAnalyze->m_wFlags &= (0xFFFF ^ IDisassemble::CAnalyze::flIndirect);				
          LONG lDisp = (NecExtractBits(dwInst, 17, 32) << 1);
          SignExtend(lDisp, 16);
          dwNextAddress = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) - (DWORD) lDisp;					
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flRelative;
        }
        return 4;
      }
    case 0x017:
      {
        SET_STR(bQD, Instruction, "JARL"); 
        SET_STR(bQD, Operand, NecGetDisp32(dwInst, dwInst1) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5))); 
        SET_INST_TYPE(InsType, it_Jmp); 
        if (bQD)				
        {
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
          LONG lDisp = NecExtractBits(dwInst, 16, 32) | (NecExtractBits(dwInst1, 0, 16) << 16);					
          dwNextAddress = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;					
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flRelative;
        }
        return 6; //return necJump; // JARL	
      }
    }
  }

	dwForm1 = NecExtractBits(dwInst, 6, 16);
  {
    switch(dwForm1)
    {
    case 0x008: 
      {
        SET_STR(bQD, Instruction, "CALLT"); 
        SET_STR(bQD, Operand, NecGetImm6(dwInst)); 
        SET_INST_TYPE(InsType , it_Jmp); 
        if (bQD)				
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
        return 2; //return necImmReg; // CALLT
      }
    case 0x01E: 
      {
        SET_STR(bQD, Instruction, "JR"); 
        SET_STR(bQD, Operand, NecGetDisp22(dwInst)); 
        SET_INST_TYPE(InsType , it_Jmp);
        if (bQD)
        {
          m_pAnalyze->m_wFlags &= (0xFFFF ^ IDisassemble::CAnalyze::flIndirect);				
          LONG lDisp = NecExtractBits(dwInst, 16, 32) | (NecExtractBits(dwInst, 0, 6) << 16);
          SignExtend(lDisp, 22);
          dwNextAddress = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;					
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flRelative;
        }
        return 4; //return necJump; // JR
      }
    }
  }

	dwForm1 = NecExtractBits(dwInst,  5, 11);
  {
    dwForm2 = NecExtractBits(dwInst, 16, 27);	
    switch(dwForm1)
    {
    case 0x3F:
      {
        dwForm3 = NecExtractBits(dwInst,  0,  5);
        switch(dwForm2)
        {
        case 0x458: 
          {
            switch(dwForm3)
            {
            case 0x00: SET_STR(bQD, Instruction, "ABSF.D"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ABSF.D
            case 0x01: SET_STR(bQD, Instruction, "NEGF.D"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // NEGF.D
            }
            break;
          }	

        case 0x448: 
          {
            switch(dwForm3)
            {
            case 0x00: SET_STR(bQD, Instruction, "ABSF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ABSF.S
            case 0x01: SET_STR(bQD, Instruction, "NEGF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // NEGF.S
            }
            break;
          }

        case 0x470: SET_STR(bQD, Instruction, "ADDF.D"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ADDF.D
        case 0x460: SET_STR(bQD, Instruction, "ADDF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ADDF.S
        case 0x47E: SET_STR(bQD, Instruction, "DIVF.D"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // DIVF.D
        case 0x46E: SET_STR(bQD, Instruction, "DIVF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // DIVF.S
        case 0x478: SET_STR(bQD, Instruction, "MAXF.D"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // MAXF.D
        case 0x468: SET_STR(bQD, Instruction, "MAXF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // MAXF.S
        case 0x47A: SET_STR(bQD, Instruction, "MINF.D"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // MINF.D
        case 0x46A: SET_STR(bQD, Instruction, "MINF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // MINF.S
        case 0x474: SET_STR(bQD, Instruction, "MULF.D"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // MULF.D
        case 0x464: SET_STR(bQD, Instruction, "MULF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // MULF.S
        case 0x472: SET_STR(bQD, Instruction, "SUBF.D"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // SUBF.D
        case 0x462: SET_STR(bQD, Instruction, "SUBF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // SUBF.S

        case 0x452:
          { 
            switch(dwForm3)
            {
            case 0x03: SET_STR(bQD, Instruction, "CVTF.DS");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.DS						
            case 0x02: SET_STR(bQD, Instruction, "CVTF.SD");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.SD
            case 0x01: SET_STR(bQD, Instruction, "CVTF.LD");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.LD

            case 0x11: SET_STR(bQD, Instruction, "CVTF.ULD"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.ULD
            case 0x10: SET_STR(bQD, Instruction, "CVTF.UWD"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.UWD
            case 0x00: SET_STR(bQD, Instruction, "CVTF.WD");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.WD
            }
            break;				 
          }

        case 0x454:
          { 
            switch(dwForm3)
            {
            case 0x02: SET_STR(bQD, Instruction, "CEILF.DL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CEILF.DL
            case 0x12: SET_STR(bQD, Instruction, "CEILF.DUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CEILF.DUL

            case 0x04: SET_STR(bQD, Instruction, "CVTF.DL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.DL							
            case 0x14: SET_STR(bQD, Instruction, "CVTF.DUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.DUL							

            case 0x03: SET_STR(bQD, Instruction, "FLOORF.DL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // FLOORF.DL
            case 0x13: SET_STR(bQD, Instruction, "FLOORF.DUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // FLOORF.DUL

            case 0x00: SET_STR(bQD, Instruction, "ROUNDF.DL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ROUNDF.DL
            case 0x10: SET_STR(bQD, Instruction, "ROUNDF.DUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ROUNDF.DUL

            case 0x01: SET_STR(bQD, Instruction, "TRNCF.DL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // TRNCF.DL
            case 0x11: SET_STR(bQD, Instruction, "TRNCF.DUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // TRNCF.DUL
            }
            break;				 
          }

        case 0x450:
          { 
            switch(dwForm3)
            {
            case 0x12: SET_STR(bQD, Instruction, "CEILF.DUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CEILF.DUW
            case 0x02: SET_STR(bQD, Instruction, "CEILF.DW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CEILF.DW

            case 0x14: SET_STR(bQD, Instruction, "CVTF.DUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.DUW
            case 0x04: SET_STR(bQD, Instruction, "CVTF.DW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.DW

            case 0x13: SET_STR(bQD, Instruction, "FLOORF.DUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // FLOORF.DUW
            case 0x03: SET_STR(bQD, Instruction, "FLOORF.DW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // FLOORF.DW

            case 0x10: SET_STR(bQD, Instruction, "ROUNDF.DUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ROUNDF.DUW
            case 0x00: SET_STR(bQD, Instruction, "ROUNDF.DW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ROUNDF.DW

            case 0x11: SET_STR(bQD, Instruction, "TRNCF.DUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // TRNCF.DUW
            case 0x01: SET_STR(bQD, Instruction, "TRNCF.DW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // TRNCF.DW
            }
            break;				 
          }

        case 0x444:
          { 
            switch(dwForm3)
            {
            case 0x02: SET_STR(bQD, Instruction, "CEILF.SL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CEILF.SL
            case 0x12: SET_STR(bQD, Instruction, "CEILF.SUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CEILF.SUL

            case 0x04: SET_STR(bQD, Instruction, "CVTF.SL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.SL
            case 0x14: SET_STR(bQD, Instruction, "CVTF.SUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.SUL

            case 0x03: SET_STR(bQD, Instruction, "FLOORF.SL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // FLOORF.SL
            case 0x13: SET_STR(bQD, Instruction, "FLOORF.SUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // FLOORF.SUL

            case 0x00: SET_STR(bQD, Instruction, "ROUNDF.SL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ROUNDF.SL
            case 0x10: SET_STR(bQD, Instruction, "ROUNDF.SUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ROUNDF.SUL

            case 0x01: SET_STR(bQD, Instruction, "TRNCF.SL");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // TRNCF.SL
            case 0x11: SET_STR(bQD, Instruction, "TRNCF.SUL"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // TRNCF.SUL
            }
            break;				 
          }

        case 0x440:
          { 
            switch(dwForm3)
            {
            case 0x12: SET_STR(bQD, Instruction, "CEILF.SUW");SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CEILF.SUW
            case 0x02: SET_STR(bQD, Instruction, "CEILF.SW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CEILF.SW

            case 0x14: SET_STR(bQD, Instruction, "CVTF.SUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.SUW
            case 0x04: SET_STR(bQD, Instruction, "CVTF.SW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.SW

            case 0x13: SET_STR(bQD, Instruction, "FLOORF.SUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // FLOORF.SUW
            case 0x03: SET_STR(bQD, Instruction, "FLOORF.SW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // FLOORF.SW

            case 0x10: SET_STR(bQD, Instruction, "ROUNDF.SUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ROUNDF.SUW
            case 0x00: SET_STR(bQD, Instruction, "ROUNDF.SW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // ROUNDF.SW

            case 0x11: SET_STR(bQD, Instruction, "TRNCF.SUW"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // TRNCF.SUW
            case 0x01: SET_STR(bQD, Instruction, "TRNCF.SW");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // TRNCF.SW
            }
            break;				 
          }

        case 0x442:
          { 
            switch(dwForm3)
            {
            case 0x01: SET_STR(bQD, Instruction, "CVTF.LS");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.LS						  
            case 0x11: SET_STR(bQD, Instruction, "CVTF.ULS"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.ULS

            case 0x10: SET_STR(bQD, Instruction, "CVTF.UWS"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.UWS
            case 0x00: SET_STR(bQD, Instruction, "CVTF.WS");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CVTF.WS
            }
            break;				 
          }

        case 0x45E:
          { 
            switch(dwForm3)
            {
            case 0x01: SET_STR(bQD, Instruction, "RECIPF.D");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // RECIPF.D
            case 0x02: SET_STR(bQD, Instruction, "RSQRTF.D");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // RSQRTF.D

            case 0x00: SET_STR(bQD, Instruction, "SQRTF.D");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // SQRTF.D
            }
            break;				 
          }

        case 0x44E:
          { 
            switch(dwForm3)
            {
            case 0x01: SET_STR(bQD, Instruction, "RECIPF.S");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // RECIPF.S
            case 0x02: SET_STR(bQD, Instruction, "RSQRTF.S");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // RSQRTF.S

            case 0x00: SET_STR(bQD, Instruction, "SQRTF.S");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // SQRTF.S
            }
            break;				 
          }
        }

        dwForm3 = NecExtractBits(dwInst,  16,  17) | (NecExtractBits(dwInst,  20,  27) << 1);
        switch(dwForm3)
        {
        case 0x82: SET_STR(bQD, Instruction, "CMOVF.D"); SET_STR(bQD, Operand, ToString(NecExtractBits(dwInst, 17, 20))+ ","+ NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CMOVF.D
        case 0x80: SET_STR(bQD, Instruction, "CMOVF.S"); SET_STR(bQD, Operand, ToString(NecExtractBits(dwInst, 17, 20))+ ","+ NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31))); return 4; // CMOVF.S

        case 0x86: SET_STR(bQD, Instruction, "CMPF.D"); SET_STR(bQD, Operand, NecGetFPCondCodeStr(NecExtractBits(dwInst, 27, 31))+ ","+ NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + ToString(NecExtractBits(dwInst, 17, 20))); return 4; // CMPF.D
        case 0x84: SET_STR(bQD, Instruction, "CMPF.S"); SET_STR(bQD, Operand, NecGetFPCondCodeStr(NecExtractBits(dwInst, 27, 31))+ ","+ NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + ToString(NecExtractBits(dwInst, 17, 20))); return 4; // CMPF.D
        }

        dwForm3 = NecExtractBits(dwInst,  16,  17) | (NecExtractBits(dwInst,  21,  23) << 1) | (NecExtractBits(dwInst,  24,  27) << 3);
        DWORD dwReg = NecExtractBits(dwInst, 23, 24) | (NecExtractBits(dwInst, 17, 21) << 1);
        switch(dwForm3)
        {
        case 0x28: SET_STR(bQD, Instruction, "MADDF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31)) + "," + NecGetReg(dwReg)); return 4; // MADDF.S
        case 0x2A: SET_STR(bQD, Instruction, "MSUBF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31)) + "," + NecGetReg(dwReg)); return 4; // MSUBF.S
        case 0x2C: SET_STR(bQD, Instruction, "NMADDF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31)) + "," + NecGetReg(dwReg)); return 4; // NMADDF.S
        case 0x2E: SET_STR(bQD, Instruction, "NMSUBF.S"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + NecGetReg(NecExtractBits(dwInst, 27, 31)) + "," + NecGetReg(dwReg)); return 4; // NMSUBF.S
        }

      }
    }
  }

  dwForm1 = NecExtractBits(dwInst,  5, 11);
  {
    dwForm2 = NecExtractBits(dwInst, 14, 16);
    switch(dwForm1)
    {
    case 0x3E:
      {
        switch(dwForm2)
        {
        case 0x0: SET_STR(bQD, Instruction, "SET1"); SET_STR(bQD, Operand, GetBitManOperand(dwInst)); return 4;// SET1
        case 0x1:	SET_STR(bQD, Instruction, "NOT1"); SET_STR(bQD, Operand, GetBitManOperand(dwInst)); return 4;// NOT1
        case 0x2:	SET_STR(bQD, Instruction, "CLR1"); SET_STR(bQD, Operand, GetBitManOperand(dwInst)); return 4;// CLR1
        case 0x3:	SET_STR(bQD, Instruction, "TST1"); SET_STR(bQD, Operand, GetBitManOperand(dwInst)); return 4;// TST1
          //return necBitManipulation;
        }
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst,  4, 11);
  {
    switch(dwForm1)
    {
    case 0x6: SET_STR(bQD, Instruction, "SLD.BU"); SET_STR(bQD, Operand, "#"+jstring(Hex(NecExtractBits(dwInst, 0, 4),4)) + "[R30]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SLD.BU
    case 0x7:	SET_STR(bQD, Instruction, "SLD.HU"); SET_STR(bQD, Operand, "#"+jstring(Hex((NecExtractBits(dwInst, 0, 4) << 1),4)) + "[R30]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SLD.HU
      //return nec16bitLdSt;
    }
  }

  dwForm1 = NecExtractBits(dwInst,  5, 11);
  {
    switch(dwForm1)
    {
    case 0x00: SET_STR(bQD, Instruction, "MOV");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // MOV reg1, reg2
    case 0x01: SET_STR(bQD, Instruction, "NOT");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; //	NOT reg1, reg2
    case 0x02: SET_STR(bQD, Instruction, "DIVH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // DIVH reg1, reg2
    case 0x04: SET_STR(bQD, Instruction, "SATSUBR"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SATSUBR	reg1, reg2
    case 0x05: SET_STR(bQD, Instruction, "SATSUB");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SATSUB	reg1, reg2
    case 0x06: SET_STR(bQD, Instruction, "SATADD");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SATADD	reg1, reg2
    case 0x07: SET_STR(bQD, Instruction, "MULH"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // MULH reg1, reg2
    case 0x08: SET_STR(bQD, Instruction, "OR");   SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // OR reg1, reg2
    case 0x09: SET_STR(bQD, Instruction, "XOR");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // XOR reg1, reg2
    case 0x0A: SET_STR(bQD, Instruction, "AND");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // AND reg1, reg2
    case 0x0B: SET_STR(bQD, Instruction, "TST");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // TST reg1, reg2
    case 0x0C: SET_STR(bQD, Instruction, "SUBR"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SUBR reg1, reg2
    case 0x0D: SET_STR(bQD, Instruction, "SUB");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SUB reg1, reg2
    case 0x0E: SET_STR(bQD, Instruction, "ADD");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // ADD reg1, reg2
    case 0x0F: SET_STR(bQD, Instruction, "CMP");  SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // CMP reg1, reg2
      //return necRegReg;

    case 0x10: SET_STR(bQD, Instruction, "MOV");    SET_STR(bQD, Operand, NecGetDisp5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // MOV imm5, reg2
    case 0x11: SET_STR(bQD, Instruction, "SATADD"); SET_STR(bQD, Operand, NecGetDisp5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SATADD imm5, reg2
    case 0x12: SET_STR(bQD, Instruction, "ADD");    SET_STR(bQD, Operand, NecGetDisp5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // ADD imm5, reg2
    case 0x13: SET_STR(bQD, Instruction, "CMP");    SET_STR(bQD, Operand, NecGetDisp5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // CMP imm5, reg2
    case 0x14: SET_STR(bQD, Instruction, "SHR");    SET_STR(bQD, Operand, NecGetImm5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SHR imm5, reg2
    case 0x15: SET_STR(bQD, Instruction, "SAR");    SET_STR(bQD, Operand, NecGetImm5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SAR imm5, reg2
    case 0x16: SET_STR(bQD, Instruction, "SHL");    SET_STR(bQD, Operand, NecGetImm5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SHL imm5, reg2
    case 0x17: SET_STR(bQD, Instruction, "MULH");   SET_STR(bQD, Operand, NecGetDisp5(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // MULH imm5, reg2
      //return necImmReg;																																																											 

    case 0x30: SET_STR(bQD, Instruction, "ADDI");    SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // ANDI imm16, reg1, reg2
    case 0x31: SET_STR(bQD, Instruction, "MOVEA");   SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // MOVEA	imm16, reg1, reg2
    case 0x32: SET_STR(bQD, Instruction, "MOVHI");   SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; //	MOVHI	imm16, reg1, reg2
    case 0x33: SET_STR(bQD, Instruction, "SATSUBI"); SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // SATSUBI imm16, reg1, reg2
    case 0x34: SET_STR(bQD, Instruction, "ORI");     SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // ORI imm16, reg1, reg2
    case 0x35: SET_STR(bQD, Instruction, "XORI");    SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // XORI	imm16, reg1, reg2
    case 0x36: SET_STR(bQD, Instruction, "ANDI");    SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; //	ANDI	imm16, reg1, reg2
    case 0x37: SET_STR(bQD, Instruction, "MULHI");   SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // MULHI imm16, reg1, reg2
      //return nec3Op;																																																																																					 

    case 0x38: SET_STR(bQD, Instruction, "LD.B"); SET_STR(bQD, Operand, NecGetImmN16(dwInst) + "[" + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // LD.B
    case 0x39: 
      if (NecBitIsSet(dwInst, 16))
      {
        SET_STR(bQD, Instruction, "LD.W"); SET_STR(bQD, Operand, jstring(NecGetDisp16(NecExtractBits(dwInst, 17, 32) << 1)) + "[" + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // LD.W
      }
      else
      {
        SET_STR(bQD, Instruction, "LD.H"); SET_STR(bQD, Operand, jstring(NecGetDisp16(NecExtractBits(dwInst, 17, 32) << 1)) + "[" + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // LD.H
      }
    case 0x3A: SET_STR(bQD, Instruction, "ST.B"); SET_STR(bQD, Operand, jstring(NecGetReg(NecExtractBits(dwInst, 11, 16))) + jstring(",") + NecGetImmN16(dwInst) + jstring("[") + jstring(NecGetReg(NecExtractBits(dwInst, 0, 5))) + jstring("]")); return 4; // ST.B
    case 0x3B: 
      if (NecBitIsSet(dwInst, 16))
      {
        SET_STR(bQD, Instruction, "ST.W"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + jstring(NecGetDisp16(NecExtractBits(dwInst, 17, 32) << 1)) + "[" + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "]"); return 4; // ST.W
      }
      else
      {
        SET_STR(bQD, Instruction, "ST.H"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + jstring(NecGetDisp16(NecExtractBits(dwInst, 17, 32) << 1)) + "[" + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "]"); return 4; // ST.H
      }
    case 0x3F: 
      if (NecBitIsSet(dwInst, 16))
      {
        if (0 != NecExtractBits(dwInst, 11, 16))  // R0 destination is not valid for this instruction - U19949EJ1V0UM00_V850E2M core architecture.pdf p.116
        {                                         // added to catch GHS 5.1.6 compiler BNE coding bug
          SET_STR(bQD, Instruction, "LD.HU"); SET_STR(bQD, Operand, jstring(NecGetDisp16(NecExtractBits(dwInst, 17, 32) << 1)) + "[" + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; // LD.HU
        }
      }
      //return nec32LdSt;
      break;
    }
  }

  dwForm1 = NecExtractBits(dwInst,  6, 11);
  {
    switch (dwForm1)
    {
    case 0x1E: 
      if (NecBitIsSet(dwInst, 16))
      {
        SET_STR(bQD, Instruction, "LD.BU"); SET_STR(bQD, Operand, jstring(NecGetDisp16((NecExtractBits(dwInst, 17, 32) << 1) | NecExtractBits(dwInst, 5, 6))) + "[" + NecGetReg(NecExtractBits(dwInst, 0, 5)) + "]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 4; //return nec32LdSt;	// LD.BU
      }
      else
      {
        SET_STR(bQD, Instruction, "JARL"); 
        SET_STR(bQD, Operand, NecGetDisp22(dwInst) + "," + NecGetReg(NecExtractBits(dwInst, 11, 16))); 
        SET_INST_TYPE(InsType, it_Jmp); 
        if (bQD)				
        {
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flCall;
          LONG lDisp = NecExtractBits(dwInst, 16, 32) | (NecExtractBits(dwInst, 0, 6) << 16);
          SignExtend(lDisp, 22);
          dwNextAddress = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;					
          m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flRelative;
        }
        return 4; //return necJump; // JARL	
      }
    }
  }

  dwForm1 = NecExtractBits(dwInst,  7, 11);
  {
    switch (dwForm1)
    {
    case 0xB: 
      {
        SET_STR(bQD, Instruction, "B" + NecGetCondCodeStr(NecExtractBits(dwInst, 0, 4))); 
        SET_STR(bQD, Operand, NecGetDisp9(dwInst)); 
        SET_INST_TYPE(InsType , it_CondJmp); 
        if (bQD)			
        {	
          if (NecExtractBits(dwInst, 0, 4) == 5)
          {
            m_pAnalyze->m_wFlags &= (0xFFFF ^ IDisassemble::CAnalyze::flIndirect);
            SET_INST_TYPE(InsType, it_Jmp); 
          }
          else
          {
            FlagConditional();
            if (WantConditionOutcome())
            {
              if (false) // evaluate flags affecting the instruction execution here
              {
                FlagConditionFalse();
              }
            }
          }

          LONG lDisp = (NecExtractBits(dwInst, 4, 7) | (NecExtractBits(dwInst, 11, 16) << 3)) << 1;
          SignExtend(lDisp, 9);
          dwNextAddress = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + (DWORD) lDisp;					
          if (bQD)
            m_pAnalyze->m_wFlags |= IDisassemble::CAnalyze::flRelative;
        }
        //!! Dodaj m_pAnalyze->m_wFlags status tudi za pogojne skoke..
        return 2; //return necCondBranch; // Bcond
      }
    case 0x6:	SET_STR(bQD, Instruction, "SLD.B"); SET_STR(bQD, Operand, jstring(Hex(NecExtractBits(dwInst, 0, 7), 4)) + "[R30]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SLD.B
    case 0x7:	SET_STR(bQD, Instruction, "SST.B"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + Hex(NecExtractBits(dwInst, 0, 7), 4) + "[R30]"); return 2; // SST.B
    case 0x8:	SET_STR(bQD, Instruction, "SLD.H"); SET_STR(bQD, Operand, jstring(Hex((NecExtractBits(dwInst, 0, 7) << 1), 4)) + "[R30]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // // SLD.H
    case 0x9:	SET_STR(bQD, Instruction, "SST.H"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," + Hex((NecExtractBits(dwInst, 0, 7) << 1),4) + "[R30]"); return 2; // SST.H
    case 0xA:	
      if (NecBitIsSet(dwInst, 0))
      {
        SET_STR(bQD, Instruction, "SST.W"); SET_STR(bQD, Operand, NecGetReg(NecExtractBits(dwInst, 11, 16)) + "," +jstring(Hex((NecExtractBits(dwInst, 1, 7) << 2),4)) + "[R30]"); return 2; // SST.W
      }
      else															 
      {
        SET_STR(bQD, Instruction, "SLD.W"); SET_STR(bQD, Operand, jstring(Hex((NecExtractBits(dwInst, 1, 7) << 2),4)) + "[R30]," + NecGetReg(NecExtractBits(dwInst, 11, 16))); return 2; // SLD.W
      }
    }
  }

  return 0;
}

int CDisassemblerNEC850::DisassmNEC850All(BOOL bQD, PCBYTE Buff, int Bytes, int& InsType, u32& dwNextAddress, int& NumCycles, jstring & rjstrDasm)
{
	jstring Instruction;  
	jstring Operand;
	int RetValue = 0;
	int LenInstr=12;

    m_dwExtra	= 0;
	dwNextAddress = 0x00000001; //DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + 4;
	SET_INST_TYPE(InsType , it_Continue);

	u32 dwInst  = 0;
	u32 dwInst1 = 0;
	NecGetInst(Buff, dwInst, dwInst1);
	RetValue = DissNEC850(bQD, dwInst, dwInst1, InsType, dwNextAddress, Instruction, Operand);
    //if (0x00000001 == dwNextAddress)
    //dwNextAddress = DWORD_FROMADDROFFS(m_pParameters->m_aAddress) + RetValue;

	if (!bQD)
	{
    if (0 == RetValue || RetValue > Bytes)
    {
      RetValue = 0;
    }
    else
    {
      FormatOpCodeString(rjstrDasm, Instruction, Operand, LenInstr);
    }
	}
    if (NULL!=m_pAnalyze)
    {
      // m_pAnalyze->m_byMemAccessSizeMAUs = (BYTE)m_dwExtra;
    }

	return RetValue;
}


