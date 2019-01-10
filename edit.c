/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987,1988*/
#include "proto.h"

void Fast Reset_Width()
{Out_Zap();
 Reset_Screen();
 Set_Cursor(1,1);
#if defined(VMS) | defined(UNIX)
 if (Org_Width>80)
 {if (Scr_Width == 80)
   Wide_Screen ();
 } else
 if (Org_Width == 80) 
 {if (Scr_Width>80)
   Narrow_Screen ();
 }
#endif
#if defined(DOS) & !(defined(Windows) | defined(TV)) & defined(PC100)
 if (Scr_Width==132)
  Narrow_Screen();
#endif
 Reset_Term();
}

/*      This is the main editor routine. It initializes the edit session and
        then keeps reading and executing commands until the variable Not_End
	is set to false.
*/
void Fast Editor()
/*Main Editor function*/
{struct File F;
 char S[80];
 Cur_Buffer=0;
 /*Opt_Cursor(Home,(long)1);*/
 Out_Zap();
 Update_Off();
 if (Infile!=NULL)
 {
#if !((defined(DOS) & defined(Windows)) | (defined(ATARI) & defined(GEM)) | defined(TV))
 } else
 if (Remember&&!File_Arguments)
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(WIN32)
 {if ((Find_Env("sedt.rem",S,"SEDTP")!=NULL&&F_open(S,"r",&F)!=NULL)||
   (Find_Env("sedt.rem",S,"SEDT")!=NULL&&F_open(S,"r",&F)!=NULL)||
#if defined(UNIX)
   (Find_Env("sedt.rem",S,"HOME")!=NULL&&F_open(S,"r",&F)!=NULL)||
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
   (Find_From("sedt.rem",S)!=NULL&&F_open(S,"r",&F)!=NULL)||
#endif
   (Find_Path("sedt.rem",S)!=NULL&&F_open(S,"r",&F)!=NULL)||
   F_open("sedt.rem","r",&F)!=NULL)
#endif
#if defined(VMS)
 {if (F_open("sedt.rem","r",&F)!=NULL||
	F_open(Find_Dir("sedt.rem",S),"r",&F)!=NULL||
	F_open(Find_Lib("sedt.rem",S),"r",&F)!=NULL)
#endif
#if defined(MPE)
 {if (F_open("rem.sed","r",&F)!=NULL)
#endif
  {
#if defined(UNIX) | defined(DOS) | defined(WIN32)
	struct stat File_Stat;
#endif
	F_gets(Infile=Outfile=New_File,512,&F);
	Read_File(Outfile);
#if defined (UNIX)
	if (stat(New_File,&File_Stat)!=0)
	{Plain_Message(No_Stat_Txt);
	} else
	if (Reservation)
	{File_Reserved[Cur_Buffer]=0;
	 if ((File_Stat.st_mode&(S_IWUSR+S_IWGRP+S_IWOTH))==0)
	 {if (Yes_No(File_Reserved_Txt)==1)
	  {Read_Only[Cur_Buffer]=1;
	  } else
	  {return;
	  }
	 }
	 {if (File_Stat.st_uid==My_UID)
	  {if (chmod(New_File,File_Stat.st_mode&~(S_IWUSR+S_IWGRP+S_IWOTH))!=0)
		 Plain_Message(No_Protection_Change_Txt); else
		{Plain_Message(Reserved_Txt);
		 File_Reserved[Cur_Buffer]=1;
		 File_Protection[Cur_Buffer]=File_Stat.st_mode;
		}
	  } else
	  if (File_Stat.st_gid==My_GID)
	  {Plain_Message(Group_Txt);
	  } else
	  {Plain_Message(Other_Txt);
	  }
	 }
	}
#endif
#if defined (DOS)  | defined(WIN32)
	if (stat(New_File,&File_Stat)!=0)
	{Plain_Message(No_Stat_Txt);
	} else
	if (Reservation)
	{File_Reserved[Cur_Buffer]=0;
	 if ((File_Stat.st_mode&(S_IWRITE))==0)
	 {if (Yes_No(File_Reserved_Txt)==1)
	  {Read_Only[Cur_Buffer]=1;
	  } else
	  {return;
	  }
	 }
	 {if (chmod(New_File,File_Stat.st_mode&~(S_IWRITE))!=0)
		Plain_Message(No_Protection_Change_Txt); else
	  {Plain_Message(Reserved_Txt);
		File_Reserved[Cur_Buffer]=1;
		File_Protection[Cur_Buffer]=File_Stat.st_mode;
	  }
	 }
	}
#endif
	Update_Flag=1;
	W_D_Border();
	Update_Flag=0;
	Auto_Saved=0;
	if (!Err_Flag&&Save_Context)
	 Read_Context();
	F_close(&F);
  }
#endif
 }
 S_Header();
 if (Show_Ruler)
  S_Ruler();
 Fill_Screen(Rel_First,Rel_Last);
#if defined(DOS) & defined(TV)
 TV_Draw_OK();
#endif
 if (First_Line!=1)
 {Init_Entity();
  Ent_Count=First_Line;
  Abs_Line();
  if (!Err_Flag)
   Move_Entity();
 }
 if (Conf_Error)
  Error(2,Conf_Synt_Txt); else
  Plain_Message(Copyright_Txt);
 Err_Flag=0;
 if (strlen(Init_File)!=0)
 {
#if defined(MPE)
  if (F_open(Init_File,"r",&F)!=NULL)
#endif
#if defined(VMS)
  if (F_open(Init_File,"r",&F)!=NULL||
   F_open(Find_Dir(Init_File,S),"r",&F)!=NULL||
   F_open(Find_Lib(Init_File,S),"r",&F)!=NULL)
#endif
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(UNIX) | defined(OS2) | defined(WIN32)
  if (F_open(Init_File,"r",&F)!=NULL||
   (Find_Env(Init_File,S,"SEDTP")!=NULL&&F_open(S,"r",&F)!=NULL)||
   (Find_Env(Init_File,S,"SEDT")!=NULL&&F_open(S,"r",&F)!=NULL)||
#if defined(UNIX)
   (Find_Env(Init_File,S,"HOME")!=NULL&&F_open(S,"r",&F)!=NULL)||
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
   (Find_From(Init_File,S)!=NULL&&F_open(S,"r",&F)!=NULL)||
#endif
   (Find_Path(Init_File,S)!=NULL&&F_open(S,"r",&F)!=NULL))
#endif
  {Ex_File(&F);
   if (Not_End)
    S_Header();
  } else
   Error(2,No_Init_Txt,Init_File);
 } else
 {
#if defined(MPE)
  if (F_open("com.sed","r",&F)!=NULL)
#endif
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(UNIX) | defined(OS2) | defined(WIN32)
  if (F_open("sedtcom.txt","r",&F)!=NULL||
   (Find_Env("sedtcom.txt",S,"SEDTP")!=NULL&&F_open(S,"r",&F)!=NULL)||
   (Find_Env("sedtcom.txt",S,"SEDT")!=NULL&&F_open(S,"r",&F)!=NULL)
#if defined(UNIX)
   ||(Find_Env("sedtcom.txt",S,"HOME")!=NULL&&F_open(S,"r",&F)!=NULL))
#else
   )
#endif
#endif
#if defined(VMS)
  if (F_open("sedtcom$txt","r",&F)!=NULL||	/* 290a */
   F_open("sedtcom.txt","r",&F)!=NULL||
   F_open(Find_Dir("sedtcom.txt",S),"r",&F)!=NULL||
   F_open(Find_Lib("sedtcom.txt",S),"r",&F)!=NULL)
#endif
  {Ex_File(&F);
   if (Not_End)
    S_Header();
  }
 }
 while (Not_End)
 {Do_Command();
  if (!Not_End)
  {
#if defined(VMS)
   if (Parent_PID!=0)
   {if (Gotmouse)	/* 290a */
     Disable_Mouse();
    T$Cancel();
    Dump_Messages();
    if (Lib$Attach(&Parent_PID)==1)
    {Not_End=1;
     T$Init();
     Set_Cursor(1,1);
     if (Gotmouse)	/* 290a */
      Enable_Mouse();
     Refresh();
    }
   }
#endif
  }
 }
}

#if defined(VMS)
Dump_Messages()
{if (Mbx_Count>0)
 {register int I;
  for (I=0;I<Mbx_Count;I++)
   printf("%s\n",Mbx_Mess[I]);
  Mbx_Count=0;
 }
}
#endif

/*      Read the current ruler from the ruler definition file given in C.
*/
void Fast Readruler(char C)
{char S1[16], S2[64];
 struct File F;
#if defined(MPE)
 strcpy(S1,"rulern.sed");
#else
 strcpy(S1,"rulern.txt");
#endif
 S1[5]=C;
#if defined(MPE)
 if (F_open(S1,"r",&F)==NULL)
#endif
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(UNIX) | defined(OS2) | defined(WIN32)
 if (F_open(S1,"r",&F)==NULL&&
  (Find_Env(S1,S2,"SEDTP")==NULL||F_open(S2,"r",&F)==NULL)&&
  (Find_Env(S1,S2,"SEDT")==NULL||F_open(S2,"r",&F)==NULL)&&
#if defined(UNIX)
  (Find_Env(S1,S2,"HOME")==NULL||F_open(S2,"r",&F)==NULL)&&
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
  (Find_From(S1,S2)==NULL||F_open(S2,"r",&F)==NULL)&&
#endif
  (Find_Path(S1,S2)==NULL||F_open(S2,"r",&F)==NULL))
#endif
#if defined(VMS)
 if (F_open(S1,"r",&F)==NULL&&
  F_open(Find_Dir(S1,S2),"r",&F)==NULL&&
  F_open(Find_Lib(S1,S2),"r",&F)==NULL)
#endif
  Error(2,No_Ruler_Txt,S1); else
 {{register int I;
   for (I=1;I<=512;I++)
    Tabs[I]=0;
  }
  Left_Margin=Word_Wrap=1;
  Right_Margin=1;
  Auto_Margin=0;
  while (1)
  {register long I;
   I=0;
   if ((C=F_getc(&F))==EOF)
    break;
   {register char T;
    while (isdigit(T=F_getc(&F)))
     I=I*10+T-'0';
   }
   switch (C)
   {case 'L':
    {Left_Margin=I;
     break;
    }
    case 'W':
    {Word_Wrap=I;
     Auto_Margin=0;
     break;
    }
    case 'R':
    {Right_Margin=I;
     Justify=0;
     break;
    }
    case 'J':
    {Right_Margin=I;
     Justify=1;
     break;
    }
    case 'I':
    {Indent=I;
     break;
    }
    case 'T':
    {Tabs[I]=1;
     break;
    }
    case 'A':
    {Word_Wrap=I;
     Auto_Margin=1;
     break;
    }
    case 0:
    case ' ':
    case '\r':
    case '\n':
    {F_close(&F);
     return;
    }
    default:
    {Error(2,Ill_Ruler_Txt);
     break;
    }
   }
  }
  F_close(&F);
 }
}

/*      Fill the screen from the current cursor position until either the
        end of the buffer or a line terminator is seen.
*/
void Fast Fill_Rest()
{long Sav_Col, This_Pos, End_Pos, Sel_Pos, Next_Mark_Start, Next_Mark_End;
 int C, Sav_Attributes;
 Set_Region();
 Sav_Col=My_Col;
 End_Pos=Current->Bufsize;
 B_Goto(Current,This_Pos=My_Pos);
 if ((Sel_Pos=Points[0])!=0)
 {if (Blk_Mode)
  {if ((My_Col>=Sel_Col&&My_Col<=Sav_Col)&&
    This_Pos>=Sel_Pos&&This_Pos<=My_Pos)
   {Cur_Attributes=A_Reverse;
    if (This_Pos==Sel_Pos)
     Sel_Col=My_Col;
   } else
   if (Cur_Attributes!=0)
    Cur_Attributes=0;
  } else
  {if ((This_Pos>=Sel_Pos&&This_Pos<=My_Pos)||
    (This_Pos<=Sel_Pos&&This_Pos>=My_Pos))
   {if (Cur_Attributes!=A_Reverse)
     Cur_Attributes=A_Reverse;
   } else
   if (Cur_Attributes!=0)
    Cur_Attributes=0;
  }
 } else
 if (Cur_Attributes!=0)
  Cur_Attributes=0;
 Next_Mark_Start=Next_Mark_End=0;
 {register int I;
  for (I=1;I<=Maxpoints;I++)
  if (Points[I]!=0)
  {if (Points[I]<=This_Pos&&End_Points[I]>=This_Pos&&
    (End_Points[I]>Next_Mark_End||Next_Mark_Start>This_Pos))
   {Next_Mark_Start=Points[I];
    Next_Mark_End=End_Points[I];
   } else
   if (Points[I]>This_Pos&&(Points[I]<Next_Mark_Start||Next_Mark_Start==0))
   {Next_Mark_Start=Points[I];
    Next_Mark_End=End_Points[I];
   }
  }
 }
 if (This_Pos>=Next_Mark_Start&&This_Pos<=Next_Mark_End)
  Cur_Attributes|=A_Bold;
 while (1)
 {if (Sel_Pos!=0)
  {if (Blk_Mode)
   {if ((My_Col>=Sel_Col&&My_Col<=Sav_Col)&&
     This_Pos>=Sel_Pos&&This_Pos<=My_Pos)
    {Cur_Attributes=A_Reverse;
     if (This_Pos==Sel_Pos)
      Sel_Col=My_Col;
    } else
    if (Cur_Attributes!=0)
     Cur_Attributes=0;
   } else
   {if (This_Pos==My_Pos)
     Cur_Attributes=Sel_Pos<=My_Pos?0:A_Reverse; else
    if (This_Pos==Sel_Pos)
     Cur_Attributes=Sel_Pos<=My_Pos?A_Reverse:0;
   }
  }
  if (This_Pos==Next_Mark_End)
  {Next_Mark_Start=Next_Mark_End=0;
   {register int I;
    for (I=1;I<=Maxpoints;I++)
    if (Points[I]!=0)
    {if (Points[I]<=This_Pos&&End_Points[I]>=This_Pos&&
      (End_Points[I]>Next_Mark_End||Next_Mark_Start>This_Pos))
     {Next_Mark_Start=Points[I];
      Next_Mark_End=End_Points[I];
     } else
     if (Points[I]>This_Pos&&(Points[I]<Next_Mark_Start||Next_Mark_Start==0))
     {Next_Mark_Start=Points[I];
      Next_Mark_End=End_Points[I];
     }
    }
   }
  }
  if (This_Pos>=Next_Mark_Start&&This_Pos<=Next_Mark_End)
   Cur_Attributes|=A_Bold; else
   Cur_Attributes&=~A_Bold;
  if (This_Pos>End_Pos)
  {if (Show_Marks)
   {Sav_Attributes=Cur_Attributes;
    Cur_Attributes=A_Reverse+A_Bold;
    Opt_String(Txt_Eof);
    Cur_Attributes=Sav_Attributes;
   }
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
   if (Lst_Col[My_Line+Top_Row]>=My_Col-Scr_Shift)
#else
   if (Lst_Col[My_Line]>=My_Col-Scr_Shift)
#endif
    Opt_Erase_Line();
   break;
  }
  This_Pos++;
#if defined(UNIX) | defined(MPE)
  if ((C=B_Get(Current))=='\n')
  {if (Lst_Col[My_Line+Top_Row]>=My_Col-Scr_Shift)
    Opt_Erase_Line();
   break;
  } else
#else
  if ((C=B_Get(Current))=='\n')
  {Put_Rep(C);
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
   if (Lst_Col[My_Line+Top_Row]>=My_Col-Scr_Shift)
#else
   if (Lst_Col[My_Line]>=My_Col-Scr_Shift)
#endif
    Opt_Erase_Line();
   break;
  } else
  if (C=='\r')
  {if (Current->Pos<=Current->Bufsize&&B_Get(Current)=='\n')
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
   {if (Lst_Col[My_Line+Top_Row]>=My_Col-Scr_Shift)
#else
   {if (Lst_Col[My_Line]>=My_Col-Scr_Shift)
#endif
     Opt_Erase_Line();
    break;
   } else
   {B_Goto(Current,Current->Pos-1);
    Put_Rep(C);
   }
  } else
#endif
  if (My_Col-Scr_Shift<=Scr_Width+1)
   Put_Rep(C); else
   break;
 }
 My_Col=Sav_Col;
}

/*      Fill the screen from First lines relative to the current line to
        Last lines relative to the current line. Last must be greater than
        or equal to First but they can both be negative, zero of positive.
*/
void Fast Fill_Screen(int First,int Last)
/*Fills the screen with data from the buffer*/
{register int Howfar, C;
 int Sav_Line, In_Buffer;
 char Sav_Attributes;
 long First_Pos, This_Pos, Sel_Pos, End_Pos, Sav_Col, Next_Mark_Start,
  Next_Mark_End;
 /*Diag("Fill_Screen:Top_Row=%d,Left_Col=%d,Scr_Length=%d,Scr_Width=%d",
  Top_Row,Left_Col,Scr_Length,Scr_Width);*/
 Set_Region();
 End_Pos=Current->Bufsize;
 Sav_Attributes=Cur_Attributes;
 B_Goto(Current,My_Pos);
 This_Pos=Current->Pos;
 Sel_Pos=Points[0];
 In_Buffer=1;
 if (First==0)
 {Howfar=0;
  First_Pos=B_B_Search(Current,'\n',1L)?Current->Pos+1:1;
 } else
 if (First<0)
 {Howfar=0;
  while (1)
  {if (!B_B_Search(Current,'\n',1L))
   {First_Pos=1;
    break;
   }
   if (Howfar==First)
   {First_Pos=Current->Pos+1;
    break;
   }
   Howfar--;
  }
 } else
 if (First>0)
 {Howfar=0;
  while (Howfar<First)
  {if (!B_Search(Current,'\n',End_Pos+1))
   {Howfar++;
    In_Buffer=0;
    First_Pos=Current->Pos;
    break;
   }
   Howfar++;
   First_Pos=Current->Pos;
  }
 }
 Sav_Line=My_Line;
 Sav_Col=My_Col;
 My_Col=1;
 if (First<Howfar)
 {for (My_Line=Home+First;My_Line<Home+Howfar;My_Line++)
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
   if (Lst_Col[My_Line+Top_Row]>0)
#else
   if (Lst_Col[My_Line]>0)
#endif
    Opt_Erase_Line();
 }
 if (First_Pos==1&&First<Howfar)
 {if (Show_Marks)
  {Opt_Cursor(Home+Howfar-1,(long)1);
   Cur_Attributes=A_Reverse+A_Bold;
   Opt_String(Txt_Bof);
   Cur_Attributes=Sav_Attributes;
  }
 }
 Opt_Cursor(Home+Howfar,(long)1);
 B_Goto(Current,First_Pos);
 This_Pos=First_Pos;
 if (Sel_Pos!=0)
 {if (Blk_Mode)
  {if ((My_Col>=Sel_Col&&My_Col<=Sav_Col)&&
    This_Pos>=Sel_Pos&&This_Pos<=My_Pos)
   {Cur_Attributes=A_Reverse;
    if (This_Pos==Sel_Pos)
     Sel_Col=My_Col;
   } else
   if (Cur_Attributes!=0)
    Cur_Attributes=0;
  } else
  {if ((This_Pos>=Sel_Pos&&This_Pos<=My_Pos)||
    (This_Pos<=Sel_Pos&&This_Pos>=My_Pos))
   {if (Cur_Attributes!=A_Reverse)
     Cur_Attributes=A_Reverse;
   } else
   if (Cur_Attributes!=0)
    Cur_Attributes=0;
  }
 } else
 if (Cur_Attributes!=0)
  Cur_Attributes=0;
 Next_Mark_Start=Next_Mark_End=0;
 {register int I;
  for (I=1;I<=Maxpoints;I++)
  if (Points[I]!=0)
  {if (Points[I]<=This_Pos&&End_Points[I]>=This_Pos&&
    (End_Points[I]>Next_Mark_End||Next_Mark_Start>This_Pos))
   {Next_Mark_Start=Points[I];
    Next_Mark_End=End_Points[I];
   } else
   if (Points[I]>This_Pos&&(Points[I]<Next_Mark_Start||Next_Mark_Start==0))
   {Next_Mark_Start=Points[I];
    Next_Mark_End=End_Points[I];
   }
  }
 }
 if (This_Pos>=Next_Mark_Start&&This_Pos<=Next_Mark_End)
  Cur_Attributes|=A_Bold;
 while (Howfar<=Last)
 {if (Sel_Pos!=0)
  {if (Blk_Mode)
   {if ((My_Col>=Sel_Col&&My_Col<=Sav_Col)&&
     This_Pos>=Sel_Pos&&This_Pos<=My_Pos)
    {Cur_Attributes=A_Reverse;
     if (This_Pos==Sel_Pos)
      Sel_Col=My_Col;
    } else
    if (Cur_Attributes!=0)
     Cur_Attributes=0;
   } else
   {if (This_Pos==My_Pos)
     Cur_Attributes=Sel_Pos<=My_Pos?0:A_Reverse; else
    if (This_Pos==Sel_Pos)
     Cur_Attributes=Sel_Pos<=My_Pos?A_Reverse:0;
   }
  }
  if (This_Pos==Next_Mark_End)
  {Next_Mark_Start=Next_Mark_End=0;
   {register int I;
    for (I=1;I<=Maxpoints;I++)
    if (Points[I]!=0)
    {if (Points[I]<=This_Pos&&End_Points[I]>=This_Pos&&
      (End_Points[I]>Next_Mark_End||Next_Mark_Start>This_Pos))
     {Next_Mark_Start=Points[I];
      Next_Mark_End=End_Points[I];
     } else
     if (Points[I]>This_Pos&&(Points[I]<Next_Mark_Start||Next_Mark_Start==0))
     {Next_Mark_Start=Points[I];
      Next_Mark_End=End_Points[I];
     }
    }
   }
  }
  if (This_Pos>=Next_Mark_Start&&This_Pos<=Next_Mark_End)
   Cur_Attributes|=A_Bold; else
   Cur_Attributes&=~A_Bold;
  if (This_Pos>End_Pos)
  {if (In_Buffer)
   {if (Show_Marks)
    {Cur_Attributes=A_Reverse+A_Bold;
     Opt_String(Txt_Eof);
     Cur_Attributes=Sav_Attributes;
    }
   }
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
   if (Lst_Col[My_Line+Top_Row]>=My_Col-Scr_Shift)
#else
   if (Lst_Col[My_Line]>=My_Col-Scr_Shift)
#endif
    Opt_Erase_Line();
   My_Col=1;
   for (My_Line++;My_Line<=Home+Last;My_Line++)
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
    if (Lst_Col[My_Line+Top_Row]>0)
#else
    if (Lst_Col[My_Line]>0)
#endif
     Opt_Erase_Line();
   break;
  }
  This_Pos++;
#if defined(UNIX) | defined(MPE)
  if ((C=B_Get(Current))=='\n')
  {Howfar++;
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
   if (Lst_Col[My_Line+Top_Row]>=My_Col-Scr_Shift)
#else
   if (Lst_Col[My_Line]>=My_Col-Scr_Shift)
#endif
    Opt_Erase_Line();
   Opt_Cursor(My_Line+1,(long)1);
  } else
#else
  if ((C=B_Get(Current))=='\n')
  {Howfar++;
   Put_Rep('\n');
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
   if (Lst_Col[My_Line+Top_Row]>=My_Col-Scr_Shift)
#else
   if (Lst_Col[My_Line]>=My_Col-Scr_Shift)
#endif
    Opt_Erase_Line();
   Opt_Cursor(My_Line+1,(long)1);
  } else
  if (C=='\r')
  {if (Current->Pos<=Current->Bufsize&&(C=B_Get(Current))=='\n')
   {This_Pos++;
    Howfar++;
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
    if (Lst_Col[My_Line+Top_Row]>=My_Col-Scr_Shift)
#else
    if (Lst_Col[My_Line]>=My_Col-Scr_Shift)
#endif
     Opt_Erase_Line();
    Opt_Cursor(My_Line+1,(long)1);
   } else
   {B_Goto(Current,Current->Pos-1);
    Put_Rep('\r');
   }
  } else
#endif
  if (My_Col-Scr_Shift<=Scr_Width+1)
   Put_Rep(C);
 }
 Cur_Attributes=Sav_Attributes;
 Opt_Cursor(Sav_Line,Sav_Col);
}

/*      Load a file into the current buffer
*/
void Fast Read_File(char *N)
/*Reads the file named N into buffer B*/
{
#if defined(VMS)
 char Autofile[80];
#else
 char Autofile[512];
#endif
 FILE *F;
 if (Auto_Saved)
  Rem_Autosave();
 Filnam(N,Autofile);
#if defined(UNIX)
 strcat(Autofile,".aus");
#else
 strcat(Autofile,".AUS");
#endif
 if ((F=fopen(Autofile,"r"))!=NULL)
 {fclose(F);
  if (Yes_No(Txt_Got_Auto))
  {Include_File(Autofile,Txt_New_File);
   Auto_Saved=1;
  } else
   Include_File(N,Txt_New_File);
 } else
 {if (Err_Flag)
   return;
  Include_File(N,Txt_New_File);
 }
#if defined(VMS)
 Strip_Version();
#endif
#if defined(ATARI) & defined(GEM)
 GEM_Name(top_window,Outfile);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Win_Name(Outfile);
#endif
#if defined(DOS) & defined(TV)
 TV_Name(Outfile);
#endif
 W_D_Border();
}

/*      Remove the file type from the file name given in N and return it
        in T.
*/
void Fast Filnam(char *N,char *T)
{char *S;
 S=T;
 {register char C;
  while((C= *(N++))!=0)
  {
#if !defined(UNIX)
   if (islower(C))
    C=_toupper(C);
#endif
   *(S++)=C;
  }
 }
 *S=0;
 S=T;
#if defined(UNIX)
 while ((T=strchr(S,'/'))!=NULL)
  S=T+1;
 if ((T=strchr(S,'.'))!=NULL&&T!=S)
 {S=T+1;
  while ((T=strchr(S,'.'))!=NULL)
   S=T+1;
  S--;
 } else
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 while ((T=strchr(S,':'))!=NULL)
  S=T+1;
 while ((T=strchr(S,'\\'))!=NULL)
  S=T+1;
 while ((T=strchr(S,'/'))!=NULL)
  S=T+1;
 if ((T=strchr(S,'.'))!=NULL)
  S=T; else
#endif
#if defined(ATARI) 
 while ((T=index(S,':'))!=NULL)
  S=T+1;
 while ((T=index(S,'\\'))!=NULL)
  S=T+1;
 if ((T=index(S,'.'))!=NULL)
  S=T; else
#endif
#if defined(VMS)
 while ((T=strchr(S,':'))!=NULL)
  S=T+1;
 if ((T=strchr(S,']'))!=NULL)
  S=T+1; else
 if ((T=strchr(S,'>'))!=NULL)
  S=T+1;
 if ((T=strchr(S,'.'))!=NULL)
  S=T; else
#endif
  while (*S)
   S++;
 (*S)=0;
}

/*      Find the file type from the file name given in N and return it
        in T.
*/
void Fast Filtyp(char *N,char *T)
{char *S, *U, C;
 S=N;
#if defined(UNIX)
 while ((U=strchr(S,'/'))!=NULL)
  S=U+1; 
 if ((U=strchr(S,'.'))!=NULL)
 {S=U+1; 
  while ((U=strchr(S,'.'))!=NULL)
   S=U+1;
 } else
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 while ((U=strchr(S,':'))!=NULL)
  S=U+1;
 while ((U=strchr(S,'\\'))!=NULL)
  S=U+1;
 while ((U=strchr(S,'/'))!=NULL)
  S=U+1;
 if ((U=strchr(S,'.'))!=NULL)
  S=U+1; else
#endif
#if defined(ATARI) 
 while ((U=index(S,':'))!=NULL)
  S=U+1;
 while ((U=index(S,'\\'))!=NULL)
  S=U+1;
 if ((U=index(S,'.'))!=NULL)
  S=U+1; else
#endif
#if defined(VMS)
 while ((U=strchr(S,':'))!=NULL)
  S=U+1;
 if ((U=strchr(S,']'))!=NULL)
  S=U+1;
 if ((U=strchr(S,'.'))!=NULL)
  S=U+1; else
#endif
  while (*S)
   S++;
 while((C= *(S++))!=0)
 {
#if !defined(UNIX)
  if (islower(C))
   C=_toupper(C);
#endif
  *(T++)=C;
 }
 *T=0;
}

/*      Find the file name  without directory from the file name given in
	N \and return it in T.
*/
void Fast Filjnam(char *N,char *T)
{char *S, C;
 S=N;
#if defined(UNIX)
 while ((N=strchr(S,'/'))!=NULL)
  S=N+1; 
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 while ((N=strchr(S,':'))!=NULL)
  S=N+1;
 while ((N=strchr(S,'\\'))!=NULL)
  S=N+1;
 while ((N=strchr(S,'/'))!=NULL)
  S=T+1;
#endif
#if defined(ATARI) 
 while ((N=index(S,':'))!=NULL)
  S=N+1;
 while ((N=index(S,'\\'))!=NULL)
  S=N+1;
#endif
#if defined(VMS)
 while ((N=strchr(S,':'))!=NULL)
  S=N+1;
 if ((N=strchr(S,']'))!=NULL)
  S=N+1;
#endif
 while((C= *(S++))!=0)
 {if (islower(C))
   C=_toupper(C);
  *(T++)=C;
 }
 *T=0;
}

/*      Find the device and directory from the file name given in N and return
        it in T.
*/
void Fast Fildir(char *N,char *T)
{char *S;
 S=T;
 {register char C;
  while((C= *(N++))!=0)
  {
#if !defined(UNIX)
   if (islower(C))
    C=_toupper(C);
#endif
   *(S++)=C;
  }
 }
 *S=0;
 S=T;
#if defined(UNIX)
 while ((T=strchr(S,'/'))!=NULL)
  S=T+1;
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 while ((T=strchr(S,':'))!=NULL)
  S=T+1;
 while ((T=strchr(S,'\\'))!=NULL)
  S=T+1;
 while ((T=strchr(S,'/'))!=NULL)
  S=T+1;
#endif
#if defined(ATARI) 
 while ((T=index(S,':'))!=NULL)
  S=T+1;
 while ((T=index(S,'\\'))!=NULL)
  S=T+1;
#endif
#if defined(VMS)
 while ((T=strchr(S,':'))!=NULL)
  S=T+1;
 if ((T=strchr(S,']'))!=NULL)
  S=T+1;
#endif
 (*S)=0;
}

/*      Read the file with the name given in N into the current buffer at the 
	current cursor position.
*/
void Fast Include_File(char *N,char *E)
/*Reads the file named N into buffer B*/
{int In_Count=0;
 struct File F;
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(OS2) | defined(WIN32)
 struct stat S_Buffer;
#endif
 long Pad_Pos, Included;
 Plain_Message(Txt_Loading);
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 if (stat(N,&S_Buffer)==0&&S_Buffer.st_mode&S_IFDIR)
 {Error(2,Dir_Load_Txt);
  return;
 }
#endif
#if defined(UNIX)
 if (stat(N,&S_Buffer)==0&&S_Buffer.st_mode&0040000)
 {Error(2,Dir_Load_Txt);
  return;
 }
 if (stat(N,&S_Buffer)==0&&S_Buffer.st_mode&0070000)
 {Error(2,Spec_Load_Txt);
  return;
 }
#endif
 if (Err_Flag)
  return;
 if (F_open(N,"r",&F)==NULL)
 {Error(4,E);
  return;
 }
 Included=0;
 if (Current->Pos>Current->Bufsize)
 {while ((In_Count=F_read(Dsk_Rec,1,Blksize,&F))!=0)
  {B_Put_S(Current,Dsk_Rec,In_Count);
   Included+=In_Count;
  }
 } else
 {B_Goto(&Pad,Pad.Bufsize);
  Pad_Pos=Pad.Pos;
  while ((In_Count=F_read(Dsk_Rec,1,Blksize,&F))!=0)
  {B_Put_S(&Pad,Dsk_Rec,In_Count);
   Included+=In_Count;
  }
  B_Goto(Current,My_Pos);
  B_Goto(&Pad,Pad_Pos);
  B_Copy(&Pad,Current,Included);
  B_Goto(&Pad,Pad_Pos);
  B_Rem(&Pad,Included);
 }
 Adj_Points(Included,My_Pos);
 Init_Entity();
 B_Goto(Current,My_Pos);
 Cnt_F_Lines(My_Pos+Included);
 Adj_Lines(End_Line-Beg_Line);
#if defined(VMS)
 if (Outfile!=NULL&&strcmp(N,Outfile)==0)
 {strncpy(Outfile,F.Parsed_Name,F.Parsed_Name_Len);
  Outfile[F.Parsed_Name_Len]=0;
 }
#endif
 F_close(&F);
 Plain_Message(Txt_Done);
}

/*      Display an error on the bottom of the display in bold. Ring the
	bell if the value given in L is less than or equal to the global
	value Bell_Level. Set the global variable Err_Flag which signals
	to the rest of SEDT that an error has occurred.
*/
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(WIN32)
void Error(int L,char *E,...)
#else
void Error(int L,char *E,int P1,int P2,int P3,int P4,int P5,int P6,int P7,int P8,int P9)
#endif
/*Flashes an error message on the bottom of the screen*/
{static char S[128];
 char *T;
 int Sav_Line, Sav_Update;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;
 if (Show_Errors)
 {Clr_Typeahead();
  Set_Physical();
  Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,A_Bold);
  Sav_Shift=Scr_Shift;
  Scr_Shift=0;
  Sav_Update=Update_Flag;
  Update_Flag=1;
  Err_Flag=Err_Display=1;
  Working_Flag=0;
#if !((defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))))
  if (L<=Bell_Level)
#if defined(ATARI)
   Cconout(7);
#else
   Out_C('\7');
#endif
#endif
#if defined(VMS) | defined(UNIX) | defined(DOS) | defined(NT) | defined(OS2) | defined(MPE) | defined(WIN32)
  if (strchr(E,'%')==NULL)
#else
  if (index(E,'%')==NULL)
#endif
   T=E; else
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(WIN32)
  {va_list Ap;
   va_start(Ap,E);
   vsprintf(T=S,E,Ap);
   va_end(Ap);
  }
#else
   sprintf(T=S,E,P1,P2,P3,P4,P5,P6,P7,P8,P9);
#endif
  {register char C;
   while ((C=(*T++))!=0)
    Put_Rep(C);
  }
  Opt_Erase_Line();
  Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
  Set_Logical();
  Scr_Shift=Sav_Shift;
  Update_Flag=Sav_Update;
 } else
  Err_Flag=1;
}

/*      Display an error on the bottom of the display in bold. Ring the
	bell if the value given in L is less than or equal to the global
	value Bell_Level. Set the global variable Err_Flag which signals
	to the rest of SEDT that an error has occurred.
*/
void Fast Plain_Error(int L,char *E)
/*Flashes an error message on the bottom of the screen*/
{int Sav_Line, Sav_Update;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;
 if (Show_Errors)
 {Clr_Typeahead();
  Set_Physical();
  Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,A_Bold);
  Sav_Shift=Scr_Shift;
  Scr_Shift=0;
  Sav_Update=Update_Flag;
  Update_Flag=1;
  Err_Flag=Err_Display=1;
  Working_Flag=0;
#if !((defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & defined(Windows)))
  if (L<=Bell_Level)
#if defined(ATARI)
   Cconout(7);
#else
   Out_C('\7');
#endif
#endif
  {register char C;
   while ((C=(*E++))!=0)
    Put_Rep(C);
  }
  Opt_Erase_Line();
  Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
  Set_Logical();
  Scr_Shift=Sav_Shift;
  Update_Flag=Sav_Update;
 } else
  Err_Flag=1;
}

