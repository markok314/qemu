/*
 * Rh850 virtual CPU header.
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

#ifndef RH850_CPU_H
#define RH850_CPU_H

#define TARGET_LONG_BITS 32

#define CPUArchState struct CPURh850State

/* cpu_rh850_map_address_* in CPURh850TLBContext need this decl.  */
struct Rh850CPU;

#include "qemu-common.h"
#include "exec/cpu-defs.h"
#include "fpu/softfloat.h"
#include "qom/cpu.h"

#define TYPE_RH850_CPU "or1k-cpu"

#define RH850_CPU_CLASS(klass) \
    OBJECT_CLASS_CHECK(Rh850CPUClass, (klass), TYPE_RH850_CPU)
#define RH850_CPU(obj) \
    OBJECT_CHECK(Rh850CPU, (obj), TYPE_RH850_CPU)
#define RH850_CPU_GET_CLASS(obj) \
    OBJECT_GET_CLASS(Rh850CPUClass, (obj), TYPE_RH850_CPU)

/**
 * Rh850CPUClass:
 * @parent_realize: The parent class' realize handler.
 * @parent_reset: The parent class' reset handler.
 *
 * A Rh850 CPU model.
 */
typedef struct Rh850CPUClass {
    /*< private >*/
    CPUClass parent_class;
    /*< public >*/

    DeviceRealize parent_realize;
    void (*parent_reset)(CPUState *cpu);
} Rh850CPUClass;

#define NB_MMU_MODES    3
#define TARGET_INSN_START_EXTRA_WORDS 1

enum {
    MMU_NOMMU_IDX = 0,
    MMU_SUPERVISOR_IDX = 1,
    MMU_USER_IDX = 2,
};

#define TARGET_PAGE_BITS 13

#define TARGET_PHYS_ADDR_SPACE_BITS 32
#define TARGET_VIRT_ADDR_SPACE_BITS 32

#define SET_FP_CAUSE(reg, v)    do {\
                                    (reg) = ((reg) & ~(0x3f << 12)) | \
                                            ((v & 0x3f) << 12);\
                                } while (0)
#define GET_FP_ENABLE(reg)       (((reg) >>  7) & 0x1f)
#define UPDATE_FP_FLAGS(reg, v)   do {\
                                      (reg) |= ((v & 0x1f) << 2);\
                                  } while (0)

/* Version Register */
#define SPR_VR 0xFFFF003F

/* Interrupt */
#define NR_IRQS  32

/* Unit presece register */
enum {
    UPR_UP = (1 << 0),
    UPR_DCP = (1 << 1),
    UPR_ICP = (1 << 2),
    UPR_DMP = (1 << 3),
    UPR_IMP = (1 << 4),
    UPR_MP = (1 << 5),
    UPR_DUP = (1 << 6),
    UPR_PCUR = (1 << 7),
    UPR_PMP = (1 << 8),
    UPR_PICP = (1 << 9),
    UPR_TTP = (1 << 10),
    UPR_CUP = (255 << 24),
};

/* CPU configure register */
enum {
    CPUCFGR_NSGF = (15 << 0),
    CPUCFGR_CGF = (1 << 4),
    CPUCFGR_OB32S = (1 << 5),
    CPUCFGR_OB64S = (1 << 6),
    CPUCFGR_OF32S = (1 << 7),
    CPUCFGR_OF64S = (1 << 8),
    CPUCFGR_OV64S = (1 << 9),
    /* CPUCFGR_ND = (1 << 10), */
    /* CPUCFGR_AVRP = (1 << 11), */
    CPUCFGR_EVBARP = (1 << 12),
    /* CPUCFGR_ISRP = (1 << 13), */
    /* CPUCFGR_AECSRP = (1 << 14), */
};

/* DMMU configure register */
enum {
    DMMUCFGR_NTW = (3 << 0),
    DMMUCFGR_NTS = (7 << 2),
    DMMUCFGR_NAE = (7 << 5),
    DMMUCFGR_CRI = (1 << 8),
    DMMUCFGR_PRI = (1 << 9),
    DMMUCFGR_TEIRI = (1 << 10),
    DMMUCFGR_HTR = (1 << 11),
};

