/*
 * RH850_soc emulation.
 *
 * Copyright (c) 2018 iSYSTEM Labs.
 * Written by Marko Klopcic
 *
 * This code is licensed under the GPL.
 */

#include "qemu/osdep.h"
#include "hw/rh850/rh850_soc.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "cpu.h"
#include "hw/sysbus.h"
#include "hw/loader.h"
#include "hw/qdev-properties.h"
#include "elf.h"
#include "sysemu/qtest.h"
#include "qemu/error-report.h"
#include "exec/address-spaces.h"

static void rh850_soc_instance_init(Object *obj)
{
    // RH850_SOC_State *s = RH850_SOC(obj);

    /* Can't init the cpu here, we don't yet know which model to use */

    // Is this needed? MK
    // memory_region_init(&s->container, obj, "rh850-container", UINT32_MAX);

//     object_initialize(&s->nvic, sizeof(s->nvic), TYPE_NVIC);
//    qdev_set_parent_bus(DEVICE(&s->nvic), sysbus_get_default());
//    object_property_add_alias(obj, "num-irq",
//                              OBJECT(&s->nvic), "num-irq", &error_abort);
}


/**
main() -> machne.c:machine_run_board_init() -> rh850mini.c:rh850mini_init() -> add_cpu()
  -> qdev.c:qdev_init_nofail() -> object_property_set_bool() ->
                               -> object_property_set_qobject()
                               -> object_property_set()
                               -> property_set_bool()
  -> qdev.c:device_set_realized() -> rh850_soc_realize()
*/
static void rh850_soc_realize(DeviceState *dev, Error **errp)
{
    RH850_SOC_State *s = RH850_SOC(dev);
    Error *err = NULL;

//    if (!s->board_memory) {
//        error_setg(errp, "memory property was not set");
//        return;
//    }

//    memory_region_add_subregion_overlap(&s->container, 0, s->board_memory, -1);

    s->cpu = RH850_CPU(object_new(s->cpu_type));

    // Commented line with 'OBJECT(&s->container)' adds container memory to CPU,
    // but container is empty - we'd have to add FLASH and RAM to container.
    // Line with OBJECT(get_system_memory()) adds system memory to CPU, which contains
    // FLASH and RAM. Use monitor command 'info mtree' to see memory tree.
    //object_property_set_link(OBJECT(s->cpu), OBJECT(&s->container), "memory",
    object_property_set_link(OBJECT(s->cpu), OBJECT(get_system_memory()), "memory",
                             &error_abort);
    object_property_set_bool(OBJECT(s->cpu), true, "realized", &err);
    if (err != NULL) {
        error_propagate(errp, err);
        return;
    }

    /* Note that we must realize the NVIC after the CPU */
//    object_property_set_bool(OBJECT(&s->nvic), true, "realized", &err);
//    if (err != NULL) {
//        error_propagate(errp, err);
//        return;
//    }

    /* Alias the NVIC's input and output GPIOs as our own so the board
     * code can wire them up. (We do this in realize because the
     * NVIC doesn't create the input GPIO array until realize.)
     */
/*    qdev_pass_gpios(DEVICE(&s->nvic), dev, NULL);
    qdev_pass_gpios(DEVICE(&s->nvic), dev, "SYSRESETREQ");

    // Wire the NVIC up to the CPU
    sbd = SYS_BUS_DEVICE(&s->nvic);
    sysbus_connect_irq(sbd, 0,
                       qdev_get_gpio_in(DEVICE(s->cpu), ARM_CPU_IRQ));
    s->cpu->env.nvic = &s->nvic;
*/
/*    memory_region_add_subregion(&s->container, 0xe000e000,
                                sysbus_mmio_get_region(sbd, 0));

    for (i = 0; i < ARRAY_SIZE(s->bitband); i++) {
        Object *obj = OBJECT(&s->bitband[i]);
        SysBusDevice *sbd = SYS_BUS_DEVICE(&s->bitband[i]);

        object_property_set_int(obj, bitband_input_addr[i], "base", &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }
        object_property_set_link(obj, OBJECT(s->board_memory),
                                 "source-memory", &error_abort);
        object_property_set_bool(obj, true, "realized", &err);
        if (err != NULL) {
            error_propagate(errp, err);
            return;
        }

        memory_region_add_subregion(&s->container, bitband_output_addr[i],
                                    sysbus_mmio_get_region(sbd, 0));
    }
*/
}

/*
 * Properties are structs with prop name as string and offset to the member
 * in struct. Struct members are then set as properties.
 */
static Property rh850_soc_properties[] = {
    DEFINE_PROP_STRING("cpu-type", RH850_SOC_State, cpu_type),
    DEFINE_PROP_LINK("memory", RH850_SOC_State, board_memory, TYPE_MEMORY_REGION,
                     MemoryRegion *),
//    DEFINE_PROP_LINK("idau", ARMv7MState, idau, TYPE_IDAU_INTERFACE, Object *),
//    DEFINE_PROP_UINT32("init-svtor", ARMv7MState, init_svtor, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void rh850_soc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rh850_soc_realize;
    dc->props = rh850_soc_properties;
}

static const TypeInfo rh850_soc_info = {
    .name = TYPE_RH850_SOC,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RH850_SOC_State),
    .instance_init = rh850_soc_instance_init,
    .class_init = rh850_soc_class_init,
};


static void rh850_soc_register_types(void)
{
    type_register_static(&rh850_soc_info);
}


type_init(rh850_soc_register_types)




