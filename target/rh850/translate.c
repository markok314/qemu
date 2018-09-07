/*
 * RH850 emulation for qemu: main translation routines.
 *
 * Copyright (c) 2016-2017 Sagar Karandikar, sagark@eecs.berkeley.edu
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
#include "tcg-op.h"
#include "disas/disas.h"
#include "exec/cpu_ldst.h"
#include "exec/exec-all.h"
#include "exec/helper-proto.h"
#include "exec/helper-gen.h"

#include "exec/log.h"

#include "instmap.h"

/* global register indices */
static TCGv cpu_gpr[32], cpu_pc, cpu_sysRegs[30], cpu_sysIntrRegs[5], cpu_sysMpuRegs[56],
			cpu_sysCacheRegs[7], cpu_sysDatabuffRegs[1];
static TCGv_i64 cpu_fpr[32]; /* assume F and D extensions */
static TCGv load_res;
static TCGv load_val;

// PSW register flags
TCGv_i32 cpu_ZF, cpu_SF, cpu_OVF, cpu_CYF, cpu_SATF, cpu_ID, cpu_EP, cpu_NP,
		cpu_EBV, cpu_CU0, cpu_CU1, cpu_CU2, cpu_UM;


// system registers indices
enum{
	EIPC_register 	= 0,
	EIPSW_register 	= 1,
	FEPC_register 	= 2,
	FEPSW_register 	= 3,
	PSW_register 	= 5,
	FPSR_register	= 6,
	FPEPC_register	= 7,
	FPST_register 	= 8,
	FPCC_register	= 9,
	FPCFG_register	= 10,
	FPEC_register	= 11,
	EIIC_register	= 13,
	FEIC_register 	= 14,
	CTPC_register	= 16,
	CTPSW_register	= 17,
	CTBP_register	= 20,
	EIWR_register	= 28,
	FEWR_register	= 29,
	BSEL_register	= 31,

};

#include "exec/gen-icount.h"

typedef struct DisasContext {
    struct TranslationBlock *tb;
    target_ulong pc;
    target_ulong next_pc;
    uint32_t opcode;
    uint32_t opcode1;
    uint32_t flags;
    uint32_t mem_idx;


    int singlestep_enabled;
    int bstate;
    /* Remember the rounding mode encoded in the previous fp instruction,
       which we have already installed into env->fp_status.  Or -1 for
       no previous fp instruction.  Note 32that we exit the TB when writing
       to any system register, which includes CSR_FRM, so we do not have
       to reset this known value.  */
    int frm;
} DisasContext;

enum {
    BS_NONE     = 0, /* When seen outside of translation while loop, indicates
                     need to exit tb due to end of page. */
    BS_STOP     = 1, /* Need to exit tb for syscall, sret, etc. */
    BS_BRANCH   = 2, /* Need to exit tb for branch, jal, etc. */
};

/* convert rh850 funct3 to qemu memop for load/store */
/*
static const int tcg_memop_lookup[8] = {
    [0 ... 7] = -1,
	[0] = MO_UB,
	[1] = MO_TEUW,
	[2] = MO_TEUL,
	[4] = MO_SB,
	[5] = MO_TESW,
	[6] = MO_TESL,
};
*/


enum {
	V_COND 		= 0,		//OV = 1
	C_COND 		= 1,		//CY = 1
	Z_COND 		= 2,		//Z = 1
	NH_COND 	= 3,		//(CY or Z) = 1
	S_COND		= 4,		//S = 1
	T_COND		= 5,		//Always
	LT_COND		= 6,		//(S xor OV) = 1
	LE_COND 	= 7,		//((S xor OV) or Z) = 1

	NV_COND 	= 8,		//OV = 0
	NC_COND 	= 9,		//CY = 0
	NZ_COND 	= 10,		//Z = 0
	H_COND 		= 11,		//(CY or Z) = 0
	NS_COND		= 12,		//S = 0
	SA_COND		= 13,		//SAT = 1
	GE_COND		= 14,		//(S xor OV) = 0
	GT_COND 	= 15,		//((S xor OV) or Z) = 0
};

#define CASE_OP_32_64(X) case X
/*
static void generate_exception(DisasContext *ctx, int excp)
{
    tcg_gen_movi_tl(cpu_pc, ctx->pc);
    TCGv_i32 helper_tmp = tcg_const_i32(excp);
    gen_helper_raise_exception(cpu_env, helper_tmp);
    tcg_temp_free_i32(helper_tmp);
    ctx->bstate = BS_BRANCH;
}

*/

static void generate_exception_mbadaddr(DisasContext *ctx, int excp)
{
    tcg_gen_movi_tl(cpu_pc, ctx->pc);
    tcg_gen_st_tl(cpu_pc, cpu_env, offsetof(CPURH850State, badaddr));
    TCGv_i32 helper_tmp = tcg_const_i32(excp);
    gen_helper_raise_exception(cpu_env, helper_tmp);
    tcg_temp_free_i32(helper_tmp);
    ctx->bstate = BS_BRANCH;
}

static void gen_exception_inst_addr_mis(DisasContext *ctx)
{
    generate_exception_mbadaddr(ctx, 0xc0);
}



static void gen_exception_debug(void)
{
    TCGv_i32 helper_tmp = tcg_const_i32(EXCP_DEBUG);
    gen_helper_raise_exception(cpu_env, helper_tmp);
    tcg_temp_free_i32(helper_tmp);
}
/*
static void gen_exception_illegal(DisasContext *ctx)
{
    generate_exception(ctx, RH850_EXCP_ILLEGAL_INST);
}
*/

static inline bool use_goto_tb(DisasContext *ctx, target_ulong dest)
{
    if (unlikely(ctx->singlestep_enabled)) {
        //return false;
    	return true; // set true for testing purposes
    }

#ifndef CONFIG_USER_ONLY
    return (ctx->tb->pc & TARGET_PAGE_MASK) == (dest & TARGET_PAGE_MASK);
#else
    return true;
#endif
}

static void gen_goto_tb(DisasContext *ctx, int n, target_ulong dest)
{
    if (use_goto_tb(ctx, dest)) {
        /* chaining is only allowed when the jump is to the same page */
        tcg_gen_goto_tb(n);
        tcg_gen_movi_tl(cpu_pc, dest);
        tcg_gen_exit_tb((uintptr_t)ctx->tb + n);
    } else {
        tcg_gen_movi_tl(cpu_pc, dest);
        if (ctx->singlestep_enabled) {
            gen_exception_debug();
        } else {
            tcg_gen_exit_tb(0);
        }
    }
}

/* Wrapper for getting reg values - need to check of reg is zero since
 * cpu_gpr[0] is not actually allocated
 */
static inline void gen_get_gpr(TCGv t, int reg_num)
{
    if (reg_num == 0) {
        tcg_gen_movi_tl(t, 0);
    } else {
        tcg_gen_mov_tl(t, cpu_gpr[reg_num]);
    }
}


/* Selection based on group ID needs to be added, once
 * the system register groups are implemented
 */
static inline void gen_get_sysreg(TCGv t, int reg_num)
{
     tcg_gen_mov_tl(t, cpu_sysRegs[reg_num]);
}

/* Wrapper for setting reg values - need to check of reg is zero since
 * cpu_gpr[0] is not actually allocated. this is more for safety purposes,
 * since we usually avoid calling the OP_TYPE_gen function if we see a write to
 * $zero
 */
static inline void gen_set_gpr(int reg_num_dst, TCGv t)
{
    if (reg_num_dst != 0) {
        tcg_gen_mov_tl(cpu_gpr[reg_num_dst], t);
    }
}


/*
 * TODO: Selection based on group ID needs to be added,
 * once the system register groups are implemented
 */
static inline void gen_set_sysreg(int reg_num_dst, TCGv t)
{
	tcg_gen_mov_tl(cpu_sysRegs[reg_num_dst], t);
}

static inline void gen_set_psw(TCGv t)
{
	tcg_gen_mov_tl(cpu_sysRegs[PSW_register], t);
}

static inline void gen_get_psw(TCGv t)
{
	tcg_gen_mov_tl(t, cpu_sysRegs[PSW_register]);
}

static inline void gen_reset_flags(DisasContext *ctx)
{
	tcg_gen_movi_i32(cpu_ZF, 0x0);
	tcg_gen_movi_i32(cpu_SF, 0x0);
	tcg_gen_movi_i32(cpu_OVF, 0x0);
	tcg_gen_movi_i32(cpu_CYF, 0x0);
	tcg_gen_movi_i32(cpu_SATF, 0x0);
}

static TCGv condition_satisfied(int cond)
{
	TCGv condResult = tcg_temp_new_i32();
	tcg_gen_movi_i32(condResult, 0x0);

	switch(cond){
	case V_COND:
		return cpu_OVF;
	case C_COND:
		return cpu_CYF;
	case Z_COND:
		return cpu_ZF;
	case NH_COND:
		tcg_gen_or_i32(condResult, cpu_CYF, cpu_ZF);
		break;
	case S_COND:
		return cpu_SF;
	case T_COND:
		tcg_gen_movi_i32(condResult, 0x1);
		break;
	case LT_COND:
		tcg_gen_xor_i32(condResult, cpu_SF, cpu_OVF);
		break;
	case LE_COND:
		tcg_gen_xor_i32(condResult, cpu_SF, cpu_OVF);
		tcg_gen_or_i32(condResult, condResult, cpu_ZF);
		break;
	case NV_COND:
		tcg_gen_not_i32(condResult, cpu_OVF);
		tcg_gen_andi_i32(condResult, condResult, 0x1);
		break;
	case NC_COND:
		tcg_gen_not_i32(condResult, cpu_CYF);
		tcg_gen_andi_i32(condResult, condResult, 0x1);
		break;
	case NZ_COND:
		tcg_gen_not_i32(condResult, cpu_ZF);
		tcg_gen_andi_i32(condResult, condResult, 0x1);
		break;
	case H_COND:
		tcg_gen_or_i32(condResult, cpu_CYF, cpu_ZF);
		tcg_gen_not_i32(condResult, condResult);
		tcg_gen_andi_i32(condResult, condResult, 0x1);
		break;
	case NS_COND:
		tcg_gen_not_i32(condResult, cpu_SF);
		tcg_gen_andi_i32(condResult, condResult, 0x1);
		break;
	case SA_COND:
		return cpu_SATF;
	case GE_COND:
		tcg_gen_xor_i32(condResult, cpu_SF, cpu_OVF);
		tcg_gen_not_i32(condResult, condResult);
		tcg_gen_andi_i32(condResult, condResult, 0x1);
		break;
	case GT_COND:
		tcg_gen_xor_i32(condResult, cpu_SF, cpu_OVF);
		tcg_gen_or_i32(condResult, condResult, cpu_ZF);
		tcg_gen_not_i32(condResult, condResult);
		tcg_gen_andi_i32(condResult, condResult, 0x1);
		break;
	}
	return condResult;
}

static void gen_add_CC(TCGv_i32 t0, TCGv_i32 t1)
{
	TCGLabel *cont;
	TCGLabel *end;

    TCGv_i32 tmp = tcg_temp_new_i32();
    tcg_gen_movi_i32(tmp, 0);
    tcg_gen_add2_i32(cpu_SF, cpu_CYF, t0, tmp, t1, tmp);
    tcg_gen_mov_i32(cpu_ZF, cpu_SF);

    tcg_gen_xor_i32(cpu_OVF, cpu_SF, t0);
    tcg_gen_xor_i32(tmp, t0, t1);
    tcg_gen_andc_i32(cpu_OVF, cpu_OVF, tmp);

    tcg_gen_shri_i32(cpu_SF, cpu_SF, 0x1f);
    tcg_gen_shri_i32(cpu_OVF, cpu_OVF, 0x1f);
    tcg_temp_free_i32(tmp);

    cont = gen_new_label();
	end = gen_new_label();

	//tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, cpu_ZF, 0x0);
	// uncomment this and test it!

	tcg_gen_brcondi_i32(TCG_COND_NE, cpu_ZF, 0x0, cont);
	tcg_gen_movi_i32(cpu_ZF, 0x1);
	tcg_gen_br(end);

	gen_set_label(cont);
	tcg_gen_movi_i32(cpu_ZF, 0x0);

	gen_set_label(end);
}

static void gen_adf_second_CC(TCGv_i32 t0, TCGv_i32 t1)
{
	TCGLabel *cont;
	TCGLabel *end;
	TCGLabel *keepOVF;
	TCGLabel *keepCYF;

	TCGv OV_flag = tcg_temp_local_new_i32();
	TCGv CY_flag = tcg_temp_local_new_i32();

    TCGv_i32 tmp = tcg_temp_new_i32();
    tcg_gen_movi_i32(tmp, 0);
    tcg_gen_add2_i32(cpu_SF, CY_flag, t0, tmp, t1, tmp);
    tcg_gen_mov_i32(cpu_ZF, cpu_SF);

    tcg_gen_xor_i32(OV_flag, cpu_SF, t0);
    tcg_gen_xor_i32(tmp, t0, t1);
    tcg_gen_andc_i32(OV_flag, OV_flag, tmp);

    tcg_gen_shri_i32(cpu_SF, cpu_SF, 0x1f);
    tcg_gen_shri_i32(OV_flag, OV_flag, 0x1f);
    tcg_temp_free_i32(tmp);

    cont = gen_new_label();
	end = gen_new_label();
	keepOVF = gen_new_label();
	keepCYF = gen_new_label();

	//tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, cpu_ZF, 0x0);
	// uncomment this and test it!

	tcg_gen_brcondi_i32(TCG_COND_NE, cpu_ZF, 0x0, cont);
	tcg_gen_movi_i32(cpu_ZF, 0x1);
	tcg_gen_br(end);

	gen_set_label(cont);
	tcg_gen_movi_i32(cpu_ZF, 0x0);


	gen_set_label(end);

	tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x0, keepOVF);
	//tcg_gen_mov_i32(cpu_OVF, OV_flag);
	tcg_gen_xor_i32(cpu_OVF, cpu_OVF, OV_flag);
	gen_set_label(keepOVF);
	tcg_gen_brcondi_i32(TCG_COND_NE, cpu_CYF, 0x0, keepCYF);
	tcg_gen_mov_i32(cpu_CYF, CY_flag);

	gen_set_label(keepCYF);

}


static void gen_satadd_CC(TCGv_i32 t0, TCGv_i32 t1, TCGv_i32 result)
{
	TCGLabel *cont;
	TCGLabel *end;

    TCGv_i32 tmp = tcg_temp_new_i32();
    tcg_gen_movi_i32(tmp, 0);
    tcg_gen_add2_i32(cpu_SF, cpu_CYF, t0, tmp, t1, tmp);
    tcg_gen_mov_i32(cpu_ZF, cpu_SF);
    tcg_gen_xor_i32(cpu_OVF, cpu_SF, t0);
    tcg_gen_xor_i32(tmp, t0, t1);
    tcg_gen_andc_i32(cpu_OVF, cpu_OVF, tmp);

    tcg_gen_shri_i32(cpu_SF, result, 0x1f);
    tcg_gen_shri_i32(cpu_OVF, cpu_OVF, 0x1f);
    tcg_temp_free_i32(tmp);

    cont = gen_new_label();
	end = gen_new_label();

	tcg_gen_brcondi_i32(TCG_COND_NE, cpu_ZF, 0x0, cont);
	tcg_gen_movi_i32(cpu_ZF, 0x1);
	tcg_gen_br(end);

	gen_set_label(cont);
	tcg_gen_movi_i32(cpu_ZF, 0x0);

	gen_set_label(end);
}

static void gen_sub_CC(TCGv_i32 t0, TCGv_i32 t1)
{
	TCGLabel *cont;
	TCGLabel *end;

    TCGv_i32 tmp;
    tcg_gen_sub_tl(cpu_SF, t0, t1);
    tcg_gen_mov_i32(cpu_ZF, cpu_SF);
    tcg_gen_setcond_i32(TCG_COND_GTU, cpu_CYF, t1, t0);
    tcg_gen_xor_i32(cpu_OVF, cpu_SF, t0);
    tmp = tcg_temp_new_i32();
    tcg_gen_xor_i32(tmp, t0, t1);
    tcg_gen_and_i32(cpu_OVF, cpu_OVF, tmp);

    tcg_gen_shri_i32(cpu_SF, cpu_SF, 0x1f);
	tcg_gen_shri_i32(cpu_OVF, cpu_OVF, 0x1f);
    tcg_temp_free_i32(tmp);

    cont = gen_new_label();
	end = gen_new_label();

	tcg_gen_brcondi_i32(TCG_COND_NE, cpu_ZF, 0x0, cont);
	tcg_gen_movi_i32(cpu_ZF, 0x1);
	tcg_gen_br(end);

	gen_set_label(cont);
	tcg_gen_movi_i32(cpu_ZF, 0x0);

	gen_set_label(end);
}

