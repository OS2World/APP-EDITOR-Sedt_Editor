/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"

#ifdef WIN32
#define MAPKEY_VERSION 4
#else
#define MAPKEY_VERSION 3
#endif

/*      Rems an item from the Pad buffer and adjusts all pointers to
        reflect the change.
*/
void Fast Rem_Pad(long *Where)
/*Adjustes all pad pointers for the removal of a group*/
{long Start, Length;
 Start=Where[0];
 Length=Where[1];
 if (Start==0)
  return;
 if (Start+Length<=Pad.Bufsize)
  Adj_Pad(Start,-Length);
 B_Goto(&Pad,Start);
 B_Rem(&Pad,Length);
}

/*      Rems an item from the Del buffer and adjusts all pointers to
        reflect the change.
*/
void Fast Rem_Del(long *Where)
/*Adjustes all Del pointers for the removal of a group*/
{long Start, Length;
 Start=Where[0];
 Length=Where[1];
 if (Start==0)
  return;
 if (Start+Length<=Del.Bufsize)
  Adj_Del(Start,-Length);
 B_Goto(&Del,Start);
 B_Rem(&Del,Length);
}

/*      Adjusts all pad items to reflect the removal of an item.
*/
void Fast Adj_Pad(long Start,long Chars)
{register int I;
 long
   *P;
 for (I=0;I<Maxfunc;I++)
  if ((P=Funcs[I])[0]>Start)
   P[0]+=Chars;
}

/*      Adjusts all delete pointers to reflect removal of one.
*/
void Fast Adj_Del(long Start,long Chars)
{register int I;
 long
   *P;
 for (I=0;I<Pointers+Maxbuffers;I++)
  if ((P=Dl_Pointers[I])[0]>Start)
   P[0]+=Chars;
}


/*      Gets the definition of the key K and the variables Function and Gold.
        If the definition of the key ends in a period the contents of the Com
        buffer is interpreted. If the definition does not end in a period the
        process is repeated until either an error occurs or a period is seen
        at the end of a definition.
*/
void Fast Exec_Key(int K)
/*Expands a Key Definition and continues till a command is read*/
{Com_End=0;
 do
 {Count=1;
  if (!Function&&Gold&&isdigit(K))
  {while (!Function&&Gold&&isdigit(K))
    K=Read_Count(K);
   Count=Local_Count;
  }
  if (Count>1)
   Put_Count(Count);
  Key_Get(K,Funcs);
  if (!(Com_End||Err_Flag))
  {Get16();
   K=Function;
  }
 } while (!(Com_End||Err_Flag));
 if (!Err_Flag)
 {Expand_Com((long)1);
  if (!Err_Flag)
  {Skip=Severe_Error=0;
   Com_Parse((long)1,0);
  }
 }
 B_Reset(&Com);
}

