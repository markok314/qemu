/*
 * QEMU RH850 CPU
 *
 * Copyright (c) 2018-2019 iSYSTEM Labs d.o.o.
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

/* Program registers (rh850_prog_regnames):
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

const char * const rh850_prog_regnames[] = {
  "r0-zero", "r1", "r2", "r3-sp", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10 ", "r11", "r12", "r13", "r14", "r15",
  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r2 ",
  "r24", "r25", "r26", "r27", "r28", "r29", "r30-ep", "r31-lp"
};

/* Basic system registers (rh850_sys__basic_regnames):
 * sr0, 0  - eipc
 * sr1, 0  - eipsw
 * sr2, 0  - fepc
 * sr3, 0  - fepsw
 * sr5, 0  - psw
 * sr6, 0  - fpsr
 * sr7, 0  - fpepc
 * sr8, 0  - fpst
 * sr9, 0  - fpcc
 * sr10, 0 - fpcfg
 * sr11, 0 - fpec
 * sr13, 0 - eiic
 * sr14, 0 - feic
 * sr16, 0 - ctpc
 * sr17, 0 - ctpsw
 * sr20, 0 - ctbp
 * sr28, 0 - eiwr
 * sr29, 0 - fewr
 * sr31, 0 - bsel (backward compatibility with v850e2)
 * sr0, 1  - mcfg0
 * sr2, 1  - rbase
 * sr3, 1  - ebase
 * sr4, 1  - intbp
 * sr5, 1  - mctl
 * sr6, 1  - pid
 * sr11, 1 - sccfg
 * sr12, 1 - scbp
 * sr0, 2  - htcfg0
 * sr6, 2  - mea
 * sr7, 2  - asid
 * sr8, 2  - mei
 */

const char * const rh850_sys_basic_regnames[] = {
  "eipc", "eipsw", "fepc", "fepsw", "psw",  "fpsr", "fpepc",  "fpst",
  "fpcc", "fpcfg", "fpec", "eiic",  "feic", "ctpc", "ctpsw",  "ctbp",
  "eiwr", "fewr",  "bsel", "mcfg0", "rbase","ebase","intbp ", "mctl",
  "pid ", "sccfg", "scbp", "htcfg0","mea",  "asid", "mei",    ""
};

/* Interrupt function registers (rh850_sys_intr_regnames):
 * sr7,  1 - fpipr
 * sr10, 2 - ispr
 * sr11, 2 - pmr
 * sr12, 2 - icsr
 * sr13, 2 - intcfg
 */

const char * const rh850_sys_intr_regnames[] = { /*Interrupt function registers*/
  "fpipr", "ispr", "pmr", "icsr", "intcfg"
};

/* Floating point function registers (rh850_sys_fpr_regnames):
 * sr6, 0  - fpsr
 * sr7, 0  - fpepc
 * sr8, 0  - fpst
 * sr9, 0  - fpcc
 * sr10, 0 - fpcfg
 * sr11, 0 - fpec
 */


const char * const rh850_sys_fpr_regnames[] = { /*Floating point function registers*/
  "fpsr ", "fpepc ", "fpst ", "fpcc ", "fpcfg ", "fpec "
};

/* MPU function system registers (rh850_sys_mpu_regnames):
 * sr0, 5  - mpm
 * sr1, 5  - mprc
 * sr4, 5  - mpbrgn
 * sr5, 5  - mptrgn
 * sr8, 5  - mca
 * sr9, 5  - mcs
 * sr10, 5 - mcc
 * sr11, 5 - mcr
 * sr0, 6  - mpla0
 * sr1, 6  - mpua0
 * sr2, 6  - mpat0
 * sr4, 6  - mpla1
 * sr5, 6  - mpua1
 * sr6, 6  - mpat1
 * sr8, 6  - mpla2
 * sr9, 6  - mpua2
 * sr10, 6 - mpat2
 * sr12, 6 - mpla3
 * sr13, 6 - mpua3
 * sr14, 6 - mpat3
 * sr16, 6 - mpla4
 * sr17, 6 - mpua4
 * sr18, 6 - mpat4
 * sr20, 6 - mpla5
 * sr21, 6 - mpua5
 * sr22, 6 - mpat5
 * sr24, 6 - mpla6
 * sr25, 6 - mpua6
 * sr26, 6 - mpat6
 * sr28, 6 - mpla7
 * sr29, 6 - mpua7
 * sr30, 6 - mpat7
 * sr0, 7  - mpla8
 * sr1, 7  - mpua8
 * sr2, 7  - mpat8
 * sr4, 7  - mpla9
 * sr5, 7  - mpua9
 * sr6, 7  - mpat9
 * sr8, 7  - mpla10
 * sr9, 7  - mpua10
 * sr10, 7 - mpat10
 * sr12, 7 - mpla11
 * sr13, 7 - mpua11
 * sr14, 7 - mpat11
 * sr16, 7 - mpla12
 * sr17, 7 - mpua12
 * sr18, 7 - mpat12
 * sr20, 7 - mpla13
 * sr21, 7 - mpua13
 * sr22, 7 - mpat13
 * sr24, 7 - mpla14
 * sr25, 7 - mpua14
 * sr26, 7 - mpat14
 * sr28, 7 - mpla15
 * sr29, 7 - mpua15
 * sr30, 7 - mpat15
 */

