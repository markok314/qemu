
wine ~/bin/iar/bin/iasmrh850.exe test1.s
wine ~/bin/iar/bin/ilinkrh850.exe --no_remove --vfe -osample.out --map \
 sample.map --config_def CSTACK_SIZE=0x1000 --config_def HEAP_SIZE=4096 test1.o
