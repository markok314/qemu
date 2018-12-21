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


    def _printRegisters(self, debugCtrl):
        print("----------")
        registerName = "pc"
        value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
        print(registerName, '=', f"{self._unsigned64(value.getLong()):#0{10}x}")
        registerName = "psw"
        value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
        print(registerName, '=', f"{self._unsigned64(value.getLong()):#0{10}x}")
        for x in range(32):
            registerName = "r" + str(x)
            value = debugCtrl.readRegister(ic.IConnectDebug.fRealTime, registerName)
            print(registerName, '=', f"{self._unsigned64(value.getLong()):#0{10}x}")


    def openConnection(self):
        cmgr = ic.ConnectionMgr()
        cmgr.connectMRU('')
        self.debugCtrl = ic.CDebugFacade(cmgr)
        self.ideCtrl = ic.CIDEController(cmgr)
        self.ec = ic.CExecutionController(cmgr)


    def check_registers_blue_box(self, file):
        sys.stdout = open('log_bluebox_'+file+'.log', 'w')
        downloadFile = '../test/' + file + '.elf'
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
            #sys.stdout = sys.__stdout__
            if new_pc == old_pc:
                self._printRegisters(self.debugCtrl)
                self.debugCtrl.stepInst()
                isEnd = False

            NUM_OF_INSTRUCTIONS = NUM_OF_INSTRUCTIONS + 1
            self._printRegisters(self.debugCtrl)
            self.debugCtrl.stepInst()

            #status = self.ec.getCPUStatus(True)
            #print(hex(status.getExecutionPoint()))
            #print(hex(addr.getAddress()))
            old_pc = new_pc

        self._removeFileForDownload(downloadFile, self.ideCtrl)
        return NUM_OF_INSTRUCTIONS

