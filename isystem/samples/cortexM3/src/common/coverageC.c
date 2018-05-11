
#include "coverageA.h"

extern int g_profilerCCState;

void CoverageC()
{
  int iDummy = 0;
  int i;
  int iA = 1;
  int iB = 0;
  
  if (iA)
    iDummy++;
  
  if (iB)
    iDummy++;   // NE
  
  if (iA && iA)
    iDummy++;
  
  if (iB && iB)
    iDummy++;   // NE
  
  if (iA && iB)
    iDummy++;   // NE
  
  if (iB && iA) // PE
    iDummy++;   // NE
  
  if (iA || iB) // PE
    iDummy++;
  
  if (iB || iA)
    iDummy++;
  
  if (iA || (iB && iDummy)) // PE
    iDummy++;
  
  
  for (i = 0; i < 10; i++)
  {
    if (i % 2)
      iDummy++;
    else
      iDummy--;
  }
}


int doSomething()
{
  return 2*10;
}


void coverageTestMain()
{
    int i;
    g_profilerCCState = 2;
    
    
    for (i = 0; i < 10; i++) {
        doSomething();
    }
    
    CoverageC();

    g_profilerCCState = 3;
    CoverageA();
}