const char * const rh850_sys_mpu_regnames[] = { /* MPU function system registers */
		"mpm", "mprc", "mpbrgn", "mptrgn", "mca", "mcs",
		"mcc", "mcr", "mpla0", "mpua0", "mpat0", "mpla1",
		"mpua1", "mpat1", "mpla2", "mpua2", "mpat2", "mpla3",
		"mpua3", "mpat3", "mpla4", "mpua4", "mpat4", "mpla5",
		"mpua5", "mpat5", "mpla6", "mpua6", "mpat6", "mpla7",
		"mpua7", "mpat7", "mpla8", "mpua8", "mpat8", "mpla9",
		"mpua9", "mpat9", "mpla10", "mpua10", "mpat10", "mpla11",
		"mpua11", "mpat11", "mpla12", "mpua12", "mpat12", "mpla13",
		"mpua13", "mpat13", "mpla14", "mpua14", "mpat14", "mpla15",
		"mpua15", "mpat15"
};

/* Cache operation function registers (rh850_sys_cacheop_regnames):
 * sr16, 4 - ictagl
 * sr17, 4 - ictagh
 * sr18, 4 - icdatl
 * sr19, 4 - icdath
 * sr24, 4 - icctrl
 * sr26, 4 - iccfg
 * sr28, 4 - icerr
 */

const char * const rh850_sys_cacheop_regnames[] = { /* Cache operation function registers */
  "ictagl", "ictagh", "icdatl", "icdath", "icctrl", "iccfg", "icerr"
};

/*Data Buffer Operation Registers (rh850_sys_databuff_regnames):
 * sr24, 13 - cbdcr
 */

const char * const rh850_sys_databuff_regnames[] = { /* Data buffer operation registers */
  "cbdcr"
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
    env->sysBasicRegs[rbase_idx] = resetvec;
}

static void rh850_any_cpu_init(Object *obj)
{
    CPURH850State *env = &RH850_CPU(obj)->env;
    env->features = 0;  // no features at the moment (eg. no MMU, no FPU).
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
/*
// isys
static int is_supervisor_mode(CPUState *cs)
{
	CPUState *cs
}
*/

static void rh850_cpu_dump_state(CPUState *cs, FILE *f,
    fprintf_function cpu_fprintf, int flags)
{
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;
    int i;

    cpu_fprintf(f, " %-7s " TARGET_FMT_lx "\n", "pc", env->pc);
    cpu_fprintf(f, " %-7s " TARGET_FMT_lx "\n", "psw", env->sysBasicRegs[psw_idx]);

    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "eiic    ", env->eiic);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "feic    ", env->feic);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mcfg0   ", env->mcfg0);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "rbase   ", env->rbase);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "ebase   ", env->ebase);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mctl    ", env->mctl);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "pid     ", env->pid);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "htcfg0  ", env->htcfg0);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mea     ", env->mea);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mei     ", env->mei);
#ifndef CONFIG_USER_ONLY
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mhartid ", env->mhartid);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mstatus ", env->mstatus);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mip     ",
    //    (target_ulong)atomic_read(&env->mip));
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mie     ", env->mie);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mideleg ", env->mideleg);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "medeleg ", env->medeleg);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mtvec   ", env->mtvec);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mepc    ", env->mepc);
    //cpu_fprintf(f, " %s " TARGET_FMT_lx "\n", "mcause  ", env->mcause);
