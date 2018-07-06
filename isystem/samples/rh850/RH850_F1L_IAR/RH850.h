#include <intrinsics.h>
#pragma language=extended

#include <iorh850.h>

extern void watchdogUpdate();

void InitClocks();
void InitOSTM();
void InitGPIO();
void GPIOPortToggle();
__interrupt void TimerInterruptHandler();






