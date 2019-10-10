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
EIPSW_MASK = 0xfffff1df # ignore ID flag, because HW behavior does not match doc - EIPSW.ID is set,
                        # but PSW.ID is not???

# files provifing common functionality to test files
DO_NOT_TEST_FILES = ['gpr_init.s', 'RH850G3M_insts.s', 'br.s']  # br.s does not end 
                            # properly - for manual testing during development only
QEMU_LOG_FILE = "../../../../rh850.log"
# path to workspace must be relative, it seems winIDEA can not open abs path on Linux.
QEMU_WORKSPACE_F1L = "RH850_F1L_IAR/SampleGDB.xjrf"
HW_WORKSPACE_F1L = "RH850_F1L_IAR/SampleiC5k-testStand.xjrf"
QEMU_WORKSPACE_F1KM = '/home/isystem/proj/qemu/isystem/samples/rh850/RH850_F1KM_GHS/SampleiC5k-GDB.xjrf'
HW_WORKSPACE_F1KM = '/home/isystem/proj/qemu/isystem/samples/rh850/RH850_F1KM_GHS/SampleiC5k.xjrf'

RESULT_PASSED = "PASSED"
RESULT_FAILED = "FAILED"

_g_isVerboseLog = True

RH850_PC = 'PC'

RH850_REGS = [
    'R0', 'R1', 'R2', 'R3', 'R4', 'R5', 'R6', 'R7',
    'R8', 'R9', 'R10', 'R11', 'R12', 'R13', 'R14', 'R15',
    'R16', 'R17', 'R18', 'R19', 'R20', 'R21', 'R22', 'R23', 'R24', 'R25', 
    'R26', 'R27', 'R28', 'R29', 'R30', 'R31',
    "psw", "eipc", "eipsw", "fepc", "fepsw", 
    "eiic",  "feic", "ctpc", "ctpsw",  "ctbp",
    "eiwr", "fewr",  "rbase","ebase","intbp", # "mctl",  # "mcfg0",
    "sccfg", "scbp", "htcfg0","mea",  "asid", "mei"  # "pid", 
]

# regs initilized on target to be equal to QEMU ones on reset
# See also test/gpr_init.s.
RH850_INIT_REGS = [
    'R1', 'R2', 'R3', 'R4', 'R5', 'R6', 'R7',
    'R8', 'R9', 'R10', 'R11', 'R12', 'R13', 'R14', 'R15',
    'R16', 'R17', 'R18', 'R19', 'R20', 'R21', 'R22', 'R23',
    'R24', 'R25', 'R26', 'R27', 'R28', 'R29', 'R30', 'R31',
    "asid", "ctpc", "ctbp",  "eipc", "eiwr", "ebase", 
    "fepc", "fewr", "intbp",
    "sccfg", "scbp", "mea",  "mei"
]
PSW_IDX = RH850_REGS.index('psw')
EIPSW_IDX = RH850_REGS.index('eipsw')
FEPSW_IDX = RH850_REGS.index('fepsw')
CTPSW_IDX = RH850_REGS.index('ctpsw')

# floating point regs are currently not supported in QEMU
# "fpsr", "fpepc",  "fpst", "fpcc", "fpcfg", "fpec", 
# Not available in winIDEA
# "bsel", 

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
            '-kernel', f'test/bin/{asmFileStem}.elf', '-monitor', 'tcp:127.0.0.1:55555,server,nowait']

    # run detached
    sp.Popen(args)
    # wait until started
    isStarted = False
    while not isStarted:
        try:
            sp.check_output("netstat -lt | grep 55555", shell=True)
            isStarted = True
            print("QEMU started ...")
        except sp.CalledProcessError:
            print("Waiting for QEMU to start ...")
            time.sleep(1)


def getQemuLogFName(asmFileStem):
    return 'bin/' + asmFileStem + '_qemu.log'


def getBlueBoxLogFName(asmFileStem):
    return 'bin/' + asmFileStem + '_bluebox.log'