static void gen_satsub_CC(TCGv_i32 t0, TCGv_i32 t1, TCGv_i32 result)
{
	TCGLabel *cont;
	TCGLabel *end;

    TCGv_i32 tmp;
    tcg_gen_sub_tl(cpu_SF, t0, t1);

    tcg_gen_mov_i32(cpu_ZF, cpu_SF);
    tcg_gen_setcond_i32(TCG_COND_GTU, cpu_CYF, t1, t0);
    tcg_gen_xor_i32(cpu_OVF, cpu_SF, t0);
    tmp = tcg_temp_new_i32();
    tcg_gen_xor_i32(tmp, t0, t1);
    tcg_gen_and_i32(cpu_OVF, cpu_OVF, tmp);

    tcg_gen_shri_i32(cpu_SF, result, 0x1f);
	tcg_gen_shri_i32(cpu_OVF, cpu_OVF, 0x1f);
    tcg_temp_free_i32(tmp);

    cont = gen_new_label();
	end = gen_new_label();

	tcg_gen_brcondi_i32(TCG_COND_NE, cpu_ZF, 0x0, cont);
	tcg_gen_movi_i32(cpu_ZF, 0x1);
	tcg_gen_br(end);

	gen_set_label(cont);
	tcg_gen_movi_i32(cpu_ZF, 0x0);

	gen_set_label(end);
}

static void gen_logic_CC(TCGv_i32 result){

	TCGLabel *cont;
	TCGLabel *end;

	tcg_gen_movi_i32(cpu_OVF, 0x0);
	tcg_gen_shri_i32(cpu_SF, result, 0x1f);

	cont = gen_new_label();
	end = gen_new_label();

	tcg_gen_brcondi_i32(TCG_COND_NE, result, 0x0, cont);
	tcg_gen_movi_i32(cpu_ZF, 0x1);
	tcg_gen_br(end);

	gen_set_label(cont);
	tcg_gen_movi_i32(cpu_ZF, 0x0);

	gen_set_label(end);
}
/*
 static void gen_load(DisasContext *ctx, int memop, int rd, int rs1, target_long imm)
{
    TCGv t0 = tcg_temp_new();
    TCGv t1 = tcg_temp_new();
    TCGv tcg_imm = tcg_temp_new();

    switch(memop){
    	case MO_UB:
    		printf("Load 8 bit \n");
		 	gen_get_gpr(t0, rs1);
		    tcg_gen_movi_i32(tcg_imm, imm);
		    tcg_gen_ext16s_i32(tcg_imm, tcg_imm);
		    tcg_gen_add_tl(t0, t0, tcg_imm);

		    tcg_gen_qemu_ld8u(t1, t0, 0);
		    tcg_gen_ext8s_i32(t1, t1);
		    gen_set_gpr(rd, t1);
		break;
    	case MO_TESW:
    		printf("Load 16 bit \n");
    		gen_get_gpr(t0, rs1);
			tcg_gen_movi_i32(tcg_imm, imm);
			tcg_gen_ext16s_i32(tcg_imm, tcg_imm);
			tcg_gen_add_tl(t0, t0, tcg_imm);

			tcg_gen_qemu_ld16u(t1, t0, 0);
			tcg_gen_ext16s_i32(t1, t1);
			gen_set_gpr(rd, t1);
    	break;
    	case MO_TESL:
    		printf("Load 32 bit \n");
    		gen_get_gpr(t0, rs1);
			tcg_gen_movi_i32(tcg_imm, imm);
			tcg_gen_ext16s_i32(tcg_imm, tcg_imm);
			tcg_gen_add_tl(t0, t0, tcg_imm);

			tcg_gen_qemu_ld32u(t1, t0, 0);
			gen_set_gpr(rd, t1);
    	break;
    }

    tcg_temp_free(t0);
    tcg_temp_free(t1);
}

static void gen_store(DisasContext *ctx, int memop, int rs1, int rs2,
        target_long imm)
{
    TCGv t0 = tcg_temp_new();		//temp
    TCGv dat = tcg_temp_new();		//temp
    TCGv tcg_imm = tcg_temp_new();

    switch(memop){
    	case 4:
    		 printf("Store 8 bit \n");
    		 gen_get_gpr(t0, rs1);
    		 tcg_gen_movi_i32(tcg_imm, imm);
    		 tcg_gen_ext16s_i32(tcg_imm, tcg_imm);

    		 tcg_gen_add_tl(t0, t0, tcg_imm);
    		 gen_get_gpr(dat, rs2);
    		 tcg_gen_qemu_st8(dat, t0, 0);
    	break;
    	case 5:
    		 printf("Store 16 bit \n");
    		 gen_get_gpr(t0, rs1);
			 tcg_gen_movi_i32(tcg_imm, imm);
			 tcg_gen_ext16s_i32(tcg_imm, tcg_imm);

			 tcg_gen_add_tl(t0, t0, tcg_imm);
			 gen_get_gpr(dat, rs2);
			 tcg_gen_qemu_st16(dat, t0, 0);
    	break;
    	case 6:
    		printf("Store 32 bit \n");
    		 gen_get_gpr(t0, rs1);
			 tcg_gen_movi_i32(tcg_imm, imm);
			 tcg_gen_ext16s_i32(tcg_imm, tcg_imm);

			 tcg_gen_add_tl(t0, t0, tcg_imm);
			 gen_get_gpr(dat, rs2);
			 tcg_gen_qemu_st32(dat, t0, 0);
    	break;
    	case MO_64:
    		printf("Store 64 bit \n");
    		 gen_get_gpr(t0, rs1);
			 tcg_gen_movi_i32(tcg_imm, imm);
			 tcg_gen_ext16s_i32(tcg_imm, tcg_imm);

			 tcg_gen_add_tl(t0, t0, tcg_imm);
			 gen_get_gpr(dat, rs2);
			 //TODO:SHOULD BE FIXED: DATA = REG OR (REG + 1)
			 //tcg_gen_qemu_st64(dat, t0, 0);
    	break;
    }


    tcg_temp_free(t0);
    tcg_temp_free(dat);
    tcg_temp_free(tcg_imm);
}
 */


/*
	MO_UB  => 8 unsigned
	MO_SB  => 8 signed
	MO_TEUW => 16 unsigned
	MO_TESW => 16 signed
	MO_TEUL => 32 unsigned
	MO_TESL => 32 signed
	MO_TEQ => 64

*/
static void gen_load(DisasContext *ctx, int memop, int rd, int rs1, target_long imm)
{
    TCGv t0 = tcg_temp_local_new();
    TCGv t1 = tcg_temp_local_new();
    TCGv tcg_imm = tcg_temp_local_new();

    gen_get_gpr(t0, rs1);

    tcg_gen_movi_i32(tcg_imm, imm);
    tcg_gen_ext16s_i32(tcg_imm, tcg_imm);
    tcg_gen_add_tl(t0, t0, tcg_imm);

    ///tcg_gen_addi_tl(t0, t0, imm);

    tcg_gen_qemu_ld_tl(t1, t0, ctx->mem_idx, memop);

    //tcg_gen_qemu_ld_tl(t1, tcg_imm, 0, MO_TESW);
    //tcg_gen_qemu_ld32u(t1, tcg_imm, 0);
    //tcg_gen_ext16s_i32(t1, t1);

    gen_set_gpr(rd, t1);

	printf("gen_load: memop = %d \n",memop);
	printf("source register: r%d, offset: %x \n", rs1, imm);
	printf("target register: r%d \n", rd);

    tcg_temp_free(t0);
    tcg_temp_free(t1);
    tcg_temp_free(tcg_imm);
}

static void gen_store(DisasContext *ctx, int memop, int rs1, int rs2,
        target_long imm)
{
    TCGv t0 = tcg_temp_local_new();
    TCGv dat = tcg_temp_local_new();
    TCGv tcg_imm = tcg_temp_local_new();

    gen_get_gpr(t0, rs1);			//loading rs1 to t0
    tcg_gen_movi_i32(tcg_imm, imm);
    //tcg_gen_ext16s_i32(tcg_imm, tcg_imm);
    //tcg_gen_add_tl(t0, t0, tcg_imm);	//adding displacement to t0

    gen_get_gpr(dat, rs2);			//getting data from rs2
    //gen_set_gpr(11, tcg_imm);
    //gen_set_gpr(12, dat);
    tcg_gen_qemu_st32(dat,tcg_imm,0);
    //tcg_gen_st32_tl(dat, ctx->mem_idx, 0);

    printf("gen_store: memop = %d \n", memop);
    printf("source register: r%d \n", rs2);
    printf("address: r%d + %x \n", rs1, imm);

    tcg_temp_free(t0);
    tcg_temp_free(dat);
    tcg_temp_free(tcg_imm);
}
/*
static void decode_load_store_0(CPURH850State *env, DisasContext *ctx)
{
	printf("this oneasd");
	int rs1;
	int rs3;
	target_long disp;
	uint32_t op;

	op = MASK_OP_ST_LD0(ctx->opcode);
	rs1 = GET_RS1(ctx->opcode);
	rs3 = GET_RS3(ctx->opcode);
	disp = GET_DISP(ctx->opcode);

	//TODO:There is someting wrong with a displacement
	//GET_DISP is on wrong bits, in function gen_load, gen_load imm is manualy fixed

	switch(op) {
		case OPC_RH850_LDB2:
			gen_load(ctx, MO_SB, rs1, rs3, disp);			// LD.B (Format XIV)
			break;

		case OPC_RH850_LDH2:

	    	if ( extract32(ctx->opcode, 16, 1) == 0 )	// LD.H (Format XIV)
				gen_load(ctx, MO_TESW, rs1, rs3, disp);
			break;

		case OPC_RH850_LDW2:

	    	if ( extract32(ctx->opcode, 16, 1) == 0 )	// LD.W (Format XIV)
				gen_load(ctx, MO_TESL, rs1, rs3, disp);
			break;

		case OPC_RH850_STB2:
			gen_store(ctx, MO_8, rs1, rs3, disp);   // get lower 8 bits of reg3
			break;

		case OPC_RH850_STW2:
			gen_store(ctx, MO_32, rs1, rs3, disp);  // get 32bits of reg3
			break;
	}
}

static void decode_load_store_1(CPURH850State *env, DisasContext *ctx)
{
	printf("this one asdasd");
	int rs1;
	int rs3;
	target_long disp;
	uint32_t op;
	rs1 = GET_RS1(ctx->opcode);
	rs3 = GET_RS3(ctx->opcode);
	disp = GET_DISP(ctx->opcode);


	op = MASK_OP_ST_LD1(ctx->opcode);

	switch(op) {
		case OPC_RH850_LDBU2:
			gen_load(ctx, MO_UB, rs1, rs3, disp);			// LD.BU (Format XIV)
			break;

		case OPC_RH850_LDHU2:
	    	if ( extract32(ctx->opcode, 16, 1) == 0 )	// LD.HU (Format XIV)
				gen_load(ctx, MO_TESW, rs1, rs3, disp);
			break;

		case OPC_RH850_LDDW:

	    	if ( extract32(ctx->opcode, 16, 1) == 0 )	// LD.W (Format XIV)
				gen_load(ctx, MO_64, rs1, rs3, disp);
			break;

		case OPC_RH850_STDW:
			gen_store(ctx, MO_64, rs1, rs3, disp);  //get lower 32bits + higher 32bits+1 of reg3
			break;

		case OPC_RH850_STH2:
			gen_store(ctx, MO_16, rs1, rs3, disp);  //get lower 16bits of reg3
			break;
	}

}
*/

static void gen_multiply(DisasContext *ctx, int rs1, int rs2, int operation)	// completed
{
	TCGv r1 = tcg_temp_new();		//temp
	TCGv r2 = tcg_temp_new();		//temp
	//TCGv comp = tcg_temp_new();
	gen_get_gpr(r1, rs1);			//loading rs1 to t0
	gen_get_gpr(r2, rs2);			//loading rs2 to t1
	int imm = rs1;
	int imm_32;
	int int_rs3;

	TCGv tcg_imm = tcg_temp_new();
	TCGv tcg_imm32 = tcg_temp_new();
	TCGv tcg_r3 = tcg_temp_new();
	TCGv tcg_temp = tcg_temp_new();

	switch(operation){
		case OPC_RH850_MUL_reg1_reg2_reg3:
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);

			tcg_gen_muls2_i32(r2, tcg_r3, r1, r2);
			if(rs2!=int_rs3){
				gen_set_gpr(rs2, r2);
			}
			gen_set_gpr(int_rs3,tcg_r3);
			break;

		case OPC_RH850_MUL_imm9_reg2_reg3:
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);

			imm_32 = extract32(ctx->opcode, 18, 4);
			imm_32 = imm | (imm_32 << 5);

			// sign extension
			if((imm_32 & 0x100) == 0x100){
				imm_32 = imm_32 | (0x7f << 9);
			}
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_ext16s_tl(tcg_imm32, tcg_imm32);

			tcg_gen_muls2_i32(r2, tcg_r3, tcg_imm32, r2);

			if(rs2!=int_rs3){
				gen_set_gpr(rs2, r2);
			}
			gen_set_gpr(int_rs3, tcg_r3);
			break;

		case OPC_RH850_MULH_reg1_reg2:

			tcg_gen_andi_tl(r1, r1,0x0000FFFF);
			tcg_gen_andi_tl(r2, r2,0x0000FFFF);
			tcg_gen_ext16s_i32(r1, r1);
			tcg_gen_ext16s_i32(r2, r2);

			tcg_gen_mul_tl(r2, r2, r1);
			gen_set_gpr(rs2, r2);
			break;

		case OPC_RH850_MULH_imm5_reg2:

			if ((imm & 0x10) == 0x10){
				imm = imm | (0x7 << 5);
			}
			tcg_gen_andi_tl(r2, r2,0x0000FFFF);
			tcg_gen_ext16s_i32(r2, r2);

			tcg_gen_movi_tl(tcg_imm, imm);
			tcg_gen_ext8s_i32(tcg_imm, tcg_imm);
			tcg_gen_mul_tl(r2, r2, tcg_imm);
			gen_set_gpr(rs2, r2);
			break;

		case OPC_RH850_MULHI_imm16_reg1_reg2:

			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_ext16s_i32(tcg_imm32, tcg_imm32);

			tcg_gen_andi_tl(r1, r1, 0x0000FFFF);
			tcg_gen_ext16s_i32(r1, r1);

			tcg_gen_mul_tl(r2, r1, tcg_imm32);

			gen_set_gpr(rs2, r2);
			break;

		case OPC_RH850_MULU_reg1_reg2_reg3:

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);

			tcg_gen_mulu2_i32(r2, tcg_r3, r2, r1);

			if(rs2!=int_rs3){
				gen_set_gpr(rs2, r2);
			}
			gen_set_gpr(int_rs3,tcg_r3);
			break;

		case OPC_RH850_MULU_imm9_reg2_reg3:

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);

			imm_32 = extract32(ctx->opcode, 18, 4);
			imm_32 = imm | (imm_32 << 5);
			tcg_gen_movi_tl(tcg_imm32, imm_32);

			tcg_gen_ext16u_tl(tcg_imm32, tcg_imm32);

			tcg_gen_mulu2_i32(r2, tcg_r3, tcg_imm32, r2);

			if(rs2!=int_rs3){
				gen_set_gpr(rs2, r2);
			}
			gen_set_gpr(int_rs3,tcg_r3);
			break;
	}

	tcg_temp_free(r1);
	tcg_temp_free(r2);
	tcg_temp_free(tcg_r3);
	tcg_temp_free(tcg_temp);
	tcg_temp_free(tcg_imm);
	tcg_temp_free(tcg_imm32);

}

static void gen_mul_accumulate(DisasContext *ctx, int rs1, int rs2, int operation)	// completed
{
	TCGv r1 = tcg_temp_new();
	TCGv r2 = tcg_temp_new();
	TCGv addLo = tcg_temp_new();
	TCGv addHi = tcg_temp_new();
	TCGv resLo = tcg_temp_new();
	TCGv resHi = tcg_temp_new();
	TCGv destLo = tcg_temp_new();
	TCGv destHi = tcg_temp_new();

	gen_get_gpr(r1, rs1);
	gen_get_gpr(r2, rs2);

	int rs3;
	int rs4;

	rs3 = extract32(ctx->opcode, 28, 4) << 1;
	rs4 = extract32(ctx->opcode, 17, 4) << 1;

	gen_get_gpr(addLo, rs3);
	gen_get_gpr(addHi, rs3+1);

	switch(operation){
		case OPC_RH850_MAC_reg1_reg2_reg3_reg4:

			tcg_gen_muls2_i32(resLo, resHi, r1, r2);
			tcg_gen_add2_i32(destLo, destHi, resLo, resHi, addLo, addHi);

			gen_set_gpr(rs4, destLo);
			gen_set_gpr(rs4+1, destHi);
			break;

		case OPC_RH850_MACU_reg1_reg2_reg3_reg4:
			tcg_gen_mulu2_i32(resLo, resHi, r1, r2);
			tcg_gen_add2_i32(destLo, destHi, resLo, resHi, addLo, addHi);

			gen_set_gpr(rs4, destLo);
			gen_set_gpr((rs4+1), destHi);
			break;
	}

}

