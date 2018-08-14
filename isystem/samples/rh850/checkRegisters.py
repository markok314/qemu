#	This script is used to format a readable output
#	of QEMU log file.
#	It prints out the PC, instruction, status register PSW
#	and 32 general purpose registers.
#	----------
#	Usage: 'python checkRegisters.py logfile.log'.
#	----------
#	Automated building of a test file and running of
#	QEMU will be implemented soon.
#	Number of instructions to print is currently limited
#	for easier testing of short test programs, the limitation
#	will be removed when needed.

import sys
import os

instNumber = 0
counter = 0
out = ['','']
log_object = open(sys.argv[1], "r")
NUM_OF_LINES_WITH_GPR_VALUES = 12
NUM_OF_INTS_TO_PRINT = int(sys.argv[2])

#if (len(sys.argv)<3):
#	print('Arguments missing!')
#	print('Usage: python checkRegisters.py \"path_to_log_file\" \"name_of_test_file_without_extensions\"')
#	exit()
#if os.path.getmtime(sys.argv[2]+'.elf') < os.path.getmtime(sys.argv[2]+'.s'):
#	os.system('./build.sh '+sys.argv[2])
#	os.system('../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -singlestep -d nochain,exec,in_asm,cpu -D rh850.log -kernel ./robert-test.elf')

for line in log_object:

    if line.startswith("0x"):
        instNumber += 1
        readLogLine = True
        counter = 0
        raw_line = line.split()

        if len(raw_line) == 4:
            print(raw_line[0], raw_line[1], raw_line[2], raw_line[3])

    elif line.startswith(" "):
        raw_line = line.split()
        if readLogLine:
            val = 0
            for i in raw_line:
                if val % 2 == 0:
                    out[0] = i + ":"
                else:
                    out[1] = i
                    print(f"{out[0]:>5} {out[1]}")
                val += 1

    counter = counter + 1
    if counter > NUM_OF_LINES_WITH_GPR_VALUES:
        readLogLine = False
    if instNumber > NUM_OF_INTS_TO_PRINT:
        break
