/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/

/*
	This module contains all terminal dependent code. To implement a new
terminal type it should only be necessary to change and recompile the code here
and relink Sedt.
*/

#include "proto.h"
#if defined(WIN32)
#undef WIN32
#include <windows.h>
#define WIN32=1
#define IBMcursor this is a compile error instead of a link error
#include "win32edt.h"
#endif

#if defined(DOS) & !(defined(Windows) | defined(TV))
#if defined(DESQVIEW)
unsigned int api_init(void);
unsigned long api_shadow(void);
unsigned long win_me(void);
unsigned long ptr_new(void);
void ptr_open(unsigned long ptrhan, unsigned long winhan);
void ptr_addto(unsigned long ptrhan, unsigned int flags);
int ptr_sizeof(unsigned long ptrhan);
#endif
#endif


#define Fn 512

#if defined(VMS)
extern int T$Application();
#endif

#if defined(DOS) | defined(NT) | defined(OS2)
#include <dos.h>
#define Mcapsl 0x40
#define Malt 0x8
#define Mctl 0x4
#define Mlsh 0x2
#define Mrsh 0x1
#endif

#if defined(OS2)
void Mon_Thread()
{unsigned Key_Count;
 while (1)
 {Key_Count=sizeof(Key_Buff);
  DosMonRead((PBYTE)&InBuff,(USHORT)0,(PBYTE)&Key_Buff,(PUSHORT)&Key_Count);
  {register int I;
   if (Key_Buff.ddflags&0x37)
   {if (Key_Buff.cp.scan==0)
    {if (Key_Buff.mnflags==17792)
     {for (I=0;I<Scan_Defs;I++)
      if (Scan_Tab[I].Scancode==0x45)
      {Key_Buff.ddflags&=0x7740;
       Key_Buff.cp.scan=0x45;
       break;
      }
     } else
     if (Key_Buff.mnflags==18048)
     {for (I=0;I<Scan_Defs;I++)
      if (Scan_Tab[I].Scancode==0x46)
      {Key_Buff.ddflags&=0x7740;
       Key_Buff.cp.scan=0x46;
       break;
      }
     }
    }
   }
   for (I=0;I<Scan_Defs;I++)
   {if (Scan_Tab[I].Flags&Ext_Code&&Key_Buff.cp.scan==Scan_Tab[I].Scancode&&
     Key_Buff.cp.asc==Scan_Tab[I].Char)
    {Key_Buff.ddflags&=0x7740;
     break;
    }
    if (Key_Buff.cp.scan==Scan_Tab[I].Scancode)
    {Key_Buff.ddflags&=0x7740;
     Key_Buff.cp.asc=Scan_Tab[I].Char;
     break;
    }
   }
  }
  DosMonWrite((PBYTE)&OutBuff,(PBYTE)&Key_Buff,Key_Count);
 }
}

void Mouse_Thread()
{int Line, Col, Left, Right, Move, Mask;
 struct _NOPTRRECT No_Pointer;
 MOUEVENTINFO Event;
 Move=0;
 Ptr_Loc.row=0;
 Ptr_Loc.col=Scr_Width-1;
 DosSemRequest((HSEM)&Mouse_Sem,-1L);
 DosSemClear((HSEM)&Mouse_Sem);
 MouSetPtrPos((PPTRLOC)&Ptr_Loc,Mouse_Handle);
 while (1)
 {Read_Type=0;
  MouReadEventQue((PMOUEVENTINFO)&Event,(unsigned far *)&Read_Type,
   Mouse_Handle);
  if (Event.time!=0)
  {DosSemRequest((HSEM)&Mouse_Sem,-1L);
   Line=Event.row;
   Col=Event.col;
   Mask=Event.fs;
   Left=Mask&((1<<2)+(1<<1));
   Right=Mask&(1<<4);
   No_Pointer.row=0;
   No_Pointer.col=0;
   No_Pointer.cRow=Scr_Length-1;
   No_Pointer.cCol=Scr_Width-1;
   MouRemovePtr((PNOPTRRECT)&No_Pointer,Mouse_Handle);
   Mouse_Call(Line-Sub_X,Col-Sub_Y,Left,Right,Move);
   if (Left||Right)
    Move|=Mask&(1<<1); else
    Move=0;
   MouDrawPtr(Mouse_Handle);
   DosSemClear((HSEM)&Mouse_Sem);
  } else
   DosSleep(100L);
 }
}
#endif

#if defined (DOS) & (defined(PC100) | defined(TV))
void Mouse_Int(void)
{;
}
#endif

#if defined(DOS) & !(defined(Windows) | defined(TV)) & !defined(PC100)
void Mouse_Int(int P1,int P2,int P3,int P4)
{int M1, Abs_Row, Abs_Col;
 /*Diag("M:P1=%d,P2=%d,P3=%d,P4=%d",P1,P2,P3,P4);*/
 if (Mouse_OK)
 {Clear_Mouse();
  Out_Zap();
  M1=2;
  Mouse(&M1,NULL,NULL,NULL);
  Abs_Row=P4/8-Sub_X;
  Abs_Col=P3/8-Sub_Y;
  if (Abs_Row>=0&&Abs_Row<=Phys_Length&&Abs_Col>=0&&Abs_Col<=Phys_Width)
   Mouse_Call(Abs_Row,Abs_Col,P2&1,P2&2,P1&1);
  M1=1;
  Mouse(&M1,NULL,NULL,NULL);
  M1=1;
  Mouse(&M1,NULL,NULL,NULL);
  Set_Mouse();
 }
}
#endif

#if defined(VMS) 
void Reposition_Input() 
{
 if (--This_In>=0)
  Character=TTY_Buffer[This_In]&255;
}
#endif
#if defined(UNIX)
void Reposition_Input() 
{
 Same_Character=1;
}
#endif

#if defined(VMS) | defined(UNIX)
void Enable_Mouse() 
{if (Use_Mouse)
 {Out_S("\033[1;2'z");	/* Enable Locator */
  Out_Zap();
 }
} 

void Disable_Mouse() 
{
 Out_S("\033[0'z");	/* Disable Locator */
 Out_Zap();
} 

void  Mouse_1_Down()
{
 Ldown=1; 
 Move=Rdown=0; 
 if (Mouse_Get_Rest()==1)
  Mouse_Call();
 Abs_Row_Sav=Abs_Row;
 Abs_Col_Sav=Abs_Col;
}

void  Mouse_1_Up()
{
 Rdown=0; 
 Ldown=1; 
 if (Mouse_Get_Rest()==1)
 {Move=Abs_Row_Sav!=Abs_Row||Abs_Col_Sav!=Abs_Col;
  if (Move)
   Mouse_Call();
 }
}

void  Mouse_2_Down()
{
 Mouse_Get_Rest();
}

void  Mouse_2_Up()
{
 Mouse_Get_Rest();
}

void  Mouse_3_Down()
{
 Rdown=1; 
 Move=Ldown=0; 
 if (Mouse_Get_Rest()==1)
  Mouse_Call();
}

void  Mouse_3_Up()
{
 Move=Ldown=0; 
 Rdown=1; 
 Mouse_Get_Rest()==1;
}

void  Mouse_4_Down()
{
 Mouse_Get_Rest();
}

void  Mouse_4_Up()
{
 Mouse_Get_Rest();
}

void  Mouse_Outside()
{
}

/* read in rest of locator input escape sequence and compute row/column
   Ansi Locator report has following format: VAX/VWS: <CSI>Pe;Pb;Pr;Pc;Pp&w
   VAX/DECwindows <CSI>Pe;Pb;Pr;Pc&w
   Pe Event code: 2 Left   Button Down	  3 Left   Button Up
                  4 Middle Button Down    5 Middle Button Up
                  6 Right  Button Down    7 Right  Button Up
                  8 Fourth Button Down    9 Fourth Button Up
                 10 Locator outside filter rectangle
   Pb Button status: 0 No Buttons down    1 Right 2 Middle 4 Left 8 Fourth
   Pr row coordinate
   Pc column coordinate
   Pp page coordinate (may be omitted)
   The current position in the input stream is at the end of <CSI>Pe;
*/
int Mouse_Get_Rest()
{
 int field[4];
 int *pf;
 register char C;
 pf=field;
 *pf=0;
#if defined(VMS)
 C=Termin();
#else
#if defined(TERMCAP)
 C=UGetrm();
#else
 C=getch();
#endif
#endif
 while (C!='&'&&pf<=(&field[3]))
 {
  if (isdigit(C))
  {*pf=(*pf)*10+C-'0';
  }else
  if (C==';')
   *(++pf)=0;
  else
  {Error(5,Ill_Mouse_Mess,C);
   Err_Flag=0;
   return (0);
  }
#if defined(VMS)
 C=Termin();
#else
#if defined(TERMCAP)
 C=UGetrm();
#else
 C=getch();
#endif
#endif
 }
 if ((
#if defined(VMS)
      C=Termin()
#else
#if defined(TERMCAP)
      C=UGetrm()
#else
      C=getch()
#endif
#endif
                )=='w') 
 {Abs_Row=(--field[1]);
  Abs_Col=(--field[2]);
  return (1);
 }
 else
 {Error(5,Ill_Mouse_Mess,C);
  Err_Flag=0;
  return (0);
 }
}
#endif

