# Script is used for to check which dlls are necessary for executing qemu

import sys
import os
import glob
import subprocess

for file in glob.glob("*.dll"):
    os.rename(file, file+'_tst')
    print(file, end='')
    try:
        cmd = ['wine qemu-system-arm.exe -M help > /dev/null 2>&1']
        subprocess.check_call(cmd, shell=True, executable='/bin/bash')
        print()
    except:
        print(" NEEDED")

    os.rename(file + '_tst', file)

