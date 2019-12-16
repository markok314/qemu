This file lists most important features and fixes added by iSYSTEM Labs

1. eb7a810008896c6b6514dff77caa372c7b55ab38
   Fixed `stm` instruction for ARMv6M - base register is incremented also when 
   it is in reg. list.

   target/arm/translate.c

2. commit 2f911f8614a21750a58dd8f5952d6fee6ee014aa
   address of flash alias is now configurable for stm32F205

   hw/arm/stm32f205_soc.c
   include/hw/arm/stm32f205_soc.h
   
3. 4c57da48e57376768de70b97f8d51c26a1107968
   added hack for loading of GHS produced elf files
   include/hw/elf_ops.h

4. Commit 8f746edc324dad9567d1d75beb0c508724bc5bef
   Up to 5 memory areas for FLASH and RAM can be specified for RH850

     hw/arm/netduino2.c
     hw/arm/stm32f205_soc.c
     hw/rh850/rh850mini.c
     qemu-options.hx
     vl.c

To see list of all changed files:

git log --name-only --author markok314 --since 2019-01-01 > gitlog
grep -E "(^Author)|(^Date)|(^commit)|(^Merge)|(^   )" gitlog -v | sort | uniq

Changed files not in isystem or target/rh850 dirs:

    Conflicted when mergining upstream 4.2.0:
        configure
        disas.c
        hw/arm/netduino2.c
        hw/arm/stm32f205_soc.c
        hw/core/loader.c
        target/arm/translate.c
        include/disas/bfd.h
        tests/docker/docker.py
        util/Makefile.objs
 

include/hw/loader.h  - comment only
.gitignore           - added *.swp
include/hw/elf_ops.h - hack for loading GHS elf files

qemu-options.hx      - options for FLASH and RAM start and size
vl.c                 - options for FLASH and RAM start and size
arch_init.c          - added target RH850
include/elf.h        - added RH850 version of ELF files
include/hw/arm/stm32f205_soc.h - added #define FLASH_ALIAS_BASE_ADDRESS 0
include/sysemu/arch_init.h - added RH850 target:  QEMU_ARCH_RH850 = (1 << 20),

Files added by iSYSTEM Labs:

include/qemu/soc-options.h 
util/soc-options.c
