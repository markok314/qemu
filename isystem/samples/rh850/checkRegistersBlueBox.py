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
dataCtrl = ic.CDataController2(cmgr)
loadCtrl = ic.CLoaderController(cmgr)
ideCtrl = ic.CIDEController(cmgr)

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

downloadFile = '../' + sys.argv[1] + '.elf'
filetype = "ELF"
addFileForDownload(downloadFile, filetype)

debugCtrl.download()

for x in range(int(sys.argv[2])):
  printRegisters()
  debugCtrl.stepInst()
printRegisters()

removeFileForDownload(downloadFile)

