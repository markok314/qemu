# This script loads file, executes some instructions and then write PC,PSW and GPR values
# to file.

import isystem.connect as ic
import time
import sys


class BlueBox:
    def _unsigned64(self, x):
      """
      This function performs a kind of cast from signed 64-bint int to unsigned one.
      """
      return x & 0xffffffffffffffff


    def _addFileForDownload(self, filePath, fileType, ideCtrl):
        idx = ideCtrl.addDynamicOption('/IDE/Debug.DownloadFiles.File')
        ideCtrl.setOption('/IDE/Debug.DownloadFiles.File[' + str(idx) + '].Path', filePath)
        ideCtrl.setOption('/IDE/Debug.DownloadFiles.File[' + str(idx) + '].Options.Type', fileType)


    def _removeFileForDownload(self, filePath, ideCtrl):
        idx = ideCtrl.findDynamicOption('/IDE/Debug.DownloadFiles.File', 'Path', filePath)
        ideCtrl.removeDynamicOption('/IDE/Debug.DownloadFiles.File', idx)


    def _printRegisters(self, debugCtrl, bbLogFile):
        print("----------", file=bbLogFile)
        registerName = "pc"
        value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
        print(registerName, '=', f"{self._unsigned64(value.getLong()):#0{10}x}", file=bbLogFile)
        registerName = "psw"
        value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
        print(registerName, '=', f"{self._unsigned64(value.getLong()):#0{10}x}", file=bbLogFile)
        for x in range(32):
            registerName = "r" + str(x)
            value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
            print(registerName, '=', f"{self._unsigned64(value.getLong()):#0{10}x}", file=bbLogFile)


    def openConnection(self):
        cmgr = ic.ConnectionMgr()
        cmgr.connectMRU('')
        self.debugCtrl = ic.CDebugFacade(cmgr)
        self.ideCtrl = ic.CIDEController(cmgr)


    def check_registers_blue_box(self, file):

        with open('log_bluebox_'+file+'.log', 'w') as bbLogFile:

            downloadFile = '../test/bin/' + file + '.elf'
            filetype = "ELF"
            self._addFileForDownload(downloadFile, filetype, self.ideCtrl)

            self.debugCtrl.download()

            isEnd = True
            old_pc = -1
            NUM_OF_INSTRUCTIONS = -1

            while isEnd:
                registerName = "pc"
                new_pc = self.debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
                new_pc = f"{self._unsigned64(new_pc.getLong()):#0{10}x}"
                if new_pc == old_pc:
                    self._printRegisters(self.debugCtrl, bbLogFile)
                    self.debugCtrl.stepInst()
                    isEnd = False

                NUM_OF_INSTRUCTIONS = NUM_OF_INSTRUCTIONS + 1
                self._printRegisters(self.debugCtrl, bbLogFile)
                self.debugCtrl.stepInst()

                old_pc = new_pc

            self._removeFileForDownload(downloadFile, self.ideCtrl)
            return NUM_OF_INSTRUCTIONS

