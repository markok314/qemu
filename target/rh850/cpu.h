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

#ifndef RH850_CPU_H
#define RH850_CPU_H

/* QEMU addressing/paging config */
#define TARGET_PAGE_BITS 12 /* 4 KiB Pages */

#if defined(TARGET_RH850)
#define TARGET_LONG_BITS 32
#define TARGET_PHYS_ADDR_SPACE_BITS 34
#define TARGET_VIRT_ADDR_SPACE_BITS 32
#endif

#define TCG_GUEST_DEFAULT_MO 0

#define ELF_MACHINE EM_RH850
#define CPUArchState struct CPURH850State

#include "qemu-common.h"
#include "qom/cpu.h"
#include "exec/cpu-defs.h"
#include "fpu/softfloat.h"

#define TYPE_RH850_CPU "rh850-cpu"

#define RH850_CPU_TYPE_SUFFIX "-" TYPE_RH850_CPU
#define RH850_CPU_TYPE_NAME(name) (name RH850_CPU_TYPE_SUFFIX)
#define CPU_RESOLVING_TYPE TYPE_RH850_CPU
#define TYPE_RH850_CPU_ANY              RH850_CPU_TYPE_NAME("any")
/*
#define TYPE_RH850_CPU_RV32GCSU_V1_09_1 RH850_CPU_TYPE_NAME("rv32gcsu-v1.9.1")
#define TYPE_RH850_CPU_RV32GCSU_V1_10_0 RH850_CPU_TYPE_NAME("rv32gcsu-v1.10.0")
#define TYPE_RH850_CPU_RV32IMACU_NOMMU  RH850_CPU_TYPE_NAME("rv32imacu-nommu")
#define TYPE_RH850_CPU_RV64GCSU_V1_09_1 RH850_CPU_TYPE_NAME("rv64gcsu-v1.9.1")
#define TYPE_RH850_CPU_RV64GCSU_V1_10_0 RH850_CPU_TYPE_NAME("rv64gcsu-v1.10.0")
#define TYPE_RH850_CPU_RV64IMACU_NOMMU  RH850_CPU_TYPE_NAME("rv64imacu-nommu")
#define TYPE_RH850_CPU_SIFIVE_E31       RH850_CPU_TYPE_NAME("sifive-e31")
#define TYPE_RH850_CPU_SIFIVE_E51       RH850_CPU_TYPE_NAME("sifive-e51")
#define TYPE_RH850_CPU_SIFIVE_U34       RH850_CPU_TYPE_NAME("sifive-u34")
#define TYPE_RH850_CPU_SIFIVE_U54       RH850_CPU_TYPE_NAME("sifive-u54")
*/
#define RV32 ((target_ulong)1 << (TARGET_LONG_BITS - 2))
#define RV64 ((target_ulong)2 << (TARGET_LONG_BITS - 2))

#if defined(TARGET_RH850)
#define RVXLEN RV32
#elif defined(TARGET_RH85064)
#define RVXLEN RV64
#endif

#define RV(x) ((target_ulong)1 << (x - 'A'))

#define RVI RV('I')
#define RVM RV('M')
#define RVA RV('A')
#define RVF RV('F')
#define RVD RV('D')
#define RVC RV('C')
#define RVS RV('S')
#define RVU RV('U')

/* S extension denotes that Supervisor mode exists, however it is possible
   to have a core that support S mode but does not have an MMU and there
   is currently no bit in misa to indicate whether an MMU exists or not
   so a cpu features bitfield is required */
enum {
    RH850_FEATURE_MMU
};

#define USER_VERSION_2_02_0 0x00020200
#define PRIV_VERSION_1_09_1 0x00010901
#define PRIV_VERSION_1_10_0 0x00011000

#define TRANSLATE_FAIL 1
#define TRANSLATE_SUCCESS 0
#define NB_MMU_MODES 4
#define MMU_USER_IDX 3

#define MAX_RH850_PMPS (16)

typedef struct CPURH850State CPURH850State;

#include "pmp.h"

#include "register_indices.h"

struct CPURH850State {


    //target_ulong gpr[32];   /*rh850 program registers*/
    //uint64_t fpr[32]; /* assume both F and D extensions */
    //target_ulong pc;


