/*
 * Rh850/F1L SoC
 *
 * Copyright (c) 2018 iSYSTEM Labs d.o.o.
 *
 * This file implements simple board with RH850 microcontroller and
 * no peripherals.
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "qemu/error-report.h"
#include "qapi/error.h"
#include "hw/loader.h"
#include "elf.h"
//#include "hw/sysbus.h"
//#include "hw/ssi/ssi.h"
#include "hw/rh850/rh850_soc.h"
//#include "hw/devices.h"
//#include "qemu/timer.h"
//#include "hw/i2c/i2c.h"
//#include "net/net.h"
#include "hw/boards.h"
#include "exec/memory.h"
//#include "qemu/log.h"
#include "exec/address-spaces.h"
#include "sysemu/qtest.h"
//#include "sysemu/sysemu.h"
//#include "hw/char/pl011.h"
//#include "hw/misc/unimp.h"
#include "cpu.h"

// see RH850/F1L, https://www.renesas.com/en-us/products/microcontrollers-microprocessors/rh850/rh850f1x/rh850f1l.html
const uint32_t FLASH_SIZE = 2* (1 << 20); //2M
const uint32_t SRAM_SIZE = 192 * (1 << 10); // 192 kB
const uint32_t FLASH_START = 0;
const uint32_t SRAM_START = 0xfedd8000;  // start of RAM for F1L devices, is not the same for other RH850 devices


static void rh850_reset(void *opaque)
{
	RH850CPU *cpu = opaque;

    cpu_reset(CPU(cpu));
}


static void add_memory(void)
{
    MemoryRegion *sram = g_new(MemoryRegion, 1);
    MemoryRegion *flash = g_new(MemoryRegion, 1);
    MemoryRegion *system_memory = get_system_memory();

    /* Flash programming is done via the SCU, so pretend it is ROM.  */
    memory_region_init_ram(flash, NULL, "rh850.flash", FLASH_SIZE, &error_fatal);
    memory_region_set_readonly(flash, true);
    memory_region_add_subregion(system_memory, FLASH_START, flash);

    memory_region_init_ram(sram, NULL, "rh850.sram", SRAM_SIZE, &error_fatal);
    memory_region_add_subregion(system_memory, SRAM_START, sram);
}


// main() -> machne.c:machine_run_board_init() -> rh850mini.c:rh850mini_init() -> add_cpu()
static void add_cpu(const char *cpu_type)
{
    DeviceState *rh850cpu;

    rh850cpu = qdev_create(NULL, TYPE_RH850_SOC);

    // qdev_prop_set_uint32(rh850cpu, "num-irq", 1);
    qdev_prop_set_string(rh850cpu, "cpu-type", cpu_type);
    object_property_set_link(OBJECT(rh850cpu), OBJECT(get_system_memory()),
                                     "memory", &error_abort);
    /* This will exit with an error if the user passed us a bad cpu_type */
    qdev_init_nofail(rh850cpu);

}


/**
 * Loads elf file specified in command line - kernel or barebone app.
 * Copied from armv7m.c.
 */
static void load_rh_kernel(RH850CPU *cpu, const char *kernel_filename, int mem_size)
{
    if (!kernel_filename && !qtest_enabled()) {
        error_report("Guest image must be specified (using -kernel)");
        exit(1);
    }

    if (kernel_filename) {

    	CPUState *cs = CPU(cpu);
    	AddressSpace *as = cpu_get_address_space(cs, 0);
        const int IS_BIG_ENDIAN = 0;

        uint64_t entry;
        uint64_t lowaddr;

        int image_size = load_elf_as(kernel_filename, NULL, NULL, &entry, &lowaddr,
        		                     NULL, IS_BIG_ENDIAN, EM_RH850, 1, 0, as);

        if (image_size == ELF_LOAD_WRONG_ARCH) { // V850 is subset of RH850, so it is also allowed
            error_report("Loading kernel as V850 code: '%s'", kernel_filename);
        	image_size = load_elf_as(kernel_filename, NULL, NULL, &entry, &lowaddr,
           		                     NULL, IS_BIG_ENDIAN, EM_V850, 1, 0, as);
        }

        if (image_size < 0) {
            image_size = load_image_targphys_as(kernel_filename, 0,
                                                mem_size, as);
            lowaddr = 0;
        }
        if (image_size < 0) {
            error_report("Could not load kernel '%s'", kernel_filename);
            exit(1);
        }
    }

    /* CPU objects (unlike devices) are not automatically reset on system
     * reset, so we must always register a handler to do so.
     */
    qemu_register_reset(rh850_reset, cpu);
}


// main() -> machne.c:machine_run_board_init() -> rh850mini.c:rh850mini_init()
static void rh850mini_init(MachineState *ms)
{
    add_memory();
    add_cpu(ms->cpu_type);
    load_rh_kernel(RH850_CPU(first_cpu), ms->kernel_filename, FLASH_SIZE);

//    nvic = armv7m_init(system_memory, flash_size, NUM_IRQ_LINES,
//                       ms->kernel_filename, ms->cpu_type);

//    qdev_connect_gpio_out_named(nvic, "SYSRESETREQ", 0,
//                                qemu_allocate_irq(&do_sys_reset, NULL, 0));

//    stellaris_sys_init(0x400fe000, qdev_get_gpio_in(nvic, 28),
//                       board, nd_table[0].macaddr.a);

//    if (board->dc4 & (1 << 28)) {
//        DeviceState *enet;
//
//        qemu_check_nic_model(&nd_table[0], "stellaris");
//
//        enet = qdev_create(NULL, "stellaris_enet");
//        qdev_set_nic_properties(enet, &nd_table[0]);
//        qdev_init_nofail(enet);
//        sysbus_mmio_map(SYS_BUS_DEVICE(enet), 0, 0x40048000);
//        sysbus_connect_irq(SYS_BUS_DEVICE(enet), 0, qdev_get_gpio_in(nvic, 42));
//    }

    /* Add dummy regions for the devices we don't implement yet,
     * so guest accesses don't cause unlogged crashes.
     */
//    create_unimplemented_device("wdtimer", 0x40000000, 0x1000);
//    create_unimplemented_device("i2c-0", 0x40002000, 0x1000);
}


static void rh850mini_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "iSYSTEM RH850 Mini";
    mc->init = rh850mini_init;
    mc->ignore_memory_transaction_failures = true;
    mc->default_cpu_type = RH850_CPU_TYPE_NAME("any");
}

static const TypeInfo rh850mini_type = {
    .name = MACHINE_TYPE_NAME("rh850mini"),
    .parent = TYPE_MACHINE,
    .class_init = rh850mini_class_init,
};

static void rh850mini_machine_init(void)
{
    type_register_static(&rh850mini_type);
}

type_init(rh850mini_machine_init)