static void gen_arithmetic(DisasContext *ctx, int rs1, int rs2, int operation)	// completed
{
	TCGv r1 = tcg_temp_new();
	TCGv r2 = tcg_temp_new();
	gen_get_gpr(r1, rs1);
	gen_get_gpr(r2, rs2);

	int imm = rs1;
	int imm_32;
	uint64_t opcode48;

	TCGv tcg_imm = tcg_temp_new();
	TCGv tcg_imm32 = tcg_temp_new();
	TCGv tcg_r3 = tcg_temp_new();
	TCGv tcg_result = tcg_temp_new();

	switch(operation) {

		case OPC_RH850_ADD_reg1_reg2: {

			tcg_gen_add_tl(tcg_result, r2, r1);
			gen_set_gpr(rs2, tcg_result);

			gen_add_CC(r1, r2);

		}	break;

		case OPC_RH850_ADD_imm5_reg2:
			if((imm & 0x10) == 0x10){
				imm = imm | (0x7 << 5);
			}
			tcg_gen_movi_i32(tcg_imm, imm);
			tcg_gen_ext8s_i32(tcg_imm, tcg_imm);
			tcg_gen_add_tl(tcg_result, r2, tcg_imm);
			gen_set_gpr(rs2, tcg_result);

			gen_add_CC(r2, tcg_imm);

			break;

		case OPC_RH850_ADDI_imm16_reg1_reg2:
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_ext16s_tl(tcg_imm32, tcg_imm32);
			tcg_gen_add_tl(r2,r1, tcg_imm32);
			gen_set_gpr(rs2, r2);

			gen_add_CC(r1, tcg_imm32);

			break;

		case OPC_RH850_CMP_reg1_reg2:	{
			gen_sub_CC(r2, r1);
		}	break;

		case OPC_RH850_CMP_imm5_reg2:	{

			TCGv tcg_imm = tcg_temp_new();

			if ((imm & 0x10) == 0x10){
				imm = imm | (0x7 << 5);
			}
			tcg_gen_movi_tl(tcg_imm, imm);
			tcg_gen_ext8s_i32(tcg_imm, tcg_imm);

			gen_sub_CC(r2, tcg_imm);

		}	break;

		case OPC_RH850_MOV_reg1_reg2:
			tcg_gen_mov_tl(r2, r1);
			gen_set_gpr(rs2, r2);
			break;

		case OPC_RH850_MOV_imm5_reg2:
			if ((imm & 0x10) == 0x10){
				imm = imm | (0x7 << 5);
			}
			tcg_gen_movi_tl(r2, imm);
			tcg_gen_ext8s_i32(r2, r2);

			gen_set_gpr(rs2, r2);
			break;

		case OPC_RH850_MOV_imm32_reg1:	// 48bit instruction
			opcode48 = (ctx->opcode1);
			opcode48 = (ctx->opcode) | (opcode48  << 0x20);
			imm_32 = extract64(opcode48, 16, 32) & 0xffffffff;
			tcg_gen_movi_i32(r2, imm_32);
			gen_set_gpr(rs2, r2);
			break;

		case OPC_RH850_MOVEA_imm16_reg1_reg2:
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_i32(tcg_imm, imm_32);
			tcg_gen_ext16s_i32(tcg_imm, tcg_imm);

			tcg_gen_add_i32(r2, tcg_imm, r1);
			gen_set_gpr(rs2, r2);
			break;

		case OPC_RH850_MOVHI_imm16_reg1_reg2:
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_i32(tcg_imm, imm_32);
			tcg_gen_shli_i32(tcg_imm, tcg_imm, 0x10);

			tcg_gen_add_i32(r2, tcg_imm, r1);
			gen_set_gpr(rs2, r2);
			break;

		case OPC_RH850_SUB_reg1_reg2:

			tcg_gen_sub_tl(tcg_result, r2, r1);
			gen_set_gpr(rs2, tcg_result);
			gen_sub_CC(r2, r1);
			break;

		case OPC_RH850_SUBR_reg1_reg2:
			tcg_gen_sub_tl(tcg_result, r1, r2);
			gen_set_gpr(rs2, tcg_result);
			gen_sub_CC(r1, r2);
			break;
	}

	tcg_temp_free(r1);
	tcg_temp_free(r2);
	tcg_temp_free(tcg_r3);
	tcg_temp_free(tcg_result);
}

static void gen_cond_arith(DisasContext *ctx, int rs1, int rs2, int operation)	// completed
{
	TCGv r1 = tcg_temp_new();
	TCGv r2 = tcg_temp_new();

	TCGLabel *cont;
	TCGLabel *addToR2;
	TCGLabel *dontSecondPassCC;

	gen_get_gpr(r1, rs1);
	gen_get_gpr(r2, rs2);

	int int_rs3;
	int int_cond;

	TCGv condResult = tcg_temp_new();

	switch(operation){

		case OPC_RH850_ADF_cccc_reg1_reg2_reg3:{

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();
			TCGv addIfCond = tcg_temp_local_new_i32();

			int_rs3 = extract32(ctx->opcode, 27, 5);
			int_cond = extract32(ctx->opcode, 17, 4);
			if(int_cond == 0xd){
				//throw exception/warning for inappropriate condition (SA)
				break;
			}

			tcg_gen_mov_i32(r1_local, r1);
			tcg_gen_mov_i32(r2_local, r2);
			gen_get_gpr(r3_local,int_rs3);
			tcg_gen_movi_i32(addIfCond, 0x1);

			condResult = condition_satisfied(int_cond);
			cont = gen_new_label();
			dontSecondPassCC = gen_new_label();

			tcg_gen_brcondi_i32(TCG_COND_NE, condResult, 0x1, cont);
			gen_add_CC(r1_local, r2_local);
			tcg_gen_add_tl(r3_local, r2_local, r1_local);
			gen_adf_second_CC(r3_local, addIfCond);
			tcg_gen_add_tl(r3_local, r3_local, addIfCond);
			tcg_gen_br(dontSecondPassCC);

			gen_set_label(cont);
			tcg_gen_add_tl(r3_local, r2_local, r1_local);
			gen_add_CC(r1_local, r2_local);

			gen_set_label(dontSecondPassCC);
			gen_set_gpr(int_rs3, r3_local);

			tcg_temp_free_i32(r1_local);
			tcg_temp_free_i32(r2_local);
			tcg_temp_free_i32(r3_local);
		}
			break;

		case OPC_RH850_SBF_cccc_reg1_reg2_reg3:{

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();

			int_rs3 = extract32(ctx->opcode, 27, 5);
			int_cond = extract32(ctx->opcode, 17, 4);
			if(int_cond == 0xd){
				//throw exception/warning for inappropriate condition (SA)
				break;
			}

			tcg_gen_mov_i32(r1_local, r1);
			tcg_gen_mov_i32(r2_local, r2);
			gen_get_gpr(r3_local,int_rs3);

			condResult = condition_satisfied(int_cond);
			cont = gen_new_label();
			addToR2 = gen_new_label();

			tcg_gen_brcondi_i32(TCG_COND_NE, condResult, 0x1, cont);
			tcg_gen_brcondi_i32(TCG_COND_EQ, r1_local, 0x7fffffff, addToR2);
			tcg_gen_addi_tl(r1_local, r1_local, 0x1);
			tcg_gen_br(cont);

			gen_set_label(addToR2);
			tcg_gen_subi_i32(r2_local, r2_local, 0x1);

			gen_set_label(cont);
			tcg_gen_sub_tl(r3_local, r2_local, r1_local);
			gen_sub_CC(r2_local, r1_local);
			gen_set_gpr(int_rs3, r3_local);

			tcg_temp_free_i32(r1_local);
			tcg_temp_free_i32(r2_local);
			tcg_temp_free_i32(r3_local);
		}
			break;
	}

	tcg_temp_free_i32(r1);
	tcg_temp_free_i32(r2);
}

static void gen_sat_op(DisasContext *ctx, int rs1, int rs2, int operation)		// completed
{
	TCGv r1 = tcg_temp_new();
	TCGv r2 = tcg_temp_new();
	gen_get_gpr(r1, rs1);
	gen_get_gpr(r2, rs2);

	int imm = rs1;
	int int_rs3;

	TCGLabel *end;
	TCGLabel *cont;
	TCGLabel *cont2;
	TCGLabel *setMax;
	TCGLabel *dontChange;

	switch(operation){

		case OPC_RH850_SATADD_reg1_reg2: {

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv min = tcg_temp_local_new();
			TCGv max = tcg_temp_local_new();
			TCGv zero = tcg_temp_local_new();
			tcg_gen_movi_i32(min, 0x80000000);
			tcg_gen_movi_i32(max, 0x7fffffff);
			tcg_gen_mov_i32(r1_local, r1);
			tcg_gen_mov_i32(r2_local, r2);
			tcg_gen_movi_i32(zero, 0x0);
			end = gen_new_label();
			cont = gen_new_label();
			cont2 = gen_new_label();


			tcg_gen_add_i32(result, r1_local, r2_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, r2_local, check, end);
			tcg_gen_mov_i32(result, max);
			tcg_gen_movi_i32(cpu_SATF, 0x1);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);
			tcg_gen_movi_i32(cpu_SATF, 0x1);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);

			gen_satadd_CC(r1_local, r2_local, result);

			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;

		case OPC_RH850_SATADD_imm5_reg2: {

			TCGv imm_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv min = tcg_temp_local_new();
			TCGv max = tcg_temp_local_new();
			TCGv zero = tcg_temp_local_new();
			tcg_gen_movi_i32(min, 0x80000000);
			tcg_gen_movi_i32(max, 0x7fffffff);
			tcg_gen_mov_i32(r2_local, r2);
			tcg_gen_movi_i32(zero, 0x0);
			end = gen_new_label();
			cont = gen_new_label();
			cont2 = gen_new_label();

			if ((imm & 0x10) == 0x10){
				imm = imm | (0x7 << 5);
			}

			tcg_gen_movi_tl(imm_local, imm);
			tcg_gen_ext8s_tl(imm_local, imm_local);

			tcg_gen_add_i32(result, imm_local, r2_local);

			tcg_gen_brcond_tl(TCG_COND_LT, imm_local, zero, cont);

			tcg_gen_sub_i32(check, max, imm_local);
			tcg_gen_brcond_tl(TCG_COND_LE, r2_local, check, end);
			tcg_gen_mov_i32(result, max);
			tcg_gen_movi_i32(cpu_SATF, 0x1);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, imm_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);
			tcg_gen_movi_i32(cpu_SATF, 0x1);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);

			gen_satadd_CC(r2_local, imm_local, result);

			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(imm_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;

		case OPC_RH850_SATADD_reg1_reg2_reg3: {

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv min = tcg_temp_local_new();
			TCGv max = tcg_temp_local_new();
			TCGv zero = tcg_temp_local_new();
			tcg_gen_movi_i32(min, 0x80000000);
			tcg_gen_movi_i32(max, 0x7fffffff);
			tcg_gen_mov_i32(r1_local, r1);
			tcg_gen_mov_i32(r2_local, r2);
			tcg_gen_movi_i32(zero, 0x0);
			end = gen_new_label();
			cont = gen_new_label();
			cont2 = gen_new_label();

			int_rs3 = extract32(ctx->opcode, 27, 5);
			tcg_gen_add_i32(result, r1_local, r2_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);		//if (r1 > 0)

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, r2_local, check, end);			//if (r2 > MAX-r1)
			tcg_gen_mov_i32(result, max);										//return MAX;
			tcg_gen_movi_i32(cpu_SATF, 0x1);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont); 										//else
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);		//if (r2 < MIN-r1)
			tcg_gen_mov_i32(result, min);										//return MIN;
			tcg_gen_movi_i32(cpu_SATF, 0x1);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(int_rs3, result);

			gen_satadd_CC(r1_local, r2_local, result);

			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;

		case OPC_RH850_SATSUB_reg1_reg2: {

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv min = tcg_temp_local_new();
			TCGv max = tcg_temp_local_new();
			TCGv zero = tcg_temp_local_new();
			tcg_gen_movi_i32(min, 0x80000000);
			tcg_gen_movi_i32(max, 0x7fffffff);
			tcg_gen_mov_i32(r1_local, r1);
			tcg_gen_mov_i32(r2_local, r2);
			tcg_gen_movi_i32(zero, 0x0);
			end = gen_new_label();
			cont = gen_new_label();
			cont2 = gen_new_label();
			setMax = gen_new_label();
			dontChange = gen_new_label();

			/*
			 * Negating second operand and using satadd code. When negating an operand
			 * with value 0x80000000, the result overflows positive numbers and is not
			 * negated. If this happens, the operand is first incremented, and then negated.
			 * The second operand is as well incremented, if it's value is less than 0x7fffffff.
			 * Otherwise, the result is set to MAX and SATF is set.
			 * This was done in all following saturated subtraction functions.
			 */

			tcg_gen_brcondi_i32(TCG_COND_NE, r1_local, 0x80000000, dontChange);
			tcg_gen_brcondi_i32(TCG_COND_EQ, r2_local, 0x7fffffff, setMax);

			tcg_gen_addi_i32(r1_local, r1_local, 0x1);
			tcg_gen_addi_i32(r2_local, r2_local, 0x1);
			gen_set_label(dontChange);

			tcg_gen_neg_i32(r1_local, r1_local);
			tcg_gen_add_i32(result, r1_local, r2_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, r2_local, check, end);
			gen_set_label(setMax);
			tcg_gen_mov_i32(result, max);
			tcg_gen_movi_i32(cpu_SATF, 0x1);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);
			tcg_gen_movi_i32(cpu_SATF, 0x1);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);

			gen_satsub_CC(r2_local, r1_local, result);

			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;

		case OPC_RH850_SATSUB_reg1_reg2_reg3: {
			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv min = tcg_temp_local_new();
			TCGv max = tcg_temp_local_new();
			TCGv zero = tcg_temp_local_new();
			tcg_gen_movi_i32(min, 0x80000000);
			tcg_gen_movi_i32(max, 0x7fffffff);
			tcg_gen_mov_i32(r1_local, r1);
			tcg_gen_mov_i32(r2_local, r2);
			tcg_gen_movi_i32(zero, 0x0);
			end = gen_new_label();
			cont = gen_new_label();
			cont2 = gen_new_label();
			setMax = gen_new_label();
			dontChange = gen_new_label();
			int_rs3 = extract32(ctx->opcode, 27, 5);

			tcg_gen_brcondi_i32(TCG_COND_NE, r1_local, 0x80000000, dontChange);
			tcg_gen_brcondi_i32(TCG_COND_EQ, r2_local, 0x7fffffff, setMax);

			tcg_gen_addi_i32(r1_local, r1_local, 0x1);
			tcg_gen_addi_i32(r2_local, r2_local, 0x1);
			gen_set_label(dontChange);

			tcg_gen_neg_i32(r1_local, r1_local);
			tcg_gen_add_i32(result, r1_local, r2_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, r2_local, check, end);
			gen_set_label(setMax);
			tcg_gen_mov_i32(result, max);
			tcg_gen_movi_i32(cpu_SATF, 0x1);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);
			tcg_gen_movi_i32(cpu_SATF, 0x1);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(int_rs3, result);

			tcg_gen_neg_i32(r1_local, r1_local);
			gen_satsub_CC(r2_local, r1_local, result);

			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;

		case OPC_RH850_SATSUBI_imm16_reg1_reg2: {
			TCGv r1_local = tcg_temp_local_new();
			TCGv imm_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv min = tcg_temp_local_new();
			TCGv max = tcg_temp_local_new();
			TCGv zero = tcg_temp_local_new();
			tcg_gen_movi_i32(min, 0x80000000);
			tcg_gen_movi_i32(max, 0x7fffffff);
			tcg_gen_mov_i32(r1_local, r1);
			imm = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_i32(imm_local, imm);
			tcg_gen_ext16s_i32(imm_local, imm_local);
			tcg_gen_movi_i32(zero, 0x0);
			end = gen_new_label();
			cont = gen_new_label();
			cont2 = gen_new_label();
			setMax = gen_new_label();
			dontChange = gen_new_label();

			tcg_gen_brcondi_i32(TCG_COND_NE, r1_local, 0x80000000, dontChange);
			tcg_gen_brcondi_i32(TCG_COND_EQ, imm_local, 0x7fffffff, setMax);

			tcg_gen_addi_i32(r1_local, r1_local, 0x1);
			tcg_gen_addi_i32(imm_local, imm_local, 0x1);
			gen_set_label(dontChange);


			tcg_gen_neg_i32(imm_local, imm_local);

			tcg_gen_add_i32(result, r1_local, imm_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, imm_local, check, end);
			gen_set_label(setMax);
			tcg_gen_mov_i32(result, max);
			tcg_gen_movi_i32(cpu_SATF, 0x1);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, imm_local, check, cont2);
			tcg_gen_mov_i32(result, min);
			tcg_gen_movi_i32(cpu_SATF, 0x1);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);

			gen_satsub_CC(r1_local, imm_local, result);

			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(imm_local);
			tcg_temp_free(zero);

		}	break;

		case OPC_RH850_SATSUBR_reg1_reg2: {

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv min = tcg_temp_local_new();
			TCGv max = tcg_temp_local_new();
			TCGv zero = tcg_temp_local_new();
			tcg_gen_movi_i32(min, 0x80000000);
			tcg_gen_movi_i32(max, 0x7fffffff);
			tcg_gen_mov_i32(r1_local, r2);
			tcg_gen_mov_i32(r2_local, r1);
			tcg_gen_movi_i32(zero, 0x0);
			end = gen_new_label();
			cont = gen_new_label();
			cont2 = gen_new_label();
			setMax = gen_new_label();
			dontChange = gen_new_label();

			/*
			 * Negating second operand and using satadd code. When negating an operand
			 * with value 0x80000000, the result overflows positive numbers and is not
			 * negated. If this happens, the operand is first incremented, and then negated.
			 * The second operand is as well incremented, if it's value is less than 0x7fffffff.
			 * Otherwise, the result is set to MAX and SATF is set.
			 * This was done in all following saturated subtraction functions.
			 */

			tcg_gen_brcondi_i32(TCG_COND_NE, r1_local, 0x80000000, dontChange);
			tcg_gen_brcondi_i32(TCG_COND_EQ, r2_local, 0x7fffffff, setMax);

			tcg_gen_addi_i32(r1_local, r1_local, 0x1);
			tcg_gen_addi_i32(r2_local, r2_local, 0x1);
			gen_set_label(dontChange);

			tcg_gen_neg_i32(r1_local, r1_local);
			tcg_gen_add_i32(result, r1_local, r2_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, r2_local, check, end);
			gen_set_label(setMax);
			tcg_gen_mov_i32(result, max);
			tcg_gen_movi_i32(cpu_SATF, 0x1);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);
			tcg_gen_movi_i32(cpu_SATF, 0x1);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);

			gen_satsub_CC(r2_local, r1_local, result);

			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;
	}

	tcg_temp_free(r1);
	tcg_temp_free(r2);
}

