#include "endlessCoverageTest.h"

void endlessCoverageTest(void)
{
  endlessCoverageTestTransitionFlag = 0;
  
  endlessCoverageA();
  while(!endlessCoverageTestTransitionFlag);
  endlessCoverageB();
}

void endlessCoverageA()
{
  dummyFunction();     
}

void endlessCoverageB()
{
  dummyFunction();   
}