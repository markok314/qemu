/*
 * RH850 emulation for qemu: Instruction decode helpers
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

#define MASK_OP_MAJOR(op)  (op & (0x3F << 5))	//most instructions for rh850 have opcodes at bits 5-10
enum {

	OPC_RH850_LDB 	  = (0x38 << 5),
	OPC_RH850_LDH_LDW = (0x39 << 5),
	OPC_RH850_STB 	  = (0x3A << 5),
	OPC_RH850_STH_STW = (0x3B << 5), 	//the store halfword and store word instructions differ on LSB displacement bit 16 (0=ST.H, 1=ST.W) (format VII)

	OPC_RH850_ST_LD_0 = (0x3C << 5), 	//5 instructions share this opcode, sub-op bits 11-15 are 0, inst. differ in sub-op bits 16-19 (ST.B2=D, ST.W2=F) (format XIV)
	OPC_RH850_ST_LD_1 = (0x3D << 5), 	//5 instructions share this opcode, sub-op bits 11-15 are 0, inst. differ in sub-op bits 16-19 (ST.DW=F, ST.H2=D) (format XIV)
	//OPC_RH850_LDHU  = (0x3F << 5),	//bits 11-15 are not all 0

	/* FORMAT I */						// unique opcodes and grouped instructions
	OPC_RH850_16bit_0 = (0x0 << 5),		// group with opcode 0x0 (nop, synci, synce, syncm, syncp, mov)
	OPC_RH850_16bit_2 = (0x2 << 5),		// group with opcode 0x2 (rie, switch, divh, fetrap)
	OPC_RH850_16bit_4 = (0x4 << 5),		// group with opcode 0x4 (zyb, satsub)
	OPC_RH850_16bit_5 = (0x5 << 5),		// group with opcode 0x5 (sxb, satsub)
	OPC_RH850_16bit_6 = (0x6 << 5),		// group with opcode 0x6 (zyh, satadd)
	OPC_RH850_16bit_7 = (0x7 << 5),		// group with opcode 0x7 (sxh, mulh)
	OPC_RH850_NOT 	= (0x1 << 5),
	OPC_RH850_JMP 	= (0x3 << 5),
	OPC_RH850_OR 	= (0x8 << 5),
	OPC_RH850_XOR 	= (0x9 << 5),
	OPC_RH850_AND 	= (0xA << 5),
	OPC_RH850_TST 	= (0xB << 5),
	OPC_RH850_SUBR 	= (0xC << 5),
	OPC_RH850_SUB 	= (0xD << 5),
	OPC_RH850_ADD 	= (0xE << 5),
	OPC_RH850_CMP 	= (0xF << 5),
	OPC_RH850_16bit_16 = (0x10 << 5),	// group with opcode 0x10 (mov,callt)
	OPC_RH850_16bit_17 = (0x11 << 5),	// group with opcode 0x11 (callt, satadd)
	OPC_RH850_16bit_ADD= (0x12 << 5),    // group with opcode 0x12 (add)
	OPC_RH850_16bit_CMP = (0x13 << 5),	// group with opcode 0x13 (cmp)
	OPC_RH850_16bit_SHR = (0x14 << 5),	// group with opcode 0x14 (shr)
	OPC_RH850_16bit_SAR = (0x15 << 5),	// group with opcode 0x15 (sar)
	OPC_RH850_16bit_SHL = (0x16 << 5),	// group with opcode 0x16 (shl)
	OPC_RH850_16bit_MULH = (0x17 << 5),	// group with opcode 0x17 (mulh)

	OPC_RH850_32bit_1 = (0x3F << 5),	//this is 111111


	/* FORMAT II */
	OPC_RH850_ADD2 = (0x12 << 5),
	OPC_RH850_CALLT_0 = (0x10 << 5),	//bits 15-11 are 0
	OPC_RH850_CALLT_1 = (0x11 << 5),	//bits 15-11 are 0
	OPC_RH850_CMP2 = (0x13 << 5),
	OPC_RH850_MOV2 = (0x10 << 5), 	 	//bits 15-11 are not all 0
	OPC_RH850_MULH	= (0x17 << 5),
	OPC_RH850_SAR = (0x15 << 5),
	OPC_RH850_SATADD2 = (0x11 << 5),	//bits 11-15 are not all 0
	OPC_RH850_SHL = (0x16 << 5),
	OPC_RH850_SHR = (0x14 << 5),


	/* FORMAT VI */
	OPC_RH850_ADDI	=	(0x30 << 5),
	OPC_RH850_ANDI	=	(0x36 << 5),
	OPC_RH850_MOVEA	=	(0x31 << 5),
	OPC_RH850_MOVHI	=	(0x32 << 5),

	/* FORMAT VII */

	OPC_RH850_LOOP	=	(0x37 << 5),

	/* FORMAT VIII */

	OPC_RH850_CLR	=	(0x3E << 5),

	////////////////////////////////////////

    OPC_RISC_LUI    = (0x37),
    OPC_RISC_AUIPC  = (0x17),
    OPC_RISC_JAL    = (0x6F),
    OPC_RISC_JALR   = (0x67),
    OPC_RISC_BRANCH = (0x63),
    OPC_RISC_LOAD   = (0x03),
    OPC_RISC_STORE  = (0x23),
    OPC_RISC_ARITH_IMM  = (0x13),
    OPC_RISC_ARITH      = (0x33),
    OPC_RISC_FENCE      = (0x0F),
    OPC_RISC_SYSTEM     = (0x73),

    /* rv64i, rv64m */
    OPC_RISC_ARITH_IMM_W = (0x1B),
    OPC_RISC_ARITH_W = (0x3B),

    /* rv32a, rv64a */
    OPC_RISC_ATOMIC = (0x2F),

    /* floating point */
    OPC_RISC_FP_LOAD = (0x7),
    OPC_RISC_FP_STORE = (0x27),
	OPC_RISC_FMADD = (0x0),
	OPC_RISC_FMSUB, OPC_RISC_FNMADD, OPC_RISC_FNMSUB, OPC_RISC_FP_ARITH = (0x0),

	OPC_RH850_MULH1 = (0x7 << 5),		//DOUBLE CHECK OP CODE
	OPC_RH850_MULH2 = (0x17 << 5),		//bits 15-11 are not all 0


};

