# This script is used to test GDB support on QEMU for target RH850
# Currently it is set up for ARM and WIN10 due to issues with isystem.connect and winIDEA on Ubuntu
# 

import isystem.connect as ic
import os
import sys
import time
import subprocess as sp
import targetcontrol

# (Ubuntu) 
# workspacePath = "./RH850_F1L_IAR/SampleGDB.xjrf"
# winidea = "/home/qemu/bin/winIDEA/winidea.sh"

# Compile assembly code (Ubuntu)
# cmd = ['./build.sh ' + './rk_arithmetic_test' + ' > ./winidea.log']
# sp.check_call(cmd, shell=True, executable='/bin/bash', cwd='test')
# print("Test compiled successfully.")

# Start QEMU (Ubuntu)
# print("Starting QEMU...")
# cmd = ['../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -S -singlestep -kernel ./test/bin/rk_arithmetic_test.elf']
# sp.Popen(cmd, shell=True, executable='/bin/bash')

if len(sys.argv) < 4:
	print("Arguments missing!")
	print("Usage:")
	print("    python3 testGDB.py path_to_winIDEA path_to_winIDEA_workspace path_to_QEMU path_to_ELF_file")
	print("-----------------------------------------------------------------")
	print("Using default paths (Robert @ win10)")
	workspacePath = "C:/Users/student2/proj/qemu/isystem/samples/cortexM3/SampleSTM32-GDB.xjrf"
	winidea = "winidea"
	qemuPath = 'C:/Users/student2/Desktop\VBshared/qemu-system-arm.exe'
	elfPath = 'C:/Users/student2/proj/qemu/isystem/samples/cortexM3/build/exe/stm32/release/lm3s.elf'
else:
	winidea = sys.argv[1]
	workspacePath = sys.argv[2]
	qemuPath = sys.argv[3]
	elfPath = sys.argv[4]

time.sleep(3)

# Start QEMU on win10
print("Starting QEMU...")
cmd = qemuPath + ' -M lm3s6965evb -m 256M -s -S -kernel ' + elfPath
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

print("Downloading to target...")
# Clear winIDEAs download list
loader.clearDownloadList(ic.CLoaderController.DLIST_PRIMARY)
# Add to download list on win10
loader.addToDownloadList(dlConfig, ic.CLoaderController.DLIST_PRIMARY, '../cortexM3/build/exe/stm32/release/lm3s.elf', '')
# Download to target
success = loader.download()
if success == 0:
	print("Download successfull.")
time.sleep(1)






def testWritingToMemory(dataBuffer, memoryAddress):

	numLocations = len(dataBuffer)

	# Reading memory at given address
	dataAndAccessInfo = debugCtrl.readMemory(ic.IConnectDebug.fMonitor, 0, memoryAddress, numLocations, 1)
	data = dataAndAccessInfo[:numLocations]

	print("Current " + str(numLocations) + " bytes of data at address " + str(hex(memoryAddress)))
	for byte in data:
		print("    data: ", hex(byte))

	# Write to target memory
	debugCtrl.writeMemory(ic.IConnectDebug.fMonitor, 0, memoryAddress, len(dataBuffer), 1, dataBuffer)
	time.sleep(1)

	# Reading again to check write outcome
	dataAndAccessInfo = debugCtrl.readMemory(ic.IConnectDebug.fMonitor, 0, memoryAddress, numLocations, 1)
	data = dataAndAccessInfo[:numLocations]

	print("Data after writing: ")
	for i in range(len(data)):
		print("    data: ", hex(data[i]))
		if data[i] == dataBuffer[i]:
			dataTest = 'OK'
		else:
			dataTest = 'NOT OK!'
			break

	print("Writing data : "+ dataTest)
	print("------------------------------")

def testGotoAddress(gotoAddr):

	execCtrl.gotoAddress(0, gotoAddr )
	status = debugCtrl.getCPUStatus()
	currentAddress = status.getExecutionPoint()
	if gotoAddr == currentAddress:
		print("Goto address "+ str(hex(gotoAddr)) +": OK")
	else:
		print("Goto address : NOT OK!")

def step(numOfSteps):
	for i in range(numOfSteps):
		# Step into
		debugCtrl.stepInst()
		# Get cpu status
		status = debugCtrl.getCPUStatus()
		print("Stepped to instruction at address " + str(hex(status.getExecutionPoint())) )
		time.sleep(1)
	
def stepOver(numOfSteps):
	for i in range(numOfSteps):
		# Step over
		debugCtrl.stepOverInst()
		status = debugCtrl.getCPUStatus()
		print("Stepped over instruction at address " + str(hex(status.getExecutionPoint())) )
		time.sleep(1)


outBuffer1 = ic.VectorBYTE([0, 0, 0, 0, 0, 0, 0, 0])
outBuffer2 = ic.VectorBYTE([0xaa, 0xab, 0x54, 0xfe, 0x1, 0x7f, 0x92, 0xe4])
outBuffer3 = ic.VectorBYTE([0x42, 0x4c, 0xa4, 0x62, 0x9, 0x2a, 0xb5, 0x2d])

print("------------------------------")
print("Testing memory read/write")
print("------------------------------")

testWritingToMemory(outBuffer1, 0x0)
testWritingToMemory(outBuffer2, 0x50)
testWritingToMemory(outBuffer3, 0x100)


print("------------------------------")
print("Testing goto address")
print("------------------------------")

testGotoAddress(0x0)
testGotoAddress(0x100)
testGotoAddress(0x50)
testGotoAddress(0x22)

print("------------------------------")
print("Testing stepping")
print("------------------------------")

step(10)
stepOver(10)




# TODO: Testing of breakpoints, reading/writing system registers

#path = "C:/Users/student2/proj/qemu/isystem/samples/cortexM3/src/common/main.c"
#debugCtrl.setBP(30, path) 
#brkCtrl.setBP('main')

#pyIsDebugTest = debugCtrl.evaluate(ic.IConnectDebug.fMonitor, "@r13")	# which registers can be checked?

#print("Value of register r13 =" + str(hex(pyIsDebugTest.getLong())) )
#print("Changing value of r13 to 0xabc")
#debugCtrl.modify(ic.IConnectDebug.fMonitor, "@r13", "0xabc")
#pyIsDebugTest = debugCtrl.evaluate(ic.IConnectDebug.fMonitor, "@r13")
#print("Value of register r13 =" + str(hex(pyIsDebugTest.getLong())) )

#execCtrl.run()

