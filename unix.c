/*
  Terminal I/O routines for SEDT (a la UNIX)
*/
#include <stdio.h>
#include <stdlib.h>
#if defined(ULTRIX)
#include <sys/ioctl.h>
#include <sgtty.h>
#else
#include <sys/ioctl.h>
#if defined(CONVEX)
#include <sys/termios.h>
#else
#include <termio.h>
#endif
#if defined(SGI)
#include <sys/ttold.h>
#endif
#endif

int     tty;
#if defined(ULTRIX)
struct	sgttyb	restore, special;
#else
#if defined(CONVEX)
struct  termios  restore, special;
#else
struct  termio  restore, special;
#endif
#endif

#if defined(GPM)
#include "gpm_mouse.h"
static int gpm=0;
#endif

UInit()
{
    /*char    *ttyname();*/

/*
  Open up the terminal device user is on.  Get the characteristics
  of this terminal, make a copy, and set for no post-processing,
  no echo, no canonical input processing, and process each character
  when typed rather than await a new-line.
*/
/*printf("DEBUG: ttyname(fileno(stdin)) = %s\n", ttyname(fileno(stdin)) );
*/
    if((tty = open(ttyname(fileno(stdin)), 2)) <= 0)
    {
    	perror("Can't attach the terminal");
    	exit(0);
    }
#if defined(ULTRIX)
    ioctl(tty, TIOCGETP, &restore);
#else
    ioctl(tty, TCGETA, &restore);
#endif
    special = restore;
#if defined(ULTRIX)
    special.sg_flags &= ~(ECHO | CRMOD);
    special.sg_flags |= CBREAK;
    ioctl(tty, TIOCSETP, &special);
#else
    special.c_iflag &= ~ICRNL;
    special.c_oflag &= ~OPOST;
    special.c_lflag &= ~(ECHO | ICANON | ISIG);
    special.c_cc[VEOF] = 1;
    ioctl(tty, TCSETA, &special);
#endif

#if defined(GPM)
    gpm = gpm_init(stdin);
#endif

}

UEnd()
{

/*
  Restore terminal attributes and close.
*/
#if defined(ULTRIX)
    ioctl(tty, TIOCSETP, &restore);
#else
    ioctl(tty, TCSETA, &restore);
#endif
    close(tty);
    
#if defined(GPM)
    if (gpm) gpm = gpm_fini();
#endif
    

}

USysenv()
{

/*
  Restore terminal attributes prior to Spawning a command.
*/
#if defined(ULTRIX)
    ioctl(tty, TIOCSETP, &restore);
#else
    ioctl(tty, TCSETA, &restore);
#endif
    close(tty);
}

USedtenv()
{
/*    char    *ttyname();*/

/*
  Set terminal attributes back to Sedt specifications.
*/
/*printf("DEBUG: ttyname(fileno(stdin)) = %s\n", ttyname(fileno(stdin)) );
*/
    if((tty = open(ttyname(fileno(stdin)), 2)) <= 0)
    {
    	perror("Can't attach the terminal");
    	exit(0);
    }
#if defined(ULTRIX)
    ioctl(tty, TIOCSETP, &special);
#else
    ioctl(tty, TCSETA, &special);
#endif
}

UGetrm()
{
unsigned
    char    key;

#if defined(GPM)
    int ch;
    if (gpm)
    {
        if ( (ch = gpm_getc()) == EOF)
        {
            perror("Can't read from gpm/terminal");
            exit(0);
        }
        return ch;
    }
#endif

    if((read(tty, &key, 1)) < 1)
    {
    	perror("Can't read from terminal");
    	exit(0);
    }
    return((int)key);
}

UPutrm(string, length)
char    *string;
int     length;
{
    write(tty, string, length);
}

#if defined(SUN)
char _toupper(C)
char C;
{return toupper(C);
}

char _tolower(C)
char C;
{return tolower(C);
}
#endif
