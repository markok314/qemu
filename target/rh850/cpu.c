/*
 * QEMU RH850 CPU
 *
 * Copyright (c) 2016-2017 Sagar Karandikar, sagark@eecs.berkeley.edu
 * Copyright (c) 2017-2018 SiFive, Inc.
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
#include "qemu/log.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "qapi/error.h"
#include "migration/vmstate.h"

/* RH850 CPU definitions */

static const char rh850_exts[26] = "IMAFDQECLBJTPVNSUHKORWXYZG";

/* Program registers:
 * r0 - zero
 * r1 - assembler reserved register
 * r2 - real-time OS register / address and data variable register
 * r3 - stack pointer
 * r4 - global pointer
 * r5 - text pointer
 * r6-r29 - address and data variable registers
 * r30 - element pointer
 * r31 - link pointer
 */

const char * const rh850_prog_reg_names[] = {
  "zero", "ar  ", "r2  ", "sp  ", "gp  ", "tp  ", "r6  ", "r7  ",
  "r8  ", "r9  ", "r10  ", "r11  ", "r12  ", "r13  ", "r14  ", "r15  ",
  "r16  ", "r17  ", "r18  ", "r19  ", "r20  ", "r21  ", "r22  ", "r23  ",
  "r24  ", "r25  ", "r26 ", "r27 ", "r28  ", "r29  ", "ep  ", "lp  "
};

const char * const rh850_fpr_regnames[] = {
  "ft0 ", "ft1 ", "ft2 ", "ft3 ", "ft4 ", "ft5 ", "ft6 ",  "ft7 ",
  "fs0 ", "fs1 ", "fa0 ", "fa1 ", "fa2 ", "fa3 ", "fa4 ",  "fa5 ",
  "fa6 ", "fa7 ", "fs2 ", "fs3 ", "fs4 ", "fs5 ", "fs6 ",  "fs7 ",
  "fs8 ", "fs9 ", "fs10", "fs11", "ft8 ", "ft9 ", "ft10",  "ft11"
};

const char * const rh850_excp_names[] = {
    "misaligned_fetch",
    "fault_fetch",
    "illegal_instruction",
    "breakpoint",
    "misaligned_load",
    "fault_load",
    "misaligned_store",
    "fault_store",
    "user_ecall",
    "supervisor_ecall",
    "hypervisor_ecall",
    "machine_ecall",
    "exec_page_fault",
    "load_page_fault",
    "reserved",
    "store_page_fault"
};

const char * const rh850_intr_names[] = {
    "u_software",
    "s_software",
    "h_software",
    "m_software",
    "u_timer",
    "s_timer",
    "h_timer",
    "m_timer",
    "u_external",
    "s_external",
    "h_external",
    "m_external",
    "coprocessor",
    "host"
};

typedef struct RH850CPUInfo {
    const int bit_widths;
    const char *name;
    void (*initfn)(Object *obj);
} RH850CPUInfo;
/*
static void set_misa(CPURH850State *env, target_ulong misa)
{
    env->misa = misa;
}

static void set_versions(CPURH850State *env, int user_ver, int priv_ver)
{
    env->user_ver = user_ver;
    env->priv_ver = priv_ver;
}
*/

/*
static void set_feature(CPURH850State *env, int feature)
{
    env->features |= (1ULL << feature);
} */

static void set_resetvec(CPURH850State *env, int resetvec)
{
    env->resetvec = resetvec;
}

