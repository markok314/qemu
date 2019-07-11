
#include "target.h"
#include "projectDefs.h"
#include "pidController.h"

#include "debug.h"
#include "coverageC.h"
#include "profilerC.h"
#include "itest.h"
#include "AsystTrace.h"

#include <string.h>

/* These vars must be initialized by test scripts, before run
   because some targets do not support static initialization. */
int isInterruptsEnabled=0;
int isDebugTest=0;
int isITest=0;
int isTraceTest=0;
int isProfilerTest=0;
int isCoverageTest=0;
int isControllerTest = 0;

int g_ProfilerLoopCount; // as many iterations as the trace can capture
int g_profilerCCState;
  
char appName[] = "testApp";
char *appNamePtr = appName;


int main(void)
{
    strcpy(appName, "testApp");
    appNamePtr = appName;
    g_profilerCCState = 1;
    
    dummyFunction(1);

    // Configure terminal as Shared Memory in Tools | Options | Terminal.
    // Works on Cortex only, no effect on MPC.
    AsystTrace("shared mem terminal print");
    
    targetInit();  // sets target clock and performs other required initialization

    if (isInterruptsEnabled) {
        targetEnableInterrupts();
    }

    if (isDebugTest) {
        debugMain();
    } 
    
    initGlobals();
    
    if (isTraceTest) {
        // trace is tested on the same code as debug 
        debugMain();
    }
    
    if (isCoverageTest) {
        coverageTestMain();
    } 
   
    if (isProfilerTest) {
        profilerTestMain(g_ProfilerLoopCount);
    }

    #ifdef USE_FLOAT_TYPE
        if (isControllerTest) {
            controllerLoop();
        }
    #endif
    
    if (isITest) {
        iTestMain();
    }         
    
    dummyFunction(100);

    targetShutdown();
    
    for (;isITest < 10;) {
        dummyFunction(100);
    }
}