#define MASK_OP_FORMAT_I_0(op)	(MASK_OP_MAJOR(op) | (op & (0x1F << 11)) | (op & (0x1F << 0)))
enum {
	OPC_RH850_NOP 	= OPC_RH850_16bit_0 | (0x0 << 11) | (0x0 << 0),
	OPC_RH850_SYNCI = OPC_RH850_16bit_0 | (0x0 << 11) | (0x1C << 0),
	OPC_RH850_SYNCE = OPC_RH850_16bit_0 | (0x0 << 11) | (0x1D << 0),
	OPC_RH850_SYNCM = OPC_RH850_16bit_0 | (0x0 << 11) | (0x1E << 0),
	OPC_RH850_SYNCP = OPC_RH850_16bit_0 | (0x0 << 11) | (0x1F << 0)
};



#define MASK_OP_ST_LD0(op)   (MASK_OP_MAJOR(op) | (op & (0x1F << 11)) | (op & (0xF << 16)))
enum {

	OPC_RH850_LDB2 	= OPC_RH850_ST_LD_0 | (0x00 << 11 ) | (0x5 << 16),
	OPC_RH850_LDH2 	= OPC_RH850_ST_LD_0 | (0x00 << 11 ) | (0x7 << 16),
	OPC_RH850_LDW2 	= OPC_RH850_ST_LD_0 | (0x00 << 11 ) | (0x9 << 16),
	OPC_RH850_STB2 	= OPC_RH850_ST_LD_0 | (0x00 << 11 ) | (0xD << 16),		//sub-op bits 11-15 are 0, inst. differ in sub-op bits 16-19 (ST.B2=D, ST.W2=F) (format XIV)
	OPC_RH850_STW2	= OPC_RH850_ST_LD_0 | (0x00 << 11 ) | (0xF << 16),

};
#define MASK_OP_ST_LD1(op)   (MASK_OP_MAJOR(op) | (op & (0x1F << 11)) | (op & (0xF << 16)))
enum {

