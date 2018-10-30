# This script is used for testing implementation of microcontroller RH850 in qemu.
# Script launches all files with .s extension in folder /test in qemu and on real microcontroller
# RH850 using Isystem BlueBox and then compares the results.

# Module checkRegisters is used for formatting a readable output of QEMU log file.
# Module checkRegistersBlueBox is used to read registers pc and psw from real microcontroller RH850.

#Usage
# Run this script in qemu/isystem/samples/rh850/
# Example of usage python3 test.py

# Optional
# Check flags.
# Default usage doesnt check flags, run script with --flag to check them also.

import sys
import os
import time
import argparse
import subprocess
import sys
import glob
import checkRegisters as cr
import checkRegistersBlueBox as crbb

parser = argparse.ArgumentParser()
parser.add_argument("-f", "--flags", help="also check flags")
args = parser.parse_args()

#go into folder gcc and test all files with .s extension
os.chdir("test")

testing_files = []
results = []
result_for_file = "PASSED"

for file in glob.glob("*.s"):
    if(file != 'gpr_init.s' and file != 'RH850G3M_insts.s'):
        result_for_file = "PASSED"
        testing_files.append(file)
        sys.stdout = sys.__stdout__
        print(file)
        file = file[:-2]
        #for removing the .s extension

        cmd = ['./build.sh '+file+' >build_'+file+'.log']
        subprocess.check_call(cmd, shell=True, executable='/bin/bash')

        subprocess.check_call("../../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -singlestep -d"
                              " nochain,exec,in_asm,cpu -D ../../../../rh850.log -kernel "+file+".elf &", shell=True)

        time.sleep(3)
        subprocess.check_call('kill $(pgrep qemu-system)', shell=True, executable='/bin/bash')

        NUM_OF_REGISTERS = 31

        NUM_OF_ALL_INST =  crbb.check_registers_blue_box(file)

        cr.check_registers('../../../../rh850.log', NUM_OF_ALL_INST,file)

        sys.stdout = sys.__stdout__


        log_qemu = open('log_qemu_'+file+'.log', 'r')
        log_blubox = open('log_bluebox_'+file+'.log', 'r')
    
        index = 0
        start = 0;
        isOkay = True
    
        NUM_OF_PRINTED_REGISTERS = 35;
        NUM_OF_REGS_TO_NOT_CHECK = NUM_OF_REGISTERS * NUM_OF_PRINTED_REGISTERS
    
        # EXAMPLE OF LOG FILE
        # QEMU              BLUEBOX     index
    
        # INSTRUCTION       --------    0
        # PC                PC          1
        # PSW               PSW         2
        # R0                R0          3
        # R1                R1
        # R2                R2
        # ...               ...
    
        print("----------")
        for line1, line2 in zip(log_qemu, log_blubox):
            if start>=NUM_OF_REGS_TO_NOT_CHECK:
                if(index == 0):
                    #NAME OF INSTRUCTION
                    print("-----------------")
                    print(line1[:-1])
                elif(index == 2):
                    #PSW
                    if args.flags:
                        if line1[-2:] != line2[-2:]:
                            # CHECKING JUST LAST 4 BITS OF PSW REG
                            print("ERROR" + line1[:-1] +" "+ line2[:-1])
                            isOkay = False
                        elif ((int(line1[-3]) % 2) != (int(line2[-3]) % 2)):
			    #CHECKING SAT FLAG
                            print("ERROR" + line1[:-1] +" "+ line2[:-1])
                            isOkay = False
                else:
                    #PC AND OTHER GPR
                    if line1.split(': ')[1] != line2.split('x')[1]:
                        print("ERROR" + line1[:-1] +" " +  line2[:-1])
                        isOkay = False
    
                index = index + 1
    
                if(index == NUM_OF_PRINTED_REGISTERS):
                    if isOkay:
                        print("OK")
                        print("-----------------")
                    else:
                        print("FAILED")
                        result_for_file = "FAILED"
                        print("-----------------")
                    index = 0
                    isOkay = True

            start = start + 1

        log_qemu.close()
        log_blubox.close()
        results.append(result_for_file)

final_results = zip(testing_files,results)
print("---------------------FINAL RESULTS-------------------------")
for x in final_results:
    print(x)
