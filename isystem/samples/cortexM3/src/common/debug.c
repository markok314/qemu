#include "projectDefs.h"
#include "mpc5554.h"

#include "math.h"
#include "itest.h"
#include "target.h"
#include <stdint.h>
/**
   This file contains code for testing of debug functionality.
*/

extern struct struct_t g_struct;

int iCounter = 0;
char g_mode = 0;
short g_timer = 0;
int iForRWMemTest = 0;
int adIn = 0;
int outputValue = 0;

int sysTestVal_A = 0;
int sysTestVal_B = 0;

int testMem1 = 0;   // used to test RW memory 
int testMem2 = 0x1; // used to test RW memory

#ifdef USE_FLOAT_TYPE
float g_voltage = 2;
#endif

struct strA
{
    int m_iA;
    char m_iB;
    char m_a[4];
    long m_l;
};

union uniA
{
    long m_l;
    char m_a[4];
};

struct bitA 
{
    int m_b1:3;
    int m_b2:2;
    int m_b3:5;
    int m_b4:4;
};

struct strX
{
    int m_a;
    struct strX *m_pS;
    union uniA m_u;
    char m_c[2];
};


typedef enum 
{
    TE_INIT,
    TE_RUNNING,
    TE_STOPPED,
    TE_UNDEFINED,
    TE_FINISHED
} enumA;

enumA g_enumA;


int Factorial(int i) 
{
    if (i == 1) {
        return i;
    }
    return Factorial(i-1)*i;
}


void CPU_Recursion()
{
    g_enumA = TE_INIT;
    int x = 0;
    ++x;
    g_enumA = TE_RUNNING;
    x=Factorial(5); // x must be 120
    ++x;
    ++iCounter;
    g_enumA = TE_FINISHED;
}


void Type_Simple()
{
    char c = 0;
    int i = 0;
    int iCount = 0;
    
    #ifdef USE_FLOAT_TYPE
      g_voltage = (i + 1) *3.2 + 4.56;
      i = g_voltage;
      float f = (float)0.0;
      double d = 1;
    #endif

    for (iCount = -2; iCount <= 2; ++iCount)
    {
        c = iCount+3;
        i = iCount;
        #ifdef USE_FLOAT_TYPE
          d = c;
          f = (float)iCount;
        #endif
    }


    i = 0x1234;
    #ifdef USE_FLOAT_TYPE
      f = (float)1.345678;
    #endif
    
    ++iCounter;
    outputValue = (char)(d + f);
}


void Type_Arrays()
{
    char a[2][3][4];
    int i,j,k;

    for (i = 0;i<2;++i)
        for (j = 0;j<3;++j)
            for (k = 0;k<4;++k)
                a[i][j][k] = i+j+k;

    ++iCounter;
    outputValue = a[0][0][0];
}			


void Type_Pointers()
{
    char c;
    char *pC;
    char **ppC;

    c = 'A';
    pC = &c;
    ++c;
    ++*pC;

    ppC = &pC;
    ++(**ppC);

    ++iCounter;
}


void Type_Struct()
{
    struct strA sA;
    union uniA uA;
    int i;

    for (i = 0;i<4;++i)
    {
        sA.m_iA = i;
        sA.m_iB = i;
        sA.m_a[i] = i;
        sA.m_l = (long)i;
    }


    uA.m_l = 0x12345678L;
    uA.m_a[0] = 0x11;
    uA.m_a[1] = 0x22;
    uA.m_a[2] = 0x33;
    uA.m_a[3] = 0x44;

    ++iCounter;
    outputValue = sA.m_iA + sA.m_iB + uA.m_a[0];
}


void Type_Bitfields()
{
    struct bitA bA;

    bA.m_b1 = -1;
    bA.m_b2 = 3;
    bA.m_b3 = 7;
    bA.m_b4 = -3;

    ++iCounter;
    outputValue = bA.m_b1;
}


void Type_Enum()
{
  
    int a = TE_INIT;
    int b = TE_RUNNING;
    ++a;
    ++b;
  
    g_enumA = TE_STOPPED;
    
    if (iCounter % 3) {
        g_enumA = TE_FINISHED;
    }

    if (iCounter % 4) {
        g_enumA = TE_UNDEFINED;
    }

    ++iCounter;
}                           
                    

