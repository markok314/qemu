

# This script demonstrates reading of GPRs using readRegister() and
# SFRs with bitfields using method evaluate().
# It reads from a register specified in command line and prints its value.
#
# (c) iSystem AG, Nov 2010

from __future__ import print_function

import isystem.connect as ic
import time
import sys


def unsigned64(x):
  """
  This function performs a kind of cast from signed 64-bint int to unsigned one.
  """
  return x & 0xffffffffffffffff


# First we obtain connection and controller objects
cmgr = ic.ConnectionMgr()
cmgr.connectMRU('')

debugCtrl = ic.CDebugFacade(cmgr)

debugCtrl.download()

def printRegisters():
  for x in range(32):
    registerName = "r" + str(x)
    value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
    print(registerName, '=', f"{unsigned64(value.getLong()):#0{10}x}")

for x in range(4):
  printRegisters()
  print("----------")
  debugCtrl.stepInst()
printRegisters()