/*      Interprets all ?, ! and % commands in the command string in the Com
        buffer from the position P.
*/
void Fast Expand_Com(long P)
/*Expands Commands with prompted input*/
{char Delim, Sav_Attributes, S[80], *Q;
 int C, Sav_Line, L, Lst_Index, Sav_Update;
 long St_P, End_P, Sav_P, Sav_Col, Sav_C, Sav_Shift;
 B_Goto(&Com,P);
 while (B_Search(&Com,'!',Com.Bufsize+1))
 {if (Com.Pos>Com.Bufsize)
  {Error(2,No_Delimiter_Txt);
   return;
  }
  if ((Delim=B_Del(&Com))!='!')
  {B_B_Get(&Com);
   B_Del(&Com);
   Mark_Cursor();
   Set_Physical();
#if defined(VMS) | defined(UNIX)
   if (Gotmouse)	/* 290a */
    Disable_Mouse();
#endif
   Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,A_Bold);
   Sav_Shift=Scr_Shift;
   Scr_Shift=0;
   Sav_Update=Update_Flag;
   Update_Flag=1;
   Err_Display=0;
   while (1)
   {if (Com.Pos>Com.Bufsize)
    {Error(2,No_Delimiter_Txt);
     Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
     Set_Logical();
#if defined(VMS) | defined(UNIX)
     if (Gotmouse)	/* 290a */
      Enable_Mouse();
#endif
     Scr_Shift=Sav_Shift;
     Update_Flag=Sav_Update;
     Unmark_Cursor();
     return;
    }
    if ((C=B_Del(&Com))==Delim)
     break;
    Opt_Char((char)C);
   }
   Opt_Erase_Line();
   St_P=End_P=Com.Pos;
   Get16();
   if (Map_Index==-1)
   {if (Key_Number>0)
    {Map_Tab[Map_Defs].Scancode=0;
     Map_Tab[Map_Defs].Char=Key_Code;
     Map_Tab[Map_Defs].Flags=Key_Code+Gold?Gold_Code:0;
    } else
    {Map_Tab[Map_Defs].Scancode=Scancode;
     Map_Tab[Map_Defs].Char=Character;
     Map_Tab[Map_Defs].Flags=Gold?Gold_Code:0;
    }
    Map_Tab[Map_Defs].F_Code= -1;
    Map_Index=Map_Defs++;
   }
   B_Put(&Com,(char)((Map_Index/100)+'0'));
   B_Put(&Com,(char)(((Map_Index/10)%10)+'0'));
   B_Put(&Com,(char)((Map_Index%10)+'0'));
   Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
   Set_Logical();
#if defined(VMS) | defined(UNIX)
   if (Gotmouse)	/* 290a */
    Enable_Mouse();
#endif
   Scr_Shift=Sav_Shift;
   Update_Flag=Sav_Update;
   Err_Display=1;
   Working_Flag=0;
   Unmark_Cursor();
  }
 }
 B_Goto(&Com,P);
 while (B_Search(&Com,'%',Com.Bufsize+1))
 {if (Com.Pos>Com.Bufsize)
  {Error(2,No_Token_Txt);
   return;
  }
  B_B_Get(&Com);
  B_Del(&Com);
  if ((Delim=B_Del(&Com))=='%')
   B_Put(&Com,'%'); else
  if (Delim=='F'||Delim=='f')
  {if (Outfile!=NULL)
    B_Put_S(&Com,Outfile,strlen(Outfile));
  } else
  if (Delim=='N'||Delim=='n')
  {if (Outfile!=NULL)
   {Filnam(Outfile,S);
    B_Put_S(&Com,S,strlen(S));
   }
  } else
  if (Delim=='T'||Delim=='t')
  {if (Outfile!=NULL)
   {Filtyp(Outfile,S);
    B_Put_S(&Com,S,strlen(S));
   }
  } else
  if (Delim=='D'||Delim=='d')
  {if (Outfile!=NULL)
   {Fildir(Outfile,S);
    B_Put_S(&Com,S,strlen(S));
   }
  } else
  if (Delim=='E'||Delim=='e')
  {
#if defined(VMS)
   B_Put_S(&Com,"Sedt$Library:",13);
#endif
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(OS2) | defined(WIN32)
   Q=getenv("SEDT");
   B_Put_S(&Com,Q,strlen(Q));
#endif
  } else
  if (Delim=='A'||Delim=='a')
  {
#if defined(VMS)
   B_Put_S(&Com,"Sedt$Dir:",9);
#endif
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(OS2) | defined(WIN32)
   Q=getenv("SEDTP");
   B_Put_S(&Com,Q,strlen(Q));
#endif
  } else
  if (Delim=='L'||Delim=='l')
  {
#if defined(HPUX)
   strcpy(Dsk_Rec,ltoa(This_Line));
#else
   ltoa(This_Line,Dsk_Rec,10);
#endif
   B_Put_S(&Com,Dsk_Rec,strlen(Dsk_Rec));
  } else
  if (Delim=='C'||Delim=='c')
  {
#if defined(HPUX)
   strcpy(Dsk_Rec,ltoa(My_Pos));
#else
   ltoa(My_Pos,Dsk_Rec,10);
#endif
   B_Put_S(&Com,Dsk_Rec,strlen(Dsk_Rec));
  } else
  if (Delim=='W'||Delim=='w')
  {
#if defined(HPUX)
   strcpy(Dsk_Rec,ltoa(My_Col));
#else
   ltoa(My_Col,Dsk_Rec,10);
#endif
   B_Put_S(&Com,Dsk_Rec,strlen(Dsk_Rec));
  } else
  if (Delim=='P'||Delim=='p')
  {if (Dl_Pointers[Past_String][0]!=0)
   {B_Goto(&Del,Dl_Pointers[Past_String][0]);
    B_Copy(&Del,&Com,Dl_Pointers[Past_String][1]);
   }
  } else
  if (Delim=='B'||Delim=='b')
  {
#if defined(VMS)
   if (Mbx_Count>0)
   {B_Put_S(&Com,Mbx_Mess[0],strlen(Mbx_Mess[0]));
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
    C_Move(Mbx_Mess[1],Mbx_Mess[0],99*1024);
#else
    C_Move(Mbx_Mess[1],Mbx_Mess[0],99*1024,1);
#endif
    Mbx_Count--;
   }
#endif
  } else
  if (Delim=='#')
  {C=B_Del(&Com);
   C=toupper(C);
   if (C<'A'||C>'Z')
    Syntax_Error(); else
   {Counter_Index=C-'A';
#if defined(HPUX)
    strcpy(Dsk_Rec,ltoa(Counter[Counter_Index]));
#else
    ltoa(Counter[Counter_Index],Dsk_Rec,10);
#endif
    B_Put_S(&Com,Dsk_Rec,strlen(Dsk_Rec));
   }
  } else
  if (Delim=='R'||Delim=='r')
  {
#if defined(UNIX) | defined(MPE)
   B_Put(&Com,'\n');
#else
   B_Put(&Com,'\r');
   B_Put(&Com,'\n');
#endif
  } else
  if (Delim=='H'||Delim=='h')
  {Map_Index=0;
   {register int I;
    for (I=1;I<=3;I++)
    {if (End_Com())
     {Com_Error();
      return;
     }
     C=B_Del(&Com);
     if (!isdigit(C))
     {Com_Error();
      return;
     }
     Map_Index=Map_Index*10+C-'0';
    }
   }
   if (Map_Tab[Map_Index].F_Code>=0)
   {if (Funcs[Map_Tab[Map_Index].F_Code][2]!=0)
    {B_Goto(&Pad,Funcs[Map_Tab[Map_Index].F_Code][2]);
     while ((C=B_Get(&Pad))!=0)
      B_Put(&Com,(char)C);
    }
   }
  } else
   Error(2,Illegal_Token_Txt);
 }
 B_Goto(&Com,P);
 while (B_Search(&Com,'?',Com.Bufsize+1))
 {if (Com.Pos>Com.Bufsize)
  {Error(2,No_Delimiter_Txt);
   return;
  }
  if ((Delim=B_Del(&Com))!='?')
  {B_B_Get(&Com);
   B_Del(&Com);
   Mark_Cursor();
   Expand_Tabs=0;
#if ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM))
   if (Dialog)
   {Q=Dsk_Rec;
    while (1)
    {if (Com.Pos>Com.Bufsize)
     {Error(2,No_Delimiter_Txt);
      Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
      Set_Logical();
      Scr_Shift=Sav_Shift;
      Update_Flag=Sav_Update;
      Expand_Tabs=1;
      Unmark_Cursor();
      return;
     }
     if ((C=B_Del(&Com))==Delim)
      break;
     (*(Q++))=C;
    }
    (*(Q++))=0;
    if (Prompt_Input())
     B_Put_S(&Com,Dsk_Rec,strlen(Dsk_Rec)); else
     Error(3,Txt_Cancelled);
   } else
#endif
   {Set_Physical();
    Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,A_Bold);
    Sav_Shift=Scr_Shift;
    Scr_Shift=0;
    Sav_Update=Update_Flag;
    Update_Flag=1;
    Err_Display=0;
    while (1)
    {if (Com.Pos>Com.Bufsize)
     {Error(2,No_Delimiter_Txt);
      Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
      Set_Logical();
      Scr_Shift=Sav_Shift;
      Update_Flag=Sav_Update;
      Expand_Tabs=1;
      Unmark_Cursor();
      return;
     }
     if ((C=B_Del(&Com))==Delim)
      break;
     Opt_Char((char)C);
    }
    Cur_Attributes=0;
#if defined(VMS) | defined(UNIX)
   if (Gotmouse)	/* 290a */
    Disable_Mouse();
#endif
    Opt_Erase_Line();
    St_P=End_P=Com.Pos;
    Lst_Index= -1;
    while (1)
    {
#if defined(VMS)
     Do_Echo=1;
#endif
     Get16();
     if (Key_Number==27)
     {while (Com.Pos!=St_P)
      {B_B_Get(&Com);
       if ((C=B_Del(&Com))<' ')
       {My_Col-=L=Control_Display?1:Char_Rep(C,NULL);
        {register int I;
         for (I=1;I<=L;I++)
          Opt_Char(' ');
        }
        My_Col-=L;
       } else
       {My_Col--;
        Opt_Char(' ');
        My_Col--;
       }
       End_P--;
      }
      if (Lst_Index<9)
      {Lst_Index++;
       {register int I;
        I=0;
        while((C=Lst_Input[Lst_Index][I++])!=0)
        {B_Put(&Com,(char)C);
         if (C<' '||C==127)
          Put_Rep(C); else
          Opt_Char((char)C);
         End_P++;
        }
       }
      }
     } else
     if (Key_Number==29)
     {while (Com.Pos!=St_P)
      {B_B_Get(&Com);
       if ((C=B_Del(&Com))<' ')
       {My_Col-=L=Control_Display?1:Char_Rep(C,NULL);
        {register int I;
         for (I=1;I<=L;I++)
          Opt_Char(' ');
        }
        My_Col-=L;
       } else
       {My_Col--;
        Opt_Char(' ');
        My_Col--;
       }
       End_P--;
      }
      if (Lst_Index>=0)
      {Lst_Index--;
       if (Lst_Index>=0)
       {register int I;
        I=0;
        while((C=Lst_Input[Lst_Index][I++])!=0)
        {B_Put(&Com,(char)C);
         if (C<' '||C==127)
          Put_Rep(C); else
          Opt_Char((char)C);
         End_P++;
        }
       }
      }
     } else
     if (Key_Number==28)
     {if (Com.Pos!=St_P)
       My_Col-=Char_Rep(B_B_Get(&Com),NULL);
     } else
     if (Key_Number==30)
     {if (Com.Pos!=End_P)
       My_Col+=Char_Rep(B_Get(&Com),NULL);
     } else
     if (Key_Number==49)
     {if (Com.Pos!=St_P)
      {B_B_Get(&Com);
       if ((C=B_Del(&Com))<' ')
       {My_Col-=L=Control_Display?1:Char_Rep(C,NULL);
        {register int I;
         for (I=1;I<=L;I++)
          Opt_Char(' ');
        }
        My_Col-=L;
       } else
       {My_Col--;
        Opt_Char(' ');
        My_Col--;
       }
       End_P--;
      }
     } else
     if (Key_Number==50)
     {if (Gold)
      {
#if !(defined(UNIX) | defined(MPE))
       Put_Rep('\r');
       B_Put(&Com,'\r');
       End_P++;
#endif
       Put_Rep('\n');
       B_Put(&Com,'\n');
       End_P++;
      } else
       break;
     } else
     if (Key_Number==48&&!Gold)
      break; else
     if (Character==127)
     {if (Com.Pos!=St_P)
      {B_B_Get(&Com);
       if ((C=B_Del(&Com))<' ')
       {My_Col-=L=Control_Display?1:Char_Rep(C,NULL);
        {register int I;
         for (I=1;I<=L;I++)
          Opt_Char(' ');
        }
        My_Col-=L;
       } else
       {My_Col--;
        Opt_Char(' ');
        My_Col--;
       }
       End_P--;
      }
     } else
     if (Character==21)
     {Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
      Set_Logical();
      Scr_Shift=Sav_Shift;
      Update_Flag=Sav_Update;
      Error(3,Txt_Cancelled);
      Expand_Tabs=1;
      Unmark_Cursor();
#if defined(VMS) | defined(UNIX)
      if (Gotmouse)	/* 290a */
       Enable_Mouse();
#endif
      return;
     } else
     if (Function&&Test_Dir_Key(Funcs,Function))
      break; else
     if (Function&&Ins_Defs)
      End_P+=Ins_Key(Funcs,Function); else
     if (Character<' '||Character==127)
     {Put_Rep(Character);
      B_Put(&Com,(char)Character);
      End_P++;
     } else
     {B_Put(&Com,(char)Character);
#if defined(VMS)
      if (Echoed&&
       (Character<'['||(Character>']'&&Character<'{')||Character>160))
       Opt_E_Char(Character); else
#endif
      Opt_Char((char)Character);
      End_P++;
     }
     if (Com.Pos!=End_P)
     {Sav_P=Com.Pos;
      Sav_C=My_Col;
      while (Com.Pos!=End_P)
       Put_Rep(B_Get(&Com));
      Opt_Erase_Line();
      B_Goto(&Com,Sav_P);
      My_Col=Sav_C;
     }
    }
    B_Goto(&Com,St_P);
    if (Com.Pos!=End_P)
    {int Save_It=0;
     {register int I;
      I=0;
      while (I<131)
      {if (Com.Pos==End_P)
        break;
       if (Lst_Input[0][I++]!=B_Get(&Com))
       {Save_It=1;
        break;
       }
      }
     }
     if (Save_It)
     {B_Goto(&Com,St_P);
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
      C_Move(Lst_Input[0],Lst_Input[1],132*9);
#else
      C_Move(Lst_Input[0],Lst_Input[1],132*9,1);
#endif
      {register int I;
       I=0;
       while (I<131)
       {if (Com.Pos==End_P)
         break;
        Lst_Input[0][I++]=B_Get(&Com);
       }
       Lst_Input[0][I]=0;
      }
     }
    }
    B_Goto(&Com,End_P);
    Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
    Set_Logical();
    Scr_Shift=Sav_Shift;
    Update_Flag=Sav_Update;
#if defined(VMS) | defined(UNIX)
    if (Gotmouse)	/* 290a */
     Enable_Mouse();
#endif
    Out_Zap();
    Err_Display=1;
    Working_Flag=0;
   }
  }
  Expand_Tabs=1;
  Unmark_Cursor();
 }
}

