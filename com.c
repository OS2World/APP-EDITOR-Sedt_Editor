/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"

#if defined(XTERM_MOUSE)     
extern void xmouse_on();     
extern void xmouse_off();    
extern void xmouse_toggle(); 
extern void xmouse_handler();
#endif                       


#if defined(VMS) | defined(UNIX) | defined(MPE)
#if defined(ANSIC)
struct Com_Rec {char *Name;int Type;int Skipok;
 void (Fast *Function)(void);}
#else
struct Com_Rec {char *Name;int Type;int Skipok;
 void (Fast *Function)();}
#endif
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
struct Com_Rec {char *Name;int Type, Skipok;void (Fast *Function)(void);}
#endif
#if defined(ATARI)
struct Com_Rec {char *Name;int Type, Skipok;void (Fast *Function)();}
#endif

 Com_Tab[]=
  {{"A",C_Unknown,1,Advance},
   {"B",C_Unknown,1,Backup},
   {"CB",C_Unknown,1,Curs_Bottom},
   {"CC",C_Unknown,0,Ch_Case},
   {"CD",C_Unknown,0,Ch_Lower},
   {"CE",C_Unknown,1,Ch_Edt_Style},
   {"CH",C_Unknown,1,Ascii},
   {"CK",C_Unknown,1,Check_Point},
   {"CL",C_Center,1,Center_Line},
   {"CM",C_Unknown,1,Curs_Middle},
   {"CNA",C_Unknown,0,Counter_Add},
   {"CNG",C_Unknown,0,Counter_Get},
   {"CNI",C_Unknown,0,Counter_Increment},
   {"CNP",C_Unknown,0,Counter_Put},
   {"CNS",C_Unknown,0,Counter_Set},
   {"CNV",C_Unknown,0,Counter_Absolute_Value},
   {"CS",C_Unknown,1,Context_Save},
   {"CT",C_Unknown,1,Curs_Top},
   {"CU",C_Unknown,0,Ch_Upper},
   {"CW",C_Unknown,1,Change_Window},
   {"D",C_Delete,0,Del_It},
   {"EAP",C_Delete,0,Del_Append},
   {"EF",C_Unknown,1,Execute_Function},
   {"EK",C_Unknown,0,Execute_Key},
   {"EL",C_Unknown,1,Err_Level},
   {"EM",C_Unknown,1,End_Mark},
   {"ERC",C_Unknown,1,Cancel_Edit_Region},
   {"ERS",C_Unknown,0,Set_Edit_Region},
   {"FD",C_Unknown,0,Def_Function},
   {"FE",C_Unknown,0,File_Edit},
   {"FG",C_Unknown,0,File_Get},
   {"FNL",C_Unknown,1,Font_Larger},
   {"FNS",C_Unknown,1,Font_Smaller},
   {"FS",C_Unknown,0,File_Save},
   {"FT",C_Unknown,0,Fill},
   {"FW",C_Unknown,0,File_Write},
   {"FX",C_Unknown,0,File_Execute},
   {"GAP",C_Unknown,0,Get_Append},
   {"GE",C_Unknown,0,Get},
   {"GR",C_Unknown,1,Get_Ruler},
   {"H",C_Unknown,0,Help},
   {"I",C_Unknown,0,Insert},
   {"KA",C_Unknown,0,Def_Key_Alpha},
   {"KF",C_Unknown,0,Def_Key_Function},
   {"KGA",C_Unknown,0,Def_Key_G_Alpha},
   {"KGK",C_Unknown,0,Def_Key_G_Key},
   {"KK",C_Unknown,0,Def_Key_Key},
   {"KN",C_Unknown,0,Def_Key_Number},
#if defined(VMS) | defined(UNIX)
   {"L0D",C_Unknown,1,Mouse_Outside},
   {"L1D",C_Unknown,1,Mouse_1_Down},
   {"L1U",C_Unknown,1,Mouse_1_Up},
   {"L2D",C_Unknown,1,Mouse_2_Down},
   {"L2U",C_Unknown,1,Mouse_2_Up},
   {"L3D",C_Unknown,1,Mouse_3_Down},
   {"L3U",C_Unknown,1,Mouse_3_Up},
   {"L4D",C_Unknown,1,Mouse_4_Down},
   {"L4U",C_Unknown,1,Mouse_4_Up},
#endif
   {"LD",C_Unknown,1,Ind_Decrease},
   {"LI",C_Unknown,1,Ind_Increase},
   {"LK",C_Unknown,0,Load_Key},
   {"LNE",C_Unknown,0,Ex_Learn},
   {"LNL",C_Unknown,0,Load_Learn},
   {"LNX",C_Unknown,1,End_Learn},
   {"LS",C_Unknown,1,Ind_Set},
   {"M",C_Unknown,1,Mark_Pos},
   {"NL",C_Unknown,1,A_New_Line},
   {"NS",C_Subs,1,Subs_Next},
   {"NWC",C_Unknown,1,Window_Changed},
   {"NWG",C_Unknown,1,Window_Cycle},
   {"NWN",C_Unknown,1,Window_Not_Used},
   {"NWU",C_Unknown,1,Window_Used},
   {"OE",C_Unknown,0,Out_Error},
   {"OL",C_Unknown,1,Open_Line},
   {"OM",C_Unknown,0,Out_Message},
   {"OS",C_Unknown,1,Overstrike_Mode},
   {"OW",C_Unknown,1,Other_Window},
   {"PD",C_Unknown,1,Put_Date},
   {"PL",C_Unknown,0,Past_Load},
   {"POPCH",C_Unknown,1,Pop_Changes},
   {"PR",C_Unknown,0,Print},
   {"PS",C_Unknown,0,Past_Save},
   {"PT",C_Unknown,1,Put_Time},
   {"PUSHCH",C_Unknown,1,Push_Changes},
   {"Q",C_Unknown,1,Quit},
   {"REP",C_Rep,1,Repl},
   {"RF",C_Unknown,1,Refresh},
   {"RK",C_Unknown,0,Redefine_Key},
#if defined(VMS) | defined(UNIX)
   {"RI",C_Unknown,1,Reposition_Input},
#endif
   {"RL",C_Unknown,1,Ruler},
   {"RMCHA",C_Unknown,1,Remove_Char},
   {"RMHEL",C_Unknown,1,Remove_Help},
   {"RMLIN",C_Unknown,1,Remove_Line},
   {"RMPAG",C_Unknown,1,Remove_Page},
   {"RMPAR",C_Unknown,1,Remove_Par},
   {"RMPAS",C_Unknown,1,Remove_Paste},
   {"RMSEA",C_Unknown,1,Remove_Search},
   {"RMSEN",C_Unknown,1,Remove_Sent},
   {"RMSUB",C_Unknown,1,Remove_Subs},
   {"RMUNK",C_Unknown,1,Remove_Unknown},
   {"RMWOR",C_Unknown,1,Remove_Word},
   {"RS",C_Unknown,1,Reset},
   {"RTAB",C_Unknown,0,Remove_Tabs},
   {"RUL",C_Unknown,0,Rul_Load},
   {"RUS",C_Unknown,1,Rul_Save},
   {"SASC",C_Unknown,1,Show_Ascii},
   {"SAVF",C_Unknown,1,Save_Functions},
   {"SAVK",C_Unknown,1,Save_Keys},
   {"SB",C_Unknown,1,Show_Bcast},
   {"SC",C_Unknown,1,Show_Column},
   {"SEL",C_Unknown,1,Select},
   {"SETB",C_Unknown,1,Set_Block},
   {"SETD",C_Unknown,1,Set_Direction},
   {"SETF",C_Unknown,1,Set_Cursor_Free},
   {"SETR",C_Unknown,1,Set_Replace},
   {"SETT",C_Unknown,1,Set_Tab_Expand},
   {"SETU",C_Unknown,1,Set_Case_Sensitive},
   {"SETV",C_Unknown,1,Set_Reg_Search},
   {"SETX",C_Unknown,1,Set_Ins_Defs},
   {"SI",C_Unknown,1,Shift_In},
   {"SK",C_Unknown,0,Sav_Key},
   {"SL",C_Unknown,0,Search_Load},
   {"SO",C_Unknown,1,Shift_Out},
   {"STAB",C_Unknown,0,Set_Tabs},
   {"STRP",C_Unknown,0,Strip_Trailing},
   {"SUB",C_Subs,0,Subs},
   {"SUC",C_Subs,0,Subs_Conf},
   {"SV",C_Unknown,1,Show_Version},
   {"SYQ",C_Unknown,0,Q_Spawn},
   {"SYR",C_Unknown,0,R_Spawn},
   {"SYS",C_Unknown,0,Spawn},
   {"TAB",C_Unknown,1,Tab},
   {"TAD",C_Unknown,0,Adj_Tabs},
   {"TB",C_Unknown,1,Tog_Block},
   {"TC",C_Unknown,1,Tog_Control},
   {"TD",C_Unknown,1,Tog_Direction},
   {"TF",C_Unknown,1,Cursor_Free_Toggle},
   {"TI",C_Unknown,1,Test_Input},
   {"TM",C_Unknown,1,Tog_Marks},
   {"TP",C_Unknown,1,Tog_Cursor},
   {"TR",C_Unknown,1,Tog_Ruler},
   {"TS",C_Unknown,1,Shift_Toggle},
   {"TT",C_Unknown,1,Tab_Toggle},
   {"TU",C_Unknown,1,Case_Toggle},
   {"TV",C_Unknown,1,Tog_Reg_Search},
   {"TX",C_Unknown,1,Ins_Defs_Toggle},
#if defined(XTERM_MOUSE)               
   {"TZ0",C_Unknown,1,xmouse_toggle},  
   {"TZ1",C_Unknown,1,xmouse_handler}, 
#endif
   {"UC",C_Unknown,1,Und_Char},
   {"UL",C_Unknown,1,Und_Line},
   {"UM",C_Undo,1,Undo},
   {"UOFF",C_Unknown,1,Update_Off},
   {"UON",C_Unknown,1,Update_On},
   {"UPAG",C_Unknown,1,Und_Pg},
   {"UPAR",C_Unknown,1,Und_Par},
   {"UPD",C_Unknown,1,Upd_Screen},
   {"USEN",C_Unknown,1,Und_Sen},
   {"USR",C_Unknown,1,Paste},
   {"UU",C_Unknown,1,Und_Unknown},
   {"UW",C_Unknown,1,Und_Word},
   {"VQ",C_Unknown,1,VKS_Quit},		/* Special Quit for VKS */
   {"VE",C_Unknown,1,VKS_Exit},		/* Special Exit for VKS */
   {"WA",C_Unknown,1,Window_Cascade},
   {"WC",C_Unknown,1,Window_Zoom},
   {"WD",C_Unknown,1,Width},
   {"W80",C_Unknown,1,Width80},		/* Switch to 80 chars,  VKS extension */
   {"W132",C_Unknown,1,Width132},	/* Switch to 132 chars, VKS extension */
   {"WFB",C_Unknown,1,Window_Fix_Bottom},
   {"WFL",C_Unknown,1,Window_Fix_Left},
   {"WFR",C_Unknown,1,Window_Fix_Right},
   {"WFT",C_Unknown,1,Window_Fix_Top},
   {"WG",C_Unknown,1,Window_Grow},
   {"WI",C_Unknown,1,Window},
   {"WM",C_Unknown,1,Window_Move},
   {"WP",C_Unknown,1,Window_Position},
   {"WS",C_Unknown,1,Window_Shrink},
   {"WT",C_Unknown,1,Window_Tile},
   {"X",C_Unknown,1,Exit_Sedt},
   {"YN",C_Unknown,0,Query},
   {"Z",C_Unknown,1,Zero}
  };

