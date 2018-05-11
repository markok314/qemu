
#include "projectDefs.h"

int g_nProfilerC = 0;
extern int g_profilerCCState;


void ProfilerC_1()
{
    #ifdef USE_FLOAT_TYPE
      float a = 22.0, b = 7.0;
      a /= b;  
      g_nProfilerC = 0x0001;
    #endif
}



void ProfilerC_2()
{
  g_nProfilerC = 0x0002;
  
  ProfilerC_1();
  
  g_nProfilerC = 0x0300;
}

void ProfilerC_Interrupt()
{
  g_nProfilerC = 0xABCD;
}


void ProfilerC(int nLoops)
{
  int nLoop;
  for (nLoop = 0; nLoop < nLoops; ++nLoop)
  {
    ProfilerC_1();
    ProfilerC_2();
  }
}


void profilerTestMain(int nLoops)
{
    g_profilerCCState = 4;
    ProfilerC(nLoops);
    g_profilerCCState = 5;
}
