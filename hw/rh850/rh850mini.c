/*
 * Rh850 SoC
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
//#include "qapi/error.h"
//#include "hw/sysbus.h"
//#include "hw/ssi/ssi.h"
//#include "hw/arm/arm.h"
//#include "hw/devices.h"
//#include "qemu/timer.h"
//#include "hw/i2c/i2c.h"
//#include "net/net.h"
#include "hw/boards.h"
//#include "qemu/log.h"
//#include "exec/address-spaces.h"
//#include "sysemu/sysemu.h"
//#include "hw/char/pl011.h"
//#include "hw/misc/unimp.h"
#include "cpu.h"

const int FLASH_SIZE = 1 << 20; // 1M
const int SRAM_SIZE = 1 << 16;  // 64k
const int FLASH_START = 0;
const int SRAM_START = 0;

static void rh850mini_init(MachineState *ms)
{
    int sram_size;
    int flash_size;
    DeviceState *dev;
    int i;
    int j;

    MemoryRegion *sram = g_new(MemoryRegion, 1);
    MemoryRegion *flash = g_new(MemoryRegion, 1);
    MemoryRegion *system_memory = get_system_memory();

    flash_size = FLASH_SIZE;
    sram_size = SRAM_SIZE;

    /* Flash programming is done via the SCU, so pretend it is ROM.  */
    memory_region_init_ram(flash, NULL, "rh850mini.flash", flash_size,
                           &error_fatal);
    memory_region_set_readonly(flash, true);
    memory_region_add_subregion(system_memory, FLASH_START, flash);

    memory_region_init_ram(sram, NULL, "rh850mini.sram", sram_size,
                           &error_fatal);
    memory_region_add_subregion(system_memory, SRAM_START, sram);

    nvic = armv7m_init(system_memory, flash_size, NUM_IRQ_LINES,
                       ms->kernel_filename, ms->cpu_type);

//    qdev_connect_gpio_out_named(nvic, "SYSRESETREQ", 0,
//                                qemu_allocate_irq(&do_sys_reset, NULL, 0));

    stellaris_sys_init(0x400fe000, qdev_get_gpio_in(nvic, 28),
                       board, nd_table[0].macaddr.a);

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
    mc->default_cpu_type = RH850_CPU_TYPE_NAME("rh850");
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
