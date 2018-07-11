# this script works from MinGW on windows only, because X is not visible from VM
/x/GHS/V850/comp_201655/as850.exe -cpu=v850e2 -G testghs.s
/x/GHS/V850/comp_201655/elxr -T ghs.ld testghs.o -o testghs.elf
