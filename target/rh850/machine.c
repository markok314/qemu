/*
 * Rh850 Machine
 *
 * Copyright (c) 2011-2012 Jia Liu <proljc@gmail.com>
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
#include "qemu-common.h"
#include "cpu.h"
#include "hw/hw.h"
#include "hw/boards.h"
#include "migration/cpu.h"

static int env_post_load(void *opaque, int version_id)
{
    CPURh850State *env = opaque;

    /* Restore MMU handlers */
    if (env->sr & SR_DME) {
        env->tlb->cpu_rh850_map_address_data =
            &cpu_rh850_get_phys_data;
    } else {
        env->tlb->cpu_rh850_map_address_data =
            &cpu_rh850_get_phys_nommu;
    }

    if (env->sr & SR_IME) {
        env->tlb->cpu_rh850_map_address_code =
            &cpu_rh850_get_phys_code;
    } else {
        env->tlb->cpu_rh850_map_address_code =
            &cpu_rh850_get_phys_nommu;
    }


    return 0;
}

static const VMStateDescription vmstate_tlb_entry = {
    .name = "tlb_entry",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINTTL(mr, Rh850TLBEntry),
        VMSTATE_UINTTL(tr, Rh850TLBEntry),
        VMSTATE_END_OF_LIST()
    }
};

static const VMStateDescription vmstate_cpu_tlb = {
    .name = "cpu_tlb",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField[]) {
        VMSTATE_STRUCT_2DARRAY(itlb, CPURh850TLBContext,
                             ITLB_WAYS, ITLB_SIZE, 0,
                             vmstate_tlb_entry, Rh850TLBEntry),
        VMSTATE_STRUCT_2DARRAY(dtlb, CPURh850TLBContext,
                             DTLB_WAYS, DTLB_SIZE, 0,
                             vmstate_tlb_entry, Rh850TLBEntry),
        VMSTATE_END_OF_LIST()
    }
};

#define VMSTATE_CPU_TLB(_f, _s)                             \
    VMSTATE_STRUCT_POINTER(_f, _s, vmstate_cpu_tlb, CPURh850TLBContext)


static int get_sr(QEMUFile *f, void *opaque, size_t size, VMStateField *field)
{
    CPURh850State *env = opaque;
    cpu_set_sr(env, qemu_get_be32(f));
    return 0;
}

static int put_sr(QEMUFile *f, void *opaque, size_t size,
                  VMStateField *field, QJSON *vmdesc)
{
    CPURh850State *env = opaque;
    qemu_put_be32(f, cpu_get_sr(env));
    return 0;
}

static const VMStateInfo vmstate_sr = {
    .name = "sr",
    .get = get_sr,
    .put = put_sr,
};

static const VMStateDescription vmstate_env = {
    .name = "env",
    .version_id = 6,
    .minimum_version_id = 6,
    .post_load = env_post_load,
    .fields = (VMStateField[]) {
        VMSTATE_UINTTL_2DARRAY(shadow_gpr, CPURh850State, 16, 32),
        VMSTATE_UINTTL(pc, CPURh850State),
        VMSTATE_UINTTL(ppc, CPURh850State),
        VMSTATE_UINTTL(jmp_pc, CPURh850State),
        VMSTATE_UINTTL(lock_addr, CPURh850State),
        VMSTATE_UINTTL(lock_value, CPURh850State),
        VMSTATE_UINTTL(epcr, CPURh850State),
        VMSTATE_UINTTL(eear, CPURh850State),

        /* Save the architecture value of the SR, not the internally
           expanded version.  Since this architecture value does not
           exist in memory to be stored, this requires a but of hoop
           jumping.  We want OFFSET=0 so that we effectively pass ENV
           to the helper functions, and we need to fill in the name by
           hand since there's no field of that name.  */
        {
            .name = "sr",
            .version_id = 0,
            .size = sizeof(uint32_t),
            .info = &vmstate_sr,
            .flags = VMS_SINGLE,
            .offset = 0
        },

        VMSTATE_UINT32(vr, CPURh850State),
        VMSTATE_UINT32(upr, CPURh850State),
        VMSTATE_UINT32(cpucfgr, CPURh850State),
        VMSTATE_UINT32(dmmucfgr, CPURh850State),
        VMSTATE_UINT32(immucfgr, CPURh850State),
        VMSTATE_UINT32(evbar, CPURh850State),
        VMSTATE_UINT32(pmr, CPURh850State),
        VMSTATE_UINT32(esr, CPURh850State),
        VMSTATE_UINT32(fpcsr, CPURh850State),
        VMSTATE_UINT64(mac, CPURh850State),

        VMSTATE_CPU_TLB(tlb, CPURh850State),

        VMSTATE_TIMER_PTR(timer, CPURh850State),
        VMSTATE_UINT32(ttmr, CPURh850State),

        VMSTATE_UINT32(picmr, CPURh850State),
        VMSTATE_UINT32(picsr, CPURh850State),

        VMSTATE_END_OF_LIST()
    }
};

const VMStateDescription vmstate_rh850_cpu = {
    .name = "cpu",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_CPU(),
        VMSTATE_STRUCT(env, Rh850CPU, 1, vmstate_env, CPURh850State),
        VMSTATE_END_OF_LIST()
    }
};
