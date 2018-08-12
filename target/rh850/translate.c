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
	PSW_register 	= 4,
	FPSR_register	= 5,
	FPEPC_register	= 6,
	FPST_register 	= 7,
	FPCC_register	= 8,
	FPCFG_register	= 9,
	FPEC_register	= 10,
	EIIC_register	= 11,
	FEIC_register 	= 12,
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

static void gen_load(DisasContext *ctx, int memop, int rd, int rs1, target_long imm)
{
    TCGv t0 = tcg_temp_new();
    TCGv t1 = tcg_temp_new();
    TCGv tcg_imm = tcg_temp_new();

    gen_get_gpr(t0, rs1);
    tcg_gen_movi_i32(tcg_imm, imm);
    tcg_gen_ext16s_i32(tcg_imm, tcg_imm);
    tcg_gen_add_tl(t0, t0, tcg_imm);

    if (memop < 0) {
        gen_exception_illegal(ctx);
        return;
    }

    tcg_gen_qemu_ld_tl(t1, t0, 0, MO_8);
    tcg_gen_ext8s_i32(t0, t0);
    gen_set_gpr(rd, t1);

    printf("\n");
	printf("To je reg = vsebina:r%x \n", rd);
	printf("To je naslov = vsebina:r%x + %x \n", rs1, imm);
	printf("\n");

    tcg_temp_free(t0);
    tcg_temp_free(t1);
}

