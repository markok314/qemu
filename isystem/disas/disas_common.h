#pragma once

#include <stdint.h>
#include <string>


#define interface struct
typedef const char *LPCTSTR;
#define PURE =0

typedef uint32_t u32;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;
typedef uint64_t ADDROFFS;

typedef char * LPTSTR;
typedef const char * LPCSTR;
typedef int BOOL;

typedef std::string jstring;

// MemArea.h

enum EMemoryProperty
{
  mempMode0       = 0,   // CPU specific mode, should be on index 0
  mempMode1       = 1,   // CPU specific mode

  mempEndianBig   = 3,   // this region is explicitly big endian
  mempEndianLittle= 4,   // this region is explicitly little endian
  mempReserved5   = 5,
  mempReserved6   = 6,
  mempReserved7   = 7,
// all detectable modes (thumb, endian) should be in range 0-7

  mempModeEx0,     // CPU specific pseudo modes
  mempModeEx1,
  mempModeEx2,
  mempModeEx3,

  mempBlockRead,
  mempBlockWrite,

  mempBlock1MAUAccess,  // what memory access should not be used
  mempBlock2MAUAccess,
  mempBlock4MAUAccess,
  mempBlock8MAUAccess,

  mempNum,

  mempISANum  = 4,    // number of combinations for CPU specific modes (2 bits = 4) - Mode0 and Mode1 used together
  mempISAMask = 0x3,  // (1 << mempMode0) | (1 << mempMode1),
  mempISA0    = 0x0,
  mempISA1    = 0x1,
  mempISA2    = 0x2,
  mempISA3    = 0x3,
};

enum EEndian
{
  endianLittle,
  endianBig,
};


interface ICodeCache : public IUnknown
{
#ifndef SWIG
  /**
   * sets the interpretation of address and size parameters
   *
   */
  STDMETHOD_ (void, SetACInMAUs)
    (
    BOOL bACInMAUs                    ///< specifies whether address and size parameters are in MAU units or BYTE units. default = MAUs
    ) PURE;

  /**
   * returns current parameter configuration
   *
   * @return
   * - FALSE - Address and Size parameters are interpreted in BYTE units
   * - TRUE  - Address and Size parameters are interpreted in MAU units
   */
  STDMETHOD_ (BOOL, GetACInMAUs)
    () PURE;

  /**
   * sets the MAU size. Each addressable unit then contains MAU bytes.
   * Implicitly clears all contents.
   */
  STDMETHOD_ (void, SetMAUSize)
    (
    int nBytesPerMAU
    ) PURE;

  /**
   * returns current MAU size configuration
   *
   * @return
   * - size of one addressable unit in BYTEs
   */
  STDMETHOD_ (int, GetMAUSize)
    () PURE;

  /**
   * inserts a new chunk of code
   *
   */
  STDMETHOD_ (void, Insert)
    (
    TCC_ADDRESS aAddress,             ///< address to insert at
    TCC_ADDRESS aSize,                ///< size of the chunk
    const BYTE * pbyBuf               ///< pointer to the buffer containing the data. if NULL, no copying is performed
    ) PURE;

  /**
   * excludes a range
   *
   */
  STDMETHOD_ (void, Exclude)
    (
    TCC_ADDRESS aAddressExcl,         ///< first address of the exclusion
    TCC_ADDRESS aEndExcl              ///< last address of the exclusion
    ) PURE;

  /**
   * obtains the code from the specified region
   *
   * @return
   * - number of locations filled
   */
  STDMETHOD_ (TCC_ADDRESS, Get)
    (
    TCC_ADDRESS aAddress,             ///< starting address
    TCC_ADDRESS aSize,                ///< number of locations
    BYTE * pbyBuf,                    ///< buffer to fill
    BOOL bWantContiguousFromStart = FALSE, ///< if TRUE, the locations must be contiguous from aAddress
    BYTE * pbyAccessInfo = NULL       ///< pbyAccessInfo is filled with 0 for NA, 1 for OK. one byte for every MAU
    ) PURE;

  /**
   * returns number of all items
   *
   * @return
   * - number of items
   */
  STDMETHOD_ (int, GetNumRecs)
    () PURE;

