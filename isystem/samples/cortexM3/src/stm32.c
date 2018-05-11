
int iInterruptCounter;


char cTestBuffer[5];  //used for iConnectTestSuite for testing R/W memory and...
short sShortBPTest0;  //...for testing access BPs 
short sShortBPTest1;
char cByteBPTest0; 
char cByteBPTest1; 

int iInterruprCounter;

// not found in library - check why not!

void *memcpy(void *dest, const void *src, unsigned len)
{
  char *d = (char *)dest;
  const char *s = (const char *)src;
  
  for (unsigned i = 0; i < len; i++) {
    *d++ = *s++;
  }
  
  return dest;
} 



void __eabi()
{
}


void InterruptRoutine()
{
    iInterruptCounter++;
}


void targetInit()
{
    cTestBuffer[0]=0x11;
    cTestBuffer[1]=0x22;
    cTestBuffer[2]=0x33;
    cTestBuffer[3]=0x44;
    cTestBuffer[4]=0x55;
  
    sShortBPTest0=0;
    sShortBPTest1=0;
    cByteBPTest0=0;
    cByteBPTest1=0;
}


void targetEnableInterrupts()
{
}


void targetDebug()
{
    ++sShortBPTest0;
    ++sShortBPTest1;
    ++cByteBPTest0;
    ++cByteBPTest1;
  
    sShortBPTest0 = sShortBPTest1;
    cByteBPTest0 = cByteBPTest1;
}


void targetShutdown()
{
}



/* Declared as C function, because
 * on MPC5554 this is assmebler function and it's name is not mangled.
 */
extern "C" {
  void CoverageA()
  {
    return;
  }
}