	OPC_RH850_LDBU2 = OPC_RH850_ST_LD_1 | (0x00 << 11 ) | (0x5 << 16),
	OPC_RH850_LDHU2 = OPC_RH850_ST_LD_1 | (0x00 << 11 ) | (0x7 << 16),
	OPC_RH850_LDDW 	= OPC_RH850_ST_LD_1 | (0x00 << 11 ) | (0x9 << 16),
	OPC_RH850_STDW 	= OPC_RH850_ST_LD_1 | (0x00 << 11 ) | (0xF << 16),
	OPC_RH850_STH2 	= OPC_RH850_ST_LD_1 | (0x00 << 11 ) | (0xD << 16),
};

#define MASK_OP_32BIT_SUB(op)	(op & (0xF << 23))
enum {
	OPC_RH850_LDSR		=	(0x0 << 23),
	OPC_RH850_FORMAT_IX	=	(0x1 << 23),
	OPC_RH850_FORMAT_X	=	(0x2 << 23),
	OPC_RH850_MUL_INSTS	=	(0x4 << 23),
	OPC_RH850_FORMAT_XI	=	(0x5 << 23),
	OPC_RH850_FORMAT_XII=	(0x6<< 23),
};

#define MASK_OP_FORMAT_X(op) (op & (0xFFF << 11))
enum {
	OPC_RH850_CLL 	= (0xC1F << 11),
	OPC_RH850_CTRET	= (0x880 << 11),
	OPC_RH850_DI	= (0xC00 << 11),
	OPC_RH850_EI	= (0XC10 << 11),
	OPC_RH850_EIRET	= (0X900 << 11),
	OPC_RH850_FERET	= (0X940 << 11),
	OPC_RH850_HALT	= (0X400 << 11),
	OPC_RH850_JARL	= (0XC18 << 11)
};

#define MASK_OP_FORMAT_IX(op) (op & (0x7 << 21))
enum {
	OPC_RH850_BINS_0	= (0x4  << 21),
	OPC_RH850_BINS_1	= (0x5  << 21),
	OPC_RH850_BINS_2	= (0x6  << 21),
	OPC_RH850_CLR1		= (0x7  << 21),
	//OPC_RH850_LDSR		= (0x1  << 21),
};



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

#define MASK_OP_ARITH(op)   (MASK_OP_MAJOR(op) | (op & ((0x7 << 12) | (0x7F << 25))))
enum {
    OPC_RISC_ADD   = OPC_RISC_ARITH | (0x0 << 12) | (0x00 << 25),
    OPC_RISC_SUB   = OPC_RISC_ARITH | (0x0 << 12) | (0x20 << 25),
    OPC_RISC_SLL   = OPC_RISC_ARITH | (0x1 << 12) | (0x00 << 25),
    OPC_RISC_SLT   = OPC_RISC_ARITH | (0x2 << 12) | (0x00 << 25),
    OPC_RISC_SLTU  = OPC_RISC_ARITH | (0x3 << 12) | (0x00 << 25),
    OPC_RISC_XOR   = OPC_RISC_ARITH | (0x4 << 12) | (0x00 << 25),
    OPC_RISC_SRL   = OPC_RISC_ARITH | (0x5 << 12) | (0x00 << 25),
    OPC_RISC_SRA   = OPC_RISC_ARITH | (0x5 << 12) | (0x20 << 25),
    OPC_RISC_OR    = OPC_RISC_ARITH | (0x6 << 12) | (0x00 << 25),
    OPC_RISC_AND   = OPC_RISC_ARITH | (0x7 << 12) | (0x00 << 25),

    /* RV64M */
    OPC_RISC_MUL    = OPC_RISC_ARITH | (0x0 << 12) | (0x01 << 25),
    OPC_RISC_MULH   = OPC_RISC_ARITH | (0x1 << 12) | (0x01 << 25),
    OPC_RISC_MULHSU = OPC_RISC_ARITH | (0x2 << 12) | (0x01 << 25),
    OPC_RISC_MULHU  = OPC_RISC_ARITH | (0x3 << 12) | (0x01 << 25),

