import sys
import os
import time

if len(sys.argv)<3:
    print('Arguments missing!')
    print('Usage: python3 test.py name_of_test_file_without_extensions numbers_of_instructions')
    exit()
if os.path.getmtime(sys.argv[1]+'.elf') < os.path.getmtime(sys.argv[1]+'.s'):
    os.system('./build.sh '+sys.argv[1])

os.system('../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -singlestep -d nochain,exec,in_asm,cpu -D rh850.log -kernel '+sys.argv[1]+'.elf &')
time.sleep(1)
os.system('kill $(pgrep qemu-system)')
stevilo = int(sys.argv[2]) + 31

os.system('python3 checkRegistersBlueBox.py ' + sys.argv[1] + ' ' + str(stevilo) + ' > log1.log')
os.system('python3 checkRegisters.py ../../../rh850.log ' + str(stevilo) + ' > log2.log')

f1 = open("log2.log", "r")
f2 = open("log1.log", "r")

index = 0
start = 0;
okay = True

for line1, line2 in zip(f1, f2):
    if start>=35*31:
        if(index == 0):
            print(line1)
        elif(index > 2):
            if line1.split(': ')[1] != line2.split('x')[1]:
                print("ni enako" + line1)
                okay = False
        index = index + 1

        if(index == 35):
            if okay:
                print("OKAY")
            else:
                print("NOT OKAY")
            index = 0
            okay = True

    start = start + 1
f1.close()
f2.close()

