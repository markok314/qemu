#pragma once

#include "hw/sysbus.h"
#include "target/rh850/cpu.h"
#include "hw/rh850/rh850_soc.h"

#define TYPE_RH850_SOC "rh850_soc"
#define RH850_SOC(obj) OBJECT_CHECK(RH850_SOC_State, (obj), TYPE_RH850_SOC)

/* RH850_SOC container object.
 * + Unnamed GPIO input lines: external IRQ lines for the NVIC
 * + Named GPIO output SYSRESETREQ: signalled for guest AIRCR.SYSRESETREQ
 * + Property "cpu-type": CPU type to instantiate
 * + Property "num-irq": number of external IRQ lines
 * + Property "memory": MemoryRegion defining the physical address space
 *   that CPU accesses see. (The NVIC, bitbanding and other CPU-internal
 *   devices will be automatically layered on top of this view.)
 * + Property "idau": IDAU interface (forwarded to CPU object)
 * + Property "init-svtor": secure VTOR reset value (forwarded to CPU object)
 */
typedef struct RH850_SOC_State {
    /*< private >*/
    SysBusDevice parent_obj;
    /*< public >*/
    RH850CPU *cpu;

    /* MemoryRegion we pass to the CPU, with our devices layered on
     * top of the ones the board provides in board_memory.
     */
    MemoryRegion container;

    /* Properties */
    char *cpu_type;
    /* MemoryRegion the board provides to us (with its devices, RAM, etc) */
    MemoryRegion *board_memory;
} RH850_SOC_State;

