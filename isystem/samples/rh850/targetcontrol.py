# This script loads file, executes some instructions and then write PC,PSW and GPR values
# to file.

import isystem.connect as ic
import time
import sys
import os

TRAP_MASK = 0xffffffe0
TRAP_OPCODE = 0x010007e0
               
SYSCALL_MASK = 0xc7ffffe0
SYSCALL_OPCODE = 0x0160d7e0

CALLT_MASK = 0xffc0
CALLT_OPCODE = 0x0200

FETRAP_MASK = 0x87ff
FETRAP_OPCODE = 0x0040

RIE_MASK_16bit = 0xffff
RIE_OPCODE_16bit = 0x0040

RIE_MASK_32bit = 0xffff07f0
RIE_OPCODE_32bit = 0x000007f0

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
        endOfInitAddr = 0x78    # adjust this address if modifying gpr_init.s
        self.debugCtrl.setBP(0, endOfInitAddr)  

        while (True):    # repeat, because QEMU stops execution on every branch
            self.debugCtrl.run()
            self.debugCtrl.waitUntilStopped()
            pc = self.debugCtrl.getCPUStatus().getExecutionPoint()
            if pc == endOfInitAddr:
                break


    def readRegisters(self, registerNames, pcName):
        REG_PREFIX = '@' 
        regValues = []
        for reg in registerNames:
            value = self.debugCtrl.evaluate(ic.IConnectDebug.fMonitor, REG_PREFIX + reg.upper())
            regValues.append(self._unsigned64(value.getLong()))

        pc = self.debugCtrl.evaluate(ic.IConnectDebug.fMonitor, REG_PREFIX + pcName.upper())
        
        return regValues, self._unsigned64(pc.getLong())
                

    def step(self):
        self.debugCtrl.stepInst()


    def isDoubleStepInst(self, pc):
        """
        After some instructions real device stops only on the second instruction after the
        instruction: TRAP, SYSCALL, HVTRAP, CALLT, FETRAP, HVCALL, RIE
        """
        stype = ic.SType()
        stype.m_byType = ic.SType.tUnsigned;
        stype.m_byBitSize = 32
        opCodeInt = self.debugCtrl.readValue(ic.IConnectDebug.fMonitor, 0, pc, stype).getLong()

        opCode = self._unsigned64(opCodeInt)

        return (opCode & TRAP_MASK == TRAP_OPCODE or
                opCode & SYSCALL_MASK == SYSCALL_OPCODE or
		opCode & CALLT_MASK == CALLT_OPCODE or
		opCode & FETRAP_MASK == FETRAP_OPCODE or
		opCode & RIE_MASK_16bit == RIE_OPCODE_16bit or
		opCode & RIE_MASK_32bit == RIE_OPCODE_32bit)

