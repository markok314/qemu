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
        self.loaderCtrl = ic.CLoaderController(cmgr)


    def check_registers_blue_box(self, asmFileStem, blueBoxLogFName):

        with open(blueBoxLogFName, 'w') as bbLogFile:

            downloadFile = '../test/bin/' + asmFileStem + '.elf'
            filetype = "ELF"
            self.loaderCtrl.clearDownloadList(ic.CLoaderController.DLIST_PRIMARY)
            self._addFileForDownload(downloadFile, filetype, self.ideCtrl)

            self.debugCtrl.download()

            old_pc = -1
            new_pc = 0
            NUM_OF_INSTRUCTIONS = -1

            # Test end is signalled with:
            #     lbl:  br lbl
            # This means PC does not change after instruction execution.
            while old_pc != new_pc:

                NUM_OF_INSTRUCTIONS = NUM_OF_INSTRUCTIONS + 1
                self._printRegisters(self.debugCtrl, bbLogFile)
                self.debugCtrl.stepInst()

                old_pc = new_pc
                new_pc = self.debugCtrl.readRegister(ic.IConnectDebug.fRealTime, "pc")
                new_pc = self._unsigned64(new_pc.getLong())
                print('#', end='')  # indicate progress
                sys.stdout.flush()

            print()

            return NUM_OF_INSTRUCTIONS