/* IMMU configure register */
enum {
    IMMUCFGR_NTW = (3 << 0),
    IMMUCFGR_NTS = (7 << 2),
    IMMUCFGR_NAE = (7 << 5),
    IMMUCFGR_CRI = (1 << 8),
    IMMUCFGR_PRI = (1 << 9),
    IMMUCFGR_TEIRI = (1 << 10),
    IMMUCFGR_HTR = (1 << 11),
};

/* Power management register */
enum {
    PMR_SDF = (15 << 0),
    PMR_DME = (1 << 4),
    PMR_SME = (1 << 5),
    PMR_DCGE = (1 << 6),
    PMR_SUME = (1 << 7),
};

/* Float point control status register */
enum {
    FPCSR_FPEE = 1,
    FPCSR_RM = (3 << 1),
    FPCSR_OVF = (1 << 3),
    FPCSR_UNF = (1 << 4),
    FPCSR_SNF = (1 << 5),
    FPCSR_QNF = (1 << 6),
    FPCSR_ZF = (1 << 7),
    FPCSR_IXF = (1 << 8),
    FPCSR_IVF = (1 << 9),
    FPCSR_INF = (1 << 10),
    FPCSR_DZF = (1 << 11),
};

/* Exceptions indices */
enum {
    EXCP_RESET    = 0x1,
    EXCP_BUSERR   = 0x2,
    EXCP_DPF      = 0x3,
    EXCP_IPF      = 0x4,
    EXCP_TICK     = 0x5,
    EXCP_ALIGN    = 0x6,
    EXCP_ILLEGAL  = 0x7,
    EXCP_INT      = 0x8,
    EXCP_DTLBMISS = 0x9,
    EXCP_ITLBMISS = 0xa,
    EXCP_RANGE    = 0xb,
    EXCP_SYSCALL  = 0xc,
    EXCP_FPE      = 0xd,
    EXCP_TRAP     = 0xe,
    EXCP_NR,
};

/* Supervisor register */
enum {
    SR_SM = (1 << 0),
    SR_TEE = (1 << 1),
    SR_IEE = (1 << 2),
    SR_DCE = (1 << 3),
    SR_ICE = (1 << 4),
    SR_DME = (1 << 5),
    SR_IME = (1 << 6),
    SR_LEE = (1 << 7),
    SR_CE  = (1 << 8),
    SR_F   = (1 << 9),
    SR_CY  = (1 << 10),
    SR_OV  = (1 << 11),
    SR_OVE = (1 << 12),
    SR_DSX = (1 << 13),
    SR_EPH = (1 << 14),
    SR_FO  = (1 << 15),
    SR_SUMRA = (1 << 16),
    SR_SCE = (1 << 17),
};

/* Tick Timer Mode Register */
enum {
    TTMR_TP = (0xfffffff),
    TTMR_IP = (1 << 28),
    TTMR_IE = (1 << 29),
    TTMR_M  = (3 << 30),
};

/* Timer Mode */
enum {
    TIMER_NONE = (0 << 30),
    TIMER_INTR = (1 << 30),
    TIMER_SHOT = (2 << 30),
    TIMER_CONT = (3 << 30),
};

/* TLB size */
enum {
    DTLB_WAYS = 1,
    DTLB_SIZE = 64,
    DTLB_MASK = (DTLB_SIZE-1),
    ITLB_WAYS = 1,
    ITLB_SIZE = 64,
    ITLB_MASK = (ITLB_SIZE-1),
};

/* TLB prot */
enum {
    URE = (1 << 6),
    UWE = (1 << 7),
    SRE = (1 << 8),
    SWE = (1 << 9),

    SXE = (1 << 6),
    UXE = (1 << 7),
};

/* check if tlb available */
enum {
    TLBRET_INVALID = -3,
    TLBRET_NOMATCH = -2,
    TLBRET_BADADDR = -1,
    TLBRET_MATCH = 0
};

