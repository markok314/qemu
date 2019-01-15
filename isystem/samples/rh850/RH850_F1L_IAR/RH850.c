#include "RH850.h"
#include "common.h"

char GpioOffOn = 0;

extern void ProfilerC_Interrupt();

void InterruptRoutine()
{
  //rt TimerInterrupt: profilerFunctionName=ProfilerC_Interrupt, timeoutMSec=0.96, maxDeltaPeriodPct=5
  //rt DaqTimerInterrupt: daqItem=daqInterruptCounter, periodInMicroSeconds=48800, maxDeltaPeriodInPercents=4
  //rt FunctionMinGrossTime: toleranceInPercent = 1 
  //rt FunctionMaxGrossTime: toleranceInPercent = 1
  //rt FunctionMinNetTime: toleranceInPercent = 1 
  //rt FunctionMaxNetTime: toleranceInPercent = 1
  iInterruptCounter++;
 
  //GPIOPortToggle();
  ProfilerC_Interrupt();
  
  if (IS_INTERRUPT_COUNTER_FOR_DAQ())
  {
    daqInterruptCounter++;
    GPIOPortToggle();
  }
  else
  {
  }  
}

void CPU_Init()
{
  __DI(); //disable interrupts
  InitOSTM();
  InitGPIO();
}

void InitGPIO()
{
  PM0 = 0x0;
  P0 = 0xFFFF;
}

void InitOSTM()
{
  OSTM0CMP=0x00000E00;  // load compare value
  OSTM0CTL=0x1;           // select interrupt mode
  ICOSTM0=0x0;     // disable interrupt masking
}

#pragma vector=0x100
__interrupt void TimerInterruptHandler(void)
{
  __DI();
  InterruptRoutine();
  __EI();
}

void GPIOPortToggle()
{
  GpioOffOn = !GpioOffOn;
  if (GpioOffOn)
  { 
    P0 = 0xFFFF; //pin 20 
  }
  else
  {
    P0 = 0x00; //pin 20 
  }
}

void profilerAddTestMessages(void)
{
}


void targetEnableInterrupts()
{
  OSTM0TS=0x1;   // start trigger
  __EI();        // unmask interrupts
}

void targetDisableInterrupts()
{
  __DI();
}

void targetInit()
{
  targetDisableInterrupts();
  CPU_Init();
}

void targetShutdown()
{
}

//missing function definitions
void __iar_system_Mtxdst(){while(1);}
void __iar_system_Mtxinit(){while(1);}
void __iar_system_Mtxlock(){while(1);}
void __iar_system_Mtxunlock(){while(1);}
void __iar_file_Mtxinit(){while(1);}
void __iar_file_Mtxdst(){while(1);}
void __iar_file_Mtxlock(){while(1);}
void __iar_file_Mtxunlock(){while(1);}
__cc_version1 unsigned char __low_level_init()
{
  return 0;
}






