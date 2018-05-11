#include "AsystTrace.h"
/* buffers must be 2^n in size ( max 0x100) */
#define TWBUFF_SIZE 128 
#define TRBUFF_SIZE 128

/* target and client pointers at the end of the buffers */
#define TWBUFF_TPTR (g_TWBuffer[TWBUFF_SIZE + 0])
#define TWBUFF_CPTR (g_TWBuffer[TWBUFF_SIZE + 1])
#define TRBUFF_TPTR (g_TRBuffer[TRBUFF_SIZE + 0])
#define TRBUFF_CPTR (g_TRBuffer[TRBUFF_SIZE + 1])
/* number of pointers */
#define TBUFF_PTR 2
/* buffer manipulation helpers */
#define TWBUFF_EMPTY() (TWBUFF_TPTR == TWBUFF_CPTR)
#define TWBUFF_FULL()  (TWBUFF_TPTR == ((TWBUFF_CPTR - 1) & (TWBUFF_SIZE - 1)))
#define TWBUFF_INC(n)  ((n + 1) & (TWBUFF_SIZE - 1))
#define TRBUFF_EMPTY() (TRBUFF_TPTR == TRBUFF_CPTR)
#define TRBUFF_FULL()  (TRBUFF_TPTR == ((TRBUFF_CPTR - 1) & (TRBUFF_SIZE - 1)))
#define TRBUFF_INC(n)  ((n + 1) & (TRBUFF_SIZE - 1))

/* buffers */
static volatile unsigned char g_TWBuffer[TRBUFF_SIZE + TBUFF_PTR];
static volatile unsigned char g_TRBuffer[TWBUFF_SIZE + TBUFF_PTR];
/* g_TConn!=0 when debugguer is connected */
static volatile char g_TConn=0; 

void AsystPutch(char c)
{
  while(TWBUFF_FULL())
  {
    if(!g_TConn)
    {
      return;
    }
  }
  unsigned char nLen = TWBUFF_TPTR;
  g_TWBuffer[nLen] = c;
  nLen = TWBUFF_INC(nLen);
  TWBUFF_TPTR = nLen;
}

void AsystTrace(const char *p)
{
  for (; *p; p++)
  {
    AsystPutch(*p);
  }
}

int AsystKbhit()
{
  if(!g_TConn)
  {
    return 0;
  }
  return !TRBUFF_EMPTY(); 
}
 
char AsystGetch(void)
{
  char nChar;
  unsigned char nLen;
  while(!AsystKbhit());
  nChar = g_TRBuffer[TRBUFF_TPTR];
  nLen = TRBUFF_TPTR;
  nLen = TRBUFF_INC(nLen);
  TRBUFF_TPTR = nLen;
  return nChar;
}  

///////////////////////////////////////////////

void AsystPutch_ITM(char port, char c)
{
  enum 
  { 
    _ITM_PORT_BASE = 0xE0000000,
    _WAIT = 0x7FFF,      
  };
  unsigned long  *pdwITMSWIPortAddr = (unsigned long*)(_ITM_PORT_BASE + (port << 2));
  unsigned i = 0;
  for (; (i < _WAIT) && g_TConn; i++)
  {
    if (0 != *pdwITMSWIPortAddr)
    {
      *pdwITMSWIPortAddr = c;
      break;
    }
  }
}

void AsystTrace_ITM(char port, char *p)
{
  for (; *p; p++)
  {
    AsystPutch_ITM(port, *p);
  }
}
