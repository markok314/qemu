# This script is used to test GDB support on QEMU for target RH850

import isystem.connect as ic
import os
import sys
import time
import subprocess as sp
import targetcontrol

# workspacePath = "./RH850_F1L_IAR/SampleGDB.xjrf"
# winidea = "/home/qemu/bin/winIDEA/winidea.sh"

# workspace on win10
workspacePath = "C:/Users/student2/proj/qemu/isystem/samples/cortexM3/SampleSTM32-GDB.xjrf"
winidea = "winidea"

# Compile assembly code
# cmd = ['./build.sh ' + './rk_arithmetic_test' + ' > ./winidea.log']
# sp.check_call(cmd, shell=True, executable='/bin/bash', cwd='test')
# print("Test compiled successfully.")

# Start QEMU
# print("Starting QEMU...")
# cmd = ['../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -S -singlestep -kernel ./test/bin/rk_arithmetic_test.elf']
# sp.Popen(cmd, shell=True, executable='/bin/bash')

i = 99099999
# Start QEMU on win10
print("Starting QEMU...")
cmd = 'C:/Users/student2/Desktop\VBshared/qemu-system-arm.exe -M lm3s6965evb -m 256M -s -S -kernel C:/Users/student2/proj/qemu/isystem/samples/cortexM3/build/exe/stm32/release/lm3s.elf'
p = sp.Popen(cmd, shell=True)

# Start winIDEA and connect to it
cmgr = ic.ConnectionMgr()
print("Starting winIDEA")
cmgr.connectMRU(workspacePath)
print("Connected to winIDEA")


loader = ic.CLoaderController(cmgr)
dlConfig = ic.CDownloadConfiguration()
execCtrl = ic.CExecutionController(cmgr)
debugCtrl = ic.CDebugFacade(cmgr)
brkCtrl = ic.CBreakpointController(cmgr)

# Clear winIDEAs download list
loader.clearDownloadList(ic.CLoaderController.DLIST_PRIMARY)

# Add to download list
# loader.addToDownloadList(dlConfig, ic.CLoaderController.DLIST_PRIMARY, 'test/bin/rk_arithmetic_test.elf', '')

# Add to download list on win10
loader.addToDownloadList(dlConfig, ic.CLoaderController.DLIST_PRIMARY, '../cortexM3/build/exe/stm32/release/lm3s.elf', '')

# Download
success = loader.download()
if success == 0:
	print("Download successfull.")

# Goto address
execCtrl.gotoAddress(0, 0x68 )

# Delete all brakpoints
brkCtrl.deleteAll()

# Set breakpoint (do this while stopped and while running)
bc.setBP(37, r'src/common/main.c')

# Delete breakpoint
bc.deleteBP(0, bpAddr)

# Run
execCtrl.run()

# Step into
debugCtrl.stepInst()

# Step over
debugCtrl.stepInstOver()