    OPC_RISC_DIV    = OPC_RISC_ARITH | (0x4 << 12) | (0x01 << 25),
    OPC_RISC_DIVU   = OPC_RISC_ARITH | (0x5 << 12) | (0x01 << 25),
    OPC_RISC_REM    = OPC_RISC_ARITH | (0x6 << 12) | (0x01 << 25),
    OPC_RISC_REMU   = OPC_RISC_ARITH | (0x7 << 12) | (0x01 << 25),
};


#define MASK_OP_ARITH_IMM(op)   (MASK_OP_MAJOR(op) | (op & (0x7 << 12)))   //arithmetic with immediate
enum {
    OPC_RISC_ADDI   = OPC_RISC_ARITH_IMM | (0x0 << 12),
    OPC_RISC_SLTI   = OPC_RISC_ARITH_IMM | (0x2 << 12),
    OPC_RISC_SLTIU  = OPC_RISC_ARITH_IMM | (0x3 << 12),
    OPC_RISC_XORI   = OPC_RISC_ARITH_IMM | (0x4 << 12),
    OPC_RISC_ORI    = OPC_RISC_ARITH_IMM | (0x6 << 12),
    OPC_RISC_ANDI   = OPC_RISC_ARITH_IMM | (0x7 << 12),
    OPC_RISC_SLLI   = OPC_RISC_ARITH_IMM | (0x1 << 12), /* additional part of
                                                           IMM */
    OPC_RISC_SHIFT_RIGHT_I = OPC_RISC_ARITH_IMM | (0x5 << 12) /* SRAI, SRLI */
};

#define MASK_OP_BRANCH(op)     (MASK_OP_MAJOR(op) | (op & (0x7 << 12)))		//branch instructions
enum {
    OPC_RISC_BEQ  = OPC_RISC_BRANCH  | (0x0  << 12),
    OPC_RISC_BNE  = OPC_RISC_BRANCH  | (0x1  << 12),
    OPC_RISC_BLT  = OPC_RISC_BRANCH  | (0x4  << 12),
    OPC_RISC_BGE  = OPC_RISC_BRANCH  | (0x5  << 12),
    OPC_RISC_BLTU = OPC_RISC_BRANCH  | (0x6  << 12),
    OPC_RISC_BGEU = OPC_RISC_BRANCH  | (0x7  << 12)
};

enum {
    OPC_RISC_ADDIW   = OPC_RISC_ARITH_IMM_W | (0x0 << 12),
    OPC_RISC_SLLIW   = OPC_RISC_ARITH_IMM_W | (0x1 << 12), /* additional part of
                                                              IMM */
    OPC_RISC_SHIFT_RIGHT_IW = OPC_RISC_ARITH_IMM_W | (0x5 << 12) /* SRAI, SRLI
                                                                  */
};

enum {
    OPC_RISC_ADDW   = OPC_RISC_ARITH_W | (0x0 << 12) | (0x00 << 25),
    OPC_RISC_SUBW   = OPC_RISC_ARITH_W | (0x0 << 12) | (0x20 << 25),
    OPC_RISC_SLLW   = OPC_RISC_ARITH_W | (0x1 << 12) | (0x00 << 25),
    OPC_RISC_SRLW   = OPC_RISC_ARITH_W | (0x5 << 12) | (0x00 << 25),
    OPC_RISC_SRAW   = OPC_RISC_ARITH_W | (0x5 << 12) | (0x20 << 25),

    /* RV64M */
    OPC_RISC_MULW   = OPC_RISC_ARITH_W | (0x0 << 12) | (0x01 << 25),
    OPC_RISC_DIVW   = OPC_RISC_ARITH_W | (0x4 << 12) | (0x01 << 25),
    OPC_RISC_DIVUW  = OPC_RISC_ARITH_W | (0x5 << 12) | (0x01 << 25),
    OPC_RISC_REMW   = OPC_RISC_ARITH_W | (0x6 << 12) | (0x01 << 25),
    OPC_RISC_REMUW  = OPC_RISC_ARITH_W | (0x7 << 12) | (0x01 << 25),
};