static void gen_store(DisasContext *ctx, int memop, int rs1, int rs2,    //make cases for opcodes
        target_long imm)
{
    TCGv t0 = tcg_temp_new();		//temp
    TCGv dat = tcg_temp_new();		//temp
    TCGv tcg_imm = tcg_temp_new();

    gen_get_gpr(t0, rs1);			//loading rs1 to t0
    tcg_gen_movi_i32(tcg_imm, imm);
    tcg_gen_ext16s_i32(tcg_imm, tcg_imm);

    tcg_gen_add_tl(t0, t0, tcg_imm);	//adding displacement to t0
    gen_get_gpr(dat, rs2);			//getting data from rs2

    if (memop < 0) {
        gen_exception_illegal(ctx);
        return;
    }

    tcg_gen_qemu_st_tl(dat, t0, 0, MO_8);

    printf("\n");
    printf("To je dat = vsebina:r%x \n", rs2);
    printf("To je t0 = vsebina:r%x + %x \n", rs1, imm);
    printf("\n");

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

static void gen_arithmetic(DisasContext *ctx, int memop, int rs1, int rs2, int operation)
{
	TCGv r1 = tcg_temp_new();		//temp
	TCGv r2 = tcg_temp_new();		//temp
	//TCGv comp = tcg_temp_new();
	gen_get_gpr(r1, rs1);			//loading rs1 to t0
	gen_get_gpr(r2, rs2);			//loading rs2 to t1
	int imm = rs1;
	int imm_32;
	int int_rs3;
	uint64_t opcode48;

	TCGv tcg_imm = tcg_temp_new();
	TCGv tcg_imm32 = tcg_temp_new();
	TCGv tcg_r3 = tcg_temp_new();
	TCGv tcg_temp = tcg_temp_new();

	//TCGLabel *l = gen_new_label();
	//TCGLabel * l1;
	//TCGLabel * l2;
	TCGLabel *end;
	TCGLabel *cont;
	TCGLabel *cont2;
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

		case 6:	//ADD FORMAT 2
			if((imm & 0x10) == 0x10){
				imm = imm | (0x7 << 5);
			}
			tcg_gen_movi_i32(tcg_imm, imm);
			tcg_gen_ext8s_i32(tcg_imm, tcg_imm);
			tcg_gen_add_tl(r2, r2, tcg_imm);
			gen_set_gpr(rs2, r2);
			break;

		case 8://MULH (format I)				//move to gen_multiply!
			tcg_gen_andi_tl(r2, r2,0x0000FFFF);
			tcg_gen_andi_tl(r1, r1,0x0000FFFF);
			tcg_gen_mul_tl(r2, r2, r1);
			gen_set_gpr(rs2, r2);
			break;
		case 9://MULH (format II)				//move to gen_multiply!
			if ((imm & 0x10) == 0x10){
				imm = imm | (0x7 << 5);
			}
			// signed multiplication!!!
			tcg_gen_movi_tl(tcg_imm, imm);
			tcg_gen_ext8s_i32(tcg_imm, tcg_imm);
			tcg_gen_andi_tl(tcg_imm, tcg_imm,0x0000FFFF);
			tcg_gen_mul_tl(r2, r2, tcg_imm);
			gen_set_gpr(rs2, r2);
			break;
		case 10://SXH			//this is a duplicate, the correct SXH is in data_manipulation

			break;

		case 12://DIVH							//move to gen_divide!!
			tcg_gen_andi_i32(r1, r1,0x0000ffff);
			tcg_gen_ext16s_i32(r1, r1);
			tcg_gen_div_i32(r2, r2, r1);
			gen_set_gpr(rs2, r2);
			break;
		case 13: //ADDI
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_ext16s_tl(tcg_imm32, tcg_imm32);
			tcg_gen_add_tl(r2,r1, tcg_imm32);
			gen_set_gpr(rs2, r2);
			break;
		case 14: //ADF								//move to gen_cond_arith!!
			int_rs3 = extract32(ctx->opcode, 27, 5);
			//int_cond = extract32(ctx->opcode, 17, 4);
			gen_get_gpr(tcg_r3,int_rs3);
			//if condition then
			tcg_gen_addi_tl(r2, r2, 1);
			tcg_gen_add_tl(tcg_r3, r2, r1);
			//else
			tcg_gen_add_tl(tcg_r3, r2, r1);
			gen_set_gpr(int_rs3, tcg_r3);
			break;

		case 16:
			if ((imm & 0x10) == 0x10){	//extending 5bit immediate to 8 bits
				imm = imm | (0x7 << 5);
			}
			tcg_gen_movi_tl(r2, imm); // MOV Format 2
			tcg_gen_ext8s_i32(r2, r2);	//extending to 32 bits
			gen_set_gpr(rs2, r2);
			break;
		case 17://MOVEA
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_i32(tcg_imm, imm_32);
			tcg_gen_ext16s_i32(tcg_imm, tcg_imm);

			tcg_gen_add_i32(r2, tcg_imm, r1);
			gen_set_gpr(rs2, r2);
			break;
		case 18://MOV3   ---  48bit instruction
			opcode48 = (ctx->opcode1);
			opcode48 = (ctx->opcode) | (opcode48  << 0x20);
			imm_32 = extract64(opcode48, 16, 32) & 0xffffffff;
			tcg_gen_movi_i32(r2, imm_32);
			gen_set_gpr(rs2, r2);
			break;
		case 19: //MUL FORMAT XI				//move to gen_multiply
			int_rs3 = extract32(ctx->opcode, 26, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_mul_tl(r2, r2, r1);
			//tcg_gen_muls2_i32()

			// R3(higher 32 bits) IN R2(lower 32 bits)
			tcg_gen_addi_tl(tcg_temp, tcg_temp, 32);
			tcg_gen_sar_tl(tcg_r3, r2,tcg_temp);
			tcg_gen_andi_tl(r2, r2,0xFFFFFFFF);
			gen_set_gpr(int_rs3,tcg_r3);
			gen_set_gpr(rs2, r2);
			break;
		case 20: //MUL FORMAT XII				//move to gen_multiply
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
		case 21: //MULHI						//move to gen_multiply
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_andi_tl(tcg_temp, r1,0xFFFF); //GET LOWER 16 BITS OF R1
			tcg_gen_mul_tl(r2, tcg_temp, tcg_imm32);
			gen_set_gpr(rs2, r2);
			break;
		case 22://MULU FORMAT XI				//move to gen_multiply
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_mulsu2_i32(r2, tcg_r3, r2, r1);

			gen_set_gpr(int_rs3,tcg_r3);
			gen_set_gpr(rs2, r2);
			break;
		case 23://MULU FORMAT XII				//move to gen_multiply
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			imm_32 = extract32(ctx->opcode, 18, 4);

			tcg_gen_movi_tl(tcg_imm32, imm_32);
			tcg_gen_movi_tl(tcg_imm, rs1);

			tcg_gen_shli_i32(tcg_imm32, tcg_imm32, 0x5);
			tcg_gen_or_i32(tcg_imm32, tcg_imm32, tcg_imm);
			tcg_gen_ext16u_tl(tcg_imm32, tcg_imm32);

			tcg_gen_mulsu2_i32(r2, tcg_r3, tcg_imm32, r2);

			gen_set_gpr(int_rs3,tcg_r3);
			gen_set_gpr(rs2, r2);
			break;

		case 24: {//SATADD1 FORMAT I: SATADD reg1, reg2		//move to gen_sat_op!!

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
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);
			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;
		case 25: {	//SATADD2 FORMAT II, SATADD imm5, reg2    //move to gen_sat_op!!


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
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, imm_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);
			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(imm_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;
		case 26: {//SATADD3 FORMAT XI: SATADD reg1, reg2, reg3    //move to gen_sat_op!!

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
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont); 										//else
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);		//if (r2 < MIN-r1)
			tcg_gen_mov_i32(result, min);										//return MIN;

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(int_rs3, result);
			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;

		case 27: {//SATSUB reg1, reg2    //move to gen_sat_op!!

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

			tcg_gen_neg_i32(r1_local, r1_local);

			tcg_gen_add_i32(result, r1_local, r2_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, r2_local, check, end);
			tcg_gen_mov_i32(result, max);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);
			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}	break;
		case 28: {///SATSUB reg1, reg2, reg3      //move to gen_sat_op!!

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
			printf("to je rs3 %x  \n", int_rs3);
			tcg_gen_neg_i32(r1_local, r1_local);

			tcg_gen_add_i32(result, r1_local, r2_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, r2_local, check, end);
			tcg_gen_mov_i32(result, max);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, r2_local, check, cont2);
			tcg_gen_mov_i32(result, min);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(int_rs3, result);
			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(r2_local);
			tcg_temp_free(zero);

		}
			break;
		case 29: {//SATSUBI imm16, reg1, reg2     //move to gen_sat_op!!

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
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_i32(imm_local, imm_32);
			tcg_gen_ext16s_i32(imm_local, imm_local);
			tcg_gen_movi_i32(zero, 0x0);
			end = gen_new_label();
			cont = gen_new_label();
			cont2 = gen_new_label();

			tcg_gen_neg_i32(imm_local, imm_local);

			tcg_gen_add_i32(result, r1_local, imm_local);

			tcg_gen_brcond_tl(TCG_COND_LT, r1_local, zero, cont);

			tcg_gen_sub_i32(check, max, r1_local);
			tcg_gen_brcond_tl(TCG_COND_LE, imm_local, check, end);
			tcg_gen_mov_i32(result, max);
			tcg_gen_br(end);

			//---------------------------------------------------------------------------------
			gen_set_label(cont);
			tcg_gen_sub_i32(check, min, r1_local);
			tcg_gen_brcond_tl(TCG_COND_GE, imm_local, check, cont2);
			tcg_gen_mov_i32(result, min);

			gen_set_label(cont2);
			gen_set_label(end);
			gen_set_gpr(rs2, result);
			tcg_temp_free(result);
			tcg_temp_free(check);
			tcg_temp_free(min);
			tcg_temp_free(max);
			tcg_temp_free(r1_local);
			tcg_temp_free(imm_local);
			tcg_temp_free(zero);

		}	break;
		case 31: //MOVHI
			imm_32 = extract32(ctx->opcode, 16, 16);
			tcg_gen_movi_i32(tcg_imm, imm_32);
			tcg_gen_shli_i32(tcg_imm, tcg_imm, 0x10);

			tcg_gen_add_i32(r2, tcg_imm, r1);
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

static void gen_logical(DisasContext *ctx, int rs1, int rs2, int operation)
{
	TCGv r1 = tcg_temp_new();		//temp
	TCGv r2 = tcg_temp_new();		//temp

	gen_get_gpr(r1, rs1);			//loading rs1 to t0
	gen_get_gpr(r2, rs2);			//loading rs2 to t1

	//int imm = rs1;
	int imm_32;
	//int int_rs3;
	//uint64_t opcode48;

	TCGv tcg_imm = tcg_temp_new();
	//TCGv tcg_imm32 = tcg_temp_new();
	//TCGv tcg_r3 = tcg_temp_new();
	//TCGv tcg_temp = tcg_temp_new();

	TCGLabel *end;
	TCGLabel *cont;
	//TCGLabel *cont2;

	switch(operation){
	case OPC_RH850_AND:
		tcg_gen_and_tl(r2, r2, r1);
		gen_set_gpr(rs2, r2);
		break;
	case OPC_RH850_ANDI:
		imm_32 = extract32(ctx->opcode, 16, 16);
		tcg_gen_movi_tl(tcg_imm, imm_32);
		tcg_gen_ext16u_tl(tcg_imm, tcg_imm);
		tcg_gen_and_tl(r2, r1, tcg_imm);
		gen_set_gpr(rs2, r2);
		break;
	case OPC_RH850_NOT:
		tcg_gen_not_i32(r2, r1);
		gen_set_gpr(rs2, r2);
		break;
	case OPC_RH850_OR:
		tcg_gen_or_tl(r2, r2, r1);
		gen_set_gpr(rs2, r2);
		break;
	case OPC_RH850_ORI:
		imm_32 = extract32(ctx->opcode, 16, 16);
		tcg_gen_movi_i32(tcg_imm, imm_32);
		tcg_gen_ext16u_i32(tcg_imm,tcg_imm);

		tcg_gen_or_i32(r2, r1, tcg_imm);
		gen_set_gpr(rs2, r2);
		break;
	case OPC_RH850_TST:	{
		TCGv r1_local = tcg_temp_local_new();
		TCGv r2_local = tcg_temp_local_new();
		TCGv check = tcg_temp_local_new();
		TCGv result = tcg_temp_local_new();
		end = gen_new_label();
		cont = gen_new_label();

		tcg_gen_mov_i32(r1_local, r1);
		tcg_gen_mov_i32(r2_local, r2);

		tcg_gen_and_i32(result, r1_local, r2_local);

		tcg_gen_brcondi_tl(TCG_COND_NE, result, 0x0, cont);
		tcg_gen_movi_i32(cpu_ZF, 0x1);
		tcg_gen_br(end);

		gen_set_label(cont);

		tcg_gen_andi_i32(check, result, (0x1 << 31));
		tcg_gen_brcondi_tl(TCG_COND_NE, check, (0x1 << 31), end);
		tcg_gen_movi_i32(cpu_SF, 0x1);

		gen_set_label(end);

		tcg_temp_free(result);
		tcg_temp_free(check);
		tcg_temp_free(r1_local);
		tcg_temp_free(r2_local);
	}	break;
	case OPC_RH850_XOR:
		tcg_gen_xor_tl(r2, r2, r1);
		gen_set_gpr(rs2, r2);
		break;
	case OPC_RH850_XORI:
		imm_32 = extract32(ctx->opcode, 16, 16);
		tcg_gen_movi_i32(tcg_imm, imm_32);
		tcg_gen_ext16u_i32(tcg_imm,tcg_imm);

		tcg_gen_xor_i32(r2, r1, tcg_imm);
		gen_set_gpr(rs2, r2);
		break;
	}

	tcg_temp_free(r1);
	tcg_temp_free(r2);
	tcg_temp_free(tcg_imm);
}

//static void gen_multiply(DisasContext *ctx, int rs1, int rs2, int operation){}
//static void gen_divide(DisasContext *ctx, int rs1, int rs2, int operation){}
//static void gen_cond_arith(DisasContext *ctx, int rs1, int rs2, int operation){}
//static void gen_sat_op(DisasContext *ctx, int rs1, int rs2, int operation){}


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
			tcg_gen_ext16u_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;
		case 2: //ZXB
			printf("ZXB \n");
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFF);
			tcg_gen_ext8u_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;
		case 3://SXH
			printf("SXH \n");
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFFFF);
			tcg_gen_ext16s_tl(tcg_r1, tcg_r1);
			gen_set_gpr(rs1, tcg_r1);
			break;
		case 4://SXB
			tcg_gen_andi_tl(tcg_r1, tcg_r1,0xFF);
			printf("SXB  \n");
			tcg_gen_ext8s_tl(tcg_r1, tcg_r1);
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
			tcg_gen_ext8u_tl(tcg_imm, tcg_imm);
			tcg_gen_shr_tl(tcg_r2, tcg_r2, tcg_imm);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 7: //SHR Format XI
			printf("SHR Format XI\n");
			int_rs3 = extract32(ctx->opcode, 27, 5);
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
			tcg_gen_ext8u_tl(tcg_imm, tcg_imm);
			tcg_gen_shl_tl(tcg_r2, tcg_r2, tcg_imm);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 10://SHL Format XI
			printf("SHL Format XI \n");
			int_rs3 = extract32(ctx->opcode, 27, 5);
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
			tcg_gen_ext8u_tl(tcg_imm, tcg_imm);
			tcg_gen_sar_tl(tcg_r2, tcg_r2, tcg_imm);
			gen_set_gpr(rs2, tcg_r2);
			break;
		case 13://SAR Format XI
			printf("SAR Format XI \n");
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_sar_tl(tcg_r3, tcg_r2, tcg_r1);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 14: // ROTL Format VII Imm
			printf("ROTL z imm\n");
			tcg_gen_movi_tl(tcg_imm, int_imm);
			tcg_gen_ext8u_tl(tcg_imm, tcg_imm);
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_rotl_tl(tcg_r3, tcg_r2, tcg_imm);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 15: // ROTL Format VII
			printf("ROTL \n");
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_rotl_tl(tcg_r3, tcg_r2, tcg_r1);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 16: //HSW
			printf("HSW \n");
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);

			tcg_gen_andi_tl(tcg_temp, tcg_r2, 0xffff);
			tcg_gen_shli_tl(tcg_temp, tcg_temp, 0x10);
			tcg_gen_andi_tl(tcg_temp2, tcg_r2, 0xffff0000);
			tcg_gen_shri_tl(tcg_temp2, tcg_temp2, 0x10);

			tcg_gen_or_tl(tcg_r3, tcg_temp2, tcg_temp);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 17: //HSH
			printf("HSH \n");
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_set_gpr(int_rs3, tcg_r1);
			break;
		case 18: //BSW
			printf("BSW \n");
			int_rs3 = extract32(ctx->opcode, 27, 5);
			gen_get_gpr(tcg_r3,int_rs3);
			tcg_gen_bswap32_tl(tcg_r3, tcg_r2);
			gen_set_gpr(int_rs3, tcg_r3);
			break;
		case 19: //BSH
			printf("BSH \n");
			int_rs3 = extract32(ctx->opcode, 27, 5);
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
	TCGv imm = tcg_temp_new();
	uint32_t imm_9;


	gen_get_gpr(tcg_r1, rs1);
	gen_get_gpr(tcg_r2, rs2);

	//TESTING

	printf("test");
	//gen_goto_tb(ctx,2, 0x2);
	tcg_gen_movi_tl(cpu_pc, 0x0);

	switch(operation){
		case 1:	//BCOND disp9
			imm_9 = extract32(ctx->opcode, 4, 3) | (extract32(ctx->opcode, 11, 5) << 0x3);
			tcg_gen_movi_i32(imm, imm_9);
			printf("this is the displacement in BCOND: %x \n", ctx->opcode);
			printf("this is the displacement in BCOND: %x \n", (extract32(ctx->opcode, 11, 5) << 0x3));
			printf("this is the displacement in BCOND: %x \n", imm_9);
	}

}