#define Commands (sizeof(Com_Tab)/sizeof(struct Com_Rec))

/*      Interpret the count argument to a command.
*/
long Fast Get_Count()
{register long Count;
 long Com_Bufsize, Com_Pos;
 Com_Bufsize=Com.Bufsize;
 Com_Pos=Com.Pos;
 if (Com_Char=='$')
 {if (Com_Pos>Com_Bufsize||!isdigit(Com_Char=B_Get(&Com)))
  {Syntax_Error();
   return 0L;
  }
  Com_Pos++;
 }
 Count=Com_Char-'0';
 while (1)
 {if (Com_Pos>Com_Bufsize)
  {Syntax_Error();
   break;
  }
  if (!isdigit(Com_Char=B_Get(&Com)))
   break;
  Com_Pos++;
  Count=Count*10+Com_Char-'0';
 }
 Com_Pos++;
 if (Com_Char=='$')
 while (1)
 {if (Com_Pos>Com_Bufsize)
  {Syntax_Error();
   break;
  }
  if (!isdigit(Com_Char=B_Get(&Com)))
   break;
  Com_Pos++;
 }
 return Count;
}

/*      Parse the contents of the command buffer from position S.
*/
void Fast Com_Parse(long S,int Skipall)
{register long Com_Count;
 long Com_Pos;
 int Sav_Flag, Sav_Char;
 B_Goto(&Com,S);
 while (Com.Pos<=Com.Bufsize)
 {Wr_Count=0;
  Check_CC();
  if (Severe_Error)
   return;
  if (Working_Flag)
   Clr_Working();
  Cur_Direction=' ';
  Direction=Com_Direction;
  Count=1;
  if ((Com_Char=B_Get(&Com))=='+'||Com_Char=='-')
  {Direction=Com_Char=='+';
   Cur_Direction=Com_Char;
   if (Com.Pos>Com.Bufsize)
   {Syntax_Error();
    Skip=1;
    return;
   }
   Com_Char=B_Get(&Com);
  }
  if (isdigit(Com_Char)||Com_Char=='$')
   Count=Get_Count();
  switch (Com_Char)
  {case 0:
   {Error(2,Learned_Input_Txt);
    Severe_Error=1;
    return;
   }
   case ':':
   {Ex_Command();
    break;
   }
   case '@':
   case '+':
   case '-':
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
   {if (Com_Char=='+'||Com_Char=='-')
    {Direction=Com_Char=='+';
     Cur_Direction=Com_Char;
     if (Com.Pos>Com.Bufsize)
     {Syntax_Error();
      Skip=1;
      return;
     }
     Com_Char=B_Get(&Com);
    }
    if (isdigit(Com_Char)||Com_Char=='$')
     Count=Count*Get_Count();
    if (Com_Char=='@')
    {Lst_Cmd=This_Cmd;
     This_Cmd=C_Movecursor;
     Ent_Count=Count;
     Ent_Direction=Direction;
     Ex_Entity();
     if (!Skip)
     {Lst_Cmd=This_Cmd;
      This_Cmd=C_Movecursor;
      if (Ent_Direction)
      {Lst_Char_Move=End_Pos-Beg_Pos;
       Lst_Line_Move=End_Line-Beg_Line;
      } else
      {Lst_Char_Move=Beg_Pos-End_Pos;
       Lst_Line_Move=Beg_Line-End_Line;
      }
      Move_Entity();
      if (Err_Flag)
       This_Cmd=C_Unknown;
     }
    } else
     Syntax_Error();
    break;
   }
   case '.':
    break;
   case '(':
   {Com_Pos=Com.Pos;
    Com_Count=Count;
    if (Com_Count)
    {do
      Com_Parse(Com_Pos,0);
     while (!Err_Flag&&--Com_Count&&!Skip);
    } else
    {do
      Com_Parse(Com_Pos,0);
     while (!Err_Flag&&!Skip);
    }
    break;
   }
   case ')':
    return;
   case '^':
   {if (Com.Pos>Com.Bufsize)
     Syntax_Error(); else
    if (isalpha(Com_Char=B_Get(&Com)))
    {if (islower(Com_Char))
      Com_Char=_toupper(Com_Char);
     if (Com_Char=='C'||Com_Char=='E'||Com_Char=='S'||Com_Char=='Z')
     {if (Com_Char=='C')
       Skip|=(!(Any_Changes[Cur_Buffer]&1)); else
      if (Com_Char=='S')
       Skip|=Points[0]==0; else
      if (Com_Char=='E')
      {if (Com.Pos>Com.Bufsize||B_Get(&Com)!='(')
        Syntax_Error(); else
       {Sav_Flag=Show_Errors;
        Show_Errors=0;
        Com_Parse(Com.Pos,Skip);
        Show_Errors=Sav_Flag;
        Skip=!Err_Flag;
        Err_Flag=0;
       }
      } else
      if (Com_Char=='Z')
       Skip|=!(Current->Bufsize==0&&Outfile==NULL);
      if (!Err_Flag)
      {if (Com.Pos>Com.Bufsize||B_Get(&Com)!='(')
        Syntax_Error(); else
       {if (Skip)
	{Com_Parse(Com.Pos,Skip=1);
	 Skip=0;
	 if (Com_Char=='|')
	  Com_Parse(Com.Pos,Skip=Skipall);
	} else
        {Com_Parse(Com.Pos,Skip=Skipall);
	 if (Com_Char=='|')
	 {Com_Parse(Com.Pos,Skip=1);
	  Skip=0;
         }
	}
       }
      }
     } else
     if (Com_Char=='T')
     {if (Com.Pos>Com.Bufsize)
       Syntax_Error(); else
      {if (islower(Sav_Char=B_Get(&Com)))
        Sav_Char=_toupper(Sav_Char);
       switch (Sav_Char)
       {case 'D':
        {Sav_Flag=Com_Direction;
         break;
        }
        case 'I':
        {Sav_Flag=Replace_Mode;
         break;
        }
        case 'B':
        {Sav_Flag=Blk_Mode;
         break;
        }
        case 'T':
        {Sav_Flag=Tab_Expand;
         break;
        }
        case 'U':
        {Sav_Flag=Case_Sensitive;
         break;
        }
        case 'V':
        {Sav_Flag=Reg_Search;
         break;
        }
        case 'X':
        {Sav_Flag=Ins_Defs;
         break;
        }
        case 'F':
        {Sav_Flag=Cursor_Free;
         break;
        }
        default:
        {Syntax_Error();
         break;
        }
       }
       if (!Err_Flag)
       {if (Com.Pos>Com.Bufsize)
         Syntax_Error(); else
        {Com_Parse(Com.Pos,Skip);
         switch (Sav_Char)
         {case 'D':
          {Com_Direction=Sav_Flag;
           break;
          }
          case 'I':
          {Replace_Mode=Sav_Flag;
           break;
          }
          case 'B':
          {Blk_Mode=Sav_Flag;
           break;
          }
          case 'T':
          {Tab_Expand=Sav_Flag;
           break;
          }
          case 'U':
          {Case_Sensitive=Sav_Flag;
           break;
          }
          case 'V':
          {Reg_Search=Sav_Flag;
           break;
          }
          case 'X':
          {Ins_Defs=Sav_Flag;
           break;
          }
          case 'F':
          {Cursor_Free=Sav_Flag;
           break;
          }
         }
         Hdr_Changes=1;
        }
       }
      }
     } else
     if (Com_Char=='I')
     {if (Com.Pos>Com.Bufsize)
       Syntax_Error(); else
      {if (islower(Sav_Char=B_Get(&Com)))
        Sav_Char=_toupper(Sav_Char);
       switch (Sav_Char)
       {case 'D':
        {Skip|=!Def_Direction;
         break;
        }
        case 'I':
        {Skip|=Replace_Mode;
         break;
        }
        case 'B':
        {Skip|=Blk_Mode;
         break;
        }
        case 'T':
        {Skip|=!Tab_Expand;
         break;
        }
        case 'U':
        {Skip|=Case_Sensitive;
         break;
        }
        case 'V':
        {Skip|=Reg_Search;
         break;
        }
        case 'X':
        {Skip|=Ins_Defs;
         break;
        }
        case 'F':
        {Skip|=Cursor_Free;
         break;
        }
        default:
        {Syntax_Error();
         break;
        }
       }
       if (!Err_Flag)
       {if (Com.Pos>Com.Bufsize)
         Syntax_Error(); else
        {if (B_Get(&Com)!='(')
          Syntax_Error(); else
         {if (Skip)
          {Com_Parse(Com.Pos,Skip=1);
	   Skip=0;
	   if (Com_Char=='|')
	    Com_Parse(Com.Pos,Skip=Skipall);
	  } else
          {Com_Parse(Com.Pos,Skip=Skipall);
	   if (Com_Char=='|')
	   {Com_Parse(Com.Pos,Skip=1);
	    Skip=0;
           }
	  }
         }
         Hdr_Changes=1;
        }
       }
      }
     } else
     if (Com_Char=='L')
     {if (Com.Pos>Com.Bufsize)
       Syntax_Error(); else
      {if (islower(Sav_Char=B_Get(&Com)))
        Sav_Char=_toupper(Sav_Char);
       switch (Sav_Char)
       {case 'B':
        {Skip|=My_Col!=1;
         break;
        }
        case 'E':
        {B_Goto(Current,My_Pos);
         Skip|=!Chk_EL(Current,B_Get(Current));
         break;
        }
        default:
        {Syntax_Error();
         break;
        }
       }
       if (!Err_Flag)
       {if (Com.Pos>Com.Bufsize)
         Syntax_Error(); else
        {if (B_Get(&Com)!='(')
          Syntax_Error(); else
         {if (Skip)
          {Com_Parse(Com.Pos,Skip=1);
	   Skip=0;
	   if (Com_Char=='|')
	    Com_Parse(Com.Pos,Skip=Skipall);
	  } else
          {Com_Parse(Com.Pos,Skip=Skipall);
	   if (Com_Char=='|')
	   {Com_Parse(Com.Pos,Skip=1);
	    Skip=0;
           }
	  }
         }
         Hdr_Changes=1;
        }
       }
      }
     } else
      Syntax_Error();
    } else
     Syntax_Error();
    break;
   }
   case '|':
    return;
   default:
    Syntax_Error();
  }
  if (Err_Flag)
   Skip=Skipall=1;
  Out_Zap();
 }
}

