#include "endlessCoverageTest.h"

void endlessCoverageTest()
{
  endlessCoverageTestTransitionFlag = 0;
  
  endlessCoverageA();
  while(!endlessCoverageTestTransitionFlag);
  endlessCoverageB();
}

void endlessCoverageA()
{
  hotAttachMain();     
}

void endlessCoverageB()
{
  hotAttachMain();   
}
