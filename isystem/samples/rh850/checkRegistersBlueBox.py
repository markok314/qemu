# This script is part of files used for testing implementation of microcontroller rh850
# in qemu. It loads file, executes some instructions and then write PC,PSW and GPR values
# to file.

from __future__ import print_function
import isystem.connect as ic
import time
import sys


def unsigned64(x):
  """
  This function performs a kind of cast from signed 64-bint int to unsigned one.
  """
  return x & 0xffffffffffffffff

def addFileForDownload(filePath, fileType,ideCtrl):
    idx = ideCtrl.addDynamicOption('/IDE/Debug.DownloadFiles.File')
    ideCtrl.setOption('/IDE/Debug.DownloadFiles.File[' + str(idx) + '].Path', filePath)
    ideCtrl.setOption('/IDE/Debug.DownloadFiles.File[' + str(idx) + '].Options.Type', fileType)


def removeFileForDownload(filePath,ideCtrl):
    idx = ideCtrl.findDynamicOption('/IDE/Debug.DownloadFiles.File', 'Path', filePath)
    ideCtrl.removeDynamicOption('/IDE/Debug.DownloadFiles.File', idx)

def printRegisters(debugCtrl):
    print("----------")
    registerName = "pc"
    value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
    print(registerName, '=', f"{unsigned64(value.getLong()):#0{10}x}")
    registerName = "psw"
    value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
    print(registerName, '=', f"{unsigned64(value.getLong()):#0{10}x}")
    for x in range(32):
        registerName = "r" + str(x)
        value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
        print(registerName, '=', f"{unsigned64(value.getLong()):#0{10}x}")

def check_registers_blue_box(file):
    sys.stdout = open('log_bluebox_'+file+'.log', 'w')
    cmgr = ic.ConnectionMgr()
    cmgr.connectMRU('')
    debugCtrl = ic.CDebugFacade(cmgr)
    dataCtrl = ic.CDataController2(cmgr)
    loadCtrl = ic.CLoaderController(cmgr)
    ideCtrl = ic.CIDEController(cmgr)
    downloadFile = '../test/' + file + '.elf'
    filetype = "ELF"
    addFileForDownload(downloadFile, filetype,ideCtrl)
    ec = ic.CExecutionController(cmgr)

    debugCtrl.download()

    addrCtrl = ic.CAddressController(cmgr)

    isEnd = True
    old_pc = -1
    NUM_OF_INSTRUCTIONS = -1

    while isEnd:
        registerName = "pc"
        new_pc = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
        new_pc = f"{unsigned64(new_pc.getLong()):#0{10}x}"
        #sys.stdout = sys.__stdout__
        if new_pc == old_pc:
            printRegisters(debugCtrl)
            debugCtrl.stepInst()
            isEnd = False

        NUM_OF_INSTRUCTIONS = NUM_OF_INSTRUCTIONS + 1
        printRegisters(debugCtrl)
        debugCtrl.stepInst()

        #status = ec.getCPUStatus(True)
        #print(hex(status.getExecutionPoint()))
        #print(hex(addr.getAddress()))
        old_pc = new_pc

    removeFileForDownload(downloadFile,ideCtrl)
    return NUM_OF_INSTRUCTIONS
