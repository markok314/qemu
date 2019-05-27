# This script is used to test GDB support on QEMU for target RH850

import isystem.connect as ic
import os
import sys
import time
import subprocess as sp
import targetcontrol

workspacePath = "./RH850_F1L_IAR/SampleGDB.xjrf"
winidea = "/home/qemu/bin/winIDEA/winidea.sh"

# Compile assembly code
cmd = ['./build.sh ' + './rk_arithmetic_test' + ' > ./winidea.log']
sp.check_call(cmd, shell=True, executable='/bin/bash', cwd='test')
print("Test compiled successfully.")

# Start QEMU
print("Starting QEMU...")
cmd = ['../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -S -singlestep -kernel ./test/bin/rk_arithmetic_test.elf']
sp.Popen(cmd, shell=True, executable='/bin/bash')


# Start winIDEA and connect to it
cmgr = ic.ConnectionMgr()
print("Starting winIDEA")
cmgr.connectMRU(workspacePath)
print("Connected to winIDEA  ")


loader = ic.CLoaderController(cmgr)
dlConfig = ic.CDownloadConfiguration()



# Clear winIDEAs download list
loader.clearDownloadList(ic.CLoaderController.DLIST_PRIMARY)

# Add to download list
loader.addToDownloadList(dlConfig, ic.CLoaderController.DLIST_PRIMARY, 'test/bin/rk_arithmetic_test.elf', '')

# Download
success = loader.download()
if success == 0:
	print("Download successfull.")

