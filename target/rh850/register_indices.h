/*
 * register_indices.h
 *
 *  Created on: Jun 18, 2018
 *
 */

#ifndef TARGET_RH850_REGISTER_INDICES_H_
#define TARGET_RH850_REGISTER_INDICES_H_


	// sysBasicRegs indices
#define psw_idx	 	4	//program status word
#define eiic_idx	11	//EI level exception cause
#define feic_idx	12	//FI level exception cause
#define mcfg0_idx	19	//machine configuration
#define rbase_idx	20	//reset vector base address (if psw.ebv==0, this is also exception vector)
#define ebase_idx	21	//exception handler vector address
#define mctl_idx	23	//CPU control
#define pid_idx		24	//processor ID
#define htcfg0_idx	27	//thread configuration
#define mea_idx		28	//memory error address (when misaligned or MPU)
#define asid_idx	29	//memory error address (when misaligned or MPU)
#define mei_idx		30	//memory error info (info about instruction that caused exception)

	// sysInterruptRegs indices
#define icsr_idx	3	//interrupt control status register
#define intcfg_idx	4	//interrupt function setting

	// sysFpuRegs indices
#define fpsr_idx	0	//floating-point configuration/status   <---write the bit defines
#define fpepc_idx	1	//floating point exception PC

	// sysMpuRegs indices
#define mpm_idx		0	//memory protection operation mode


#endif /* TARGET_RH850_REGISTER_INDICES_H_ */
