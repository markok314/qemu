void myCustomTest(void);
char g_cCustomTestBreakPoint;
char isEndlessCoverageTest;
extern void endlessCoverageTest();

void customTestsMain(void)
{
  myCustomTest();
  
  if(isEndlessCoverageTest)
  {
    endlessCoverageTest();
  }
}

void myCustomTest(void) //rt TargetTestsInfo: id=myCustomTest, name=myCustomTest
{
	g_cCustomTestBreakPoint = 10;  //rt TestPoint: id=myCustomTestTP, symbol=g_cCustomTestBreakPoint, value=10 
	g_cCustomTestBreakPoint++;
}