#if defined(GEM) | defined(Windows) | defined(TV)
#define SCREEN_LENGTH Scr_Length-1
#else
#define SCREEN_LENGTH Scr_Length
#endif
#if defined(VMS) | defined(UNIX)
void Fast Mouse_Call()	/* 290a */
#endif
#if ((defined(DOS) | defined(NT)) & !defined(PC100)) | defined(OS2) | (defined(ATARI) & defined(GEM))
void Fast Mouse_Call(int Abs_Row,int Abs_Col,int Ldown,int Rdown,int Move)
#endif
#if defined(VMS) | ((defined(DOS) | defined(NT)) & !defined(PC100)) | defined(OS2) | (defined(ATARI) & defined(GEM)) | defined(UNIX)
{
 /*Diag("Mouse_Call:Row=%d,Col=%d,Ldown=%d,Rdown=%d,Move=%d",
  Abs_Row,Abs_Col,Ldown,Rdown,Move);*/
 if (Down_Type==0)
#if defined(GEM) | defined(Windows) | defined(TV)
 {if (Abs_Row>=Top_Row&&Abs_Col>=Left_Col&&Abs_Row<Top_Row+SCREEN_LENGTH&&
   Abs_Col<Left_Col+Scr_Width)
#else
 {if ((Abs_Row>=Top_Row&&Abs_Col>=Left_Col&&Abs_Row<Top_Row+SCREEN_LENGTH&&
   Abs_Col<Left_Col+Scr_Width)||Abs_Row==Max_Length-2)
#endif
    Mouse_Execute(Abs_Row+1-Top_Row,Abs_Col+1-Left_Col,Ldown,Rdown,Move); else
  if (Ldown)
  {Down_Type=0;
   if (Abs_Row==Top_Row-1&&Abs_Col>=Left_Col&&Abs_Col<Left_Col+Scr_Width)
   {Down_Type=1;
   } else
   if (Abs_Row==Top_Row+SCREEN_LENGTH&&Abs_Col>=Left_Col&&Abs_Col<Left_Col+Scr_Width)
   {Down_Type=2;
   } else
   if (Abs_Col==Left_Col-1&&Abs_Row>=Top_Row&&Abs_Row<Top_Row+SCREEN_LENGTH)
   {Down_Type=3;
   } else
   if (Abs_Col==Left_Col+Scr_Width&&Abs_Row>=Top_Row&&Abs_Row<Top_Row+SCREEN_LENGTH)
   {Down_Type=4;
   } else
   if (Abs_Row==Top_Row-1&&Abs_Col==Left_Col-1)
   {Down_Type=5;
   } else
   if (Abs_Row==Top_Row-1&&Abs_Col==Left_Col+Scr_Width)
   {Down_Type=6;
   } else
   if (Abs_Row==Top_Row+SCREEN_LENGTH&&Abs_Col==Left_Col+Scr_Width)
   {Down_Type=7;
   } else
   if (Abs_Row==Top_Row+SCREEN_LENGTH&&Abs_Col==Left_Col-1)
   {Down_Type=8;
   } else
   {register int I;
    Sav_Window(Cur_Buffer);
    for (I=1;I<Buffers;I++)
    {Load_Window(Buffer_Stack[I]);
     if (Abs_Row>=Top_Row-1&&Abs_Col>=Left_Col-1&&
      Abs_Row<=Top_Row+SCREEN_LENGTH&&Abs_Col<=Left_Col+Scr_Width)
     {Load_Window(Cur_Buffer);
      Ch_Window(Buffer_Stack[I]);
      Opt_Move_Cursor(My_Line,My_Col);
      break;
     }
     if (I==Buffers-1)
      Load_Window(Cur_Buffer);
    }
   }
  }
 } else	/* Down_Type!=0 */
 {if (Ldown&&Move) 
  {Zoomed=0;
   switch (Down_Type)
   {case 1:
     if (Scr_Length+Top_Row-Abs_Row>4)
     {Scr_Length+=Top_Row-Abs_Row-1;
      Top_Row=Abs_Row+1;
     }
     break;
    case 2:
     if (Abs_Row-Top_Row>2&&Abs_Row<Max_Length)
#if defined(GEM) | defined(Windows) | defined(TV)
      Scr_Length=Abs_Row-Top_Row+1;
#else
      Scr_Length=Abs_Row-Top_Row;	/* 290a */
     if (Scr_Length+Top_Row>=Max_Length-2) Scr_Length=Max_Length-3-Top_Row;
#endif
     break;
    case 3:
     if (Scr_Width+Left_Col-Abs_Col>4)
     {Scr_Width+=Left_Col-1-Abs_Col;
      Left_Col=Abs_Col+1;
     }
     break;
    case 4:
     if (Abs_Col-Left_Col>3)
      Scr_Width=Abs_Col-Left_Col;
     break;
    case 5:
     if (Scr_Length+Top_Row-Abs_Row>4)
     {Scr_Length+=Top_Row-Abs_Row-1;
      Top_Row=Abs_Row+1;
     }
     if (Scr_Width+Left_Col-Abs_Col>4)
     {Scr_Width+=Left_Col-1-Abs_Col;
      Left_Col=Abs_Col+1;
     }
     break;
    case 6:
     if (Scr_Length+Top_Row-Abs_Row>4)
     {Scr_Length+=Top_Row-Abs_Row-1;
      Top_Row=Abs_Row+1;
     }
     if (Abs_Col-Left_Col>3)
      Scr_Width=Abs_Col-Left_Col;
     break;
    case 7:
     if (Abs_Row-Top_Row>2&&Abs_Row<Max_Length)
#if defined(GEM) | defined(Windows) | defined(TV)
      Scr_Length=Abs_Row-Top_Row+1;
#else
      Scr_Length=Abs_Row-Top_Row;	/* 290a */
     if (Scr_Length+Top_Row>=Max_Length-2) Scr_Length=Max_Length-3-Top_Row;
#endif
     if (Abs_Col-Left_Col>3)
      Scr_Width=Abs_Col-Left_Col;
     break;
    case 8:
     if (Abs_Row-Top_Row>2&&Abs_Row<Max_Length)
#if defined(GEM) | defined(Windows) | defined(TV)
      Scr_Length=Abs_Row-Top_Row+1;
#else
      Scr_Length=Abs_Row-Top_Row;	/* 290a */
     if (Scr_Length+Top_Row>=Max_Length-2) Scr_Length=Max_Length-3-Top_Row;
#endif
     if (Scr_Width+Left_Col-Abs_Col>4)
     {Scr_Width+=Left_Col-1-Abs_Col;
      Left_Col=Abs_Col+1;
     }
     break;
   }
   Open_Window();
   /*Down_Type=0;*/	/* 290a */
  } else
#if defined(VMS) | defined(UNIX)
/* allow multiple move operations without refresh: 
   second move starts with (Ldown&&!Move) 
*/
  if (Rdown||Move)	/* 290a */ 
  {Down_Type=0;
   Refresh_Window();
   Opt_Move_Cursor(My_Line,My_Col);
  } else
  Down_Type=0;
#else
  if (!((Ldown)&&(Move)))
  {Down_Type=0;
   Refresh_Window();
   Opt_Move_Cursor(My_Line,My_Col);
  } 
#endif
 }
}
#endif

#if defined(VMS) | ((defined(DOS) | defined(NT)) & !defined(PC100)) | defined(OS2) | (defined(ATARI) & defined(GEM)) | defined(UNIX)
void Fast Mouse_Execute(int Line,int Col,int Left,int Right,int Move)
{int C;
 long Chars, Sel_Pos, Sav_Col, R_Col;
 if (!(Left||Right))
  return;
 if (Err_Display)
  Clr_Error();
 if (Working_Flag)
  Clr_Working();
 Line-=Home;
 R_Col=Col+Scr_Shift;
 Sav_Col=My_Col;
 Chars=0;
 B_Goto(Current,My_Pos);
#if defined(VMS) | defined(UNIX)
 if (Abs_Row==Max_Length-2)	/* 290a */
 {R_Col-=Scr_Shift-Left_Col;
#elif defined(GEM) | defined(Windows) | defined(TV)
 if (Line+Home+Top_Row==Scr_Length-1)
 {R_Col-=Scr_Shift;
#else
 if (Line+Home+Top_Row==Phys_Length-1)
 {R_Col-=Scr_Shift-Left_Col;
#endif
  if (!Move&&!Right)	/* 290a */
  {if (R_Col<Replace_Pos)
     Def_Direction=Def_Direction?0:1; else
   if (R_Col<Block_Pos)
    Replace_Mode=Replace_Mode?0:1; else
   if (R_Col<Tab_Pos)
   {Blk_Mode=Blk_Mode?0:1;
    if ((Sel_Pos=Points[0])!=0)
    {if (Sel_Pos<My_Pos)
      Fill_Screen(Rel_First,0); else
      Fill_Screen(0,Rel_Last);
    }
   } else
   if (R_Col<Case_Pos)
    Tab_Expand=Tab_Expand?0:1; else
   if (R_Col<Reg_Pos)
    Case_Sensitive=Case_Sensitive?0:1; else
   if (R_Col<Def_Pos)
    Reg_Search=Reg_Search?0:1; else
   if (R_Col<Free_Pos)
    Ins_Defs=Ins_Defs?0:1; else
   if (R_Col<Shift_Pos)
    Cursor_Free=Cursor_Free?0:1; else
   if (R_Col<Learning_Pos)
   {Do_Shift=Do_Shift?0:1;
    if (Do_Shift)
    {if (My_Col-Scr_Shift<1)
     {while (My_Col-Scr_Shift<1)
       Scr_Shift-=Shift_Screen?Shift_Screen:Scr_Width/2;
      if (Show_Ruler)
       S_Ruler();
      Fill_Screen(Rel_First,Rel_Last);
     } else
     if (My_Col-Scr_Shift>=Scr_Width)
     {while (My_Col-Scr_Shift>=Scr_Width)
       Scr_Shift+=Shift_Screen?Shift_Screen:Scr_Width/2;
      if (Show_Ruler)
       S_Ruler();
      Fill_Screen(Rel_First,Rel_Last);
     } else
     if (My_Col-Scr_Shift<1&&Scr_Shift>0)
     {while (My_Col-Scr_Shift<1&&Scr_Shift>0)
       Scr_Shift-=Shift_Screen?Shift_Screen:Scr_Width/2;
      if (Show_Ruler)
       S_Ruler();
      Fill_Screen(Rel_First,Rel_Last);
     }
    } else
    if (Scr_Shift>0)
    {Scr_Shift=0;
     Hdr_Changes=1;
     if (Show_Ruler)
      S_Ruler();
     Fill_Screen(Rel_First,Rel_Last);
    }
   } else	/* 290a */
   if (R_Col>=Mod_Pos)
    Window_Changed(); 
   else
   {Error(3,Mouse_Toggle_Txt);
    Err_Flag=0;
   }
   Hdr_Changes=1;
   Upd_Header();
  }
 } else
 if (Show_Ruler&&Line+Home==2)
 {if (R_Col<=512&&!Move&&!Right)	/* 290a */
  {Tabs[(int)R_Col]=Tabs[(int)R_Col]?0:1;
   S_Ruler();
  }
 } else
#if defined(VMS)
 if (Line+Home==1&&Col==Scr_Width)	/* 290a */
 {if (Mbx_Count>0&&!Move&&!Right)
  {Plain_Message(Mbx_Mess[0]);
   Err_Display=0;
   Mbx_Count--;
   C_Move(Mbx_Mess[1],Mbx_Mess[0],99*1024,1);
  } 
 } else
#endif
 if (Line+Home==1)	/* 290a */
 {if (Col>=Buffer_Pos&&!Move&&!Right)
  {Window_Used();
   Set_Cursor(My_Line,(int)My_Col); /* mdj why is column a long? */
  }
 }
 else
 {if (Line==0)
  {if (R_Col<My_Col)
   {if (B_B_Search(Current,'\n',1L))
     B_Get(Current);
    Chars=Current->Pos-My_Pos;
    My_Col=1;
   }
   while (My_Col<R_Col&&Current->Pos<=Current->Bufsize&&
    !Chk_EL(Current,C=B_Get(Current))&&C!='\n')
   {My_Col+=Char_Rep(C,NULL);
    Chars++;
   }
   Init_Entity();
   if (Chars<0)
   {Ent_Direction=0;
    Ent_Count= -Chars;
   } else
    Ent_Count=Chars;
   A_Char();
   My_Col=Sav_Col;
  } else
  {if (Line<Rel_First||Line>Rel_Last)
    /* Error(3,Mouse_Window_Txt) */;	/* 290a */
#if !(defined(GEM) | (defined(Windows) | defined(TV)))
    else
#endif
   if (!Err_Flag)
   {if (!Move&&Left&&Points[0])
     Reset();
    My_Col=R_Col;
    Init_Entity();
    if (Line<0)
    {Ent_Direction=0;
     Ent_Count= -Line;
    } else
     Ent_Count=Line;
    Vertical();
   }
   My_Col=Sav_Col;
  }
  if (Err_Flag)
   Err_Flag=0; else
  {if (Left)
   {if (Move)
    {if (Points[0]==(Ent_Direction?End_Pos:Beg_Pos))
      Points[0]=0; else
     if (Points[0]==0)
      Points[0]=My_Pos;
    }
    Move_Entity();
   } else
   if (Right)
   {if ((Sel_Pos=Points[0])!=0&&
     (Ent_Direction?Sel_Pos<My_Pos:Sel_Pos>My_Pos))
    {Points[0]=0;
     if (Sel_Pos<My_Pos)
      Fill_Screen(Rel_First,0); else
      Fill_Screen(0,Rel_Last);
    }
    if (Ent_Direction==0)
    {Points[0]=Beg_Pos;
     Fill_Screen(Rel_First,0);
    } else
    {Points[0]=End_Pos;
     Fill_Screen(0,Rel_Last);
     if (Blk_Mode)
      Fill_Screen(0,Rel_Last);
    }
   }
  }
 }
 Upd_Header();
 if (My_Col!=His_Col||My_Line!=His_Line)
  Opt_Move_Cursor(My_Line,My_Col);
 Out_Zap();
}
#endif

void Fast Set_Terminal()
/*Determine exactly what type of terminal and keyboard Sedt is being run from.
This routine sets the variables
	Term_Type
        Keyb_Type
	Scl_Regions
*/
{
#if defined(WIN32)
   CONSOLE_SCREEN_BUFFER_INFO  pcsbi;
   win32ConsoleHnd = GetStdHandle(STD_OUTPUT_HANDLE);
   win32KeybHnd = GetStdHandle(STD_INPUT_HANDLE);
   GetConsoleScreenBufferInfo( win32ConsoleHnd , &pcsbi );
   Scr_Length=pcsbi.srWindow.Bottom-pcsbi.srWindow.Top+1;  /*pcsbi.dwSize.Y;*/
   Scr_Width=pcsbi.srWindow.Right-pcsbi.srWindow.Left+1;/*pcsbi.dwSize.X;*/
   if (Scr_Length > Maxlines)
     Scr_Length = Maxlines;
   if (Scr_Width > Maxcol)
      Scr_Width = Maxcol;
   Phys_Length=Max_Length=Scr_Length;
   Max_Length=Sub_Window?Sub_H:Max_Length;  /* 290a */
   Scr_Length=Max_Length-2;		/* 290a */
   Phys_Width=Max_Width=Scr_Width;
   Max_Width=Scr_Width=Sub_Window?Sub_W:Scr_Width; /* 290a */
   Scl_Regions=1;
   Get_Type();
   Term_Type=Defined;
   Keyb_Type=Keyb_Default;
   WIN32init();
#endif

#if defined(UNIX)
 register char C, *S;
 char *p;
#if defined(TERMCAP)
 char *tgetstr();
#endif
 Term_Type = -1;
 S = getenv ("TERM");
#if defined(TERMCAP)
 if (tgetent (tcapbuf, S) == -1)
 {printf (No_Termcap_Txt,S);
  exit (1);
 }
#endif
 if (Call_Type!=0)
 {switch (Call_Type)
  {case 1:
    Term_Type=VT1XX;
    break;
   case 2:
    Term_Type=VT2XX;
    break;
   case 3:
    Term_Type=VT52;
    break;
   case 4:
    Term_Type=VK100;
    break;
   case 5:
    Term_Type=ANSI;
    break;
   case 6:
    Term_Type=VT3XX;
    break;
   case 7:
    Term_Type=Tcap;
    break;
   case 8:
    Term_Type=Defined;
    if (strncmp (Defined_Terminal,"vt52", 4) == 0)
     Term_Type = VT52; else
    if (strncmp (Defined_Terminal,"vt1", 3) == 0)
     Term_Type = VT1XX; else
    if (strncmp (Defined_Terminal,"vt2", 3) == 0)
     Term_Type = VT2XX; else
    if (strncmp(Defined_Terminal,"vt3",3)==0)
     Term_Type=VT3XX; else
    if (strncmp(Defined_Terminal,"vt4",3)==0)
     Term_Type=VT4XX; else
    if (strncmp(Defined_Terminal,"ansi",4)==0)
     Term_Type=ANSI;
    break;
  }
 } else
 {if (strncmp (S, "vt52", 4) == 0)
   Term_Type = VT52; else
  if (strncmp (S, "vt1", 3) == 0)
   Term_Type = VT1XX; else
  if (strncmp (S, "vt2", 3) == 0)
   Term_Type = VT2XX; else
  if (strncmp(S,"vt3",3)==0)
   Term_Type=VT3XX; else
  if (strncmp(S,"vt4",3)==0)
   Term_Type=VT4XX; else
  if (strncmp(S,"ansi",4)==0)
   Term_Type=ANSI; else
   Term_Type=Tcap;
 }
 if (Term_Type==-1)
 {Fatalerror(Unsupported_Term_Txt,S);
  Reset_Width();
  B_Reset(&Pad);
  B_Reset(&Com);
  B_Reset(&Del);
  Reset_Buf();
#if defined(TERMCAP)
  UEnd();
#endif
  if (Sharemess)
  {printf("%s\n",Share_1_Txt);
   printf("%s\n",Share_2_Txt);
   printf("%s\n",Share_3_Txt);
   printf("%s\n",Share_4_Txt);
   printf("%s\n",Share_5_Txt);
   printf("%s\n",Share_6_Txt);
  }
  _exit(1);
 }
#if !defined(SCO) & !defined(HPUX) & !defined(LINUX)
 if (ioctl(0,TIOCGWINSZ,&ws)!=-1&&ws.ws_row!=0&&ws.ws_col!=0)
 {Scr_Length=ws.ws_row;
  Scr_Width=ws.ws_col;
 }else
 {
#if defined(TERMCAP)
  Scr_Length = tgetnum ("li");/* get number of lines/columes from termcap */
  Scr_Width = tgetnum ("co");
#endif
 }
#else
#if defined(TERMCAP)
 Scr_Length = tgetnum ("li");/* get number of lines/columes from termcap */
 Scr_Width = tgetnum ("co");
#endif
#endif
#if defined(TERMCAP)
 p = tcap;
 CH = tgetstr ("ch", &p);
 CE = tgetstr ("ce", &p);
 CL = tgetstr ("cl", &p);
 CM = tgetstr ("cm", &p);
 CS = tgetstr ("cs", &p);
 IS = tgetstr ("is", &p);
 KE = tgetstr ("ke", &p);
 KS = tgetstr ("ks", &p);
 MB = tgetstr ("mb", &p);
 MD = tgetstr ("md", &p);
 ME = tgetstr ("me", &p);
 MR = tgetstr ("mr", &p);
 UL = tgetstr ("ul", &p);
 SR = tgetstr ("sr", &p);
 SO = tgetstr ("so", &p);
 SE = tgetstr ("se", &p);
 if (SR == NULL) 
  SR = "\033M";
 SF = tgetstr ("sf", &p);	
 if (SF == NULL)
  SF = "\033D";
#endif
 if (Scr_Length > Maxlines)
  Scr_Length = Maxlines;
 if (Scr_Width > Maxcol)
  Scr_Width = Maxcol;
 Phys_Length=Max_Length=Scr_Length;
 Max_Length=Sub_Window?Sub_H:Max_Length;  /* 290a */
 Scr_Length=Max_Length-2;		/* 290a */ 
 Phys_Width=Max_Width=Scr_Width;
 Max_Width=Scr_Width=Sub_Window?Sub_W:Scr_Width; /* 290a */
#if defined(TERMCAP)
 Org_Width = Scr_Width;
 Scl_Regions=Term_Type==VT1XX||Term_Type==VT2XX||Term_Type==VT3XX||
  Term_Type==VT4XX||CS!=NULL;
#endif
#endif
#if defined(VMS)
 register char C, *S;
 VMS_Application=T$Application();
 if (Call_Type!=0)
 {switch (Call_Type)
  {case 1:
    Term_Type=VT1XX;
    break;
   case 2:
    Term_Type=VT2XX;
    break;
   case 3:
    Term_Type=VT52;
    break;
   case 4:
    Term_Type=VK100;
    break;
   case 5:
    Term_Type=ANSI;
    break;
   case 6:
    Term_Type=VT3XX;
    break;
  }
 } else
 {Term_Type=VT2XX;
  S=getenv("TERM");
  if (strcmp(S,"vt52")==0)
   Term_Type=VT52; else
  if (strncmp(S,"vt1",3)==0)
  {Term_Type=VT1XX;
  } else
  if (strncmp(S,"pro",3)==0)
  {Term_Type=VT1XX;
  } else
  if (strncmp(S,"vt2",3)==0)
  {Term_Type=VT2XX;
  } else
  if (strncmp(S,"vt3",3)==0)
  {Term_Type=VT3XX;
  } else
  if (strncmp(S,"vt4",3)==0)
  {Term_Type=VT4XX;
  } else
  {Out_S("\033[0c");
   if (Termin()=='\033')
   {if ((C=Termin())=='['&&(C=Termin())=='?')
    {if ((C=Termin())=='6')
     {if ((C=Termin())==';'||C=='c'||C=='C'||C=='1')
       Term_Type=VT1XX; else
      if (C=='2')
       Term_Type=VT2XX; else
      if (C=='3')
       Term_Type=VT3XX; else
      if (C=='4')
       Term_Type=VT4XX; else
       Term_Error();
     } else
     if (C=='5')
      Term_Type=VK100; else
     if (C=='1')
      Term_Type=VT1XX; else
      Term_Error();
     while (C!='c'&&C!='C')
      C=Termin();
    } else
     Term_Error();
   } else
   if (C==155&&(C=Termin())=='?')
   {if ((C=Termin())=='6')
    {if ((C=Termin())==';'||C=='c'||C=='C'||C=='1')
      Term_Type=VT1XX; else
     if (C=='2')
      Term_Type=VT2XX; else
     if (C=='3')
      Term_Type=VT3XX; else
     if (C=='4')
      Term_Type=VT4XX; else
      Term_Error();
    } else
     Term_Error();
    while (C!='c'&&C!='C')
     C=Termin();
   } else
    Term_Error();
  }
 }
 T$GETSIZE(&Scr_Length,&Scr_Width);
 if (Scr_Length>Maxlines)
  Scr_Length=Maxlines;
 if (Scr_Width>Maxcol)
  Scr_Width=Maxcol;
 Phys_Length=Max_Length=Scr_Length;
 Max_Length=Sub_Window?Sub_H:Max_Length;  /* 290a */
 Scr_Length=Max_Length-2;		/* 290a */ 
 Phys_Width=Max_Width=Scr_Width;
 Org_Width=Scr_Width;
 Max_Width=Scr_Width=Sub_Window?Sub_W:Scr_Width; /* 290a */
 Scl_Regions=Term_Type==VT1XX||Term_Type==VT2XX||Term_Type==VT3XX||
  Term_Type==VT4XX;
#endif
#if defined(DOS) | defined(NT) 
#if !defined(Windows)
#if !defined(PC100)
 Get_Type();
 Term_Type=Call_Type;
 switch (Term_Type)
 {case 0xD2:
  case 0xFC:
  {Color=Call_Color==0?((GETEQ()>>4)&3)!=3:Call_Color==2;
   if (Term_Type==0xFC)
   {Term_Type=IBM_AT;
    switch (Call_Model)
    {case 0x1:
      Term_Type=AT_339;
      Int16_Ext=1;
      break;
     case 0x2:
      Term_Type=XT_286;
      Int16_Ext=1;
      break;
     case 0x4:
      Term_Type=PS2_50;
      Int16_Ext=1;
      break;
     case 0x5:
      Term_Type=PS2_60;
      Int16_Ext=1;
      break;
    }
   } else
    Term_Type=PCXX;
   OURKBD();
   if (Keyb_Type==0)
   {if (!pc_enh_keyb())
     Keyb_Type=Keyb_AT; else
     Keyb_Type=Keyb_New;
   }
   if (Set_Num_Lock==-1&&(Mapping==EDT_Mapping||Mapping==WPS_Mapping))
   {switch (Keyb_Type)
    {case Keyb_PC:
     case Keyb_250:
     case Keyb_New:
     case Keyb_Conv:
     case Keyb_TC:
     case Keyb_AT:
      Set_Num_Lock=1;
      break;
     case Keyb_SLT:
      Set_Num_Lock=0;
      break;
    }
   }
#if !defined(TV)
   Sub_Set(25,80);
#endif
   break;
  }
  case 0xFE:
  {OURKBD();
   Color=Call_Color==0?((GETEQ()>>4)&3)!=3:Call_Color==2;
   Term_Type=IBM_XT;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_PC;
#if !defined(TV)
   Sub_Set(25,80);
#endif
   break;
  }
  case 0xFB:
  {OURKBD();
   Color=Call_Color==0?((GETEQ()>>4)&3)!=3:Call_Color==2;
   Term_Type=XT_2;
   Int16_Ext=1;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New;
#if !defined(TV)
   Sub_Set(25,80);
#endif
   break;
  }
  case 0xF9:
  {OURKBD();
   Color=Call_Color==0?((GETEQ()>>4)&3)!=3:Call_Color==2;
   Term_Type=IBM_Conv;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_Conv;
#if !defined(TV)
   Sub_Set(25,80);
#endif
   break;
  }
  case 0xFF:
  {OURKBD();
   Color=Call_Color==0?((GETEQ()>>4)&3)!=3:Call_Color==2;
   Term_Type=IBM_PC;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_PC;
#if !defined(TV)
   Sub_Set(25,80);
#endif
   break;
  }
  case 0xFA:
   OURKBD();
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New;
   Color=Call_Color==0?((GETEQ()>>4)&3)!=3:Call_Color==2;
   Term_Type=PS2_30;
   Int16_Ext=1;
#if !defined(TV)
   Sub_Set(25,80);
#endif
   break;
  case 0xF8:
   OURKBD();
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New;
   Color=Call_Color==0?((GETEQ()>>4)&3)!=3:Call_Color==2;
   Term_Type=PS2_80;
   Int16_Ext=1;
#if !defined(TV)
   Sub_Set(25,80);
#endif
   break;
  case 2:
   OURKBD();
   if (Keyb_Type==0)
    Keyb_Type=Keyb_TC;
   Color=Call_Color==0?((GETEQ()>>4)&3)!=3:Call_Color==2;
   Term_Type=TANDY_1400;
#if !defined(TV)
   Sub_Set(25,80);
#endif
   break;
  case 1:
  default:
  {Term_Type=AT_339;
   Keyb_Type=Keyb_New;
   break;
  }
 }
#if defined(DESQVIEW)
 if ((DV=api_init())!=0)
 {Ptr=ptr_new();
  ptr_open(Ptr,win_me());
  ptr_addto(Ptr,0x4);
 }
#endif
#if !defined(TV)
 if (Dir_Video)
 {
#if defined(DESQVIEW)
  if (DV)
  {if ((Video_Seg=((api_shadow()))>>16)==0)
    Video_Seg=Color?0xB800:0xB000;
  } else
#endif
   Video_Seg=Color?0xB800:0xB000;
 }
 if (EGA43)
  Sub_Set(43,80); else
 {if (New_Mode)
   Old_Mode=Set_Mode(New_Mode);
  Get_Mode((int far *)&Scr_Length,(int far *)&Scr_Width);
  Sub_Set(Scr_Length,Scr_Width);
  if (New_Mode)
   Set_Mode(Old_Mode);
 }
#endif
#else
 Term_Type=PC100;
 Keyb_Type=0;
#endif
 Scl_Regions=1;
#else
 Get_Type(); 
 Term_Type=Call_Type;
 switch (Term_Type)
 {case 0xD2:
  case 0xFC:
  {if (Term_Type==0xFC)
   {Term_Type=IBM_AT;
    switch (Call_Model)
    {case 0x1:
      Term_Type=AT_339;
      if (Keyb_Type==0)
       Keyb_Type=Keyb_New;
      break;
     case 0x2:
      Term_Type=XT_286;
      if (Keyb_Type==0)
       Keyb_Type=Keyb_New;
      break;
     case 0x4:
      Term_Type=PS2_50;
      if (Keyb_Type==0)
       Keyb_Type=Keyb_New;
      break;
     case 0x5:
      Term_Type=PS2_60;
      if (Keyb_Type==0)
       Keyb_Type=Keyb_New;
      break;
     default:
      if (Keyb_Type==0)
       Keyb_Type=Keyb_AT;
      break;
    } 
   } else
   {Term_Type=PCXX;
    if (Keyb_Type==0)
     Keyb_Type=Keyb_250;
   }
   break;
  }
  case 0xFE:
  {Term_Type=IBM_XT;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_PC;
   break;
  }
  case 0xFB:
  {Term_Type=XT_2;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New;
   break;
  }
  case 0xF9:
  {Term_Type=IBM_Conv;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_Conv;
   break;
  }
  case 0xFF:
  {Term_Type=IBM_PC;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_PC;
   break;
  }
  case 0xFA:
   Term_Type=PS2_30;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New;
   break;
  case 0xF8:
   Term_Type=PS2_80;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New;
   break;
  default:
  {Term_Type=AT_339;
   Keyb_Type=Keyb_New;
   break;
  }
 }
 if (Set_Num_Lock==-1&&(Mapping==EDT_Mapping||Mapping==WPS_Mapping))
 {switch (Keyb_Type)
  {case Keyb_PC:
   case Keyb_250:
   case Keyb_New:
   case Keyb_Conv:
   case Keyb_TC:
   case Keyb_AT:
    Set_Num_Lock=1;
    break;
   case Keyb_SLT:
    Set_Num_Lock=0;
    break;
  }
 }
 Scl_Regions=1;
#endif
#endif
#if defined(OS2)
 USHORT Mask;
 Get_Type();
 Term_Type=Call_Type;
 switch (Term_Type)
 {case 0xFB:
  {Term_Type=XT_2;
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New; 
   break;
  }
  case 0xFA:
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New;
   Term_Type=PS2_30;
   Int16_Ext=1;
   break;
  case 0xF8:
   if (Keyb_Type==0)
    Keyb_Type=Keyb_New;
   Term_Type=PS2_80;
   Int16_Ext=1;
   break;
  case 0xD2:
  case 0xFC:
  default:
   if (Term_Type==0xFC)
   {Term_Type=IBM_AT;
    switch (Call_Model)
    {case 0x1:
      Term_Type=AT_339;
      if (Keyb_Type==0)
       Keyb_Type=Keyb_New;
      Int16_Ext=1;
      break;
     case 0x2:
      Term_Type=XT_286;
      if (Keyb_Type==0)
       Keyb_Type=Keyb_New;
      Int16_Ext=1;
      break;
     case 0x4:
      Term_Type=PS2_50;
      if (Keyb_Type==0)
       Keyb_Type=Keyb_New;
      Int16_Ext=1;
      break;
     case 0x5:
      Term_Type=PS2_60;
      if (Keyb_Type==0)
       Keyb_Type=Keyb_New;
      Int16_Ext=1;
      break;
     default:
      if (Keyb_Type==0)
       Keyb_Type=Keyb_AT;
    }
   } else
   {Term_Type=PCXX;
    if (Keyb_Type==0)
     Keyb_Type=Keyb_250;
   }
   break;
 }
 if (Set_Num_Lock==-1&&(Mapping==EDT_Mapping||Mapping==WPS_Mapping))
 {switch (Keyb_Type)
  {case Keyb_PC:
   case Keyb_250:
   case Keyb_New:
   case Keyb_Conv:
   case Keyb_TC:
   case Keyb_AT:
    Set_Num_Lock=1;
    break;
   case Keyb_SLT:
    Set_Num_Lock=0;
    break;
  }
 }
 Mode_Data.cb=12;
 VioGetMode((PVIOMODEINFO)&Mode_Data,0);
 Sub_Set(Mode_Data.row,Mode_Data.col);
 Old_Length=Scr_Length;
 VioGetCurType((PVIOCURSORINFO)&Cursor_Data,0);
 DosGetInfoSeg((PSEL)&Gdt_Descriptor,(PSEL)&Ldt_Descriptor);
 Gdt=MAKEPGINFOSEG(Gdt_Descriptor);
 Ldt=MAKEPLINFOSEG(Ldt_Descriptor);
 if (Scan_Defs!=0)
 {DosMonOpen("KBD$",(PHMONITOR)&KBD_Handle);
  InBuff.bufflen=64;
  OutBuff.bufflen=64;
  if (DosMonReg(KBD_Handle,(PBYTE)&InBuff,(PBYTE)&OutBuff,0x0002,  Gdt->sgCurrent))
   Initerror(Not_Enough_Memory_Txt);
  Thread_ID=_beginthread((void (far *)(void far *))Mon_Thread,(void far *)NULL,  4096,(void far *)NULL);
  DosSetPrty(PRTYS_THREAD,PRTYC_REGULAR,+2,Thread_ID);
 }
 if (Use_Mouse)
 {DosSemRequest((HSEM)&Mouse_Sem,-1L);
  if (MouOpen(0L,(PHMOU)&Mouse_Handle))
   Gotmouse=0; else
  {Mask=(1<<1)+(1<<2)+(1<<4);
   MouSetEventMask((PUSHORT)&Mask,Mouse_Handle);
   Mouse_ID=_beginthread((void (far *)(void far *))Mouse_Thread,(void far *)NULL,4096,    (void far *)NULL);
   /*DosSetPrty(PRTYS_THREAD,PRTYC_REGULAR,+1,Mouse_ID);*/
   Gotmouse=1;
  }
 }
 Scl_Regions=1;
#endif
#if defined(ATARI)
 Scl_Regions=1;
 Max_Length=Scr_Length;
 Max_Width=Scr_Width;
#endif
#if defined(MPE)
 Max_Length=Scr_Length;
 Max_Width=Scr_Width;
 Scl_Regions=0;
#endif
}

#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
void Fast Sub_Set(int H,int W)
{Phys_Length=H;
 Phys_Width=W;
 Max_Length=Scr_Length=Sub_Window?Sub_H:H;
 Max_Width=Scr_Width=Sub_Window?Sub_W:W;
}
#endif

/*      Set the physical cursor to position Line, Col.
*/
void Fast Set_Cursor(int Line,int Col)
{
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type)
 {case ANSI:
  case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  {Out_S("\033[");
   Out_I(Line+Top_Row);
   Out_C(';');
   Out_I(Col+Left_Col);
   Out_C('H');
   break;
  }
  case VT52:
  {Out_S("\033Y");
   Out_C(037+Line+Top_Row);
   Out_C(037+Col+Left_Col);
   break;
  }
  case Tcap:
  case Default:
  default:
   tputs (tgoto (CM, Col+Left_Col-1,Line+Top_Row-1),1,Out_C);
 }
#else
   Out_Zap();
   move(Line+Top_Row-1,Col+Left_Col-1);
#endif
#endif
#if defined(VMS)
 if (Echo_Read)
  Force_Cursor=1;
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
  case Default:
  {Out_S("\033[");
   Out_I(Line+Top_Row);
   Out_C(';');
   Out_I(Col+Left_Col);
   Out_C('H');
   break;
  }
  case VT52:
  {Out_S("\033Y");
   Out_C(037+Line+Top_Row);
   Out_C(037+Col+Left_Col);
  }
 }
#endif
#if defined(WIN32)
 Out_Zap();
 WIN32cursor(Sub_X+Top_Row+Line-1,Sub_Y+Left_Col+Col-1);
#endif

#if defined(DOS) | defined(NT) 
#if !(defined(Windows) | defined(TV))
#if !defined(PC100)
 Out_Zap();
 IBMcursor(Sub_X+Top_Row+Line-1,Sub_Y+Left_Col+Col-1);
#else
 Out_S("\033[");
 Out_I(Line);
 Out_C(';');
 Out_I(Col);
 Out_C('H');
#endif
#else
 if (Out_Count)
  Out_Zap();
#if defined(TV)
 TV_Cursor(Line,Col);
#endif
#endif
#endif
#if defined(OS2)
 VioSetCurPos(Sub_X+Line-1+Top_Row,Sub_Y+Col-1+Left_Col,0);
#endif
#if defined(ATARI)
#if !defined(GEM)
 Out_S("\033Y");
 Out_C(037+Line+Top_Row);
 Out_C(037+Col+Left_Col);
#else
 if (Out_Count)
  Out_Zap();
#endif
#endif
#if defined(MPE)
 Out_S("\033&a");
 Out_I(Col-1);
 Out_C('c');
 Out_I(Line-1);
 Out_C('R');
#endif
 His_Line=Line;
 His_Col=Col;
}

/*      Scrolls the screen and database Scroll lines
        The routine is divided into two parts. One for terminals with scroll 
        regions, which can use hardware scrolling. The other for terminals 
	with no scroll regions, which Sedt must simulate scroll regions on.
*/
void Fast Opt_Scroll(int Scroll,int First,int Last)
{
#if defined(VMS) | defined(UNIX)
 int Sav_Line, L, This_Last, That_Last;
 long Sav_Col;
 char *P, *A;
 /*Diag("Opt_Scroll:Scroll=%d,First=%d,Last=%d",Scroll,First,Last);*/
 if (!Update_Flag)
  return;
 Set_Scroll(First+=Home,Last+=Home);
 if (Scl_Regions&&Scr_Width==Max_Width)
 {if (Scroll>0)
  {Set_Cursor(Last,1);
   Scl_Up(Scroll);
   {register int I;
    for (I=First;I<=Last-Scroll;I++)
    {
     This_Last=Lst_Col[I+Top_Row];
     if (That_Last=Lst_Col[I+Scroll+Top_Row])
     {C_Move(My_Screen[I+Scroll+Top_Row]+1+Left_Col,
       My_Screen[I+Top_Row]+1+Left_Col,That_Last,0);
      C_Move(My_Attributes[I+Scroll+Top_Row]+1+Left_Col,
       My_Attributes[I+Top_Row]+1+Left_Col,That_Last,0);
     }
     if (This_Last>That_Last)
     {Space_Fill(My_Screen[I+Top_Row]+That_Last+1+Left_Col,
       This_Last-That_Last);
      Zero_Fill(My_Attributes[I+Top_Row]+That_Last+1+Left_Col,
       This_Last-That_Last);
     }
     Lst_Col[I+Top_Row]=That_Last;
    }
   }
   {register int I;
    for (I=Last-Scroll+1;I<=Last;I++)
     if (This_Last=Lst_Col[I+Top_Row])
     {Space_Fill(My_Screen[I+Top_Row]+1+Left_Col,This_Last);
      Zero_Fill(My_Attributes[I+Top_Row]+1+Left_Col,This_Last);
      Lst_Col[I+Top_Row]=0;
     }
   }
  } else
  {Scroll=(-Scroll);
   Set_Cursor(First,1);
   Scl_Down(Scroll);
   {register int I;
    for (I=Last;I>=First+Scroll;I--)
    {
     This_Last=Lst_Col[I+Top_Row];
     if (That_Last=Lst_Col[I-Scroll+Top_Row])
     {C_Move(My_Screen[I-Scroll+Top_Row]+1+Left_Col,
       My_Screen[I+Top_Row]+1+Left_Col,That_Last,0);
      C_Move(My_Attributes[I-Scroll+Top_Row]+1+Left_Col,
       My_Attributes[I+Top_Row]+1+Left_Col,That_Last,0);
     }
     if (This_Last>That_Last)
     {Space_Fill(My_Screen[I+Top_Row]+That_Last+1+Left_Col,
       This_Last-That_Last);
      Zero_Fill(My_Attributes[I+Top_Row]+That_Last+1+Left_Col,
       This_Last-That_Last);
     }
     Lst_Col[I+Top_Row]=That_Last;
    }
   }
   {register int I;
    for (I=First;I<=Scroll+First-1;I++)
     if (This_Last=Lst_Col[I+Top_Row])
     {Space_Fill(My_Screen[I+Top_Row]+1+Left_Col,This_Last);
      Zero_Fill(My_Attributes[I+Top_Row]+1+Left_Col,This_Last);
      Lst_Col[I+Top_Row]=0;
     }
   }
  }
 } else
 {if (Scroll>0)
  {Sav_Line=My_Line;
   Sav_Col=My_Col;
   {register int I;
    for (I=Scl_Top;I<=Scl_Bottom-Scroll;I++)
    {Opt_Cursor(I,(long)1);
     P=My_Screen[My_Line+Scroll+Top_Row]+Left_Col;
     A=My_Attributes[My_Line+Scroll+Top_Row]+Left_Col;
     L=Lst_Col[My_Line+Scroll+Top_Row];
     My_Col=Scr_Shift+1;
     for (Scr_Col=1;Scr_Col<=L;Scr_Col++)
     {Cur_Attributes=A[Scr_Col];
      Opt_Char(P[Scr_Col]);
     }
     Opt_Erase_Line();
    }
   }
   {register int I;
    for (I=Scl_Bottom-Scroll+1;I<=Scl_Bottom;I++)
    {Opt_Cursor(I,(long)1);
     Opt_Erase_Line();
    }
   }
   Opt_Cursor(Sav_Line,Sav_Col);
  } else
  {Scroll=(-Scroll);
   Sav_Line=My_Line;
   Sav_Col=My_Col;
   {register int I;
    for (I=Scl_Bottom;I>=Scl_Top+Scroll;I--)
    {Opt_Cursor(I,(long)1);
     P=My_Screen[My_Line-Scroll+Top_Row]+Left_Col;
     A=My_Attributes[My_Line-Scroll+Top_Row]+Left_Col;
     L=Lst_Col[My_Line-Scroll+Top_Row];
     My_Col=Scr_Shift+1;
     for (Scr_Col=1;Scr_Col<=L;Scr_Col++)
     {Cur_Attributes=A[Scr_Col];
      Opt_Char(P[Scr_Col]);
     }
     Opt_Erase_Line();
    }
   }
   {register int I;
    for (I=Scl_Top+Scroll-1;I>=Scl_Top;I--)
    {Opt_Cursor(I,(long)1);
     Opt_Erase_Line();
    }
   }
   Opt_Cursor(Sav_Line,Sav_Col);
  }
 }
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (!Update_Flag)
  return;
 Out_Zap();
 Set_Scroll(First+=Home,Last+=Home);
 if (Scroll>0)
 {Scl_Up(Scroll);
  {register int I;
   for (I=First;I<=Last-Scroll;I++)
    Lst_Col[I]=Lst_Col[I+Scroll];
   for (I=Last-Scroll+1;I<=Last;I++)
    Lst_Col[I]=0;
  }
 } else
 {Scroll=(-Scroll);
  Scl_Down(Scroll);
  {register int I;
   for (I=Last;I>=First+Scroll;I--)
    Lst_Col[I]=Lst_Col[I-Scroll];
   for (I=First;I<=Scroll+First-1;I++)
    Lst_Col[I]=0;
  }
 }
#endif
#if (defined(DOS) & !defined(Windows) & !defined(TV)) | defined(OS2)  | defined(WIN32)
 if (!Update_Flag)
  return;
 Out_Zap();
 Set_Scroll(First+=Home,Last+=Home);
 if (Scroll>0)
 {Scl_Up(Scroll);
  {register int I;
   for (I=First;I<=Last-Scroll;I++)
    Lst_Col[I]=Lst_Col[I+Scroll];
  }
  if (Lst_Col[Last-Scroll+1]!=0)
   Lst_Col[Last-Scroll+1]=0;
  {register int I;
   for (I=Last-Scroll+2;I<=Last;I++)
   {if (Lst_Col[I]!=0)
     Lst_Col[I]=0;
   }
  }
 } else
 {Scroll=(-Scroll);
  Scl_Down(Scroll);
  {register int I;
   for (I=Last;I>=First+Scroll;I--)
    Lst_Col[I]=Lst_Col[I-Scroll];
  }
  if (Lst_Col[First]!=0)
   Lst_Col[First]=0;
  {register int I;
   for (I=First+1;I<=Scroll+First-1;I++)
   {if (Lst_Col[I]!=0)
     Lst_Col[I]=0;
   }
  }
 }
#endif
#if defined(DOS) & defined(TV)
 int Sav_Line, L;
 long Sav_Col;
 char *P, *A;
 /*Diag("Opt_Scroll:Scroll=%d,First=%d,Last=%d",Scroll,First,Last);*/
 if (!Update_Flag)
  return;
 Set_Scroll(First+=Home,Last+=Home);
 if (Scroll>0)
 {Sav_Line=My_Line;
  Sav_Col=My_Col;
  {register int I;
   for (I=Scl_Top;I<=Scl_Bottom-Scroll;I++)
   {Opt_Cursor(I,(long)1);
    P=My_Screen[My_Line+Scroll];
    A=My_Attributes[My_Line+Scroll];
    L=Lst_Col[My_Line+Scroll];
    My_Col=Scr_Shift+1;
    for (Scr_Col=1;Scr_Col<=L;Scr_Col++)
    {Cur_Attributes=A[Scr_Col];
     Opt_Char(P[Scr_Col]);
    }
    Opt_Erase_Line();
   }
  }
  {register int I;
   for (I=Scl_Bottom-Scroll+1;I<=Scl_Bottom;I++)
   {Opt_Cursor(I,(long)1);
    Opt_Erase_Line();
   }
  }
  Opt_Cursor(Sav_Line,Sav_Col);
 } else
 {Scroll=(-Scroll);
  Sav_Line=My_Line;
  Sav_Col=My_Col;
  {register int I;
   for (I=Scl_Bottom;I>=Scl_Top+Scroll;I--)
   {Opt_Cursor(I,(long)1);
    P=My_Screen[My_Line-Scroll];
    A=My_Attributes[My_Line-Scroll];
    L=Lst_Col[My_Line-Scroll];
    My_Col=Scr_Shift+1;
    for (Scr_Col=1;Scr_Col<=L;Scr_Col++)
    {Cur_Attributes=A[Scr_Col];
     Opt_Char(P[Scr_Col]);
    }
    Opt_Erase_Line();
   }
  }
  {register int I;
   for (I=Scl_Top+Scroll-1;I>=Scl_Top;I--)
   {Opt_Cursor(I,(long)1);
    Opt_Erase_Line();
   }
  }
  Opt_Cursor(Sav_Line,Sav_Col);
 }
#endif
#if defined(ATARI)
 int This_Last, That_Last, Sav_Line, L, Scr_Col;
 long Sav_Col;
 char *P, *A;
 if (!Update_Flag)
  return;
 Set_Scroll(First+=Home,Last+=Home);
 if (Scr_Width==Max_Width)
 {if (Scroll>0)
  {Scl_Up(Scroll);
   {register int I;
    for (I=First;I<=Last-Scroll;I++)
    {
     This_Last=Lst_Col[I+Top_Row];
     if (That_Last=Lst_Col[I+Scroll+Top_Row])
     {C_Move(My_Screen[I+Scroll+Top_Row]+1+Left_Col,
       My_Screen[I+Top_Row]+1+Left_Col,That_Last,0);
      C_Move(My_Attributes[I+Scroll+Top_Row]+1+Left_Col,
       My_Attributes[I+Top_Row]+1+Left_Col,That_Last,0);
     }
     if (This_Last>That_Last)
     {Space_Fill(My_Screen[I+Top_Row]+That_Last+1+Left_Col,This_Last-That_Last);
      Zero_Fill(My_Attributes[I+Top_Row]+That_Last+1+Left_Col,This_Last-That_Last);
     }
     Lst_Col[I+Top_Row]=That_Last;
    }
   }
  } else
  {Scroll=(-Scroll);
   Scl_Down(Scroll);
   {register int I;
    for (I=Last;I>=First+Scroll;I--)
    {
     This_Last=Lst_Col[I+Top_Row];
     if (That_Last=Lst_Col[I-Scroll+Top_Row])
     {C_Move(My_Screen[I-Scroll+Top_Row]+1+Left_Col,
       My_Screen[I+Top_Row]+1+Left_Col,That_Last,0);
      C_Move(My_Attributes[I-Scroll+Top_Row]+1+Left_Col,
       My_Attributes[I+Top_Row]+1+Left_Col,That_Last,0);
     }
     if (This_Last>That_Last)
     {Space_Fill(My_Screen[I+Top_Row]+That_Last+1+Left_Col,This_Last-That_Last);
      Zero_Fill(My_Attributes[I+Top_Row]+That_Last+1+Left_Col,This_Last-That_Last);
     }
     Lst_Col[I+Top_Row]=That_Last;
    }
   }
  }
 } else
 {if (Scroll>0)
  {Sav_Line=My_Line;
   Sav_Col=My_Col;
   {register int I;
    for (I=Scl_Top;I<=Scl_Bottom-Scroll;I++)
    {Opt_Cursor(I,(long)1);
     P=My_Screen[My_Line+Scroll+Top_Row]+Left_Col;
     A=My_Attributes[My_Line+Scroll+Top_Row]+Left_Col;
     L=Lst_Col[My_Line+Scroll+Top_Row];
     My_Col=Scr_Shift+1;
     for (Scr_Col=1;Scr_Col<=L;Scr_Col++)
     {Cur_Attributes=A[Scr_Col];
      Opt_Char(P[Scr_Col]);
     }
     Opt_Erase_Line();
    }
   }
   {register int I;
    for (I=Scl_Bottom-Scroll+1;I<=Scl_Bottom;I++)
    {Opt_Cursor(I,(long)1);
     Opt_Erase_Line();
    }
   }
   Opt_Cursor(Sav_Line,Sav_Col);
  } else
  {Scroll=(-Scroll);
   Sav_Line=My_Line;
   Sav_Col=My_Col;
   {register int I;
    for (I=Scl_Bottom;I>=Scl_Top+Scroll;I--)
    {Opt_Cursor(I,(long)1);
     P=My_Screen[My_Line-Scroll+Top_Row]+Left_Col;
     A=My_Attributes[My_Line-Scroll+Top_Row]+Left_Col;
     L=Lst_Col[My_Line-Scroll+Top_Row];
     My_Col=Scr_Shift+1;
     for (Scr_Col=1;Scr_Col<=L;Scr_Col++)
     {Cur_Attributes=A[Scr_Col];
      Opt_Char(P[Scr_Col]);
     }
     Opt_Erase_Line();
    }
   }
   {register int I;
    for (I=Scl_Top+Scroll-1;I>=Scl_Top;I--)
    {Opt_Cursor(I,(long)1);
     Opt_Erase_Line();
    }
   }
   Opt_Cursor(Sav_Line,Sav_Col);
  }
 }
#endif
#if defined(MPE)
 int Sav_Line, Sav_Col, L, Scr_Col;
 char *P, *A;
 if (!Update_Flag)
  return;
 Set_Scroll(First+=Home,Last+=Home);
 if (Scroll>0)
 {Sav_Line=My_Line;
  Sav_Col=My_Col;
  {register int I;
   for (I=Scl_Top;I<=Scl_Bottom-Scroll;I++)
   {Opt_Cursor(I,(long)1);
    P=My_Screen[My_Line+Scroll+Top_Row]+Left_Col;
    A=My_Attributes[My_Line+Scroll+Top_Row]+Left_Col;
    L=Lst_Col[My_Line+Scroll+Top_Row];
    My_Col=Scr_Shift+1;
    for (Scr_Col=1;Scr_Col<=L;Scr_Col++)
    {Cur_Attributes=A[Scr_Col];
     Opt_Char(P[Scr_Col]);
    }
    Opt_Erase_Line();
   }
  }
  {register int I;
   for (I=Scl_Bottom-Scroll+1;I<=Scl_Bottom;I++)
   {Opt_Cursor(I,(long)1);
    Opt_Erase_Line();
   }
  }
  Opt_Cursor(Sav_Line,Sav_Col);
 } else
 {Scroll=(-Scroll);
  Sav_Line=My_Line;
  Sav_Col=My_Col;
  {register int I;
   for (I=Scl_Bottom;I>=Scl_Top+Scroll;I--)
   {Opt_Cursor(I,(long)1);
    P=My_Screen[My_Line-Scroll+Top_Row]+Left_Col;
    A=My_Attributes[My_Line-Scroll+Top_Row]+Left_Col;
    L=Lst_Col[My_Line-Scroll+Top_Row];
    My_Col=Scr_Shift+1;
    for (Scr_Col=1;Scr_Col<=L;Scr_Col++)
    {Cur_Attributes=A[Scr_Col];
    Opt_Char(P[Scr_Col]);
     }
    Opt_Erase_Line();
   }
  }
  {register int I;
   for (I=Scl_Top+Scroll-1;I>=Scl_Top;I--)
   {Opt_Cursor(I,(long)1);
    Opt_Erase_Line();
   }
  }
  Opt_Cursor(Sav_Line,Sav_Col);
 }
#endif
}

#if !((defined(DOS) & defined(TV)) | defined(MPE))
/*      Scroll the current scroll region up by Scroll lines. This routine is 
	not called for terminals with no hardware for scroll regions.
*/
void Fast Scl_Up(int Scroll)
{
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
  {register int I;
   for (I=1;I<=Scroll;I++)
    Out_S("\033D");
   break;
  }
  case VT52:
  {register int I;
   for (I=1;I<=Scroll;I++)
    Out_S("\033B");
   break;
  }
  case Tcap:
  case Default:
  default:
   {register int I;
    for (I=1;I<=Scroll;I++)
     tputs (SF, 1, Out_C);
   }
 }
#endif
#endif
#if defined(VMS)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
  case Default:
  {register int I;
   for (I=1;I<=Scroll;I++)
    Out_S("\033D");
   break;
  }
  case VT52:
  {register int I;
   for (I=1;I<=Scroll;I++)
    Out_S("\033B");
  }
 }