#define MASK_OP_LOAD(op)   (MASK_OP_MAJOR(op) | (op & (0x7 << 12)))
enum {
    OPC_RISC_LB   = OPC_RISC_LOAD | (0x0 << 12),
    OPC_RISC_LH   = OPC_RISC_LOAD | (0x1 << 12),
    OPC_RISC_LW   = OPC_RISC_LOAD | (0x2 << 12),
    OPC_RISC_LD   = OPC_RISC_LOAD | (0x3 << 12),
    OPC_RISC_LBU  = OPC_RISC_LOAD | (0x4 << 12),
    OPC_RISC_LHU  = OPC_RISC_LOAD | (0x5 << 12),
    OPC_RISC_LWU  = OPC_RISC_LOAD | (0x6 << 12),
};

#define MASK_OP_STORE(op)   (MASK_OP_MAJOR(op) | (op & (0x7 << 12)))
enum {
    OPC_RISC_SB   = OPC_RISC_STORE | (0x0 << 12),
    OPC_RISC_SH   = OPC_RISC_STORE | (0x1 << 12),
    OPC_RISC_SW   = OPC_RISC_STORE | (0x2 << 12),
    OPC_RISC_SD   = OPC_RISC_STORE | (0x3 << 12),
};

#define MASK_OP_JALR(op)   (MASK_OP_MAJOR(op) | (op & (0x7 << 12)))
/* no enum since OPC_RISC_JALR is the actual value */

#define MASK_OP_ATOMIC(op) \
    (MASK_OP_MAJOR(op) | (op & ((0x7 << 12) | (0x7F << 25))))
#define MASK_OP_ATOMIC_NO_AQ_RL_SZ(op) \
    (MASK_OP_MAJOR(op) | (op & (0x1F << 27)))

enum {
    OPC_RISC_LR          = OPC_RISC_ATOMIC | (0x02 << 27),
    OPC_RISC_SC          = OPC_RISC_ATOMIC | (0x03 << 27),
    OPC_RISC_AMOSWAP     = OPC_RISC_ATOMIC | (0x01 << 27),
    OPC_RISC_AMOADD      = OPC_RISC_ATOMIC | (0x00 << 27),
    OPC_RISC_AMOXOR      = OPC_RISC_ATOMIC | (0x04 << 27),
    OPC_RISC_AMOAND      = OPC_RISC_ATOMIC | (0x0C << 27),
    OPC_RISC_AMOOR       = OPC_RISC_ATOMIC | (0x08 << 27),
    OPC_RISC_AMOMIN      = OPC_RISC_ATOMIC | (0x10 << 27),
    OPC_RISC_AMOMAX      = OPC_RISC_ATOMIC | (0x14 << 27),
    OPC_RISC_AMOMINU     = OPC_RISC_ATOMIC | (0x18 << 27),
    OPC_RISC_AMOMAXU     = OPC_RISC_ATOMIC | (0x1C << 27),
};

#define MASK_OP_SYSTEM(op)   (MASK_OP_MAJOR(op) | (op & (0x7 << 12)))
enum {
    OPC_RISC_ECALL       = OPC_RISC_SYSTEM | (0x0 << 12),
    OPC_RISC_EBREAK      = OPC_RISC_SYSTEM | (0x0 << 12),
    OPC_RISC_ERET        = OPC_RISC_SYSTEM | (0x0 << 12),
    OPC_RISC_MRTS        = OPC_RISC_SYSTEM | (0x0 << 12),
    OPC_RISC_MRTH        = OPC_RISC_SYSTEM | (0x0 << 12),
    OPC_RISC_HRTS        = OPC_RISC_SYSTEM | (0x0 << 12),
    OPC_RISC_WFI         = OPC_RISC_SYSTEM | (0x0 << 12),
    OPC_RISC_SFENCEVM    = OPC_RISC_SYSTEM | (0x0 << 12),

    OPC_RISC_CSRRW       = OPC_RISC_SYSTEM | (0x1 << 12),
    OPC_RISC_CSRRS       = OPC_RISC_SYSTEM | (0x2 << 12),
    OPC_RISC_CSRRC       = OPC_RISC_SYSTEM | (0x3 << 12),
    OPC_RISC_CSRRWI      = OPC_RISC_SYSTEM | (0x5 << 12),
    OPC_RISC_CSRRSI      = OPC_RISC_SYSTEM | (0x6 << 12),
    OPC_RISC_CSRRCI      = OPC_RISC_SYSTEM | (0x7 << 12),
};