static void gen_jmp(DisasContext *ctx, int rs1, int rs2, int operation){

	TCGv dest = tcg_temp_new();
	gen_get_gpr(dest, rs1);

	switch(operation){
	case 0:
		printf("skocni ukaz se bo izvedel \n");
		tcg_gen_mov_tl(cpu_pc, dest);
		ctx->next_pc = ctx->pc+8;
		break;
	}

};

static void decode_RH850_48(CPURH850State *env, DisasContext *ctx)
{
	uint32_t op;
	int sub_opcode;
	int rs2;
	uint64_t opcode48;

	op = MASK_OP_MAJOR(ctx->opcode);	// opcode is at b5-b10
	sub_opcode = GET_RS2(ctx->opcode);
	rs2 = GET_RS1(ctx->opcode);

	opcode48 = (ctx->opcode1);
	opcode48 = (ctx->opcode) | (opcode48  << 0x20);

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
	if(extract32(ctx->opcode, 5, 11) == 0x31){
		gen_arithmetic(ctx, 0, 0, rs2, 18);				// this is MOV3 (48bit inst)
	} else if (extract32(ctx->opcode, 5, 12) == 0x37) {
		// this is JMP2 (48bit inst)
	} else if (extract32(ctx->opcode, 5, 11) == 0x17) {
		if (rs2 == 0x0){
			// this is JR2 (48bit inst)
		} else {
			// this is JARL2 (48bit inst)
		}
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
	uint32_t check32bitZERO;
	target_long imm;
	target_long imm_32;
	target_long ld_imm;

	op = MASK_OP_MAJOR(ctx->opcode);
	rs1 = GET_RS1(ctx->opcode);			// rs1 is at b0-b4;
	rs2 = GET_RS2(ctx->opcode);			// rs2 is at b11-b15;
	rd = GET_RD(ctx->opcode);
	imm = GET_IMM(ctx->opcode);
	TCGv r1 = tcg_temp_local_new();		//temp
	TCGv r2 = tcg_temp_local_new();		//temp
	imm_32 = GET_IMM_32(ctx->opcode);
	ld_imm = extract32(ctx->opcode, 16, 16);

	gen_get_gpr(r1, rs1);		//loading rs1 to r1
	gen_get_gpr(r2, rs2);		//loading rs2 to r2

	switch(op){

		case OPC_RH850_LDB:			// LD.B
	        gen_load(ctx, MO_SB, rs2, rs1, ld_imm);
	    	break;

	    case OPC_RH850_LDH_LDW:		//
	    	if ( extract32(ctx->opcode, 16, 1) == 0 )	// LD.H
	    		gen_load(ctx, MO_TESW, rs2, rs1, imm);
	    	else
	    		gen_load(ctx, MO_TESL, rs2, rs1, imm);		// LD.W
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
	    	printf("ADDI");
	    	gen_arithmetic(ctx, 0, rs1,rs2, 13);
	    	break;
	    case OPC_RH850_ANDI:
	    	gen_logical(ctx, rs1, rs1, OPC_RH850_ANDI);
	    	break;
	    case OPC_RH850_MOVEA:
	    	if ( extract32(ctx->opcode, 11, 5) == 0 ){
	    		gen_arithmetic(ctx, 0, imm_32, rs1, 18);
	    		//this is MOV3
	    	} else {
	    		gen_arithmetic(ctx, 0, rs1, rs2, 17);
	    		//MOVEA
	    	}
	    	break;
	    case OPC_RH850_MOVHI:
	    	gen_arithmetic(ctx, 0, rs1, rs2, 31);
	    	break;
	    case OPC_RH850_ORI:
	    	gen_logical(ctx, rs1, rs2, OPC_RH850_ORI);
	    	break;
	    case OPC_RH850_SATSUBI:
	    	gen_arithmetic(ctx, 0, rs1, rs2, 29);
	    	break;
	    case OPC_RH850_XORI:
	    	gen_logical(ctx, rs1, rs2, OPC_RH850_XORI);
	    	break;
	    case OPC_RH850_LOOP:
	    	if (extract32(ctx->opcode, 11, 5) == 0x0){
	    		//loop
	    	} else {
	    		//mulhi
	    	}

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
				case OPC_RH850_LDSR_RIE_SETF_STSR:
					check32bitZERO = extract32(ctx->opcode, 21, 2);
					switch(check32bitZERO){
					case 0:
						if(extract32(ctx->opcode, 4, 1)==1){
							//RIE
						} else {
							//SETF
						}
						break;
					case 1:
						//LDSR
						break;
					case 2:
						//STSR
						break;
					}
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
										//ROTL1
										decode_data_manipulation(ctx,0, rs1, rs2, 14);
									}
									else{
										//SHL format IX
										decode_data_manipulation(ctx,0, rs1, rs2, 8);
									}
								}else{
									if (extract32(ctx->opcode, 18, 1) == 1){
										//ROTL2
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
									//CAXI
								}
							}
							break;
					}
					break;
				case OPC_RH850_FORMAT_X:	// 0010 //format X instructions
							// 		//(+JARL3 - added due to MASK_OP_FORMAT_X matching)
					formXop = MASK_OP_FORMAT_X(ctx->opcode);
					switch(formXop){
						case OPC_RH850_CLL_CACHE:
							if ((extract32(ctx->opcode, 27, 5) == 0x1E) && (extract32(ctx->opcode, 0, 5) == 0x1F)){
								//CLL
							} else {
								//CACHE; if cacheop bits are 1111110, opcode matches CLL ins,
								//then thay are THE SAME instruction, so this should be correct
							}
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
						case OPC_RH850_JARL3:
							break;
						case OPC_RH850_SNOOZE:
							break;
						case OPC_RH850_SYSCALL:
							break;
						case OPC_RH850_TRAP:
							break;
						case OPC_RH850_PREF:
							break;
						case OPC_RH850_POPSP:
							break;
						case OPC_RH850_PUSHSP:
							break;

					}
					break;
				case OPC_RH850_MUL_INSTS:		//MUL instructions
					if (extract32(ctx->opcode, 22, 1) == 0){
						if (extract32(ctx->opcode, 21, 1) == 0){
							//SASF
						} else {
							if (extract32(ctx->opcode, 17, 1) == 1){
								gen_arithmetic(ctx, 0, rs1, rs2, 22);// MULU in format XI
							} else {
								// MUL in format XI
							}

						}
						break;
					} else if (extract32(ctx->opcode, 22, 1) == 1){
						if (extract32(ctx->opcode, 17, 1) == 1){
							gen_arithmetic(ctx, 0, rs1, rs2, 23);// MULU in format XII
						} else {
							// MUL in format XII
						}
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
							if (extract32(ctx->opcode, 19, 2) == 0x0){
								//SCH1R
							} else if (extract32(ctx->opcode, 19, 2) == 0x3){
								//STCW
							}

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

				case OPC_RH850_ADDIT_ARITH:
					formXop = extract32(ctx->opcode, 21, 2);
					switch(formXop){

						case OPC_RH850_ADF_SATADD3:
							if (extract32(ctx->opcode, 16, 5) == 0x1A){
								gen_arithmetic(ctx, 0, rs1, rs2, 26);
								// SATADD3 (format XI)
							} else {
								// ADF
							}
							break;
						case OPC_RH850_SBF_SATSUB2:
							if (extract32(ctx->opcode, 16, 5) == 0x1A){
								// SATSUB2 (format XI)
								gen_arithmetic(ctx, 0, rs1, rs2, 28);
							} else {
								// SBF
							}
							break;
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
				//JARL1
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
			gen_arithmetic(ctx, 0, rs1, rs2, 0);	// MOV Format 1
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
				gen_arithmetic(ctx, 0, rs1, rs2, 12);
				break;
			}
		}
		break;
	case OPC_RH850_BCOND: //BCOND disp9
		decode_branch_operations(ctx, 0, rs1, rs2, 1);
		break;

	case OPC_RH850_16bit_4:
		if (rs2 == 0){
			//ZXB
			decode_data_manipulation(ctx,0,rs1,rs2,2);
			break;
		} else {
			//SATSUBR (calling SATSUB reg1, reg2 but with switched registers)
			gen_arithmetic(ctx, 0, rs2, rs1, 27);
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
			gen_arithmetic(ctx, 0, rs1, rs2, 27);
			break;
		}
		break;
	case OPC_RH850_16bit_6:
		if (rs2 == 0){
			//ZXH
			decode_data_manipulation(ctx,0,rs1,rs2,1);
			break;
		} else {
			//SATADD1 (format I)
			gen_arithmetic(ctx, 0, rs1, rs2, 24);
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
			gen_arithmetic(ctx, 0, rs1,rs2, 8);
			break;
		}
		break;
	case OPC_RH850_NOT:
		gen_logical(ctx, rs1, rs2, OPC_RH850_NOT);
		break;
	case OPC_RH850_16bit_3:
		if (rs2 == 0){
			gen_jmp(ctx, rs1, rs2, 0);
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
		gen_logical(ctx, rs1, rs2, OPC_RH850_OR);
		break;
	case OPC_RH850_XOR:
		gen_logical(ctx, rs1, rs2, OPC_RH850_XOR);
		break;
	case OPC_RH850_AND:
		gen_logical(ctx, rs1, rs2, OPC_RH850_AND);
		break;
	case OPC_RH850_TST:
		gen_logical(ctx, rs1, rs2, OPC_RH850_TST);
		break;
	case OPC_RH850_SUBR:
		gen_arithmetic(ctx, 0, rs1,rs2, 3);
		break;
	case OPC_RH850_SUB:
		gen_arithmetic(ctx, 0, rs1,rs2, 2);
		break;
	case OPC_RH850_ADD:
		gen_arithmetic(ctx, 0, rs1,rs2, 1);
		break;
	case OPC_RH850_CMP_reg1_reg2:
		break;
	case OPC_RH850_16bit_16:
		if (rs2 == 0){
			//CALLT
			break;
		} else {
			gen_arithmetic(ctx, 0, imm, rs2, 16);	//MOV format II
			break;
		}
		break;
	case OPC_RH850_16bit_17:
		if (rs2 == 0){
			//CALLT
			break;
		} else {
			//SATADD2 (format II)
			printf("tu smo v satadd2 \n");
			gen_arithmetic(ctx, 0, rs1, rs2, 25);
			break;
		}
		break;
	case OPC_RH850_16bit_ADD:
		gen_arithmetic(ctx, 0, rs1,rs2, 6);	//add format II
		break;
	case OPC_RH850_CMP_imm5_reg2:
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
		gen_arithmetic(ctx, 0, rs1,rs2, 9);
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

    tcg_gen_movi_i32(cpu_sysRegs[4], 0x0);

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
        gen_reset_flags(&ctx);		//function that resets flags in ctx

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
			decode_RH850_16(env, &ctx);		//be careful about JR and JARL (32-bit FORMAT VI)
        } else {
        	ctx.opcode = (ctx.opcode) | (cpu_lduw_code(env, ctx.pc+2) << 0x10);
        	if ( (extract32(ctx.opcode, 6, 11) == 0x41e) ||	//bits are 10000011110
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

        //tcg_gen_ori_i32(cpu_sysRegs[4],cpu_sysRegs[4], (ctx.Z_flag << 0));

        tcg_gen_shli_i32(cpu_sysRegs[PSW_register], cpu_SF, 0x1);
        tcg_gen_or_i32(cpu_sysRegs[PSW_register],cpu_sysRegs[PSW_register],cpu_ZF);
        //tcg_gen_ori_i32(cpu_sysRegs[4],cpu_sysRegs[4], (ctx.OV_flag << 2));
        //tcg_gen_ori_i32(cpu_sysRegs[4],cpu_sysRegs[4], (ctx.CY_flag << 3));
        //tcg_gen_ori_i32(cpu_sysRegs[4],cpu_sysRegs[4], (ctx.SAT_flag << 4));


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
