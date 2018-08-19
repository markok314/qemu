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

def addFileForDownload(filePath, fileType):
    idx = ideCtrl.addDynamicOption('/IDE/Debug.DownloadFiles.File')
    ideCtrl.setOption('/IDE/Debug.DownloadFiles.File[' + str(idx) + '].Path', filePath)
    ideCtrl.setOption('/IDE/Debug.DownloadFiles.File[' + str(idx) + '].Options.Type', fileType)


def removeFileForDownload(filePath):
    idx = ideCtrl.findDynamicOption('/IDE/Debug.DownloadFiles.File', 'Path', filePath)
    ideCtrl.removeDynamicOption('/IDE/Debug.DownloadFiles.File', idx)

def printRegisters():
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

def check_registers_blue_box(file, num_of_inst_to_print):
    sys.stdout = open('log_blubox.log', 'w')
    cmgr = ic.ConnectionMgr()
    cmgr.connectMRU('')
    debugCtrl = ic.CDebugFacade(cmgr)
    dataCtrl = ic.CDataController2(cmgr)
    loadCtrl = ic.CLoaderController(cmgr)
    ideCtrl = ic.CIDEController(cmgr)
    downloadFile = '../' + file + '.elf'
    filetype = "ELF"
    addFileForDownload(downloadFile, filetype)

    debugCtrl.download()

    for x in range(num_of_inst_to_print):
      printRegisters()
      debugCtrl.stepInst()
    printRegisters()

    removeFileForDownload(downloadFile)