/*      Display a message on the bottom line of the display in normal
        intensity.
*/
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(WIN32)
void Message(char *T,...)
#else
void Message(Char *T,int P1,int P2,int P3,int P4,int P5,int P6,int P7,int P8, int P9)
#endif
/*Flashes a message on the bottom of the screen*/
{static char S[128], *U;
 int Sav_Line, Sav_Update;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;
 if (Err_Flag)
  return;
 Set_Physical();
 Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,0);
 Sav_Shift=Scr_Shift;
 Scr_Shift=0;
 Sav_Update=Update_Flag;
 Update_Flag=1;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(WIN32)
 {va_list Ap;
  va_start(Ap,T);
  vsprintf(U=S,T,Ap);
  va_end(Ap);
 }
#else
 sprintf(U=S,T,P1,P2,P3,P4,P5,P6,P7,P8,P9);
#endif
 {register char C;
  while ((C=(*U++))!=0)
   Put_Rep(C);
 }
 Opt_Erase_Line();
 Err_Display=0;
 Working_Flag=0;
 Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
 Set_Logical();
 Scr_Shift=Sav_Shift;
 Update_Flag=Sav_Update;
 Out_Zap();
}

/*      Display a message on the bottom line of the display in normal
	intensity.
*/
void Fast Plain_Message(char *T)
/*Flashes a message on the bottom of the screen*/
{int Sav_Line, Sav_Update;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;
 if (Err_Flag)
  return;
 Set_Physical();
 Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,0);
 Sav_Shift=Scr_Shift;
 Scr_Shift=0;
 Sav_Update=Update_Flag;
 Update_Flag=1;
 {register char C;
  while ((C=(*T++))!=0)
   Put_Rep(C);
 }
 Opt_Erase_Line();
 Err_Display=0;
 Working_Flag=0;
 Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
 Set_Logical();
 Scr_Shift=Sav_Shift;
 Update_Flag=Sav_Update;
 Out_Zap();
}