#define MASK_OP_FP_LOAD(op)   (MASK_OP_MAJOR(op) | (op & (0x7 << 12)))
enum {
    OPC_RISC_FLW   = OPC_RISC_FP_LOAD | (0x2 << 12),
    OPC_RISC_FLD   = OPC_RISC_FP_LOAD | (0x3 << 12),
};

#define MASK_OP_FP_STORE(op)   (MASK_OP_MAJOR(op) | (op & (0x7 << 12)))
enum {
    OPC_RISC_FSW   = OPC_RISC_FP_STORE | (0x2 << 12),
    OPC_RISC_FSD   = OPC_RISC_FP_STORE | (0x3 << 12),
};

#define MASK_OP_FP_FMADD(op)   (MASK_OP_MAJOR(op) | (op & (0x3 << 25)))
enum {
    OPC_RISC_FMADD_S = OPC_RISC_FMADD | (0x0 << 25),
    OPC_RISC_FMADD_D = OPC_RISC_FMADD | (0x1 << 25),
};

#define MASK_OP_FP_FMSUB(op)   (MASK_OP_MAJOR(op) | (op & (0x3 << 25)))
enum {
    OPC_RISC_FMSUB_S = OPC_RISC_FMSUB | (0x0 << 25),
    OPC_RISC_FMSUB_D = OPC_RISC_FMSUB | (0x1 << 25),
};

#define MASK_OP_FP_FNMADD(op)   (MASK_OP_MAJOR(op) | (op & (0x3 << 25)))
enum {
    OPC_RISC_FNMADD_S = OPC_RISC_FNMADD | (0x0 << 25),
    OPC_RISC_FNMADD_D = OPC_RISC_FNMADD | (0x1 << 25),
};

#define MASK_OP_FP_FNMSUB(op)   (MASK_OP_MAJOR(op) | (op & (0x3 << 25)))
enum {
    OPC_RISC_FNMSUB_S = OPC_RISC_FNMSUB | (0x0 << 25),
    OPC_RISC_FNMSUB_D = OPC_RISC_FNMSUB | (0x1 << 25),
};

#define MASK_OP_FP_ARITH(op)   (MASK_OP_MAJOR(op) | (op & (0x7F << 25)))
enum {
    /* float */
    OPC_RISC_FADD_S    = OPC_RISC_FP_ARITH | (0x0 << 25),
    OPC_RISC_FSUB_S    = OPC_RISC_FP_ARITH | (0x4 << 25),
    OPC_RISC_FMUL_S    = OPC_RISC_FP_ARITH | (0x8 << 25),
    OPC_RISC_FDIV_S    = OPC_RISC_FP_ARITH | (0xC << 25),

    OPC_RISC_FSGNJ_S   = OPC_RISC_FP_ARITH | (0x10 << 25),
    OPC_RISC_FSGNJN_S  = OPC_RISC_FP_ARITH | (0x10 << 25),
    OPC_RISC_FSGNJX_S  = OPC_RISC_FP_ARITH | (0x10 << 25),

    OPC_RISC_FMIN_S    = OPC_RISC_FP_ARITH | (0x14 << 25),
    OPC_RISC_FMAX_S    = OPC_RISC_FP_ARITH | (0x14 << 25),

    OPC_RISC_FSQRT_S   = OPC_RISC_FP_ARITH | (0x2C << 25),

    OPC_RISC_FEQ_S     = OPC_RISC_FP_ARITH | (0x50 << 25),
    OPC_RISC_FLT_S     = OPC_RISC_FP_ARITH | (0x50 << 25),
    OPC_RISC_FLE_S     = OPC_RISC_FP_ARITH | (0x50 << 25),

    OPC_RISC_FCVT_W_S  = OPC_RISC_FP_ARITH | (0x60 << 25),
    OPC_RISC_FCVT_WU_S = OPC_RISC_FP_ARITH | (0x60 << 25),
    OPC_RISC_FCVT_L_S  = OPC_RISC_FP_ARITH | (0x60 << 25),
    OPC_RISC_FCVT_LU_S = OPC_RISC_FP_ARITH | (0x60 << 25),

