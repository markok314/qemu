import sys
import os

st=0
stevec=0
out = ['','']
log_object = open(sys.argv[1], "r")

#if (len(sys.argv)<3):
#	print('Arguments missing!')
#	print('Usage: python checkRegisters.py \"path_to_log_file\" \"name_of_test_file_without_extensions\"')
#	exit()
#if os.path.getmtime(sys.argv[2]+'.elf') < os.path.getmtime(sys.argv[2]+'.s'):
#	os.system('./build.sh '+sys.argv[2])
#	os.system('../../../rh850-softmmu/qemu-system-rh850 -M rh850mini -s -singlestep -d nochain,exec,in_asm,cpu -D rh850.log -kernel ./robert-test.elf')

for line in log_object:

    if line.startswith("0x"):
        st = st + 1
        poberi=True
        stevec=0
        raw_line = line.split()
        print("\n")
        if raw_line.__len__()==4:
            print(raw_line[0], raw_line[1], raw_line[2], raw_line[3])
        else:
            print(raw_line[0], raw_line[1])
        print("-------------------------------------")

    elif line.startswith(" "):
        raw_line = line.split()
        if poberi:
            val = 0
            for i in raw_line:
                if val%2==0:
                    out[0] = i+":"
                else:
                    out[1] = i
                    print('%5s' % out[0],out[1])
                val=val+1

    stevec = stevec + 1
    if stevec>12:
        poberi=False
    if st>23:
        break