/*      Print a diagnostic message on the bottom line of the display. Be
        careful not to call Diag from any routines that it itself calls,
        which would cause an infinite loop. The arguments given are the
        same as for printf. Do not include a \n in the format line.
*/
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(WIN32)
void Diag(char *F,...)
#else
void Diag(char *F,int P1,int P2,int P3,int P4,int P5,int P6,int P7,int P8, int P9)
#endif
/*Prints a diagnostic message*/
{FILE *Df;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(WIN32)
 va_list Ap;
 va_start(Ap,F);
 Df=fopen("dump","a");
 vfprintf(Df,F,Ap);
#else
 Df=fopen("dump","a");
 fprintf(Df,F,P1,P2,P3,P4,P5,P6,P7,P8,P9);
#endif
 fprintf(Df,"\n");
 fclose(Df);
 va_end(Ap);
}

#if defined(VMS)
/*      Issue a QIO to read from the terminal. The QIO used depends in whether
        the cursor is at the end of the line and the position relative to
        the right margin on the ruler and the screen. This routine optimizes
        the number of QIO's by issuing multicharacter echoing input reqests
        when possible.
*/
Get_Input()
{int L;
 /* char Prompt_String[10];
 switch (Term_Type)
 {case VT2XX:
  case VT3XX:
  case VT4XX:
  case VT1XX:
  case VK100:
  case ANSI:
   sprintf(Prompt_String,"\033[%d;%dH",My_Line,(int)My_Col-Scr_Shift);
   break;
  case VT52:
   sprintf(Prompt_String,"\033Y%c%c",037+My_Line,(int) 037+My_Col-Scr_Shift);
 } 290a */
/* if ((Mapping==EDT_Mapping||Mapping==WPS_Mapping)&&
  !Display_Column&&Do_Echo&&(My_Col<Right_Margin||Right_Margin==1)&&
  My_Col-Scr_Shift>Lst_Col[My_Line+Top_Row]&&My_Col-Scr_Shift<Scr_Width&&
  My_Col-Scr_Shift>0)
 {L=Right_Margin==1||Right_Margin-Scr_Shift>Scr_Width?
   Scr_Width-(My_Col-Scr_Shift):Right_Margin-My_Col+1;
  if (Auto_Save&&Outfile!=NULL)
  {if (Auto_Count<Auto_Save-Auto_Warn)
   {if (L>Auto_Save-Auto_Count-Auto_Warn)
     L=Auto_Save-Auto_Count-Auto_Warn;
   } else
   if (L>Auto_Save-Auto_Count)
    L=Auto_Save-Auto_Count;
  }
  Echoed=1;
  Got_In=T$Startmany(TTY_Buffer,L);
 } else*/
 {Echoed=0;
  Got_In=T$Startone(TTY_Buffer,Maxcol);
 }
}