def compareLoop(qemuTarget, hwTarget, count=0):   # count == 0 means until stop condition is encountered (br .)
    prevPC = -1

    while (True):
        qemuRegisters, qemuPC = qemuTarget.readRegisters(RH850_REGS, RH850_PC)
        hwRegisters, hwPC = hwTarget.readRegisters(RH850_REGS, RH850_PC)

        if len(qemuRegisters) != len(hwRegisters):
            raise Exception("ERROR: Internal error - register lists size does not match!\n" + 
                            str(qemuRegisters) + '\n' + str(hwRegisters))

        for i in range(len(RH850_REGS)):

            if i == EIPSW_IDX:
                if ((qemuRegisters[i] & EIPSW_MASK) != (hwRegisters[i] & EIPSW_MASK)):
                    raise Exception(f"ERROR: Register values do not match! addr: {hex(qemuPC)}: {RH850_REGS[i]}    qemu: {hex(qemuRegisters[i])}    hw: {hex(hwRegisters[i])}")
                continue
            if i == PSW_IDX  or  i == FEPSW_IDX  or  i == CTPSW_IDX:
                if ((qemuRegisters[i] & PSW_MASK) != (hwRegisters[i] & PSW_MASK)):
                    raise Exception(f"ERROR: Register values do not match! addr: {hex(qemuPC)}: {RH850_REGS[i]}    qemu: {hex(qemuRegisters[i])}    hw: {hex(hwRegisters[i])}")
                continue

            if qemuRegisters[i] != hwRegisters[i]:
                raise Exception(f"ERROR: Register values do not match! addr: {hex(qemuPC)}: {RH850_REGS[i]}    qemu: {hex(qemuRegisters[i])}    hw: {hex(hwRegisters[i])}")

        if qemuPC != hwPC:
            raise Exception(f"ERROR: Register values do not match! addr: {hex(qemuPC)}: PC    qemu: {hex(qemuPC)}    hw: {hex(hwPC)}")

        log('PC = ' + hex(qemuPC))
        if prevPC == qemuPC:  # instruction BR 0 means end of test program
            break

        prevPC = qemuPC

        qemuTarget.step()
        hwTarget.step()

        if qemuTarget.isDoubleStepInst(qemuPC):
            log('double step instruction detected')
            qemuTarget.step()
              
        print('#', end='')
        if count > 0:
            count -= 1
            if count == 0:
                break

    print()


def runTest(asmFileStem, qemuTarget, hwTarget):
    
    _startQemu(asmFileStem)

    qemuTarget.initTarget(asmFileStem)
    hwTarget.initTarget(asmFileStem)

    log("Stepping ...")
    try:
        compareLoop(qemuTarget, hwTarget)
    except Exception as ex:
        return str(ex)
    finally:
        _killQemu()

    return 'OK'



def updateRegs():
    pass


def testPresetSystem(qemuTarget, hwTarget):
    """
    This function steps and compares registers on winIDEA, which are manually 
    set up.
    """

    NUM_STEPS = 20
    while True:
        try:
            compareLoop(qemuTarget, hwTarget, NUM_STEPS)
            print(f"Block of {NUM_STEPS} steps executed.")
        except Exception as ex:
            print(str(ex))

        ans = input('Continue? [y]/n ')
        if ans == ''  or ans.upper() == 'Y':
            updateRegs()
        else:
            break


def initTargets(qemuWorkspace, hwWorkspace):
    log("Starting QEMU winIDEA ...")
    qemuTarget = targetcontrol.TargetController(qemuWorkspace)
    qemuTarget.initWinIDEA()

    log("Starting target winIDEA ...")
    hwTarget = targetcontrol.TargetController(hwWorkspace)
    hwTarget.initWinIDEA()

    return qemuTarget, hwTarget


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

    parser.add_argument("-t", dest="isManualTest", action='store_true', default=False,
                        help="If specified, this script does not open or initialize "
                             "winIDEA or target, but starts stepping and comparing "
                             "register values. Workspace names must be manually "
                             "updated in the script.")

    parser.add_argument("-r", dest="isResetRegisters", action='store_true', default=False,
                        help="If specified, registers on HW target are reset before any "
                             "other action is taken. Useful to make regs equal to qemu ones before run.")

    parser.add_argument("--f1km", dest="isF1KM", action='store_true', default=False,
                        help="If specified, workspace for RH850_F1KM is used.") 

    args = parser.parse_args()

    _g_isVerboseLog = args.isVerboseLog

    return args


def main():

    args = _parseArgs()
    asmFiles = args.files

    qemuWorkspace = QEMU_WORKSPACE_F1KM if args.isF1KM else QEMU_WORKSPACE_F1L
    hwWorkspace = HW_WORKSPACE_F1KM if args.isF1KM else HW_WORKSPACE_F1L

    qemuTarget, hwTarget = initTargets(qemuWorkspace, hwWorkspace)

    if args.isResetRegisters:
        hwTarget.resetRegisters(RH850_INIT_REGS)

    if args.isManualTest:
        testPresetSystem(qemuTarget, hwTarget)

    if args.isResetRegisters or args.isManualTest:
        sys.exit(0)
        
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

        testResult = runTest(asmFileStem, qemuTarget, hwTarget)

        print(asmFileStem, testResult)
        results.append(testResult)

    final_results = zip(tested_files, results)
    print("--------------------- RESULTS -------------------------")
    for x in final_results:
        print(x)


if __name__ == '__main__':
    main()
