/*
 * wrap.h
 *
 *  Created on: May 30, 2018
 */



#ifdef __cplusplus
extern "C" {
#endif

void disasm_wrap(char *buf, size_t buflen, uint64_t pc, uint64_t inst, int insLen);

#ifdef __cplusplus
}
#endif
