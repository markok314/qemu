/*
 * RH850 GDB Server Stub
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
#include "qemu-common.h"
#include "exec/gdbstub.h"
#include "cpu.h"

/* Mapping of winIDEA register index to env->sysBasicRegs() index. (see mail
 * from Matic 2019-05-06 and isystem/doc/v850-tdep.c)
       QEMU idx             wI idx
        32, //      eipc      0
        33, //      eipsw     1
        34, //      fepc      2
        35, //      fepsw     3
        37, //      psw       4
       128, //      fpsr      5
       129, //      fpepc     6
       130, //      fpst      7
       131, //      fpcc      8
       132, //      fpcfg     9
       133, //      fpec     10
        44, //      SESR     NA
        45, //      EIIC     11
        46, //  	FEIC     12
        48, //      CTPC     13
        49, //      CTPSW    14
        52, //      CTBP     15
        60, //      EIWR     16
        61, //      FEWR     17
        63, //      BSEL     18
       150, //      mcfg0    19
       152, //	    RBASE    20
       153, //      EBASE    21
       154, //      intbp    22
       155, //      mctl	 23
       156, //      pid      24
       161, //      sccfg    25
       162, //      scbp     26
       182, //      htcfg0   27
       188, //      mea      28
       189, //      asid     29
       190  //      mei      30
*/
typedef int IdxType;
const IdxType winIdeaRegIdx2qemuSysRegIdx[] = {
//     1     2     3     4     5     6     7     8     9
// ---------------------------------------------
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  0
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  1
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  2

-1,   -1,    0,    1,    2,    3,   -1,    4,   -1,   -1, //  3
-1,   -1,   -1,   -1,   -1,   11,   12,   -1,   13,   14, //  4
-1,   -1,   15,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  5

16,   17,   -1,   18,   -1,   -1,   -1,   -1,   -1,   -1, //  6
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  7
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  8

-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  9
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // 10
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // 11

-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,    5,    6, // 12
 7,    8,    9,   10,   -1,   -1,   -1,   -1,   -1,   -1, // 13
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // 14

19,   -1,   20,   21,   22,   23,   24,   -1,   -1,   -1, // 15
-1,   25,   26,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // 16
-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // 17

-1,   -1,   27,   -1,   -1,   -1,   -1,   -1,   28,   29, // 18
30,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // 19
};

const int NUM_GDB_REGS = sizeof(winIdeaRegIdx2qemuSysRegIdx) / sizeof(IdxType);

int rh850_cpu_gdb_read_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;

    if (n < 32) {
        return gdb_get_regl(mem_buf, env->progRegs[n]);  //gpr is now supposed to be progRegs
    } else if (n == 64) {
        return gdb_get_regl(mem_buf, env->pc);
    } else if (n < NUM_GDB_REGS) {
    	int sysRegIdx = winIdeaRegIdx2qemuSysRegIdx[n];
    	if (sysRegIdx >= 0) {
            return gdb_get_regl(mem_buf, env->sysBasicRegs[sysRegIdx]); // eipc, eipsw, fepc, fepsw, psw, ...
    	}
    }

    *((uint32_t *)mem_buf) = 0xBAD0BAD0;
    return 4; // registers in slots not set above are ignored
}

int rh850_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;
    // at the moment our GDB server has different indices for writing single register
    // will fix this if batch write will have to be supported or interfacing
    // to other GDB servers for RH850 will be needed.
    if (n > 0  &&  n < 32) {  // skip R0, because it is always 0
        env->progRegs[n] = ldtul_p(mem_buf);
    } else if (n == 64) {
        env->pc = ldtul_p(mem_buf);
    } else if (n < NUM_GDB_REGS) {
    	int sysRegIdx = winIdeaRegIdx2qemuSysRegIdx[n];
    	if (sysRegIdx >= 0) {
    	    env->sysBasicRegs[sysRegIdx] = ldtul_p(mem_buf); // eipc, eipsw, fepc, fepsw, psw, ...
    	}
    }

    /* arm example
    if (n == 0) {
       ...
    } else if (n < 65) {
        env->fpr[n - 33] = ldq_p(mem_buf); // always 64-bit
        return sizeof(uint64_t);
    } else if (n < 4096 + 65) {
        csr_write_helper(env, ldtul_p(mem_buf), n - 65);
    }
*/
    return sizeof(target_ulong);
}
