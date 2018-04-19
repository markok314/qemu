/*
 * QEMU RH850 CPU
 *
 * Copyright (c) 2018 iSYSTEM Labs
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "cpu.h"
#include "qemu-common.h"
#include "exec/exec-all.h"

static void rh850_cpu_set_pc(CPUState *cs, vaddr value)
{
    Rh850CPU *cpu = RH850_CPU(cs);

    cpu->env.pc = value;
}

static bool rh850_cpu_has_work(CPUState *cs)
{
    return cs->interrupt_request & (CPU_INTERRUPT_HARD |
                                    CPU_INTERRUPT_TIMER);
}

/* CPUClass::reset() */
static void rh850_cpu_reset(CPUState *s)
{
    Rh850CPU *cpu = RH850_CPU(s);
    Rh850CPUClass *occ = RH850_CPU_GET_CLASS(cpu);

    occ->parent_reset(s);

    memset(&cpu->env, 0, offsetof(CPURh850State, end_reset_fields));

    cpu->env.pc = 0x100;
    cpu->env.sr = SR_FO | SR_SM;
    cpu->env.lock_addr = -1;
    s->exception_index = -1;

    cpu->env.upr = UPR_UP | UPR_DMP | UPR_IMP | UPR_PICP | UPR_TTP |
                   UPR_PMP;
    cpu->env.dmmucfgr = (DMMUCFGR_NTW & (0 << 2)) | (DMMUCFGR_NTS & (6 << 2));
    cpu->env.immucfgr = (IMMUCFGR_NTW & (0 << 2)) | (IMMUCFGR_NTS & (6 << 2));

#ifndef CONFIG_USER_ONLY
    cpu->env.picmr = 0x00000000;
    cpu->env.picsr = 0x00000000;

    cpu->env.ttmr = 0x00000000;
#endif
}

static void rh850_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState *cs = CPU(dev);
    Rh850CPUClass *occ = RH850_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    qemu_init_vcpu(cs);
    cpu_reset(cs);

    occ->parent_realize(dev, errp);
}

static void rh850_cpu_initfn(Object *obj)
{
    CPUState *cs = CPU(obj);
    Rh850CPU *cpu = RH850_CPU(obj);

    cs->env_ptr = &cpu->env;

#ifndef CONFIG_USER_ONLY
    cpu_rh850_mmu_init(cpu);
#endif
}

/* CPU models */

static ObjectClass *rh850_cpu_class_by_name(const char *cpu_model)
{
    ObjectClass *oc;
    char *typename;

    typename = g_strdup_printf(RH850_CPU_TYPE_NAME("%s"), cpu_model);
    oc = object_class_by_name(typename);
    g_free(typename);
    if (oc != NULL && (!object_class_dynamic_cast(oc, TYPE_RH850_CPU) ||
                       object_class_is_abstract(oc))) {
        return NULL;
    }
    return oc;
}

static void or1200_initfn(Object *obj)
{
    Rh850CPU *cpu = RH850_CPU(obj);

    cpu->env.cpucfgr = CPUCFGR_NSGF | CPUCFGR_OB32S | CPUCFGR_OF32S |
                       CPUCFGR_EVBARP;
}

static void rh850_any_initfn(Object *obj)
{
    Rh850CPU *cpu = RH850_CPU(obj);

    cpu->env.cpucfgr = CPUCFGR_NSGF | CPUCFGR_OB32S | CPUCFGR_EVBARP;
}

static void rh850_cpu_class_init(ObjectClass *oc, void *data)
{
    Rh850CPUClass *occ = RH850_CPU_CLASS(oc);
    CPUClass *cc = CPU_CLASS(occ);
    DeviceClass *dc = DEVICE_CLASS(oc);

    occ->parent_realize = dc->realize;
    dc->realize = rh850_cpu_realizefn;

    occ->parent_reset = cc->reset;
    cc->reset = rh850_cpu_reset;

    cc->class_by_name = rh850_cpu_class_by_name;
    cc->has_work = rh850_cpu_has_work;
    cc->do_interrupt = rh850_cpu_do_interrupt;
    cc->cpu_exec_interrupt = rh850_cpu_exec_interrupt;
    cc->dump_state = rh850_cpu_dump_state;
    cc->set_pc = rh850_cpu_set_pc;
    cc->gdb_read_register = rh850_cpu_gdb_read_register;
    cc->gdb_write_register = rh850_cpu_gdb_write_register;
#ifdef CONFIG_USER_ONLY
    cc->handle_mmu_fault = rh850_cpu_handle_mmu_fault;
#else
    cc->get_phys_page_debug = rh850_cpu_get_phys_page_debug;
    dc->vmsd = &vmstate_rh850_cpu;
#endif
    cc->gdb_num_core_regs = 32 + 3;
    cc->tcg_initialize = rh850_translate_init;
}

/* Sort alphabetically by type name, except for "any". */
static gint rh850_cpu_list_compare(gconstpointer a, gconstpointer b)
{
    ObjectClass *class_a = (ObjectClass *)a;
    ObjectClass *class_b = (ObjectClass *)b;
    const char *name_a, *name_b;

    name_a = object_class_get_name(class_a);
    name_b = object_class_get_name(class_b);
    if (strcmp(name_a, "any-" TYPE_RH850_CPU) == 0) {
        return 1;
    } else if (strcmp(name_b, "any-" TYPE_RH850_CPU) == 0) {
        return -1;
    } else {
        return strcmp(name_a, name_b);
    }
}

static void rh850_cpu_list_entry(gpointer data, gpointer user_data)
{
    ObjectClass *oc = data;
    CPUListState *s = user_data;
    const char *typename;
    char *name;

    typename = object_class_get_name(oc);
    name = g_strndup(typename,
                     strlen(typename) - strlen("-" TYPE_RH850_CPU));
    (*s->cpu_fprintf)(s->file, "  %s\n",
                      name);
    g_free(name);
}

void cpu_rh850_list(FILE *f, fprintf_function cpu_fprintf)
{
    CPUListState s = {
        .file = f,
        .cpu_fprintf = cpu_fprintf,
    };
    GSList *list;

    list = object_class_get_list(TYPE_RH850_CPU, false);
    list = g_slist_sort(list, rh850_cpu_list_compare);
    (*cpu_fprintf)(f, "Available CPUs:\n");
    g_slist_foreach(list, rh850_cpu_list_entry, &s);
    g_slist_free(list);
}

#define DEFINE_RH850_CPU_TYPE(cpu_model, initfn) \
    {                                               \
        .parent = TYPE_RH850_CPU,                \
        .instance_init = initfn,                    \
        .name = RH850_CPU_TYPE_NAME(cpu_model),  \
    }

static const TypeInfo rh850_cpus_type_infos[] = {
    { /* base class should be registered first */
        .name = TYPE_RH850_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(Rh850CPU),
        .instance_init = rh850_cpu_initfn,
        .abstract = true,
        .class_size = sizeof(Rh850CPUClass),
        .class_init = rh850_cpu_class_init,
    },
    DEFINE_RH850_CPU_TYPE("or1200", or1200_initfn),
    DEFINE_RH850_CPU_TYPE("any", rh850_any_initfn),
};

DEFINE_TYPES(rh850_cpus_type_infos)