    target_ulong progRegs[32];
    target_ulong sysBasicRegs[31];
    target_ulong sysInterruptRegs[5];
    uint64_t sysFpuRegs[6];  //using rh850 basic system registers(sr6-sr11), 32-bit or 64-bit precision
    target_ulong sysMpuRegs[56];
    target_ulong sysCacheRegs[7];
    target_ulong sysDatabuffRegs[1];
    target_ulong pc;

    // flags contained in PSW register
    uint32_t Z_flag;
    uint32_t S_flag;
    uint32_t OV_flag;
    uint32_t CY_flag;
    uint32_t SAT_flag;
    uint32_t ID_flag;
    uint32_t EP_flag;
    uint32_t NP_flag;
    uint32_t EBV_flag;
    uint32_t CU0_flag;
    uint32_t CU1_flag;
    uint32_t CU2_flag;
    uint32_t UM_flag;

    uint32_t condSatisfied;



    target_ulong icsr;		//interrupt control status register
    target_ulong intcfg;	//interrupt function setting

    target_ulong fpsr;		//floating-point configuration/status   <---write the bit defines
    target_ulong fpepc;		//floating point exception PC

    target_ulong mpm;		//memory protection operation mode


    target_ulong load_res;    	// inst addr for TCG
    target_ulong load_val;    	// inst val for TCG

    target_ulong cpu_LLbit;    	// register for mutual exclusion (LDL.W, STC.W)
    target_ulong cpu_LLAddress;    	// register for mutual exclusion (LDL.W, STC.W)

    target_ulong frm;			//  CSR floating point rounding mode

    target_ulong badaddr;		//changed to mea

    //target_ulong user_ver;
    target_ulong priv_ver;

    target_ulong misa;

    uint32_t features;

#ifndef CONFIG_USER_ONLY
    target_ulong priv;

    target_ulong mhartid;		//hardware thread ID  ===> is this same as HTCFG0.PEID ???? rh850 doesnt support multithread?
    target_ulong mstatus;		//machine status
    /*
     * CAUTION! Unlike the rest of this struct, mip is accessed asynchonously
     * by I/O threads and other vCPUs, so hold the iothread mutex before
     * operating on it.  CPU_INTERRUPT_HARD should be in effect iff this is
     * non-zero.  Use rh850_cpu_set_local_interrupt.
     */
    uint32_t mip;        /* allow atomic_read for >= 32-bit hosts */
    target_ulong mie; 		//machine interrupt enable
    target_ulong mideleg;	//machine interrupt delegation register

    target_ulong sptbr;  /* until: priv-1.9.1 */
    target_ulong satp;   /* since: priv-1.10.0 */
    target_ulong sbadaddr;
    target_ulong mbadaddr;
    target_ulong medeleg;

    target_ulong stvec;		//supervisor trap vector base
    target_ulong sepc;		//supervisor exception program counter
    target_ulong scause;	//suprevisor cause register

    target_ulong mtvec;		//machine trap handler base address
    target_ulong mepc;		//machine exception program counter
    target_ulong mcause;	//machine trap cause
    target_ulong mtval;  /* since: priv-1.10.0 */

    uint32_t mucounteren;	//user counter enable
    uint32_t mscounteren;	//supervisor counter enable
    target_ulong scounteren; /* since: priv-1.10.0 */
    target_ulong mcounteren; /* since: priv-1.10.0 */

    target_ulong sscratch;
    target_ulong mscratch;

    /* temporary htif regs */
    //uint64_t mfromhost;
    //uint64_t mtohost;
    //uint64_t timecmp;

    /* physical memory protection */
    pmp_table_t pmp_state;				//this should be modified
#endif

    float_status fp_status;

    /* QEMU */
    CPU_COMMON

    /* Fields from here on are preserved across CPU reset. */
    QEMUTimer *timer; /* Internal timer */
};

#define RH850_CPU_CLASS(klass) \
    OBJECT_CLASS_CHECK(RH850CPUClass, (klass), TYPE_RH850_CPU)
#define RH850_CPU(obj) \
    OBJECT_CHECK(RH850CPU, (obj), TYPE_RH850_CPU)
#define RH850_CPU_GET_CLASS(obj) \
    OBJECT_GET_CLASS(RH850CPUClass, (obj), TYPE_RH850_CPU)

/**
 * RH850CPUClass:
 * @parent_realize: The parent class' realize handler.
 * @parent_reset: The parent class' reset handler.
 *
 * A RH850 CPU model.
 */
