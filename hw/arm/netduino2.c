/*
 * Netduino 2 Machine Model
 *
 * Copyright (c) 2014 Alistair Francis <alistair@alistair23.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "qemu/error-report.h"
#include "hw/arm/stm32f205_soc.h"
#include "hw/arm/arm.h"

static void netduino2_init(MachineState *machine)
{
    DeviceState *dev;

    dev = qdev_create(NULL, TYPE_STM32F205_SOC);
    qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m3"));
    object_property_set_bool(OBJECT(dev), true, "realized", &error_fatal);

    // see also stm32f3015_soc.c for start address and RAM
    uint32_t flash_size = FLASH_SIZE;
    get_memory_ranges("0", NULL, &flash_size, NULL, NULL);

    armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename,
                       flash_size);

    /* The folowing two calls are a workaround for problem with rom in aliased
     * addresses on STM32. It is also possible to call qemu_system_reset()
     * twice in main(), but then we modify code for all targets. This solution
     * registers rom reset twice, but the change is local to STM32.
     * For investigation set breakpoints in:
     * - armv7m.c [line: 261]	- cpu_reset(CPU(cpu));
     * - armv7m.c [line: 281]   - armv7m_load_kernel(ARM_CPU(first_cpu), kernel_filename, mem_size);
     * - cpu.c [line: 235]      - SP and PC are loaded: initial_msp = ldl_phys(s->as, vecbase);
     * - loader.c [line: 1070]  - rom_insert(rom);
     * - loader.c [line: 1095]  - rom_reset() { ... }
     * - loader.c [line: 1119]	- rom_check_and_register_reset()
     *
     * Description:
     * Kernel is loaded into Rom objects, which are stored in loader.c. Only
     * in
     * void rom_reset() {
     *   ...
     *    cpu_physical_memory_write_rom(rom->as, rom->addr, rom->data,
     *                                  rom->datasize);
     *   ...
     * }
     * are Roms copied to memory. However, since CPU reset is registered BEFORE
     * rom_reset(), SP and PC are read from memory BEFORE roms are copied to
     * memory. CPU reset also checks Roms for data, so this is normally not a
     * problem, but in STM32 PC and SP are read from aliased memory, and these
     * memory addresses are not mapped to Roms. Since ROMs are private in
     * loader.c the only option to copy Roms to memory is through reset. So we
     * first have to register Roms for reset, and then trigger system reset, which
     * copies Roms to mem, so that reset vectors are in memory, when
     * qemu_system_reset() is called again from main()
     */
    rom_check_and_register_reset();
    qemu_system_reset(SHUTDOWN_CAUSE_NONE);
}

static void netduino2_machine_init(MachineClass *mc)
{
    mc->desc = "Netduino 2 Machine";
    mc->init = netduino2_init;
    mc->ignore_memory_transaction_failures = true;
}

DEFINE_MACHINE("netduino2", netduino2_machine_init)