static void gen_logical(DisasContext *ctx, int rs1, int rs2, int operation)		// completed
{
	TCGv r1 = tcg_temp_new();
	TCGv r2 = tcg_temp_new();
	TCGv result = tcg_temp_new();
	gen_get_gpr(r1, rs1);
	gen_get_gpr(r2, rs2);

	int imm_32;
	TCGv tcg_imm = tcg_temp_new();

	switch(operation){

		case OPC_RH850_AND_reg1_reg2:
			tcg_gen_and_tl(r2, r2, r1);
			gen_set_gpr(rs2, r2);
			gen_logic_CC(r2);
			break;

		case OPC_RH850_ANDI_imm16_reg1_reg2:
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm, imm_32);
			tcg_gen_ext16u_i32(tcg_imm, tcg_imm);
			tcg_gen_and_i32(r2, r1, tcg_imm);
			gen_set_gpr(rs2, r2);
			gen_logic_CC(r2);
			break;

		case OPC_RH850_NOT_reg1_reg2:
			tcg_gen_not_i32(r2, r1);
			gen_set_gpr(rs2, r2);
			gen_logic_CC(r2);
			break;

		case OPC_RH850_OR_reg1_reg2:
			tcg_gen_or_tl(r2, r2, r1);
			gen_set_gpr(rs2, r2);
			gen_logic_CC(r2);
			break;

		case OPC_RH850_ORI_imm16_reg1_reg2:
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_i32(tcg_imm, imm_32);
			tcg_gen_ext16u_i32(tcg_imm,tcg_imm);

			tcg_gen_or_i32(r2, r1, tcg_imm);
			gen_set_gpr(rs2, r2);
			gen_logic_CC(r2);
			break;

		case OPC_RH850_TST_reg1_reg2:
			tcg_gen_and_i32(result, r1, r2);
			gen_logic_CC(result);
			break;

		case OPC_RH850_XOR_reg1_reg2:
			tcg_gen_xor_i32(result, r2, r1);
			gen_set_gpr(rs2, result);
			gen_logic_CC(result);
			break;

		case OPC_RH850_XORI_imm16_reg1_reg2:
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_i32(tcg_imm, imm_32);
			tcg_gen_ext16u_i32(tcg_imm,tcg_imm);

			tcg_gen_xor_i32(result, r1, tcg_imm);
			gen_set_gpr(rs2, result);
			gen_logic_CC(result);
			break;
	}

	tcg_temp_free(r1);
	tcg_temp_free(r2);
	tcg_temp_free(tcg_imm);
	tcg_temp_free(result);
}

static void gen_data_manipulation(DisasContext *ctx, int rs1, int rs2, int operation)	// completed
//TODO: check BINS
{
	TCGv tcg_r1 = tcg_temp_new();
	TCGv tcg_r2 = tcg_temp_new();
	TCGv tcg_r3 = tcg_temp_new();
	TCGv tcg_imm = tcg_temp_new();
	TCGv tcg_temp = tcg_temp_new();
	TCGv tcg_temp2 = tcg_temp_new();
	TCGv condResult = tcg_temp_new();
	TCGv insert = tcg_temp_new();

	TCGLabel *cont;
	TCGLabel *end;
	TCGLabel *set;

	int int_imm = rs1;
	int int_rs3;
	int int_cond;
	int pos;
	int width;
	int mask;

	gen_get_gpr(tcg_r1, rs1);
	gen_get_gpr(tcg_r2, rs2);

	switch(operation) {

		case 123456: //BINS

			mask = 0;
			pos = extract32(ctx->opcode, 17, 3) | (extract32(ctx->opcode, 27, 1) << 3);

			width = extract32(ctx->opcode, 28, 4) - pos + 1;

			for(int i = 0; i < width; i++){
				mask = mask | (0x1 << i);
			}

			tcg_gen_andi_i32(insert, tcg_r1, mask);		//insert has the bits from reg1

			tcg_gen_movi_i32(tcg_temp, mask);
			tcg_gen_shli_i32(tcg_temp, tcg_temp, pos);	//inverting and shifting the mask
			tcg_gen_not_i32(tcg_temp, tcg_temp);		//for deletion of bits in reg2

			tcg_gen_and_i32(tcg_r2, tcg_r2, tcg_temp);	//deleting bits that will be replaced
			tcg_gen_shli_i32(insert, insert, pos);		//shifting bits to right position
			tcg_gen_or_i32(tcg_r2, tcg_r2, insert);		//placing bits into reg2

			gen_set_gpr(rs2, tcg_r2);

			tcg_gen_setcond_i32(TCG_COND_EQ, cpu_ZF, tcg_r2, 0x0);
			tcg_gen_shli_i32(cpu_SF, tcg_r2, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);


			break;

		case OPC_RH850_BSH_reg2_reg3: {

			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();
			TCGv count_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();


			int_rs3 = extract32(ctx->opcode, 27, 5);
			//gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_mov_tl(tcg_temp2, tcg_r2);
			tcg_gen_movi_i32(tcg_r3, 0x0);

			tcg_gen_andi_tl(tcg_temp, tcg_temp2, 0xff000000);
			tcg_gen_shri_tl(tcg_temp, tcg_temp, 0x8);
			tcg_gen_or_tl(tcg_r3, tcg_r3, tcg_temp);

			tcg_gen_andi_tl(tcg_temp, tcg_temp2, 0x00ff0000);
			tcg_gen_shli_tl(tcg_temp, tcg_temp, 0x8);
			tcg_gen_or_tl(tcg_r3, tcg_r3, tcg_temp);

			tcg_gen_andi_tl(tcg_temp, tcg_temp2, 0x0000ff00);
			tcg_gen_shri_tl(tcg_temp, tcg_temp, 0x8);
			tcg_gen_or_tl(tcg_r3, tcg_r3, tcg_temp);

			tcg_gen_andi_tl(tcg_temp, tcg_temp2, 0x000000ff);
			tcg_gen_shli_tl(tcg_temp, tcg_temp, 0x8);
			tcg_gen_or_tl(tcg_r3, tcg_r3, tcg_temp);

			gen_set_gpr(int_rs3, tcg_r3);

			tcg_gen_mov_i32(r2_local, tcg_r2);
			tcg_gen_mov_i32(r3_local, tcg_r3);

			cont = gen_new_label();
			end = gen_new_label();
			set = gen_new_label();

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r3_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r3_local, 0x1f);

			tcg_gen_movi_i32(count_local, 0x0);

			gen_set_label(cont);////

			tcg_gen_andi_i32(temp_local, r3_local, 0x000000ff);
			tcg_gen_brcondi_i32(TCG_COND_EQ, temp_local, 0x0, set);////
			tcg_gen_addi_i32(count_local, count_local, 0x1);
			tcg_gen_shri_i32(r3_local, r3_local, 0x1);
			tcg_gen_brcondi_i32(TCG_COND_NE, count_local, 0x9, cont);////
			tcg_gen_movi_i32(cpu_CYF, 0x0);
			tcg_gen_br(end);

			gen_set_label(set);////
			tcg_gen_movi_i32(cpu_CYF, 0x1);

			gen_set_label(end);////
			tcg_gen_movi_i32(cpu_OVF, 0x0);

		}	break;

		case OPC_RH850_BSW_reg2_reg3: {

			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();
			TCGv count_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();

			cont = gen_new_label();
			end = gen_new_label();
			set = gen_new_label();

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_bswap32_i32(tcg_r3, tcg_r2);
			gen_set_gpr(int_rs3, tcg_r3);

			tcg_gen_mov_i32(r2_local, tcg_r2);
			tcg_gen_mov_i32(r3_local, tcg_r3);

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r3_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r3_local, 0x1f);

			tcg_gen_movi_i32(count_local, 0x0);

			gen_set_label(cont);////

			tcg_gen_andi_i32(temp_local, r3_local, 0x000000ff);
			tcg_gen_brcondi_i32(TCG_COND_EQ, temp_local, 0x0, set);////
			tcg_gen_addi_i32(count_local, count_local, 0x1);
			tcg_gen_shri_i32(r3_local, r3_local, 0x1);
			tcg_gen_brcondi_i32(TCG_COND_NE, count_local, 0x1a, cont);////
			tcg_gen_movi_i32(cpu_CYF, 0x0);
			tcg_gen_br(end);

			gen_set_label(set);////
			tcg_gen_movi_i32(cpu_CYF, 0x1);

			gen_set_label(end);////
			tcg_gen_movi_i32(cpu_OVF, 0x0);

		}
			break;

		case OPC_RH850_CMOV_cccc_reg1_reg2_reg3: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();

			int_rs3 = extract32(ctx->opcode, 27, 5);

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_mov_i32(r2_local, tcg_r2);
			gen_get_gpr(r3_local,int_rs3);

			int_cond = extract32(ctx->opcode, 17, 4);
			condResult = condition_satisfied(int_cond);
			cont = gen_new_label();

			tcg_gen_mov_tl(r3_local, r2_local);
			tcg_gen_brcondi_i32(TCG_COND_NE, condResult, 0x1, cont);
			tcg_gen_mov_tl(r3_local, r1_local);
			gen_set_label(cont);

			gen_set_gpr(int_rs3, r3_local);

			tcg_temp_free_i32(r1_local);
			tcg_temp_free_i32(r2_local);
			tcg_temp_free_i32(r3_local);
		}
			break;

		case OPC_RH850_CMOV_cccc_imm5_reg2_reg3: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();

			if((int_imm & 0x10)==0x10){
				int_imm = int_imm | (0x7 << 5);
			}
			tcg_gen_movi_tl(tcg_temp, int_imm);
			tcg_gen_ext8s_i32(r1_local, tcg_temp);

			tcg_gen_mov_i32(r2_local, tcg_r2);

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(r3_local,int_rs3);

			int_cond = extract32(ctx->opcode, 17, 4);
			condResult = condition_satisfied(int_cond);
			cont = gen_new_label();

			tcg_gen_mov_tl(r3_local, r2_local);
			tcg_gen_brcondi_i32(TCG_COND_NE, condResult, 0x1, cont);
			tcg_gen_mov_tl(r3_local, r1_local);

			gen_set_label(cont);

			gen_set_gpr(int_rs3, r3_local);

			tcg_temp_free_i32(r1_local);
			tcg_temp_free_i32(r2_local);
			tcg_temp_free_i32(r3_local);
		}
			break;

		case OPC_RH850_HSH_reg2_reg3:

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_set_gpr(int_rs3, tcg_r1);

			tcg_gen_shri_i32(cpu_SF, tcg_r1, 0x1f);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, tcg_r1, 0x0);
			tcg_gen_andi_i32(tcg_temp, tcg_r1, 0x0000ffff);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_CYF, tcg_temp, 0x0);
			tcg_gen_movi_i32(cpu_OVF, 0x0);

			break;

		case OPC_RH850_HSW_reg2_reg3: {
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();
			TCGv temp2_local = tcg_temp_local_new_i32();
			TCGv temp3_local = tcg_temp_local_new_i32();
			TCGv count_local = tcg_temp_local_new_i32();

			cont = gen_new_label();
			end = gen_new_label();
			set = gen_new_label();

			tcg_gen_mov_i32(r2_local, tcg_r2);

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(r3_local,int_rs3);

			tcg_gen_andi_tl(temp_local, r2_local, 0xffff);
			tcg_gen_shli_tl(temp_local, temp_local, 0x10);
			tcg_gen_andi_tl(temp2_local, r2_local, 0xffff0000);
			tcg_gen_shri_tl(temp2_local, temp2_local, 0x10);

			tcg_gen_or_tl(r3_local, temp2_local, temp_local);
			gen_set_gpr(int_rs3, r3_local);

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r3_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r3_local, 0x1f);

			tcg_gen_movi_i32(count_local, 0x0);

			gen_set_label(cont);////

			tcg_gen_andi_i32(temp3_local, r3_local, 0x0000ffff);
			tcg_gen_brcondi_i32(TCG_COND_EQ, temp3_local, 0x0, set);////
			tcg_gen_addi_i32(count_local, count_local, 0x1);
			tcg_gen_shri_i32(r3_local, r3_local, 0x1);
			tcg_gen_brcondi_i32(TCG_COND_NE, count_local, 0x11, cont);////
			tcg_gen_movi_i32(cpu_CYF, 0x0);
			tcg_gen_br(end);

			gen_set_label(set);////
			tcg_gen_movi_i32(cpu_CYF, 0x1);

			gen_set_label(end);////
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_ROTL_imm5_reg2_reg3:
			tcg_gen_movi_tl(tcg_imm, int_imm);
			tcg_gen_ext8u_tl(tcg_imm, tcg_imm);
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_rotl_tl(tcg_r3, tcg_r2, tcg_imm);
			gen_set_gpr(int_rs3, tcg_r3);

			tcg_gen_andi_i32(cpu_CYF, tcg_r3, 0x1);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, tcg_r3, 0x0);
			tcg_gen_shri_i32(cpu_SF, tcg_r3, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);

			break;

		case OPC_RH850_ROTL_reg1_reg2_reg3:
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_rotl_tl(tcg_r3, tcg_r2, tcg_r1);
			gen_set_gpr(int_rs3, tcg_r3);

			tcg_gen_andi_i32(cpu_CYF, tcg_r3, 0x1);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, tcg_r3, 0x0);
			tcg_gen_shri_i32(cpu_SF, tcg_r3, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);

			break;

		case OPC_RH850_SAR_reg1_reg2: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_andi_i32(r1_local, r1_local, 0x1f);	//shift by only lower 5 bits of reg1
			tcg_gen_mov_i32(r2_local, tcg_r2);

			tcg_gen_sar_i32(r2_local, r2_local, r1_local);

			gen_set_gpr(rs2, r2_local);

			cont = gen_new_label();
			//cpu_CYF is MSB after shift
			tcg_gen_shri_i32(cpu_CYF, r2_local, 0x1f);

			tcg_gen_brcondi_i32(TCG_COND_NE, r1_local, 0x0, cont);
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(cont);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r2_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SAR_imm5_reg2: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();

			tcg_gen_movi_tl(r1_local, int_imm);
			tcg_gen_ext8u_i32(r1_local, r1_local);
			tcg_gen_mov_i32(r2_local, tcg_r2);

			tcg_gen_sar_i32(r2_local, r2_local, r1_local);
			gen_set_gpr(rs2, r2_local);

			cont = gen_new_label();

			tcg_gen_shri_i32(cpu_CYF, r2_local, 0x1f);

			tcg_gen_brcondi_i32(TCG_COND_NE, r1_local, 0x0, cont);
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(cont);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r2_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SAR_reg1_reg2_reg3: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_andi_i32(r1_local, r1_local, 0x1f);	//shift by only lower 5 bits of reg1
			tcg_gen_mov_i32(r2_local, tcg_r2);
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(r3_local, int_rs3);

			tcg_gen_sar_i32(r3_local, r2_local, r1_local);
			gen_set_gpr(int_rs3, r3_local);

			cont = gen_new_label();

			tcg_gen_shri_i32(cpu_CYF, r2_local, 0x1f);

			tcg_gen_brcondi_i32(TCG_COND_NE, r1_local, 0x0, cont);
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(cont);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r2_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SASF_cccc_reg2: {
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv operand_local = tcg_temp_local_new_i32();

			int_cond = extract32(ctx->opcode,0,4);
			condResult = condition_satisfied(int_cond);
			cont = gen_new_label();

			tcg_gen_shli_tl(r2_local, tcg_r2, 0x1);

			tcg_gen_movi_i32(operand_local, 0x00000000);
			tcg_gen_brcondi_i32(TCG_COND_NE, condResult, 0x1, cont);
			tcg_gen_movi_i32(operand_local, 0x00000001);

			gen_set_label(cont);
			tcg_gen_or_tl(r2_local, r2_local, operand_local);

			gen_set_gpr(rs2, r2_local);
		}
			break;

		case OPC_RH850_SETF_cccc_reg2:{

			TCGv operand_local = tcg_temp_local_new_i32();
			int_cond = extract32(ctx->opcode,0,4);
			condResult = condition_satisfied(int_cond);
			cont = gen_new_label();

			tcg_gen_movi_i32(operand_local, 0x00000000);
			tcg_gen_brcondi_i32(TCG_COND_NE, condResult, 0x1, cont);
			tcg_gen_movi_i32(operand_local, 0x00000001);

			gen_set_label(cont);

			gen_set_gpr(rs2, operand_local);
		}
			break;

		case OPC_RH850_SHL_reg1_reg2: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_mov_i32(r2_local, tcg_r2);

			cont = gen_new_label();
			end = gen_new_label();

			tcg_gen_andi_i32(r1_local, r1_local, 0x1f); 	//get only lower 5 bits

			tcg_gen_brcondi_i32(TCG_COND_EQ, r1_local, 0x0, cont);

			tcg_gen_subi_i32(temp_local, r1_local, 0x1); 	// shifting for [r1]-1
			tcg_gen_shl_tl(r2_local, r2_local, temp_local);

			tcg_gen_shri_i32(cpu_CYF, r2_local, 0x1f);	// checking the last bit to shift
			tcg_gen_shli_i32(r2_local, r2_local, 0x1);		// shifting for that remaining 1

			gen_set_gpr(rs2, r2_local);
			tcg_gen_br(end);

			gen_set_label(cont);////
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(end);////
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r2_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SHL_imm5_reg2: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();

			tcg_gen_mov_i32(r2_local, tcg_r2);

			tcg_gen_movi_tl(r1_local, int_imm);
			tcg_gen_ext8u_tl(r1_local, r1_local);

			cont = gen_new_label();
			end = gen_new_label();

			tcg_gen_brcondi_i32(TCG_COND_EQ, r1_local, 0x0, cont);

			tcg_gen_subi_i32(temp_local, r1_local, 0x1);
			tcg_gen_shl_tl(r2_local, r2_local, temp_local);
			tcg_gen_shri_i32(cpu_CYF, r2_local, 0x1f);
			tcg_gen_shli_tl(r2_local, r2_local, 0x1);
			tcg_gen_br(end);

			gen_set_label(cont);////
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(end);////
			gen_set_gpr(rs2, r2_local);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r2_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SHL_reg1_reg2_reg3: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_andi_i32(r1_local, r1_local, 0x1f);
			tcg_gen_mov_i32(r2_local, tcg_r2);

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(r3_local,int_rs3);

			cont = gen_new_label();
			end = gen_new_label();

			tcg_gen_brcondi_i32(TCG_COND_EQ, r1_local, 0x0, cont); 	// when reg1 = 0, do not shift

			tcg_gen_subi_i32(temp_local, r1_local, 0x1);
			tcg_gen_shl_tl(r3_local, r2_local, temp_local);

			tcg_gen_shri_i32(cpu_CYF, r3_local, 0x1f);
			tcg_gen_shli_tl(r3_local, r3_local, 0x1);
			tcg_gen_br(end);

			gen_set_label(cont);////
			tcg_gen_mov_i32(r3_local, r2_local);
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(end);////
			gen_set_gpr(int_rs3, r3_local);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r3_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r3_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SHR_reg1_reg2: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_andi_i32(r1_local, r1_local, 0x1f);
			tcg_gen_mov_i32(r2_local, tcg_r2);
			gen_set_gpr(18, r1_local);
			cont = gen_new_label();
			end = gen_new_label();

			tcg_gen_brcondi_i32(TCG_COND_EQ, r1_local, 0x0, cont); //checking for non-shift

			tcg_gen_subi_i32(temp_local, r1_local, 0x1); 	// shifting for [r1]-1
			tcg_gen_shr_tl(r2_local, r2_local, temp_local);


			tcg_gen_andi_i32(cpu_CYF, r2_local, 0x1);	// checking the last bit to shift (LSB)
			tcg_gen_shri_i32(r2_local, r2_local, 0x1);

			tcg_gen_br(end);

			gen_set_label(cont);
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(end);
			gen_set_gpr(rs2, r2_local);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r2_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SHR_imm5_reg2: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();

			tcg_gen_mov_i32(r2_local, tcg_r2);

			tcg_gen_movi_tl(r1_local, int_imm);
			tcg_gen_ext8u_tl(r1_local, r1_local);

			cont = gen_new_label();
			end = gen_new_label();

			tcg_gen_brcondi_i32(TCG_COND_EQ, r1_local, 0x0, cont); //checking for non-shift

			tcg_gen_subi_i32(temp_local, r1_local, 0x1); 	// shifting for [r1]-1
			tcg_gen_shr_tl(r2_local, r2_local, temp_local);

			tcg_gen_andi_i32(cpu_CYF, r2_local, 0x1);	// checking the last bit to shift (LSB)
			tcg_gen_shri_i32(r2_local, r2_local, 0x1);

			tcg_gen_br(end);

			gen_set_label(cont);
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(end);
			gen_set_gpr(rs2, r2_local);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r2_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SHR_reg1_reg2_reg3: {

			TCGv r1_local = tcg_temp_local_new_i32();
			TCGv r2_local = tcg_temp_local_new_i32();
			TCGv r3_local = tcg_temp_local_new_i32();
			TCGv temp_local = tcg_temp_local_new_i32();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_andi_i32(r1_local, r1_local, 0x1f);
			tcg_gen_mov_i32(r2_local, tcg_r2);
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(r3_local, int_rs3);

			cont = gen_new_label();
			end = gen_new_label();

			tcg_gen_brcondi_i32(TCG_COND_EQ, r1_local, 0x0, cont); //checking for non-shift

			tcg_gen_subi_i32(temp_local, r1_local, 0x1); 	// shifting for [r1]-1
			tcg_gen_shr_tl(r3_local, r2_local, temp_local);

			tcg_gen_andi_i32(cpu_CYF, r3_local, 0x1);	// checking the last bit to shift (LSB)
			tcg_gen_shri_i32(r3_local, r3_local, 0x1);

			tcg_gen_br(end);

			gen_set_label(cont);
			tcg_gen_movi_i32(cpu_CYF, 0x0);

			gen_set_label(end);
			gen_set_gpr(int_rs3, r3_local);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r3_local, 0x0);
			tcg_gen_shri_i32(cpu_SF, r3_local, 0x1f);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
		}
			break;

		case OPC_RH850_SXB_reg1:
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFF);
			tcg_gen_ext8s_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;

		case OPC_RH850_SXH_reg1:
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFFFF);
			tcg_gen_ext16s_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;

		case OPC_RH850_ZXH_reg1:
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFFFF);
			tcg_gen_ext16u_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;

		case OPC_RH850_ZXB_reg1:
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFF);
			tcg_gen_ext8u_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;
	}

	tcg_temp_free(tcg_r1);
	tcg_temp_free(tcg_r2);
	tcg_temp_free(tcg_r3);
	tcg_temp_free(tcg_imm);
	tcg_temp_free(tcg_temp);
	tcg_temp_free(tcg_temp2);

}

