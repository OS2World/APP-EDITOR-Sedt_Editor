#if defined(VMS)
#define Recsize 32*1024
#define Blksize 32*1024
#include <rms.h>
struct File
 {struct RAB *Handle;
  char *Parsed_Name;
  unsigned char Parsed_Name_Len;
  int Point, Length, Access;
  char Record[Recsize];
 };
#else
#define Recsize 64
#if defined(ATARI)
#define Blksize 8192
#else
#if defined(MPE)
#define Blksize 512
#else
#define Blksize 2048
#endif
#endif
#if defined(MPE)
struct File
 {FILE *Handle;
  int Point, Length, Access;
  char Record[Recsize];
 };
#else
struct File
 {int Handle, Point, Length, Access;
  char Record[Recsize];
 };
#endif
#endif