static void rh850_any_cpu_init(Object *obj)
{
    CPURH850State *env = &RH850_CPU(obj)->env;
    // set_misa(env, RVXLEN | RVI | RVM | RVA | RVF | RVD | RVC | RVU);
    // set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, DEFAULT_RSTVEC);
}
/*
#if defined(TARGET_RH850)

static void rv32gcsu_priv1_09_1_cpu_init(Object *obj)
{
    CPURH850State *env = &RH850_CPU(obj)->env;
    set_misa(env, RV32 | RVI | RVM | RVA | RVF | RVD | RVC | RVS | RVU);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_09_1);
    set_resetvec(env, DEFAULT_RSTVEC);
    set_feature(env, RH850_FEATURE_MMU);
}

static void rv32gcsu_priv1_10_0_cpu_init(Object *obj)
{
    CPURH850State *env = &RH850_CPU(obj)->env;
    set_misa(env, RV32 | RVI | RVM | RVA | RVF | RVD | RVC | RVS | RVU);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, DEFAULT_RSTVEC);
    set_feature(env, RH850_FEATURE_MMU);
}

static void rv32imacu_nommu_cpu_init(Object *obj)
{
    CPURH850State *env = &RH850_CPU(obj)->env;
    set_misa(env, RV32 | RVI | RVM | RVA | RVC | RVU);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, DEFAULT_RSTVEC);
}

#elif defined(TARGET_RH85064)

static void rv64gcsu_priv1_09_1_cpu_init(Object *obj)
{
    CPURH850State *env = &RH850_CPU(obj)->env;
    set_misa(env, RV64 | RVI | RVM | RVA | RVF | RVD | RVC | RVS | RVU);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_09_1);
    set_resetvec(env, DEFAULT_RSTVEC);
    set_feature(env, RH850_FEATURE_MMU);
}

static void rv64gcsu_priv1_10_0_cpu_init(Object *obj)
{
    CPURH850State *env = &RH850_CPU(obj)->env;
    set_misa(env, RV64 | RVI | RVM | RVA | RVF | RVD | RVC | RVS | RVU);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, DEFAULT_RSTVEC);
    set_feature(env, RH850_FEATURE_MMU);
}

static void rv64imacu_nommu_cpu_init(Object *obj)
{
    CPURH850State *env = &RH850_CPU(obj)->env;
    set_misa(env, RV64 | RVI | RVM | RVA | RVC | RVU);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, DEFAULT_RSTVEC);
}

#endif
*/


static ObjectClass *rh850_cpu_class_by_name(const char *cpu_model)
{
    ObjectClass *oc;
    char *typename;
    char **cpuname;

    cpuname = g_strsplit(cpu_model, ",", 1);
    typename = g_strdup_printf(RH850_CPU_TYPE_NAME("%s"), cpuname[0]);
    oc = object_class_by_name(typename);
    g_strfreev(cpuname);
    g_free(typename);
    if (!oc || !object_class_dynamic_cast(oc, TYPE_RH850_CPU) ||
        object_class_is_abstract(oc)) {
        return NULL;
    }
    return oc;
}

static void rh850_cpu_dump_state(CPUState *cs, FILE *f,
    fprintf_function cpu_fprintf, int flags)
{
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;
    int i;

    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "pc      ", env->pc);
#ifndef CONFIG_USER_ONLY
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mhartid ", env->mhartid);
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mstatus ", env->mstatus);
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mip     ",
        (target_ulong)atomic_read(&env->mip));
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mie     ", env->mie);
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mideleg ", env->mideleg);
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "medeleg ", env->medeleg);
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mtvec   ", env->mtvec);
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mepc    ", env->mepc);
    cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mcause  ", env->mcause);
#endif

    for (i = 0; i < 32; i++) {
        cpu_fprintf(f, " %s " TARGET_FMT_lx,
            rh850_int_regnames[i], env->gpr[i]);
        if ((i & 3) == 3) {
            cpu_fprintf(f, "\n");
        }
    }
    for (i = 0; i < 32; i++) {
        cpu_fprintf(f, " %s %016" PRIx64,
            rh850_fpr_regnames[i], env->fpr[i]);
        if ((i & 3) == 3) {
            cpu_fprintf(f, "\n");
        }
    }
}

static void rh850_cpu_set_pc(CPUState *cs, vaddr value)
{
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;
    env->pc = value;
}

static void rh850_cpu_synchronize_from_tb(CPUState *cs, TranslationBlock *tb)
{
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;
    env->pc = tb->pc;
}