typedef struct Rh850TLBEntry {
    uint32_t mr;
    uint32_t tr;
} Rh850TLBEntry;

#ifndef CONFIG_USER_ONLY
typedef struct CPURh850TLBContext {
    Rh850TLBEntry itlb[ITLB_WAYS][ITLB_SIZE];
    Rh850TLBEntry dtlb[DTLB_WAYS][DTLB_SIZE];

    int (*cpu_rh850_map_address_code)(struct Rh850CPU *cpu,
                                         hwaddr *physical,
                                         int *prot,
                                         target_ulong address, int rw);
    int (*cpu_rh850_map_address_data)(struct Rh850CPU *cpu,
                                         hwaddr *physical,
                                         int *prot,
                                         target_ulong address, int rw);
} CPURh850TLBContext;
#endif

typedef struct CPURh850State {
    target_ulong shadow_gpr[16][32]; /* Shadow registers */

    target_ulong pc;          /* Program counter */
    target_ulong ppc;         /* Prev PC */
    target_ulong jmp_pc;      /* Jump PC */

    uint64_t mac;             /* Multiply registers MACHI:MACLO */

    target_ulong epcr;        /* Exception PC register */
    target_ulong eear;        /* Exception EA register */

    target_ulong sr_f;        /* the SR_F bit, values 0, 1.  */
    target_ulong sr_cy;       /* the SR_CY bit, values 0, 1.  */
    target_long  sr_ov;       /* the SR_OV bit (in the sign bit only) */
    uint32_t sr;              /* Supervisor register, without SR_{F,CY,OV} */
    uint32_t vr;              /* Version register */
    uint32_t upr;             /* Unit presence register */
    uint32_t dmmucfgr;        /* DMMU configure register */
    uint32_t immucfgr;        /* IMMU configure register */
    uint32_t esr;             /* Exception supervisor register */
    uint32_t evbar;           /* Exception vector base address register */
    uint32_t pmr;             /* Power Management Register */
    uint32_t fpcsr;           /* Float register */
    float_status fp_status;

    target_ulong lock_addr;
    target_ulong lock_value;

    uint32_t dflag;           /* In delay slot (boolean) */

    /* Fields up to this point are cleared by a CPU reset */
    struct {} end_reset_fields;

    CPU_COMMON

    /* Fields from here on are preserved across CPU reset. */
    uint32_t cpucfgr;         /* CPU configure register */

#ifndef CONFIG_USER_ONLY
    CPURh850TLBContext * tlb;

    QEMUTimer *timer;
    uint32_t ttmr;          /* Timer tick mode register */
    int is_counting;

    uint32_t picmr;         /* Interrupt mask register */
    uint32_t picsr;         /* Interrupt contrl register*/
#endif
    void *irq[32];          /* Interrupt irq input */
} CPURh850State;

/**
 * Rh850CPU:
 * @env: #CPURh850State
 *
 * A Rh850 CPU.
 */
typedef struct Rh850CPU {
    /*< private >*/
    CPUState parent_obj;
    /*< public >*/

    CPURh850State env;

} Rh850CPU;

static inline Rh850CPU *rh850_env_get_cpu(CPURh850State *env)
{
    return container_of(env, Rh850CPU, env);
}

#define ENV_GET_CPU(e) CPU(rh850_env_get_cpu(e))

#define ENV_OFFSET offsetof(Rh850CPU, env)

void cpu_rh850_list(FILE *f, fprintf_function cpu_fprintf);
void rh850_cpu_do_interrupt(CPUState *cpu);
bool rh850_cpu_exec_interrupt(CPUState *cpu, int int_req);
void rh850_cpu_dump_state(CPUState *cpu, FILE *f,
                             fprintf_function cpu_fprintf, int flags);