#endif
#if defined(WIN32)
  WIN32scrup(Scroll,Sub_X+Top_Row+Scl_Top-1,Sub_Y+Left_Col,
   Sub_X+Top_Row+Scl_Bottom-1,Sub_Y+Left_Col+Scr_Width-1,Attab[0]);
#endif
#if defined(DOS) | defined(NT)
#if !(defined(Windows) | defined(TV))
#if !defined(PC100)
 if (Dir_Video)
 {register int I;
  for (I=Scl_Top;I<=Scl_Bottom-Scroll;I++)
#if defined(__BORLANDC__)
   movedata(Video_Seg,((Sub_X+Top_Row+I+Scroll-1)*Phys_Width+Sub_Y+Left_Col)*2,
#else
   _movedata(Video_Seg,((Sub_X+Top_Row+I+Scroll-1)*Phys_Width+Sub_Y+Left_Col)*2,
#endif
    Video_Seg,((Sub_X+Top_Row+I-1)*Phys_Width+Sub_Y+Left_Col)*2,
    Scr_Width*2);
  for (I=Scl_Bottom-Scroll+1;I<=Scl_Bottom;I++)
   Dir_Set(Sub_X+Top_Row+I-1,Sub_Y+Left_Col,' ',Attab[0],Scr_Width);
 } else
{
  IBMscrup(Scroll,Sub_X+Top_Row+Scl_Top-1,Sub_Y+Left_Col,
   Sub_X+Top_Row+Scl_Bottom-1,Sub_Y+Left_Col+Scr_Width-1,Attab[0]);
}
#else
 register int I;
 for (I=1;I<=Scroll;I++)
 Out_S("\033D");
#endif
#else
 Out_Zap();
#if defined(Windows)
 Win_Scroll(Scl_Top,Scl_Bottom,Scroll);
#endif
#endif
#endif
#if defined(OS2)
 char Cell[2];
 Cell[0]=' ';
 Cell[1]=Attab[0];
 VioScrollUp(Sub_X+Scl_Top-1+Top_Row,Sub_Y+Left_Col,
  Sub_X+Scl_Bottom-1+Top_Row,Sub_Y+Scr_Width-1+Left_Col,Scroll,
  (char far *)Cell,0);
#endif
#if defined(ATARI)
#if ! defined(GEM)
 register char *F, *T;
 Out_Zap();
 F=Logbase();
 T=F+(Scl_Top-1)*1280;
 F=F+(Scl_Top-1+Scroll)*1280;
 movmem(F,T,(Scl_Bottom-Scl_Top-Scroll+1)*1280);
#else
 Out_Zap();
 GEM_Up_Scroll(Scl_Top,Scl_Bottom,Scroll);
#endif
#endif
}

