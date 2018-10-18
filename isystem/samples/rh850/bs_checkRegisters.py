#	This script is used to format a readable output
#	of QEMU log file.
#	It prints out the PC, instruction, status register PSW
#	and 32 general purpose registers.
#	----------
#	Usage: 'python checkRegisters.py logfile.log'.


import sys
import os


def check_registers(file,NUM_OF_INTS_TO_PRINT,file_name):
    #sys.stdout = open('log_qemu_'+file_name+'.log', 'w')
    instNumber = 0
    counter = 0
    out = ['','']
    log_object = open(file, "r")
    NUM_OF_LINES_WITH_GPR_VALUES = 6
    raw_line_ = [0]*4

    for line in log_object:

        if line.startswith("0x"):
            print()
            instNumber += 1
            readLogLine = True
            counter = 0
            raw_line = line.split()

            if len(raw_line) == 4:
                if instNumber > 1:
                    print( raw_line_[0], raw_line_[1], raw_line_[2], raw_line_[3])
                raw_line_[0] = raw_line[0]
                raw_line_[1] = raw_line[1]
                raw_line_[2] = raw_line[2]
                raw_line_[3] = raw_line[3]

        elif line.startswith(" "):
            raw_line = line.split()
            if readLogLine:
                val = 0
                for i in raw_line:
                    if val % 2 == 0:
                        out[0] = i + ":"
                    else:
                        out[1] = i
                        print(f"{out[0]:>5} {out[1]}", end="")
                    val += 1
                print()
            
                
        counter = counter + 1
        if counter > NUM_OF_LINES_WITH_GPR_VALUES:
            readLogLine = False
        if instNumber > NUM_OF_INTS_TO_PRINT:
            break

check_registers("../../../../rh850.log", 10, "out")
