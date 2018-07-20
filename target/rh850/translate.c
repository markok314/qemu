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
static TCGv cpu_gpr[32], cpu_pc, cpu_sysRegs[30], cpu_sysIntrRegs[5], cpu_sysMpuRegs[56], cpu_sysCacheRegs[7], cpu_sysDatabuffRegs[1];
static TCGv_i64 cpu_fpr[32]; /* assume F and D extensions */
static TCGv load_res;
static TCGv load_val;

#include "exec/gen-icount.h"

typedef struct DisasContext {
    struct TranslationBlock *tb;
    target_ulong pc;
    target_ulong next_pc;
    uint32_t opcode;
    uint32_t flags;
    uint32_t mem_idx;
    int singlestep_enabled;
    int bstate;
    /* Remember the rounding mode encoded in the previous fp instruction,
       which we have already installed into env->fp_status.  Or -1 for
       no previous fp instruction.  Note that we exit the TB when writing
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
#define CASE_OP_32_64(X) case X

static void generate_exception(DisasContext *ctx, int excp)
{
    tcg_gen_movi_tl(cpu_pc, ctx->pc);
    TCGv_i32 helper_tmp = tcg_const_i32(excp);
    gen_helper_raise_exception(cpu_env, helper_tmp);
    tcg_temp_free_i32(helper_tmp);
    ctx->bstate = BS_BRANCH;
}



static void gen_exception_debug(void)
{
    TCGv_i32 helper_tmp = tcg_const_i32(EXCP_DEBUG);
    gen_helper_raise_exception(cpu_env, helper_tmp);
    tcg_temp_free_i32(helper_tmp);
}

static void gen_exception_illegal(DisasContext *ctx)
{
    generate_exception(ctx, RH850_EXCP_ILLEGAL_INST);
}


static inline bool use_goto_tb(DisasContext *ctx, target_ulong dest)
{
    if (unlikely(ctx->singlestep_enabled)) {
        return false;
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


static void gen_load(DisasContext *ctx, int memop, int rd, int rs1, target_long imm)
{
    TCGv t0 = tcg_temp_new();
    TCGv t1 = tcg_temp_new();
    gen_get_gpr(t0, rs1);
    tcg_gen_addi_tl(t0, t0, imm);

    if (memop < 0) {
        gen_exception_illegal(ctx);
        return;
    }

    tcg_gen_qemu_ld_tl(t1, t0, ctx->mem_idx, memop);
    gen_set_gpr(rd, t1);
    tcg_temp_free(t0);
    tcg_temp_free(t1);
}

static void gen_store(DisasContext *ctx, int memop, int rs1, int rs2,    //make cases for opcodes
        target_long imm)
{
    TCGv t0 = tcg_temp_new();		//temp
    TCGv dat = tcg_temp_new();		//temp
    gen_get_gpr(t0, rs1);			//loading rs1 to t0
    tcg_gen_addi_tl(t0, t0, imm);	//adding displacement to t0
    gen_get_gpr(dat, rs2);			//getting data from rs2
    //int memop = tcg_memop_lookup[(opc >> 12) & 0x7]; 		//pass memop in arguments?

    if (memop < 0) {
        gen_exception_illegal(ctx);
        return;
    }

    tcg_gen_qemu_st_tl(dat, t0, ctx->mem_idx, memop);
    tcg_temp_free(t0);
    tcg_temp_free(dat);
}

static void decode_load_store_0(CPURH850State *env, DisasContext *ctx)
{
	int rs1;
	int rs3;
	target_long disp;
	uint32_t op;

	op = MASK_OP_ST_LD0(ctx->opcode);
	rs1 = GET_RS1(ctx->opcode);
	rs3 = GET_RS3(ctx->opcode);
	disp = GET_DISP(ctx->opcode);

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

static void decode_arithmetic(DisasContext *ctx, int memop, int rs1, int rs2, int operation)
{
	TCGv r1 = tcg_temp_new();		//temp
	TCGv r2 = tcg_temp_new();		//temp
	gen_get_gpr(r1, rs1);			//loading rs1 to t0
	gen_get_gpr(r2, rs2);			//loading rs2 to t1
	int imm = rs1;
	int imm_16;
	long imm_323;
	int imm_32;
	int int_rs3;
	TCGv tcg_imm = tcg_temp_new();
	TCGv tcg_imm32 = tcg_temp_new();

	TCGv tcg_r3 = tcg_temp_new();
	TCGv tcg_temp = tcg_temp_new();

	switch(operation) {
		case 0:
			tcg_gen_mov_tl(r2, r1); // MOV (Format 1)
			gen_set_gpr(rs2, r2);
			break;
		case 1:
			tcg_gen_add_tl(r2, r2, r1); //ADD FORMAT 1
			gen_set_gpr(rs2, r2);
			break;
		case 2:
			tcg_gen_sub_tl(r2, r2, r1);	//SUB
			gen_set_gpr(rs2, r2);
			break;
		case 3:
			tcg_gen_sub_tl(r2, r1, r2);	//SUBR
			gen_set_gpr(rs2, r2);
			break;
		case 4:
			tcg_gen_or_tl(r2, r2, r1); //OR
			gen_set_gpr(rs2, r2);
			break;
		case 5:
			tcg_gen_xor_tl(r2, r2, r1);	//XOR
			gen_set_gpr(rs2, r2);
			break;
		case 6:
			tcg_gen_addi_tl(r2, r2, imm); //ADD FORMAT 2 IMM
			gen_set_gpr(rs2, r2);
			break;
		case 7:
			tcg_gen_and_tl(r2, r2, r1); //AND
			gen_set_gpr(rs2, r2);
			break;
		case 8://MULH
			tcg_gen_andi_tl(r2, r2,0x0000FFFF);
			tcg_gen_andi_tl(r1, r1,0x0000FFFF);
			tcg_gen_mul_tl(r2, r2, r1);
			gen_set_gpr(rs2, r2);
			break;
		case 9://MULH FORMAT 2 IMM
			tcg_gen_movi_tl(tcg_imm, imm);
			tcg_gen_andi_tl(tcg_imm, tcg_imm,0x0000FFFF);
			tcg_gen_andi_tl(r1, r1,0x0000FFFF);
			tcg_gen_mul_tl(r2, tcg_imm, r1);
			gen_set_gpr(rs2, r2);
			break;
		case 10://SXH
			tcg_gen_ext16s_tl(r1, r1);
			gen_set_gpr(rs1, r1);
			break;
		case 11://NOT
			tcg_gen_not_i32(r2, r1);
			gen_set_gpr(rs2, r2);
			break;
		case 12://DIVH
			tcg_gen_andi_i32(r1, r1,0x0000ffff);
			tcg_gen_div_i32(r2, r2, r1);
			gen_set_gpr(rs2, r2);
			break;
		case 13: //ADDI
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_ext32s_tl(tcg_imm32, tcg_imm32); //SIGN EXTETEND IMM
			tcg_gen_addi_tl(r2,r1, imm_32);
			gen_set_gpr(rs2, r2);
			break;
		case 14: //ADF
			int_rs3 = extract32(ctx->opcode, 26, 5);
			//int_cond = extract32(ctx->opcode, 17, 4);
			gen_get_gpr(tcg_r3,int_rs3);
			//if condition then
			tcg_gen_addi_tl(r2, r2, 1);
			tcg_gen_add_tl(tcg_r3, r2, r1);
			//else
			tcg_gen_add_tl(tcg_r3, r2, r1);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 15://ANDI
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_ext32s_tl(tcg_imm32, tcg_imm32); //SIGN EXTETEND IMM
			tcg_gen_and_tl(r2, r1, tcg_imm32);
			gen_set_gpr(rs2, r2);
			break;
		case 16:
			tcg_gen_movi_tl(r2, imm); // MOV imm. Format 2
			gen_set_gpr(rs2, r2);
			break;
		case 17://ANDI
			imm_16 = extract32(ctx->opcode, 16, 12);
			tcg_gen_andi_i32(r2, r1, imm_16);
			gen_set_gpr(rs2, r2);
			break;
		case 18://MOV3   ---  48bit instruction
			imm_323 = extract64(ctx->opcode, 16, 32);
			//tcg_gen_movi_tl(tcg_imm32, imm_32);
			//tcg_gen_ext32s_tl(tcg_imm32, tcg_imm32);
			printf("This is 32bit immediate: %x", ctx->opcode);
			tcg_gen_movi_i32(r2, imm_323);
			gen_set_gpr(rs2, r2);
			break;
		case 19: //MUL FORMAT XI
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_mul_tl(r2, r2, r1);

			// R3(higher 32 bits) IN R2(lower 32 bits)
			tcg_gen_addi_tl(tcg_temp, tcg_temp, 32);
			tcg_gen_sar_tl(tcg_r3, r2,tcg_temp);
			tcg_gen_andi_tl(r2, r2,0xFFFFFFFF);
			gen_set_gpr(int_rs3,tcg_r3);
			gen_set_gpr(rs2, r2);
			break;
		case 20: //MUL FORMAT XII
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			imm_32 = extract32(ctx->opcode, 19, 4);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_ext32s_tl(tcg_imm32, tcg_imm32); //SIGN EXTETEND IMM
			tcg_gen_mul_tl(r2, r2, tcg_imm32);

			// R3(higher 32 bits) IN R2(lower 32 bits)
			tcg_gen_addi_tl(tcg_temp, tcg_temp, 32);
			tcg_gen_sar_tl(tcg_r3, r2,tcg_temp);
			tcg_gen_andi_tl(r2, r2,0xFFFFFFFF);
			gen_set_gpr(int_rs3,tcg_r3);
			break;
		case 21: //MULHI
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_andi_tl(tcg_temp, r1,0xFFFF); //GET LOWER 15 BITS OF R1
			tcg_gen_mul_tl(r2, tcg_temp, tcg_imm32);
			gen_set_gpr(rs2, r2);
			break;
		case 22://MULU FORMAT XI
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_mul_tl(r2, r2, r1);

			// R3(higher 32 bits) IN R2(lower 32 bits)
			tcg_gen_addi_tl(tcg_temp, tcg_temp, 32);
			tcg_gen_shr_tl(tcg_r3, r2,tcg_temp);
			tcg_gen_andi_tl(r2, r2,0xFFFFFFFF);
			gen_set_gpr(int_rs3,tcg_r3);
			gen_set_gpr(rs2, r2);
			break;
		case 23://MULU FORMAT XII
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			imm_32 = extract32(ctx->opcode, 19, 4);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_ext32u_tl(tcg_imm32, tcg_imm32); //ZERO EXTENDED IMM
			tcg_gen_mul_tl(r2, r2, tcg_imm32);

			// R3(higher 32 bits) IN R2(lower 32 bits)
			tcg_gen_addi_tl(tcg_temp, tcg_temp, 32);
			tcg_gen_sar_tl(tcg_r3, r2,tcg_temp);
			tcg_gen_andi_tl(r2, r2,0xFFFFFFFF);
			gen_set_gpr(int_rs3,tcg_r3);
			gen_set_gpr(rs2, r2);
			break;

		case 24: //SATADD FORMAT I
			tcg_gen_add_tl(r2, r1, r2);
			//TODO:SATURED TO 7FFFFFFFH OR 80000000H
			gen_set_gpr(rs2, r2);
			break;
		case 25: //SATADD FORMAT II
			tcg_gen_movi_tl(tcg_imm32, imm);
			tcg_gen_ext32s_tl(tcg_imm32, tcg_imm32); //SIGN EXTETEND IMM
			tcg_gen_addi_tl(r2, r1, imm);
			//TODO:SATURED TO 7FFFFFFFH OR 80000000H
			gen_set_gpr(rs2, r2);
			break;
		case 26: //SATADD FORMAT XI
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_add_tl(tcg_r3, r1, r2);
			//TODO:SATURED TO 7FFFFFFFH OR 80000000H

			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 27: //SATSUB FORMAT I
			tcg_gen_sub_tl(r2, r2, r1);
			//TODO:SATURED TO 7FFFFFFFH OR 80000000H
			gen_set_gpr(rs2, r2);
			break;
		case 28://SATSUB FORMAT XI
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_sub_tl(tcg_r3, r2, r1);
			//TODO:SATURED TO 7FFFFFFFH OR 80000000H

			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 29: //SATSUBI
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_ext32s_tl(tcg_imm32, tcg_imm32); //SIGN EXTETEND IMM
			tcg_gen_sub_tl(r2, r2, tcg_imm32);
			//TODO:SATURED TO 7FFFFFFFH OR 80000000H
			gen_set_gpr(rs2, r2);
			break;
		case 30: //SATSUBR
			tcg_gen_sub_tl(r2, r1, r2);
			//TODO:SATURED TO 7FFFFFFFH OR 80000000H
			gen_set_gpr(rs2, r2);
			break;
	}



	tcg_temp_free(r1);
	tcg_temp_free(r2);
	tcg_temp_free(tcg_r3);
	tcg_temp_free(tcg_temp);
}
/*POSSIBLE TCG_COND

TCG_COND_EQ
TCG_COND_NE
TCG_COND_LT // signed
TCG_COND_GE // signed
TCG_COND_LE // signed
TCG_COND_GT // signed
TCG_COND_LTU // unsigned
TCG_COND_GEU // unsigned
TCG_COND_LEU // unsigned
TCG_COND_GTU // unsigned
*/

static void decode_data_manipulation(DisasContext *ctx, int memop, int rs1, int rs2, int operation)
{
	TCGv tcg_r1 = tcg_temp_new();
	TCGv tcg_r2 = tcg_temp_new();
	TCGv tcg_r3 = tcg_temp_new();
	TCGv tcg_imm = tcg_temp_new();
	TCGv tcg_temp = tcg_temp_new();
	TCGv tcg_temp2 = tcg_temp_new();

	int int_imm = rs1; //imm is usually in r1(5-0)
	int int_rs3;
	//int int_cond;

	gen_get_gpr(tcg_r1, rs1);
	gen_get_gpr(tcg_r2, rs2);

	switch(operation) {
		case 1: //ZXH
			printf("ZXH \n");
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFFFF);
			tcg_gen_ext32u_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;
		case 2: //ZXB
			printf("ZXB \n");
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFF);
			tcg_gen_ext32u_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;
		case 3://SXH
			printf("SXH \n");
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFFFF);
			tcg_gen_ext32s_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;
		case 4://SXB
			printf("SXB \n");
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFF);
			tcg_gen_ext32s_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;
		case 5: //SHR Format IX
			printf("SHR Format IX\n");
			tcg_gen_shr_tl(tcg_r2, tcg_r2, tcg_r1);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 6: //SHR Format II
			printf("SHR Format II\n");
			tcg_gen_movi_tl(tcg_imm, int_imm);
			tcg_gen_ext32u_tl(tcg_imm, tcg_imm);
			tcg_gen_shr_tl(tcg_r2, tcg_r2, tcg_imm);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 7: //SHR Format XI
			printf("SHR Format XI\n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_shr_tl(tcg_r3, tcg_r2, tcg_r1);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 8://SHL Format IX
			printf("SHL Format IX\n");
			tcg_gen_shl_tl(tcg_r2, tcg_r2, tcg_r1);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 9://SHL Format II
			printf("SHL Format II \n");
			tcg_gen_movi_tl(tcg_imm, int_imm);
			tcg_gen_ext32u_tl(tcg_imm, tcg_imm);
			tcg_gen_shl_tl(tcg_r2, tcg_r2, tcg_imm);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 10://SHL Format XI
			printf("SHL Format XI \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_shl_tl(tcg_r3, tcg_r2, tcg_r1);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 11://SAR Format IX
			printf("SAR Format IX \n");
			tcg_gen_sar_tl(tcg_r2, tcg_r2, tcg_r1);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 12://SAR Format II
			printf("SAR Format II \n");
			tcg_gen_movi_tl(tcg_imm, int_imm);
			tcg_gen_ext32u_tl(tcg_imm, tcg_imm);
			tcg_gen_sar_tl(tcg_r2, tcg_r2, tcg_imm);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 13://SAR Format XI
			printf("SAR Format XI \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_sar_tl(tcg_r3, tcg_r2, tcg_r1);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 14: // ROTL Format ? Imm
			printf("ROTL z imm\n");
			tcg_gen_movi_tl(tcg_imm, int_imm);
			tcg_gen_ext32u_tl(tcg_imm, tcg_imm);
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_rotl_tl(tcg_r3, tcg_r2, tcg_imm);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 15: // ROTL Format ?
			printf("ROTL \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_rotl_tl(tcg_r3, tcg_r2, tcg_r1);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 16: //HSW
			printf("HSW \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			tcg_gen_andi_tl(tcg_temp, tcg_r2, 0xffff);
			tcg_gen_shli_tl(tcg_temp, tcg_temp, 0xe);
			tcg_gen_andi_tl(tcg_r2, tcg_r2, 0xffff0000);
			tcg_gen_or_tl(tcg_r3, tcg_r2, tcg_temp);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 17: //HSH
			printf("HSH \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_set_gpr(int_rs3, tcg_r1);
			break;
		case 18: //BSW
			printf("BSW \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_bswap32_tl(tcg_r3, tcg_r2);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 19: //BSH
			printf("BSH \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_mov_tl(tcg_temp2, tcg_r2);

			tcg_gen_andi_tl(tcg_temp, tcg_temp2, 0xff000000);
			tcg_gen_shri_tl(tcg_temp, tcg_temp, 0x8);
			tcg_gen_ori_tl(tcg_r3, tcg_temp, 0x00ff0000);

			tcg_gen_andi_tl(tcg_temp, tcg_temp2, 0x00ff0000);
			tcg_gen_shli_tl(tcg_temp, tcg_temp, 0x8);
			tcg_gen_ori_tl(tcg_r3, tcg_temp, 0xff000000);

			tcg_gen_andi_tl(tcg_temp, tcg_temp2, 0x0000ff00);
			tcg_gen_shri_tl(tcg_temp, tcg_temp, 0x8);
			tcg_gen_ori_tl(tcg_r3, tcg_temp, 0x000000ff);

			tcg_gen_andi_tl(tcg_temp, tcg_temp2, 0x000000ff);
			tcg_gen_shli_tl(tcg_temp, tcg_temp, 0x8);
			tcg_gen_ori_tl(tcg_r3, tcg_temp, 0x0000ff00);

			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 20: //SETF
			printf("SETF \n");
			//int_cond = extract32(ctx->opcode,0,4);
			//TODO if cond is true then
			tcg_gen_movi_tl(tcg_r2, 0x1);
			//else
			tcg_gen_movi_tl(tcg_r2, 0x0);

			gen_set_gpr(rs2, tcg_r2);
			break;
		case 21://SASF
			printf("SASF \n");
			//int_cond = extract32(ctx->opcode,0,4);
			//TODO if cond is true then
			tcg_gen_shli_tl(tcg_r2, tcg_r2, 0x1);
			tcg_gen_ori_tl(tcg_r2, tcg_r2, 0x00000001);
			//else
			tcg_gen_shli_tl(tcg_r2, tcg_r2, 0x1);
			tcg_gen_ori_tl(tcg_r2, tcg_r2, 0x00000000);

			gen_set_gpr(rs2, tcg_r2);
			break;
		case 22://CMOV XI
			printf("CMOV Format XI \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			//int_cond = extract32(ctx->opcode, 17, 4);
			//TODO if cond is true then
			tcg_gen_mov_tl(tcg_r3, tcg_r1);
			//else
			tcg_gen_mov_tl(tcg_r3, tcg_r2);

			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 23://CMOV XII
			printf("CMOV Format XII \n");
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			//int_cond = extract32(ctx->opcode, 17, 4);
			//TODO if cond is true then
			tcg_gen_movi_tl(tcg_temp, int_imm);
			tcg_gen_ext32s_tl(tcg_temp, tcg_temp);
			tcg_gen_mov_tl(tcg_r3, tcg_temp);
			//else
			tcg_gen_mov_tl(tcg_r3, tcg_r2);

			gen_set_gpr(int_rs3, tcg_r3);

			break;
		case 24://BINS
			printf("BINS \n");
			//TODO
			break;
	}

	tcg_temp_free(tcg_r1);
	tcg_temp_free(tcg_r2);
	tcg_temp_free(tcg_r3);
	tcg_temp_free(tcg_imm);
	tcg_temp_free(tcg_temp);

}
static void decode_branch_operations(DisasContext *ctx, int memop, int rs1, int rs2, int operation)
{
	TCGv tcg_r1 = tcg_temp_new();
	TCGv tcg_r2 = tcg_temp_new();

	gen_get_gpr(tcg_r1, rs1);
	gen_get_gpr(tcg_r2, rs2);

	//TESTING

	printf("test");
	//gen_goto_tb(ctx,2, 0x2);
	tcg_gen_movi_tl(cpu_pc, 0x0);
}

static void decode_load_store_1(CPURH850State *env, DisasContext *ctx)
{
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

static void decode_RH850_48(CPURH850State *env, DisasContext *ctx)
{
	uint32_t op;
	int sub_opcode;
	int rs2;

	op = MASK_OP_MAJOR(ctx->opcode);	// opcode is at b5-b10
	sub_opcode = GET_RS2(ctx->opcode);
	rs2 = GET_RS1(ctx->opcode);

	switch(op){

		case OPC_RH850_ST_LD_0:
			if (sub_opcode != 0) {
				decode_load_store_0(env, ctx);		//enter the  decode_load_store_1  function, check opcodes with mask  MASK_OP_ST_LD1
			}//else false instruction
			break;

		case OPC_RH850_ST_LD_1:
			if (sub_opcode != 0) {
				decode_load_store_1(env, ctx);		//enter the  decode_load_store_2  function, check opcodes with mask  MASK_OP_ST_LD2
			}//else false instruction
			break;
	}
	if(extract32(ctx->opcode, 5, 11) == 0x31){		//this is MOV3(48bit inst)
		printf("%lx", sextract64(ctx->opcode, 0, 48));
		decode_arithmetic(ctx, 0, 0, rs2, 18);
	}
}

static void decode_RH850_32(CPURH850State *env, DisasContext *ctx)
{
	int rs1;
	int rs2;
	int rd;
	uint32_t op;
	uint32_t formXop;
	uint32_t checkXII;
	target_long imm;
	target_long imm_32;

	op = MASK_OP_MAJOR(ctx->opcode);
	rs1 = GET_RS1(ctx->opcode);			// rs1 is at b0-b4;
	rs2 = GET_RS2(ctx->opcode);			// rs2 is at b11-b15;
	rd = GET_RD(ctx->opcode);
	imm = GET_IMM(ctx->opcode);
	TCGv r1 = tcg_temp_local_new();		//temp
	TCGv r2 = tcg_temp_local_new();		//temp
	imm_32 = GET_IMM_32(ctx->opcode);

	gen_get_gpr(r1, rs1);		//loading rs1 to r1
	gen_get_gpr(r2, rs2);		//loading rs2 to r2

	switch(op){

		case OPC_RH850_LDB:			// LD.B
	        gen_load(ctx, MO_SB, rd, rs1, imm);
	    	break;

	    case OPC_RH850_LDH_LDW:		//
	    	if ( extract32(ctx->opcode, 16, 1) == 0 )	// LD.H
	    		gen_load(ctx, MO_TESW, rd, rs1, imm);
	    	else
	    		gen_load(ctx, MO_TESL, rd, rs1, imm);		// LD.W
	    	break;

	    case OPC_RH850_STB:			//this opcode is unique
	    	gen_store(ctx, MO_SB, rs1, rs2, (extract32(ctx->opcode, 16, 16)));
	    	break;

	    case OPC_RH850_STH_STW:		//only two instructions share this opcode
	    	if ( extract32(ctx->opcode, 16, 1)==1 ) {
	    		gen_store(ctx, MO_TESL, rs1, rs2, (extract32(ctx->opcode, 17, 15)));
	    		//this is STORE WORD
	    		break;
	    	}
	    	gen_store(ctx, MO_TESW, rs1, rs2, (extract32(ctx->opcode, 17, 15)));
	    	//this is STORE HALFWORD
	    	break;
	    case OPC_RH850_ADDI:
	    	decode_arithmetic(ctx, 0, rs1,rs2, 6);
	    	break;
	    case OPC_RH850_ANDI:
	    	decode_arithmetic(ctx, 0, rs1, rs2, 17);
	    	break;
	    case OPC_RH850_MOVEA:
	    	if ( extract32(ctx->opcode, 11, 5) == 0 ){
	    		decode_arithmetic(ctx, 0, imm_32, rs1, 18);
	    		//this is MOV3
	    	} else {
	    		//MOVEA
	    	}
	    	break;
	    case OPC_RH850_MOVHI:

	    	break;
	    case OPC_RH850_LOOP:

	    	break;
	    case OPC_RH850_CLR:

	    	break;
		case OPC_RH850_32bit_1:				//this is the opcode=11111 group; formats IX, X, XI, XII
			if (extract32(ctx->opcode, 16, 1) == 0x1 ) { //if bit 16=1 its either b.cond or ld.hu
				if (rs2 == 0x0) {
					//this is BCOND2
					break;
				} else {
					//this is LD.HU
					gen_load(ctx, MO_TEUW, rd, rs1, imm);
					break;
				}
			}
			formXop = MASK_OP_32BIT_SUB(ctx->opcode);		//sub groups based on bits b23-b26
			switch(formXop){
				case OPC_RH850_LDSR:
					//LDSR   check if this can also be RIE!!!!
					break;
				case OPC_RH850_FORMAT_IX:		//format IX instructions
					formXop = MASK_OP_FORMAT_IX(ctx->opcode);	//mask on bits 21, 22
					switch(formXop){
						case OPC_RH850_BINS_0:
							if (extract32(ctx->opcode, 20, 1) == 1){
								//BINS0
								printf("BINS0\n");
							}
							else{
								if (extract32(ctx->opcode, 17, 1) == 0){
									//SHR format IX
									decode_data_manipulation(ctx,0, rs1, rs2, 5);
								}else{
									//SHR format XI
									decode_data_manipulation(ctx,0, rs1, rs2, 7);
								}
							}
							break;
						case OPC_RH850_BINS_1:
							if (extract32(ctx->opcode, 20, 1) == 1){
								//BINS1
								printf("BINS1\n");
							}
							else{
								if (extract32(ctx->opcode, 17, 1) == 0){
									//SAR format IX
									decode_data_manipulation(ctx,0, rs1, rs2, 11);
								}else{
									//SAR format XI
									decode_data_manipulation(ctx,0, rs1, rs2, 13);
								}
							}
						break;
						case OPC_RH850_BINS_2:
							if (extract32(ctx->opcode, 20, 1) == 1){
								//BINS2
								printf("BINS2\n");
							}
							else{
								if (extract32(ctx->opcode, 17, 1) == 0){
									if (extract32(ctx->opcode, 18, 1) == 1){
										//ROTL
										decode_data_manipulation(ctx,0, rs1, rs2, 14);
									}
									else{
										//SHL format IX
										decode_data_manipulation(ctx,0, rs1, rs2, 8);
									}
								}else{
									if (extract32(ctx->opcode, 18, 1) == 1){
										//ROTL
										decode_data_manipulation(ctx,0, rs1, rs2, 15);
									}
									else{
										//SHL format XI
										decode_data_manipulation(ctx,0, rs1, rs2, 10);
									}
								}
							}
							break;
						case OPC_RH850_CLR1:
							if (extract32(ctx->opcode, 19, 1) == 0){
								//CLR1
							} else if (extract32(ctx->opcode, 19, 1) == 1){
								//CAXI
							}
							break;
					}
					break;
				case OPC_RH850_FORMAT_X:	// 0010 //format X instructions
							// 		//(+JARL- added due to MASK_OP_FORMAT_X matching)
					formXop = MASK_OP_FORMAT_X(ctx->opcode);
					switch(formXop){
						case OPC_RH850_CLL:
							break;
						case OPC_RH850_CTRET:
							break;
						case OPC_RH850_DI:
							break;
						case OPC_RH850_EI:
							break;
						case OPC_RH850_EIRET:
							break;
						case OPC_RH850_FERET:
							break;
						case OPC_RH850_HALT:
							break;
						case OPC_RH850_JARL:
							break;
						case OPC_RH850_SNOOZE:
							break;
						case OPC_RH850_SYSCALL:
							break;
						case OPC_RH850_TRAP:
							break;
						case OPC_RH850_PREF:
							break;
					}
					break;
				case OPC_RH850_MUL_INSTS:		//MUL instructions
					if (extract32(ctx->opcode, 22, 1) == 0){
						// MUL in format XI
						break;
					} else if (extract32(ctx->opcode, 22, 1) == 1){
						// MUL in format XII
						break;
					}
					break;

				case OPC_RH850_FORMAT_XI:					// DIV instructions in format XI
					formXop = extract32(ctx->opcode, 16, 7);
					switch(formXop){
						case OPC_RH850_DIVH:
							//DIVH
							break;
						case OPC_RH850_DIVHU:
							//DIVHU
							break;
						case OPC_RH850_DIV:
							//DIV
							break;
						case OPC_RH850_DIVQ:
							//DIVQ
							break;
						case OPC_RH850_DIVQU:
							//DIVQU
							break;
						case OPC_RH850_DIVU:
							//DIVU
							break;
					}
					break;

				case OPC_RH850_FORMAT_XII:	// 0110 //format XII instructions
									//excluding MUL and including CMOV
					checkXII = extract32(ctx->opcode, 21, 2);

					switch(checkXII){
					case 0:
						//CMOV in format XII
						break;
					case 1:
						//CMOV in format XI
						break;
					case 2:
						formXop = extract32(ctx->opcode, 17, 2);
						switch(formXop){
						case OPC_RH850_BSW:
							//BSW
							decode_data_manipulation(ctx,0, rs1, rs2, 18);
							break;
						case OPC_RH850_HSW:
							//HSW
							decode_data_manipulation(ctx,0, rs1, rs2, 16);
							break;
						case OPC_RH850_HSH:
							//HSH
							decode_data_manipulation(ctx,0, rs1, rs2, 17);
							break;
						}
						break;
					case 3:	//these are SCHOL, SCHOR, SCH1L, SCH1R
						formXop = extract32(ctx->opcode, 17, 2);
						switch(formXop){
						case OPC_RH850_SCH0R:
							//SCH0R
							break;
						case OPC_RH850_SCH1R:
							//SCH1R
							break;
						case OPC_RH850_SCH0L:
							//SCH0L
							break;
						case OPC_RH850_SCH1L:
							//SCH1L
							break;
						}

					}
					break;

				case OPC_RH850_ADF_MAC_MACU:		//ADF, MAC, MACU
					formXop = extract32(ctx->opcode, 21, 2);
					switch(formXop){
						case OPC_RH850_ADF:
							//ADF
							break;
						case OPC_RH850_MAC:
							//MAC
							break;
						case OPC_RH850_MACU:
							//MACU
							break;
					}
			}
	}

	if (MASK_OP_FORMAT_V_FORMAT_XIII(ctx->opcode) == OPC_RH850_FORMAT_V_XIII){

		if(extract32(ctx->opcode, 16, 1) == 0){

			if (extract32(ctx->opcode, 11, 5) == 0){
				//JR
			} else {
				//JARL2
			}
		} else if (extract32(ctx->opcode, 16, 3) == 0x3){
			//PREPARE2
		} else if (extract32(ctx->opcode, 16, 3) == 0x1){
			//PREPARE1
		}
	}

}

static void decode_RH850_16(CPURH850State *env, DisasContext *ctx)
{
	int rs1;
	int rs2;
	int imm;
	uint32_t op;
	uint32_t subOpCheck;

	op = MASK_OP_MAJOR(ctx->opcode);
	rs1 = GET_RS1(ctx->opcode);			// rs1 is at b0-b4;
	rs2 = GET_RS2(ctx->opcode);			// rs2 is at b11-b15;
	imm = rs1;

	switch(op){
	case OPC_RH850_16bit_0:
		if (rs2 != 0) {
			decode_arithmetic(ctx, 0, rs1, rs2, 0);	// MOV Format 1
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
				//RIE
				break;
			} else {
				//SWITCH
				break;
			}
		} else {
			if (rs1 == 0){
				//FETRAP
				break;
			} else {
				//DIVH
				decode_arithmetic(ctx, 0, rs1, rs2, 12);
				break;
			}
		}
		break;
	case OPC_RH850_BCOND:
		decode_branch_operations(ctx, 0, rs1, rs2, 1);
		break;

	case OPC_RH850_16bit_4:
		if (rs2 == 0){
			//ZXB
			decode_data_manipulation(ctx,0,rs1,rs2,2);
			break;
		} else {
			//SATSUBR
			break;
		}
		break;
	case OPC_RH850_16bit_5:
		if (rs2 == 0){
			//SXB
			decode_data_manipulation(ctx,0,rs1,rs2,4);
			break;
		} else {
			//SATSUB
			break;
		}
		break;
	case OPC_RH850_16bit_6:
		if (rs2 == 0){
			//ZXH
			decode_data_manipulation(ctx,0,rs1,rs2,1);
			break;
		} else {
			//SATADD
			break;
		}
		break;
	case OPC_RH850_16bit_7:
		if (rs2 == 0){
			//SXH
			decode_data_manipulation(ctx,0, rs1, rs2, 3);
			break;
		} else {
			//MULH
			decode_arithmetic(ctx, 0, rs1,rs2, 8);
			break;
		}
		break;
	case OPC_RH850_NOT:
		decode_arithmetic(ctx, 0, rs1, rs2, 11);
		break;
	case OPC_RH850_16bit_3:
		if (rs2 == 0){
			//JMP
			break;
		} else {
			if(extract32(rs1,4,1)==1){
				//SLD.HU
			}else{
				//SLD.BU
			}
			break;
		}
		break;
	case OPC_RH850_OR:
		decode_arithmetic(ctx, 0, rs1,rs2, 4);
		break;
	case OPC_RH850_XOR:
		decode_arithmetic(ctx, 0, rs1,rs2, 5);
		break;
	case OPC_RH850_AND:
		decode_arithmetic(ctx, 0, rs1,rs2, 7);
		break;
	case OPC_RH850_TST:
		break;
	case OPC_RH850_SUBR:
		decode_arithmetic(ctx, 0, rs1,rs2, 3);
		break;
	case OPC_RH850_SUB:
		decode_arithmetic(ctx, 0, rs1,rs2, 2);
		break;
	case OPC_RH850_ADD:
		decode_arithmetic(ctx, 0, rs1,rs2, 1);
		break;
	case OPC_RH850_CMP:
		break;
	case OPC_RH850_16bit_16:
		if (rs2 == 0){
			//CALLT
			break;
		} else {
			decode_arithmetic(ctx, 0, imm, rs2, 16);	//MOV immediate
			break;
		}
		break;
	case OPC_RH850_16bit_17:
		if (rs2 == 0){
			//CALLT
			break;
		} else {
			//SATADD
			break;
		}
		break;
	case OPC_RH850_16bit_ADD:
		decode_arithmetic(ctx, 0, rs1,rs2, 6);
		break;
	case OPC_RH850_16bit_CMP:
		break;
	case OPC_RH850_16bit_SHR:
		decode_data_manipulation(ctx,0, rs1, rs2, 6);
		break;
	case OPC_RH850_16bit_SAR:
		decode_data_manipulation(ctx,0, rs1, rs2, 12);
		break;
	case OPC_RH850_16bit_SHL:
		decode_data_manipulation(ctx,0, rs1, rs2, 9);
		break;
	case OPC_RH850_16bit_MULH:
		decode_arithmetic(ctx, 0, rs1,rs2, 9);
		break;
	}

	//Format IV code bits 7-10
	uint32_t opIV = op>>2;

	switch(opIV){
	case OPC_RH850_16bit_SLDB:
		break;
	case OPC_RH850_16bit_SLDH:
		break;
	case OPC_RH850_16bit_IV10:
		if(extract32(rs1,0,1)==1){
			//SST.W
		}
		else{
			//SLD.H
		}
		break;
	case OPC_RH850_16bit_SSTB:
		break;
	case OPC_RH850_16bit_SSTH:
		break;
	}

}


static void decode_opc(CPURH850State *env, DisasContext *ctx)
{
    /* checking for 48-bit instructions */
    if ( (extract32(ctx->opcode, 6, 11) == 0x41e) ||	//bits are 10000011110
    		(extract32(ctx->opcode, 5, 11) == 0x31) ||	//MOV3 needs to be checked here
			(extract32(ctx->opcode, 5, 11) == 0x37) ) { //this fits for JMP2(48-bit) and LOOP(32-bit!!!!!)
    	ctx->next_pc = ctx->pc + 6;
    	decode_RH850_48(env, ctx);
    } else if (extract32(ctx->opcode, 9, 2) == 0x3){		//bits are 11
    	ctx->next_pc = ctx->pc + 4;
    	decode_RH850_32(env, ctx);
    } else {
    	ctx->next_pc = ctx->pc + 2;
    	decode_RH850_16(env, ctx);

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
    ctx.mem_idx = tb->flags & TB_FLAGS_MMU_MASK;
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

        ctx.opcode = cpu_ldl_code(env, ctx.pc);
        decode_opc(env, &ctx);
        ctx.pc = ctx.next_pc;

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


    cpu_pc = tcg_global_mem_new(cpu_env, offsetof(CPURH850State, pc), "pc");
    load_res = tcg_global_mem_new(cpu_env, offsetof(CPURH850State, load_res),
                             "load_res");
    load_val = tcg_global_mem_new(cpu_env, offsetof(CPURH850State, load_val),
                             "load_val");
}