static void gen_bit_search(DisasContext *ctx, int rs2, int operation)			// completed
{

	TCGv tcg_r2 = tcg_temp_new();
	TCGv tcg_r3 = tcg_temp_new();
	int int_rs3;
	int_rs3 = extract32(ctx->opcode, 27, 5);

	gen_get_gpr(tcg_r2, rs2);
	gen_get_gpr(tcg_r3, int_rs3);

	TCGLabel *end;
	TCGLabel *found;
	TCGLabel *loop;

	switch(operation){
		case OPC_RH850_SCH0L_reg2_reg3: {

			TCGv r2_local = tcg_temp_local_new();
			TCGv r3_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv count = tcg_temp_local_new();
			tcg_gen_mov_i32(r2_local, tcg_r2);
			tcg_gen_mov_i32(r3_local, tcg_r3);
			tcg_gen_movi_i32(count, 0x0);

			end = gen_new_label();
			found = gen_new_label();
			loop = gen_new_label();

			gen_set_label(loop);//---------------------------------------------------

			tcg_gen_shl_i32(check, r2_local, count);
			tcg_gen_ori_i32(check, check, 0x7fffffff);	// check MSB bit
			tcg_gen_brcondi_tl(TCG_COND_EQ, check, 0x7fffffff, found);

			tcg_gen_addi_i32(count, count, 0x1);
			tcg_gen_brcondi_tl(TCG_COND_NE, count, 0x20, loop);//--------------------

			tcg_gen_movi_i32(result, 0x0);
			tcg_gen_movi_i32(cpu_ZF, 0x1);
			tcg_gen_br(end);

			gen_set_label(found);
			tcg_gen_addi_i32(result, count, 0x1);

			tcg_gen_brcondi_tl(TCG_COND_NE, result, 0x20, end);
			tcg_gen_movi_i32(cpu_CYF, 0x1);

			gen_set_label(end);
			gen_set_gpr(int_rs3, result);

			tcg_gen_movi_i32(cpu_OVF, 0x0);
			tcg_gen_movi_i32(cpu_SF, 0x0);
			tcg_temp_free(r2_local);
			tcg_temp_free(count);
			tcg_temp_free(result);
		}	break;

		case OPC_RH850_SCH0R_reg2_reg3: {

			TCGv r2_local = tcg_temp_local_new();
			TCGv r3_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv count = tcg_temp_local_new();
			tcg_gen_mov_i32(r2_local, tcg_r2);
			tcg_gen_mov_i32(r3_local, tcg_r3);
			tcg_gen_movi_i32(count, 0x0);

			end = gen_new_label();
			found = gen_new_label();
			loop = gen_new_label();

			gen_set_label(loop);//---------------------------------------------------

			tcg_gen_shr_i32(check, r2_local, count);
			tcg_gen_ori_i32(check, check, 0xfffffffe);	// check MSB bit
			tcg_gen_brcondi_tl(TCG_COND_EQ, check, 0xfffffffe, found);

			tcg_gen_addi_i32(count, count, 0x1);
			tcg_gen_brcondi_tl(TCG_COND_NE, count, 0x20, loop);//--------------------

			tcg_gen_movi_i32(result, 0x0);
			tcg_gen_movi_i32(cpu_ZF, 0x1);
			tcg_gen_br(end);

			gen_set_label(found);
			tcg_gen_addi_i32(result, count, 0x1);

			tcg_gen_brcondi_tl(TCG_COND_NE, result, 0x20, end);
			tcg_gen_movi_i32(cpu_CYF, 0x1);

			gen_set_label(end);
			gen_set_gpr(int_rs3, result);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
			tcg_gen_movi_i32(cpu_SF, 0x0);
			tcg_temp_free(r2_local);
			tcg_temp_free(count);
			tcg_temp_free(result);
		}	break;

		case OPC_RH850_SCH1L_reg2_reg3: {

			TCGv r2_local = tcg_temp_local_new();
			TCGv r3_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv count = tcg_temp_local_new();
			tcg_gen_mov_i32(r2_local, tcg_r2);
			tcg_gen_mov_i32(r3_local, tcg_r3);
			tcg_gen_movi_i32(count, 0x0);

			end = gen_new_label();
			found = gen_new_label();
			loop = gen_new_label();

			gen_set_label(loop);//---------------------------------------------------

			tcg_gen_shl_i32(check, r2_local, count);
			tcg_gen_andi_i32(check, check, 0x80000000);	// check MSB bit
			tcg_gen_brcondi_tl(TCG_COND_EQ, check, 0x80000000, found);

			tcg_gen_addi_i32(count, count, 0x1);
			tcg_gen_brcondi_tl(TCG_COND_NE, count, 0x20, loop);//--------------------

			tcg_gen_movi_i32(result, 0x0);
			tcg_gen_movi_i32(cpu_ZF, 0x1);
			tcg_gen_br(end);

			gen_set_label(found);
			tcg_gen_addi_i32(result, count, 0x1);

			tcg_gen_brcondi_tl(TCG_COND_NE, result, 0x20, end);
			tcg_gen_movi_i32(cpu_CYF, 0x1);

			gen_set_label(end);
			gen_set_gpr(int_rs3, result);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
			tcg_gen_movi_i32(cpu_SF, 0x0);
			tcg_temp_free(r2_local);
			tcg_temp_free(count);
			tcg_temp_free(result);
		}	break;

		case OPC_RH850_SCH1R_reg2_reg3: {

			TCGv r2_local = tcg_temp_local_new();
			TCGv r3_local = tcg_temp_local_new();
			TCGv result = tcg_temp_local_new();
			TCGv check = tcg_temp_local_new();
			TCGv count = tcg_temp_local_new();
			tcg_gen_mov_i32(r2_local, tcg_r2);
			tcg_gen_mov_i32(r3_local, tcg_r3);
			tcg_gen_movi_i32(count, 0x0);

			end = gen_new_label();
			found = gen_new_label();
			loop = gen_new_label();

			gen_set_label(loop);//---------------------------------------------------

			tcg_gen_shr_i32(check, r2_local, count);
			tcg_gen_andi_i32(check, check, 0x1);	// check MSB bit
			tcg_gen_brcondi_tl(TCG_COND_EQ, check, 0x1, found);

			tcg_gen_addi_i32(count, count, 0x1);
			tcg_gen_brcondi_tl(TCG_COND_NE, count, 0x20, loop);//--------------------

			tcg_gen_movi_i32(result, 0x0);
			tcg_gen_movi_i32(cpu_ZF, 0x1);
			tcg_gen_br(end);

			gen_set_label(found);
			tcg_gen_addi_i32(result, count, 0x1);

			tcg_gen_brcondi_tl(TCG_COND_NE, result, 0x20, end);
			tcg_gen_movi_i32(cpu_CYF, 0x1);

			gen_set_label(end);
			gen_set_gpr(int_rs3, result);
			tcg_gen_movi_i32(cpu_OVF, 0x0);
			tcg_gen_movi_i32(cpu_SF, 0x0);
			tcg_temp_free(r2_local);
			tcg_temp_free(count);
			tcg_temp_free(result);
		}	break;
	}
}

