import sys
import os
import time

# Run this script in qemu/isystem/samples/rh850/ with test file without extention and number of instructions as arguments
# Example: python3 ./test.py test_file 10

if len(sys.argv)<3:
    print('Arguments missing!')
    print('Usage: python3 test.py name_of_test_file_without_extensions numbers_of_instructions')
    exit()

os.system('./build.sh '+sys.argv[1])

os.system('../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -singlestep -d nochain,exec,in_asm,cpu -D ../../../rh850.log -kernel '+sys.argv[1]+'.elf &')
time.sleep(3)
os.system('kill $(pgrep qemu-system)')
stevilo = int(sys.argv[2]) + 31

os.system('python3 checkRegisters.py ../../../rh850.log ' + str(stevilo) + ' > log2.log')
os.system('python3 checkRegistersBlueBox.py ' + sys.argv[1] + ' ' + str(stevilo) + ' > log1.log')


f1 = open("log2.log", "r")
f2 = open("log1.log", "r")

index = 0
start = 0;
okay = True

print("----------")
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

