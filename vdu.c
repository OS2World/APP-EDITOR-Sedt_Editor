/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
/*
			 Program Logic

	VDU implements  an  algorithm  for  minimizing  character 
transmission to ANSI  VDU terminals.  This is acheived by using a 
database  of  prior  output   to   the   screen  and  eliminating 
repainting.  Cursor positioning is  also  made carefully to avoid 
unnecessary character output.
*/ 

#include "proto.h"
#if defined(WIN32)
#undef WIN32
#include <windows.h>
#define WIN32=1
#endif

#if (defined(DOS) & !defined(Windows)) | defined(OS2) | defined(WIN32)
#define reverse 1
#define nobold 2
#define nounderscore 4
#define noblink 8
#endif

/*Functions*/

/*      Output a single character to the screen. The character is buffered
	and a call to Out_Zap need to be made to ensure that it is physically
        written.
*/
void Fast Out_C(char C)
{if (Out_Count==Out_Size-1)
  Out_Zap();
 Out_String[Out_Count++]=C;
}

/*      Output and integer to the screen. The output is made by calls to
        Out_C.
*/
void Fast Out_I(int I)
{if (I>=10)
 {Out_I(I/10);
  Out_C((char)('0'+I%10));
 } else
 if (I>0)
  Out_C((char)('0'+I));
}

/*      Output a null terminated string to the screen. The characters are 
	buffered and a call to Out_Zap needs to be made to ensure that the
        output is physically writtem.
*/
void Fast Out_S(char *S)
{char C;
 while ((C=(*S++))!=0)
 {if (Out_Count==Out_Size-1)
   Out_Zap();
  Out_String[Out_Count++]=C;
 }
}

/*      Ensure that all output is physically written. In several 
        implementations there are two output streams. One by video firmware
        calls and the other through normal character output routines.
*/
void Fast Out_Zap()
{
#if defined(DOS) & !(defined(Windows) | defined(TV))
 if (Zap_Count)
 {
#if defined(PC100)
  High_Out(St_Line,St_Col,Output,Attr,Zap_Count);
#else
  if (Dir_Video)
   Dir_Out(Top_Row+Sub_X+St_Line-1,Left_Col+Sub_Y+St_Col-1,(char far *)Output,
    Attab[His_Attributes],Zap_Count); else
  {register char *S;
   S=Output;
   while (Zap_Count--)
   {IBMcursor(Top_Row+Sub_X+St_Line-1,Left_Col+Sub_Y+(St_Col++)-1);
    IBMcout(1,*(S++),Attab[His_Attributes]);
   }
   IBMcursor(Top_Row+Sub_X+His_Line-1,Left_Col+Sub_Y+His_Col-1);
  }
#endif
  St_Line=St_Col=Zap_Col=Zap_Line=Zap_Count=0;
 }
#endif
#if defined(OS2)
 if (Zap_Count)
 {VioWrtCharStrAtt((char far *)Output,Zap_Count,Sub_X+St_Line-1+Top_Row,
   Sub_Y+St_Col-1+Left_Col,(char far *)Attab+His_Attributes,0);
  St_Line=St_Col=Zap_Col=Zap_Line=Zap_Count=0;
 }
#endif
#if defined(WIN32)
/*printf("Out_Zap:Zap_Count=%ld,X=%ld,Y=%ld, Output=%s\n",(long)Zap_Count, (long)(Top_Row+Sub_X+St_Line-1), (long)(Left_Col+Sub_Y+St_Col-1),(char*)ddt_s(Output));*/
#if defined(WIN32)
 if (Zap_Count)
 { DWORD dwtemp;
   COORD xy;
   xy.Y=Top_Row+Sub_X+St_Line-1;
   xy.X=Left_Col+Sub_Y+St_Col-1;
/*   SetConsoleCursorPosition( win32ConsoleHnd , xy );*/
   WIN32cursor(Top_Row+Sub_X+St_Line-1,Left_Col+Sub_Y+St_Col-1);
   WIN32attrib(His_Attributes);
   WriteConsole(win32ConsoleHnd,Output,Zap_Count, &dwtemp, (LPVOID)0);
   WIN32cursor(Top_Row+Sub_X+His_Line-1,Left_Col+Sub_Y+His_Col-1);
   St_Line=St_Col=Zap_Col=Zap_Line=Zap_Count=0;
 }
#endif /*xx*/
#endif
 if (Out_Count)
 {
#if (defined(DOS) & ! (defined(Windows) | defined(TV))) | defined(OS2) | defined(WIN32)

/* WIN32 Q? */
#if defined(WIN32)
WIN32attrib(His_Attributes);
fputs("fputs",stdout);
#endif

  Out_String[Out_Count]=0;
  fputs(Out_String,stdout);
#endif
#if defined(UNIX)
  Out_String[Out_Count]=0;
#if defined(TERMCAP)
  fputs(Out_String,stdout);
  fflush(stdout);
#else
  addstr(Out_String);
#endif
#endif
#if defined(MPE)
  FWRITE(mpe_stdout,Out_String,-Out_Count,0320);
#endif
#if defined(ATARI)
#if ! defined(GEM)
  {register int I;
   register char *S;
   S=Out_String;
   I=Out_Count;
   while (I--)
    Bconout(2,*(S++));
  }
#else
  Out_String[Out_Count]=0;
  GEM_Text(S_Line,S_Col,Out_String,His_Attributes);
#endif
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
  Out_String[Out_Count]=0;
  WinText(S_Line,S_Col,Out_String,His_Attributes);
#endif
#if defined(DOS) & defined(TV)
  Out_String[Out_Count]=0;
  TV_Text(S_Line,S_Col,Out_String,Attab[His_Attributes]);
#endif
#if defined(VMS)
  T$Put(Out_String,Out_Count);
#endif
  Out_Count=0;
 }
}

