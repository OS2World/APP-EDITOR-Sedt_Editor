/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"
#include <errno.h>

#if defined(WIN32)
#include <process.h>
#endif

#if defined(ATARI) & defined(GEM)
extern int TC_Height, TC_Width, T_Baseline, Buf_Points, wi_handle[];
#endif

/*      Interactive modification of the ruler line.
*/
void Fast Ruler()
{
 int Sav_Line;
 long Sav_Col;
 FILE *F;
 char S1[16],S2[64];
 Update_On();
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#endif
 if (!Show_Ruler)
  S_Ruler();
 Sav_Set(&Sav_Line,&Sav_Col,NULL,2,(long)1,(char)Cur_Attributes);
#if defined(VMS) | defined(UNIX)
 Show_Column();
#endif
 while (1)
 {if (!Err_Flag)
   Err_Display=0;
  if (My_Col-Scr_Shift<1)
  {while (My_Col-Scr_Shift<1)
    Scr_Shift-=Shift_Screen?Shift_Screen:Scr_Width/2;
   Hdr_Changes=1;
   Fill_Screen(Rel_First,Rel_Last);
   S_Ruler();
  } else
  if (My_Col-Scr_Shift>=Scr_Width)
  {while (My_Col-Scr_Shift>=Scr_Width)
    Scr_Shift+=Shift_Screen?Shift_Screen:Scr_Width/2;
   Hdr_Changes=1;
   Fill_Screen(Rel_First,Rel_Last);
   S_Ruler();
  } else
  if (My_Col-Scr_Shift<1&&Scr_Shift>0)
  {while (My_Col-Scr_Shift<1&&Scr_Shift>0)
    Scr_Shift-=Shift_Screen?Shift_Screen:Scr_Width/2;
   Hdr_Changes=1;
   Fill_Screen(Rel_First,Rel_Last);
   S_Ruler();
  }
  Get16();
  if (Key_Number==30)
    My_Col++; else
  if (Key_Number==28)
  {if (My_Col>1)
    My_Col--;
  } else
  if (Character==24)
   My_Col=1; else
  if (Key_Number==48)
  {Opt_Cursor(Sav_Line,Sav_Col);
#if defined(VMS) | defined(UNIX)
   if (Gotmouse)	/* 290a */
    Enable_Mouse();
#endif
   Err_Display=1;
   if (!Show_Ruler)
    Fill_Screen(Rel_First,Rel_First);
   return;
  } else
  if (Gold)
  {if (isdigit(Character))
#if defined(MPE)
   {strcpy(S1,"rulern.sed");
#else
   {strcpy(S1,"rulern.txt");
#endif
    S1[5]=Character;
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(OS2) | defined(ATARI) | defined(WIN32)
    if ((Find_Env(S1,S2,"SEDTP")==NULL||(F=fopen(S2,"w"))==NULL)&&
     (Find_Env(S1,S2,"SEDT")==NULL||(F=fopen(S2,"w"))==NULL)&&
#if defined(DOS) | defined(NT) | defined(WIN32)
     (Find_From(S1,S2)==NULL||(F=fopen(S2,"w"))==NULL)&&
#endif
     (Find_Path(S1,S2)==NULL||(F=fopen(S2,"w"))==NULL)&&
     (F=fopen(S1,"w"))==NULL)
#endif
#if defined(VMS) 
    if ((F=fopen(Find_Dir(S1,S2),"w","rfm=var"))==NULL&&
     (F=fopen(Find_Lib(S1,S2),"w","rfm=var"))==NULL&&
     (F=fopen(S1,"w","rfm=var"))==NULL)
#endif
#if defined(MPE)
   if ((F=fopen(S1,"w"))==NULL)
#endif
     Error(2,No_Ruler_Txt,S1); else
    {fprintf(F,"L%ld ",Left_Margin);
     fprintf(F,Auto_Margin?"A%ld ":"W%ld ",Word_Wrap);
     fprintf(F,Justify?"J%ld ":"R%ld ",Right_Margin);
     fprintf(F,"I%ld ",Indent);
     {register int I;
      for (I=1;I<=512;I++)
       if (Tabs[I])
        fprintf(F,"T%d ",I);
     }
     fprintf(F,"\n");
     if (fclose(F)==EOF)
      Error(2,Writing_Ruler_Txt);
    }
   } else
    Error(2,Illegal_Char_Txt);
  } else
  switch (Character)
  {case 8:
   {My_Col=1;
    break;
   }
   case 9:
   {while (1)
     if (++My_Col>512)
     {if (My_Col%8==1)
      break;
     } else
     if (Tabs[My_Col])
      break; else
     if (My_Col==Left_Margin||My_Col==Word_Wrap||My_Col==Right_Margin||
      My_Col==Indent)
      break;
    break;
   }
   case 13:
   {Opt_Cursor(Sav_Line,Sav_Col);
#if defined(VMS) | defined(UNIX)
    if (Gotmouse)	/* 290a */
     Enable_Mouse();
#endif
    Err_Display=1;
    if (!Show_Ruler)
     Fill_Screen(Rel_First,Rel_First);
    return;
   }
   case ' ':
   {if (My_Col<=512&&Tabs[My_Col])
      Tabs[My_Col]=0;
    if (Right_Margin==My_Col)
     Right_Margin=1;
    if (Left_Margin==My_Col)
     Left_Margin=1;
    if (Word_Wrap==My_Col)
     Word_Wrap=1;
    if (Indent==My_Col)
     Indent=1;
    break;
   }
   case 'l':
   case 'L':
   {if (My_Col<Right_Margin||Right_Margin==1)
    {Left_Margin=My_Col;
     if (Auto_Margin)
     {Auto_Margin=0;
      Word_Wrap=1;
     }
    } else
    Error(2,Left_Margin_Txt);
    break;
   }
   case 'w':
   case 'W':
   {if (My_Col<Right_Margin||Right_Margin==1)
    {Word_Wrap=My_Col;
     Auto_Margin=0;
    } else
     Error(2,Word_Wrap_Txt);
    break;
   }
   case 'r':
   case 'R':
   {if (My_Col>Left_Margin&&My_Col>Word_Wrap||My_Col==1)
    {Right_Margin=My_Col;
     Justify=0;
    } else
     Error(2,Right_Margin_Txt);
    break;
   }
   case 'j':
   case 'J':
   {if (My_Col>Left_Margin&&My_Col>Word_Wrap||My_Col==1)
    {Right_Margin=My_Col;
     Justify=1;
    } else
     Error(2,Right_Margin_Txt);
    break;
   }
   case 'i':
   case 'I':
   {Indent=My_Col;
    break;
   }
   case 't':
   case 'T':
   {if (My_Col<=512)
     Tabs[My_Col]=1;
    break;
   }
   case 'a':
   case 'A':
   {Word_Wrap=My_Col;
    Auto_Margin=1;
    Left_Margin=1;
    break;
   }
   case '-':
   {if (My_Col<=512)
     Tabs[My_Col]=0;
    break;
   }
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
   {Readruler((char)Character);
    break;
   }
   default:
    Error(2,Illegal_Char_Txt);
  }
  S_Ruler();
#if defined(VMS) | defined(UNIX)
  Show_Column();
#endif
 }
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Enable_Mouse();
#endif
}

/*      Skip across a series of spaces. Utility procedure for Do_Fill.
*/
void Fast Get_Spaces(int C)
{long *End_P;
 End_P=Points+Maxpoints+1;
 while (1)
 {if (C=='\n')
  {My_Col=1;
   End_Line++;
  } else
   My_Col+=Char_Rep(C,NULL);
  My_Pos++;
  if (My_Pos>=*End_P||(C=B_Get(Current))!='\n'&&C!=' '&&C!=9&&C!=12&&C!='\r')
   return;
 }
}

/*      Skip over a series of non-space characters. Utility procedure for
        Do_Fill.
*/
char Fast Get_Word(int C)
{long *End_P;
 char Old_C;
 End_P=Points+Maxpoints+1;
 while (1)
 {My_Col+=Char_Rep(C,NULL);
  My_Pos++;
  Old_C=C;
  if (My_Pos>=*End_P||(C=B_Get(Current))=='\n'||C==' '||C==9||C==12||C=='\r')
   return(Old_C);
 }
}

/*      Fill or justify a portion of text between the current cursor position
        and the end of an entity.
*/
void Fast Fill()
{if (!Get_Entity()||Skip)
  return;
 if (Right_Margin==1)
 {if (E_Type==E_Paste)
   Clear_Region();
  Error(2,No_Right_Margin_Txt);
  return;
 }
 if (Auto_Margin)
  Find_Margin();
 if (Right_Margin<=Word_Wrap)
 {if (E_Type==E_Paste)
   Clear_Region();
  Error(2,Word_Wrap_Txt);
  return;
 }
 Do_Fill();
}

/*      Insert a single character in the buffer and adjust all pointers. Does
        not affect the screen database.
*/
void Fast Put_A_Char(int C)
{B_Put(Current,(char)C);
 Adj_Points((long) 1,My_Pos);
}

/*      Tab to the next stop on the ruler line. Does not affect the screen
        database.
*/
void Fast Blind_Tab_To(long Col)
{if (!Tab_Expand)
  while (((My_Col-1)/8+1)*8+1<=Col)
  {Put_A_Char(9);
   My_Col=((My_Col-1)/8+1)*8+1;
  }
 while (My_Col<Col)
 {Put_A_Char(' ');
  My_Col++;
 }
}

void Fast Do_Fill()
/*Performs the actual fill or justify operation*/
{register int C, End_C=' ';
 static long Skip;
 long Sav_Pos, St_Pos, Line_Pos, Word_End, *My_P, *End_P, St_Col, Line_E_Col,
  Beg_Col, Spaces, Words, Before_Lines, After_Lines, This_Skip;
 if (Beg_Pos==End_Pos)
  return;
 Before_Lines=End_Line-Beg_Line+1;
 (*(End_P=Points+Maxpoints+1))=End_Pos;
 (*(My_P=Points+Maxpoints+2))=My_Pos;
 Move_Back();
 St_Pos=Beg_Pos;
 St_Col=My_Col;
 B_Goto(Current,My_Pos);
 if (My_Pos>=*End_P)
  return;
 if ((C=B_Get(Current))=='\n'||C==' '||C==9||C==12||C=='\r')
  Get_Spaces(C);
 B_Goto(Current,My_Pos);
 if (Justify)
 {Line_E_Col=My_Col;
  Line_Pos=My_Pos;
 }
 Words=Word_End=0;
 while (1)
 {if (My_Pos>=*End_P)
   break;
  if ((C=B_Get(Current))=='\n'||C==' '||C==9||C==12||C=='\r')
  {End_Line=This_Line;
   Beg_Col=My_Col;
   Beg_Pos=My_Pos;
   Get_Spaces(C);
   B_Goto(Current,My_Pos);
   if (My_Pos>=*End_P)
    break;
   Ent_Count=My_Pos-Beg_Pos;
   if (Justify?End_Line-This_Line<=1:End_Line-This_Line==1)
   {B_Goto(Current,My_Pos=Beg_Pos);
    My_Col=Beg_Col;
    B_Rem(Current,(long) Ent_Count);
    Adj_Points(-((long) Ent_Count),My_Pos);
    Put_A_Char(' ');
    My_Pos++;
    My_Col++;
    if (End_C=='.'||End_C==';'||End_C==':'||End_C=='?'||End_C=='!')
    {Put_A_Char(' ');
     My_Pos++;
     My_Col++;
    }
   } else
   if (End_Line-This_Line>0)
   {Words=0;
    Line_Pos=My_Pos;
   }
  } else
  {Sav_Pos=My_Pos; /*Word*/
   End_C=Get_Word(C);
   B_Goto(Current,My_Pos);
   if (My_Col>Right_Margin+1&&Words>0)
   {B_Goto(Current,My_Pos=Sav_Pos);
    if (Justify&&Words>1)
    {Spaces=Right_Margin-Line_E_Col+1;
     This_Skip=Skip=(Skip+1)%Words;
     while (Spaces)
     {B_Goto(Current,My_Pos=Line_Pos);
      C=B_Get(Current);
      while (Spaces)
      {if (C==' '||C==9||C==12)
        Get_Spaces(C);
       End_C=Get_Word(C);
       if (My_Pos==Word_End)
        break;
       if (This_Skip)
        This_Skip--; else
       {Put_A_Char(' ');
        Sav_Pos++;
        Word_End++;
        My_Pos++;
        Spaces--;
       }
      }
     }
    }
    B_Goto(Current,Sav_Pos);
    Ent_Count=0;
    while (B_B_Get(Current)==' ')
     Ent_Count++;
    if (Ent_Count)
    {B_Get(Current);
     My_Pos=Current->Pos;
     B_Rem(Current,(long) Ent_Count);
     Adj_Points(-((long) Ent_Count),My_Pos);
    }
    B_Goto(Current,My_Pos=Sav_Pos-Ent_Count);
#if ! (defined(UNIX) | defined(MPE))
    Put_A_Char('\r');
#endif
    Put_A_Char('\n');
    My_Col=1;
    Blind_Tab_To(Word_Wrap);
    My_Pos=Current->Pos;
    if (Justify)
     Line_Pos=My_Pos;
    Words=0;
   } else
   {if (Justify)
    {Line_E_Col=My_Col;
     Word_End=My_Pos;
    }
    Words++;
   }
  }
 }
 B_Goto(Current,My_Pos=St_Pos);
 My_Col=St_Col;
 Init_Entity();
 Ent_Count=(*End_P)-My_Pos;
 A_Char();
 After_Lines=End_Line-Beg_Line+1;
 Adj_Lines((long)(-Before_Lines));
 Adj_Lines((long)After_Lines);
 if (Before_Lines>Rel_Last||After_Lines>Rel_Last)
  Fill_Screen(0,Rel_Last); else
 {if (Before_Lines!=After_Lines)
  {if (Before_Lines>After_Lines)
   {Opt_Scroll((int)(Before_Lines-After_Lines),(int)After_Lines,Rel_Last);
    Fill_Screen(Rel_Last-(int)(Before_Lines+After_Lines),Rel_Last);
    Fill_Screen((int)After_Lines+1,(int)Before_Lines);
   } else
    Opt_Scroll((int)(Before_Lines-After_Lines),(int)Before_Lines,Rel_Last);
  }
  Fill_Screen(0,(int)After_Lines);
 }
 Init_Entity();
 Ent_Count=(*My_P)-My_Pos;
 B_Goto(Current,My_Pos);
 A_Char();
 Move_Entity();
 Set_Changes();
}

void Fast Adj_Tabs()
/*Moves a block of text either right or left by a number of spaces*/
{register int C;
 long St_Col, End_Col, St_Pos, *My_P, *End_P, I;
 if (!Get_Entity()||Skip)
  return;
 (*(End_P=Points+Maxpoints+1))=End_Pos;
 (*(My_P=Points+Maxpoints+2))=My_Pos;
 Move_Back();
 St_Pos=Beg_Pos;
 St_Col=My_Col;
 B_Goto(Current,My_Pos);
 while (1)
 {if (My_Pos>=*End_P)
   break;
  if (My_Col==1)
  {Beg_Pos=My_Pos;
   while (1)
   {if (My_Pos>=*End_P)
     break;
    if ((C=B_Get(Current))!=' '&&C!=9)
     break;
    My_Pos++;
    My_Col+=Char_Rep(C,NULL);
   }
   B_Goto(Current,My_Pos);
   if (C!='\n'&&My_Pos<*End_P)
   {Ent_Count=My_Pos-Beg_Pos;
    if (Direction)
    {End_Col=My_Col;
     I=Count;
     while (I--)
      while (End_Col<512&&!Tabs[++End_Col])
       ;
     Blind_Tab_To(End_Col);
     My_Pos=Current->Pos;
    } else
    {End_Col=My_Col;
     I=Count;
     while (I--)
      while (End_Col>1&&!Tabs[--End_Col])
       ;
     B_Goto(Current,My_Pos=Beg_Pos);
     My_Col=1;
     B_Rem(Current,(long) Ent_Count);
     Adj_Points(-((long) Ent_Count),My_Pos);
     Blind_Tab_To(End_Col);
     My_Pos=Current->Pos;
    }
   }
  }
  My_Col=(C=B_Get(Current))=='\n'?1:My_Col+Char_Rep(C,NULL);
  My_Pos++;
 }
 B_Goto(Current,My_Pos=St_Pos);
 My_Col=St_Col;
 Fill_Screen(0,Rel_Last);
 Init_Entity();
 Ent_Count=(*My_P)-My_Pos;
 B_Goto(Current,My_Pos);
 A_Char();
 Move_Entity();
 Set_Changes();
}

void Fast Center_Line()
/*Centers the current line*/
{register int C;
 long Spaces;
 int Sav_Replace_Mode;
 long B_End, *My_P;
 if (Right_Margin==1)
 {Error(2,No_Right_Margin_Txt);
  return;
 }
 Sav_Replace_Mode=Replace_Mode;
 Replace_Mode=0;
 B_End=Current->Bufsize;
 (*(My_P=Points+Maxpoints+1))=My_Pos;
 B_Goto(Current,My_Pos);
 Init_Entity();
 Ent_Direction=0;
 Beg_Pos=B_B_Search(Current,'\n',(long) 1)?Current->Pos+1:1;
 Move_Entity();
 Init_Entity();
 while (1)
 {if (End_Pos==B_End+1)
  {Error(2,Empty_Line_Txt);
   End_Pos=(*My_P);
   Move_Entity();
   Replace_Mode=Sav_Replace_Mode;
   return;
  }
  if ((C=B_Get(Current))=='\n')
  {Error(2,Empty_Line_Txt);
   End_Pos=(*My_P);
   Move_Entity();
   Replace_Mode=Sav_Replace_Mode;
   return;
  }
  if (C!=' '&&C!=9)
   break;
  End_Pos++;
 }
 B_Goto(Current,My_Pos);
 Rem_It(Unknown_String);
 Init_Entity();
 B_Search(Current,'\n',(B_End=Current->Bufsize)+1);
 End_Pos=Current->Pos;
 B_Goto(Current,My_Pos);
 if ((Spaces=(Right_Margin-Left_Margin+1)/2-
  (End_Pos-Beg_Pos)/2+Left_Margin+1)>0)
  Tab_To(Spaces);
 /*Diag("Center:L_M=%d,R_M=%d,B_P=%ld,E_P=%ld,Sp=%d",Left_Margin,Right_Margin,
  Beg_Pos,End_Pos,Spaces);*/
 Init_Entity();
 if (My_Pos>*My_P)
 {Ent_Direction=0;
  Ent_Count=My_Pos-*My_P;
 } else
 {Ent_Direction=1;
  Ent_Count=(*My_P)-My_Pos;
 }
 B_Goto(Current,My_Pos);
 A_Char();
 Move_Entity();
 Set_Changes();
 Replace_Mode=Sav_Replace_Mode;
}

void Fast Refresh()
{
#if (defined(ATARI) & defined(GEM)) | defined(DOS) | defined(NT) | defined(OS2)  | defined(WIN32)
 int Offset;
 Offset=Rel_First+Rel_Last;
 Rel_First=Show_Ruler?3:2;
 Rel_Last=Scr_Length-2;
 My_Line=Home=(Rel_First+Rel_Last+1)/2;
 Rel_First-=Home;
 Rel_Last-=Home;
 if (Float_Cursor)
  Set_Offset(Offset);
#endif
#if defined(VMS) | (defined(DOS) & !((defined(Windows) | defined(TV)) | defined(TV)) & defined(PC100)) | defined(UNIX) | defined(OS2)
 Reset_Term();
 Setup_Term();
 Reset_Screen();
 if (!Zoomed) 	/* 290a */
 {
  Sav_Window(Cur_Buffer);
  {register int I;
   for (I=1;I<Buffers-1;I++)	/* 290a search next zoomed window in stack */
   {Load_Window(Buffer_Stack[I]);
    if (Zoomed) break;
   }
   for (;I>=0;I--)
   {Out_Zap();
   Load_Window(Cur_Buffer=Buffer_Stack[I]);
   if (I==0)
     W_D_Border(); else
     W_S_Border();
   Repaint();
   }
  }
 } else
 Repaint();
#else
 Opt_Init();
 S_Header();
 if (Show_Ruler)
  S_Ruler();
 Fill_Screen(Rel_First,Rel_Last);
#endif
 Clr_Error();
}

void Fast Repaint()
{int Offset;
 Offset=Rel_First+Rel_Last;
 Rel_First=Show_Ruler?3:2;
#if defined(GEM) | defined(Windows) | defined(TV)
 Rel_Last=Scr_Length-2;
#else
 Rel_Last=Scr_Length;	/* 290a */ 
#endif
 My_Line=Home=(Rel_First+Rel_Last+1)/2;
 Rel_First-=Home;
 Rel_Last-=Home;
 if (Zoomed)
 {if (Scr_Width!=Max_Width)
   Scr_Width=Max_Width;
 } else
 {if (Left_Col>Max_Width-10)
   Left_Col=Max_Width-10;
  if (Left_Col+Scr_Width>Max_Width)
   Scr_Width=Max_Width-Left_Col;
 }
 if (Float_Cursor)
  Set_Offset(Offset);
 Scr_Shift=0;
 if (Do_Shift)
 {if (My_Col-Scr_Shift>=Scr_Width)
  {while (My_Col-Scr_Shift>=Scr_Width)
    Scr_Shift+=Shift_Screen?Shift_Screen:Scr_Width/2;
  }
 }
 {register int I;
  for (I=1;I<=Scr_Length;I++)
   Lst_Col[I]=Scr_Width;
 }
 S_Header();
 if (Show_Ruler)
  S_Ruler();
 Fill_Screen(Rel_First,Rel_Last);
 Clr_Error();
}

void Fast Width80()
{
#if (defined(DOS) & !(defined(Windows) | defined(TV)) & defined(PC100)) | defined(VMS) | defined(UNIX)
 Scr_Shift=0;
 Hdr_Changes=1;
 Narrow_Screen();
 Refresh();
#endif
}

void Fast Width132()
{
#if (defined(DOS) & !(defined(Windows) | defined(TV)) & defined(PC100)) | defined(VMS) | defined(UNIX)
 Scr_Shift=0;
 Hdr_Changes=1;
 Wide_Screen(); 
 Refresh();
#endif
}
void Fast Width()
{
#if (defined(DOS) & !(defined(Windows) | defined(TV)) & defined(PC100)) | defined(VMS) | defined(UNIX)
 Scr_Shift=0;
 Hdr_Changes=1;
 if (Phys_Width==80)
  Wide_Screen(); else
  Narrow_Screen();
 Refresh();
#endif
}

void Fast Spawn()
{
#if !defined(Windows) & !defined(GEM) & !defined(TV)
char *S, *P;
 int Spawn_Error=0, I;
#if defined(UNIX)
 char *cp, *getenv();
/* extern int errno;*/
#endif
#if defined(VMS)
 static $DESCRIPTOR(Desc,Com_Arg);
#endif
#if defined(DOS) | defined(NT) | defined(OS2)
 char Curdir[67];
#endif
#if defined(WIN32)
 char Curdir[300];
#endif
 if (!Get_Arg()||Skip)
  return;
#if defined(DOS) & !defined(Windows)
#if !defined(PC100)
 KEYREL();
#endif
 Reset_Crit();
#endif
 Out_Zap();
 Sav_Window(Cur_Buffer);
 Top_Row=Left_Col=0;
 Set_Cursor(1,1);
 CC_Enable=0;
 Set_Scroll(1,Scr_Length);
 Reset_Width();
 Out_Zap();
#if defined(DOS) & defined(TV)
 TV_Suspend();
#endif
 if (Arg_Length==0)
#if (defined(DOS) & !defined(Windows)) | defined(OS2) | defined(WIN32)
#if defined(__BORLANDC__)
 {getcwd(Curdir,sizeof(Curdir)-1);
#else
 {_getcwd(Curdir,sizeof(Curdir)-1);
#endif
  if ((S=getenv("COMSPEC"))==NULL)
   Error(2,No_Compec_Txt); else
   {strcpy(Dsk_Rec,S);
    if ((P=strchr(Dsk_Rec,' '))!=NULL)
     *P=0;
    if ((P=strchr(Dsk_Rec,'-'))!=NULL)
     *P=0;
    if ((P=strchr(Dsk_Rec,'/'))!=NULL)
     *P=0;
    if ((I=spawnl(P_WAIT,Dsk_Rec,Dsk_Rec,NULL))==-1)
     Error(2,No_Spawn_Txt); else
      if (I)
       Spawn_Error=1;
   }
#if defined(__BORLANDC__)
  chdir(Curdir);
#else
  _chdir(Curdir);
#endif
#if defined(DOS) | defined(NT)
#if !defined(PC100)
  OURKBD();
#endif
  Set_Crit();
#endif
 } else
#endif
#if defined(ATARI)  | defined(MPE)
 {if (system("msh"))
   Spawn_Error=1;
 } else
#endif
#if defined(UNIX)
 {
#if defined(TERMCAP)
  USysenv();
#endif
  if ((cp = getenv("SHELL")) != NULL)
   system(cp);	else
   system("exec /bin/sh");
#if defined(TERMCAP)
  USedtenv();
#endif
 } else
 {
#if defined(TERMCAP)
  USysenv();
#endif
  if (system(Com_Arg))
   Spawn_Error=1;
#if defined(TERMCAP)
  USedtenv();
#endif
 }
#endif
#if defined(VMS)
 {T$Cancel();
  lib$spawn();
  T$Init();
 } else
#endif
 {
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
#if defined(__BORLANDC__)
  getcwd(Curdir,sizeof(Curdir)-1);
#else
  _getcwd(Curdir,sizeof(Curdir)-1);
#endif
  if (system(Com_Arg))
   Spawn_Error=1;
#if defined(__BORLANDC__)
  chdir(Curdir);
#else
  _chdir(Curdir);
#endif
#endif
#if defined(ATARI) | defined(MPE)
  if (system(Com_Arg))
   Spawn_Error=1;
#endif
#if defined(VMS)
  Desc.dsc$w_length=Arg_Length;
  T$Cancel();
  lib$spawn(&Desc);
  T$Init();
#endif
#if defined(DOS) & !defined(Windows)
#if !defined(PC100)
  OURKBD();
#endif
  Set_Crit();
#endif
  printf(Return_To_Sedt_Txt);
  Get16();      /* 290a */
 }
#if defined(DOS) & defined(TV)
 Load_Window(Cur_Buffer);
 Setup_Term();
 TV_Resume();
#else
 Setup_Term();
 Reset_Screen();
#endif
#if defined(VMS) | defined(UNIX)
 if (Org_Width>80)
 {if (Scr_Width == 80)
   Narrow_Screen();
 } else
 if (Org_Width == 80)
 {if (Scr_Width>80)
   Wide_Screen();
 }
#endif
#if defined(DOS) & !((defined(Windows) | defined(TV)) | defined(TV)) & defined(PC100)
 if (Scr_Width==132)
  Wide_Screen();
#endif
#if !defined(TV)
 if (!Zoomed)
 {Load_Window(Cur_Buffer);
  Refresh_Window();
 } else
  Repaint();
#endif
 CC_Enable=1;
 if (Spawn_Error)
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(OS2) | defined(WIN32)
   Error(2,(char *) Spawn_Error_Txt,errno);
#else
   Error(2,No_Spawn_Txt);
#endif
#elif defined(TV)
char *S, *P;
 int Spawn_Error=0, I;
 char Curdir[67];
 if (!Get_Arg()||Skip)
  return;
 Reset_Crit();
 Out_Zap();
 CC_Enable=0;
 TV_Suspend();
 if (Arg_Length==0)
 {getcwd(Curdir,66);
  if ((S=getenv("COMSPEC"))==NULL)
   Error(2,No_Compec_Txt); else
   {strcpy(Dsk_Rec,S);
    if ((P=strchr(Dsk_Rec,' '))!=NULL)
     *P=0;
    if ((P=strchr(Dsk_Rec,'-'))!=NULL)
     *P=0;
    if ((P=strchr(Dsk_Rec,'/'))!=NULL)
     *P=0;
    if ((I=spawnl(P_WAIT,Dsk_Rec,Dsk_Rec,NULL))==-1)
     Error(2,No_Spawn_Txt); else
      if (I)
       Spawn_Error=1;
   }
  chdir(Curdir);
  Set_Crit();
 } else
 {
  getcwd(Curdir,66);
  if (system(Com_Arg))
   Spawn_Error=1;
  chdir(Curdir);
  Set_Crit();
  printf(Return_To_Sedt_Txt);
  getchar();
 }
 TV_Resume();
 CC_Enable=1;
 if (Spawn_Error)
   Error(2,(char *) Spawn_Error_Txt,errno);
#else
 if (!Get_Arg()||Skip)
  return;
 Error(2,Not_Implemented_Txt);
#endif
}

void Fast Q_Spawn()
{
#if !defined(Windows) & !defined(GEM) & !defined(TV)
 int Spawn_Error=0;
#if defined(UNIX)
 char *cp, *getenv();
/* extern int errno;*/
#endif
#if defined(VMS)
 static $DESCRIPTOR(Desc,Com_Arg);
#endif
#if defined(DOS) | defined(NT) | defined(OS2)
 char Curdir[67];
#endif
#if defined(WIN32)
 char Curdir[300];
#endif
 if (!Get_Arg()||Skip)
  return;
 if (Arg_Length==0)
  Error(2,No_Command_Txt); else
 {
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
#if defined(__BORLANDC__)
  getcwd(Curdir,sizeof(Curdir)-1);
#else
  _getcwd(Curdir,sizeof(Curdir)-1);
#endif
  if (system(Com_Arg))
   Spawn_Error=1;
#if defined(__BORLANDC__)
  chdir(Curdir);
#else
  _chdir(Curdir);
#endif
#endif
#if defined(ATARI) | defined(MPE)
  if (system(Com_Arg))
   Spawn_Error=1;
#endif
#if defined(VMS)
  Desc.dsc$w_length=Arg_Length;
  T$Cancel();
  lib$spawn(&Desc);
  T$Init();
#endif
#if defined(UNIX)
#if defined(TERMCAP)
  USysenv();
#endif
  if (system(Com_Arg))
   Spawn_Error=1;
#if defined(TERMCAP)
  USedtenv();
#endif
#endif
 }
 if (Spawn_Error)
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(WIN32)
   Error(2,Spawn_Error_Txt,errno);
#else
   Error(2,No_Spawn_Txt);
#endif
#else
 if (!Get_Arg()||Skip)
  return;
 Error(2,Not_Implemented_Txt);
#endif
}

void Fast R_Spawn()
{
#if !defined(Windows) & !defined(GEM) & !defined(TV) & !defined(MPE)
 int Spawn_Error=0, New_stdout, Sav_stdout,
  New_stderr, Sav_stderr;
#if defined(UNIX)
 char *cp, *getenv();
/* extern int errno; */
#endif
#if defined(VMS)
 static $DESCRIPTOR(Desc,Com_Arg);
#endif
#if defined(DOS) | defined(NT) | defined(OS2)
 char Curdir[67];
#endif
#if defined(WIN32)
 char Curdir[300];
#endif
 Get_Arg();
 if (!(Skip||Err_Flag))
 {
#if defined(VMS)
  delete(Com_Arg);
#else
  unlink(Com_Arg);
#endif
#if defined(ATARI) | defined(MPE)
  if ((New_stdout=creat(Com_Arg,0))==-1)
#else
#if defined(NT)
  if ((New_stdout=_creat(Com_Arg,S_IREAD|S_IWRITE))==-1)
#else
  if ((New_stdout=creat(Com_Arg,S_IREAD|S_IWRITE))==-1)
#endif
#endif
   Error(2,No_Stdout_Txt); else
  {
#if defined(NT)
   Sav_stdout=_dup(1);
   _dup2(New_stdout,1);
#else
   Sav_stdout=dup(1);
   dup2(New_stdout,1);
#endif
  }
 }
 B_B_Get(&Com);
 if (!Get_Arg())
 {
#if defined(NT)
  _close(New_stdout);
  _dup2(Sav_stdout,1);
#else
  close(New_stdout);
  dup2(Sav_stdout,1);
#endif
  return;
 }
 if (!(Skip||Err_Flag))
 {
#if defined(VMS)
  delete(Com_Arg);
#else
  unlink(Com_Arg);
#endif
#if defined(ATARI) | defined(MPE)
  if ((New_stderr=creat(Com_Arg,0))==-1)
#else
#if defined(NT)
  if ((New_stderr=_creat(Com_Arg,S_IREAD|S_IWRITE))==-1)
#else
  if ((New_stderr=creat(Com_Arg,S_IREAD|S_IWRITE))==-1)
#endif
#endif
  {Error(2,No_Stderr_Txt);
#if defined(NT)
   _close(New_stdout);
   _dup2(Sav_stdout,1);
#else
   close(New_stdout);
   dup2(Sav_stdout,1);
#endif
  } else
  {
#if defined(NT)
   Sav_stderr=_dup(2);
   _dup2(New_stderr,2);
#else
   Sav_stderr=dup(2);
   dup2(New_stderr,2);
#endif
  }
 }
 B_B_Get(&Com);
 if (!Get_Arg())
 {
#if defined(NT)
  _close(New_stdout);
  _dup2(Sav_stdout,1);
  _close(New_stderr);
  _dup2(Sav_stderr,2);
#else
  close(New_stdout);
  dup2(Sav_stdout,1);
  close(New_stderr);
  dup2(Sav_stderr,2);
#endif
  return;
 }
 if ((Skip||Err_Flag))
  return;
 if (Arg_Length==0)
  Error(2,No_Command_Txt); else
 {
#if defined(ATARI) | defined(MPE)
  if (system(Com_Arg))
   Spawn_Error=1;
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
#if defined(__BORLANDC__)
  getcwd(Curdir,sizeof(Curdir)-1);
#else
  _getcwd(Curdir,sizeof(Curdir)-1);
#endif
  if (system(Com_Arg))
   Spawn_Error=1;
#if defined(__BORLANDC__)
  chdir(Curdir);
#else
  _chdir(Curdir);
#endif
#endif
#if defined(VMS)
  Desc.dsc$w_length=Arg_Length;
  T$Cancel();
  lib$spawn(&Desc);
  T$Init();
#endif
#if defined(UNIX)
#if defined(TERMCAP)
  USysenv();
#endif
  if (system(Com_Arg))
   Spawn_Error=1;
#if defined(TERMCAP)
  USedtenv();
#endif
#endif
 }
 if (Spawn_Error)
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(WIN32)
   Error(2,Spawn_Error_Txt,errno);
#else
   Error(2,No_Spawn_Txt);
#endif
#if defined(NT)
 _close(New_stdout);
 _dup2(Sav_stdout,1);
 _close(New_stderr);
 _dup2(Sav_stderr,2);
#else
 close(New_stdout);
 dup2(Sav_stdout,1);
 close(New_stderr);
 dup2(Sav_stderr,2);
#endif
#else
 if (!Get_Arg()||Skip)
  return;
 Error(2,Not_Implemented_Txt);
#endif
}

void Fast Ch_Case()
{What_Kind=Change;
 if (!Get_Entity()||Skip)
  return;
 Ch_It();
}

void Fast Ch_Lower()
{What_Kind=Lower;
 if (!Get_Entity()||Skip)
  return;
 Ch_It();
}

void Fast Ch_Upper()
{What_Kind=Upper;
 if (!Get_Entity()||Skip)
  return;
 Ch_It();
}

void Fast Ch_It()
{register int C;
 long This_Pos;
 This_Pos=Beg_Pos;
 B_Goto(Current,This_Pos);
 if (!Current->Modified)
  B_Modify(Current);
 while (This_Pos!=End_Pos)
 {This_Pos++;
  if (What_Kind==Upper)
  {if (islower(C=B_Get(Current)))
    Current->Membuf[Current->Memptr-1]=_toupper(C);
   if (C>=224&&C<=255)
    Current->Membuf[Current->Memptr-1]=C-32;
  } else
  if (What_Kind==Lower)
  {if (isupper(C=B_Get(Current)))
    Current->Membuf[Current->Memptr-1]=_tolower(C);
   if (C>=192&&C<=223)
    Current->Membuf[Current->Memptr-1]=C+32;
  } else
  if (What_Kind==Change)
  {if (islower(C=B_Get(Current)))
    Current->Membuf[Current->Memptr-1]=_toupper(C); else
   if (isupper(C))
    Current->Membuf[Current->Memptr-1]=_tolower(C);
   if (C>=224&&C<=255)
    Current->Membuf[Current->Memptr-1]=C-32; else
   if (C>=192&&C<=223)
    Current->Membuf[Current->Memptr-1]=C+32;
  }
 }
 if (Beg_Line<This_Line)
 {Fill_Screen(This_Line-Beg_Line<=-Rel_First?(int)(Beg_Line-This_Line):
   Rel_First,0);
 } else
 if (End_Line>This_Line)
 {Fill_Screen(0,End_Line-This_Line<=Rel_Last?(int)(End_Line-This_Line):
   Rel_Last);
 } else
 if (My_Pos==Beg_Pos)
  Fill_Rest(); else
  Fill_Screen(0,0);
 Set_Changes();
}

void Fast Repl()
{if (Lst_Cmd!=C_Movecursor||E_Type!=E_Find)
 {Error(2,Not_Search_Txt);
  return;
 }
 Init_Entity();
 Ent_Count=Dl_Pointers[Search_String][1];
 A_Char();
 Rem_It(Unknown_String);
 Undelete(Past_String);
 Set_Changes();
}

void Fast Print()
{FILE *F;
 register int C;
 register int Line, Lines;
 long B_End, This_Pos;
 if (!Get_Arg()||Skip)
  return;
#if defined(DOS) & defined(Windows)
 Print_Buffer();
#else
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 if (Arg_Length==0)
  strcpy(Com_Arg,"PRN");
#endif
#if defined(ATARI) | defined(MPE)
 if (Arg_Length==0)
  strcpy(Com_Arg,"PRN:");
#endif
#if defined(VMS)
 if (Arg_Length==0)
  strcpy(Com_Arg,"LP0:");
#endif
#if defined(UNIX)
 if (Arg_Length==0)
  strcpy(Com_Arg,"/dev/lp0");
#endif
 if (Count==1)
  Lines=61; else
  Lines=Count;
 while (Count--&&!Check_CC())
 {if ((F=fopen(Com_Arg,"w"))==NULL)
  {Error(2,No_Print_Txt);
   return;
  }
  B_End=Current->Bufsize;
  Plain_Message(Txt_Printing);
  B_Goto(Current,(long)(Line=This_Pos=1));
  while (This_Pos<=B_End)
  {This_Pos++;
   fputc(C=B_Get(Current),F);
   if (C=='\n')
   {Line++;
    if (Line==Lines)
    {fputc(12,F);
     Line=1;
    }
#if !((defined(ATARI) & defined(GEM)) | defined(UNIX))
    if (Input_Test())
    {Error(2,Abort_Print_Txt);
     fputc(12,F);
     fclose(F);
     return;
    }
#endif
   } else
   if (C==12)
   {fputc(13,F);
    Line=1;
#if !((defined(ATARI) & defined(GEM)) | defined(UNIX))
    if (Input_Test())
    {Error(2,Abort_Print_Txt);
     fclose(F);
     return;
    }
#endif
   }
  }
 }
 fputc(12,F);
 fclose(F);
 Plain_Message("");
#endif
}

void Fast File_Execute()
/*Executed commands in a file*/
{struct File F;
 if (!Get_Arg()||Skip)
  return;
 if (F_open(Com_Arg,"r",&F)!=NULL)
  Ex_File(&F); else
  Error(2,Command_File_Txt);
}

void Fast Ex_File(struct File *F)
{register int C, Old_C;
 long P, O;
 O=Com.Pos;
 P=Com.Bufsize+1;
 while (!Err_Flag&&(C=F_getc(F))!=EOF)
 {B_Goto(&Com,P);
  do
   B_Put(&Com,(char)(Old_C=C));
  while ((C=F_getc(F))!=EOF&&C!='\n');
  if (C=='\n'&&Old_C=='\r')
  {B_B_Get(&Com);
   B_Del(&Com);
  }
  B_Goto(&Com,P);
  if (Com.Pos<=Com.Bufsize)
  {if (B_Get(&Com)!=';')
   {Com_Direction=Def_Direction;
    Expand_Com(P);
    Skip=Severe_Error=0;
    Com_Parse(P,0);
   }
  }
  B_Goto(&Com,P);
  B_Rem(&Com,Com.Bufsize-P+1);
 }
 F_close(F);
 B_Goto(&Com,O);
}

void Fast Ind_Set()
{Indent=My_Col;
 if (Show_Ruler)
  S_Ruler();
}

void Fast Ind_Decrease()
{Indent-=Count;
 if (Indent<1)
  Indent=1;
 if (Show_Ruler)
  S_Ruler();
}

void Fast Ind_Increase()
{Indent+=Count;
 if (Show_Ruler)
  S_Ruler();
}

void Fast Key_Load(long *Where)
{register long Chars;
 Rem_Pad(Where);
 Set_Region();
 Where[0]=Pad.Bufsize+1;
 Where[1]=0;
 B_Goto(&Pad,Where[0]);
 Chars=End_Pos-Beg_Pos;
 B_Goto(Current,Beg_Pos);
 B_Copy(Current,&Pad,Chars);
 Where[1]=Chars;
}

void Fast Get_Key()
{Map_Index=0;
 {register int I;
  register char C;
  for (I=1;I<=3;I++)
  {if (End_Com())
   {Com_Error();
    return;
   }
   C=B_Get(&Com);
   if (!isdigit(C))
   {Com_Error();
    return;
   }
   Map_Index=Map_Index*10+C-'0';
  }
 }
 if (Skip)
  return;
}

void Fast Load_Key()
{Get_Key();
 if (Err_Flag||!Get_Entity()||Skip)
  return;
 if ((Function=Map_Tab[Map_Index].F_Code)<=0||Function<=Top_Func)
 {if (Next_Func==Maxfunc-1)
  {Error(2,Max_Func_Txt);
   return;
  } else
   Map_Tab[Map_Index].F_Code=Function= ++Next_Func;
 }
 Key_Load(Key_Addr(Function));
 if (E_Type==E_Paste)
  Clear_Region();
}

void Fast Key_Save(long *Where)
{register long Lines, Chars;
 B_Goto(Current,My_Pos);
 if (Where[0]==0)
  return;
 B_Goto(&Pad,Where[0]);
 if ((Chars=Where[1])==0)
  return;
 Adj_Points(Chars,My_Pos);
 B_Copy(&Pad,Current,Chars);
 B_Goto(Current,My_Pos);
 Lines=0;
 while (B_Search(Current,'\n',My_Pos+Chars))
  Lines++;
 B_Goto(Current,My_Pos);
 if (Lines)
 {Adj_Lines(Lines);
  if (Lines<Rel_Last)
  {Opt_Scroll((int)-Lines,0,Rel_Last);
   Fill_Screen(0,(int)Lines);
  } else
  {Fill_Screen(0,Rel_Last);
  }
 } else
  Fill_Rest();
 Set_Changes();
}

void Fast Sav_Key()
{Get_Key();
 if (!Skip&&!Err_Flag)
  Key_Save(Key_Addr(Function=Map_Tab[Map_Index].F_Code));
}

void Fast Del_Append()
{Append_It=1;
 Del_It();
 Append_It=0;
}

void Fast Get_Append()
{Append_It=1;
 Get();
 Append_It=0;
}

void Fast Ch_Edt_Style()
{What_Kind=Change;
 Init_Entity();
 Ent_Count=Count;
 Ent_Direction=Direction;
 if (Points[0]!=0)
 {E_Type=E_Paste;
  Sel_Range();
  Ch_It();
 } else
 if (Lst_Cmd==C_Movecursor&&E_Type==E_Find)
 {Ent_Count=Dl_Pointers[Search_String][1];
  A_Char();
  if (Err_Flag)
   return;
  Ch_It();
 } else
 {A_Char();
  if (Err_Flag)
   return;
  Ch_It();
  B_Goto(Current,Beg_Pos);
  Move_Entity();
 }
 Set_Changes();
}

void Fast Past_Load()
{struct File F;
 int In_Count=0;
 if (!Get_Arg()||Skip)
  return;
 if ((F_open(Com_Arg,"r",&F))==NULL)
 {Error(2,Paste_File_Txt);
  return;
 }
 if (Err_Flag)
  return;
 Erase_Del(Dl_Pointers[Past_String]);
 while ((In_Count=F_read(Dsk_Rec,1,Blksize,&F))!=0)
  B_Put_S(&Del,Dsk_Rec,In_Count);
 Dl_Pointers[Past_String][1]=Del.Pos-Dl_Pointers[Past_String][0];
 F_close(&F);
}

void Fast Past_Save()
/*Saves the paste buffer in a file*/
{long Count;
 struct File F;
 long Out_Count=0, L;
 if (!Get_Arg()||Skip)
  return;
 if ((F_open(Com_Arg,"w",&F))==NULL)
 {Error(2,Paste_Create_Txt);
  return;
 }
 if (Err_Flag)
  return;
 Count=Dl_Pointers[Past_String][1];
 B_Goto(&Del,Dl_Pointers[Past_String][0]);
 Out_Count=0;
 while (Count)
 {if ((L=Blksize-1)>Count)
   L=Count;
  Dsk_Rec[Out_Count=(long)B_Get_S(&Del,Dsk_Rec,(int)L)]=0;
  Count-=Out_Count;
  if (!Put_Out(&F,Dsk_Rec,(int)Out_Count))
   return;
  Out_Count=0;
 }
 if (F_close(&F)<0)
 {Wr_Error();
  return;
 }
}

void Fast Open_Line()
/*Insert a new line*/
{while (Count--&&!Check_CC())
 {B_Goto(Current,My_Pos);
#if ! (defined(UNIX) | defined(MPE))
  Put_A_Char('\r');
#endif
  Put_A_Char('\n');
  B_Goto(Current,My_Pos);
  Opt_Scroll(-1,0,Rel_Last);
  if (My_Col!=1)
   Fill_Screen(0,1);
#if defined(ATARI) | defined(MPE)
  else
   Fill_Screen(0,0);
#endif
 }
 Set_Changes();
}

void Fast Upd_Screen()
{Upd_Header();
 Out_Zap();
}

void Fast Load_Learn()
{long
  *Where;
 if (Learning)
 {Error(2,Already_Learning_Txt);
  End_Learn();	/* 290a */
  return;
 }
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#endif
 Get_Key();
 if (Skip||Err_Flag)
  return;
 if ((Function=Map_Tab[Map_Index].F_Code)<=0||Function<=Top_Func)
 {if (Next_Func==Maxfunc-1)
  {Error(2,Max_Func_Txt);
   return;
  } else
   Map_Tab[Map_Index].F_Code=Function= ++Next_Func;
 }
 Ln_Function=Function;
 Rem_Pad(Where=Key_Addr(Function));
 Where[0]=Pad.Bufsize+1;
 Where[1]=1;
 B_Goto(&Pad,Where[0]);
 B_Put(&Pad,15);
 Learning=1;
 Hdr_Changes=1;
}

void Fast Ex_Learn()
{long
  *K;
/*
 if (Learning)
 {Error(2,Already_Learning_Txt);
  return;
 }
*/
 Get_Key();
 if (Skip||Err_Flag)
  return;
 if (Ex_Debth==10)
 {Error(2,To_Deep_Ex_Txt);
  return;
 }
 Ex_Function[Ex_Debth]=Function;
 Ex_Index[Ex_Debth]=1;
 Ex_Count[Ex_Debth]=(int)Count;
 if ((K=Key_Addr(Ex_Function[Ex_Debth]))[1]==0)
  Key_Mess(); else
 {B_Goto(&Pad,K[0]);
  if (B_Get(&Pad)!=15)
   Key_Mess(); else
  if (Ex_Index[Ex_Debth]>=K[1])
   Plain_Message(Null_String_Txt); else
  {Executing=1;
   Ex_Debth++;
   Hdr_Changes=1;
  }
 }
}

void Fast Search_Load()
/*Get an entity*/
{if (!Get_Entity()||Skip)
  return;
 if (E_Type==E_Paste)
   Clear_Region();
 Rem_It(Search_String);
}

void Fast Out_Message()
{if (!Get_Arg()||Skip)
  return;
 Plain_Message(Com_Arg);
}

void Fast Put_Date()
{
#if defined(VMS)
 char *S, C;
 Today_Desc.dsc$w_length=11;
 Today[11]=0;
 Lib$Date_Time(&Today_Desc);
 S=Today;
 while (C=(*S++))
  Ins_Char(C);
#endif
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(WIN32)
 char *S, *T;
 long Time;
 Time=time(NULL);
 T=ctime(&Time);
 S=T+8;
 if (*S=='0')
  (*S)=' ';
 {register int I;
  for (I=1;I<3;I++)
   Ins_Char(*S++);
 }
 Ins_Char('-');
 S=T+4;
 {register int I;
  for (I=1;I<4;I++)
   Ins_Char(*S++);
 }
 Ins_Char('-');
 S=T+20;
 {register int I;
  for (I=1;I<5;I++)
   Ins_Char(*S++);
 }
#endif
#if defined(ATARI)
 int Date;
 char D_String[14], C, *S;
 Date=Tgetdate();
 switch ((Date>>5)%16)
 {case 1:
   S=(char *) January_Txt;
   break;
  case 2:
   S=(char *) February_Txt;
   break;
  case 3:
   S=(char *) March_Txt;
   break;
  case 4:
   S=(char *) April_Txt;
   break;
  case 5:
   S=(char *) May_Txt;
   break;
  case 6:
   S=(char *) June_Txt;
   break;
  case 7:
   S=(char *) July_Txt;
   break;
  case 8:
   S=(char *) August_Txt;
   break;
  case 9:
   S=(char *) September_Txt;
   break;
  case 10:
   S=(char *) October_Txt;
   break;
  case 11:
   S=(char *) November_Txt;
   break;
  case 12:
   S=(char *) December_Txt;
   break;
 }
 sprintf(D_String,"%2d-%s-19%2d",Date%32,S,(Date>>9)+80);
 S=D_String;
 while (C=(*(S++)))
  Ins_Char(C);
#endif
#if defined(OS2)
 struct _DATETIME Date_Time;
 char D_String[14], C, *S;
 DosGetDateTime((PDATETIME)&Date_Time);
 switch (Date_Time.month)
 {case 1:
   S=(char *) January_Txt;
   break;
  case 2:
   S=(char *) February_Txt;
   break;
  case 3:
   S=(char *) March_Txt;
   break;
  case 4:
   S=(char *) April_Txt;
   break;
  case 5:
   S=(char *) May_Txt;
   break;
  case 6:
   S=(char *) June_Txt;
   break;
  case 7:
   S=(char *) July_Txt;
   break;
  case 8:
   S=(char *) August_Txt;
   break;
  case 9:
   S=(char *) September_Txt;
   break;
  case 10:
   S=(char *) October_Txt;
   break;
  case 11:
   S=(char *) November_Txt;
   break;
  case 12:
   S=(char *) December_Txt;
   break;
 }
 sprintf(D_String,"%2d-%s-%4d",Date_Time.day,S,Date_Time.year);
 S=D_String;
 while (C=(*(S++)))
  Ins_Char(C);
#endif
 Set_Changes();
}