    OPC_RISC_FCVT_S_W  = OPC_RISC_FP_ARITH | (0x68 << 25),
    OPC_RISC_FCVT_S_WU = OPC_RISC_FP_ARITH | (0x68 << 25),
    OPC_RISC_FCVT_S_L  = OPC_RISC_FP_ARITH | (0x68 << 25),
    OPC_RISC_FCVT_S_LU = OPC_RISC_FP_ARITH | (0x68 << 25),

    OPC_RISC_FMV_X_S   = OPC_RISC_FP_ARITH | (0x70 << 25),
    OPC_RISC_FCLASS_S  = OPC_RISC_FP_ARITH | (0x70 << 25),

    OPC_RISC_FMV_S_X   = OPC_RISC_FP_ARITH | (0x78 << 25),

    /* double */
    OPC_RISC_FADD_D    = OPC_RISC_FP_ARITH | (0x1 << 25),
    OPC_RISC_FSUB_D    = OPC_RISC_FP_ARITH | (0x5 << 25),
    OPC_RISC_FMUL_D    = OPC_RISC_FP_ARITH | (0x9 << 25),
    OPC_RISC_FDIV_D    = OPC_RISC_FP_ARITH | (0xD << 25),

    OPC_RISC_FSGNJ_D   = OPC_RISC_FP_ARITH | (0x11 << 25),
    OPC_RISC_FSGNJN_D  = OPC_RISC_FP_ARITH | (0x11 << 25),
    OPC_RISC_FSGNJX_D  = OPC_RISC_FP_ARITH | (0x11 << 25),

    OPC_RISC_FMIN_D    = OPC_RISC_FP_ARITH | (0x15 << 25),
    OPC_RISC_FMAX_D    = OPC_RISC_FP_ARITH | (0x15 << 25),

    OPC_RISC_FCVT_S_D = OPC_RISC_FP_ARITH | (0x20 << 25),

    OPC_RISC_FCVT_D_S = OPC_RISC_FP_ARITH | (0x21 << 25),

    OPC_RISC_FSQRT_D   = OPC_RISC_FP_ARITH | (0x2D << 25),

    OPC_RISC_FEQ_D     = OPC_RISC_FP_ARITH | (0x51 << 25),
    OPC_RISC_FLT_D     = OPC_RISC_FP_ARITH | (0x51 << 25),
    OPC_RISC_FLE_D     = OPC_RISC_FP_ARITH | (0x51 << 25),

    OPC_RISC_FCVT_W_D  = OPC_RISC_FP_ARITH | (0x61 << 25),
    OPC_RISC_FCVT_WU_D = OPC_RISC_FP_ARITH | (0x61 << 25),
    OPC_RISC_FCVT_L_D  = OPC_RISC_FP_ARITH | (0x61 << 25),
    OPC_RISC_FCVT_LU_D = OPC_RISC_FP_ARITH | (0x61 << 25),

    OPC_RISC_FCVT_D_W  = OPC_RISC_FP_ARITH | (0x69 << 25),
    OPC_RISC_FCVT_D_WU = OPC_RISC_FP_ARITH | (0x69 << 25),
    OPC_RISC_FCVT_D_L  = OPC_RISC_FP_ARITH | (0x69 << 25),
    OPC_RISC_FCVT_D_LU = OPC_RISC_FP_ARITH | (0x69 << 25),

    OPC_RISC_FMV_X_D   = OPC_RISC_FP_ARITH | (0x71 << 25),
    OPC_RISC_FCLASS_D  = OPC_RISC_FP_ARITH | (0x71 << 25),

    OPC_RISC_FMV_D_X   = OPC_RISC_FP_ARITH | (0x79 << 25),
};

#define GET_B_IMM(inst) ((extract32(inst, 8, 4) << 1) \
                         | (extract32(inst, 25, 6) << 5) \
                         | (extract32(inst, 7, 1) << 11) \
                         | (sextract64(inst, 31, 1) << 12))

#define GET_STORE_IMM(inst) ((extract32(inst, 7, 5)) \
                             | (sextract64(inst, 25, 7) << 5))