/*      Initialize the screen optimization database and clear the screen.
*/
void Fast Opt_Init()
{
 {register int Line;
  for (Line=1;Line<=Scr_Length;++Line)
  {
#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE)
   Space_Fill(My_Screen[Line+Top_Row]+1+Left_Col,Scr_Width);
   Zero_Fill(My_Attributes[Line+Top_Row]+1+Left_Col,Scr_Width);
   Lst_Col[Line+Top_Row]=0;
#else
   Lst_Col[Line]=0;
#endif
  }
 }
#if defined(DOS) & !(defined(Windows) | defined(TV)) & !defined(PC100)
 IBMpage(0);
#endif
#if (defined(DOS) & !defined(Windows)) | defined(OS2) | defined(WIN32)
#if !defined(TV)
 St_Line=St_Col=Zap_Col=Zap_Line=Zap_Count=0;
#endif
 {register int I, At;
  for (I=0;I<16;I++)
  if (Attab[I]==0)
  {
#if !(defined(PC100) | defined(TV))
   if (I&A_Reverse)
   {if (I&A_Bold)
    {if (I&A_Underline)
      At=Color?113:112; else
      At=Color?113:112;
    } else
    {if (I&A_Underline)
      At=Color?33:120; else
      At=Color?33:120;
    }
   } else
   {if (I&A_Bold)
    {if (I&A_Underline)
      At=Color?31:9; else
      At=Color?31:15;
    } else
    {if (I&A_Underline)
      At=Color?23:1; else
      At=Color?23:7;
    }
   }
   if (I&A_Blink)
    At+=128;
#elif defined(TV)
   if (I&A_Reverse)
   {if (I&A_Bold)
    {if (I&A_Underline)
      At=7; else
      At=7;
    } else
    {if (I&A_Underline)
      At=7; else
      At=7;
    }
   } else
   {if (I&A_Bold)
    {if (I&A_Underline)
      At=6; else
      At=6;
    } else
    {if (I&A_Underline)
      At=6; else
      At=6;
    }
   }
#else
   At=0;
   if (I&A_Reverse)
    At=reverse;
   if (!(I&A_Bold))
    At+=nobold;
   if (!(I&A_Underline))
    At+=nounderscore;
   if (!(I&A_Blink))
    At+=noblink;
#endif
   Attab[I]=At;
  }
 }
#endif
 Clr_Screen();
 Set_Cursor(1,1);
 Set_Scroll(1,Scr_Length);
 Opt_Force_Attributes((char)(Cur_Attributes=0));
}