/*      Get a single character from the keyboard. This is used to read the 
        terminal response to a type query escape sequence.
*/
Termin()
/*Get a character from the console*/
{register int C;
 Out_Zap();
 if (Got_In==This_In)
 {do
   Got_In=Get_Input();
  while (Got_In==0);
  This_In=0;
 }
 C=TTY_Buffer[This_In++];
 Do_Echo=0;
 return C;
}      
#endif

int Fast Pad_Put(int What)
{char Rep[10];
 if (What>=' '&&What<127)
 {B_Put(&Pad,'A');
  B_Put(&Pad,(char)What);
  return 2;
 }
 B_Put(&Pad,'N');
#if defined(HPUX)
 strcpy(Rep,ltoa((long) What));
#else
 ltoa((long) What,Rep,10);
#endif
 B_Put_S(&Pad,Rep,strlen(Rep));
 return strlen(Rep)+1;
}

int Fast Pad_Get()
{int C, Val, Sign=1;
 if (B_Get(&Pad)=='A')
  return B_Get(&Pad);
 Val=0;
 while (1)
 {if (Pad.Pos>Pad.Bufsize)
   return Val*Sign;
  if ((C=B_Get(&Pad))>='0'&&C<='9')
   Val=Val*10+C-'0'; else
  if (C=='-')
   Sign= -1; else
  {B_B_Get(&Pad);
   return Val*Sign;
  }
 }
}

