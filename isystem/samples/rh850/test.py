# This script is used for testing implementation of microcontroller RH850 in
# qemu. The script launches all files with .s extension located in folder rh850/test in
# qemu and on real RH850 microcontroller using iSYSTEM BlueBox and then
# compares core contexts (values of core registers).
#
# Module checkRegistersBlueBox is used to read registers from real
# microcontroller RH850.
#
# Run this script in directory qemu/isystem/samples/rh850/. Use option -h to 
# see available options.

import os
import sys
import time
import argparse
import subprocess as sp
import glob
import targetcontrol
import itertools

PSW_MASK = 0xfffff1ff   # ignore flags for debug mode, because they are set by winIDEA

# files provifing common functionality to test files
DO_NOT_TEST_FILES = ['gpr_init.s', 'RH850G3M_insts.s']
QEMU_LOG_FILE = "../../../../rh850.log"
# path to workspace must be relative, it seems winIDEA can not open apbs path on Linux.
QEMU_WORKSPACE = "RH850_F1L_IAR/SampleGDB.xjrf"
HW_WORKSPACE = "RH850_F1L_IAR/SampleiC5k-testStand.xjrf"
RESULT_PASSED = "PASSED"
RESULT_FAILED = "FAILED"

_g_isVerboseLog = True

RH850_PC = 'PC'
RH850_PSW = 'PSW'

RH850_REGS = [
    'R0', 'R1', 'R2', 'R3', 'R4', 'R5', 'R6', 'R7',
    'R8', 'R9', 'R10', 'R11', 'R12', 'R13', 'R14', 'R15',
    'R16', 'R17', 'R18', 'R19', 'R20', 'R21', 'R22', 'R23',
    'R24', 'R25', 'R26', 'R27', 'R28', 'R29', 'R30', 'R31',
#    "eipc", "eipsw", 
#    "eiic",  "feic", "ctpc", "ctpsw",  "ctbp",
#    "eiwr", "fewr",  "mcfg0", "rbase","ebase","intbp", "mctl",
#    "pid", "sccfg", "scbp", "htcfg0","mea",  "asid", "mei"
]
# floating point regs are currently not supported in QEMU
# "fpsr", "fepc", "fepsw", "fpepc",  "fpst", "fpcc", "fpcfg", "fpec", 
# "bsel"

def log(msg: str):
    if _g_isVerboseLog:
        print(msg)


def buildElf(fileName):
    log(f"Building '{fileName}' ...")
    cmd = ['./build.sh ' + fileName + ' > bin/' + fileName + '_build.log']
    sp.check_call(cmd, shell=True, executable='/bin/bash', cwd='test')


def _killQemu():
    # os.sheck_call('pkill qemu', shell=True)

    try:
        sp.check_output("netstat -lt | grep 55555", shell=True)
        sp.check_call("echo q | nc -N 127.0.0.1 55555", shell=True)
        print("QEMU stopped")
        time.sleep(3)
    except sp.CalledProcessError:
        print("OK: No stale instance of QEMU is running.")


def _startQemu(asmFileStem):
    """
    Normally winIDEA should strat QEMU, but it has problems killing it.
    Until this is fixed on Linux, start QEMU in advance here and have
    winIDEA workspace configured to only attach to existing QEMU.
    """
    _killQemu()
    log('Starting QEMU ...')
    args = ['../../../rh850-softmmu/qemu-system-rh850',
            '-M', 'rh850mini', '-s', '-S', '-singlestep',
            '-kernel', f'test/bin/{asmFileStem}.elf']

    # sp.check_call(args)
    sp.Popen(args)
    log('QEMU started')


# deprecated
def runQemu(fileName, logFile):

    quitQemuIfRunning()

    # Runs QEMU, which stops on first isntruction and waits for winIDEA to connect as GDB client
    # winIDEA can not run QEMU on Linux at the moment.
    sp.check_call("../../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -S -nographic"
                          " -kernel bin/" + fileName + ".elf",
						  shell=True)
    # Runs QEMU, which logs instructions and regs to file, where this script reads them from 
    #sp.check_call("../../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -singlestep -nographic "
    #                      " -d nochain,exec,in_asm,cpu -D " + logFile + 
    #                      " -kernel bin/" + fileName + ".elf -monitor tcp:127.0.0.1:55555,server,nowait "
    #                      " | tee file1 > bin/qemu.stdout &", shell=True)

    # wait until BR 0000, which indicates the end of program


def getQemuLogFName(asmFileStem):
    return 'bin/' + asmFileStem + '_qemu.log'


def getBlueBoxLogFName(asmFileStem):
    return 'bin/' + asmFileStem + '_bluebox.log'


def runTest(asmFileStem):
    
    _startQemu(asmFileStem)

    log("Starting QEMU winIDEA ...")
    qemuTarget = targetcontrol.TargetController(QEMU_WORKSPACE)
    qemuTarget.initTarget(asmFileStem)

    log("Starting target winIDEA ...")
    hwTarget = targetcontrol.TargetController(HW_WORKSPACE)
    hwTarget.initTarget(asmFileStem)

    prevPC = -1

    # init registers before comparison to get the same starting point on both ends

    log("Stepping ...")
    while (True):
        qemuRegisters, qemuPC, qemuPSW = qemuTarget.readRegisters(RH850_REGS, RH850_PC, RH850_PSW)
        hwRegisters, hwPC, hwPSW = hwTarget.readRegisters(RH850_REGS, RH850_PC, RH850_PSW)

        if len(qemuRegisters) != len(hwRegisters):
            return ("ERROR: Internal error - register lists size does not match!\n" + 
                            str(qemuRegisters) + '\n' + str(hwRegisters))

        for i in range(len(RH850_REGS)):

            if qemuRegisters[i] != hwRegisters[i]:
                return f"ERROR: Register values do not match! reg: {RH850_REGS[i]}    qemu: {qemuRegisters[i]}    hw: {hwRegisters[i]}"

        if qemuPC != hwPC:
            return f"ERROR: Register values do not match! reg: PC    qemu: {qemuPC}    hw: {hwPC}"

        if ((qemuPSW & PSW_MASK) != (hwPSW & PSW_MASK)):
            return f"ERROR: Register values do not match! reg: PSW    qemu: {qemuPSW & PSW_MASK}    hw: {hwPC & PSW_MASK}"

        print('===', prevPC, qemuPC)
        if prevPC == qemuPC:  # instruction BR 0 means end of test program
            break

        prevPC = qemuPC

        qemuTarget.step()
        hwTarget.step()
          
    _killQemu()

    return 'OK'


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
    if not asmFiles:   # no files specified in cmd line, test all of them
        asmFiles = glob.glob("*.s")
        asmFiles.sort()

    print(f"Testing {len(asmFiles)} files:")
    print(asmFiles)

    tested_files = []
    results = []

    for idx, asmFile in enumerate(asmFiles):

        asmFile = os.path.basename(asmFile) 

        if asmFile in DO_NOT_TEST_FILES:
            print(f'- ({idx + 1}/{len(asmFiles)}) Skipped {asmFile}, this is utility file included in other tests.')
            continue

        tested_files.append(asmFile)
        print(f"--- ({idx + 1}/{len(asmFiles)})  {asmFile}")
        asmFileStem = asmFile[:-2]  # remove the .s extension

        buildElf(asmFileStem)

        testResult = runTest(asmFileStem)

        print(asmFileStem, testResult)
        results.append(testResult)

    final_results = zip(tested_files, results)
    print("--------------------- RESULTS -------------------------")
    for x in final_results:
        print(x)


if __name__ == '__main__':
    main()