#if defined(Noasm) | defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(MPE) | defined(WIN32)
/*      Fill the area pointed to by Cp by N spaces.
*/
void Fast Space_Fill(char *Cp,int N)
{
#if !defined(Noasm)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(WIN32)
 memset(Cp,' ',N);
#endif
#if defined(ATARI) | defined(MPE)
 setmem(Cp,N,' ');
#endif
#else
 register char *L_Cp;
 L_Cp=Cp;
 N++;
 while (--N)
  (*L_Cp++)=' ';
#endif
}

/*      Fill the area pointed to by Cp with N null characters.
*/
void Fast Zero_Fill(char *Cp,int N)
{
#if !defined(Noasm)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(WIN32)
 memset(Cp,0,N);
#endif
#if defined(ATARI) | defined(MPE)
 setmem(Cp,N,0);
#endif
#else
 register char *L_Cp;
 L_Cp=Cp;
 N++;
 while (--N)
  (*L_Cp++)=0;
#endif
}
#endif

/*      Set the virtual cursor position to Line and Col.
*/
void Fast Opt_Cursor(int Line,long Col)
{My_Line=Line;
 My_Col=Col;
}

/*      Output a character to the screen and update the cursor position. This
        routine can only be used for printable characters.
*/
void Fast Put_Char(char C)
{if (Out_Count==Out_Size-1)
  Out_Zap();
 Out_String[Out_Count++]=C;
 His_Col++;
#if defined(UNIX)
 if (His_Col>Scr_Width)
  Set_Cursor(His_Line,--His_Col);
#endif
}

/*      Move the physical cursor to Line and Col. The move is made with as few 
        characters output as possible.
*/
void Fast Opt_Move_Cursor(int Line,long Col)
{
#if !((defined(ATARI) & defined(GEM)) | defined(MPE))
#if !(defined(DOS) | defined(NT) | defined(OS2))
 char *Scr, *Atr;
#endif
 if (Col<=Scr_Shift)
  Col=1; else
  Col-=Scr_Shift;
 if (Col>Scr_Width)
  Col=Scr_Width;
#if !(defined(DOS) | defined(NT) | defined(OS2) | (defined(UNIX) & !defined(TERMCAP)) |defined(WIN32))
 Scr=(&My_Screen[His_Line+Top_Row][His_Col+Left_Col]);
 Atr=(&My_Attributes[His_Line+Top_Row][His_Col+Left_Col]);
 if (Line==His_Line)
 {if (Col>His_Col)
  {if (Col-His_Col<4)
   {while (His_Col!=Col)
    {if (*Atr!=His_Attributes)
     {Move_Right((int)Col);
      break;
     } else
      Put_Char(*Scr);
     Scr++;
     Atr++;
    }
   } else
    Move_Right((int)Col);
  } else
  if (Col<His_Col)
  {if (His_Col>Scr_Width)
    Set_Cursor(Line,(int)Col); else
   if (His_Col-Col<4)
   {do
     Out_C('\b');
    while ((--His_Col)!=Col);
   } else
    Move_Left((int)Col);
  }
 } else
 if (Line==His_Line+1&&His_Line!=Scl_Bottom)
 {if (Zoomed&&Col<6)
  {Out_C(10);
   Out_C(13);
   His_Line=Line;
   His_Col=1;
   Scr=(&My_Screen[His_Line+Top_Row][His_Col+Left_Col]);
   Atr=(&My_Attributes[His_Line+Top_Row][His_Col+Left_Col]);
   while (His_Col!=Col)
   {if (*Atr!=His_Attributes)
    {Move_Right((int)Col);
     break;
    } else
     Put_Char(*Scr);
    Scr++;
    Atr++;
   }
  } else
   Set_Cursor(Line,(int)Col);
 } else
  Set_Cursor(Line,(int)Col);
#else
  Set_Cursor(Line,(int)Col);
#endif
#else
 if (Col<=Scr_Shift)
  Col=1; else
  Col-=Scr_Shift;
 if (Col>Scr_Width)
  Col=Scr_Width;
 Set_Cursor(Line,(int)Col);
#endif
}