  /**
   * finds an item in the requested range
   *
   * @return
   * - index of the item
   * - -1 - no item in the range
   */
  STDMETHOD_ (int, FindRec)
    (
    TCC_ADDRESS aAddress,             ///< starting address of the range
    TCC_ADDRESS aSize = 1             ///< size of the range
    ) PURE;

  /**
   * returns the requested item
   *
   * @return
   * - pointer to item
   */
  STDMETHOD_ (ICodeCacheRec *, GetRec)
    (
    int nIndex                        ///< item index
    ) PURE;

  /**
   * clears all items
   *
   */
  STDMETHOD_ (void, Clear)
    () PURE;

  /**
   * enumerates all chunks in specified region
   *
   * @return
   * - TRUE  - the callback function returned TRUE always
   * - FALSE - enumeration was aborted when callback function returned FALSE
   */
  STDMETHOD_ (BOOL, Enum)
    (
    TCC_ADDRESS aAddress,             ///< starting address of the range
    TCC_ADDRESS aEndAddress,          ///< last address in the range
    BOOL fnCallBack(TCC_ADDRESS aAddress, TCC_ADDRESS aEndAddress, const BYTE * pbyCode, PVOID pInfo), ///< enumeration call back function
    PVOID pInfo                       ///< pointer to pass back to the enumeration
    ) PURE;

  /**
   * returns total number of contained locations
   *
   * @return
   * - number of contained locations
   */
  STDMETHOD_ (TCC_ADDRESS, GetTotal)
    () PURE;

  /**
   * returns lowest and highest loaded location
   *
   * @return
   * - TRUE if at least one location is contained. FLASE if empty
   */
  STDMETHOD_ (BOOL, GetRange)
    (
    TCC_ADDRESS * paLo,              ///< pointer variable to receive the lowest address
    TCC_ADDRESS * paHi               ///< pointer variable to receive the highest address
    ) PURE;
#endif  // SWIG
  enum ECopyMergeFlags
  {
    cfNoData = 0x00000000,           ///< do not copy data
    cfData   = 0x00000001,           ///< copy data
  };
#ifndef SWIG

  #ifdef H_PERL
  #pragma push_macro("Copy")
  #undef Copy
  #endif

  /**
   * copies from another code cache instance. Previous contents are cleared
   *
   */
  STDMETHOD_ (void, Copy)
    (
    DWORD dwFlags,                  ///< copy flags. uses ECopyMergeFlags
    ICodeCache * pICodeCacheSrc     ///< source code cache to copy from
    ) PURE;

  #ifdef H_PERL
  #pragma pop_macro("Copy")
  #endif

  /**
   * merges with contents from another code cache instance
   *
   */
  STDMETHOD_ (void, Merge)
    (
    DWORD dwFlags,                  ///< copy flags. uses ECopyMergeFlags
    ICodeCache * pICodeCacheSrc,    ///< source code cache to merge from
    TCC_ADDRESS aOffset = 0         ///< merge offset to add to all items in the source
    ) PURE;

#endif  // SWIG


  enum EDifType
  {
    dtEqual  = 0,  ///< this part was in both and was equal
    dtNotIn1 = 1,  ///< this part was not in first ICodeCache
    dtNotIn2 = 2,  ///< this part was not in second ICodeCache
    dtDif    = 3,  ///< this part was in both, but content differs
  };
  enum EDifFlags
  {
    dfReportEqual   = 1 << dtEqual,   ///< report locations with equal contents
    dfReportNotIn1  = 1 << dtNotIn1,  ///< report locations that are not in first ICodeCache
    dfReportNotIn2  = 1 << dtNotIn2,  ///< report locations that are not in second ICodeCache
    dfReportDif     = 1 << dtDif,     ///< report locations with different contents

    dfReportAllDif  = dfReportNotIn1 | dfReportNotIn2 | dfReportDif, ///< report all differing locations and missing in one or other
    dfReportAll     = dfReportAllDif | dfReportEqual,                ///< report everything
  };

#ifndef SWIG

  /**
   * performs a difference check with another code cache
   *
   * @return
   * - a new code cache instance, which contains difference between this and the pICodeCache2. the data in the Dif is BYTE using EDifType values
   */
  STDMETHOD_ (ICodeCache *, Dif)
    (
    DWORD dwFlags,                  ///< Dif flags. uses EDifFlags
    ICodeCache * pICodeCache2       ///< code cache to compare with
    ) PURE;

#endif  // SWIG