#define GET_JAL_IMM(inst) ((extract32(inst, 21, 10) << 1) \
                           | (extract32(inst, 20, 1) << 11) \
                           | (extract32(inst, 12, 8) << 12) \
                           | (sextract64(inst, 31, 1) << 20))


#define GET_RS1(inst)  extract32(inst, 0, 5)		//appropriate for RH850
#define GET_RS2(inst)  extract32(inst, 11, 5)		//appropriate for RH850
#define GET_RS3(inst)  extract32(inst, 27, 5)		//appropriate for RH850
#define GET_DISP(inst) (extract32(inst, 20, 7) | (sextract32(inst, 32, 16) << 7 ) ) //b47-b32 + b26-b20


#define GET_RM(inst)   extract32(inst, 12, 3)
#define GET_RD(inst)   extract32(inst, 7, 5)
#define GET_IMM(inst)  sextract64(inst, 20, 12)

/* RVC decoding macros */
#define GET_C_IMM(inst)             (extract32(inst, 2, 5) \
                                    | (sextract64(inst, 12, 1) << 5))
#define GET_C_ZIMM(inst)            (extract32(inst, 2, 5) \
                                    | (extract32(inst, 12, 1) << 5))
#define GET_C_ADDI4SPN_IMM(inst)    ((extract32(inst, 6, 1) << 2) \
                                    | (extract32(inst, 5, 1) << 3) \
                                    | (extract32(inst, 11, 2) << 4) \
                                    | (extract32(inst, 7, 4) << 6))
#define GET_C_ADDI16SP_IMM(inst)    ((extract32(inst, 6, 1) << 4) \
                                    | (extract32(inst, 2, 1) << 5) \
                                    | (extract32(inst, 5, 1) << 6) \
                                    | (extract32(inst, 3, 2) << 7) \
                                    | (sextract64(inst, 12, 1) << 9))
#define GET_C_LWSP_IMM(inst)        ((extract32(inst, 4, 3) << 2) \
                                    | (extract32(inst, 12, 1) << 5) \
                                    | (extract32(inst, 2, 2) << 6))
#define GET_C_LDSP_IMM(inst)        ((extract32(inst, 5, 2) << 3) \
                                    | (extract32(inst, 12, 1) << 5) \
                                    | (extract32(inst, 2, 3) << 6))
#define GET_C_SWSP_IMM(inst)        ((extract32(inst, 9, 4) << 2) \
                                    | (extract32(inst, 7, 2) << 6))
#define GET_C_SDSP_IMM(inst)        ((extract32(inst, 10, 3) << 3) \
                                    | (extract32(inst, 7, 3) << 6))
#define GET_C_LW_IMM(inst)          ((extract32(inst, 6, 1) << 2) \
                                    | (extract32(inst, 10, 3) << 3) \
                                    | (extract32(inst, 5, 1) << 6))
#define GET_C_LD_IMM(inst)          ((extract32(inst, 10, 3) << 3) \
                                    | (extract32(inst, 5, 2) << 6))
#define GET_C_J_IMM(inst)           ((extract32(inst, 3, 3) << 1) \
                                    | (extract32(inst, 11, 1) << 4) \
                                    | (extract32(inst, 2, 1) << 5) \
                                    | (extract32(inst, 7, 1) << 6) \
                                    | (extract32(inst, 6, 1) << 7) \
                                    | (extract32(inst, 9, 2) << 8) \
                                    | (extract32(inst, 8, 1) << 10) \
                                    | (sextract64(inst, 12, 1) << 11))
#define GET_C_B_IMM(inst)           ((extract32(inst, 3, 2) << 1) \
                                    | (extract32(inst, 10, 2) << 3) \
                                    | (extract32(inst, 2, 1) << 5) \
                                    | (extract32(inst, 5, 2) << 6) \
                                    | (sextract64(inst, 12, 1) << 8))
#define GET_C_SIMM3(inst)           extract32(inst, 10, 3)
#define GET_C_RD(inst)              GET_RD(inst)
#define GET_C_RS1(inst)             GET_RD(inst)
#define GET_C_RS2(inst)             extract32(inst, 2, 5)
#define GET_C_RS1S(inst)            (8 + extract32(inst, 7, 3))
#define GET_C_RS2S(inst)            (8 + extract32(inst, 2, 3))