/*      Scroll the current scroll region down by Scroll lines. This routine
	is not called for terminals with no hardware for scroll regions.
*/ 
void Fast Scl_Down(int Scroll)
{
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type) 
 {case VT2XX: 
  case VT3XX:
  case VT4XX:
  case VT1XX: 
  case VK100:
  case ANSI:
  {register int I;
   for (I=1;I<=Scroll;I++) 
    Out_S("\033M"); 
   break; 
  } 
  case VT52: 
  {register int I;
   for (I=1;I<=Scroll;I++)
    Out_S("\033A");
   break;
  }
  case Tcap:
  case Default:
  default:
   {register int I;
    for (I=1;I<=Scroll;I++)
     tputs (SR, 1, Out_C);
   }
 } 
#endif
#endif
#if defined(VMS)
 switch (Term_Type) 
 {case VT2XX: 
  case VT3XX:
  case VT4XX:
  case VT1XX: 
  case VK100:
  case ANSI:
  case Default:
  {register int I;
   for (I=1;I<=Scroll;I++) 
    Out_S("\033M"); 
   break; 
  } 
  case VT52: 
  {register int I;
   for (I=1;I<=Scroll;I++)
    Out_S("\033A");
  }
 } 
#endif
#if defined(WIN32)
  WIN32scrdown(Scroll,Sub_X+Top_Row+Scl_Top-1,Sub_Y+Left_Col,
   Sub_X+Top_Row+Scl_Bottom-1,Sub_Y+Left_Col+Scr_Width-1,Attab[0]);