int Fast Check_CC()
{if (CC_Flag&&!Severe_Error)
 {Signal_CC();
  return 1;
 } else
  return 0;
}

void Fast Signal_CC()
{Error(1,Cc_Mess);
 Severe_Error=1;
}

/*      Parse and execute a single command. The current position is just
        after a : in the command buffer.
*/
void Fast Ex_Command()
{register int L=0, Done=0, Command;
 char S[10];
 struct Com_Rec *P;
 while (!Done)
 {if (Com.Pos>Com.Bufsize)
  {Unk_Err();
   return;
  }
  if (L==8)
  {Unk_Err();
   return;
  }
  if (islower(Com_Char=B_Get(&Com)))
   Com_Char=_toupper(Com_Char);
  S[L++]=Com_Char;
  S[L]=0;
  for (Command=0;Command<Commands;Command++)
   {if (S[0]==(*(P=(&Com_Tab[Command]))).Name[0]&&!strcmp(S,(*(P)).Name))
    {Lst_Cmd=This_Cmd;
     This_Cmd=(*P).Type;
     if (!(Skip&&(*P).Skipok))
     {
      (*((*P).Function))();
     }
     Done=1;
     if (Err_Flag)
      This_Cmd=C_Unknown;
     break;
   }
  }
 }
}