/*      Returns a keystroke. The global variables Function and Gold are set
	to identify the type of keystroke. This routine also clears the bottom
	line of the display and Updates the header line if no typeahead has
	been performed.
*/
void Fast Get16()
/*Gets a 16 bit code from the keyboard*/
{long *Where, Count;
 Function=Gold=Enhanced=0;
 if (Executing)
 {
#if defined(VMS)
  Echoed=0;
#endif
  Where=Key_Addr(Ex_Function[Ex_Debth-1]);
  B_Goto(&Pad,(long)Where[0]+Ex_Index[Ex_Debth-1]);
  if (B_Get(&Pad)=='G')
   Gold=1;
  Character=Pad_Get();
  Key_Number=Pad_Get();
  Function=Pad_Get();
  Ex_Index[Ex_Debth-1]=Pad.Pos-Where[0];
  if (Ex_Index[Ex_Debth-1]>=Where[1])
  {if (--Ex_Count[Ex_Debth-1]>0)
    Ex_Index[Ex_Debth-1]=1; else
   {if ((--Ex_Debth)==0)
    {Executing=0;
     if (!Update_Flag)
      Update_On();
    }
   }
   Hdr_Changes=1;
  }
  return;
 }
 Upd_Header();
 /*Diag("Get16:My_Line=%d,My_Col=%d",My_Line,My_Col);*/
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Out_Zap();
 Win_On_Curs();
#endif
#if defined(ATARI)
 Out_Zap();
#if defined(GEM)
 GEM_On_Curs();
#else
 Cursconf(1,0);
#endif
#endif
 Getkbd();
#if defined(VMS)
 if (!Echoed&&Err_Display)
#else
 if (Err_Display)
#endif
  Clr_Error();
 if (Learning)
 {if (Ln_Function==Function)
  {Learning=0;
   Hdr_Changes=1;
#if defined(VMS) | defined(UNIX)
   if (Gotmouse)	/* 290a */
    Enable_Mouse();
#endif
   Upd_Header();
   Out_Zap();
#if (defined(DOS) | defined(NT)) & defined(Windows)
   Win_Off_Curs();
#endif
#if defined(ATARI)
#if defined(GEM)
   GEM_Off_Curs();
#else
   Cursconf(0,0);
#endif
#endif
   Get16();
   return;
  } else
  {Where=Key_Addr(Ln_Function);
   B_Goto(&Pad,Where[0]+Where[1]);
   if (Gold)
    B_Put(&Pad,'G'); else
    B_Put(&Pad,'N');
   Count=1;
   Count+=Pad_Put(Character);
   Count+=Pad_Put(Key_Number);
   Count+=Pad_Put(Function);
   Where[1]+=Count;
   Adj_Pad(Where[0],(long) Count);
  }
 }
 /*Diag("Get16:Gold=%d,Function=%d,C=%d",Gold,Function,C);*/
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Win_Off_Curs();
#endif
#if defined(ATARI)
#if defined(GEM)
 GEM_Off_Curs();
#else
 Cursconf(0,0);
#endif
#endif
 return;
}