  enum ESerializeFlags
  {
    sfAddressSize4 = 0x02,          ///< save 32-bit addresses
    sfAddressSize8 = 0x03,          ///< save 64-bit addresses
    sfAddressMask  = 0x03,

    sfData         = 0x04,          ///< serialize data
    sfMAUSize      = 0x08,          ///< serialize also MAUSize - not included per default in legacy saves
  };

#ifndef SWIG

  /**
   * serialize the object
   *
   * @return
   * - TRUE - serialization successful, FALSE otherwise
   */
  STDMETHOD_ (BOOL, Serialize)
    (
    DWORD dwFlags,                  ///< flags. uses ESerializeFlags
    interface ISerializer * pISerializer ///< pointer to serializer
    ) PURE;

#endif  // SWIG


  enum ESaveLoadFlags
  {
    lFormatAuto      = 0x00000000, ///< automatically detect file type
    lFormatMotorolaS = 0x00000001, ///< use MotorolaS format
    lFormatIntelHex  = 0x00000002, ///< use Intel hex format
    lFormatBinary    = 0x00000003, ///< use binary format
    lFormatELF       = 0x00000004, ///< use ELF format
    lFormatMask      = 0x0000000F,

    lAllowOverlap    = 0x00000010, ///< tolerate overlaps when loading

    lSaveBytes10     = 0x00000000, ///< when saving save 0x10 bytes per line
    lSaveBytes20     = 0x00000020, ///< when saving save 0x20 bytes per line
    lSaveBytes40     = 0x00000040, ///< when saving save 0x40 bytes per line
    lSaveBytes80     = 0x00000060, ///< when saving save 0x80 bytes per line
    lSaveBytesMask   = 0x00000060,

    // format sub-options
    lOptionELF_Addr_PH_Virtual  = 0x00000000, ///< use ELF ProgramHeader virtual as load address
    lOptionELF_Addr_PH_Physical = 0x00000100, ///< use ELF ProgramHeader physical as load address
    lOptionELF_Addr_Mask        = 0x00000300, ///< mask for Elf Address
    lOptionELF_IgnoreZeros      = 0x00000400, ///< ignore headers with zero content
    lOptionELF_RecognizeAuto    = 0x00008000, ///< if set, then ELF will be recognized in lFormatAuto

    lOptionMask                 = 0x0000FF00,
  };
  enum ESaveLoadReturn
  {
    E_OPEN           = 0x80040000,   // can't open file
    E_FORMAT         = 0x80040001,   // file format error
    E_OVERLAPPING    = 0x80040002,   // code overlaps
    E_ADDRESSRANGE   = 0x80040003,   // code does not fit format address limit
    E_NOTSUPPORTED   = 0x80040004,   // option, format or operation is not supported
  };

#ifndef LSB2DWORD
#define LSB2DWORD(B)      (SwapDWORD(*(DWORD*)(B)))
#endif

#define ISMASK(rValue, Mask) (((rValue) & (Mask))!=0)

#define DWORD2MSB(DW, B)  (*(DWORD*)(B)=DW)
#define MSB2DWORD(B)      (*(DWORD*)(B))


typedef char * LPTSTR;
typedef const char * LPCSTR;
typedef int BOOL;


#ifndef SWIG

  /**
   * load from a file
   *
   * @return
   * - S_OK - load successful
   * - ESaveLoadReturn encoded error code
   */
  STDMETHOD (Load)
    (
    DWORD dwFlags,                   ///< flags, uses ESaveLoadFlags
    LPCSTR pszFileName,              ///< path to the file
    TCC_ADDRESS aOffset              ///< offset to add to all addresses in the file
    ) PURE;

  /**
   * save to file
   *
   * @return
   * - S_OK - save successful
   * - ESaveLoadReturn encoded error code
   */
  STDMETHOD (Save)
    (
    DWORD dwFlags,                   ///< flags, uses ESaveLoadFlags
    LPCSTR pszFileName,              ///< path to the file
    TCC_ADDRESS aOffset              ///< offset to add to all addresses in the file
    ) PURE;

  /**
   * @return last error
   *
   */
  STDMETHOD_ (LPCSTR, GetErrorString)
    (
    ) PURE;

#endif  // SWIG