#endif
#if defined(DOS) | defined(NT) 
#if !(defined(Windows) | defined(TV))
#if !defined(PC100)
 if (Dir_Video)
 {register int I;
  for (I=Scl_Bottom;I>=Scl_Top+Scroll;I--)
#if defined(__BORLANDC__)
   movedata(Video_Seg,((Sub_X+Top_Row+I-Scroll-1)*Phys_Width+Sub_Y+Left_Col)*2,
    Video_Seg,((Sub_X+Top_Row+I-1)*Phys_Width+Sub_Y+Left_Col)*2,Scr_Width*2);
#else
   _movedata(Video_Seg,((Sub_X+Top_Row+I-Scroll-1)*Phys_Width+Sub_Y+Left_Col)*2,
    Video_Seg,((Sub_X+Top_Row+I-1)*Phys_Width+Sub_Y+Left_Col)*2,Scr_Width*2);
#endif
  for (I=Scl_Top+Scroll-1;I>=Scl_Top;I--)
   Dir_Set(Sub_X+Top_Row+I-1,Sub_Y+Left_Col,' ',Attab[0],Scr_Width);
 } else
{
  IBMscrdown(Scroll,Sub_X+Top_Row+Scl_Top-1,Sub_Y+Left_Col,
   Sub_X+Top_Row+Scl_Bottom-1,Sub_Y+Left_Col+Scr_Width-1,Attab[0]);
}
#else
 register int I;
 for (I=1;I<=Scroll;I++) 
  Out_S("\033M"); 
#endif
#else
 Out_Zap();
#if defined(Windows)
 Win_Scroll(Scl_Top,Scl_Bottom,-Scroll);
#endif
#endif
#endif
#if defined(OS2)
 char Cell[2];
 Cell[0]=' ';
 Cell[1]=Attab[0];
 VioScrollDn(Sub_X+Scl_Top-1+Top_Row,Sub_Y+Left_Col,
  Sub_X+Scl_Bottom-1+Top_Row,Sub_X+Scr_Width-1+Left_Col,Scroll,
  (char far *)Cell,0);
#endif
#if defined(ATARI)
#if ! defined(GEM)
 register char *F, *T;
 Out_Zap();
 F=Logbase();
 T=F+(Scl_Top-1+Scroll)*1280;
 F=F+(Scl_Top-1)*1280;
 movmem(F,T,(Scl_Bottom-Scl_Top-Scroll+1)*1280);
#else
 Out_Zap();
 GEM_Down_Scroll(Scl_Top,Scl_Bottom,Scroll);
#endif
#endif
}
#endif

/*      Clear the physical screen.
*/
void Fast Clr_Screen()
{
#if defined(WIN32)
 int Line, Col;

 WIN32scrclear();
#ifdef _removed_for_win32_clr








#endif
#endif

#if defined(UNIX)
 int Line, Col;
 if (Zoomed)
 {
#if defined(TERMCAP)
  switch (Term_Type)
  {case ANSI:
    if (His_Attributes!=0)
     Opt_Force_Attributes(0);
   case VT2XX:
   case VT3XX:
   case VT4XX:
   case VT1XX:
   case VK100:
   {Out_S("\033[2J");
    break;
   }
   case VT52:
   {Set_Cursor(1,1);
    Out_S("\033J");
    break;
   }
   case Tcap:
   case Default:
   default:
    if (His_Attributes!=0)
     Opt_Force_Attributes(0);
    tputs (CL, 1, Out_C);
  }
#else
    Out_Zap();
    clear();
#endif
 } else
 {Opt_Force_Attributes(0);
  for (Line=1;Line<=Scr_Length;Line++)
  {Set_Cursor(Line,1);
   for (Col=1;Col<=Scr_Width;Col++)
    Out_C(' ');
  }
  Out_Zap();
 }
#endif
#if defined(VMS)
 int Line, Col;
 if (Zoomed)
 {switch (Term_Type)
  {case VT2XX:
   case VT3XX:
   case VT4XX:
   case VT1XX:
   case VK100:
   case ANSI:
   case Default:
   {Out_S("\033[2J");
    break;
   }
   case VT52:
   {Set_Cursor(1,1);
    Out_S("\033J");
   }
  }
 } else
 {Opt_Force_Attributes(0);
  for (Line=1;Line<=Scr_Length;Line++)
  {Set_Cursor(Line,1);
   for (Col=1;Col<=Scr_Width;Col++)
    Out_C(' ');
  }
  Out_Zap();
 }
#endif
#if defined(DOS) | defined(NT)
#if !(defined(Windows) | defined(TV))
#if !defined(PC100)
 if (Dir_Video)
  {register int I;
   for (I=1;I<=Scr_Length;I++)
    Dir_Set(Sub_X+Top_Row+I-1,Sub_Y+Left_Col,' ',Attab[0],Scr_Width);
  } else
  IBMscrdown(0,Sub_X+Top_Row,Sub_Y+Left_Col,
   Sub_X+Top_Row+Scr_Length-1,Sub_Y+Left_Col+Scr_Width-1,Attab[0]);
#else
  Out_S("\033[2J");
#endif
#else
#if defined(TV)
 TV_Clear();
#else
 Win_Clear();
#endif
#endif
#endif
#if defined(OS2)
 char Cell[2];
 Cell[0]=' ';
 Cell[1]=Attab[0];
 VioScrollDn(Sub_X+Top_Row,Sub_Y+Left_Col,Sub_X+Scr_Length-1+Top_Row,
  Sub_Y+Scr_Width-1+Left_Col,Scr_Length,(char *)Cell,0);
#endif
#if defined(ATARI)
#if ! defined(GEM)
 int Line, Col;
 if (Zoomed)
 {Set_Cursor(1,1);
  Out_S("\033J");
 } else
 {Opt_Force_Attributes(0);
  for (Line=1;Line<=Scr_Length;Line++)
  {Set_Cursor(Line,1);
   for (Col=1;Col<=Scr_Width;Col++)
    Out_C(' ');
  }
  Out_Zap();
 }
#else
 GEM_Clear();
#endif
#endif
#if defined(MPE)
 Out_S("\033h");
 Out_S("\033J");
 His_Line=His_Col=1;
#endif
 {register int Line;
  for (Line=1;Line<=Maxlines;++Line)
  {
#if defined(VMS) | defined(ATARI) | defined(UNIX) |  (defined(DOS) & defined(TV)) | defined(MPE) | defined(WIN32)
   Space_Fill(My_Screen[Line]+1,Maxcol);
   Zero_Fill(My_Attributes[Line]+1,Maxcol);
   Lst_Col[Line]=0;
#else
   Lst_Col[Line]=0;
#endif
  }
 }
}

/*      Clear the physical screen. and reset to original attributes
*/
void Fast Reset_Screen()
{
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type)
 {case ANSI:
   if (His_Attributes!=0)
    Opt_Force_Attributes(0);
  case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  {Out_S("\033[2J");
   break;
  }
  case VT52:
  {Set_Cursor(1,1);
   Out_S("\033J");
   break;
  }
  case Tcap:
  case Default:
  default:
   if (His_Attributes!=0)
    Opt_Force_Attributes(0);
   tputs (CL, 1, Out_C);
 }
#else
   Out_Zap();
   clear();
#endif
#endif
#if defined(VMS)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
  case Default:
  {Out_S("\033[2J");
   break;
  }
  case VT52:
  {Set_Cursor(1,1);
   Out_S("\033J");
  }
 }
#endif
#if defined(DOS) | defined(NT) 
#if !(defined(Windows) | defined(TV))
#if !defined(PC100)
 if (Dir_Video)
  {register int I;
   for (I=1;I<=Max_Length;I++)
    Dir_Set(Sub_X+I-1,Sub_Y,' ',7,Max_Width);
  } else
   IBMscrdown(0,Sub_X,Sub_Y,Sub_X+Max_Length-1,Sub_Y+Max_Width-1,7);
#else
  Out_S("\033[2J");
#endif
#else
#if defined(TV)
 TV_Clear();
#else
 Win_Clear();
#endif
#endif
#endif
#if defined(OS2)
 char Cell[2];
 Cell[0]=' ';
 Cell[1]=7;
 VioScrollDn(Sub_X,Sub_Y,Sub_X+Max_Length-1,
  Sub_Y+Max_Width-1,Max_Length,(char *)Cell,0);
#endif
#if defined(ATARI)
#if ! defined(GEM)
 Set_Cursor(1,1);
 Out_S("\033J");
#else
 GEM_Clear();
#endif
#endif
#if defined(MPE)
 Out_S("\033h");
 Out_S("\033J");
 His_Line=His_Col=1;
#endif
 {register int Line;
  for (Line=1;Line<=Maxlines;++Line)
  {
#if defined(VMS) | defined(ATARI) | defined(UNIX) | (defined(DOS) & defined(TV)) | defined(MPE)
   Space_Fill(My_Screen[Line]+1,Maxcol);
   Zero_Fill(My_Attributes[Line]+1,Maxcol);
   Lst_Col[Line]=0;
#else
   Lst_Col[Line]=0;
#endif
  }
 }
}

/*      Set Scroll Region. This routine is also called for terminals with no 
        hardware scroll regions.
        The following variables define the scroll region
        	Scl_Top	The first ine in the region
        	Scl_Bottom	The last line in the region
*/
void Fast Set_Scroll(int Top,int Bottom)
{
 Top+=Top_Row;
 Bottom+=Top_Row;
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  {if (Scl_Top!=Top||Scl_Bottom!=Bottom)
   {Out_S("\033[");
    if (Top!=1)
     Out_I(Top);
    Out_C(';');
    if (Bottom!=Max_Length) 	/* 290a */ 
     Out_I(Bottom);
    Out_C('r');
    Set_Cursor(Top,1);
   }
   break;
  }
  case Tcap:
  case Default:
  default:
   if (CS != NULL)
    tputs (tgoto (CS, Bottom - 1, Top - 1), 1, Out_C);
 }
 His_Line=His_Col= -1;
#endif
#endif
#if defined(VMS)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case Default:
  {if (Scl_Top!=Top||Scl_Bottom!=Bottom)
   {Out_S("\033[");
    if (Top!=1)
     Out_I(Top);
    Out_C(';');
    if (Bottom!=Max_Length) 	/* 290a */ 
     Out_I(Bottom);
    Out_C('r');
    Set_Cursor(Top,1);
   }
   break;
  }
 }
 His_Line=His_Col= -1;
#endif
#if defined(DOS) | defined(NT) 
#if !(defined(Windows) | defined(TV))
#if defined(PC100)
 if (Scl_Top!=Top||Scl_Bottom!=Bottom)
 {Out_S("\033[");
  if (Top!=1)
   Out_I(Top);
  Out_C(';');
  if (Bottom!=Scr_Length)
   Out_I(Bottom);
  Out_C('r');
  Set_Cursor(Top,1);
 }
#endif
#endif
#endif
 Scl_Top=Top-Top_Row;
 Scl_Bottom=Bottom-Top_Row;
}

/*      Move the cursor right by Col characters without changing the characters 
        the cursor moves over.
*/
void Fast Move_Right(int Col)
{
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type)
 {case ANSI:
  case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  {Out_S("\033[");
   Out_I(Col-His_Col);
   Out_C('C');
   break;
  }
  case VT52:
  {register int I;
   for (I=His_Col;I<Col;I++)
    Out_S("\033C");
    break;
  }
  case Tcap:
  case Default:
  default:
   tputs (tgoto (CM, Col+Left_Col - 1, His_Line+Top_Row - 1), 1, Out_C);
 }
#else
   Out_Zap();
   move(His_Line+Top_Row-1,Col+Left_Col-1);
#endif
#endif
#if defined(VMS)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
  case Default:
  {Out_S("\033[");
   Out_I(Col-His_Col);
   Out_C('C');
   break;
  }
  case VT52:
  {register int I;
   for (I=His_Col;I<Col;I++)
    Out_S("\033C");
  }
 }
#endif
#if defined(WIN32)
 Set_Cursor(His_Line,Col);
#endif
#if defined(DOS) | defined(NT)
#if !(defined(Windows) | defined(TV))
#if !defined(PC100)
  IBMcursor(Top_Row+His_Line-1,Left_Col+Col-1);
#else
 Out_S("\033[");
 Out_I(Col-His_Col);
 Out_C('C');
#endif
#else
 Set_Cursor(His_Line,Col);
#endif
#endif
#if defined(OS2)
 VioSetCurPos(Sub_X+His_Line-1+Top_Row,Sub_Y+Col-1+Left_Col,0);
#endif
#if defined(ATARI)
#if ! defined(GEM)
 {register int I;
  for (I=His_Col;I<Col;I++)
   Out_S("\033C");
 }
#else
 Set_Cursor(His_Line,Col);
#endif
#endif
#if defined(MPE)
 Out_S("\033&a+");
 Out_I(Col-His_Col);
 Out_S("c+0R");
#endif
 His_Col=Col;
}

/*      Move the cursor Col characters left without changing the characters 
        the cursor moves over.
*/
void Fast Move_Left(int Col)
{
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
  {Out_S("\033[");
   Out_I(His_Col-Col);
   Out_C('D');
   break;
  }
  case VT52:
  {register int I;
   for (I=His_Col;I>Col;I--)
    Out_S("\033D");
   break;
  }
  case Tcap:
  case Default:
  default:
   tputs (tgoto (CM, Col+Left_Col - 1, His_Line+Top_Row - 1), 1, Out_C);
 }
#else
  Out_Zap();
  move(His_Line+Top_Row-1,Col+Left_Col-1);
#endif
#endif
#if defined(VMS)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
  case Default:
  {Out_S("\033[");
   Out_I(His_Col-Col);
   Out_C('D');
   break;
  }
  case VT52:
  {register int I;
   for (I=His_Col;I>Col;I--)
    Out_S("\033D");
  }
 }
#endif
#if defined(WIN32)
 Set_Cursor(His_Line,Col);
#endif
#if defined(DOS) | defined(NT) 
#if !(defined(Windows) | defined(TV))
#if !defined(PC100)
 His_Col-=Col;
 IBMcursor(Top_Row+His_Line-1,Left_Col+His_Col-1);
#else
 Out_S("\033[");
 Out_I(His_Col-Col);
 Out_C('D');
#endif
#else
 Set_Cursor(His_Line,Col);
#endif
#endif
#if defined(OS2)
 His_Col-=Col;
 VioSetCurPos(Sub_X+His_Line-1+Top_Row,Sub_Y+His_Col-1+Left_Col,0);
#endif
#if defined(ATARI)
#if ! defined(GEM)
 {register int I;
  for (I=His_Col;I>Col;I--)
   Out_S("\033D");
 }
#else
 Set_Cursor(His_Line,Col);
#endif
#endif
#if defined(MPE)
 Out_S("\033&a-");
 Out_I(His_Col-Col);
 Out_S("c+0R");
#endif
 His_Col=Col;
}