#endif

    for (i = 0; i < 32; i++) {
        cpu_fprintf(f, " %-7s " TARGET_FMT_lx,
            rh850_prog_regnames[i], env->progRegs[i]);
        if ((i & 3) == 3) {
            cpu_fprintf(f, "\n");
        }
    }

    cpu_fprintf(f, "\n");

    for (i = 0; i < 31; i++) {
    	if (i!=psw_idx){
        cpu_fprintf(f, " %-7s " TARGET_FMT_lx,
            rh850_sys_basic_regnames[i], env->sysBasicRegs[i]);
    	}
        if ((i & 3) == 3) {
            cpu_fprintf(f, "\n");
        }
    }

    cpu_fprintf(f, "\n");

    for (i = 0; i < 5; i++) {
        cpu_fprintf(f, " %-7s " TARGET_FMT_lx,
            rh850_sys_intr_regnames[i], env->sysInterruptRegs[i]);
        if ((i & 3) == 3) {
            cpu_fprintf(f, "\n");
        }

    }

    cpu_fprintf(f, "\n");


    for (i = 0; i < 6; i++) {
        cpu_fprintf(f, " %s %016" PRIx64,
        		rh850_sys_fpr_regnames[i], env->sysFpuRegs[i]);
        if ((i & 3) == 3) {
            cpu_fprintf(f, "\n");
        }
    }

    cpu_fprintf(f, "\n");

    for (i = 0; i < 56; i++) {
		cpu_fprintf(f, " %s " TARGET_FMT_lx,
			rh850_sys_mpu_regnames[i], env->sysMpuRegs[i]);
		if ((i & 3) == 3) {
			cpu_fprintf(f, "\n");
		}

	}

	cpu_fprintf(f, "\n");

	for (i = 0; i < 7; i++) {
		cpu_fprintf(f, " %s " TARGET_FMT_lx,
			rh850_sys_cacheop_regnames[i], env->sysCacheRegs[i]);
		if ((i & 3) == 3) {
			cpu_fprintf(f, "\n");
		}

	}

	cpu_fprintf(f, "\n");

	cpu_fprintf(f, " %s " TARGET_FMT_lx,
				rh850_sys_databuff_regnames[0], env->sysDatabuffRegs[0]);
	cpu_fprintf(f, "\n");
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


static gchar *rh850_gdb_arch_name(CPUState *cs)
{
    return g_strdup("rh850");
}

/* not yet adapted for rh850 from ARM
int rh850_gen_dynamic_xml(CPUState *cs)
{
    RH850CPU *cpu = RH850_CPU(cs);
    GString *s = g_string_new(NULL);
    RegisterSysregXmlParam param = {cs, s};

    cpu->dyn_xml.num_cpregs = 0;
    cpu->dyn_xml.cpregs_keys = g_new(uint32_t, g_hash_table_size(cpu->cp_regs));
    g_string_printf(s, "<?xml version=\"1.0\"?>");
    g_string_append_printf(s, "<!DOCTYPE target SYSTEM \"gdb-target.dtd\">");
    g_string_append_printf(s, "<feature name=\"org.qemu.gdb.arm.sys.regs\">");
    g_hash_table_foreach(cpu->cp_regs, arm_register_sysreg_for_xml, &param);
    g_string_append_printf(s, "</feature>");
    cpu->dyn_xml.desc = g_string_free(s, false);
    return cpu->dyn_xml.num_cpregs;
} */


static const char *rh850_gdb_get_dynamic_xml(CPUState *cs, const char *xmlname)
{
    // RH850CPU *cpu = RH850_CPU(cs);

    if (strcmp(xmlname, "system-registers.xml") == 0) {
        return "rh850-core.xml";
    }
    return NULL;
}


static void rh850_raise_exception(CPURH850State *env, uint32_t excp,
                           uint32_t syndrome, uint32_t target_el)
{
    CPUState *cs = CPU(rh850_env_get_cpu(env));

    cs->exception_index = excp;
    // env->exception.syndrome = syndrome;
    // env->exception.target_el = target_el;
    cpu_loop_exit(cs);
}


static void rh850_debug_excp_handler(CPUState *cs)
{
    /* Called by core code when a watchpoint or breakpoint fires;
     * need to check which one and raise the appropriate exception.
     */
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;
    CPUWatchpoint *wp_hit = cs->watchpoint_hit;

    if (wp_hit) {
        if (wp_hit->flags & BP_CPU) {
            // bool wnr = (wp_hit->flags & BP_WATCHPOINT_HIT_WRITE) != 0;
            // bool same_el = true;

            cs->watchpoint_hit = NULL;

            // env->exception.fsr = arm_debug_exception_fsr(env);
            // env->exception.vaddress = wp_hit->hitaddr;
            rh850_raise_exception(env, 0, 0, 0);
        }
    } else {
        uint64_t pc = env->pc;
        // bool same_el = true;

        /* (1) GDB breakpoints should be handled first.
         * (2) Do not raise a CPU exception if no CPU breakpoint has fired,
         * since singlestep is also done by generating a debug internal
         * exception.
         */
        if (cpu_breakpoint_test(cs, pc, BP_GDB)
            || !cpu_breakpoint_test(cs, pc, BP_CPU)) {
            return;
        }

        // env->exception.fsr = arm_debug_exception_fsr(env);
        // env->exception.vaddress = 0;
        rh850_raise_exception(env, 0, 0, 0);
    }
}