  /**
   * report to a file
   *
   * @return
   * - S_OK - save successful
   * - ESaveLoadReturn encoded error code
   */
  struct SReport                      // this structure should only have new members appended to - this ensures extendability
  {
    DWORD m_dwSize;                   ///< size of this structure
    ICodeCache * m_pICC1;             ///< when reporting a dif, this is the first reference
    ICodeCache * m_pICC2;             ///< when reporting a dif, this is the second reference

    LPCSTR m_pszHeader;               ///< text to write before first entry
    LPCSTR m_pszFormat;               ///< format string for every entry. Field encoding:
                                      ///<   %DIF%  - difference type. Replaced by either m_pszTextDif, m_pszTextNotIn1, m_pszTextNotIn2
                                      ///<   %ADDR% - address hex
                                      ///<   %SIZE% - size hex
                                      ///<   %EADDR% - end address hex
                                      ///<   %DATA%  - data
                                      ///<   %DATA1% - data from pICC1 hex
                                      ///<   %DATA2% - data from pICC1 hex
    LPCSTR m_pszFooter;               ///< text to write after last entry
    LPCSTR m_pszTextDif;              ///< text to use to specify difference between 1 and 2
    LPCSTR m_pszTextNotIn1;           ///< text to use to specify that the range is not in 1
    LPCSTR m_pszTextNotIn2;           ///< text to use to specify that the range is not in 2

    BYTE   m_byDigitsADDR;            ///< number of digits for ADDR and EADDR field. 0 means as many digits as necessary to display
    BYTE   m_byDigitsSIZE;            ///< number of digits for SIZE field. 0 means as many digits as necessary to display
    DWORD  m_dwMaxMAUsPerLine;        ///< max number of MAUs per line. 0 for unlimited
  };


  enum EReportFlags
  {
    rfTypeNormal   = 0x00000000,      ///< report contents
    rfTypeDif      = 0x00000001,      ///< the object contains dif between m_pICC1 and m_pICC2 - report dif
    rfTypeMask     = 0x00000003,      ///< mask for report type

    rfAppend       = 0x00000004,      ///< append to file rather than overwrite

    rfAlign        = 0x00000008,      ///< align addresses to m_dwMaxMAUsPerLine
  };

  STDMETHOD_ (Report)
    (
    DWORD dwFlags,                   ///< flags. use EReportFlags
    LPCSTR pszFileName,              ///< path to the report file
    const ICodeCache::SReport & rReport          ///< save format configuration
    ) PURE;

  /**
   * offset all records
   *
   */
  STDMETHOD_ (void, Offset)
    (
    TCC_ADDRESS aOffset             ///< amount to offset
    ) PURE;
};


interface IMemoryProperty: public IUnknown
{
  STDMETHOD_ (EEndian, GetEndian) (int nMemArea, ADDROFFS aAddress) const PURE;
  STDMETHOD_ (BOOL, GetMemoryProp)(int nProp, int nMemArea, ADDROFFS aAddress, ADDROFFS * paLast) const PURE;
  STDMETHOD_ (BOOL, GetMemoryProp)(int nProp, int nMemArea, ADDROFFS aAddress) const PURE;
  STDMETHOD_ (BOOL, HasMemoryProp)(int nProp, int nMemArea) const PURE;
  enum EFilterMode
  {
    fmIncluded = 0x01, // areas present in nProp are forwarded to pfnCommit
    fmExcluded = 0x02, // areas not present in nProp are forwarded to pfnCommit
    fmSplit    = fmIncluded | fmExcluded,   // all areas are forwarded to pfnCommit, but are individually split
  };
  STDMETHOD_ (BOOL, Filter)       (BYTE byFilterMode, int nProp, SAddress adrStart, ADDROFFS aEnd, BOOL (*pfnCommit)(SAddress adrStart, ADDROFFS aEnd, void* pInfo), void* pInfo) const PURE;
  STDMETHOD_ (BOOL, Serialize)    (ISerializer * pISerializer) PURE;
  STDMETHOD_ (void, SetMemoryProp)(int nProp, int nMemArea, ADDROFFS aAddress, ADDROFFS aLast) PURE; // sets nProp in specified range
  STDMETHOD_ (void, ClearMemoryProp)(int nProp, int nMemArea, ADDROFFS aAddress, ADDROFFS aLast) PURE; // sets nProp in specified range
};



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