static bool rh850_cpu_has_work(CPUState *cs)
{
#ifndef CONFIG_USER_ONLY
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;
    /*
     * Definition of the WFI instruction requires it to ignore the privilege
     * mode and delegation registers, but respect individual enables
     */
    return (atomic_read(&env->mip) & env->mie) != 0;
#else
    return true;
#endif
}

void restore_state_to_opc(CPURH850State *env, TranslationBlock *tb,
                          target_ulong *data)
{
    env->pc = data[0];
}

static void rh850_cpu_reset(CPUState *cs)
{
    RH850CPU *cpu = RH850_CPU(cs);
    RH850CPUClass *mcc = RH850_CPU_GET_CLASS(cpu);
    CPURH850State *env = &cpu->env;

    mcc->parent_reset(cs);
#ifndef CONFIG_USER_ONLY
    env->priv = PRV_M;
    env->mstatus &= ~(MSTATUS_MIE | MSTATUS_MPRV);
    env->mcause = 0;
    env->pc = env->resetvec;
#endif
    cs->exception_index = EXCP_NONE;
    set_default_nan_mode(1, &env->fp_status);
}

static void rh850_cpu_disas_set_info(CPUState *s, disassemble_info *info)
{
#if defined(TARGET_RH850)
    info->print_insn = print_insn_rh850;
#elif defined(TARGET_RH85064)
    info->print_insn = print_insn_rh85064;
#endif
}

static void rh850_cpu_realize(DeviceState *dev, Error **errp)
{
    CPUState *cs = CPU(dev);
    RH850CPUClass *mcc = RH850_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    qemu_init_vcpu(cs);
    cpu_reset(cs);

    mcc->parent_realize(dev, errp);
}

static void rh850_cpu_init(Object *obj)
{
    CPUState *cs = CPU(obj);
    RH850CPU *cpu = RH850_CPU(obj);

    cs->env_ptr = &cpu->env;
}

static const VMStateDescription vmstate_rh850_cpu = {
    .name = "cpu",
    .unmigratable = 1,
};

static void rh850_cpu_class_init(ObjectClass *c, void *data)
{
    RH850CPUClass *mcc = RH850_CPU_CLASS(c);
    CPUClass *cc = CPU_CLASS(c);
    DeviceClass *dc = DEVICE_CLASS(c);

    mcc->parent_realize = dc->realize;
    dc->realize = rh850_cpu_realize;

    mcc->parent_reset = cc->reset;
    cc->reset = rh850_cpu_reset;

    cc->class_by_name = rh850_cpu_class_by_name;
    cc->has_work = rh850_cpu_has_work;
    cc->do_interrupt = rh850_cpu_do_interrupt;
    cc->cpu_exec_interrupt = rh850_cpu_exec_interrupt;
    cc->dump_state = rh850_cpu_dump_state;
    cc->set_pc = rh850_cpu_set_pc;
    cc->synchronize_from_tb = rh850_cpu_synchronize_from_tb;
    cc->gdb_read_register = rh850_cpu_gdb_read_register;
    cc->gdb_write_register = rh850_cpu_gdb_write_register;
    cc->gdb_num_core_regs = 32;
    cc->gdb_stop_before_watchpoint = true;
    cc->disas_set_info = rh850_cpu_disas_set_info;
#ifdef CONFIG_USER_ONLY
    cc->handle_mmu_fault = rh850_cpu_handle_mmu_fault;
#else
    cc->do_unaligned_access = rh850_cpu_do_unaligned_access;
    cc->get_phys_page_debug = rh850_cpu_get_phys_page_debug;
#endif
#ifdef CONFIG_TCG
    cc->tcg_initialize = rh850_translate_init;
#endif
    /* For now, mark unmigratable: */
    cc->vmsd = &vmstate_rh850_cpu;
}

