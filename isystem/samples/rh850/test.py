# This script is used for testing implementation of microcontroller RH850 in
# qemu.  Script launches all files with .s extension in folder rh850/test in
# qemu and on real microcontroller RH850 using iSYSTEM BlueBox and then
# compares the results.
#
# Module checkRegisters is used for formatting a readable output of QEMU log file.
#
# Module checkRegistersBlueBox is used to read registers from real
# microcontroller RH850.

# Usage:
# Run this script in qemu/isystem/samples/rh850/
# Example: python3 test.py

# Optional
# Check flags.
# Default usage doesnt check flags, run script with --flag to check them also.

import os
import sys
import time
import argparse
import subprocess
import glob
import checkRegistersBlueBox as crbb
import itertools

haveBlueBox = True

NUM_OF_REGISTERS = 31
NUM_OF_PRINTED_REGISTERS = 35;
NUM_OF_REGS_TO_NOT_CHECK = NUM_OF_REGISTERS * NUM_OF_PRINTED_REGISTERS
NUM_OF_LINES_WITH_GPR_VALUES = 12
PSW_MASK = 0xfffff1ff   # ignore flags for debug mode, because they are set by winIDEA

# files provifing common functionality to test files
NOT_TEST_FILES = ['gpr_init.s', 'RH850G3M_insts.s']
QEMU_LOG_FILE = "../../../../rh850.log"

RESULT_PASSED = "PASSED"
RESULT_FAILED = "FAILED"

_g_isVerboseLog = True

def log(msg: str):
    if _g_isVerboseLog:
        print(msg)


def buildElf(fileName):
    cmd = ['./build.sh ' + fileName + ' > bin/' + fileName + '_build.log']
    subprocess.check_call(cmd, shell=True, executable='/bin/bash')


def quitQemuIfRunning():
    try:
        subprocess.check_output("netstat -lt | grep 55555", shell=True)
        subprocess.check_call("echo q | nc -N 127.0.0.1 55555", shell=True)
        print("QEMU stopped")
        time.sleep(3)
    except subprocess.CalledProcessError:
        print("OK: QEMU not running")


def runQemu(fileName, logFile):

    quitQemuIfRunning()

    subprocess.check_call("../../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -singlestep -nographic "
                          " -d nochain,exec,in_asm,cpu -D " + logFile + 
                          " -kernel bin/" + fileName + ".elf -monitor tcp:127.0.0.1:55555,server,nowait "
						  " | tee file1 > bin/qemu.stdout &", shell=True)
    time.sleep(1)

    # wait until BR 0000, which indicated end of program, is executed by QEMU
    # However, limit looping to avoid enless execution in case of missing end instr.
    MAX_LOOPS = 70
    for loopCount in range(MAX_LOOPS):
        time.sleep(0.5)
        print(loopCount, '/', MAX_LOOPS, ': ', fileName)
        try:
            subprocess.check_output("tail -1 file1 | grep \"BR          0000\"", shell=True)
            break
        except subprocess.CalledProcessError:
            subprocess.check_call("echo c | nc -N 127.0.0.1 55555", shell=True)

    subprocess.check_call("echo q | nc -N 127.0.0.1 55555", shell=True)
 
    # TODO replace with GDB control and exiting nicely
    #subprocess.check_call('kill $(pgrep qemu-system)', shell=True, executable='/bin/bash')
    #subprocess.check_call('kill $(pgrep tee)', shell=True, executable='/bin/bash')


def getQemuLogFName(asmFileStem):
    return 'bin/' + asmFileStem + '_qemu.log'

def getBlueBoxLogFName(asmFileStem):
    return 'bin/' + asmFileStem + '_bluebox.log'


def extract_registers(qemuLogFileName, num_of_ints_to_print, asmFileStem):
    """
    This function extracts instruction and registers from QEMU log file. It 
    prints out PC, instruction, status register PSW, and 32 general purpose 
    registers.
    """
    with open(getQemuLogFName(asmFileStem), 'w') as qemuRegsFile:

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
                    elif len(raw_line) == 3:
                        print(raw_line[0], raw_line[1], raw_line[2], file=qemuRegsFile)

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


# not used, seems to be redundant, if files are different sizes compare will fail anyway
def _check_file_sizes(asmFileStem):
    with open(getQemuLogFName(asmFileStem), 'r') as log_qemu: 
        with open(getBlueBoxLogFName(asmFileStem), 'r') as log_blubox: 

            num_lines_qemu = sum(1 for line in log_qemu)
            log(f'num_lines_qemu = {num_lines_qemu}')
            num_lines_bluebox = sum(1 for line in log_blubox)
            log('num_lines_bluebox = {num_lines_bluebox}')
            if num_lines_bluebox-(NUM_OF_PRINTED_REGISTERS*2) > num_lines_qemu:
                return RESULT_FAILED
                print("files are different size")
        
    return RESULT_PASSED