/*      Set the physical attributes of the terminal to A
*/
void Fast Opt_Force_Attributes(char A)
{
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type)
 {case VT2XX:
  case VT1XX:
  case VK100:
  case VT3XX:
  case VT4XX:
  {Out_S("\033[0");
   if (A&A_Bold)
    Out_S(";1");
   if (A&A_Blink)
    Out_S(";5");
   if (A&A_Underline)
    Out_S(";4");
   if (A&A_Reverse)
    Out_S(";7");
   Out_C('m');
   break;
  }
  case ANSI:
  {Out_S("\033[0m");
   if (A&A_Bold)
    Out_S("\033[1m");
   if (A&A_Blink)
    Out_S("\033[5m");
   if (A&A_Underline)
    Out_S("\033[4m");
   if (A&A_Reverse)
    Out_S("\033[7m");
   break;
  }
  case Tcap:
  case Default:
  default:
   if (ME==NULL)
    tputs(SE,1,Out_C); else
    tputs (ME, 1, Out_C);
   if (A & A_Bold)
   {if (MD==NULL)
     tputs(SO,1,Out_C); else
     tputs (MD, 1, Out_C);
   }
   if (A & A_Blink)
    tputs (MB, 1, Out_C);
   if (A & A_Underline)
    tputs (UL, 1, Out_C);
   if (A & A_Reverse)
   {if (MR==NULL)
    tputs(SO,1,Out_C); else
    tputs (MR, 1, Out_C);
   }
 }
#else
   Out_Zap();
   attrset(0);
   if (A & A_Bold)
    attron(A_BOLD);
   if (A & A_Blink)
    attron(A_BLINK);
   if (A & A_Underline)
    attron(A_UNDERLINE);
   if (A & A_Reverse)
    attron(A_REVERSE);
#endif
#endif
#if defined(VMS)
 switch (Term_Type)
 {case VT2XX:
  case VT1XX:
  case VK100:
  case VT3XX:
  case VT4XX:
  case Default:
  {Out_S("\033[0");
   if (A&A_Bold)
    Out_S(";1");
   if (A&A_Blink)
    Out_S(";5");
   if (A&A_Underline)
    Out_S(";4");
   if (A&A_Reverse)
    Out_S(";7");
   Out_C('m');
   break;
  }
  case ANSI:
  {Out_S("\033[0m");
   if (A&A_Bold)
    Out_S("\033[1m");
   if (A&A_Blink)
    Out_S("\033[5m");
   if (A&A_Underline)
    Out_S("\033[4m");
   if (A&A_Reverse)
    Out_S("\033[7m");
   break;
  }
 }
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
 Out_Zap();
#if defined(WIN32)
 WIN32attrib(A);
#endif
#if !(defined(Windows) | defined(TV))
#if defined(PC100)
 Out_S("\033[0");
 if (A&A_Bold)
  Out_S(";1");
 if (A&A_Blink)
  Out_S(";5");
 if (A&A_Underline)
  Out_S(";4");
 if (A&A_Reverse)
  Out_S(";7");
 Out_C('m');
#endif
#endif
#endif
#if defined(OS2)
 Out_Zap();
#endif
#if defined(ATARI)
#if ! defined(GEM)
 if (A&A_Reverse)
  Out_S("\033p"); else
  Out_S("\033q");
#else
 Out_Zap();
#endif
#endif
#if defined(MPE)
 int I;
 Out_S("\033&d");
 I='A'-1;
 if (A&A_Blink)
  I+=1;
 if (A&A_Reverse)
  I+=2;
 if (A&A_Underline)
  I+=4;
 if (!(A&A_Bold))
  I+=8;
 Out_C(I);
#endif
 His_Attributes=A;
}

/*      Erase the screen from the current cursor position to the end of the 
        line
*/
void Fast Del_To_End_Line()
{
#if defined(UNIX)
 long Col;
 if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
  Opt_Move_Cursor(My_Line,My_Col);
 if (Scr_Width==Max_Width)
 {
#if defined(TERMCAP)
  switch (Term_Type)
  {case ANSI:
    if (His_Attributes!=0)
     Opt_Force_Attributes(0);
   case VT2XX:
   case VT3XX:
   case VT4XX:
   case VT1XX:
   case VK100:
    Out_S("\033[K");
    break;
   case VT52:
    Out_S("\033K");
    break;
   case Tcap:
   case Default:
   default:
    if (His_Attributes!=0)
     Opt_Force_Attributes(0);
    tputs (CE, 1, Out_C);
  } 
#else
    Out_Zap();
    clrtoeol();
#endif
 } else
 {Opt_Force_Attributes(0);
  Col=My_Col;
  while (My_Col-Scr_Shift<=Lst_Col[My_Line+Top_Row]&&My_Col-Scr_Shift<=Scr_Width)
   Opt_Char(' ');
  My_Col=Col;
 }
#endif
#if defined(VMS)
 long Col;
 if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
  Opt_Move_Cursor(My_Line,My_Col);
 if (Scr_Width==Max_Width)
 {switch (Term_Type)
  {case VT2XX:
   case VT3XX:
   case VT4XX:
   case VT1XX:
   case VK100:
   case ANSI:
   case Default:
    Out_S("\033[K");
    break;
   case VT52:
    Out_S("\033K");
  }
 } else
 {Opt_Force_Attributes(0);
  Col=My_Col;
  while (My_Col-Scr_Shift<=Lst_Col[My_Line+Top_Row]&&
   My_Col-Scr_Shift<=Scr_Width)
   Opt_Char(' ');
  My_Col=Col;
 }
#endif
#if defined(WIN32)
 long Col;

 if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
  Opt_Move_Cursor(My_Line,My_Col);




 {Opt_Force_Attributes(0);
  Col=My_Col;
  while (My_Col-Scr_Shift<=Lst_Col[My_Line+Top_Row]&&
   My_Col-Scr_Shift<=Scr_Width)
   Opt_Char(' ');
  My_Col=Col;
 }
#endif
#if defined(DOS) | defined(NT) 
 Out_Zap();
#if !(defined(Windows) | defined(TV))
#if !defined(PC100)
 if (Scr_Width-Scr_Col+1>0)
 {if (Dir_Video)
   Dir_Set(Sub_X+Top_Row+My_Line-1,Sub_Y+Left_Col+Scr_Col-1,' ',
    Attab[0],Scr_Width-Scr_Col+1); else
  {IBMcursor(Top_Row+My_Line-1,Left_Col+Scr_Col-1);
   IBMcout(Scr_Width-Scr_Col+1,' ',Attab[0]);
  }
 }
#else
 Out_S("\033[K");
#endif
#else
#if defined(TV)
 TV_Erase(My_Line,My_Col-Scr_Shift,Scr_Width-Scr_Col+Scr_Shift+1);
#else
 Win_Erase(My_Line,Scr_Col,Scr_Width-Scr_Col+1);
#endif
#endif
#endif
#if defined(OS2)
 char Cell[2];
 Out_Zap();
 Cell[0]=' ';
 Cell[1]=Attab[0];
 VioWrtNCell((char far *)Cell,Scr_Width-Scr_Col+1,
  Sub_X+My_Line-1+Top_Row,Sub_Y+Scr_Col-1+Left_Col,0);
#endif
#if defined(ATARI)
#if ! defined(GEM)
 long Col;
 if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
  Opt_Move_Cursor(My_Line,My_Col);
 if (Scr_Width==Max_Width)
 {Opt_Force_Attributes(0);
  Out_S("\033K");
 } else
 {Opt_Force_Attributes(0);
  Col=My_Col;
  while (My_Col<=Lst_Col[My_Line+Top_Row]+Scr_Shift&&My_Col<=Scr_Width)
   Opt_Char(' ');
  My_Col=Col;
 }
#else
 GEM_Erase(My_Line,Scr_Col,Scr_Width-Scr_Col+1);
#endif
#endif
#if defined(MPE)
 long Col;
 if ((Scr_Col!=His_Col)||(My_Line!=His_Line))
  Opt_Move_Cursor(My_Line,My_Col);
 if (Scr_Width==Max_Width)
 {Opt_Force_Attributes(0);
  Out_S("\033K");
 } else
 {Opt_Force_Attributes(0);
  Col=My_Col;
  while (My_Col-Scr_Shift<=Lst_Col[My_Line+Top_Row]&&
   My_Col-Scr_Shift<=Scr_Width)
   Opt_Char(' ');
  My_Col=Col;
 }
#endif
}

#if defined(UNIX)
char * Environment_Name(Suffix)
char *Suffix;
{strcpy(Dsk_Rec,getenv("TERM"));
 strcat(Dsk_Rec,Suffix);
}
#endif

#if !defined(MPE)
char *Do_Mapping(char *S)
{static char Mapped_File[80];
 switch (Mapping)
 {case EDT_Mapping:
   strcpy(Mapped_File,S);
#if defined(UNIX)
   strcpy(strchr(Mapped_File,'.')+1,"edt");
#else
#if defined(ATARI)
   strcpy(index(Mapped_File,'.')+1,"EDT");
#else
   strcpy(strchr(Mapped_File,'.')+1,"EDT");
#endif
#endif
   break;
  case WPS_Mapping:
   strcpy(Mapped_File,S);
#if defined(UNIX)
   strcpy(strchr(Mapped_File,'.')+1,"wps");
#else
#if defined(ATARI)
   strcpy(index(Mapped_File,'.')+1,"WPS");
#else
   strcpy(strchr(Mapped_File,'.')+1,"WPS");
#endif
#endif
   break;
  case Other_Mapping:
   strcpy(Mapped_File,S);
#if defined(UNIX)
   strcpy(strchr(Mapped_File,'.')+1,Map_Ext);
#else
#if defined(ATARI)
   strcpy(index(Mapped_File,'.')+1,Map_Ext);
#else
   strcpy(strchr(Mapped_File,'.')+1,Map_Ext);
#endif
#endif
   break;
 }
 return Mapped_File;
}
#endif

/*      Returns the name of the file containing the key mappings
*/
char *Keyfile()
{
#if defined(WIN32)
 return Do_Mapping("FUNDEF32.EDT");
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(ATARI) | defined(VMS)
 return Do_Mapping("FUNDEF.EDT");
#endif
#if defined(UNIX)
 return Do_Mapping("fundef.edt");
#endif
#if defined(MPE)
 return "fundef.sed";
#endif
}

/*      Returns the name of the file containing the key mappings
*/
char *Mapfile()
{
#if defined(UNIX) | defined(VMS)
 char *S;
 switch (Term_Type)
 {case VT1XX:
   return Do_Mapping("vt100m.map");
  case VT2XX:
   return Do_Mapping("vt200m.map");
  case VT52:
   return Do_Mapping("vt52m.map");
  case VK100:
   return Do_Mapping("vk100m.map");
  case ANSI:
   return Do_Mapping("ansim.map");
  case VT3XX:
   return Do_Mapping("vt300m.map");
  case VT4XX:
   return Do_Mapping("vt400m.map");
  case Tcap:
   S=getenv("TERM");
   strcpy(Dsk_Rec,S);
   strcat(Dsk_Rec,"m.map");
   return Do_Mapping(Dsk_Rec);
#if defined(UNIX)
  case Defined:
   S=Defined_Terminal;
   strcpy(Dsk_Rec,S);
   strcat(Dsk_Rec,"m.map");
   return Do_Mapping(Dsk_Rec);
#endif
  case Default:
   return Do_Mapping("defm.map");
 }
#endif

#if defined(WIN32)
   return Do_Mapping("win32m.map");
#endif

#if defined(DOS) | defined(NT) 
#if !defined(Windows)
#if defined(PC100)
 return Do_Mapping("RBDOSM.MAP");
#else
 if (Keyb_Type==Keyb_PC)
  return Do_Mapping("PCDOSM.MAP"); else
 if (Keyb_Type==Keyb_250)
  return Do_Mapping("LKDOSM.MAP"); else
 if (Keyb_Type==Keyb_New)
  return Do_Mapping("NPDOSM.MAP"); else
 if (Keyb_Type==Keyb_Conv)
  return Do_Mapping("CVDOSM.MAP"); else
 if (Keyb_Type==Keyb_AT)
  return Do_Mapping("ATDOSM.MAP"); else
 if (Keyb_Type==Keyb_SLT)
  return Do_Mapping("SLTDOSM.MAP"); else
 if (Keyb_Type==Keyb_Special)
 {strcpy(Keyb_File,Keyb_Name);
  strcat(Keyb_File,"DOSM.MAP");
  return Do_Mapping(Keyb_File);
 } else
 switch (Term_Type)
 {case IBM_PC:
  case IBM_XT:
   return Do_Mapping("PCDOSM.MAP");
  case IBM_AT:
   return Do_Mapping("ATDOSM.MAP");
  case IBM_Conv:
   return Do_Mapping("CVDOSM.MAP");
  case PCXX:
   return Do_Mapping("LKDOSM.MAP");
  case TANDY_1400:
   return Do_Mapping("TCDOSM.MAP");
  case Default:
   return Do_Mapping("defdosm.map");
  default:
   return Do_Mapping("NPDOSM.MAP");
 }
#endif
#else
 if (Keyb_Type==Keyb_PC)
  return Do_Mapping("PCWINM.MAP"); else
 if (Keyb_Type==Keyb_250)
  return Do_Mapping("LKWINM.MAP"); else
 if (Keyb_Type==Keyb_New)
  return Do_Mapping("NPWINM.MAP"); else
 if (Keyb_Type==Keyb_Conv)
  return Do_Mapping("CVWINM.MAP"); else
 if (Keyb_Type==Keyb_AT)
  return Do_Mapping("ATWINM.MAP"); else
 if (Keyb_Type==Keyb_SLT)
  return Do_Mapping("SLTWINM.MAP"); else
 if (Keyb_Type==Keyb_Special)
 {strcpy(Keyb_File,Keyb_Name);
  strcat(Keyb_File,"WINM.MAP");
  return Do_Mapping(Keyb_File);
 } else
 switch (Term_Type)
 {case IBM_PC:
  case IBM_XT:
   return Do_Mapping("PCWINM.MAP");
  case IBM_AT:
   return Do_Mapping("ATWINM.MAP");
  case IBM_Conv:
   return Do_Mapping("CVWINM.MAP");
  case PCXX:
   return Do_Mapping("LKWINM.MAP");
  case TANDY_1400:
   return Do_Mapping("TCWINM.MAP");
  case Default:
   return Do_Mapping("defwinm.map");
  default:
   return Do_Mapping("NPWINM.MAP");
 }
#endif
#endif
#if defined(OS2)
 if (Keyb_Type==Keyb_PC)
  return Do_Mapping("PCOS2M.MAP"); else
 if (Keyb_Type==Keyb_250)
  return Do_Mapping("LKOS2M.MAP"); else
 if (Keyb_Type==Keyb_New)
  return Do_Mapping("NPOS2M.MAP"); else
 if (Keyb_Type==Keyb_Conv)
  return Do_Mapping("CVOS2M.MAP"); else
 if (Keyb_Type==Keyb_AT)
  return Do_Mapping("ATOS2M.MAP"); else
 if (Keyb_Type==Keyb_SLT)
  return Do_Mapping("SLTOS2M.MAP"); else
 if (Keyb_Type==Keyb_Special)
 {strcpy(Keyb_File,Keyb_Name);
  strcat(Keyb_File,"OS2M.MAP");
  return Do_Mapping(Keyb_File);
 } else
 switch (Term_Type)
 {case IBM_PC:
  case IBM_XT:
   return Do_Mapping("PCOS2M.MAP");
  case IBM_AT:
   return Do_Mapping("ATOS2M.MAP");
  case IBM_Conv:
   return Do_Mapping("CVOS2M.MAP");
  case PCXX:
   return Do_Mapping("LKOS2M.MAP");
  case TANDY_1400:
   return Do_Mapping("TCOS2M.MAP");
  case Default:
   return Do_Mapping("defos2m.map");
  default:
   return Do_Mapping("NPOS2M.MAP");
 }
#endif
#if defined(ATARI)
 return Do_Mapping("STTOSM.MAP");
#endif
#if defined(MPE)
 return "map.sed";
#endif
}