void Fast Set_Physical()
{
#if !(((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM)))
 Out_Zap();
 Push(Top_Row);
 Push(Left_Col);
 Push(Scr_Length);
 Push(Scr_Width);
 Top_Row=Left_Col=0;
 Scr_Width=Max_Width;
 Scr_Length=Max_Length;
#endif
}

void Fast Set_Logical()
{
#if !(((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM)))
 Out_Zap();
 Scr_Width=Pop();
 Scr_Length=Pop();
 Left_Col=Pop();
 Top_Row=Pop();
#endif
}

void Fast Push(int What)
{Save_Stack[In_Prompt++]=What;
}

int Fast Pop()
{return Save_Stack[--In_Prompt];
}

void Fast Mark_Cursor()
{
#if !defined(MPE)
 int Sav_Attributes, C;
 long Sav_Col;
 Sav_Col=My_Col;
 B_Goto(Current,My_Pos);
 Sav_Attributes=Cur_Attributes;
 Cur_Attributes=Cur_Attributes+A_Reverse+A_Bold;
 if (My_Pos>Current->Bufsize)
 {if (!Show_Marks)
   Opt_Char(' ');
 } else
 if (Chk_EL(Current,C=B_Get(Current)))
  Opt_Char(' '); else
 {Char_Rep(C,Dsk_Rec);
  Opt_Char(Dsk_Rec[0]);
 }
 Cur_Attributes=Sav_Attributes;
 My_Col=Sav_Col;
#endif
}