#if defined(VMS)
/*      Insert a string of characters that have been read in a single echoed
        QIO into the current buffer. For optimization purposes the keystrokes
        are inserted into the Com buffer until all characters have been read.
*/
Put_Echoed()
{B_Goto(&Com,Sav_Com);
 B_Goto(Current,Echo_Pos);
 B_Copy(&Com,Current,(long) Echo_Read);
 B_Goto(&Com,Sav_Com);
 B_Rem(&Com,(long) Echo_Read);
 Adj_Points((long) Echo_Read,My_Pos-Echo_Read);
 Echo_Read=0;
 Set_Cursor(My_Line,My_Col);
 if (Err_Display)
  Clr_Error();
}
#endif

void Fast Clr_Error()
{int Sav_Line;
 char Sav_Attributes;
 long Sav_Col, Sav_Shift;
 if (Auto_Save&&Outfile!=NULL&&(Any_Changes[Cur_Buffer]&1)&&
  Auto_Count>=Auto_Save-Auto_Warn&&Auto_Warn!=0&&
  !Read_Only[Cur_Buffer])
  Plain_Message(Txt_Auto_Warn); else
 {Set_Physical();
  Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,0);
  Sav_Shift=Scr_Shift;
  Scr_Shift=0;
  Opt_Erase_Line();
  Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
  Set_Logical();
  Scr_Shift=Sav_Shift;
 }
 Err_Display=0;
}