typedef struct RH850CPUClass {
    /*< private >*/
    CPUClass parent_class;
    /*< public >*/
    DeviceRealize parent_realize;
    void (*parent_reset)(CPUState *cpu);
} RH850CPUClass;

/**
 * RH850CPU:
 * @env: #CPURH850State
 *
 * A RH850 CPU.
 */
typedef struct RH850CPU {
    /*< private >*/
    CPUState parent_obj;
    /*< public >*/
    CPURH850State env;
} RH850CPU;

static inline RH850CPU *rh850_env_get_cpu(CPURH850State *env)
{
    return container_of(env, RH850CPU, env);
}

static inline int rh850_has_ext(CPURH850State *env, target_ulong ext)
{		// TODO: what does value 'ext' represent??
    return (env->misa & ext) != 0;
	//return false;
}

static inline bool rh850_feature(CPURH850State *env, int feature)
{
    return env->features & (1ULL << feature);
}

#include "cpu_user.h"
#include "cpu_bits.h"

extern const char * const rh850_prog_regnames[];
extern const char * const rh850_sys_basic_regnames[];
extern const char * const rh850_sys_intr_regnames[];
extern const char * const rh850_sys_fpr_regnames[];
extern const char * const rh850_sys_mpu_regnames[];
extern const char * const rh850_sys_cacheop_regnames[];
extern const char * const rh850_sys_databuff_regnames[];

extern const char * const rh850_excp_names[];
extern const char * const rh850_intr_names[];

#define ENV_GET_CPU(e) CPU(rh850_env_get_cpu(e))
#define ENV_OFFSET offsetof(RH850CPU, env)

void rh850_cpu_do_interrupt(CPUState *cpu);
int rh850_cpu_gdb_read_register(CPUState *cpu, uint8_t *buf, int reg);
int rh850_cpu_gdb_write_register(CPUState *cpu, uint8_t *buf, int reg);
bool rh850_cpu_exec_interrupt(CPUState *cs, int interrupt_request);
int rh850_cpu_mmu_index(CPURH850State *env, bool ifetch);
hwaddr rh850_cpu_get_phys_page_debug(CPUState *cpu, vaddr addr);
void  rh850_cpu_do_unaligned_access(CPUState *cs, vaddr addr,
                                    MMUAccessType access_type, int mmu_idx,
                                    uintptr_t retaddr);
int rh850_cpu_handle_mmu_fault(CPUState *cpu, vaddr address, int size,
                              int rw, int mmu_idx);

char *rh850_isa_string(RH850CPU *cpu);
void rh850_cpu_list(FILE *f, fprintf_function cpu_fprintf);

#define cpu_init(cpu_model) cpu_generic_init(TYPE_RH850_CPU, cpu_model)
#define cpu_signal_handler cpu_rh850_signal_handler
#define cpu_list rh850_cpu_list
#define cpu_mmu_index rh850_cpu_mmu_index

void rh850_set_mode(CPURH850State *env, target_ulong newpriv);

void rh850_translate_init(void);
RH850CPU *cpu_rh850_init(const char *cpu_model);
int cpu_rh850_signal_handler(int host_signum, void *pinfo, void *puc);
void QEMU_NORETURN do_raise_exception_err(CPURH850State *env,
                                          uint32_t exception, uintptr_t pc);

target_ulong cpu_rh850_get_fflags(CPURH850State *env);
void cpu_rh850_set_fflags(CPURH850State *env, target_ulong);

#define TB_FLAGS_MMU_MASK  3
#define TB_FLAGS_FP_ENABLE MSTATUS_FS

static inline void cpu_get_tb_cpu_state(CPURH850State *env, target_ulong *pc,
                                        target_ulong *cs_base, uint32_t *flags)
{
    *pc = env->pc;
    *cs_base = 0;
#ifdef CONFIG_USER_ONLY
    *flags = TB_FLAGS_FP_ENABLE;
#else
    *flags = cpu_mmu_index(env, 0) | (env->mstatus & MSTATUS_FS);
#endif
}

void csr_write_helper(CPURH850State *env, target_ulong val_to_write,
        target_ulong csrno);
target_ulong csr_read_helper(CPURH850State *env, target_ulong csrno);

#ifndef CONFIG_USER_ONLY
void rh850_set_local_interrupt(RH850CPU *cpu, target_ulong mask, int value);
#endif

#include "exec/cpu-all.h"

#endif /* RH850_CPU_H */
