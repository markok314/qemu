USAGE="Usage: $0 fileName\nwhere 'fileName' is name of assmbler file without extension\nExamples:\n    $0 test1\n"

if [ -z "$1" ]; then
    echo -e "Missing argument 1! Specify assmbler file name without extension."
    echo -e $USAGE
    exit 1
fi

wine ~/bin/iar/bin/iasmrh850.exe $1.s
wine ~/bin/iar/bin/ilinkrh850.exe --no_remove --vfe -o$1.elf --map \
 $1.map --config_def CSTACK_SIZE=0x1000 --config_def HEAP_SIZE=4096 $1.o