/*      Returns the name of the help file for the current terminal type
*/
char *Helpfile()
{
#if defined(UNIX) | defined(VMS)
 char *S;
 switch (Term_Type)
 {case VT1XX:
   return Do_Mapping("vt100h.hlp");
  case VT2XX:
   return Do_Mapping("vt200h.hlp");
  case VT52:
   return Do_Mapping("vt52h.hlp");
  case VK100:
   return Do_Mapping("vk100h.hlp");
  case ANSI:
   return Do_Mapping("ansih.hlp");
  case VT3XX:
   return Do_Mapping("vt300h.hlp");
  case VT4XX:
   return Do_Mapping("vt400h.hlp");
  case Tcap:
   S=getenv("TERM");
   strcpy(Dsk_Rec,S);
   strcat(Dsk_Rec,"h.hlp");
   return Do_Mapping(Dsk_Rec);
#if defined(UNIX)
  case Defined:
   S=Defined_Terminal;
   strcpy(Dsk_Rec,S);
   strcat(Dsk_Rec,"h.hlp");
   return Do_Mapping(Dsk_Rec);
#endif
  case Default:
   return Do_Mapping("defh.hlp");
 }
#endif
#if defined(WIN32)
   return Do_Mapping("win32h.hlp");
#endif
#if defined(DOS) | defined(NT) 
#if !defined(Windows)
#if defined(PC100)
  return Do_Mapping("RBDOSH.HLP");
#else
 if (Keyb_Type==Keyb_PC)
  return Do_Mapping("PCDOSH.HLP"); else
 if (Keyb_Type==Keyb_250)
  return Do_Mapping("LKDOSH.HLP"); else
 if (Keyb_Type==Keyb_New)
  return Do_Mapping("NPDOSH.HLP"); else
 if (Keyb_Type==Keyb_Conv)
  return Do_Mapping("CVDOSH.HLP"); else
 if (Keyb_Type==Keyb_AT)
  return Do_Mapping("ATDOSH.HLP"); else
 if (Keyb_Type==Keyb_SLT)
  return Do_Mapping("SLTDOSH.HLP"); else
 if (Keyb_Type==Keyb_Special)
 {strcpy(Keyb_File,Keyb_Name);
  strcat(Keyb_File,"DOSH.MAP");
  return Do_Mapping(Keyb_File);
 } else
 switch (Term_Type)
 {case IBM_PC:
  case IBM_XT:
   return Do_Mapping("PCDOSH.HLP");
  case IBM_AT:
   return Do_Mapping("ATDOSH.HLP");
  case IBM_Conv:
   return Do_Mapping("CVDOSH.HLP");
  case PCXX:
   return Do_Mapping("ATDOSH.HLP");
  case TANDY_1400:
   return Do_Mapping("TCDOSH.HLP");
  case Default:
   return Do_Mapping("defdosh.hlp");
  default:
   return Do_Mapping("NPDOSH.HLP");
 }
#endif
#else
 if (Keyb_Type==Keyb_PC)
  return Do_Mapping("PCWINH.HLP"); else
 if (Keyb_Type==Keyb_250)
  return Do_Mapping("LKWINH.HLP"); else
 if (Keyb_Type==Keyb_New)
  return Do_Mapping("NPWINH.HLP"); else
 if (Keyb_Type==Keyb_Conv)
  return Do_Mapping("CVWINH.HLP"); else
 if (Keyb_Type==Keyb_AT)
  return Do_Mapping("ATWINH.HLP"); else
 if (Keyb_Type==Keyb_SLT)
  return Do_Mapping("SLTWINH.HLP"); else
 if (Keyb_Type==Keyb_Special)
 {strcpy(Keyb_File,Keyb_Name);
  strcat(Keyb_File,"WINH.MAP");
  return Do_Mapping(Keyb_File);
 } else
 switch (Term_Type)
 {case IBM_PC:
  case IBM_XT:
   return Do_Mapping("PCWINH.HLP");
  case IBM_AT:
   return Do_Mapping("ATWINH.HLP");
  case IBM_Conv:
   return Do_Mapping("CVWINH.HLP");
  case PCXX:
   return Do_Mapping("LKWINH.HLP");
  case TANDY_1400:
   return Do_Mapping("TCWINH.HLP");
  case Default:
   return Do_Mapping("defwinh.hlp");
  default:
   return Do_Mapping("NPWINH.HLP");
 }
#endif
#endif
#if defined(OS2)
 if (Keyb_Type==Keyb_PC)
  return Do_Mapping("PCOS2H.HLP"); else
 if (Keyb_Type==Keyb_250)
  return Do_Mapping("LKOS2H.HLP"); else
 if (Keyb_Type==Keyb_New)
  return Do_Mapping("NPOS2H.HLP"); else
 if (Keyb_Type==Keyb_Conv)
  return Do_Mapping("CVOS2H.HLP"); else
 if (Keyb_Type==Keyb_AT)
  return Do_Mapping("ATOS2H.HLP"); else
 if (Keyb_Type==Keyb_SLT)
  return Do_Mapping("SLTOS2H.HLP"); else
 if (Keyb_Type==Keyb_Special)
 {strcpy(Keyb_File,Keyb_Name);
  strcat(Keyb_File,"OS2H.MAP");
  return Do_Mapping(Keyb_File);
 } else
 switch (Term_Type)
 {case IBM_PC:
  case IBM_XT:
   return Do_Mapping("PCOS2H.HLP");
  case IBM_AT:
   return Do_Mapping("ATOS2H.HLP");
  case IBM_Conv:
   return Do_Mapping("CVOS2H.HLP");
  case PCXX:
   return Do_Mapping("ATOS2H.HLP");
  case TANDY_1400:
   return Do_Mapping("TCOS2H.HLP");
  case Default:
   return Do_Mapping("defos2h.hlp");
  default:
   return Do_Mapping("NPOS2H.HLP");
 }
#endif
#if defined(ATARI)
 return Do_Mapping("STTOSH.HLP");
#endif
#if defined(MPE)
 return "help.sed";
#endif
}

/*      Get a character from the console.
*/
void Fast Conin()
{
#if defined(OS2)
 struct _KBDKEYINFO Key_Data;
 struct _NOPTRRECT No_Pointer;
#endif
#if defined(DOS) & !(defined(Windows) | defined(TV))
 int M1;
#endif
#if defined(ATARI)
 long Con_Input;
#endif
#if defined(VMS)
 if (Got_In==This_In)
 {Upd_Header();
  if (Force_Cursor)
  {Force_Cursor=0;
   Set_Cursor(My_Line,(int)My_Col);
  } else
  if (My_Col-Scr_Shift!=His_Col||My_Line!=His_Line)
   Opt_Move_Cursor(My_Line,My_Col);
  if (Cur_Attributes!=His_Attributes)
   Opt_Force_Attributes(Cur_Attributes);
  Out_Zap();
  do
   Got_In=Get_Input();
  while (Got_In==0);
  This_In=0;
 }
 Character=TTY_Buffer[This_In++]&255;
 Do_Echo=0;
#else
 if (My_Col-Scr_Shift!=His_Col||My_Line!=His_Line)
  Opt_Move_Cursor(My_Line,My_Col);
 Out_Zap();
#endif
#if defined(UNIX)
#if !defined(TERMCAP)
 refresh();
#endif
#if defined(UNIX)
 if (Same_Character)
  {
  Same_Character=0;
  Character=Saved_Character;
  }
 else
  {
#endif
#if defined(TERMCAP)
 Character=UGetrm();
#else
 Character=getch();
#endif
#if defined(UNIX)
  Saved_Character=Character;
  }
#endif
 Scancode=255;
#endif
#if defined(WIN32)
{
   INPUT_RECORD in;
   BOOL ok;
   DWORD cInRecords=1;
   DWORD lpcRead;
   int skipping=1;
   int key_up_event;
   int virtkey;
   long control;
   /*static */
   int num_lock_went_up=0;
   int got_focus=0;
static int cnt=0;

   while (skipping)
   {




   looping:
      do { ok = ReadConsoleInput(  win32KeybHnd, &in, cInRecords, &lpcRead );
         } while ( in.EventType != KEY_EVENT && in.EventType != FOCUS_EVENT);

      if (in.EventType == FOCUS_EVENT)
      {
         got_focus=in.Event.FocusEvent.bSetFocus;
         continue;
      }
      Character = in.Event.KeyEvent.uChar.AsciiChar;
      Scancode  = in.Event.KeyEvent.wVirtualScanCode;
      key_up_event = !in.Event.KeyEvent.bKeyDown;
      virtkey = in.Event.KeyEvent.wVirtualKeyCode;
      control = in.Event.KeyEvent.dwControlKeyState;

      if ( Scancode == 69 && got_focus && key_up_event)
      {
         got_focus = 0;
         continue;
      }













      if ( key_up_event) continue; /* skip up events  */
      /* if num lock goes up before is goes down then it's a bogus key stroke */
      if ( Scancode == 69 && num_lock_went_up)
      {  num_lock_went_up = 0;




         continue;
      }
      /* skip various extra key strokes */
      switch (virtkey)
      {/*case 0: case 17: case 18: case 12: case 93: case 16: case 20:*/
         case 0: case 17: case 18:            case 93: case 16: case 20:




         goto looping;
      }
      /* Num Lock fixup, if keypad then make it look like keypad no matter what */
      if (Character==0 && (control&NUMLOCK_ON)==0 && (control&ENHANCED_KEY)==0)
      {

         if (Scancode==82) Character='0';
         if (Scancode==79) Character='1';
         if (Scancode==80) Character='2';
         if (Scancode==81) Character='3';
         if (Scancode==75) Character='4';
         if (Scancode==76) Character='5';
         if (Scancode==83) Character='.';
         if (Scancode==77) Character='6';
         if (Scancode==71) Character='7';
         if (Scancode==72) Character='8';
         if (Scancode==73) Character='9';
         if (Scancode==55) Character='*'; /* PF3 */
         if (Scancode==74) Character='-'; /* PF4 */
         if (Scancode==78) Character='+';


      }

      /* Num Lock fixup, if keypad then make it look like keypad no matter what */
      if (Character==0 && (control&SHIFT_PRESSED) && (59 <= Scancode && Scancode <= 88))
      {  /* if (Scancode != 63) fixed, needed help file name from helpfile() *//* bug, ^F5=help crashes us for now */
         Scancode+=25;
      }





      if (control&ENHANCED_KEY)
      {

if (Character=='/' || Character==13)
{         Character=Scancode;
          Scancode=0;
}

      }
      skipping = 0;






   }











}
#endif
#if defined(DOS) | defined(NT) 
#if !(defined(Windows) | defined(TV))
 if (Gotmouse&&Mouse_OK)
 {M1=1;
  Mouse(&M1,NULL,NULL,NULL);
  M1=1;
  Mouse(&M1,NULL,NULL,NULL);
 }
#if defined(PC100)
 while ((Character=bdos(0x07,0,0)&255)==0);
 Scancode=255;
#else
#if defined(DESQVIEW)
 if (DV)
 {Save_Row=Save_Col=0;
  while (1)
  {if (ptr_sizeof(Ptr)!=0)
   {ptr_read(Ptr,(char far *)&Ptr_Buf,(int far *)&Buflen);
    /*Diag("Mouse:Row=%d,Col=%d,Button%d,Move=%d",
     Ptr_Buf->Row,Ptr_Buf->Column,Ptr_Buf->Status&3,Ptr_Buf->Status&64);*/
    if (!(Ptr_Buf->Status&64))
    {if ((Ptr_Buf->Status&3)==1)
     {Save_Row=Ptr_Buf->Row;
      Save_Col=Ptr_Buf->Column;
      Mouse_Call(Ptr_Buf->Row,Ptr_Buf->Column,
       (Ptr_Buf->Status&3)==1,(Ptr_Buf->Status&3)==2,0);
     }
    } else
    if ((Save_Row!=Ptr_Buf->Row||Save_Col!=Ptr_Buf->Column)&&
     (Ptr_Buf->Status&3)==1)
    {Mouse_Call(Save_Row,Save_Col,1,0,1);
     Mouse_Call(Ptr_Buf->Row,Ptr_Buf->Column,1,0,1);
     Mouse_Call(Ptr_Buf->Row,Ptr_Buf->Column,0,0,0);
    }
   }
   if (Firmware&1?ANYEXT():ANYCHR())
    break;
   api_pause();
  }
 }
#endif
 if (Firmware&1)
  Character=GETEXT((int far *)&Scancode); else
  Character=GETSCAN((int far *)&Scancode);
#endif
 if (Gotmouse&&Mouse_OK)
 {M1=2;
  Mouse(&M1,NULL,NULL,NULL);
 }
 /*Diag("Conin:Character=%d,Scancode=%d",Character,Scancode);*/
#else
#if defined(TV)
 TV_Char();
#else
 WinChar();
#endif
 if (Not_End==0)
  return;
 /*Diag("Conin:Scancode=%d,Character=%d",Scancode,Character);*/
#endif
#endif
#if defined(ATARI)
#if defined(GEM)
 Con_Input=GEM_char(Mouse_OK);
 if (Not_End==0)
  return;
 Character=Con_Input&0xFF;
 Scancode=(Con_Input>>8)&0xFF;
#else
 if (Key_Count||Cconis()!=0)
 {while (Cconis()!=0)
  {if (Key_Count<20)
    Key_Buffer[Key_Count++]=Crawcin(); else
    Crawcin();
  }
  Con_Input=Key_Buffer[0];
  Key_Count--;
  {register int I;
   for (I=0;I<Key_Count;I++)
    Key_Buffer[I]=Key_Buffer[I+1];
  }
 } else
  Con_Input=Crawcin();
 Character=Con_Input&0xFF;
 Scancode=(Con_Input>>16)&0xFF;
#endif
#endif
#if defined(MPE)
 {char data[4];
  FREAD(mpe_stdin, (unsigned short *)data,-1);
  Character=data[0];
  if (Character==':')
  {int dummy;
   FSETMODE(mpe_stdin,0);
   FCLOSE(mpe_stdin,0,0);
   mpe_stdin=FOPEN("$STDIN");
   FCONTROL(mpe_stdin,27,&dummy);
   FCONTROL(mpe_stdin,13,&dummy);
   FSETMODE(mpe_stdin,4);
  }
 }
#endif
#if defined(OS2)
 if (Gotmouse)
 {MouDrawPtr(Mouse_Handle);
  DosSemClear((HSEM)&Mouse_Sem);
 }
 do
 {KbdCharIn((PKBDKEYINFO)&Key_Data,0,0);
  Character=Key_Data.chChar;
  Scancode=Key_Data.chScan;
 } while ((Key_Data.fbStatus&&0x40)==0);
 if (Gotmouse)
 {DosSemRequest((HSEM)&Mouse_Sem,-1L);
  No_Pointer.row=0;
  No_Pointer.col=0;
  No_Pointer.cRow=Scr_Length-1;
  No_Pointer.cCol=Scr_Width-1;
  MouRemovePtr((PNOPTRRECT)&No_Pointer,Mouse_Handle);
 }
 if (Scancode==0x45)
 {Kbd_Status.cb=10;
  KbdGetStatus((PKBDINFO)&Kbd_Status,0);
  Kbd_Status.fsMask=1<<4;
  Kbd_Status.fsState|=(1<<5);
  KbdSetStatus((PKBDINFO)&Kbd_Status,0);
 } else
 if (Scancode==0x46)
 {Kbd_Status.cb=10;
  KbdGetStatus((PKBDINFO)&Kbd_Status,0);
  Kbd_Status.fsMask=1<<4;
  Kbd_Status.fsState&=~(1<<4);
  KbdSetStatus((PKBDINFO)&Kbd_Status,0);
 }
#endif
 /*Diag("Conin:Scancode=%x,Character=%x",Scancode,Character);*/
}

/*      Get a key from the keyboard.
*/
void Fast Getkbd()
{int Done;
 Function=0;
 Done=0;
 while (!Done)
 {Getkey();
  if (Check_Comb(0,0)<0)
   Key_Error(); else
  {Map_Index= -1;
   {register int I;
    for (I=0;;I++)
    {if (I==Map_Defs)
     {Done=1;
      break;
     }
     if (Map_Tab[I].Flags&Gold_Code?Gold:!Gold)
     {if (Map_Tab[I].Char==Key_Number&&(Map_Tab[I].Flags&Key_Code))
      {if (Map_Tab[I].F_Code==0)
        Gold=1; else
       {if ((Function=Map_Tab[I].F_Code)<0)
         Function=0;
        Map_Index=I;
        Done=1;
       }
       break;
      } else
      if (Map_Tab[I].Scancode==Scancode&&Map_Tab[I].Char==Character)
      {if (Enhanced&&(Map_Tab[I].Flags&Ext_Code))
       {if (Map_Tab[I].F_Code==0)
         Gold=1; else
        {if ((Function=Map_Tab[I].F_Code)<0)
          Function=0;
         Map_Index=I;
         Done=1;
        }
        break;
       } else
       if (!Enhanced&&!(Map_Tab[I].Flags&Ext_Code))
       {if (Map_Tab[I].F_Code==0)
         Gold=1; else
        {if ((Function=Map_Tab[I].F_Code)<0)
          Function=0;
         Map_Index=I;
         Done=1;
        }
        break;
       }
      } else
      if (Map_Tab[I].Char==Character&&(Map_Tab[I].Flags&Char_Code))
      {if (Map_Tab[I].F_Code==0)
        Gold=1; else
       {if ((Function=Map_Tab[I].F_Code)<0)
         Function=0;
        Map_Index=I;
        Done=1;
       }
       break;
      }
     }
    }
   }
  }
 }
 return;
}