void Fast Unmark_Cursor()
{
#if !defined(MPE)
 int C;
 long Sav_Col;
 Sav_Col=My_Col;
 B_Goto(Current,My_Pos);
 if (My_Pos>Current->Bufsize)
 {if (!Show_Marks)
   Opt_Char(' ');
 } else
 if (Chk_EL(Current,C=B_Get(Current)))
  Opt_Char(' '); else
 {Char_Rep(C,Dsk_Rec);
  Opt_Char(Dsk_Rec[0]);
 }
 My_Col=Sav_Col;
#endif
}

int Fast Test_Dir_Key(long (*A)[3],int I)
{long
  *P;
 char C;
 if ((P=A[I])[0]==0||P[1]!=3)
  return 0;
 B_Goto(&Pad,P[0]);
 if (B_Get(&Pad)==':')
  if ((C=B_Get(&Pad))=='A'||C=='a')
  {if (B_Get(&Pad)=='.')
   {Com_Direction=1;
    return 1;
   }
  } else
  if (C=='B'||C=='b')
  {if (B_Get(&Pad)=='.')
   {Com_Direction=0;
    return 1;
   }
  }
 return 0;
}

/*      Inserts the text from the Pad buffer given by A and I into the COM
        buffer.
*/
int Fast Ins_Key(long (*A)[3],int I)
{long
  *P;
 if ((P=A[I])[0]==0)
  return 0;
 B_Goto(&Pad,P[0]);
 B_Copy(&Pad,&Com,P[1]);
 B_Goto(&Pad,P[0]);
 {register int J;
  register char C;
  for (J=1;J<=P[1];J++)
  {if ((C=B_Get(&Pad))<' '||C==127)
    Put_Rep(C); else
    Opt_Char(C);
  }
 }
 return(P[1]);
}

