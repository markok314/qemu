#pragma once

/*
  This file defines interface for target specific code.
 */


extern int iInterruprCounter;

/** Should contain target specific initialization code. This function is the
    first one called fro mmain().
 */
void targetInit();


/** Should enable interrupts for trace/coverage/profiler tests.
    Interrupt function should also be defined in this file, but it is
    not part of public interface. The requirement for interrupt function
    is, that it increments iInterruprCounter when called.
 */
void targetEnableInterrupts();


/**
   This function is periodically called from debug test infinite loop.
 */
void targetDebug();


/**
   Any action required at end of tests
*/
void targetShutdown();