void Fast Getkey()
{Conin();
 if (Scancode==0)
 {Enhanced=1;
  Scancode=Character;
 } else
  Enhanced=0;
 /*Diag("Getkey:Scancode=%d,Character=%d",Scancode,Character);*/
 if (Scancode==Character)
 {register int I;
  for (I=0;I<Scan_Defs;I++)
  {if (Scan_Tab[I].Scancode==Scancode)
   {if (Enhanced&&(Scan_Tab[I].Flags&Ext_Code))
    {Character=Scan_Tab[I].Char;
     break;
    } else
    if (!Enhanced&&!(Scan_Tab[I].Flags&Ext_Code))
    {Character=Scan_Tab[I].Char;
     break;
    }
   }
  }
 }
 Key_Number= -1;
 {register int I;
  for (I=0;I<Key_Defs;I++)
  {/*Diag("Getkey:Scan_Tab.Scancode=%d,Scantab.Flags.Ext_Code=%d",
    Scan_Tab[I].Scancode,Scan_Tab[I].Flags&Ext_Code);*/
   if (Key_Tab[I].Scancode==Scancode&&Key_Tab[I].Char==Character)
   {if (Enhanced&&(Key_Tab[I].Flags&Ext_Code))
    {Key_Number=Key_Tab[I].Key;
     break;
    } else
    if (!Enhanced&&!(Key_Tab[I].Flags&Ext_Code))
    {Key_Number=Key_Tab[I].Key;
     break;
    }
   } else
   if (Key_Tab[I].Char==Character&&(Key_Tab[I].Flags&Char_Code))
   {Key_Number=Key_Tab[I].Key;
    break;
   }
  }
 }
}

int Fast Next_Comb(int Index,int Comb_Level)
{int Sav_Index, Sav_Line, Sav_Attributes, C;
 long Sav_Col, Sav_Shift;
 Sav_Index=Index;
#if !(((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM)) | defined(UNIX) | defined(MPE))
 if (!Input_Test()&&!In_Prompt)
#else
 if (!In_Prompt) /* DOS code has ";" which looks wrong (mdj) */ 
#endif
 {Set_Physical();
  Sav_Set(&Sav_Line,&Sav_Col,(char *)&Sav_Attributes,Scr_Length,(long)1,A_Bold);
  Sav_Shift=Scr_Shift;
  Scr_Shift=0;
  for (;Index<Comb_Defs;Index++)
  {if (Comb_Tab[Index].Level<=Comb_Level)
    break;
   if (Comb_Tab[Index].Level==Comb_Level+1)
   {if (Comb_Tab[Index].Index!=0&&Comb_Tab[Index].Flags&Prompt_Code)
    {if (My_Col!=1)
      Opt_Char('|');
     B_Goto(&Pad,(long) Comb_Tab[Index].Index);
     while ((C=B_Get(&Pad))!=0)
      Opt_Char((char)C);
    }
   }
  }
  if (My_Col!=1)
  {Opt_Erase_Line();
   Err_Display=1;
  }
  Set_Save(Sav_Line,Sav_Col,(char)Sav_Attributes);
  Set_Logical();
  Scr_Shift=Sav_Shift;
 }
 Index=Sav_Index;
 Getkey();
 Check_Comb(Index,Comb_Level);
 return 0;
}

int Fast Check_Comb(int Index,int Comb_Level)
{Comb_Level++;
 for (;Index<Comb_Defs;Index++)
 {if (Comb_Tab[Index].Level<Comb_Level)
   return -1;
  if (Comb_Tab[Index].Level==Comb_Level)
  {
   /*Diag("C=%d,S=%d,C[I].C=%d,C[I].S=%d,C[I].F=%d",
    Character,Scancode,Comb_Tab[Index].Char,Comb_Tab[Index].Scancode,
    Comb_Tab[Index].Flags);*/
   if (Comb_Tab[Index].Scancode==Scancode&&Comb_Tab[Index].Char==Character)
   {if (Enhanced&&(Map_Tab[Index].Flags&Ext_Code))
    {if (Comb_Tab[Index].Flags&Final_Code)
     {Key_Number=Comb_Tab[Index].Key;
      Character=0;
      return Check_Comb(0,0);;
     } else
      return Next_Comb(Index+1,Comb_Level);
    } else
    if (!Enhanced&&!(Map_Tab[Index].Flags&Ext_Code))
    {if (Comb_Tab[Index].Flags&Final_Code)
     {Key_Number=Comb_Tab[Index].Key;
      Character=0;
      return Check_Comb(0,0);
     } else
      return Next_Comb(Index+1,Comb_Level);
    }
   } else
   if (Comb_Tab[Index].Char==Character&&(Comb_Tab[Index].Flags&Char_Code))
   {if (Comb_Tab[Index].Flags&Final_Code)
    {Key_Number=Comb_Tab[Index].Key;
     Character=0;
     return Check_Comb(0,0);
    } else
     return Next_Comb(Index+1,Comb_Level);
   } else
   if (Comb_Tab[Index].Char==Key_Number&&(Comb_Tab[Index].Flags&Key_Code))
   {if (Comb_Tab[Index].Flags&Final_Code)
    {Key_Number=Comb_Tab[Index].Key;
     Character=0;
     return Check_Comb(0,0);
    } else
     return Next_Comb(Index+1,Comb_Level);
   }
  }
 }
 return 0;
}

/*      Set the terminal characters to the settings that SEDT requires.
*/
void Fast Setup_Term()
{
#if defined(VMS) | defined(UNIX)
 Gotmouse=0;
#endif
#if defined(UNIX)
#if defined(TERMCAP)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
   Out_S("\033=");
#if defined(UNIX)
  {if (Use_Mouse)	/* 290a */
    Enable_Mouse();
    Out_S("\033[1;3'{");	/* Select Button Up and Down Events*/
    Gotmouse=1;
  }
#endif
  case VT1XX:
  case VK100:
  {Out_S("\033[?6l");
   /*Out_S("\033[?7l");*/
   Out_S("\033[?25h");
   Out_S("\033=");
   break;       /* 290a */
  }
  case ANSI:
  {/*Out_S("\033[?7l");*/
   Out_S("\033=");
  }
 }
 Scl_Top = 0;
 Set_Scroll (1, Scr_Length);
#else
 initscr();
 nonl();
 cbreak();
 noecho();
 Max_Length=Scr_Length=stdscr->_maxy;
 Max_Width=Scr_Width=stdscr->_maxx;
#endif
#endif
#if defined(VMS)
 Gotmouse=0;
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case ANSI:
  {if (Use_Mouse)	/* 290a */
    Enable_Mouse();
    Out_S("\033[1;3'{");	/* Select Button Up and Down Events*/
    Gotmouse=1;
  }
  case VT1XX:
  case VK100:
  {Out_S("\033[?6l");
   /*Out_S("\033[?7l");*/
   Out_S("\033[?25h");
   Out_S("\033=");
  }
 }
 Scl_Top=0;
 Set_Scroll(1,Scr_Length);
#endif
#if defined(DOS) | defined(NT)
#if !defined(Windows)
 int Keyb_Resp, I;
#if !defined(TV)
 int P1, P2, P3, P4;
 if (Use_Mouse)
 {
#if (defined(DOS) & !defined(PC100)) | defined(NT)
#if defined(DESQVIEW)
  if (!DV)
#endif
   if ((bdos(0x30,0,0)&0xFF)>2)
    Mouse(&Gotmouse,NULL,NULL,NULL);
#endif
  if (Gotmouse)
   Set_Mouse();
 } else
  Gotmouse=0;
#endif
#if defined(PC100)
 Out_S("\033[?6l");
 /*Out_S("\033[?7l");*/
 Out_S("\033[?25h");
 Out_S("\033=");
 Scl_Top=0;
 Set_Scroll(1,Scr_Length);
#else
#if !defined(TV)
 if (EGA43)
 {Set43();
  if (Gotmouse)
  {P1=7;
   P3=0;
   P4=639;
   Mouse(&P1,&P2,&P3,&P4);
   P1=8;
   P3=0;
   P4=349;
   Mouse(&P1,&P2,&P3,&P4);
  }
 } else
 if (New_Mode)
  Old_Mode=Set_Mode(New_Mode);
#endif
 if (Keyb_Type==Keyb_250&&Term_Type!=IBM_XT&&Term_Type!=IBM_PC&&
  Term_Type!=IBM_Conv&&Term_Type!=TANDY_1400)
 {Keyb_Resp=SENDCMD(0xAB);
  Keyb_Resp=GETSCAN((int far *)&I);
  while (Keyb_Resp!=0x43&&Keyb_Resp!=0x41)
  {Keyb_Resp=GETSCAN((int far *)&I);
  }
  if (Keyb_Resp==0x41&&ANYCHR())
   Keyb_Resp=GETSCAN((int far *)&I);
  if (!(DEC_Mode=Keyb_Resp==0x41))
   Keyb_Resp=SENDCMD(0xAC);
 }
 if (Set_Num_Lock==1)
  SETNUMLOCK(); else
 if (Set_Num_Lock==0)
  CLRNUMLOCK();
#endif
#else
 if (Set_Num_Lock==1)
  Win_Set_Num_Lock(); else
 if (Set_Num_Lock==0)
  Win_Clear_Num_Lock();
#endif
#endif
#if defined(OS2)
 if (EGA43)
 {Mode_Data.row=Scr_Length=43;
  VioSetMode((PVIOMODEINFO)&Mode_Data,0);
  Cursor_Data.yStart=7;
  Cursor_Data.cEnd=7;
  VioSetCurType((PVIOCURSORINFO)&Cursor_Data,0);
 }
 Kbd_Status.cb=10;
 KbdGetStatus((PKBDINFO)&Kbd_Status,0);
 Sav_Shift_State=Kbd_Status.fsState;
 Kbd_Status.fsMask=1<<4;
 if (Set_Num_Lock==1)
  Kbd_Status.fsState|=(1<<5); else
 if (Set_Num_Lock==0)
  Kbd_Status.fsState&=~(1<<5);
 Kbd_Status.fsState&=~(1<<4);
 KbdSetStatus((PKBDINFO)&Kbd_Status,0);
#endif
#if defined(ATARI) & ! defined(GEM)
 Cursconf(0,0);
 Out_S("\033w");
#endif
#if defined(MPE)
 int dummy;
 mpe_stdin=FOPEN("$STDIN");
 FCONTROL(mpe_stdin,27,&dummy);
 FCONTROL(mpe_stdin,13,&dummy);
 FSETMODE(mpe_stdin,4);
 mpe_stdout=FOPEN("$STDLIST");
 FCONTROL(mpe_stdout,27,&dummy);
 Out_S("\033&s1A");
#endif
 if (Init_T_String[0]!=0)
 {Out_S(Init_T_String);
  Out_Zap();
 }
}

/*      Reset the terminal characteristics to the state they were in when SEDT
        was entered.
*/
void Fast Reset_Term()
{
#if defined(UNIX)
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#if defined(TERMCAP)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
   Out_S("\033>");
 }
 if (His_Attributes!=0)
  Opt_Force_Attributes(0);
 Set_Scroll (1, Max_Length);
 Out_Zap ();
#else
 Out_Zap();
 clear();
 refresh();
 endwin();
#endif
#endif
#if defined(VMS)
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case ANSI:
  case VT1XX:
  case VK100:
   if (!VMS_Application)
    Out_S("\033>");
 }
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
 Opt_Force_Attributes(0);
 Set_Scroll(1,Max_Length);	/* 290a */
 Out_Zap();
#endif
#if defined(WIN32)
 WIN32fini();
#endif
#if defined(DOS) | defined(NT)
#if !defined(Windows)
#if !defined(TV)
 int P1, P2, P3, P4;
 if (Use_Mouse)
 {if (Gotmouse)
   Clear_Mouse();
 }
#endif
#if defined(PC100)
 Opt_Force_Attributes(0);
 Out_S("\033>");
 Set_Scroll(1,Max_Length);
 Out_Zap();
#else
#if !defined(TV)
 if (EGA43)
 {Clr43();
  if (Gotmouse)
  {P1=7;
   P3=0;
   P4=639;
   Mouse(&P1,&P2,&P3,&P4);
   P1=8;
   P3=0;
   P4=199;
   Mouse(&P1,&P2,&P3,&P4);
  }
 } else
 if (Old_Mode)
  Set_Mode(Old_Mode);
#endif
 if (Keyb_Type==Keyb_250&&Term_Type!=IBM_XT&&Term_Type!=IBM_PC&&
  Term_Type!=IBM_Conv&&Term_Type!=TANDY_1400)
 {if (!DEC_Mode)
   SENDCMD(0xAD);
 }
 if (Set_Num_Lock>=0)
  RESNUMLOCK();
#endif
#else
 if (Set_Num_Lock>=0)
  Win_Reset_Num_Lock();
#endif
#endif
#if defined(OS2)
 char Cell[2];
 if (EGA43)
 {Mode_Data.row=Old_Length;
  VioSetMode((PVIOMODEINFO)&Mode_Data,0);
  if (Old_Length==25)
  {Cursor_Data.yStart=12;
   Cursor_Data.cEnd=13;
  } else
  {Cursor_Data.yStart=7;
   Cursor_Data.cEnd=7;
  }
  VioSetCurType((PVIOCURSORINFO)&Cursor_Data,0);
 }
 Kbd_Status.cb=20;
 KbdGetStatus((PKBDINFO)&Kbd_Status,0);
 Kbd_Status.fsMask=1<<4;
 Kbd_Status.fsState&=~((1<<4)+(1<<5));
 Kbd_Status.fsState|=Sav_Shift_State&((1<<4)+(1<<5));
 KbdSetStatus((PKBDINFO)&Kbd_Status,0);
#endif
#if defined(ATARI) & ! defined(GEM)
 Opt_Force_Attributes(0);
 Cursconf(1,0);
 Out_Zap();
#endif
#if defined(MPE)
 int dummy;
 Opt_Force_Attributes(0);
 Out_S("\033&s0A");
 Clr_Screen();
 Out_Zap();
 FCONTROL(mpe_stdin,26,&dummy);
 FCONTROL(mpe_stdin,12,&dummy);
 FSETMODE(mpe_stdin,0);
 FCLOSE(mpe_stdin,0,0);
 FCLOSE(mpe_stdout,0,0);
#endif
 if (Reset_T_String[0]!=0)
 {Out_S(Reset_T_String);
  Out_Zap();
 }
}

/*      Make the screen 132 characters wide if possible
*/
void Fast Wide_Screen()
{
#if (defined(DOS) & !(defined(Windows) | defined(TV)) & defined(PC100)) | defined(VMS) | defined(UNIX)
 Phys_Width=Max_Width=132;
 Sav_Window(Cur_Buffer);
 {register int I;
  for (I=Buffers-1;I>=0;I--)
  {Out_Zap();
   Load_Window(Cur_Buffer=Buffer_Stack[I]);
   if (Scr_Width==80)
    Scr_Width=132;
   Sav_Window(Cur_Buffer);
  }
 }
 Out_S("\033[?3h");
#endif
}

/*      Make the screen 80 characters wide if 132 character displays are
        possible
*/
void Fast Narrow_Screen()
{
#if (defined(DOS) & !(defined(Windows) | defined(TV)) & defined(PC100)) | defined(VMS) | defined(UNIX)
 Phys_Width=Max_Width=80;
 Sav_Window(Cur_Buffer);
 {register int I;
  for (I=Buffers-1;I>=0;I--)
  {Out_Zap();
   Load_Window(Cur_Buffer=Buffer_Stack[I]);
   if (Left_Col>79)
    Left_Col=79;
   if (Left_Col+Scr_Width>80)
    Scr_Width=80-Left_Col;
   Sav_Window(Cur_Buffer);
  }
 }
 Out_S("\033[?3l");
#endif
}

void Fast Term_Error()
{Error(2,Unrecognized_Terminal_Txt);
}

void Fast Key_Error()
{Error(2,Illegal_Keystroke_Txt);
 Err_Flag=0;
}

void Fast Esc_Error()
{Error(2,Undefined_Escape_Txt);
}

#if defined(DOS) | defined(NT)
#if !(defined(Windows) | defined(TV))
void Fast Mouse(int *P1,int *P2,int *P3,int *P4)
{union REGS Regs;
 if (P1!=NULL)
  Regs.x.ax=*P1;
 if (P2!=NULL)
  Regs.x.bx=*P2;
 if (P3!=NULL)
  Regs.x.cx=*P3;
 if (P4!=NULL)
  Regs.x.dx=*P4;
 int86(51,&Regs,&Regs);
 if (P1!=NULL)
  *P1=Regs.x.ax;
 if (P2!=NULL)
  *P2=Regs.x.bx;
 if (P3!=NULL)
  *P3=Regs.x.cx;
 if (P4!=NULL)
  *P4=Regs.x.dx;
}
#endif
#endif