int Mult(int x,int y)
{
    return x*y;
}


void Type_FunctionPointer()
{
    int (*pMult)(int, int);
    int a = 0;

    pMult = Mult;

    a = Mult(pMult(1,2), pMult(3,4));

    ++iCounter;
    outputValue = a;
}


void ResetStrX(struct strX* pS,int iTo)
{
    pS->m_a = iTo;
    pS->m_pS = 0;
    pS->m_u.m_l = (long)iTo;
    pS->m_c[0] = pS->m_c[1] = (char)iTo;
}


void Type_Mixed()
{
    struct strX sX1;
    struct strX sX2;

    ResetStrX(&sX1, 1);
    ResetStrX(&sX2, 2);
    sX1.m_pS = &sX2;

    ++iCounter;
    
    #ifdef USE_FLOAT_TYPE
        g_voltage += 2;
    #endif
}

/*#
  - desc: 'This test shows how to set and test function static variables.'
    init:
      TestStatic##staticI: 4
    func: [TestStatic]
    expect:
    - TestStatic##staticI == 5
 */
void TestStatic()
{
    static int staticI = 0;
    ++staticI;
}


void Address_GlobalVariables()
{
    int d = iCounter & 0x3f;
    adIn = d* d * d;
    outputValue = (iCounter >> 8) & 0xff;
    
    ++iCounter;
  
    TestStatic();
    
}


void Address_TestScopes()
{
    int i = 0, j = 0;
    union uniA X;
	
    X.m_l = 0x77;

    for (i = 0; i < 2; ++i)
    {
        char c = 4;
        X.m_l++;
        
        for (j = 0; j < 2; ++j)
        {
            int X;
            ++c;
            X = c;
            if (c == 1)
                ++X;
            else
                X += 2;
        }
    }

    ++X.m_l;
    ++iCounter;
}


/*#
    - desc: 'This tests shows coverage measurement.'
      tags:
      - Dataet-1
      func: [Func1, [], rv]
      params: [10]
      expect:
      - rv == 20
      coverage:
        runMode: start
        document: 'func1.ccv'
        openMode: w
        isSaveAfterTest: true
        isCloseAfterTest: false
        isDecisionCoverage: true
        statistics:
        - func: 'Func1'
          code: 100
          branches: 100
          taken: 100
          notTaken: 100
          both: 100
 */
int Func1(int i)
{
    int x = 0;

    for (;x < 10; ++x) {
        ++i;
        g_mode++;
    }

    return i;
}

int g_profilerTimer = 0;

long Func2(int i,char c, long l)
{
    g_profilerTimer = 0;
    g_struct.m_i++;
    // make it a state variable with only few values
    if (g_struct.m_i > 0) { 
        g_struct.m_i = 0;
        // i = l * 3 + c * i;
    }
    int x = 0;
    long y = 0;

    for (x = 0;   // split to multiple lines to make HW BPs possible 
         x < 10;
         ++x) {
        y += x + i + c + l;
    }
  
    g_profilerTimer = 1;
    return 0;
}

void StubForFunc3(long *pY)
{
    // g_timer is not incremented in stub 
    *pY = 0;
}


void Func3(long *pY)
{
    g_timer++;
    if (g_timer > 3000) {
        g_timer = 0;
    }
    
    *pY = 0;
}

#ifdef USE_FLOAT_TYPE
    float Func4(float f,char *pC,long l, float d)
    {
        float fRet = (float)0.0;
        int i, j;
        i = 5 & 2 && 3 > 5;
        j = i + (int)f;
        for (i = 0;i<5;++i)
        {
            *(pC+i) = 0xA+i;
            fRet+=f+(float)*(pC+i)+(float)l + (float)d;         
        }

        return fRet + j;
    }
#else

    /*#
     - desc: 'This demonstrates test of a function, which uses local variables as parameter.'
       locals:
         pC: 'char *'
       init:
         pC: '&g_char1'
       func: [Func4, [], rv]
       params: [3, pC, 1, 2]
       expect:
       - rv == 93
     */
    int Func4(int f,char *pC,long l, int d) // removed float
    {
        int fRet = 0;
        int i;
    
        for (i = 0;i<5;++i)
        {
            l = 0xA + i;
            fRet+=f + l + d;         
        } 
    
        return fRet;
    }