/*      Read from the keyboard until a complete command has been read. Execute
        the command.
*/
void Fast Do_Command()
/*Read and execute a command*/
{register int Count_Typed;
 long Sel_Pos;
#if defined(VMS)
 char *T;
#endif
 if (Do_Shift)
 {if (My_Col-Scr_Shift<1)
  {
#if defined(VMS)
   if (Echo_Read)
    Put_Echoed();
#endif
   while (My_Col-Scr_Shift<1)
    Scr_Shift-=Shift_Screen?Shift_Screen:Scr_Width/2;
   Hdr_Changes=1;
   if (Show_Ruler)
    S_Ruler();
   Fill_Screen(Rel_First,Rel_Last);
  } else
  if (My_Col-Scr_Shift>=Scr_Width)
  {
#if defined(VMS)
   if (Echo_Read)
    Put_Echoed();
#endif
   while (My_Col-Scr_Shift>=Scr_Width)
    Scr_Shift+=Shift_Screen?Shift_Screen:Scr_Width/2;
   Hdr_Changes=1;
   if (Show_Ruler)
    S_Ruler();
   Fill_Screen(Rel_First,Rel_Last);
  } else
  if (My_Col-Scr_Shift<1&&Scr_Shift>0)
  {
#if defined(VMS)
   if (Echo_Read)
    Put_Echoed();
#endif
   while (My_Col-Scr_Shift<1&&Scr_Shift>0)
    Scr_Shift-=Shift_Screen?Shift_Screen:Scr_Width/2;
   Hdr_Changes=1;
   if (Show_Ruler)
    S_Ruler();
   Fill_Screen(Rel_First,Rel_Last);
  }
 } else
 if (Scr_Shift>0)
 {
#if defined(VMS)
  if (Echo_Read)
   Put_Echoed();
#endif
  Scr_Shift=0;
  Hdr_Changes=1;
  if (Show_Ruler)
   S_Ruler();
  Fill_Screen(Rel_First,Rel_Last);
 }
 if (!Update_Flag&&!Executing)
  Update_On();
 if (Working_Flag)
  Clr_Working();
 if (Err_Flag)
 {Executing=Ex_Debth=Err_Flag=0;
  Hdr_Changes=1;
 }
 if ((Sel_Pos=Points[0])!=0)
 {if (Blk_Mode)
  {if (My_Pos>=Sel_Pos&&My_Pos<=My_Pos)
   {Cur_Attributes=A_Reverse;
    if (My_Pos==Sel_Pos)
     Sel_Col=My_Col;
   } else
   if (Cur_Attributes!=0)
    Cur_Attributes=0;
  } else
  if (Sel_Pos<=My_Pos)
  {if (Cur_Attributes!=A_Reverse)
    Cur_Attributes=A_Reverse;
  } else
  if (Cur_Attributes!=0)
   Cur_Attributes=0;
 } else
 if (Cur_Attributes!=0)
  Cur_Attributes=0;
#if defined(VMS)
 Do_Echo=1;
 if (Cur_Attributes!=His_Attributes)
  Opt_Force_Attributes(Cur_Attributes);
#endif
#if defined(UNIX)
 if (Cur_Attributes!=His_Attributes)
  Opt_Force_Attributes(Cur_Attributes);
#endif
#if (defined(ATARI) & defined(GEM)) | defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 Mouse_OK=1;
#endif
#if defined(VMS)
 if (Bcast_Show)
 {if (strchr(Mbx_Mess[Mbx_Count-1],7)!=NULL)
   Out_C(7);
  Plain_Message(Mbx_Mess[Mbx_Count-1]);
  Err_Display=0;
  Bcast_Show=0;
 }
 Bcast_OK=1;
#endif
 Get16();
#if (defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV)))
 Mouse_OK=0;
 if (Not_End==0)
  return;
#endif
#if defined(OS2) | (defined(DOS) & !defined(Windows) & !defined(TV))
 Mouse_OK=0;
#endif
#if defined(VMS)
 Bcast_OK=0;
#endif
 Count=1;
 Com_Direction=Def_Direction;
 if (Gold&&isdigit(Character)&&Key_Number<0)
 {while (Gold&&isdigit(Character)&&Key_Number<0)
   Read_Count(Character);
  Count_Typed=1;
  Count=Local_Count;
 } else
  Count_Typed=0;
 if (!Function)
 {
#if defined(VMS)
  if (Echoed)
  {Ins_E_Char(Character);
   Count--;
  } else
  if (Echo_Read)
   Put_Echoed();
#endif
  CC_Flag=0;
  while (Count--&&!Check_CC())
   Ins_Char(Character);
  Any_Changes[Cur_Buffer]|=1;
 } else
 {
#if defined(VMS)
  if (Echo_Read)
   Put_Echoed();
#endif
  if (Funcs[Function][0]==0)
  {while (Count--)
    Ins_Char((int) Funcs[Function][1]);
  } else
  {CC_Flag=0;
   if (Count_Typed)
    Put_Count(Count);
   Exec_Key(Function);
  }
 }
 if (!Executing&&Auto_Save&&Outfile!=0&&
  (Any_Changes[Cur_Buffer]&1)&&Auto_Count>0)
 {if (++Auto_Count==Auto_Save-Auto_Warn&&Auto_Warn!=0&&
  !Read_Only[Cur_Buffer])
   Plain_Message(Txt_Auto_Warn);
  if (Auto_Count>=Auto_Save)
  {
#if defined(VMS)
   if (Echo_Read)
    Put_Echoed();
#endif
   Save_Auto(Outfile);
   if (!Err_Flag)
    Auto_Saved++;
   Auto_Count=0;
  }
 }
}

#if defined(VMS)
Print_Mess()
{char *T;
 if (Mbx_Count==100)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move(Mbx_Mess[1],Mbx_Mess[0],99*1024); else
#else
  C_Move(Mbx_Mess[1],Mbx_Mess[0],99*1024,1); else
#endif
  Mbx_Count++;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 C_Move(Mbxmsgadr(),Mbx_Mess[Mbx_Count-1],1024);
#else
 C_Move(Mbxmsgadr(),Mbx_Mess[Mbx_Count-1],1024,0);
#endif
 if (Bcast_OK)
 {if (strchr(Mbx_Mess[Mbx_Count-1],7)!=NULL)
   Out_C(7);
  Plain_Message(Mbx_Mess[Mbx_Count-1]);
  Opt_Move_Cursor(My_Line,My_Col);
  Out_Zap();
  Err_Display=0;
 } else
  Update_Flag=Bcast_Show=1;
}
#endif

void Fast Clr_Working()
{int Sav_Line;
 char Sav_Attributes;
 long Sav_Col, Sav_Shift;
 Working_Flag=0;
 Set_Physical();
 Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,0);
 Sav_Shift=Scr_Shift;
 Scr_Shift=0;
 Opt_Erase_Line();
 Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
 Set_Logical();
 Scr_Shift=Sav_Shift;
}


/*      Insert a single character into the buffer and update both the screen
	and all buffer variables. This routine does all housekeeping necessary
	to insert a character and manages formatting with fill and
	justification. To insert blocks of characters this routine
	should not be used for performance reasons. This routine cannot be
	used to insert line feed characters which are used as line terminators.
	Use New_Line for this purpose.
*/
void Fast Ins_Char(int C)
/*Insert C in the file*/
{char S[128];
 int L, O;
 long Sel_Pos, Sav_Col;
 B_Goto(Current,My_Pos);
 Set_Region();
 if (C==10)
  Do_New_Line(1,(long)1); else
 {if ((Sel_Pos=Points[0])!=0)
  {if (Blk_Mode)
   {if (My_Pos>=Sel_Pos&&My_Pos<=My_Pos)
    {Cur_Attributes=A_Reverse;
     if (My_Pos==Sel_Pos)
      Sel_Col=My_Col;
    } else
    if (Cur_Attributes!=0)
     Cur_Attributes=0;
   } else
   if (My_Pos>=Sel_Pos)
   {if (Cur_Attributes!=A_Reverse)
     Cur_Attributes=A_Reverse;
   } else
   if (Cur_Attributes!=0)
    Cur_Attributes=0;
  } else
  if (Cur_Attributes!=0)
   Cur_Attributes=0;
  if (!Replace_Mode||My_Pos>=End_Region)
  {Put_A_Char(C);
   My_Pos++;
#if defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE)
   if (My_Col-Scr_Shift>Lst_Col[My_Line+Top_Row])
#else
   if (My_Col-Scr_Shift>Lst_Col[My_Line])
#endif
    Put_Rep(C); else
   {Put_Rep(C);
    Fill_Rest();
   }
  } else
  {O=B_Get(Current);
   if (Chk_EL(Current,O))
   {B_Goto(Current,My_Pos);
    Put_A_Char(C);
    My_Pos++;
    Put_Rep(C);
   } else
   {B_Goto(Current,My_Pos);
    B_Replace(Current,C);
    if (O==9)
    {L=Char_Rep(O,S);
     B_Put_S(Current,S,L-1);
     Adj_Points((long)L-1,My_Pos);
     O=' ';
    }
    My_Pos++;
    if ((L=Char_Rep(C,NULL))==Char_Rep(O,NULL))
     Put_Rep(C); else
    {Put_Rep(C);
     Fill_Rest();
    }
   }
  }
  if (My_Col>=Right_Margin&&Right_Margin!=1&&C!=' '&&C!=9&&
   Right_Margin>Word_Wrap)
  {if (Auto_Margin)
    Find_Margin();
   Init_Entity();
   Ent_Direction=1;
   B_Goto(Current,My_Pos);
   while ((C=B_B_Get(Current))!='\n')
   {if (Current->Pos==1)
     break;         
    if (C!=' '&&C!=9&&C!=12)
     Beg_Pos=Current->Pos;
   }                
   if (C=='\n')     
    B_Get(Current); 
   Sav_Col=My_Col;  
   My_Col=1;        
   while (My_Col<Word_Wrap)
    My_Col+=Char_Rep(C=B_Get(Current),NULL);
   My_Col=Sav_Col;
   if (Current->Pos>Beg_Pos)
    Beg_Pos=Current->Pos;
   B_Goto(Current,End_Pos=My_Pos);
   Do_Fill();
  }
 }
 if (Blk_Mode&&(Sel_Pos=Points[0])!=0)
 {if (Sel_Pos<My_Pos)
   Fill_Screen(Rel_First,0);
 }
}

