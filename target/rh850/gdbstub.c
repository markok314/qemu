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

// mapping of winIDEA register index (mail from Matic 2019-04-16) - 45
// to env->sysBasicRegs() index.
const int winIdeaRegIdx2qemuSysRegIdx[] = {0, 12, 13, 14, 15, 0, 16, 17};

int rh850_cpu_gdb_read_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;

    if (n < 32) {
        return gdb_get_regl(mem_buf, env->progRegs[n]);  //gpr is now supposed to be progRegs
    } else if (n > 31  &&  n < 36) {
        return gdb_get_regl(mem_buf, env->sysBasicRegs[n - 32]); // eipc, eipsw, fepc, fepsw
    } else if (n == 36) {
        return gdb_get_regl(mem_buf, env->sysBasicRegs[psw_idx]); // psw
    } else if (n >= 38   &&  n <= 45) {
        return gdb_get_regl(mem_buf, env->sysBasicRegs[winIdeaRegIdx2qemuSysRegIdx[n - 38]]);
    } else if (n == 46) {
        return gdb_get_regl(mem_buf, env->pc);
    } else if (78) { // asid
        return gdb_get_regl(mem_buf, env->sysBasicRegs[asid_idx]);
    }

    *((uint32_t *)mem_buf) = 0xBAD00BAD;
    return 4; // registers in slots not set above are ignored
}

int rh850_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    RH850CPU *cpu = RH850_CPU(cs);
    CPURH850State *env = &cpu->env;
    // at the moment our GDB server has different indices for writing single register
    // will fix this if batch write will have to be supported or interfacing
    // to other GDB servers for RH850 will be needed.
    if (n < 32) {
        env->progRegs[n] = ldtul_p(mem_buf);
    } else if (n > 31  &&  n < 36) {
        env->sysBasicRegs[n - 32] = ldtul_p(mem_buf); // eipc, eipsw, fepc, fepsw
    } else if (n == 37) {
        env->sysBasicRegs[psw_idx] = ldtul_p(mem_buf);
    } else if (n >= 45   &&  n <= 52) {
        env->sysBasicRegs[winIdeaRegIdx2qemuSysRegIdx[n - 45]] = ldtul_p(mem_buf);
    } else if (n == 64) {
        env->pc = ldtul_p(mem_buf);
    } else if (189) { // asid
        env->sysBasicRegs[asid_idx] = ldtul_p(mem_buf);
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