/*      Checks whether a key given by A and I have a definition and either
        issues an error or inserts the definition into the Com buffer.
*/
void Fast Key_Get(int I,long (*A)[3])
/*Does the actual key insertion*/
{long
  *P;
 if ((P=A[I])[1]==0)
  Error(2,Unprogrammed_Txt); else
  Ins_A_Key(P[0],P[1]);
}

/*      Does the actual transfer from the Pad to the Com buffer and returns
        whether a period was the last character in the global variable Com_End.
*/
void Fast Ins_A_Key(long Start,long Length)
/*Gets a key definition from Pad*/
{B_Goto(&Pad,Start);
 if (Length>0)
 {if (B_Get(&Pad)==15)
  {B_Put_S(&Com,":LNE",4);
   B_Put(&Com,(char)('0'+Map_Index/100));
   B_Put(&Com,(char)('0'+(Map_Index/10)%10));
   B_Put(&Com,(char)('0'+Map_Index%10));
   Com_End=1;
   return;
  } else
   B_B_Get(&Pad);
 }
 B_Copy(&Pad,&Com,Length);
 B_Goto(&Pad,Pad.Pos-1);
 Com_End=B_Get(&Pad)=='.';
}

/*      Reads all key definitions from the file given in N and inserts them
        in the key definition database. It is assumed that the database is 
        empty when this routine is called.
*/
int Fast Readkeys(char *N)
/*Read key definitions from files*/
{
#if defined(VMS) | defined(MPE)
 FILE *F;
#else
 struct File F;
#endif
#if defined(VMS) | defined(MPE)
 if ((F=fopen(N,"r"))==0)
#else
 if (F_open(N,"r",&F)==0)
#endif
  return 0;
#if defined(VMS) | defined(MPE)
 fread(&Top_Func,sizeof(int),1,F);
#else
#if defined(WIN95)
 F_read((char *)&Top_Func,sizeof(short),1,&F);
#else
 F_read((char *)&Top_Func,sizeof(int),1,&F);
#endif
#endif
 Next_Func=Top_Func;
#if defined(VMS) | defined(MPE)
 if (fread(Funcs,3*sizeof(long),Top_Func+1,F)<Top_Func+1)  /* 290a */
 {Sedt_Shutdown();
  Fatalerror(No_Memory_Txt,(char *) Keyfile());
  _exit(1);
 }
#else
 F_read((char *)Funcs,3*sizeof(long),Top_Func+1,&F);
#endif
 {register int L, T;
  T=Pad.Bufsize;
  for (L=1;L<=Top_Func;L++)
   if (Funcs[L][0]!=0)
   {Funcs[L][0]+=T;
    Funcs[L][2]+=T;
   }
 }
 {register int L;
#if defined(VMS) | defined(MPE)
  while ((L=fread(Dsk_Rec,1,Blksize,F))!=0)
#else
  while ((L=F_read(Dsk_Rec,1,Blksize,&F))!=0)
#endif
   B_Put_S(&Pad,Dsk_Rec,L);
 }
#if defined(VMS) | defined(MPE)
 fclose(F);
#else
 F_close(&F);
#endif
 return 1;
}