#if defined(VMS)
/*      Insert a single character that has been echoed into the temporary
        database. Update the screen datavase to reflect that the character
        is on the display.
*/
Ins_E_Char(C)
int C;
{char Old_C, S[Maxcol+1];
 register int Chars, I;
 if ((Echo_Read++)==0)
 {B_Goto(Current,My_Pos);
  Sav_Com=Com.Pos;
  Echo_Pos=My_Pos;
 }
 B_Put(&Com,C);
 My_Pos++;
 if ((C>=' '&&C<='~')||(C>160&&C<255))
  Opt_E_Char(C); else
  Put_Rep(C);
}
#endif

/*      Read a series of digits from the keyboard and display them on the
        bottom line of the display. Delete and ^U allow editing of the digits.
        Return the character that terminated the digits. C is the first digit
        that has already been read. Return the value of the dtring in
        the global variable Local_Count.
*/
long Fast Read_Count(int C)
/*Reads a count from the keyboard*/
{int Sav_Line, Sav_Update;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;
 Mark_Cursor();
 Set_Physical();
 Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,A_Bold);
 Sav_Shift=Scr_Shift;
 Scr_Shift=0;
 Sav_Update=Update_Flag;
 Update_Flag=1;
 Opt_Erase_Line();
 Opt_Char((char)C);
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#endif
 Local_Count=C-'0';
 while (1)
 {Get16();
  if (isdigit(Character)&&Key_Number<0)
  {Opt_Char((char)Character);
   Local_Count=Local_Count*10+Character-'0';
  } else
  if (Key_Number==49||Character==127)
  {if (My_Col!=1)
   {My_Col--;
    Opt_Char(' ');
    My_Col--;
    Local_Count/=10;
   }
  } else
  if (Character=='\025')
  {My_Col=1;
   Opt_Erase_Line();
   Local_Count=1;
   Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
   Set_Logical();
   Scr_Shift=Sav_Shift;
   Update_Flag=Sav_Update;
   Unmark_Cursor();
#if defined(VMS) | defined(UNIX)
   if (Gotmouse)	/* 290a */
    Enable_Mouse();
#endif
   return C;
  } else
  {Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
   Set_Logical();
   Scr_Shift=Sav_Shift;
   Update_Flag=Sav_Update;
   Err_Display=1;
   Working_Flag=0;
   Unmark_Cursor();
#if defined(VMS) | defined(UNIX)
   if (Gotmouse)	/* 290a */
    Enable_Mouse();
#endif
   return(C);
  }
 }
}

/*      Insert the representation of the integer Count into the Com buffer
*/
void Fast Put_Count(long Count)
/*Puts Count into Com*/
{if (Count<10)
  B_Put(&Com,(char)(Count+'0')); else
 {Put_Count(Count/10);
  B_Put(&Com,(char)((Count%10)+'0'));
 }
}

#if defined(UNIX)
/*      Concatenate the value of the environment variable SEDT to the string
        given in S and return the new string in N. If the SEDT is not defined
        return S.
*/
char *Find_Env(N,S,V)
char *N,*S, *V;
{static char *P;
 char *getenv();
 if ((P=getenv(V))!=NULL)
 {strcpy(S,P);
  if (S[strlen(S)] != '/')
   strcat(S,"/");
  strcat(S,N);
  return(S);
 } else
  return NULL;
}

char *Find_Path(N,S)
char *N, *S;
{char *P, *Q;
 int I;
 static struct stat F_Stat;
 char *getenv();
 if (getenv("SEDT")!=NULL||getenv("SEDTP")!=NULL)
  return NULL;
 if ((P=getenv("PATH"))!=NULL)
 {while (1)
  {if ((Q=strchr(P,':'))==NULL)
    Q=strchr(P,0);
   strncpy(S,P,I=Q-P);
   S[I]=0;
   if (I!=0)
   {if (S[I-1]!='/')
     strcat(S,"/");
   }
   strcat(S,N);
   if (stat(S,&F_Stat)==0)
    return S;
   if ((*Q)==0)
    return NULL;
   P=Q+1;
   if ((*P)==0)
    return NULL;
  }
 } else
  return NULL;
}
#endif

#if defined(DOS) | defined(NT) | defined(WIN32)
char *Find_From(char *N,char *S)
{if (Run_From[0]==0)
  return NULL;
 strcpy(S,Run_From);
 if (S[strlen(S)-1]!='\\')
  strcat(S,"\\");
 strcat(S,N);
 return S;
}
#endif

#if defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(WIN32)
/*      Concatenate the value of the environment variable SEDT to the string
        given in S and return the new string in N. If the SEDT is not defined
        return S.
*/
char *Find_Env(char *N,char *S,char *V)
{char *P;
 int I;
 if ((P=getenv(V))!=NULL)
 {strcpy(S,P);
  if ((I=strlen(S))!=0)
  {if (S[I-1]!='\\')
    strcat(S,"\\");
  }
  strcat(S,N);
  return(S);
 } else
  return NULL;
}

char *Find_Path(char *N,char *S)
{char *P, *Q;
 int I;
 static struct stat F_Stat;
 if (getenv("SEDT")!=NULL||getenv("SEDTP")!=NULL)
  return NULL;
 if ((P=getenv("PATH"))!=NULL)
 {while (1)
  {
#if defined(ATARI)
   if ((Q=index(P,','))==NULL)
    Q=index(P,0);
#else
   if ((Q=strchr(P,';'))==NULL)
    Q=strchr(P,0);
#endif
   strncpy(S,P,I=Q-P);
   S[I]=0;
   if (I!=0)
   {if (S[I-1]!='\\')
     strcat(S,"\\");
   }
   strcat(S,N);
   if (stat(S,&F_Stat)==0)
    return S;
   if ((*Q)==0)
    return NULL;
   P=Q+1;
   if ((*P)==0)
    return NULL;
  }
 } else
  return NULL;
}
#endif

#if defined(VMS)
char *Find_Dir(S1,S2)
char *S1, *S2;
{strcpy(S2,"Sedt$Dir:");
 strcat(S2,S1);
 return S2;
}

char *Find_Lib(S1,S2)
char *S1, *S2;
{strcpy(S2,"Sedt$Library:");
 strcat(S2,S1);
 return S2;
}

char *Logical_File(S)
char *S;
{
 char *T;
 T=strchr(S,'.');
 *T='$';
 return S;
}
#endif

#if defined(ATARI)
char *getenv(S)
char *S;
{char *T, *U;
 BASEPAGE *P;
 P=BP;
 T=(char *)(P->p_env);
 while ((*T)!=0)
 {U=S;
  while((*T)==(*U))
  {T++;
   U++;
  }
  if ((*T)=='='&&(*U)==0)
  {if (*(++T)==0)
    T++;
   return T;
  }
  if ((*T)!=0)
  {while ((*(T++))!=0);
  } else
   T++;
 }
 return NULL;
}
#endif

#if defined(VMS)
void Strip_Version()
{char *S, *T;
 S=New_File;
 while ((T=strchr(S,':'))!=NULL)
  S=T+1;
 if ((T=strchr(S,']'))!=NULL)
  S=T+1;
 if ((T=strchr(S,'.'))!=NULL)
 {S=T+1;
  if ((T=strchr(S,'.'))!=NULL)
   (*T)=0; else
  if ((T=strchr(S,';'))!=NULL)
   (*T)=0;
 }
}
#endif