/*      Move the current cursor position to the position defined by an
        entity.
*/
void Fast Move_Entity()
{long Sel_Pos;
 if (Err_Flag)
  return;
 if (Ent_Direction)
  {if (E_Type==E_Paste||E_Type==E_NPaste)
    Fill_Screen((int)(Beg_Line-End_Line)<Rel_First?Rel_First:
     (int)(Beg_Line-End_Line),0);
   Move_Forward();
  } else
  {if (E_Type==E_Paste||E_Type==E_NPaste)
    Fill_Screen(0,(int)(End_Line-Beg_Line)>Rel_Last?Rel_Last:
     (int)(End_Line-Beg_Line));
   Move_Back();
  }
 if (Blk_Mode&&(Sel_Pos=Points[0])!=0)
 {if (Sel_Pos<My_Pos)
   Fill_Screen(Rel_First,0); else
   Fill_Screen(0,Rel_Last);
 }
 if (Toofar)
 {Toofar=0;
  Err_Flag=1;
 }
}

void Fast Move_Forward()
/*Moves to entity end*/
{register long Scroll;
 int Curs_Move;
 B_Goto(Current,My_Pos);
 Scroll=End_Line-This_Line;
 Curs_Move=0;
 if (Float_Cursor&&Scroll&&Rel_Last!=Margin) /*We can move the cursor down*/
 {if (Scroll<Rel_Last-Margin+1) /*We won't go beyond the margin position*/
   Curs_Move=(int)Scroll; else
  if (Rel_Last>Margin)
   Curs_Move=Rel_Last-Margin;
  Scroll-=Curs_Move;
  Set_Scroll(1,Scr_Length);
  {register int I;
   I=Curs_Move;
   while (I--)
   {B_Search(Current,'\n',End_Pos);
    My_Pos=Current->Pos;
    My_Line= ++Home;
    Rel_First--;
    Rel_Last--;
   }
  }
  My_Col=1;
 }
 if (!Scroll)
 {if (Points[0]!=0)
  {if (Curs_Move)
   {Goto_This(My_Pos=End_Pos);
    Fill_Screen(-Curs_Move,0);
   } else
    Paint_To_This(My_Pos=End_Pos);
  } else
   Goto_This(My_Pos=End_Pos);
  This_Line=End_Line;
 } else
 if (Scroll<=Rel_Last-Rel_First&&(Max_Scroll?Max_Scroll>=Scroll:1))
 {if (Scl_Regions)
  {while (B_Search(Current,'\n',End_Pos))
   {My_Pos=Current->Pos;
    Opt_Scroll(1,Rel_First,Rel_Last);
    Fill_Screen(Rel_Last,Rel_Last);
    B_Goto(Current,My_Pos);
   }
   B_Goto(Current,My_Pos);
   My_Col=1;
   This_Line=End_Line;
   Goto_This((My_Pos=End_Pos));
  } else
  {B_Goto(Current,My_Pos=End_Pos);
   This_Line=End_Line;
   Backto_This(End_Pos);
   Opt_Scroll((int) Scroll,Rel_First,Rel_Last);
   Fill_Screen((int)(Rel_Last-Scroll+1),Rel_Last);
  }
  if (Points[0]!=0)
  {Scroll+=Curs_Move;
   Fill_Screen(Scroll<=-Rel_First?(int)-Scroll:Rel_First,0);
  }
 } else
 {B_Goto(Current,(My_Pos=End_Pos));
  This_Line=End_Line;
  Backto_This(End_Pos);
  Fill_Screen(Rel_First,Rel_Last);
 }
}

