#if defined(WIN32)
#undef WIN32
#endif
#include <windows.h>
#if defined(WIN32)
#undef WIN32
#endif
#define WIN32=1

#include <stdio.h>
#include <stdlib.h>
void  Diag(char *F,...);

#include "win32edt.h"

#define A_Bold 1
#define A_Blink 2
#define A_Underline 4
#define A_Reverse 8
WORD color(int Attrib);

extern HANDLE win32ConsoleHnd;
extern HANDLE win32KeybHnd;

/* SECURITY_ATTRIBUTES security_attributes = {sizeof(SECURITY_ATTRIBUTES),(LPVOID)0,1};
*/

static COORD TopLeft;
static int Rows;
static int Cols;
static int NearEndOffset=0;
void WIN32init()
{
   CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
   int i;
   GetConsoleScreenBufferInfo(win32ConsoleHnd, &csbiInfo);
   TopLeft.X = csbiInfo.dwCursorPosition.X;
   TopLeft.Y = csbiInfo.dwCursorPosition.Y;

   /*  e.g.
   0  bottom
   1
   2
   3  top =4 rows => 3-0+1
   */
   Rows = (int)(csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top)+1 ;
   Cols = (int)(csbiInfo.srWindow.Right  - csbiInfo.srWindow.Left)+1 ;

   for (i = 0; i < Rows-1; i++)
   {
#ifdef __INIT_TEST
      COORD xy;
      GetConsoleScreenBufferInfo(win32ConsoleHnd, &csbiInfo);
      printf("%d X=%d,y=%d",i,(int)csbiInfo.dwCursorPosition.X,(int)csbiInfo.dwCursorPosition.Y);
#endif
      printf(" \n",i);
   }

   if ( TopLeft.Y+Rows > csbiInfo.dwSize.Y )
   {  NearEndOffset=Rows;
      TopLeft.Y = csbiInfo.dwSize.Y-Rows;
   }

   WIN32cursor(0,0);
#ifdef __INIT_TEST
   printf("               X=%d, Y=%d",(int)TopLeft.X, (int)TopLeft.Y);
#endif
}

void WIN32scrend()
{
   SMALL_RECT srctScrollRect, srctClipRect;
   CHAR_INFO chiFill;
   COORD coordDest;

   srctClipRect.Top = (short)Rows-2+TopLeft.Y;
   srctClipRect.Bottom = (short)Rows-1+TopLeft.Y;
   srctClipRect.Left = (short)0+TopLeft.X;
   srctClipRect.Right = (short)Cols-1+TopLeft.X;

   srctScrollRect.Top = (short)Rows-2+TopLeft.Y;
   srctScrollRect.Bottom = (short)Rows-1+TopLeft.Y;
   srctScrollRect.Left = (short)0+TopLeft.X;
   srctScrollRect.Right = (short)Cols-1+TopLeft.X;

   coordDest.X = (short)0+TopLeft.X;
   coordDest.Y = (short)(Rows-3)+TopLeft.Y;

   chiFill.Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
   chiFill.Char.AsciiChar = ' ';

   ScrollConsoleScreenBuffer(
      win32ConsoleHnd,         /* screen buffer handle     */
      &srctScrollRect, /* scrolling rectangle      */
      &srctClipRect,   /* clipping rectangle       */
      coordDest,       /* top left destination cell*/
      &chiFill);       /* fill character and color */
}