static void gen_divide(DisasContext *ctx, int rs1, int rs2, int operation)	// completed
{

	TCGv tcg_r1 = tcg_temp_new();
	TCGv tcg_r2 = tcg_temp_new();

	gen_get_gpr(tcg_r1, rs1);
	gen_get_gpr(tcg_r2, rs2);

	int int_rs3;

	TCGv tcg_r3 = tcg_temp_new();

	switch(operation){

		case OPC_RH850_DIV_reg1_reg2_reg3:{

			TCGLabel *cont;
			TCGLabel *end;
			TCGLabel *fin;

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv r3_local = tcg_temp_local_new();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_mov_i32(r2_local, tcg_r2);


			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3, int_rs3);
			tcg_gen_mov_i32(r3_local, tcg_r3);
			TCGv overflowed = tcg_temp_local_new();
			TCGv overflowed2 = tcg_temp_local_new();

			cont = gen_new_label();
			end = gen_new_label();
			fin = gen_new_label();

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_OVF, r2_local, 0x0);
			tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x1, cont); 		//if r2=0 jump to end
			tcg_gen_br(fin);

			gen_set_label(cont);

			tcg_gen_setcondi_i32(TCG_COND_EQ, overflowed, r2_local, 0x80000000);
			tcg_gen_setcondi_i32(TCG_COND_EQ, overflowed2, r1_local, 0xffffffff);
			tcg_gen_and_i32(overflowed, overflowed, overflowed2);		//if both

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_OVF, overflowed, 0x1);	//are 1
			tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x1, end);
			tcg_gen_movi_i32(r2_local, 0x80000000);						//DO THIS
			tcg_gen_movi_i32(cpu_OVF, 0x1);
			gen_set_gpr(rs2, r2_local);
			tcg_gen_br(fin);

			gen_set_label(end);

			tcg_gen_rem_i32(r3_local, r2_local, r1_local);
			tcg_gen_div_i32(r2_local, r2_local, r1_local);

			if(rs2==int_rs3){
				gen_set_gpr(rs2, r3_local);
			} else {
				gen_set_gpr(rs2, r2_local);
				gen_set_gpr(int_rs3, r3_local);
			}

			gen_set_label(fin);

			tcg_gen_setcondi_i32(TCG_COND_LT, cpu_SF, r2_local, 0x0);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);

		}	break;

		case OPC_RH850_DIVH_reg1_reg2:{

			TCGLabel *cont;
			TCGLabel *end;
			TCGLabel *fin;

			tcg_gen_andi_i32(tcg_r1, tcg_r1, 0x0000FFFF);
			tcg_gen_ext16s_i32(tcg_r1, tcg_r1);

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv overflowed = tcg_temp_local_new();
			TCGv overflowed2 = tcg_temp_local_new();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_mov_i32(r2_local, tcg_r2);

			cont = gen_new_label();
			end = gen_new_label();
			fin = gen_new_label();

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_OVF, r2_local, 0x0);
			tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x1, cont); 		//if r2=0 jump to cont
			tcg_gen_br(fin);

			gen_set_label(cont);

			tcg_gen_setcondi_i32(TCG_COND_EQ, overflowed, r2_local, 0x80000000);
			tcg_gen_setcondi_i32(TCG_COND_EQ, overflowed2, r1_local, 0xffffffff);
			tcg_gen_and_i32(overflowed, overflowed, overflowed2);		//if both

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_OVF, overflowed, 0x1);	//are 1
			tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x1, end);
			tcg_gen_movi_i32(r2_local, 0x80000000);						//DO THIS
			tcg_gen_movi_i32(cpu_OVF, 0x1);
			gen_set_gpr(rs2, r2_local);
			tcg_gen_br(fin);

			gen_set_label(end);

			tcg_gen_div_i32(r2_local, r2_local, r1_local);
			gen_set_gpr(rs2, r2_local);

			gen_set_label(fin);

			tcg_gen_setcondi_i32(TCG_COND_LT, cpu_SF, r2_local, 0x0);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);

		}	break;

		case OPC_RH850_DIVH_reg1_reg2_reg3: {
			// 0x80000000/0xffffffff=0x80000000; cpu_OVF=1
			// reg2/0x0000=undefined; cpu_OVF=1
			// if reg2==reg3; reg2=remainder

			TCGLabel *cont;
			TCGLabel *end;
			TCGLabel *fin;

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv r3_local = tcg_temp_local_new();

			tcg_gen_andi_i32(tcg_r1, tcg_r1, 0x0000FFFF);
			tcg_gen_ext16s_i32(tcg_r1, tcg_r1);
			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_mov_i32(r2_local, tcg_r2);

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3, int_rs3);
			tcg_gen_mov_i32(r3_local, tcg_r3);
			TCGv overflowed = tcg_temp_local_new();
			TCGv overflowed2 = tcg_temp_local_new();

			cont = gen_new_label();
			end = gen_new_label();
			fin = gen_new_label();

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_OVF, r2_local, 0x0);
			tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x1, cont);
			tcg_gen_br(fin);

			gen_set_label(cont);	/////

			tcg_gen_setcondi_i32(TCG_COND_EQ, overflowed, r2_local, 0x80000000);
			tcg_gen_setcondi_i32(TCG_COND_EQ, overflowed2, r1_local, 0xffffffff);
			tcg_gen_and_i32(overflowed, overflowed, overflowed2);	// if result is 1, cpu_OVF = 1

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_OVF, overflowed, 0x1);
			tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x1, end);
			tcg_gen_movi_i32(r2_local, 0x80000000);
			tcg_gen_movi_i32(cpu_OVF, 0x1);
			gen_set_gpr(rs2, r2_local);
			tcg_gen_br(fin);

			gen_set_label(end);		/////

			tcg_gen_rem_i32(r3_local, r2_local, r1_local);
			tcg_gen_div_i32(r2_local, r2_local, r1_local);

			if(rs2==int_rs3){
				gen_set_gpr(rs2, r3_local);
			} else {
				gen_set_gpr(rs2, r2_local);
				gen_set_gpr(int_rs3, r3_local);
			}

			gen_set_label(fin);		/////

			tcg_gen_setcondi_i32(TCG_COND_LT, cpu_SF, r2_local, 0x0);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);

		}	break;

		case OPC_RH850_DIVHU_reg1_reg2_reg3:{

			TCGLabel *cont;
			TCGLabel *fin;

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv r3_local = tcg_temp_local_new();

			tcg_gen_andi_i32(tcg_r1, tcg_r1, 0x0000FFFF);
			tcg_gen_ext16u_i32(tcg_r1, tcg_r1);
			tcg_gen_mov_i32(r1_local, tcg_r1);

			tcg_gen_mov_i32(r2_local, tcg_r2);

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3, int_rs3);
			tcg_gen_mov_i32(r3_local, tcg_r3);

			cont = gen_new_label();
			fin = gen_new_label();

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_OVF, r2_local, 0x0);
			tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x1, cont);
			tcg_gen_br(fin);

			gen_set_label(cont);	/////
			tcg_gen_remu_i32(r3_local, r2_local, r1_local);
			tcg_gen_divu_i32(r2_local, r2_local, r1_local);


			if(rs2==int_rs3){
				gen_set_gpr(rs2, r3_local);
			} else {
				gen_set_gpr(rs2, r2_local);
				gen_set_gpr(int_rs3, r3_local);
			}

			gen_set_label(fin);		/////

			tcg_gen_setcondi_i32(TCG_COND_LT, cpu_SF, r2_local, 0x0);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
		}
			break;

		case OPC_RH850_DIVU_reg1_reg2_reg3:{	// druga

			// reg2/0x0000=undefined; cpu_OVF=1
			// if reg2==reg3; reg2=remainder

			TCGLabel *cont;
			TCGLabel *fin;

			TCGv r1_local = tcg_temp_local_new();
			TCGv r2_local = tcg_temp_local_new();
			TCGv r3_local = tcg_temp_local_new();

			tcg_gen_mov_i32(r1_local, tcg_r1);
			tcg_gen_mov_i32(r2_local, tcg_r2);

			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3, int_rs3);
			tcg_gen_mov_i32(r3_local, tcg_r3);

			cont = gen_new_label();
			fin = gen_new_label();

			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_OVF, r2_local, 0x0);
			tcg_gen_brcondi_i32(TCG_COND_NE, cpu_OVF, 0x1, cont);
			tcg_gen_br(fin);

			gen_set_label(cont);	/////

			tcg_gen_remu_i32(tcg_r3, tcg_r2, tcg_r1);
			tcg_gen_divu_i32(tcg_r2, tcg_r2, tcg_r1);

			if(rs2==int_rs3){
				gen_set_gpr(rs2, r3_local);
			} else {
				gen_set_gpr(rs2, r2_local);
				gen_set_gpr(int_rs3, r3_local);
			}

			gen_set_label(fin);		/////
			tcg_gen_setcondi_i32(TCG_COND_LT, cpu_SF, r2_local, 0x0);
			tcg_gen_setcondi_i32(TCG_COND_EQ, cpu_ZF, r2_local, 0x0);
		}
			break;
	}

	tcg_temp_free_i32(tcg_r1);
	tcg_temp_free_i32(tcg_r2);
	tcg_temp_free_i32(tcg_r3);
}

static void gen_branch(CPURH850State *env, DisasContext *ctx, uint32_t cond,
                       int rs1, int rs2, target_long bimm)
{
    TCGLabel *l = gen_new_label();
    TCGv condTest, condOK;

    condOK = tcg_temp_local_new();
    condTest = tcg_temp_local_new();

    condTest = condition_satisfied(cond);
    tcg_gen_movi_i32(condOK, 0x1);


    tcg_gen_brcond_tl(TCG_COND_EQ, condTest, condOK, l);


    gen_goto_tb(ctx, 1, ctx->next_pc);
    gen_set_label(l); /* branch taken */
    if (!rh850_has_ext(env, 4) && ((ctx->pc + bimm) & 0x3)) {

        /* misaligned */
    	printf("Address was misaligned! (PC is %x) \n  This is first if cond: %x \n ", ctx->pc + bimm, rh850_has_ext(env, 2));
        gen_exception_inst_addr_mis(ctx);
    } else {
    	printf("This is the new PC: %x \n  This is first if cond: %x \n", (ctx->pc+bimm), rh850_has_ext(env, 2));
    	gen_goto_tb(ctx, 0, ctx->pc + bimm);

    }
    ctx->bstate = BS_BRANCH;
}

//this function is only for testing, it can be deleted
static void gen_jmp(DisasContext *ctx, int rs1, int rs2, int operation){

	TCGv dest = tcg_temp_new();
	gen_get_gpr(dest, rs1);

	switch(operation){
	case 0:

		//tcg_gen_mov_i32(cpu_pc, dest);
		//ctx->next_pc = ctx->pc + 6;

		//gen_goto_tb(ctx, 0, dest);

		break;
	}

};

//static void gen_loop(DisasContext *ctx, int rs1, int rs2, int operation){}

//static void gen_bit_manipulation(DisasContext *ctx, int rs1, int rs2, int operation){}
// NEED LOAD TO WORK

static void gen_special(DisasContext *ctx, int rs1, int rs2, int operation){

	TCGv temp = tcg_temp_new_i32();
	TCGv adr = tcg_temp_new_i32();
	TCGv r2 = tcg_temp_new();
	int regID;
	int imm;

	switch(operation){
	case OPC_RH850_CALLT_imm6:

		tcg_gen_addi_i32(cpu_sysRegs[CTPC_register], cpu_pc, 0x2);
		tcg_gen_andi_i32(temp, cpu_sysRegs[PSW_register], 0xf);

		tcg_gen_andi_i32(cpu_sysRegs[CTPSW_register], cpu_sysRegs[CTPSW_register], 0xfffffff0);
		tcg_gen_or_i32(cpu_sysRegs[CTPSW_register], cpu_sysRegs[CTPSW_register], temp);

		imm = extract32(ctx->opcode, 0, 6);
		tcg_gen_movi_i32(adr, imm);
		tcg_gen_shli_i32(adr, adr, 0x1);
		tcg_gen_ext8s_i32(adr, adr);
		tcg_gen_add_i32(adr, cpu_sysRegs[CTBP_register], adr);

		// TODO: Load and set the new PC, check for misaligned access

		break;
	case OPC_RH850_CAXI_reg1_reg2_reg3:
		break;
	case OPC_RH850_CLL:

		break;

	case OPC_RH850_CTRET:

		// TODO: Load and set the new PC, check for misaligned access

		tcg_gen_mov_i32(cpu_pc, cpu_sysRegs[CTPC_register]);
		tcg_gen_andi_i32(temp, cpu_sysRegs[CTPSW_register], 0x1f);
		gen_set_gpr(25, temp);
		tcg_gen_andi_i32(cpu_sysRegs[PSW_register], cpu_sysRegs[PSW_register], 0xffffffe0);
		tcg_gen_or_i32(cpu_sysRegs[PSW_register], cpu_sysRegs[PSW_register], temp);
		break;

	case OPC_RH850_DI:
		tcg_gen_movi_i32(cpu_ID, 0x1);
		break;
	case OPC_RH850_DISPOSE_imm5_list12:

		break;

	case OPC_RH850_DISPOSE_imm5_list12_reg1:

		break;

	case OPC_RH850_EI:
		tcg_gen_movi_i32(cpu_ID, 0x0);
		break;
	case OPC_RH850_EIRET:			// TODO: Load and set the new PC, check for misaligned access
		tcg_gen_mov_i32(cpu_pc, cpu_sysRegs[EIPC_register]);
		tcg_gen_mov_i32(cpu_sysRegs[PSW_register], cpu_sysRegs[EIPSW_register]);
		break;
	case OPC_RH850_FERET:			// TODO: Load and set the new PC, check for misaligned access
		tcg_gen_mov_i32(cpu_pc, cpu_sysRegs[FEPC_register]);
		tcg_gen_mov_i32(cpu_sysRegs[PSW_register], cpu_sysRegs[FEPSW_register]);
		break;

	case OPC_RH850_FETRAP_vector4:

		tcg_gen_addi_i32(cpu_sysRegs[FEPC_register], cpu_pc, 0x2);
		tcg_gen_mov_i32(cpu_sysRegs[FEPSW_register], cpu_sysRegs[PSW_register]);
		tcg_gen_movi_i32(cpu_sysRegs[FEIC_register], 0x0); // TODO: Write the correct except. cause code
		tcg_gen_movi_i32(cpu_UM, 0x0);
		tcg_gen_movi_i32(cpu_NP, 0x1);
		tcg_gen_movi_i32(cpu_EP, 0x1);
		tcg_gen_movi_i32(cpu_ID, 0x1);
		// TODO: Write the correct except. handler address to PC

		break;

	case OPC_RH850_HALT:
		break;


	case OPC_RH850_LDSR_reg2_regID_selID:
		regID=rs2;
		if(regID==PSW_register){
			gen_reset_flags(ctx);
		}
		gen_get_gpr(r2, rs1);
		gen_set_sysreg(regID, r2);
		break;

	//case OPC_RH850_LDLW:
	//	break;

	//case OPC_RH850_NOP:
	//	break;

	case OPC_RH850_RIE:

		tcg_gen_addi_i32(cpu_sysRegs[FEPC_register], cpu_pc, 0x2);
		tcg_gen_mov_i32(cpu_sysRegs[FEPSW_register], cpu_sysRegs[PSW_register]);
		tcg_gen_movi_i32(cpu_sysRegs[FEIC_register], 0x0); // TODO: Write the correct except. cause code
		tcg_gen_movi_i32(cpu_UM, 0x0);
		tcg_gen_movi_i32(cpu_NP, 0x1);
		tcg_gen_movi_i32(cpu_EP, 0x1);
		tcg_gen_movi_i32(cpu_ID, 0x1);
		// TODO: Write the correct except. handler address to PC

		break;

	case OPC_RH850_SNOOZE:
		break;

	//case OPC_RH850_STCW:
	//	break;

	case OPC_RH850_STSR_regID_reg2_selID:
		regID=rs1;
		gen_get_sysreg(r2, regID);
		gen_set_gpr(rs2, r2);
		break;

	case OPC_RH850_SWITCH_reg1: // TODO: Load and set the new PC, check for misaligned access
		gen_get_gpr(adr, rs1);
		tcg_gen_shli_i32(adr, adr, 0x1);
		tcg_gen_add_i32(adr, adr, cpu_pc);
		tcg_gen_addi_i32(adr, adr, 0x2);
		break;

	// SYNC instructions will not be implemented

	case OPC_RH850_TRAP:
		tcg_gen_addi_i32(cpu_sysRegs[EIPC_register], cpu_pc, 0x4);
		tcg_gen_mov_i32(cpu_sysRegs[EIPSW_register], cpu_sysRegs[PSW_register]);
		tcg_gen_movi_i32(cpu_sysRegs[EIIC_register], 0x0); // TODO: Write the correct except. cause code
		tcg_gen_movi_i32(cpu_UM, 0x0);
		tcg_gen_movi_i32(cpu_EP, 0x1);
		tcg_gen_movi_i32(cpu_ID, 0x1);
		break;
	}
}

static void decode_RH850_48(CPURH850State *env, DisasContext *ctx)
{
	//uint32_t op;
	//int sub_opcode;
	int rs2;
	uint64_t opcode48;

	//op = MASK_OP_MAJOR(ctx->opcode);	// opcode is at b5-b10
	//sub_opcode = GET_RS2(ctx->opcode);
	rs2 = GET_RS1(ctx->opcode);

	opcode48 = (ctx->opcode1);
	opcode48 = (ctx->opcode) | (opcode48  << 0x20);
	uint32_t opcode20 = extract32(opcode48,0,20) & 0xfffe0;

	switch(opcode20){
		case OPC_RH850_LDDW:
			printf("ld.dw \n");
			break;
		case OPC_RH850_STDW:
			printf("st.dw \n");
		break;

	}
	if(extract32(ctx->opcode, 5, 11) == 0x31){
		gen_arithmetic(ctx, 0, rs2, OPC_RH850_MOV_imm32_reg1);		// this is MOV3 (48bit inst)
	} else if (extract32(ctx->opcode, 5, 12) == 0x37) {
		// this is JMP2 (48bit inst)
	} else if (extract32(ctx->opcode, 5, 11) == 0x17) {
		if (rs2 == 0x0){
			//gen_branch(ctx, 0, 0, rs2, OPC_RH850_JR_imm32); change this to call
			// this is JR2 (48bit inst)

		} else {
			//gen_branch(ctx, 0, 0, rs2, OPC_RH850_JARL_disp32_reg1); change this call
			// this is JARL2 (48bit inst)
		}
	}
}

