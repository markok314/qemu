/*
 * register_indices.h
 *
 *  Created on: Jun 18, 2018
 *
 */

#ifndef TARGET_RH850_REGISTER_INDICES_H_
#define TARGET_RH850_REGISTER_INDICES_H_


	// sysBasicRegs indices
#define EIPC_IDX     0
#define EIPSW_IDX    1
#define FEPC_IDX     2
#define FEPSW_IDX    3
#define PSW_IDX	 	 4	//program status word
#define EIIC_IDX	11	//EI level exception cause
#define FEIC_IDX	12	//FI level exception cause
#define CTPC_IDX    13
#define CTPSW_IDX   14
#define CTBP_IDX    15
#define EIWR_IDX    16
#define FEWR_IDX    17
#define BSEL_IDX    18
#define MCFG0_IDX	19	//machine configuration
#define RBASE_IDX	20	//reset vector base address (if psw.ebv==0, this is also exception vector)
#define EBASE_IDX	21	//exception handler vector address
#define INTBP_IDX   22
#define MCTL_IDX	23	//CPU control
#define PID_IDX		24	//processor ID
#define SCCFG_IDX   25  // SYSCALL config
#define SCBP_IDX    26  // SYSCALL base pointer
#define HTCFG0_IDX	27	//thread configuration
#define MEA_IDX		28	//memory error address (when misaligned or MPU)
#define ASID_IDX	29	//memory error address (when misaligned or MPU)
#define MEI_IDX		30	//memory error info (info about instruction that caused exception)

	// sysInterruptRegs indices
#define FPIPR_IDX   0
#define ISPR_IDX    1
#define PMR_IDX     2
#define ICSR_IDX	3	//interrupt control status register
#define INTCFG_IDX	4	//interrupt function setting

	// sysFpuRegs indices
#define FPSR_IDX	0	//floating-point configuration/status   <---write the bit defines
#define FPEPC_IDX	1	//floating point exception PC

	// sysMpuRegs indices
#define MPM_IDX		0	//memory protection operation mode


#endif /* TARGET_RH850_REGISTER_INDICES_H_ */
