#include "qemu/soc-options.h"
#include "qemu/osdep.h"
#include "qemu/cutils.h"

#include "qemu/option.h"
#include "qemu/option_int.h"
#include "qemu/config-file.h"
#include "qemu-options.h"


static void get_option_value(QemuOpts *subopts, uint32_t *opt_value, const char *opt_name) {

    if (subopts  && opt_value) {
        QemuOpt *start_opt = qemu_opt_find(subopts, opt_name);
        if (start_opt) {
            *opt_value = start_opt->value.uint;
        }
    }
}


static void get_memory_range(QemuOpts *subopts, const char *mem_postfix,
                             uint32_t *start_addr, uint32_t *size) {

    char start_name[10] = {"start"};
    pstrcat(start_name, sizeof(start_name), mem_postfix);
    get_option_value(subopts, start_addr, start_name);

    char size_name[10] = {"size"};
    pstrcat(size_name, sizeof(size_name), mem_postfix);
    get_option_value(subopts, size, size_name);
}


// Returns memory ranges as defined in opts. On input all vars should
// contain default values, because they are changed only if user specified
// cmd. line option.
void get_memory_ranges(const char *mem_postfix,
                       uint32_t *flash_start, uint32_t *flash_size,
                       uint32_t *ram_start, uint32_t *ram_size) {

    QemuOptsList *optsList = qemu_find_opts("flash");
    // printf("---->> %s = %s\n", optsList->name, optsList->implied_opt_name);
    QemuOpts *flash_opts = qemu_opts_find(optsList, NULL);
    get_memory_range(flash_opts, mem_postfix, flash_start, flash_size);

    optsList = qemu_find_opts("ram");
    QemuOpts *ram_opts = qemu_opts_find(optsList, NULL);
    get_memory_range(ram_opts, mem_postfix, ram_start, ram_size);
}
