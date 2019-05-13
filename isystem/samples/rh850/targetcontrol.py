# This script loads file, executes some instructions and then write PC,PSW and GPR values
# to file.

import isystem.connect as ic
import time
import sys
import os


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
        # sp.Popen(['/home/isystem/bin/9_17_109_0_89947/winidea.sh', self.workspaceFileName])
        cmgr = ic.ConnectionMgr()
        cmgr.connectMRU(self.workspaceFileName)
        self.debugCtrl = ic.CDebugFacade(cmgr)
        self.ideCtrl = ic.CIDEController(cmgr)
        self.loaderCtrl = ic.CLoaderController(cmgr)


    def initWinIDEA(self, ):
        self._connectToWinIDEA()


    def initTarget(self, asmFileStem):

        downloadFile = os.path.join(os.getcwd(), 'test/bin',  asmFileStem + '.elf')
        filetype = "ELF"
        self.loaderCtrl.clearDownloadList(ic.CLoaderController.DLIST_PRIMARY)
        self._addFileForDownload(downloadFile, filetype)

        self.loaderCtrl.download()
        self.debugCtrl.waitUntilStopped()

        # The first N instructions are used to initialize all registers, which have
        # random value on reset, so execute them in one big step.
        self.debugCtrl.setBP(0, 0x3e)
        self.debugCtrl.run()
        self.debugCtrl.waitUntilStopped()


    def readRegisters(self, registerNames, pcName, pswName):
        REG_PREFIX = '@' 
        regValues = []
        for reg in registerNames:
            value = self.debugCtrl.evaluate(ic.IConnectDebug.fRealTime, REG_PREFIX + reg.upper())
            regValues.append(self._unsigned64(value.getLong()))

        pc = self.debugCtrl.evaluate(ic.IConnectDebug.fRealTime, REG_PREFIX + pcName.upper())
        psw = self.debugCtrl.evaluate(ic.IConnectDebug.fRealTime, REG_PREFIX + pswName.upper())
        
        return regValues, self._unsigned64(pc.getLong()), self._unsigned64(psw.getLong())
                

    def step(self):
        self.debugCtrl.stepInst()