static void decode_RH850_32(CPURH850State *env, DisasContext *ctx)
{
	int rs1;
	int rs2;
	int cond;
	uint32_t op;
	uint32_t formXop;
	uint32_t checkXII;
	uint32_t check32bitZERO;
	target_long imm_32;
	target_long ld_imm;

	op = MASK_OP_MAJOR(ctx->opcode);
	rs1 = GET_RS1(ctx->opcode);			// rs1 is at b0-b4;
	rs2 = GET_RS2(ctx->opcode);			// rs2 is at b11-b15;
	TCGv r1 = tcg_temp_local_new();
	TCGv r2 = tcg_temp_local_new();
	imm_32 = GET_IMM_32(ctx->opcode);
	///ld_imm = extract32(ctx->opcode, 17, 15);
	ld_imm = extract32(ctx->opcode, 16, 16);///
	///ld_imm = ld_imm << 1;

	//printf("\nimm_32: %x \n", imm_32);
	//printf("ld_imm: %x \n", ld_imm);

	gen_get_gpr(r1, rs1);
	gen_get_gpr(r2, rs2);


	switch(op){

		case OPC_RH850_LDB:			// LD.B
			printf("ldb 32 \n");
	        gen_load(ctx, MO_SB, rs2, rs1, ld_imm);
	    	break;

	    case OPC_RH850_LDH_LDW:		//
	    	if ( extract32(ctx->opcode, 16, 1) == 0 ){	// LD.H
	    		printf("ldh 32 \n");
	    		gen_load(ctx, MO_TESW, rs2, rs1, ld_imm);
	    	}
	    	else{
	    		printf("ldw 32 \n");
	    		gen_load(ctx, MO_TESL, rs2, rs1, ld_imm);		// LD.W
	    	}
	    	break;

	    case OPC_RH850_STB:			//this opcode is unique
	    	printf("stb 32\n");
	    	gen_store(ctx, MO_SB, rs1, rs2, (extract32(ctx->opcode, 16, 16)));
	    	break;

	    case OPC_RH850_STH_STW:		//only two instructions share this opcode
	    	if ( extract32(ctx->opcode, 16, 1)==1 ) {
	    		printf("stw 32\n");
	    		gen_store(ctx, MO_TESL, rs1, rs2, ((extract32(ctx->opcode, 17, 15)))<<1);
	    		//this is STORE WORD
	    		break;
	    	}
	    	printf("sth 32\n");
	    	gen_store(ctx, MO_TESW, rs1, rs2, ((extract32(ctx->opcode, 17, 15)))<<1);
	    	//this is STORE HALFWORD
	    	break;

	    case OPC_RH850_ADDI_imm16_reg1_reg2:
	    	gen_arithmetic(ctx, rs1,rs2, OPC_RH850_ADDI_imm16_reg1_reg2);
	    	break;

	    case OPC_RH850_ANDI_imm16_reg1_reg2:
	    	gen_logical(ctx, rs1, rs2, OPC_RH850_ANDI_imm16_reg1_reg2);
	    	break;

	    case OPC_RH850_MOVEA:
	    	if ( extract32(ctx->opcode, 11, 5) == 0 ){
	    		// This is 48bit MOV
	    		// This instruction should be reached first in decode_RH850_48
	    	} else {
	    		gen_arithmetic(ctx, rs1, rs2, OPC_RH850_MOVEA_imm16_reg1_reg2);
	    	}
	    	break;

	    case OPC_RH850_MOVHI_imm16_reg1_reg2:
	    	if(extract32(ctx->opcode, 11, 5)!=0x0){
	    		gen_arithmetic(ctx, rs1, rs2, OPC_RH850_MOVHI_imm16_reg1_reg2);
	    	} else {
	    		if(extract32(ctx->opcode, 16, 5)==0x0){
	    			gen_special(ctx, rs1, rs2, OPC_RH850_DISPOSE_imm5_list12);
	    		} else {
	    			gen_special(ctx, rs1, rs2, OPC_RH850_DISPOSE_imm5_list12_reg1);
	    		}
	    	}
	    	break;

	    case OPC_RH850_ORI_imm16_reg1_reg2:
	    	gen_logical(ctx, rs1, rs2, OPC_RH850_ORI_imm16_reg1_reg2);
	    	break;

	    case OPC_RH850_SATSUBI_imm16_reg1_reg2:
	    	if(extract32(ctx->opcode, 11, 5)!=0x0){
	    		gen_sat_op(ctx, rs1, rs2, OPC_RH850_SATSUBI_imm16_reg1_reg2);
			} else {
				if(extract32(ctx->opcode, 16, 5)==0x0){
					gen_special(ctx, rs1, rs2, OPC_RH850_DISPOSE_imm5_list12);
				} else {
					gen_special(ctx, rs1, rs2, OPC_RH850_DISPOSE_imm5_list12_reg1);
				}
			}

	    	break;
	    case OPC_RH850_XORI_imm16_reg1_reg2:
	    	gen_logical(ctx, rs1, rs2, OPC_RH850_XORI_imm16_reg1_reg2);
	    	break;

	    case OPC_RH850_LOOP:
	    	if (extract32(ctx->opcode, 11, 5) == 0x0){
	    		//loop
	    	} else {
	    		gen_multiply(ctx, rs1, rs2, OPC_RH850_MULHI_imm16_reg1_reg2);
	    	}

	    	break;
	    case OPC_RH850_CLR:
	    		//clr
	    	break;
		case OPC_RH850_32bit_1:		/* case for opcode = 11111 ; formats IX, X, XI, XII */
			if (extract32(ctx->opcode, 16, 1) == 0x1 ) { //if bit 16=1 its either b.cond or ld.hu
				if (rs2 == 0x0) {
					//this is BCOND2
					cond = extract32(ctx->opcode, 0, 4);
					imm_32 = (extract32(ctx->opcode, 4, 1) ||
							(extract32(ctx->opcode, 17, 15) << 1)) << 1;
					if((imm_32 & 0x10000) == 0x10000){	// check 17th bit if signed
						imm_32 |= (0x7fff << 17);
					}
					gen_branch(env, ctx, cond, rs1, rs2, imm_32);

					break;
				} else {
					//this is LD.HU
					printf("ldhu 32 \n");
					gen_load(ctx, MO_TEUW, rs2, rs1, ld_imm);
					break;
				}
			}
			formXop = MASK_OP_32BIT_SUB(ctx->opcode);		//sub groups based on bits b23-b26
			switch(formXop){
				case OPC_RH850_LDSR_RIE_SETF_STSR:
					check32bitZERO = extract32(ctx->opcode, 21, 2);
					switch(check32bitZERO){
					case 0:
						if(extract32(ctx->opcode, 4, 1)==1){
							gen_special(ctx, rs1, rs2, OPC_RH850_RIE);
						} else {
							gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SETF_cccc_reg2);
						}
						break;
					case OPC_RH850_LDSR_reg2_regID_selID:
						gen_special(ctx, rs1, rs2, OPC_RH850_LDSR_reg2_regID_selID);
						break;
					case OPC_RH850_STSR_regID_reg2_selID:
						gen_special(ctx, rs1, rs2, OPC_RH850_STSR_regID_reg2_selID);
						break;
					}
					break;
				case OPC_RH850_FORMAT_IX:		//format IX instructions
					formXop = MASK_OP_FORMAT_IX(ctx->opcode);	//mask on bits 21, 22
					switch(formXop){
					case OPC_RH850_BINS_0:
						if (extract32(ctx->opcode, 20, 1) == 1){
							//BINS0
							gen_data_manipulation(ctx, rs1, rs2, 123456);
							//printf("BINS0\n");
						}
						else{
							if (extract32(ctx->opcode, 17, 1) == 0){
								gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SHR_reg1_reg2);
							}else{
								gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SHR_reg1_reg2_reg3);
							}
						}
						break;
					case OPC_RH850_BINS_1:
						if (extract32(ctx->opcode, 20, 1) == 1){
							//BINS1
							gen_data_manipulation(ctx, rs1, rs2, 123456);
							printf("BINS1\n");
						}
						else{
							if (extract32(ctx->opcode, 17, 1) == 0){
								//SAR format IX
								gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SAR_reg1_reg2);
							}else{
								//SAR format XI
								gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SAR_reg1_reg2_reg3);
							}
						}
					break;
					case OPC_RH850_BINS_2:
						if (extract32(ctx->opcode, 20, 1) == 1){
							//BINS2
							gen_data_manipulation(ctx, rs1, rs2, 123456);
							printf("BINS2\n");
						}
						else{
							if (extract32(ctx->opcode, 17, 1) == 0){
								if (extract32(ctx->opcode, 18, 1) == 1){
									gen_data_manipulation(ctx, rs1, rs2,
											OPC_RH850_ROTL_imm5_reg2_reg3);
								}
								else{
									gen_data_manipulation(ctx, rs1, rs2,
											OPC_RH850_SHL_reg1_reg2);
								}
							}else{
								if (extract32(ctx->opcode, 18, 1) == 1){
									gen_data_manipulation(ctx, rs1, rs2,
											OPC_RH850_ROTL_reg1_reg2_reg3);
								}
								else{
									gen_data_manipulation(ctx, rs1, rs2,
											OPC_RH850_SHL_reg1_reg2_reg3);
								}
							}
						}
						break;
					case OPC_RH850_CLR1:
						check32bitZERO = extract32(ctx->opcode, 16, 3);
						switch(check32bitZERO){
						case 0:
							//SET
							break;
						case 2:
							//NOT1
							break;
						case 4:
							//CLR1
							break;
						case 6:
							if (extract32(ctx->opcode, 19, 1) == 0){
								//TST1
							} else {
								gen_special(ctx, rs1, rs2, OPC_RH850_CAXI_reg1_reg2_reg3);
							}
						}
						break;
					}
					break;
				case OPC_RH850_FORMAT_X:		//format X instructions
												//(+JARL3 - added due to MASK_OP_FORMAT_X matching)
					formXop = MASK_OP_FORMAT_X(ctx->opcode);
					switch(formXop){
						case OPC_RH850_CLL_CACHE:
							if ((extract32(ctx->opcode, 27, 5) == 0x1E) &&
									(extract32(ctx->opcode, 0, 5) == 0x1F)){
								//CLL
							} else {
								//CACHE; if cacheop bits are 1111110, opcode matches CLL ins,
								//then they are THE SAME instruction, so this should be correct
							}
							break;
						case OPC_RH850_CTRET:
							gen_special(ctx, rs1, rs2, OPC_RH850_CTRET);
							break;
						case OPC_RH850_DI:
							gen_special(ctx, rs1, rs2, OPC_RH850_DI);
							break;
						case OPC_RH850_EI:
							gen_special(ctx, rs1, rs2, OPC_RH850_EI);
							break;
						case OPC_RH850_EIRET:
							gen_special(ctx, rs1, rs2, OPC_RH850_EIRET);
							break;
						case OPC_RH850_FERET:
							gen_special(ctx, rs1, rs2, OPC_RH850_FERET);
							break;
						case OPC_RH850_HALT:
							gen_special(ctx, rs1, rs2, OPC_RH850_HALT);
							break;
						case OPC_RH850_JARL3:
							//gen_branch(ctx, 0, rs1, rs2, OPC_RH850_JARL_reg1_reg3);
							break;
						case OPC_RH850_SNOOZE:
							gen_special(ctx, rs1, rs2, OPC_RH850_SNOOZE);
							break;
						case OPC_RH850_SYSCALL:
							gen_special(ctx, rs1, rs2, OPC_RH850_SYSCALL);
							break;
						case OPC_RH850_TRAP:
							gen_special(ctx, rs1, rs2, OPC_RH850_TRAP);
							break;
						case OPC_RH850_PREF:
							break;
						case OPC_RH850_POPSP:
							break;
						case OPC_RH850_PUSHSP:
							break;

					}
					break;
				case OPC_RH850_MUL_INSTS:
					if (extract32(ctx->opcode, 22, 1) == 0){
						if (extract32(ctx->opcode, 21, 1) == 0){
							gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SASF_cccc_reg2);
						} else {
							if (extract32(ctx->opcode, 17, 1) == 1){
								gen_multiply(ctx, rs1, rs2, OPC_RH850_MULU_reg1_reg2_reg3);
							} else {
								gen_multiply(ctx, rs1, rs2, OPC_RH850_MUL_reg1_reg2_reg3);
							}
						}
						break;
					} else if (extract32(ctx->opcode, 22, 1) == 1){
						if (extract32(ctx->opcode, 17, 1) == 1){
							gen_multiply(ctx, rs1, rs2, OPC_RH850_MULU_imm9_reg2_reg3);
						} else {
							gen_multiply(ctx, rs1, rs2, OPC_RH850_MUL_imm9_reg2_reg3);
						}
						break;
					}
					break;

				case OPC_RH850_FORMAT_XI:			// DIV instructions in format XI
					formXop = extract32(ctx->opcode, 16, 7);
					switch(formXop){

						case OPC_RH850_DIV_reg1_reg2_reg3:
							gen_divide(ctx, rs1, rs2, OPC_RH850_DIV_reg1_reg2_reg3);
							//DIV
							break;
						case OPC_RH850_DIVH_reg1_reg2_reg3:
							gen_divide(ctx, rs1, rs2, OPC_RH850_DIVH_reg1_reg2_reg3);
							//DIVH 2
							break;
						case OPC_RH850_DIVHU_reg1_reg2_reg3:
							gen_divide(ctx, rs1, rs2, OPC_RH850_DIVHU_reg1_reg2_reg3);
							//DIVHU
							break;

						case OPC_RH850_DIVQ:
							//DIVQ
							break;
						case OPC_RH850_DIVQU:
							//DIVQU
							break;
						case OPC_RH850_DIVU_reg1_reg2_reg3:
							gen_divide(ctx, rs1, rs2, OPC_RH850_DIVU_reg1_reg2_reg3);
							//DIVU
							break;
					}
					break;

				case OPC_RH850_FORMAT_XII:	// for opcode = 0110 ; format XII instructions
											//excluding MUL and including CMOV
											// add LDL.W and STC.W!!!
					checkXII = extract32(ctx->opcode, 21, 2);

					switch(checkXII){
					case 0:
						gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_CMOV_cccc_imm5_reg2_reg3);
						break;
					case 1:
						gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_CMOV_cccc_reg1_reg2_reg3);
						break;
					case 2:
						formXop = extract32(ctx->opcode, 17, 2);

						switch(formXop){
						case OPC_RH850_BSW_reg2_reg3:
							gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_BSW_reg2_reg3);
							break;
						case OPC_RH850_BSH_reg2_reg3:
							gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_BSH_reg2_reg3);
							break;
						case OPC_RH850_HSW_reg2_reg3:
							//HSW
							gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_HSW_reg2_reg3);
							break;
						case OPC_RH850_HSH_reg2_reg3:
							//HSH
							gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_HSH_reg2_reg3);
							break;
						}
						break;
					case 3:	//these are SCHOL, SCHOR, SCH1L, SCH1R
						formXop = extract32(ctx->opcode, 17, 2);
						switch(formXop){
						case OPC_RH850_SCH0R_reg2_reg3:
							//printf("LDL.W is here \n");
							gen_bit_search(ctx, rs2, OPC_RH850_SCH0R_reg2_reg3);
							break;
						case OPC_RH850_SCH1R_reg2_reg3:
							if (extract32(ctx->opcode, 19, 2) == 0x0){
								gen_bit_search(ctx, rs2, OPC_RH850_SCH1R_reg2_reg3);
							} else if (extract32(ctx->opcode, 19, 2) == 0x3){
								//STC.W
							}
							break;
						case OPC_RH850_SCH0L_reg2_reg3:
							gen_bit_search(ctx, rs2, OPC_RH850_SCH0L_reg2_reg3);
							break;
						case OPC_RH850_SCH1L_reg2_reg3:
							gen_bit_search(ctx, rs2, OPC_RH850_SCH1L_reg2_reg3);
							break;
						}

					}
					break;

				case OPC_RH850_ADDIT_ARITH:
					formXop = extract32(ctx->opcode, 21, 2);
					switch(formXop){

						case OPC_RH850_ADF_SATADD3:
							if (extract32(ctx->opcode, 16, 5) == 0x1A){
								gen_sat_op(ctx, rs1, rs2, OPC_RH850_SATADD_reg1_reg2_reg3);
							} else {
								gen_cond_arith(ctx, rs1, rs2, OPC_RH850_ADF_cccc_reg1_reg2_reg3);
							}
							break;
						case OPC_RH850_SBF_SATSUB:
							if (extract32(ctx->opcode, 16, 5) == 0x1A){
								gen_sat_op(ctx, rs1, rs2, OPC_RH850_SATSUB_reg1_reg2_reg3);
							} else {
								gen_cond_arith(ctx, rs1, rs2, OPC_RH850_SBF_cccc_reg1_reg2_reg3);
							}
							break;
							break;
						case OPC_RH850_MAC_reg1_reg2_reg3_reg4:
							gen_mul_accumulate(ctx, rs1, rs2, OPC_RH850_MAC_reg1_reg2_reg3_reg4);
							break;
						case OPC_RH850_MACU_reg1_reg2_reg3_reg4:
							gen_mul_accumulate(ctx, rs1, rs2, OPC_RH850_MACU_reg1_reg2_reg3_reg4);
							break;
					}
			}
	}

	if (MASK_OP_FORMAT_V_FORMAT_XIII(ctx->opcode) == OPC_RH850_FORMAT_V_XIII){

		if(extract32(ctx->opcode, 16, 1) == 0){

			if (extract32(ctx->opcode, 11, 5) == 0){
				//gen_branch(ctx, 0, rs1, rs2, OPC_RH850_JR_imm22);
				//JR
			} else {
				//gen_branch(ctx, 0, rs1, rs2, OPC_RH850_JARL_disp22_reg2);
				//JARL1


			}
		}else{
			if (extract32(ctx->opcode, 11, 5) != 0){
				//LD.BU
				printf("ld.bu \n");
				gen_load(ctx, MO_TEUW, rs2, rs1, (ld_imm));
			}else{
				if (extract32(ctx->opcode, 16, 3) == 0x3){
					//PREPARE2
				}
				 else if (extract32(ctx->opcode, 16, 3) == 0x1){
					 //PREPARE1
				 }
			}
		}
	}

}