static bool check_watchpoints(RH850CPU *cpu)
{
    // CPURH850State *env = &cpu->env;

    /* If watchpoints are disabled globally or we can't take debug
     * exceptions here then watchpoint firings are ignored.
     */
//    if (extract32(env->cp15.mdscr_el1, 15, 1) == 0
//        || !arm_generate_debug_exceptions(env)) {
//        return false;
//    }

//    for (int n = 0; n < ARRAY_SIZE(env->cpu_watchpoint); n++) {
//        if (bp_wp_matches(cpu, n, true)) {
//            return true;
//        }
//    }
//    return false;
    return true;
}


//static bool bp_wp_matches(RH850CPU *cpu, int n, bool is_wp)
//{
//     CPURH850State *env = &cpu->env;

//    if (is_wp) {
//        CPUWatchpoint *wp = env->cpu_watchpoint[n];
//
//        if (!wp || !(wp->flags & BP_WATCHPOINT_HIT)) {
//            return false;
//        }
//    } else {
//        uint64_t pc = is_a64(env) ? env->pc : env->regs[15];
//
//        if (!env->cpu_breakpoint[n] || env->cpu_breakpoint[n]->pc != pc) {
//            return false;
//        }
//    }
    /* The WATCHPOINT_HIT flag guarantees us that the watchpoint is
     * enabled and that the address and access type match; for breakpoints
     * we know the address matched; check the remaining fields, including
     * linked breakpoints. We rely on WCR and BCR having the same layout
     * for the LBN, SSC, HMC, PAC/PMC and is-linked fields.
     * Note that some combinations of {PAC, HMC, SSC} are reserved and
     * must act either like some valid combination or as if the watchpoint
     * were disabled. We choose the former, and use this together with
     * the fact that EL3 must always be Secure and EL2 must always be
     * Non-Secure to simplify the code slightly compared to the full
     * table in the ARM ARM.
     */

//    return true;
//}


//static bool check_breakpoints(RH850CPU *cpu)
//{
//    CPURH850State *env = &cpu->env;
//    int n;
//
//    for (n = 0; n < ARRAY_SIZE(env->cpu_breakpoint); n++) {
//        if (bp_wp_matches(cpu, n, false)) {
//            return true;
//        }
//    }
//    return false;
//	return true;
//}


static bool rh850_debug_check_watchpoint(CPUState *cs, CPUWatchpoint *wp)
{
    /* Called by core code when a CPU watchpoint fires; need to check if this
     * is also an architectural watchpoint match.
     */
    RH850CPU *cpu = RH850_CPU(cs);

    return check_watchpoints(cpu);
}


static void rh850_cpu_reset(CPUState *cs)
{

	RH850CPU *cpu = RH850_CPU(cs);
    RH850CPUClass *mcc = RH850_CPU_GET_CLASS(cpu);
    CPURH850State *env = &cpu->env;

    mcc->parent_reset(cs);
#ifndef CONFIG_USER_ONLY
    env->priv = PRV_M;   //sets machine privilege level, make for rh850
    env->mstatus &= ~(MSTATUS_MIE | MSTATUS_MPRV); //
    env->mcause = 0;
    env->pc = 0;
    env->sysBasicRegs[psw_idx] = 0x20; // reset value of PSW
    env->sysBasicRegs[rbase_idx] = 0;
#endif
    cs->exception_index = EXCP_NONE;
    set_default_nan_mode(1, &env->fp_status);
    env->pc = 0;
    env->sysBasicRegs[psw_idx] = 0x20; // reset value of PSW
    env->sysBasicRegs[rbase_idx] = 0;
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
    // see rh850/gdbstub.c:: rh850_cpu_gdb_read_register for number of regs supported for gdb
    cc->gdb_num_core_regs = 79;
    cc->gdb_stop_before_watchpoint = true;

    cc->gdb_core_xml_file = "rh850-core.xml";
    cc->gdb_arch_name = rh850_gdb_arch_name;
    cc->gdb_get_dynamic_xml = rh850_gdb_get_dynamic_xml;
    cc->gdb_stop_before_watchpoint = true;
    cc->debug_excp_handler = rh850_debug_excp_handler;
    cc->debug_check_watchpoint = rh850_debug_check_watchpoint;

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