hwaddr rh850_cpu_get_phys_page_debug(CPUState *cpu, vaddr addr);
int rh850_cpu_gdb_read_register(CPUState *cpu, uint8_t *buf, int reg);
int rh850_cpu_gdb_write_register(CPUState *cpu, uint8_t *buf, int reg);
void rh850_translate_init(void);
int rh850_cpu_handle_mmu_fault(CPUState *cpu, vaddr address,
                                  int rw, int mmu_idx);
int cpu_rh850_signal_handler(int host_signum, void *pinfo, void *puc);

#define cpu_list cpu_rh850_list
#define cpu_signal_handler cpu_rh850_signal_handler

#ifndef CONFIG_USER_ONLY
extern const struct VMStateDescription vmstate_rh850_cpu;

/* hw/rh850_pic.c */
void cpu_rh850_pic_init(Rh850CPU *cpu);

/* hw/rh850_timer.c */
void cpu_rh850_clock_init(Rh850CPU *cpu);
uint32_t cpu_rh850_count_get(Rh850CPU *cpu);
void cpu_rh850_count_set(Rh850CPU *cpu, uint32_t val);
void cpu_rh850_count_update(Rh850CPU *cpu);
void cpu_rh850_timer_update(Rh850CPU *cpu);
void cpu_rh850_count_start(Rh850CPU *cpu);
void cpu_rh850_count_stop(Rh850CPU *cpu);

void cpu_rh850_mmu_init(Rh850CPU *cpu);
int cpu_rh850_get_phys_nommu(Rh850CPU *cpu,
                                hwaddr *physical,
                                int *prot, target_ulong address, int rw);
int cpu_rh850_get_phys_code(Rh850CPU *cpu,
                               hwaddr *physical,
                               int *prot, target_ulong address, int rw);
int cpu_rh850_get_phys_data(Rh850CPU *cpu,
                               hwaddr *physical,
                               int *prot, target_ulong address, int rw);
#endif

#define cpu_init(cpu_model) cpu_generic_init(TYPE_RH850_CPU, cpu_model)

#define RH850_CPU_TYPE_SUFFIX "-" TYPE_RH850_CPU
#define RH850_CPU_TYPE_NAME(model) model RH850_CPU_TYPE_SUFFIX

#include "exec/cpu-all.h"

#define TB_FLAGS_DFLAG 1
#define TB_FLAGS_R0_0  2
#define TB_FLAGS_OVE   SR_OVE

static inline uint32_t cpu_get_gpr(const CPURh850State *env, int i)
{
    return env->shadow_gpr[0][i];
}

static inline void cpu_set_gpr(CPURh850State *env, int i, uint32_t val)
{
    env->shadow_gpr[0][i] = val;
}

static inline void cpu_get_tb_cpu_state(CPURh850State *env,
                                        target_ulong *pc,
                                        target_ulong *cs_base, uint32_t *flags)
{
    *pc = env->pc;
    *cs_base = 0;
    *flags = (env->dflag
              | (cpu_get_gpr(env, 0) == 0 ? TB_FLAGS_R0_0 : 0)
              | (env->sr & SR_OVE));
}

static inline int cpu_mmu_index(CPURh850State *env, bool ifetch)
{
    if (!(env->sr & SR_IME)) {
        return MMU_NOMMU_IDX;
    }
    return (env->sr & SR_SM) == 0 ? MMU_USER_IDX : MMU_SUPERVISOR_IDX;
}

static inline uint32_t cpu_get_sr(const CPURh850State *env)
{
    return (env->sr
            + env->sr_f * SR_F
            + env->sr_cy * SR_CY
            + (env->sr_ov < 0) * SR_OV);
}

static inline void cpu_set_sr(CPURh850State *env, uint32_t val)
{
    env->sr_f = (val & SR_F) != 0;
    env->sr_cy = (val & SR_CY) != 0;
    env->sr_ov = (val & SR_OV ? -1 : 0);
    env->sr = (val & ~(SR_F | SR_CY | SR_OV)) | SR_FO;
}

#define CPU_INTERRUPT_TIMER   CPU_INTERRUPT_TGT_INT_0

#endif /* RH850_CPU_H */