static void decode_RH850_16(CPURH850State *env, DisasContext *ctx)
{
	int rs1;
	int rs2;
	int cond;
	uint32_t op;
	uint32_t subOpCheck;
	uint32_t imm;

	op = MASK_OP_MAJOR(ctx->opcode);
	rs1 = GET_RS1(ctx->opcode);			// rs1 at bits b0-b4;
	rs2 = GET_RS2(ctx->opcode);			// rs2 at bits b11-b15;
	imm = rs1;

	if((op & 0xf << 7) == OPC_RH850_BCOND ){ // checking for 4 bit opcode for BCOND
		cond = extract32(ctx->opcode, 0, 4);
		imm = ( extract32(ctx->opcode, 4, 3) | (extract32(ctx->opcode, 11, 5) << 3)) << 1 ;

		if ( (imm & 0x100) == 0x100){
			imm |=  (0x7fffff << 9);
		}
		gen_branch(env, ctx, cond, rs1, rs2, imm);

		return;
	}

	switch(op){
	case OPC_RH850_16bit_0:
		if (rs2 != 0) {
			gen_arithmetic(ctx, rs1, rs2, OPC_RH850_MOV_reg1_reg2);
			break;
		} else {
			subOpCheck = MASK_OP_FORMAT_I_0(op);
			switch(subOpCheck){
				case OPC_RH850_NOP:
					break;
				case OPC_RH850_SYNCI:
					break;
				case OPC_RH850_SYNCE:
					break;
				case OPC_RH850_SYNCM:
					break;
				case OPC_RH850_SYNCP:
					break;
			}
		}
		break;

	case OPC_RH850_16bit_2:
		if (rs2 == 0){
			if (rs1 == 0){
				gen_special(ctx, rs1, rs2, OPC_RH850_RIE);
				break;
			} else {
				gen_special(ctx, rs1, rs2, OPC_RH850_SWITCH_reg1);
				break;
			}
		} else {
			if (rs1 == 0){
				gen_special(ctx, rs1, rs2, OPC_RH850_FETRAP_vector4);
				break;
			} else {
				gen_divide(ctx, rs1, rs2, OPC_RH850_DIVH_reg1_reg2);
				break;
			}
		}
		break;

	case OPC_RH850_16bit_4:
		if (rs2 == 0){
			gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_ZXB_reg1);
			break;
		} else {
			gen_sat_op(ctx, rs1, rs2, OPC_RH850_SATSUBR_reg1_reg2);
			break;
		}
		break;
	case OPC_RH850_16bit_5:
		if (rs2 == 0){
			gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SXB_reg1);
			break;
		} else {
			gen_sat_op(ctx, rs1, rs2, OPC_RH850_SATSUB_reg1_reg2);
			break;
		}
		break;
	case OPC_RH850_16bit_6:
		if (rs2 == 0){
			gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_ZXH_reg1);
			break;
		} else {
			gen_sat_op(ctx, rs1, rs2, OPC_RH850_SATADD_reg1_reg2);
			break;
		}
		break;
	case OPC_RH850_16bit_7:
		if (rs2 == 0){
			gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SXH_reg1);
			break;
		} else {
			gen_multiply(ctx, rs1, rs2, OPC_RH850_MULH_reg1_reg2);
			break;
		}
		break;
	case OPC_RH850_NOT_reg1_reg2:
		gen_logical(ctx, rs1, rs2, OPC_RH850_NOT_reg1_reg2);
		break;
	case OPC_RH850_16bit_3:
		if (rs2 == 0){
			gen_jmp(ctx, rs1, rs2, 0);
			//JMP
			break;
		} else {
			if(extract32(rs1,4,1)==1){
				//SLD.HU
				printf("sld.hu \n");
			}else{
				//SLD.BU
				printf("sld.bu \n");
			}
			break;
		}
		break;
	case OPC_RH850_OR_reg1_reg2:
		gen_logical(ctx, rs1, rs2, OPC_RH850_OR_reg1_reg2);
		break;
	case OPC_RH850_XOR_reg1_reg2:
		gen_logical(ctx, rs1, rs2, OPC_RH850_XOR_reg1_reg2);
		break;
	case OPC_RH850_AND_reg1_reg2:
		gen_logical(ctx, rs1, rs2, OPC_RH850_AND_reg1_reg2);
		break;
	case OPC_RH850_TST_reg1_reg2:
		gen_logical(ctx, rs1, rs2, OPC_RH850_TST_reg1_reg2);
		break;
	case OPC_RH850_SUBR_reg1_reg2:
		gen_arithmetic(ctx, rs1, rs2, OPC_RH850_SUBR_reg1_reg2);
		break;
	case OPC_RH850_SUB_reg1_reg2:
		gen_arithmetic(ctx, rs1, rs2, OPC_RH850_SUB_reg1_reg2);
		break;
	case OPC_RH850_ADD_reg1_reg2:
		gen_arithmetic(ctx, rs1, rs2, OPC_RH850_ADD_reg1_reg2);
		break;
	case OPC_RH850_CMP_reg1_reg2:
		gen_arithmetic(ctx, rs1, rs2, OPC_RH850_CMP_reg1_reg2);
		break;
	case OPC_RH850_16bit_16:
		if (rs2 == 0){
			gen_special(ctx, rs1, rs2, OPC_RH850_CALLT_imm6);
			break;
		} else {
			gen_arithmetic(ctx, imm, rs2, OPC_RH850_MOV_imm5_reg2);
			break;
		}
		break;
	case OPC_RH850_16bit_17:
		if (rs2 == 0){
			gen_special(ctx, rs1, rs2, OPC_RH850_CALLT_imm6);
			break;
		} else {
			gen_sat_op(ctx, rs1, rs2, OPC_RH850_SATADD_imm5_reg2);
			break;
		}
		break;
	case OPC_RH850_ADD_imm5_reg2:
		gen_arithmetic(ctx, rs1, rs2, OPC_RH850_ADD_imm5_reg2);
		break;
	case OPC_RH850_CMP_imm5_reg2:
		gen_arithmetic(ctx, rs1, rs2, OPC_RH850_CMP_imm5_reg2);
		break;
	case OPC_RH850_SHR_imm5_reg2:
		gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SHR_imm5_reg2);
		break;
	case OPC_RH850_SAR_imm5_reg2:
		gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SAR_imm5_reg2);
		break;
	case OPC_RH850_SHL_imm5_reg2:
		gen_data_manipulation(ctx, rs1, rs2, OPC_RH850_SHL_imm5_reg2);
		break;
	case OPC_RH850_MULH_imm5_reg2:
		gen_multiply(ctx, rs1, rs2, OPC_RH850_MULH_imm5_reg2);
		break;
	}

	//Format IV ; dividing on code bits b7-b10
	uint32_t opIV = op>>2;

	switch(opIV){
	case OPC_RH850_16bit_SLDB:
		printf("sld.b \n");
		break;
	case OPC_RH850_16bit_SLDH:
		printf("sld.h \n");
		break;
	case OPC_RH850_16bit_IV10:
		if(extract32(rs1,0,1)==1){
			//SST.W
			printf("sst.w \n");
		}
		else{
			//SLD.H
			printf("sld.h \n");
		}
		break;
	case OPC_RH850_16bit_SSTB:
		printf("sst.b \n");
		break;
	case OPC_RH850_16bit_SSTH:
		printf("sst.h \n");
		break;
	}
}

void gen_intermediate_code(CPUState *cs, TranslationBlock *tb)
{
    CPURH850State *env = cs->env_ptr;
    DisasContext ctx;
    target_ulong pc_start;
    target_ulong next_page_start;
    int num_insns;
    int max_insns;
    pc_start = tb->pc;
    next_page_start = (pc_start & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE;
    ctx.pc = pc_start;


    /* once we have GDB, the rest of the translate.c implementation should be
       ready for singlestep */
    ///ctx.singlestep_enabled = cs->singlestep_enabled;
    ctx.singlestep_enabled = 1;///

    ctx.tb = tb;
    ctx.bstate = BS_NONE;
    ctx.flags = tb->flags;
    ctx.mem_idx = 0;
    ctx.frm = -1;  /* unknown rounding mode */

    num_insns = 0;
    max_insns = tb->cflags & CF_COUNT_MASK;
    if (max_insns == 0) {
        max_insns = CF_COUNT_MASK;
    }
    if (max_insns > TCG_MAX_INSNS) {
        max_insns = TCG_MAX_INSNS;
    }
    gen_tb_start(tb);

    while (ctx.bstate == BS_NONE) {
        tcg_gen_insn_start(ctx.pc);
        num_insns++;

        if (unlikely(cpu_breakpoint_test(cs, ctx.pc, BP_ANY))) {
            tcg_gen_movi_tl(cpu_pc, ctx.pc);
            ctx.bstate = BS_BRANCH;
            gen_exception_debug();
            /* The address covered by the breakpoint must be included in
               [tb->pc, tb->pc + tb->size) in order to for it to be
               properly cleared -- thus we increment the PC here so that
               the logic setting tb->size below does the right thing.  */
            ctx.pc += 4;
            goto done_generating;
        }

        if (num_insns == max_insns && (tb->cflags & CF_LAST_IO)) {
            gen_io_start();
        }

        ctx.opcode = cpu_lduw_code(env, ctx.pc);

        if ((extract32(ctx.opcode, 9, 2) != 0x3) && (extract32(ctx.opcode, 5, 11) != 0x17)){
			ctx.next_pc = ctx.pc + 2;		//16 bit instructions
			decode_RH850_16(env, &ctx);		//this function includes JR and JARL (32-bit FORMAT VI)
        } else {
        	ctx.opcode = (ctx.opcode) | (cpu_lduw_code(env, ctx.pc+2) << 0x10);
        	if ( (extract32(ctx.opcode, 6, 11) == 0x41e) ||	// 48 bit instructions
			(extract32(ctx.opcode, 5, 11) == 0x31) ||	// MOVE3
			(extract32(ctx.opcode, 5, 12) == 0x37)  || //this fits for JMP2(48-bit)
			(extract32(ctx.opcode, 5, 11) == 0x17) ) { //this is for 48bit JARL and JR (format VI)

        		ctx.opcode1 = cpu_lduw_code(env, ctx.pc+4);
				ctx.next_pc = ctx.pc + 6;
				decode_RH850_48(env, &ctx);
        	} else {
        		ctx.next_pc = ctx.pc + 4;
        		decode_RH850_32(env, &ctx);
        	}
        }

        ctx.pc = ctx.next_pc;

        // Setting PSW to 0 so we can write new state

        tcg_gen_movi_i32(cpu_sysRegs[PSW_register], 0x0);

        // Writing flag values to PSW register

        TCGv temp = tcg_temp_new_i32();

        tcg_gen_or_i32(cpu_sysRegs[PSW_register],cpu_sysRegs[PSW_register],cpu_ZF);

        tcg_gen_shli_i32(temp, cpu_SF, 0x1);
        tcg_gen_or_i32(cpu_sysRegs[PSW_register],cpu_sysRegs[PSW_register],temp);

        tcg_gen_shli_i32(temp, cpu_OVF, 0x2);
        tcg_gen_or_i32(cpu_sysRegs[PSW_register],cpu_sysRegs[PSW_register],temp);

        tcg_gen_shli_i32(temp, cpu_CYF, 0x3);
        tcg_gen_or_i32(cpu_sysRegs[PSW_register],cpu_sysRegs[PSW_register],temp);

        tcg_gen_shli_i32(temp, cpu_SATF, 0x4);
        tcg_gen_or_i32(cpu_sysRegs[PSW_register],cpu_sysRegs[PSW_register],temp);

        if (cs->singlestep_enabled) {
            break;
        }
        if (ctx.pc >= next_page_start) {
            break;
        }
        if (tcg_op_buf_full()) {
            break;
        }
        if (num_insns >= max_insns) {
            break;
        }
        if (singlestep) {
            break;
        }

    }
    if (tb->cflags & CF_LAST_IO) {
        gen_io_end();
    }
    switch (ctx.bstate) {
    case BS_STOP:
        gen_goto_tb(&ctx, 0, ctx.pc);
        break;
    case BS_NONE: /* handle end of page - DO NOT CHAIN. See gen_goto_tb. */
        tcg_gen_movi_tl(cpu_pc, ctx.pc);
        if (cs->singlestep_enabled) {
            gen_exception_debug();
        } else {
            tcg_gen_exit_tb(0);
        }
        break;
    case BS_BRANCH: /* ops using BS_BRANCH generate own exit seq */
    default:
        break;
    }
done_generating:
    gen_tb_end(tb, num_insns);
    tb->size = ctx.pc - pc_start;
    tb->icount = num_insns;

#ifdef DEBUG_DISAS
    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)
        && qemu_log_in_addr_range(pc_start)) {
        qemu_log("IN: %s\n", lookup_symbol(pc_start));
        log_target_disas(cs, pc_start, ctx.pc - pc_start);
        qemu_log("\n");
    }
#endif
}

void rh850_translate_init(void)
{
    int i;

    /* cpu_gpr[0] is a placeholder for the zero register. Do not use it. */
    /* Use the gen_set_gpr and gen_get_gpr helper functions when accessing */
    /* registers, unless you specifically block reads/writes to reg 0 */
    cpu_gpr[0] = NULL;


    for (i = 1; i < 32; i++) {
        cpu_gpr[i] = tcg_global_mem_new(cpu_env,
            offsetof(CPURH850State, progRegs[i]), rh850_prog_regnames[i]);
    }


    for (i = 0; i < 31; i++) {

        cpu_sysRegs[i] = tcg_global_mem_new(cpu_env,
            offsetof(CPURH850State, sysBasicRegs[i]), rh850_sys_basic_regnames[i]);
    }

    for (i = 0; i < 5; i++) {
    	cpu_sysIntrRegs[i] = tcg_global_mem_new(cpu_env,
            offsetof(CPURH850State, sysInterruptRegs[i]), rh850_sys_intr_regnames[i]);
    }

    for (i = 0; i < 6; i++) {
        cpu_fpr[i] = tcg_global_mem_new_i64(cpu_env,
            offsetof(CPURH850State, sysFpuRegs[i]), rh850_sys_fpr_regnames[i]);
    }

    for (i = 0; i < 56; i++) {
        cpu_sysMpuRegs[i] = tcg_global_mem_new(cpu_env,
            offsetof(CPURH850State, sysMpuRegs[i]), rh850_sys_mpu_regnames[i]);
    }

    for (i = 0; i < 7; i++) {
        cpu_sysCacheRegs[i] = tcg_global_mem_new(cpu_env,
            offsetof(CPURH850State, sysCacheRegs[i]), rh850_sys_cacheop_regnames[i]);
    }

    for (i = 0; i < 1; i++) {
        cpu_sysDatabuffRegs[i] = tcg_global_mem_new(cpu_env,
            offsetof(CPURH850State, sysDatabuffRegs[i]), rh850_sys_databuff_regnames[i]);
    }

    // PSW register flags
    cpu_ZF = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, Z_flag), "ZF");
    cpu_SF = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, S_flag), "SF");
	cpu_OVF = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, OV_flag), "OVF");
	cpu_CYF = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, CY_flag), "CYF");
	cpu_SATF = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, SAT_flag), "SAT");
	cpu_ID = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, ID_flag), "ID");
    cpu_EP = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, EP_flag), "EP");
    cpu_NP = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, NP_flag), "NP");
    cpu_EBV = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, EBV_flag), "EBV");
    cpu_CU0 = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, CU0_flag), "CU0");
    cpu_CU1 = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, CU1_flag), "CU1");
    cpu_CU2 = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, CU2_flag), "CU2");
    cpu_UM = tcg_global_mem_new_i32(cpu_env, offsetof(CPURH850State, UM_flag), "UM");




    cpu_pc = tcg_global_mem_new(cpu_env, offsetof(CPURH850State, pc), "pc");
    load_res = tcg_global_mem_new(cpu_env, offsetof(CPURH850State, load_res),
                             "load_res");
    load_val = tcg_global_mem_new(cpu_env, offsetof(CPURH850State, load_val),
                             "load_val");
}