/*      Insert a character into the screen database at the virtual cursor
	position. Whether the character is physically written depends on
	the current contents of the physical screen.
*/
void Fast Opt_Char(char C)
/*Put a single character on the screen*/
{int Sav_Attributes;
 long Sav_Col;
#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE)
 register char *Scr, *Atr;
 if (!Update_Flag)
 {My_Col++;
  return;
 }
 Sav_Attributes=Cur_Attributes;
 if ((Scr_Col=My_Col-Scr_Shift)<1)
 {My_Col++;
  return;
 }
 if (Scr_Col+Left_Col>=Phys_Width&&My_Line+Top_Row==Phys_Length)
 {My_Col++;
  return;
 }
 if (Scr_Col>Scr_Width)
 {if (Scr_Col==Scr_Width+1)
  {Sav_Col=My_Col;
   My_Col=Scr_Width+Scr_Shift;
   Cur_Attributes=A_Reverse;
   Opt_Char('>');
   My_Col=Sav_Col;
   Cur_Attributes=Sav_Attributes;
  }
  My_Col++;
  return;
 }
 Scr=(&My_Screen[My_Line+Top_Row][Scr_Col+Left_Col]);
 Atr=(&My_Attributes[My_Line+Top_Row][Scr_Col+Left_Col]);
 if (*Scr!=C||*Atr!=Cur_Attributes)
 {if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
   Opt_Move_Cursor(My_Line,My_Col);
  if (His_Attributes!=Cur_Attributes)
   Opt_Force_Attributes(Cur_Attributes);
  if (Out_Count==Out_Size-1)
   Out_Zap();
#if defined(ATARI) & defined(GEM)
  if (Out_Count==0)
  {S_Line=My_Line;
   S_Col=Scr_Col;
  }
#endif
  Out_String[Out_Count++]=(*Scr)=C;
  (*Atr)=Cur_Attributes;
  if (C!=' '||Cur_Attributes)
  {if (Scr_Shift)
   {if (Lst_Col[My_Line+Top_Row]<My_Col-Scr_Shift&&My_Col>Scr_Shift)
    Lst_Col[My_Line+Top_Row]=My_Col-Scr_Shift;
   } else
   {if (Lst_Col[My_Line+Top_Row]<My_Col)
    Lst_Col[My_Line+Top_Row]=My_Col;
   }
  }
  His_Col++;
  if (His_Col>Scr_Width)
   Set_Cursor(His_Line,His_Col=Scr_Width);
 }
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (!Update_Flag)
 {My_Col++;
  return;
 }
 Sav_Attributes=Cur_Attributes;
 if ((Scr_Col=My_Col-Scr_Shift)<1)
 {My_Col++;
  return;
 }
 if (Scr_Col>Scr_Width)
 {if (Scr_Col==Scr_Width+1)
  {Sav_Col=My_Col;
   My_Col=Scr_Width+Scr_Shift;
   Cur_Attributes=A_Reverse;
   Opt_Char('>');
   My_Col=Sav_Col;
   Cur_Attributes=Sav_Attributes;
  }
  My_Col++;
  return;
 }
 if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
  Opt_Move_Cursor(My_Line,My_Col);
 if (His_Attributes!=Cur_Attributes)
  Opt_Force_Attributes((char)Cur_Attributes);
 if (Out_Count==Out_Size-1)
  Out_Zap();
 if (Out_Count==0)
 {S_Line=My_Line;
  S_Col=Scr_Col;
 }
 Out_String[Out_Count++]=C;
 if (C!=' '||Cur_Attributes)
 {if (Scr_Shift)
  {if (Lst_Col[My_Line]<My_Col-Scr_Shift&&My_Col>Scr_Shift)
   Lst_Col[My_Line]=My_Col-Scr_Shift;
  } else
  {if (Lst_Col[My_Line]<My_Col)
   Lst_Col[My_Line]=My_Col;
  }
 }
 His_Col++;
#endif
#if defined(DOS) & defined(TV)
 register char *Scr, *Atr;
 if (!Update_Flag)
 {My_Col++;
  return;
 }
 Sav_Attributes=Cur_Attributes;
 if ((Scr_Col=My_Col-Scr_Shift)<1)
 {My_Col++;
  return;
 }
 if (Scr_Col>Scr_Width)
 {if (Scr_Col==Scr_Width+1)
  {Sav_Col=My_Col;
   My_Col=Scr_Width+Scr_Shift;
   Cur_Attributes=A_Reverse;
   Opt_Char('>');
   My_Col=Sav_Col;
   Cur_Attributes=Sav_Attributes;
  }
  My_Col++;
  return;
 }
 Scr=(&My_Screen[My_Line+Top_Row][Scr_Col+Left_Col]);
 Atr=(&My_Attributes[My_Line+Top_Row][Scr_Col+Left_Col]);
 if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
  Opt_Move_Cursor(My_Line,My_Col);
 if (His_Attributes!=Cur_Attributes)
  Opt_Force_Attributes((char)Cur_Attributes);
 if (Out_Count==Out_Size-1)
  Out_Zap();
 if (Out_Count==0)
 {S_Line=My_Line;
  S_Col=Scr_Col;
 }
 *Scr=Out_String[Out_Count++]=C;
 *Atr=Cur_Attributes;
 if (C!=' '||Cur_Attributes)
 {if (Scr_Shift)
  {if (Lst_Col[My_Line]<My_Col-Scr_Shift&&My_Col>Scr_Shift)
   Lst_Col[My_Line]=My_Col-Scr_Shift;
  } else
  {if (Lst_Col[My_Line]<My_Col)
   Lst_Col[My_Line]=My_Col;
  }
 }
 His_Col++;
#endif
#if (defined(DOS) & !(defined(Windows) | defined(TV))) | defined(WIN32)



 if (!Update_Flag)
 {My_Col++;
  return;
 }
 Sav_Attributes=Cur_Attributes;
 if ((Scr_Col=My_Col-Scr_Shift)<1)
 {My_Col++;
  return;
 }
 if (Scr_Col>Scr_Width)
 {if (Scr_Col==Scr_Width+1)
  {Sav_Col=My_Col;
   My_Col=Scr_Width+Scr_Shift;
   Cur_Attributes=A_Reverse;
   Opt_Char('>');
   My_Col=Sav_Col;
   Cur_Attributes=Sav_Attributes;
  }
  My_Col++;
  return;
 }
#if defined(PC100)
 if (!Dir_Video)
 {if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
   Opt_Move_Cursor(My_Line,My_Col);
  if (Cur_Attributes!=His_Attributes)
   Opt_Force_Attributes(Cur_Attributes);
  Out_C(C);
  His_Col++;
  if (C!=' '||Cur_Attributes)
  {if (Scr_Shift)
   {if (Lst_Col[My_Line]<My_Col-Scr_Shift&&My_Col>Scr_Shift)
    Lst_Col[My_Line]=My_Col-Scr_Shift;
   } else
   {if (Lst_Col[My_Line]<My_Col)
    Lst_Col[My_Line]=My_Col;
   }
  }
 } else
 {if (Out_Count)
   Out_Zap();
  if (!Zap_Count)
  {St_Line=My_Line;
   St_Col=Zap_Col=Scr_Col;
   Zap_Line=My_Line;
  } else
  if (Zap_Col!=Scr_Col||Zap_Line!=My_Line||Zap_Count==132)
  {Out_Zap();
   St_Line=My_Line;
   St_Col=Zap_Col=Scr_Col;
   Zap_Line=My_Line;
  }
  Output[Zap_Count]=C;
  Attr[Zap_Count++]=Attab[Cur_Attributes];
  if (C!=' '||Cur_Attributes)
  {if (Scr_Shift)
   {if (Lst_Col[My_Line]<My_Col-Scr_Shift&&My_Col>Scr_Shift)
    Lst_Col[My_Line]=My_Col-Scr_Shift;
   } else
   {if (Lst_Col[My_Line]<My_Col)
    Lst_Col[My_Line]=My_Col;
   }
  }
  Zap_Col++;
 }
#else
 if (Cur_Attributes!=His_Attributes)
  Opt_Force_Attributes((char)Cur_Attributes);
 if (Out_Count)
  Out_Zap();
 if (!Zap_Count)
 {St_Line=My_Line;
  St_Col=Zap_Col=Scr_Col;
  Zap_Line=My_Line;
 } else
 if (Zap_Col!=Scr_Col||Zap_Line!=My_Line||Zap_Count==132)
 {Out_Zap();
  St_Line=My_Line;
  St_Col=Zap_Col=Scr_Col;
  Zap_Line=My_Line;
 }
 Output[Zap_Count]=C;
 Attr[Zap_Count++]=Attab[Cur_Attributes];
 if (C!=' '||Cur_Attributes)
 {if (Scr_Shift)
  {if (Lst_Col[My_Line]<My_Col-Scr_Shift&&My_Col>Scr_Shift)
   Lst_Col[My_Line]=My_Col-Scr_Shift;
  } else
  {if (Lst_Col[My_Line]<My_Col)
   Lst_Col[My_Line]=My_Col;
  }
 }
 Zap_Col++;
#endif
#endif
#if defined(OS2)
 if (!Update_Flag)
 {My_Col++;
  return;
 }
 Sav_Attributes=Cur_Attributes;
 if ((Scr_Col=My_Col-Scr_Shift)<1)
 {My_Col++;
  return;
 }
 if (Scr_Col>Scr_Width)
 {if (Scr_Col==Scr_Width+1)
  {Sav_Col=My_Col;
   My_Col=Scr_Width+Scr_Shift;
   Cur_Attributes=A_Reverse;
   Opt_Char('>');
   My_Col=Sav_Col;
   Cur_Attributes=Sav_Attributes;
  }
  My_Col++;
  return;
 }
 if (Cur_Attributes!=His_Attributes)
  Opt_Force_Attributes(Cur_Attributes);
 if (Out_Count)
  Out_Zap();
 if (!Zap_Count)
 {St_Line=My_Line;
  St_Col=Zap_Col=Scr_Col;
  Zap_Line=My_Line;
 } else
 if (Zap_Col!=Scr_Col||Zap_Line!=My_Line||Zap_Count==132)
 {Out_Zap();
  St_Line=My_Line;
  St_Col=Zap_Col=Scr_Col;
  Zap_Line=My_Line;
 }
 Output[Zap_Count]=C;
 Attr[Zap_Count++]=Attab[Cur_Attributes];
 if (C!=' '||Cur_Attributes)
 {if (Scr_Shift)
  {if (Lst_Col[My_Line]<My_Col-Scr_Shift&&My_Col>Scr_Shift)
   Lst_Col[My_Line]=My_Col-Scr_Shift;
  } else
  {if (Lst_Col[My_Line]<My_Col)
   Lst_Col[My_Line]=My_Col;
  }
 }
 Zap_Col++;
#endif
 Cur_Attributes=Sav_Attributes;
 My_Col++;
}

