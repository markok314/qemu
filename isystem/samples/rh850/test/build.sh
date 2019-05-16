USAGE="Usage: $0 fileName\nwhere 'fileName' is name of assmbler file without extension\nExamples:\n    $0 test1\n"

if [ -z "$1" ]; then
    echo -e "Missing argument 1! Specify assmbler file name without extension."
    echo -e $USAGE
    exit 1
fi

mkdir -p bin

wine ~/.wine/drive_c/SysGCC/v850-elf/bin/v850-elf-gcc.exe -Wa,-a=bin/$1.lst -g -c -mv850e2v3 -o bin/$1.o $1.s

wine ~/.wine/drive_c/SysGCC/v850-elf/bin/v850-elf-gcc.exe -Wl,--output=bin/$1.elf,--Map=bin/$1.map,--script=rh850.lnk,-n -O0 -nostartfiles bin/$1.o