/*      Reads all key mappings from the file given in N and inserts them
        in the key definition database. It is assumed that the database is 
        empty when this routine is called.
*/
int Fast Readmaps(char *N)
/*Read key mappings from files*/
{
#if defined(VMS) | defined(MPE)
 FILE *F;
#else
 struct File F;
#endif
#if defined(WIN95)
 short L;
#else
 int L;
#endif
#if defined(VMS) | defined(MPE)
 if ((F=fopen(N,"r"))==0)
#else
 if (F_open(N,"r",&F)==0)
#endif
  return 0;
#if defined(VMS) | defined(MPE)
 fread(&L,sizeof(int),1,F);
#else
#if defined(WIN95)
 F_read((char *)&L,sizeof(short),1,&F);
#else
 F_read((char *)&L,sizeof(int),1,&F);
#endif
#endif
 /*Diag("L=%d",L);*/
 if (L==MAPKEY_VERSION)
#if defined(VMS) | defined(MPE)
 {fread(&L,sizeof(int),1,F);
  fread(Init_T_String,sizeof(char),L,F);
  Init_T_String[L]=0;
  fread(&L,sizeof(int),1,F);
  fread(Reset_T_String,sizeof(char),L,F);
#else
 {
#if defined(WIN95)
  F_read((char *)&L,sizeof(short),1,&F);
#else
  F_read((char *)&L,sizeof(int),1,&F);
#endif
  F_read(Init_T_String,sizeof(char),L,&F);
  Init_T_String[L]=0;
#if defined(WIN95)
  F_read((char *)&L,sizeof(short),1,&F);
#else
  F_read((char *)&L,sizeof(int),1,&F);
#endif
  F_read(Reset_T_String,sizeof(char),L,&F);
#endif
  Reset_T_String[L]=0;
 } else
 {Sedt_Shutdown();
  Fatalerror(Txt_Bad_Version);
#if defined(OS2)
  DosExit(1,1);
#else
#if defined(DESQVIEW)
  if (DV)
   api_exit();
#endif
#if defined(DOS) & defined(TV)
  TV_Exit(1);
#endif
  _exit(1);
#endif
 }
#if defined(VMS) | defined(MPE)
 fread(&Scan_Defs,sizeof(int),1,F);
 fread(Scan_Tab,sizeof(struct Scan_Tab_Struct),Scan_Defs,F);
#else
#if defined(WIN95)
 F_read((char *)&Scan_Defs,sizeof(short),1,&F);
#else
 F_read((char *)&Scan_Defs,sizeof(int),1,&F);
#endif
 /*Diag("Scan_Defs=%d",Scan_Defs);*/
 F_read((char *)Scan_Tab,sizeof(struct Scan_Tab_Struct),Scan_Defs,&F);
#endif
 Scan_Tab[Scan_Defs].Scancode=0;
#if defined(VMS) | defined(MPE)
 fread(&Key_Defs,sizeof(int),1,F);
#else
#if defined(WIN95)
 F_read((char *)&Key_Defs,sizeof(short),1,&F);
#else
 F_read((char *)&Key_Defs,sizeof(int),1,&F);
#endif
#endif
 /*Diag("Key_Defs=%d",Key_Defs);*/
 if (Key_Defs!=0)
#if defined(DOS) & defined(TV)
  if ((Key_Tab=new struct Key_Tab_Struct[Key_Defs])==NULL)
#else
  if ((Key_Tab=(struct Key_Tab_Struct *)
   malloc((sizeof(struct Key_Tab_Struct))*Key_Defs))==NULL)
#endif
 {Sedt_Shutdown();
  Fatalerror(No_Memory_Txt);
#if defined(OS2)
  DosExit(1,1);
#else
#if defined(DESQVIEW)
  if (DV)
   api_exit();
#endif
#if defined(DOS) & defined(TV)
  TV_Exit(1);
#endif
  _exit(1);
#endif
 }
#if defined(VMS) | defined(MPE)
 fread(Key_Tab,sizeof(struct Key_Tab_Struct),Key_Defs,F);
 fread(&Comb_Defs,sizeof(int),1,F);
#else
 F_read((char *)Key_Tab,sizeof(struct Key_Tab_Struct),Key_Defs,&F);
#if defined(WIN95)
 F_read((char *)&Comb_Defs,sizeof(short),1,&F);
#else
 F_read((char *)&Comb_Defs,sizeof(int),1,&F);
#endif
 /*Diag("Comb_Defs=%d",Comb_Defs);*/
#endif
 if (Comb_Defs!=0)
#if defined(DOS) & defined(TV)
  if ((Comb_Tab=new struct Comb_Tab_Struct[Comb_Defs])==NULL)
#else
  if ((Comb_Tab=(struct Comb_Tab_Struct *)
   malloc((sizeof(struct Comb_Tab_Struct))*Comb_Defs))==NULL)
#endif
 {Sedt_Shutdown();
  Fatalerror(No_Memory_Txt);
#if defined(OS2)
  DosExit(1,1);
#else
#if defined(DESQVIEW)
  if (DV)
   api_exit();
#endif
#if defined(DOS) & defined(TV)
  TV_Exit(1);
#endif
  _exit(1);
#endif
 }
#if defined(VMS) | defined(MPE)
 fread(Comb_Tab,sizeof(struct Comb_Tab_Struct),Comb_Defs,F);
 fread(&Map_Defs,sizeof(int),1,F);
 fread(Map_Tab,sizeof(struct Map_Tab_Struct),Map_Defs,F);
#else
 F_read((char *)Comb_Tab,sizeof(struct Comb_Tab_Struct),Comb_Defs,&F);
#if defined(WIN95)
 F_read((char *)&Map_Defs,sizeof(short),1,&F);
#else
 F_read((char *)&Map_Defs,sizeof(int),1,&F);
#endif
 /*Diag("Map_Defs=%d",Map_Defs);*/
 F_read((char *)Map_Tab,sizeof(struct Map_Tab_Struct),Map_Defs,&F);
#endif
#if defined(VMS) | defined(MPE)
 while ((L=fread(Dsk_Rec,1,Blksize,F))!=0)
#else
 while ((L=F_read(Dsk_Rec,1,Blksize,&F))!=0)
#endif
  B_Put_S(&Pad,Dsk_Rec,L);
#if defined(VMS) | defined(MPE)
 fclose(F);
#else
 F_close(&F);
#endif
 return 1;
}

/*      Returns the representation of the character C in the character string
	S and the length of the representation as the result of the function.
	If S is NULL only the length is returned. To represent a TAB character
	My_Col defines the start column of the representation and also assumes
	that hardware tab stops are at every 8 columns. NOTE that the ruler
	does not affect the hardware tab stop settings.
*/
int Fast Char_Rep(int C,char *S)
/*Puts the representation of character C in S*/
{register int J;
 register long I;
 if (C=='\011'&&Expand_Tabs)
 {J=0;
  I=My_Col;
  while (1)
  {if (S!=NULL)
    S[J++]=' '; else
    J++;
   if ((++I)%8==1)
    break;
  }
  if (S!=NULL)
   S[J]='\0';
  /*Diag("Char_Rep:Tab:L=%d",J-1);*/
  return(J);
 } else
 if (C==127)
 {if (S!=NULL)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
   C_Move("<DEL>",S,6);
#else
   C_Move("<DEL>",S,6,0);
#endif
  return(5);
 } else
 if (C<32)
 {if (S!=NULL)
  {if (Control_Display)
   {S[0]=C+64;
    S[1]=0;
   } else
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
    C_Move(Rep[C],S,strlen(Rep[C])+1);
#else
    C_Move(Rep[C],S,strlen(Rep[C])+1,0);
#endif
  }
  return(Control_Display?1:strlen(Rep[C]));
 } else
#if defined(VMS) | defined(UNIX)
#if defined(UTF8_HACK)
 if ((C>=' '&&C<='~')) 
#else
 if ((C>=' '&&C<='~')||(C>160&&C<255))
#endif 
 {if (S!=NULL)
  {S[0]=C;
   S[1]=0;
  }
  return(1);
 } else
 {if (S!=NULL)
   sprintf(S,"<%3d>",C);
  return(5);
 }
#endif
#if defined(ATARI) | defined(MPE)
 if (S!=NULL)
 {S[0]=C;
  S[1]=0;
 }
 return(1);
#endif
#if defined(DOS) | defined(NT) | defined(WIN32) 
#if !defined(PC100)
 if (S!=NULL)
 {S[0]=C;
  S[1]=0;
 }
 return(1);
#else
 if ((C>=' '&&C<='~')||(C>160&&C<255))
 {if (S!=NULL)
  {S[0]=C;
   S[1]=0;
  }
  return(1);
 } else
 {if (S!=NULL)
   sprintf(S,"<%3d>",C);
  return(5);
 }
#endif
#endif
#if defined(OS2)
 if (S!=NULL)
 {S[0]=C;
  S[1]=0;
 }
 return(1);
#endif
}

/*      Rems the string that W points to in the delete string database
        and makes W point to an empty string at the end of the Del buffer.
*/
void Fast Erase_Del(long *W)
{Rem_Del(W);
 W[0]=Del.Bufsize+1;
 W[1]=0;
 B_Goto(&Del,Del.Bufsize+1);
}

/*      Returns address of the two longword pointer array that points to
        the representation of a key.
*/
long  *Key_Addr(int Function)
{return(Funcs[Function]);
}

#if defined(ATARI) | defined(VMS) | (defined(UNIX) & !defined(HPUX)) | defined(MPE)
void ltoa(long L,char *S,int R)
{sprintf(S,"%ld",L);
}
#endif