#if defined(VMS)
/*      Insert a character that has been echoed on the screen into the screen
        database.
*/
Opt_E_Char(C)
register char C;
{register char *Scr, *Atr;
 if ((Scr_Col=My_Col-Scr_Shift)<=Scr_Width)
 {Scr=(&My_Screen[My_Line+Top_Row][Scr_Col+Left_Col]);
  Atr=(&My_Attributes[My_Line+Top_Row][Scr_Col+Left_Col]);
  if (*Scr!=C||*Atr!=Cur_Attributes)
  {if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
    Opt_Move_Cursor(My_Line,My_Col);
   (*Atr)=Cur_Attributes;
   if (((*Scr=C)!=' '||Cur_Attributes)&&
     Lst_Col[My_Line+Top_Row]<My_Col-Scr_Shift&&My_Col>Scr_Shift)
    Lst_Col[My_Line+Top_Row]=My_Col-Scr_Shift;
  }
  His_Col++;
  My_Col++;
 }
}
#endif

/*      Output a null terminated string to the virtual screen.
*/
void Fast Opt_String(char *S)
{register char C;
 if (S==NULL)
  return;
 while ((C=(*S++))!=0)
  Opt_Char(C);
}

/*      Erase the virtual screen from the current virtual cursor position
        to the end of the line.
*/
void Fast Opt_Erase_Line()
/*Erases from cursor to end of line*/
{register int This_Last;
 char Sav_Attributes;
 if (!Update_Flag)
  return;
 Sav_Attributes=Cur_Attributes;
 Cur_Attributes=0;
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
 if ((This_Last=Lst_Col[My_Line+Top_Row])>=My_Col-Scr_Shift)
#else
 if ((This_Last=Lst_Col[My_Line])>=My_Col-Scr_Shift)
#endif
 {if (This_Last>0)
  {Scr_Col=My_Col-Scr_Shift;
   if (Scr_Col<1)
    Scr_Col=1;
   Del_To_End_Line();
#if defined(VMS) | defined(ATARI) | defined(UNIX) | (defined(DOS) & defined(TV)) | defined(MPE)
   Space_Fill(My_Screen[My_Line+Top_Row]+Scr_Col+Left_Col,This_Last-Scr_Col+1);
   Zero_Fill(My_Attributes[My_Line+Top_Row]+Scr_Col+Left_Col,
    This_Last-Scr_Col+1);
#endif
  }
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
  if ((Lst_Col[My_Line+Top_Row]=My_Col-Scr_Shift-1)<0)
   Lst_Col[My_Line+Top_Row]=0;
#else
  if ((Lst_Col[My_Line]=My_Col-Scr_Shift-1)<0)
   Lst_Col[My_Line]=0;
#endif
 }
 Cur_Attributes=Sav_Attributes;
}

#if defined(DOS)
void Fast Get_Type()
{if (Call_Type==0)
 {
#if defined(Windows)
  Call_Type=0xFC;
  Call_Model=0x1;
#else
  Call_Type=GETTYP();
  if (Call_Type==0xFC&&Call_Model==0)
   Call_Model=pc_sys_type()>>8;
#endif
 }
}
#endif

#if defined(WIN32)
void Fast Get_Type()
{ Call_Type=0xFC;
  Call_Model=0x1;
}
#endif

#if defined(OS2)
void Get_Type()
{unsigned char Item;
 struct _VIOMODEINFO Mode_Data;
 if (Call_Type==0)
 {DosDevConfig((PVOID)&Item,5,0);
  Call_Type=Item;
 }
 if (Call_Model==0)
 {DosDevConfig((unsigned char far *)&Item,4,0);
  Call_Model=Item;
 }
 if (Call_Color==0)
 {DosDevConfig((unsigned char far *)&Item,6,0);
  Call_Color=Item+1;
 }
 Color=Call_Color==2;
 Mode_Data.cb=12;
 VioGetMode((PVIOMODEINFO)&Mode_Data,0);
 Scr_Length=Mode_Data.row;
}
#endif