char *rh850_isa_string(RH850CPU *cpu)
{
    int i;
    const size_t maxlen = sizeof("rv128") + sizeof(rh850_exts) + 1;
    char *isa_str = g_new(char, maxlen);
    char *p = isa_str + snprintf(isa_str, maxlen, "rv%d", TARGET_LONG_BITS);
    for (i = 0; i < sizeof(rh850_exts); i++) {
        if (cpu->env.misa & RV(rh850_exts[i])) {
            *p++ = qemu_tolower(rh850_exts[i]);
        }
    }
    *p = '\0';
    return isa_str;
}

typedef struct RH850CPUListState {
    fprintf_function cpu_fprintf;
    FILE *file;
} RH850CPUListState;

static gint rh850_cpu_list_compare(gconstpointer a, gconstpointer b)
{
    ObjectClass *class_a = (ObjectClass *)a;
    ObjectClass *class_b = (ObjectClass *)b;
    const char *name_a, *name_b;

    name_a = object_class_get_name(class_a);
    name_b = object_class_get_name(class_b);
    return strcmp(name_a, name_b);
}

static void rh850_cpu_list_entry(gpointer data, gpointer user_data)
{
    RH850CPUListState *s = user_data;
    const char *typename = object_class_get_name(OBJECT_CLASS(data));
    int len = strlen(typename) - strlen(RH850_CPU_TYPE_SUFFIX);

    (*s->cpu_fprintf)(s->file, "%.*s\n", len, typename);
}

void rh850_cpu_list(FILE *f, fprintf_function cpu_fprintf)
{
    RH850CPUListState s = {
        .cpu_fprintf = cpu_fprintf,
        .file = f,
    };
    GSList *list;

    list = object_class_get_list(TYPE_RH850_CPU, false);
    list = g_slist_sort(list, rh850_cpu_list_compare);
    g_slist_foreach(list, rh850_cpu_list_entry, &s);
    g_slist_free(list);
}

#define DEFINE_CPU(type_name, initfn)      \
    {                                      \
        .name = type_name,                 \
        .parent = TYPE_RH850_CPU,          \
        .instance_init = initfn            \
    }


/*static const TypeInfo rh850_cpu_type_info = {
    .name = TYPE_RH850_CPU,
    .parent = TYPE_CPU,
    .instance_size = sizeof(RH850CPU),
    .instance_init = rh850_cpu_initfn,
    // .instance_post_init = rh850_cpu_post_init,  // not mandatory, RISCV doesn't have this
    // .instance_finalize = rh850_cpu_finalizefn,  // not mandatory, RISCV doesn't have this
    .abstract = true,
    .class_size = sizeof(ARMCPUClass),
};
 */

static const TypeInfo rh850_cpu_type_infos[] = {
    {  // this registers core (abstract CPU)
        .name = TYPE_RH850_CPU, // TYPE_RH850_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(RH850CPU),
        .instance_init = rh850_cpu_init,
        .abstract = true,
        .class_size = sizeof(RH850CPUClass),
        .class_init = rh850_cpu_class_init,
    },
    DEFINE_CPU(TYPE_RH850_CPU_ANY,              rh850_any_cpu_init),
/** #if defined(TARGET_RH850)
    DEFINE_CPU(TYPE_RH850_CPU_RV32GCSU_V1_09_1, rv32gcsu_priv1_09_1_cpu_init),
    DEFINE_CPU(TYPE_RH850_CPU_RV32GCSU_V1_10_0, rv32gcsu_priv1_10_0_cpu_init),
    DEFINE_CPU(TYPE_RH850_CPU_RV32IMACU_NOMMU,  rv32imacu_nommu_cpu_init),
    DEFINE_CPU(TYPE_RH850_CPU_SIFIVE_E31,       rv32imacu_nommu_cpu_init),
    DEFINE_CPU(TYPE_RH850_CPU_SIFIVE_U34,       rv32gcsu_priv1_10_0_cpu_init)
#endif
*/
};

DEFINE_TYPES(rh850_cpu_type_infos)