def compare_qemu_and_blue_box_regs(asmFileStem):

    # TODO - review algorithm

    # EXAMPLE OF LOG FILE
    # QEMU              BLUEBOX     index

    # INSTRUCTION       --------    0
    # PC                PC          1
    # PSW               PSW         2
    # R0                R0          3
    # R1                R1
    # R2                R2
    # ...               ...
    
    index = 0
    start = 0;
    isOkay = True
    result_for_file = RESULT_PASSED

    with open(getQemuLogFName(asmFileStem), 'r') as log_qemu:

        with open(getBlueBoxLogFName(asmFileStem), 'r') as log_blubox:

            log("----------")
            #for lineQemu, lineBB in zip(log_qemu, log_blubox):
            for lineQemu, lineBB in itertools.zip_longest(log_qemu, log_blubox):
                if start >= NUM_OF_REGS_TO_NOT_CHECK:
                    if lineQemu is None:
                         print("Missing lineQemu (qemu) !")
                         print("FAILED")
                         result_for_file = RESULT_FAILED
                         return result_for_file
                    #print("index: " + str(index))
                    if(index == 0):
                        # NAME OF INSTRUCTION
                        log("-----------------")
                        log('@@@> {lineQemu[:-1]}')
                        print(lineQemu)
                        if lineQemu[-8:-1] == 'BR 0000':
                            print("BR 0000")
                            break
                    elif(index == 2):
                        # PSW
                        # print("CHECKING FLAGS ALSO:\nQemu:", lineQemu, 'BB:', lineBB)
                        pswQemu = int(lineQemu[lineQemu.index(':') + 2:], 16)
                        pswBB = int(lineBB[lineBB.index('=') + 2:], 16)
                        if ((pswQemu & PSW_MASK) != (pswBB & PSW_MASK)):
                            print("ERROR PSW:\n QEMU "  +  lineQemu + "\n BB: " +  lineBB)
                            isOkay = False
                    else:
                        #PC AND OTHER GPR
                        #print("lineQemu: " + lineQemu + ", lineBB: " + lineBB)
                        log('lines:\n - {lineQemu}\n - {lineBB}')
                        if lineQemu.split(': ')[1] != lineBB.split('x')[1]:
                            print("ERROR: " + lineQemu[:-1]  + " " +  lineBB[:-1])
                            isOkay = False

                    index = index + 1

                    if(index == NUM_OF_PRINTED_REGISTERS):
                        if isOkay:
                            log("OK")
                        else:
                            print("FAILED")
                            result_for_file = RESULT_FAILED
                        index = 0
                        isOkay = True

                start = start + 1
                #print("start: " + str(start))

    return result_for_file


def _parseArgs():
    global _g_isVerboseLog

    usage = """
Usage: %prog [asmFilesToTest]

If no asm file is specified, all .s files in dir 'test' are tested. 
Use shell globbing to specify groups of files to be tested, for 
example:

$ %prog test/mov*.s
"""
    parser = argparse.ArgumentParser(description=usage)

    parser.add_argument("-v", dest="isVerboseLog", action='store_true', default=False,
                        help="If specified, debug logging is sent to stdout.")

    parser.add_argument('files', metavar='files', type=str, nargs='*',
                        help='list of asm files to test')

    args = parser.parse_args()

    _g_isVerboseLog = args.isVerboseLog

    return args


def main():

    args = _parseArgs()
    asmFiles = args.files
    if not asmFiles:   # no files specified in cmd line
        asmFiles = glob.glob("*.s")
        asmFiles.sort()

    print(f"Testing {len(asmFiles)} files:")
    print(asmFiles)

    tested_files = []
    results = []

    if haveBlueBox:
        blueBox = crbb.BlueBox()
        blueBox.openConnection()

    for idx, asmFile in enumerate(asmFiles):

        asmFile = os.path.basename(asmFile) # strip away path from files 
                           # specified in cmdLine. Only dir 'test' is allowed.

        if asmFile in NOT_TEST_FILES:
            print(f'- ({idx + 1}/{len(asmFiles)}) Skipped {asmFile}, this is utility file included in other tests.')
            continue

        tested_files.append(asmFile)
        print(f"--- ({idx + 1}/{len(asmFiles)})  {asmFile}")
        asmFileStem = asmFile[:-2]  # remove the .s extension

        buildElf(asmFileStem)
        log("Exec in QEMU...")
        runQemu(asmFileStem, QEMU_LOG_FILE)

        if haveBlueBox:
            log("Exec on target ...")
            num_of_all_inst =  blueBox.check_registers_blue_box(asmFileStem, 
                                                            getBlueBoxLogFName(asmFileStem))
        else:
            num_of_all_inst = 100
        log("Comparing registers ...")
        extract_registers(QEMU_LOG_FILE, num_of_all_inst, asmFileStem)

        if haveBlueBox:
            _check_file_sizes(asmFileStem)
            fileRes = compare_qemu_and_blue_box_regs(asmFileStem)
        else:
            fileRes = ""
        results.append(fileRes)

    final_results = zip(tested_files, results)
    print("---------------------FINAL RESULTS-------------------------")
    for x in final_results:
        print(x)


if __name__ == '__main__':
    os.chdir("test")
    main()
    os.chdir('..')
