# This script loads file, executes some instructions and then write PC,PSW and GPR values
# to file.

import isystem.connect as ic
import time
import sys


class TargetController:

    def __init__(self, workspaceFileName):
        self.workspaceFileName = workspaceFileName


    def _unsigned64(self, x):
      """
      This function performs a kind of cast from signed 64-bint int to unsigned one.
      """
      return x & 0xffffffffffffffff


    def _addFileForDownload(self, filePath, fileType):
        idx = self.ideCtrl.addDynamicOption('/IDE/Debug.DownloadFiles.File')
        self.ideCtrl.setOption('/IDE/Debug.DownloadFiles.File[' + str(idx) + '].Path', filePath)
        self.ideCtrl.setOption('/IDE/Debug.DownloadFiles.File[' + str(idx) + '].Options.Type', fileType)


    def _connectToWinIDEA(self):
        cmgr = ic.ConnectionMgr()
        cmgr.connectMRU(self.workspaceFileName)
        self.debugCtrl = ic.CDebugFacade(cmgr)
        self.ideCtrl = ic.CIDEController(cmgr)
        self.loaderCtrl = ic.CLoaderController(cmgr)


    def initTarget(self):
        _connectToWinIDEA()

        downloadFile = 'test/bin/' + asmFileStem + '.elf'
        filetype = "ELF"
        self.loaderCtrl.clearDownloadList(ic.CLoaderController.DLIST_PRIMARY)
        self._addFileForDownload(downloadFile, filetype)

        self.loaderCtrl.download()


    def readRegisters(self, registerNames, pcName):
        regValues = []
        for reg in registerNames:
            value = self.debugCtrl.readRegister(ic.IConnectDebug.fRealTime, reg)
            regValues.append(self._unsigned64(value.getLong()))

        pc = self.debugCtrl.readRegister(ic.IConnectDebug.fRealTime, pcName)
        
        return regValues, self._unsigned64(pc.getLong())
                

    def step(self):
        self.debugCtrl.stepInst()


