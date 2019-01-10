/*Copyright (c) Anker Berg-Sonne, 1985-1990*/
#define Txt_Fun(N,T) extern char *N;
#define Int_Fun(N,V) extern int N;
#define Chr_Fun(N,V) extern char N;
#include "vars.h"
#if defined(ANSIC)
#include "protoa.h"
#else
#include "protoc.h"
#endif
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if defined(DOS)
#include <dos.h>
#include <io.h>
#include <time.h>
#include <process.h>
#if !defined(_MSC_VER)
#include <mem.h>
#include <dir.h>
#else
#include <direct.h>
#endif
#include <conio.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#endif

#if defined(WIN32)
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <dir.h>
#include <sys\stat.h>
#endif

#if defined(NT)
#include <dos.h>
#include <io.h>
#include <time.h>
#include <process.h>
#include <memory.h>
#include <direct.h>
#include <conio.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#endif

#if defined(ATARI)
#include <osbind.h>
#include <basepage.h>
#include <stat.h>
char *strcpy(), *getenv(), *mktemp();
char *index();
FILE *fopen();
#endif

#if defined(OS2)
#include <dos.h>
#include <io.h>
#include <process.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <signal.h>
#endif

#if defined(UNIX)
#if !defined(LINUX)
#define S_IREAD 00400
#define S_IWRITE 00200
#endif
#include <sys/termios.h>
#include <time.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(DECOSF)
#include <sys/ioctl.h>
#endif
#if defined(LINUX) | defined(HPUX)
#include <unistd.h>
#endif
#include <signal.h>		/* for signal() to reset terminal */
#include <memory.h>
#if !(defined(SCO) | defined(_MSC_VER))
#endif
#if defined(TERMCAP)
char *tgoto();
#else
#if defined(ULTRIX)
#include <cursesX.h>
#else
#include <curses.h>
#endif
#endif
#if defined (HPUX)
#include <termio.h>
#endif
#include <unistd.h>
#endif

#if defined(VMS)
#include <signal.h>
#include <descrip.h>
#include <stat.h>
char *Find_Dir(), *Find_Lib();
char *Mbxmsgadr();
#endif

#if !(defined(VMS) | defined(UNIX) | defined(ATARI) | defined(OS2) | defined(NT) | defined(_MSC_VER))
#include <alloc.h>
#endif

#if defined(MPE)
#include <fcntl.h>
#define _toupper(c) toupper(c)
#define _tolower(c) tolower(c)
#endif