void WIN32fini()
{
   int i;
   DWORD dwtemp;

/*   WIN32cursor(Rows+NearEndOffset,0);*/

   WIN32cursor(Rows-2,0);
   SetConsoleTextAttribute(win32ConsoleHnd,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
WIN32scrend();

}

void WIN32cursor(int row, int col)
{ COORD xy;
  xy.Y=(short)row + TopLeft.Y;
  xy.X=(short)col + TopLeft.X;
  SetConsoleCursorPosition( win32ConsoleHnd , xy );
}

void WIN32clrtypeahead()
{
   FlushConsoleInputBuffer( win32ConsoleHnd ) ;
}

int WIN32istypeahead()
{
   INPUT_RECORD in;
   BOOL ok;
   DWORD cInRecords=1;
   DWORD lpcRead;
   int skipping=1;

   do
   {
      ok = PeekConsoleInput( win32KeybHnd, &in, cInRecords, &lpcRead );
      if ( lpcRead < 1 )
      {  return 0;
      }
      else
      if ( in.EventType != KEY_EVENT )
      {  return 1; /* but actually pretending there isn't might be better, depends on why we're being called. */
      }
      else
      if (!in.Event.KeyEvent.bKeyDown)
      {  ok = ReadConsoleInput(  win32KeybHnd, &in, cInRecords, &lpcRead );
      }
      else
      {  int virtkey;
         virtkey = in.Event.KeyEvent.wVirtualKeyCode;
         switch (virtkey)
         {case 0: case 17: case 18: case 93: case 16: case 20:
            ok = ReadConsoleInput(  win32KeybHnd, &in, cInRecords, &lpcRead );
            break;
         default:
            return 1;
         }
      }
   }
   while (1);

}

void WIN32scrclear()
{
   CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

#ifdef _crashed_win98
   DWORD console_size;
   DWORD chars_written;
   COORD coordDest = {0,0};

   console_size = csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
   FillConsoleOutputCharacter( win32ConsoleHnd, (TCHAR) ' ', console_size, coordDest, &chars_written );
   /*
   FillConsoleOutputAttribute( win32ConsoleHnd, csbi.wAttributes,console_size, coordDest, &chars_written );
   */
#endif

#ifdef _didnt_clr_the_region_win98
   int Scroll;
   int one_less;
   GetConsoleScreenBufferInfo(win32ConsoleHnd, &csbiInfo);
   one_less = (int)(csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top) -2;
   WIN32scrup( one_less,(int)csbiInfo.srWindow.Top,(int)csbiInfo.srWindow.Left,(int)csbiInfo.srWindow.Bottom,(int)csbiInfo.srWindow.Right,0);
/*   for (Scroll=0; Scroll<(int)(csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top) +1; Scroll++) */
      WIN32scrup( 1,(int)csbiInfo.srWindow.Top,(int)csbiInfo.srWindow.Left,(int)csbiInfo.srWindow.Bottom,(int)csbiInfo.srWindow.Right,0);
#endif

   int Scroll;
   GetConsoleScreenBufferInfo(win32ConsoleHnd, &csbiInfo);
   WIN32cursor(0,0);
/*   for (Scroll=0; Scroll<(int)(csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top) +1; Scroll++)
      WIN32scrup( 1,(int)csbiInfo.srWindow.Top,(int)csbiInfo.srWindow.Left,(int)csbiInfo.srWindow.Bottom,(int)csbiInfo.srWindow.Right,0);
*/
   for (Scroll=0; Scroll<Rows; Scroll++)
      WIN32scrup( 1,0,0,Rows-1,Cols-1,0);

   WIN32cursor(0,0);
}

void WIN32cleartoeol(int row, int col, int right)
{
/*   CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
   GetConsoleScreenBufferInfo(win32ConsoleHnd, &csbiInfo);
*/
   WIN32scrup( 1,row,col,row,right,0);
}


void WIN32scrup( int Scroll, int top, int left, int bottom, int right, char Attab)
{
/*   CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
*/
   SMALL_RECT srctScrollRect, srctClipRect;
   CHAR_INFO chiFill;
   COORD coordDest;

/*printf("WIN32scrup(%d,%d,%d,%d,%d,%d)\n",Scroll,top,left,bottom,right,Attab);
Dia g(__FILE__".WIN32scrup(%d,%d,%d,%d,%d,%d)",Scroll,top,left,bottom,right,Attab);
*/

/*   GetConsoleScreenBufferInfo(win32ConsoleHnd, &csbiInfo);
*/
   srctClipRect.Top = (short)top+TopLeft.Y;
   srctClipRect.Bottom = (short)bottom+TopLeft.Y;
   srctClipRect.Left = (short)left+TopLeft.X;
   srctClipRect.Right = (short)right+TopLeft.X;

   srctScrollRect.Top = (short)top+TopLeft.Y;
   srctScrollRect.Bottom = (short)bottom+TopLeft.Y;
   srctScrollRect.Left = (short)left+TopLeft.X;
   srctScrollRect.Right = (short)right+TopLeft.X;

   coordDest.X = (short)left+TopLeft.X;
   coordDest.Y = (short)(top-Scroll)+TopLeft.Y;

   chiFill.Attributes = color(Attab);
   chiFill.Char.AsciiChar = ' ';

   ScrollConsoleScreenBuffer(
      win32ConsoleHnd,         /* screen buffer handle     */
      &srctScrollRect, /* scrolling rectangle      */
      &srctClipRect,   /* clipping rectangle       */
      coordDest,       /* top left destination cell*/
      &chiFill);       /* fill character and color */
}

void WIN32scrdown( int Scroll, int top, int left, int bottom, int right, char Attab)
{
   WIN32scrup( - Scroll, top, left, bottom, right, Attab);
}

WORD color(int Attrib)
{
   WORD wAttr=0;
   if (!Attrib)            wAttr = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
   if (Attrib & A_Reverse) wAttr = BACKGROUND_GREEN | FOREGROUND_BLUE;
   if (Attrib & A_Bold)
      if (Attrib & A_Reverse) wAttr = FOREGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE;
      else                    wAttr = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; 

   if (!wAttr) wAttr = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
   return wAttr;
}

void WIN32attrib(int Attrib)
{
   WORD wAttr=0;
   if (!Attrib)            wAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
   if (Attrib & A_Reverse) wAttr = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
   if (Attrib & A_Bold)
      if (Attrib & A_Reverse) wAttr |= BACKGROUND_INTENSITY;
/*      else                    wAttr |= FOREGROUND_INTENSITY; */


   if (!wAttr) wAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

   if (Attrib & A_Blink) ;
   if (Attrib & A_Underline) ;

   wAttr = color(Attrib);
   SetConsoleTextAttribute(win32ConsoleHnd,wAttr);
}


#ifdef __TEST1x
HANDLE win32ConsoleHnd;
HANDLE win32KeybHnd;

void main(int argc, char *argv[])
{
   int Scroll, top, left, bottom, right;
   char buff[256];

   if (argc<6) { printf("usage: main scroll top left bottom right\n"); exit(1); }
   Scroll = atoi(argv[1]);
   top    = atoi(argv[2]);
   left   = atoi(argv[3]);
   bottom = atoi(argv[4]);
   right  = atoi(argv[5]);

   win32ConsoleHnd = GetStdHandle(STD_OUTPUT_HANDLE);
   WIN32cursor(top,left);

   fgets(buff,sizeof(buff),stdin);

/*   WIN32scrup( Scroll, top, left, bottom, right, ' ');
*/

}
#endif

#ifdef __TEST1
HANDLE win32ConsoleHnd;
HANDLE win32KeybHnd;

void main(int argc, char *argv[])
{
   int Scroll, top, left, bottom, right;
   int i,j;
   char buff[256];

   if (argc<6) { printf("usage: main scroll top left bottom right\n"); exit(1); }
   Scroll = atoi(argv[1]);
   top    = atoi(argv[2]);
   left   = atoi(argv[3]);
   bottom = atoi(argv[4]);
   right  = atoi(argv[5]);

   win32ConsoleHnd = GetStdHandle(STD_OUTPUT_HANDLE);
   WIN32cursor(0,0);

   for (i=0;i<40;i++)
   {
      for (j=0;j<20; j++)
      {
         printf("%d ",i);
      }
      printf("\n");
   }

   WIN32cursor(0,0);
   fgets(buff,sizeof(buff),stdin);

   WIN32scrup( Scroll, top, left, bottom, right, ' ');

   fgets(buff,sizeof(buff),stdin);

}
#endif