#endif




void Address_DifferentFunctionParameters()
{
    int x;
    long y;
    long *pY;
    #ifndef USE_SIMPLE_CHAR
      char c[5] = {1,2,3,4,5};
    #else
      char c=1;
    #endif
    
    #ifdef USE_FLOAT_TYPE
      float f = (float)3.14;
    #endif
    //double d = f;

    //f+ = c[0]+sin(d*d);
    x = Func1(5);

    y = Func2(1,2,3);
	
    pY = &y;
    Func3(pY);
    
    #ifdef USE_FLOAT_TYPE
        Func4((float)1.1, c, (long)100, (float)5);
    #else
        Func4(11, c, 100, 5); //float removed
    #endif
    
    if (x + f > 3) {
        ++iCounter;
    }
}


/* This function was added, because of pipeline problems on RTR.
   For example, trigger in trace/profiler on targetInit()
   was sometimes missed (in about 1/3 of runs), before this call was added
   before targetInit() in main(). On AT91RM9200 it was similar for
   missed last sample (RTR) when exeution stopped on targetShutdown()
   after profiling.
*/
int dummyFunction(int n)
{
  int i = 10;
  int j = 0;
  for (i = 0; i < n; i++) {
    j += i * 3;
  }
  return j;
}

/*#
- desc: 'This test shows how to test return value of a function.'
  func: [min_int, [], rv]
  params: [3, 5]
  expect:
  - rv == 3
 */
int min_int(int a, int b)
{
  if (a < b)
    return a;
  
  return b;
}


/*#
- desc: 'This test shows usage of profiler.'
  func: [max_int, [], rv]
  params: [8, 456]
  expect:
  - rv == 456
  profiler:
    runMode: start
    outFile: '${wiWorkspacePath}/max_int.profiler'
    isForceOverwrite: true
    isSaveHistory: true
    codeAreas:
    - name: 'max_int'
      minTime:
      - 4000
      - 5000
      maxTime:
      - 4000
      - 5000
      totalTime:
      - 4000
      - 5000
      hits:
      - 1
      - 1
  tests:
  - params: [-2, -4]
    expect:
    - rv == -2
  - params: [8, 2]
    expect:
    - rv == 8
    profiler:
      runMode: start
      outFile: '${wiWorkspacePath}/max_int.profiler'
      isForceOverwrite: true
      isSaveHistory: true
      codeAreas:
      - name: 'max_int'
        minTime:
        - 4001
        - 5001
        maxTime:
        - 4002
        - 5002
        totalTime:
        - 4003
        - 5003
        hits:
        - 1
        - 4
*/
int max_int(int a, int b)
{
  iCounter++;
  iCounter++;
  
  if (a < b)
    return b;
  
  return a;
}


// see also itest.c 
static int static_func(int a, int b) 
{
  return a - b;
}


int sysTest_stubbed() {
  return 10;
}

int sysTest_withTestPoint(int val)
{
    int x = val;
    
    x += 2; // sysTestTPId
    
    return x;
}


void sysTest_main()
{
    sysTestVal_A = sysTest_stubbed();
    sysTestVal_B = sysTest_withTestPoint(11);
}


void debugMain() {

    iCounter = 0;
    iForRWMemTest = 1;
    testMem1 = 0;
    testMem2 = 0x12345678;
    static_func(9, 4);

    while(1)
    {
        int i;
        Type_Simple();
        Type_Arrays();
        for(i = 0; i<25; i++)
        {
            Type_Pointers();
            Type_Struct();
            Type_Bitfields();
            Type_Enum();
            Type_FunctionPointer();
            Type_Mixed();
            min_int(12, i);
            max_int(12, i);
      
            Address_GlobalVariables();
            Address_TestScopes();
            Address_DifferentFunctionParameters();
            add_int(3, i);
            sysTest_main();
            targetDebug(); // calls target specific function
        }
    
        CPU_Recursion();                          
    }
  
}