void Fast Move_Back()
/*Moves back to entity beginning*/
{register long Scroll;
 int C, Curs_Move;
 B_Goto(Current,My_Pos);
 Scroll=This_Line-Beg_Line;
 Curs_Move=0;
 if (Float_Cursor&&Scroll&&Rel_First!=-Margin) /*We can move the cursor up first*/
 {if (Scroll< -Rel_First-Margin+1) /*We won't go beyond the margin position*/
   Curs_Move=(int)Scroll; else
  if (Rel_First< -Margin)
   Curs_Move= -Rel_First-Margin;
  Scroll-=Curs_Move;
  Set_Scroll(1,Scr_Length);
  {register int I;
   I=Curs_Move;
   while (I--)
   {B_B_Search(Current,'\n',Beg_Pos);
    My_Pos=Current->Pos;
    My_Line= --Home;
    Rel_First++;
    Rel_Last++;
   }
  }
  if (!Scroll)
  {This_Line=Beg_Line;
   Backto_This(My_Pos=Beg_Pos);
   if (Points[0]!=0&&Curs_Move)
    Fill_Screen(0,Curs_Move);
   return;
  }
 }
 if (Beg_Line<=-Rel_First)
 {Curs_Move=-Rel_First-(int)Beg_Line+1;
  Scroll-=Curs_Move;
  Set_Scroll(1,Scr_Length);
  {register int I;
   I=Curs_Move;
   while (I--)
   {B_B_Search(Current,'\n',Beg_Pos);
    My_Pos=Current->Pos;
    My_Line= --Home;
    Rel_First++;
    Rel_Last++;
   }
  }
  if (!Scroll)
  {This_Line=Beg_Line;
   Backto_This(My_Pos=Beg_Pos);
   if (Points[0]!=0&&Curs_Move)
    Fill_Screen(0,Curs_Move);
   return;
  }
 }
 if (!Scroll)
 {My_Pos=Beg_Pos;
  while (Current->Pos!=My_Pos)
#if defined(UTF8_HACK)
  {if (((C=B_B_Get(Current))>=' '&&C<='~'))
#else
  {if (((C=B_B_Get(Current))>=' '&&C<='~')||(C>160&&C<255))
#endif
    My_Col--; else
   if (C==9)
   {Backto_This(Beg_Pos);
    break;
   } else
#if ! (defined(UNIX) | defined(MPE))
   if (!(C=='\r'&&Chk_EL(Current,C)))
#endif
    My_Col-=Char_Rep(C,NULL);
  }
  if (Points[0]!=0)
   Fill_Rest();
 } else
 if (Scroll<=Rel_Last-Rel_First&&(Max_Scroll?Max_Scroll>=Scroll:1))
 {if (Scl_Regions)
  {while (B_B_Search(Current,'\n',Beg_Pos))
   {My_Pos=Current->Pos;
    Opt_Scroll(-1,Rel_First,Rel_Last);
    Fill_Screen(Rel_First,Rel_First);
    B_Goto(Current,My_Pos);
   }
   B_Goto(Current,My_Pos);
   This_Line=Beg_Line;
   Backto_This((My_Pos=Beg_Pos));
  } else
  {B_Goto(Current,My_Pos=Beg_Pos);
   This_Line=Beg_Line;
   Backto_This(Beg_Pos);
   Opt_Scroll((int) -Scroll,Rel_First,Rel_Last);
   Fill_Screen(Rel_First,(int)(Rel_First+Scroll-1));
  }
  if (Points[0]!=0)
  {Scroll+=Curs_Move;
   Fill_Screen(0,Scroll<=Rel_Last?(int)Scroll:Rel_Last);
  }
 } else
 {B_Goto(Current,(My_Pos=Beg_Pos));
  This_Line=Beg_Line;
  Backto_This(Beg_Pos);
  Fill_Screen(Rel_First,Rel_Last);
 }
}

void Fast Goto_This(long P)
/*Moves forward to P on a line*/
{register int C;
 long This_Pos;
 This_Pos=Current->Pos;
 while (This_Pos++!=P)
 {if (!Chk_EL(Current,C=B_Get(Current)))
   My_Col+=Char_Rep(C,NULL);
 }
}

void Fast Paint_To_This(long P)
/*Moves forward to P on a line and paints as it goes*/
{long This_Pos, Sel_Pos;
 Sel_Pos=Points[0];
 This_Pos=Current->Pos;
 while (This_Pos!=P)
 {if (Blk_Mode)
  {if (This_Pos==Sel_Pos||(My_Col==Sel_Col&&
   ((This_Pos>=Sel_Pos&&This_Pos<=My_Pos)||
    (This_Pos<=Sel_Pos&&This_Pos>=My_Pos))))
   {Cur_Attributes=A_Reverse;
    if (This_Pos==Sel_Pos)
     Sel_Col=My_Col;
   } else
   if (Cur_Attributes!=0)
    Cur_Attributes=0;
  } else
  {if (This_Pos<Sel_Pos)
   {if (Cur_Attributes!=0)
     Cur_Attributes=0;
   } else
   if (Cur_Attributes!=A_Reverse)
    Cur_Attributes=A_Reverse;
  }
  This_Pos++;
  Put_Rep(B_Get(Current));
 }
}

void Fast Backto_This(long P)
/*Moves back to position P on a line*/
{/*Diag("Backto_This:Pos=%D,P=%D",Current->Pos,P);*/
 if (B_B_Search(Current,'\n',(long) 1))
  B_Get(Current);
 My_Col=1;
 Goto_This(P);
}

void Fast B_Save(struct Buffer *B,char *N)
/*Saves a buffer in a file*/
{struct File F;
 register int Out_Count=0;
 register char C;
 Plain_Message(Txt_Saving);
 if (F_open(N,"w",&F)==NULL)
 {Error(2,Cannot_Create_File_Txt);
  Not_End=1;
  return;
 }
 if (Err_Flag)
  return;
 B_Rewind(B);
 B_Goto(B,(long) 1);
 Out_Count=0;
#if defined(VMS)
 while ((B->Pos)<=(B->Bufsize))
 {C=Dsk_Rec[(Out_Count=B_Get_S(B,Dsk_Rec,Blksize))-1];
  if (!Put_Out(&F,Dsk_Rec,Out_Count)||Check_CC())
  {Not_End=1;
#if defined(VMS)
   delete(N);
#endif
   F_close(&F);
   return;
  }
  Out_Count=0;
 }
#else
 Out_Count=0;
 while ((B->Pos)<=(B->Bufsize))
 {C=Dsk_Rec[(Out_Count+=B_Get_S(B,Dsk_Rec+Out_Count,Blksize-Out_Count))-1];
  if (Out_Count==Blksize)
  {if (!Put_Out(&F,Dsk_Rec,Blksize)||Check_CC())
   {Not_End=1;
#if defined(VMS)
    delete(N);
#endif
    F_close(&F);
    return;
   }
   Out_Count=0;
  }
 }
 if (Out_Count!=0)
 {if (!Put_Out(&F,Dsk_Rec,Out_Count))
  {Not_End=1;
#if defined(VMS)
   delete(N);
#endif
   F_close(&F);
   return;
  }
  Out_Count=0;
 }
 if (!Nosub&&C!=26)
  Dsk_Rec[Out_Count++]=C=26;
 if (Doreturn&&C!='\n')
 {
#if ! (defined(UNIX) | defined(MPE))
  Dsk_Rec[Out_Count++]='\r';
#endif
  Dsk_Rec[Out_Count++]='\n';
 }
#endif
 if (Out_Count!=0)
 {if (!Put_Out(&F,Dsk_Rec,Out_Count))
  {Not_End=1;
#if defined(VMS)
   delete(N);
#endif
   F_close(&F);
   return;
  }
 }
 if (F_close(&F)==EOF)
 {Wr_Error();
#if defined(VMS)
  delete(N);
#endif
  Not_End=1;
  return;
 }
 Plain_Message("");
}

int Fast Put_Out(struct File *F,char *B,int L)
{if (F_write(B,1,L,F)==EOF)
 {Wr_Error();
  F_close(F);
  return 0;
 }
 return 1;
}
   
int Fast Get_Entity()
/*Parses an entity after a command*/
{Ent_Direction=Def_Direction;
 Cur_Direction=' ';
 Ent_Count=1;
 if (Com.Pos>Com.Bufsize)
 {Syntax_Error();
  return(0);
 }
 Com_Char=B_Get(&Com);
 while (1)
 {if (Com_Char=='+'||Com_Char=='-')
  {Ent_Direction=Com_Char=='+';
   Cur_Direction=Com_Char;
   if (Com.Pos>Com.Bufsize)
   {Syntax_Error();
    return(0);
   }
   Com_Char=B_Get(&Com);
  }
  if (isdigit(Com_Char)||Com_Char=='$')
   Ent_Count=Get_Count(); else
  if (Com_Char=='@')
  {if (!Err_Flag||Skip)
    Ex_Entity();
   return(Err_Flag?0:1);
  } else
  {Syntax_Error();
   return(0);
  }
 }
}

void Fast Adj_Points(long N,long W)
/*Adjusts all points into the current buffer*/
{register long *P, Q;
 /*Diag("Adj_Points:N=%D",N);*/
 P=Points-1;
 {register int I;
  for (I=0;I<=Maxpoints+3;I++)
  {if ((Q=(*(++P)))>W)
    (*P)=Q+N>=W?Q+N:W;
  }
 }
 if ((Q=(*(++P)))>=W) /* End of region */
  (*P)=Q+N>=W?Q+N:W;
 P=End_Points;
 {register int I;
  for (I=1;I<=Maxpoints;I++)
  {if ((Q=(*(++P)))>W)
    (*P)=Q+N>=W?Q+N:W;
  }
 }
}
