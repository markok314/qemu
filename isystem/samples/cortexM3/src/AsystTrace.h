#ifndef __AsystTrace_h__
#define __AsystTrace_h__

// shared memory functions
void AsystTrace(const char *p);
void AsystPutch(char c);
int AsystKbhit(void);
char AsystGetch(void);

// ITM functions
void AsystTrace_ITM(char port, char *p);
void AsystPutch_ITM(char port, char c);

#endif

