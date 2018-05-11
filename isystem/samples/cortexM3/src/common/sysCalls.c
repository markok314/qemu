#include <sys/stat.h>

extern "C" {
  
int g_exitStatus = 0; 

int close(int file)
{
  return -1;
}

int fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int isatty(int file)
{
  return 1;
}

int lseek(int file, int ptr, int dir)
{
  return 0;
}

int read(int file, char *ptr, int len)
{
  return 0;
}

caddr_t sbrk(int incr)
{
  extern char end;
  static char *heap_end;
  char *prev_heap_end;
  if (heap_end == 0)
  {
    heap_end = &end;
  }
  prev_heap_end = heap_end;
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int write(int file, char *ptr, int len)
{
  return 0;
}


void kill()
{
}

void getpid() 
{
}

void _exit(int status)
{
  g_exitStatus = status;
  for(;;);
}

void __init() {
}

}
