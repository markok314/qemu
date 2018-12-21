
import os
import sys

NUM_OF_LINES_WITH_GPR_VALUES = 12

def extract_registers(qemuLogFileName, num_of_ints_to_print, asmFileStem):
    """
    This function extracts instruction and registers from QEMU log file. It 
    prints out PC, instruction, status register PSW, and 32 general purpose 
    registers.
    """
    with open('log_qemu_' + asmFileStem + '.log', 'w') as qemuRegsFile:

	with open(qemuLogFileName, "r") as qemuLogFile:

	    instNumber = 0
	    counter = 0
	    out = ['','']

	    for line in qemuLogFile:

		if line.startswith("0x"):
		    instNumber += 1
		    readLogLine = True
		    counter = 0
		    raw_line = line.split()

		    if len(raw_line) == 4:
			print(raw_line[0], raw_line[1], raw_line[2], raw_line[3], file=qemuRegsFile)

		elif line.startswith(" "):
		    raw_line = line.split()
		    if readLogLine:
			val = 0
			for i in raw_line:
			    if val % 2 == 0:
				out[0] = i + ":"
			    else:
				out[1] = i
				print(f"{out[0]:>5} {out[1]}", file=qemuRegsFile)
			    val += 1

		counter += 1
		if counter > NUM_OF_LINES_WITH_GPR_VALUES:
		    readLogLine = False
		if instNumber > num_of_ints_to_print:
		    break
