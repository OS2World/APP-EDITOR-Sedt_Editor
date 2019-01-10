/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"

#if defined(ATARI) & defined(GEM)
extern int TC_Height, TC_Width, T_Baseline, Buf_Points, wi_handle[];
#endif

void Fast Put_Time()
{
#if defined(VMS)
 char *S, C;
 Today_Desc.dsc$w_length=20;
 Today[20]=0;
 Lib$Date_Time(&Today_Desc);
 S=Today+12;
 while (C=(*S++))
  Ins_Char(C);
#endif
#if defined(DOS) | defined(NT) | defined(UNIX) | defined(WIN32)
 char *S, *T;
 long Time;
 Time=time(NULL);
 T=ctime(&Time);
 S=T+11;
 {register int I;
  for (I=1;I<=8;I++)
   Ins_Char(*S++);
 }
#endif
#if defined(ATARI)
 int Time;
 char T_String[14], C, *S;
 Time=Tgettime();
 sprintf(T_String,"%2d:%2.2d:%2.2d",(Time>>11)&0x1F,(Time>>5)&0x3F,
  (Time&0x1F)*2);
 S=T_String;
 while (C=(*(S++)))
  Ins_Char(C);
#endif
#if defined(OS2)
 struct _DATETIME Date_Time;
 char D_String[14], C, *S;
 DosGetDateTime((PDATETIME)&Date_Time);
 sprintf(D_String,"%2d:%2.2d:%2.2d",Date_Time.hours,Date_Time.minutes,
  Date_Time.seconds);
 S=D_String;
 while (C=(*(S++)))
  Ins_Char(C);
#endif
 Set_Changes();
}

void Fast Out_Error()
{if (!Get_Arg()||Skip)
  return;
 Plain_Error((int)Count,Com_Arg);
}

void Fast Other_Window()
{
 Ch_Window(Lst_Current>=0?Lst_Current:1);
}

void Fast Rul_Load()
{char C, K;
 long This_Pos;
 if (!Get_Entity()||Skip)
  return;
 if (E_Type==E_Paste)
  Clear_Region();
 {register int I;
  for (I=1;I<=512;I++)
   Tabs[I]=0;
 }
 Left_Margin=Word_Wrap=1;
 Right_Margin=1;
 Auto_Margin=0;
 B_Goto(Current,This_Pos=Beg_Pos);
 while (This_Pos!=End_Pos)
 {register long I;
  K=B_Get(Current);
  This_Pos++;
  if (isalpha(K))
  {C=K;
   I=0;
  } else
  if (isdigit(K))
   I=I*10+K-'0'; else
  if (K==' ')
  {switch (C)
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
    default:
    {Error(2,Ruler_Syntax_Txt);
     break;
    }
   }
   if (Err_Flag)
    break;
  } else
  {Error(2,Ruler_Syntax_Txt);
   break;
  }
 }
 if (Show_Ruler)
  S_Ruler();
}

void Fast Ins_String(char *S)
{int L;
 B_Goto(Current,My_Pos);
 B_Put_S(Current,S,strlen(S));
 Fill_Rest();
 Adj_Points((long) (L=strlen(S)),My_Pos);
 Init_Entity();
 End_Pos+=L;
 Move_Entity();
}

void Fast Rul_Save()
{char S[80];
 sprintf(S,"L%ld ",Left_Margin);
 Ins_String(S);
 sprintf(S,Auto_Margin?"A%ld ":"W%ld ",Word_Wrap);
 Ins_String(S);
 sprintf(S,Justify?"J%ld ":"R%ld ",Right_Margin);
 Ins_String(S);
 sprintf(S,"I%ld ",Indent);
 Ins_String(S);
 {register int I;
  for (I=1;I<=512;I++)
   if (Tabs[I])
   {sprintf(S,"T%d ",I);
    Ins_String(S);
   }
 }
}

void Fast Save_Keys()
{FILE *F;
 int Version, L;
 char *P, C;
 if (!Get_Arg()||Skip)
  return;
 if (Com_Arg[0]==0)
  strcpy(Com_Arg,Mapfile());
#if defined(UNIX)
 if ((F=fopen(Com_Arg,"w"))==NULL)
#else
 if ((F=fopen(Com_Arg,"wb"))==NULL)
#endif
 {Error(2,Map_Create_Error);
  return;
 }
 if (Err_Flag)
  return;
 Version=3;
 fwrite(&Version,sizeof(int),1,F);
 L=strlen(Init_T_String);
 fwrite(&L,sizeof(int),1,F);
 fwrite(Init_T_String,L,1,F);
 L=strlen(Reset_T_String);
 fwrite(&L,sizeof(int),1,F);
 fwrite(Reset_T_String,L,1,F);
 fwrite(&Scan_Defs,sizeof(int),1,F);
 fwrite(Scan_Tab,sizeof(struct Scan_Tab_Struct),Scan_Defs,F);
 fwrite(&Key_Defs,sizeof(int),1,F);
 fwrite(Key_Tab,sizeof(struct Key_Tab_Struct),Key_Defs,F);
 fwrite(&Comb_Defs,sizeof(int),1,F);
 fwrite(Comb_Tab,sizeof(struct Comb_Tab_Struct),Comb_Defs,F);
 fwrite(&Map_Defs,sizeof(int),1,F);
 fwrite(Map_Tab,sizeof(struct Map_Tab_Struct),Map_Defs,F);
 {register int I;
  for (I=0;I<Key_Defs;I++)
  if (Key_Tab[I].Index!=0)
  {B_Goto(&Pad,(long)Key_Tab[I].Index);
   P=Dsk_Rec;
   while ((C=B_Get(&Pad))!=0)
    *(P++)=C;
   *P=0;
   fwrite(Dsk_Rec,strlen(Dsk_Rec)+1,1,F);
  }
 }
 {register int I;
  for (I=0;I<Comb_Defs;I++)
  if (Comb_Tab[I].Index!=0)
  {B_Goto(&Pad,(long)Comb_Tab[I].Index);
   P=Dsk_Rec;
   while ((C=B_Get(&Pad))!=0)
    *(P++)=C;
   *P=0;
   fwrite(Dsk_Rec,strlen(Dsk_Rec)+1,1,F);
  }
 }
 {register int I;
  for (I=0;I<Map_Defs;I++)
  if (Map_Tab[I].Index!=0)
  {B_Goto(&Pad,(long)Map_Tab[I].Index);
   P=Dsk_Rec;
   while ((C=B_Get(&Pad))!=0)
    *(P++)=C;
   *P=0;
   fwrite(Dsk_Rec,strlen(Dsk_Rec)+1,1,F);
  }
 }
 fclose(F);
}

void Fast Save_Functions()
{FILE *F;
 long J, Base=0;
 char *P, C;
 if (!Get_Arg()||Skip)
  return;
 if (Com_Arg[0]==0)
  strcpy(Com_Arg,Keyfile());
#if defined(UNIX)
 if ((F=fopen(Com_Arg,"w"))==NULL)
#else
 if ((F=fopen(Com_Arg,"wb"))==NULL)
#endif
 {Error(2,Fun_Create_Error);
  return;
 }
 if (Err_Flag)
  return;
 {register int I;
  for (I=0;I<=Next_Func;I++)
  {if (Funcs[I][0]!=0)
   {if (Base==0)
     Base=Funcs[I][2]-1;
    Funcs[I][0]-=Base;
    if (I<=Top_Func)
     Funcs[I][2]-=Base;
   }
  }
 }
 fwrite(&Next_Func,sizeof(int),1,F);
 fwrite(Funcs,3*sizeof(long),Next_Func+1,F);
 {register int I;
  for (I=0;I<=Next_Func;I++)
  {if (Funcs[I][0]!=0)
   {Funcs[I][0]+=Base;
    if (I<=Top_Func)
     Funcs[I][2]+=Base;
   }
  }
 }
 {register int I;
  for (I=0;I<=Next_Func;I++)
  if (Funcs[I][2]!=0)
  {B_Goto(&Pad,(long)Funcs[I][2]);
   P=Dsk_Rec;
   while ((C=B_Get(&Pad))!=0)
    *(P++)=C;
   *P=C;
   fwrite(Dsk_Rec,strlen(Dsk_Rec)+1,1,F);
  }
 }
 {register int I;
  for (I=0;I<=Next_Func;I++)
  if (Funcs[I][0]!=0)
  {B_Goto(&Pad,(long)Funcs[I][0]);
   P=Dsk_Rec;
   J=Funcs[I][1];
   while (J--)
   {C=B_Get(&Pad);
    *(P++)=C;
   }
   fwrite(Dsk_Rec,(int)Funcs[I][1],1,F);
  }
 }
 fclose(F);
}

void Fast f_puti(FILE *F,int I)
{fputc('0'+(I/100)%10,F);
 fputc('0'+(I/10)%10,F);
 fputc('0'+I%10,F);
}

void Fast Strip_Trailing()
{long Sav_Pos, Start_Pos, L;
 char C;
 if (!Get_Entity()||Skip)
  return;
 if (E_Type==E_Paste)
  Clear_Region();
 Sav_Pos=My_Pos;
 Start_Pos=0;
 B_Goto(Current,My_Pos=Beg_Pos);
 while (My_Pos<End_Pos)
 {if ((C=B_Get(Current))==' '||C==9)
  {if (Start_Pos==0)
    Start_Pos=My_Pos;
  } else
#if ! (defined(UNIX) | defined(MPE))
  {if ((C=='\n'||C=='\r')&&Start_Pos!=0)
#else
  {if (C=='\n'&&Start_Pos!=0)
#endif
   {L=My_Pos-Start_Pos;
    End_Pos-=L;
    if (Sav_Pos>My_Pos)
     Sav_Pos-=L;
    B_Goto(Current,My_Pos=Start_Pos);
    B_Rem(Current,L);
    Adj_Points(-L,My_Pos);
    B_Get(Current);
   }
   Start_Pos=0;
  }
  My_Pos++;
 }
 My_Pos=Sav_Pos;
 if (My_Pos==Beg_Pos)
  Fill_Screen(0,Rel_Last); else
  Fill_Screen(Rel_First,0);
 Set_Changes();
}

void Fast Set_Tabs()
{long Sav_Pos, Start_Pos, L, Save_Col;
 char C;
 if (!Get_Entity()||Skip)
  return;
 if (E_Type==E_Paste)
  Clear_Region();
 Sav_Pos=My_Pos;
 if (Sav_Pos!=Beg_Pos)
 {B_Goto(Current,My_Pos=Beg_Pos);
  Backto_This(Beg_Pos);
 } else
  Save_Col=My_Col;
 B_Goto(Current,My_Pos);
 Start_Pos=0;
 while (My_Pos<End_Pos)
 {if (My_Col%8==1&&Start_Pos!=0)
  {L=My_Pos-Start_Pos-1;
   if (L!=0)
   {End_Pos-=L;
    if (Sav_Pos>My_Pos)
     Sav_Pos-=L;
    B_Goto(Current,Start_Pos);
    B_Rem(Current,L);
    Adj_Points(-L,Start_Pos);
    B_Replace(Current,9);
    My_Pos-=L;
   }
   Start_Pos=0;
  }
  if (Chk_EL(Current,C=B_Get(Current)))
  {My_Col=1;
#if ! (defined(UNIX) | defined(MPE))
   if (C=='\r')
    B_Goto(Current,(++My_Pos)+1);
#endif
   Start_Pos=0;
  } else
  {if (C==' ')
   {if (Start_Pos==0)
     Start_Pos=My_Pos;
   } else
    Start_Pos=0;
   My_Col+=Char_Rep(C,NULL);
  }
  My_Pos++;
 }
 My_Pos=Sav_Pos;
 if (Sav_Pos!=Beg_Pos)
 {B_Goto(Current,My_Pos);
  Backto_This(My_Pos);  
 } else
  My_Col=Save_Col;
 Set_Changes();
}

void Fast Remove_Tabs()
{long Save_Col, Sav_Pos, L;
 char C;
 if (!Get_Entity()||Skip)
  return;
 if (E_Type==E_Paste)
  Clear_Region();
 Sav_Pos=My_Pos;
 if (Sav_Pos!=Beg_Pos)
 {B_Goto(Current,My_Pos=Beg_Pos);
  Backto_This(Beg_Pos);
 } else
  Save_Col=My_Col;
 B_Goto(Current,My_Pos);
 while (My_Pos<End_Pos)
 {if (Chk_EL(Current,C=B_Get(Current)))
  {My_Col=1;
#if ! (defined(UNIX) | defined(MPE))
   if (C=='\r')
    B_Goto(Current,(++My_Pos)+1);
#endif
  } else
  if (C==9)
  {B_B_Get(Current);
   B_Replace(Current,' ');
   L=0;
   while (My_Col%8!=0)
   {L++;
    B_Put(Current,' ');
    My_Col++;
   }
   My_Col++;
   if (L!=0)
   {End_Pos+=L;
    if (Sav_Pos>My_Pos)
     Sav_Pos+=L;
    Adj_Points(L,My_Pos);
    My_Pos+=L;
   }
  } else
   My_Col+=Char_Rep(C,NULL);
  My_Pos++;
 }
 My_Pos=Sav_Pos;
 if (Sav_Pos!=Beg_Pos)
 {B_Goto(Current,My_Pos);
  Backto_This(My_Pos);  
 } else
  My_Col=Save_Col;
 Set_Changes();
}

void Fast Set_Edit_Region()
{if (!Get_Entity()||Skip)
  return;
 Points[Maxpoints+3]=Beg_Pos;
 Points[Maxpoints+4]=End_Pos;
 if (E_Type==E_Paste)
  Clear_Region();
}

void Fast Cancel_Edit_Region()
{Points[Maxpoints+3]=1;
 Points[Maxpoints+4]=Current->Bufsize+1;
}

void Fast Key_Mess()
{Plain_Message(Learn_Key_Txt);
}

void Fast Clear_Region()
{if (Ent_Direction)
  Fill_Screen(0,(int)(End_Line-Beg_Line)>Rel_Last?Rel_Last:
   (int)(End_Line-Beg_Line)); else
  Fill_Screen((int)(Beg_Line-End_Line)<Rel_First?Rel_First:
   (int)(Beg_Line-End_Line),0);
}

void Fast Counter_Get()
{int OK=0, C, Sign=1;
 if (!Get_Counter_Index()||Skip)
  return;
 Set_Region();
 B_Goto(Current,My_Pos);
 Count=0;
 while (1)
 {if (Current->Pos>=End_Region)
  {Error(2,No_Number_Txt);
   return;
  }
  if ((C=B_Get(Current))!=' '&&C!=9)
  {B_B_Get(Current);
   break;
  }
 }
 if (Current->Pos<End_Region)
 {if ((C=B_Get(Current))=='+'||C=='-')
  {if (C=='-')
   Sign= -1;
  } else
   B_B_Get(Current);
 }
 while (Current->Pos<End_Region)
 {if ((C=B_Get(Current))>='0'&&C<='9')
  {Count=Count*10+C-'0';
   OK=1;
  } else
   break;
 }
 if (OK)
  Counter[Counter_Index]=Count*Sign; else
  Error(2,No_Number_Txt);
}

void Fast Counter_Add()
{int OK=0, C, Sign=1;
 if (!Get_Counter_Index()||Skip)
  return;
 Set_Region();
 B_Goto(Current,My_Pos);
 Count=0;
 while (1)
 {if (Current->Pos>=End_Region)
  {Error(2,No_Number_Txt);
   return;
  }
  if ((C=B_Get(Current))!=' '&&C!=9)
  {B_B_Get(Current);
   break;
  }
 }
 if (Current->Pos<End_Region)
 {if ((C=B_Get(Current))=='+'||C=='-')
  {if (C=='-')
   Sign= -1;
  } else
   B_B_Get(Current);
 }
 while (Current->Pos<End_Region)
 {if ((C=B_Get(Current))>='0'&&C<='9')
  {Count=Count*10+C-'0';
   OK=1;
  } else
   break;
 }
 if (OK)
  Counter[Counter_Index]+=Count*Sign; else
  Error(2,No_Number_Txt);
}

int Fast Get_Counter_Index()
{int C;
 if (Com.Pos>Com.Bufsize)
  Counter_Index=0; else
 if ((C=B_Get(&Com))=='#')
 {if (Com.Pos>Com.Bufsize)
  {Counter_Index=0;
   Syntax_Error();
   return(0);
  }
  C=B_Get(&Com);
  C=toupper(C);
  if (C>='A'&&C<='Z')
   Counter_Index=C-'A'; else
  {Syntax_Error();
   return 0;
  }
 } else
 {Counter_Index=0;
  B_B_Get(&Com);
 }
 return 1;
}

void Fast Query()
{if (!Get_Arg()||Skip)
  return;
 if (Yes_No(Com_Arg))
  Err_Flag=1; else
 if (Err_Flag)
  Severe_Error=1;
}

void Fast Execute_Key()
{long
  *A;
 long P, O;
 Gold=Function=0;
 Get_Key();
 if (Err_Flag||Skip)
  return;
 if ((Function=Map_Tab[Map_Index].F_Code)<=0)
 {Error(2,Unassigned_Txt);
  return;
 }
 A=Key_Addr(Function);
 O=Com.Pos;
 P=Com.Bufsize+1;
 B_Goto(&Com,P);
 B_Goto(&Pad,A[0]);
 B_Copy(&Pad,&Com,A[1]);
 B_Goto(&Com,P);
 Com_Direction=Def_Direction;
 Expand_Com(P);
 Com_Parse(P,0);
 B_Goto(&Com,P);
 B_Rem(&Com,Com.Bufsize-P+1);
 B_Goto(&Com,O);
}

void Fast Execute_Function()
{long
  *A;
 long P, O;
 if ((Function=Count)>Next_Func)
 {Error(2,Unassigned_Txt);
  return;
 }
 A=Key_Addr(Function);
 O=Com.Pos;
 P=Com.Bufsize+1;
 B_Goto(&Com,P);
 B_Goto(&Pad,A[0]);
 B_Copy(&Pad,&Com,A[1]);
 B_Goto(&Com,P);
 Com_Direction=Def_Direction;
 Expand_Com(P);
 Com_Parse(P,0);
 B_Goto(&Com,P);
 B_Rem(&Com,Com.Bufsize-P+1);
 B_Goto(&Com,O);
}

void Fast Diag_Error()
{Error(2,Block_Sel_Txt);
}

void Fast Pos_Cursor()
{Rel_First+=Home;
 Rel_Last+=Home;
 Margin=Requested_Margin>(Rel_Last-Rel_First)/2?(Rel_Last-Rel_First)/2:Requested_Margin;
 My_Line=Home=!Float_Cursor?(Rel_First+Rel_Last+1)/2:
  This_Line<=Rel_Last-Rel_First?Rel_First+This_Line-1:(Rel_First+Rel_Last+1)/2;
 Rel_First-=Home;
 Rel_Last-=Home;
}

void Fast Set_Offset(int I)
{I-=Rel_First+Rel_Last;
 Margin=Requested_Margin>(Rel_Last-Rel_First)/2?(Rel_Last-Rel_First)/2:Requested_Margin;
 if (This_Line<1-Rel_First)
 {I=1-Rel_First-This_Line;
  Home=My_Line-=I;
  Rel_First+=I;
  Rel_Last+=I;
 } else
 if (I>0)
 {while ((I-=2)>=0&&Rel_First<(This_Line==1?0:-1)&&-Rel_First>Margin)
  {Home= --My_Line;
   Rel_First++;
   Rel_Last++;
  }
 } else
 if (I<0)
 {while ((I+=2)<=0&&Rel_Last>1&&This_Line!=1-Rel_First&&Rel_Last>Margin)
  {Home= ++My_Line;
   Rel_First--;
   Rel_Last--;
  }
 }
}

void Fast Adj_Lines(long L)
{Buf_Lines+=L;
}

void Fast Move_Key(long *Where,long Start,long Length)
{Rem_Pad(Where);
 Where[0]=Pad.Bufsize+1;
 Where[1]=Length;
 B_Goto(&Pad,Pad.Bufsize+1);
 B_Goto(&Com,Start);
 B_Copy(&Com,&Pad,Length);
}

void Fast Init_Entity()
{Ent_Direction=Ent_Count=1;
 Beg_Pos=End_Pos=My_Pos;
 Beg_Line=End_Line=This_Line;
 B_Goto(Current,My_Pos);
}

void Fast Blk_Rem_It(int Index)
{register long Lines, This_Pos;
 int C, I, Removing;
 long St_Col, End_Col, B_End, Sav_Col, Save_Right;
 if (Index>=0)
 {if (!Append_It)
   Erase_Del(Dl_Pointers[Index]); else
   B_Goto(&Del,Dl_Pointers[Index][0]+Dl_Pointers[Index][1]);
 }
 if (Err_Flag)
  return;
 Removing=This_Cmd==C_Delete;
 if (End_Pos>Points[Maxpoints+4])
 {Error(2,End_Region_Txt);
  return;
 }
 Save_Right=Right_Margin;
 Right_Margin=1;
 B_End=Points[Maxpoints+4];
 if (!Append_It)
  Dl_Positions[Index]=End_Pos==My_Pos;
 St_Col=My_Col;
 Move_Entity();
 End_Col=My_Col;
 if (My_Pos==End_Pos)
 {Ent_Count=1;
  Ent_Direction=0;
  Move_Entity();
  if (St_Col>End_Col)
  {Diag_Error();
   Right_Margin=Save_Right;
   return;
  }
 } else
 if (St_Col>End_Col)
 {I=St_Col;
  St_Col=End_Col;
  End_Col=I;
 } else
 if (St_Col!=End_Col)
 {Diag_Error();
  Right_Margin=Save_Right;
  return;
 }
 This_Pos=My_Pos;
 Sav_Col=My_Col;
 B_Goto(Current,My_Pos);
 Lines=0;
 while (My_Pos<=End_Pos)
 {if (My_Col>=St_Col&&My_Col<=End_Col)
  {if (My_Pos<B_End&&Chk_EL(Current,C=B_Get(Current)))
   {while (My_Col++<=End_Col)
     B_Put(&Del,' ');
    if (My_Pos!=End_Pos)
    {My_Col=1;
#if ! (defined(UNIX) | defined(MPE))
     B_Put(&Del,'\r');
#endif
     B_Put(&Del,'\n');
     Lines++;
    }
    My_Pos++;
#if ! (defined(UNIX) | defined(MPE))
    if (C=='\r')
     B_Goto(Current,++My_Pos);
#endif
   } else
   {if (My_Pos<B_End)
    {B_Goto(Current,Current->Pos-1);
     if (!Removing)
     {My_Col+=Char_Rep(B_Get(Current),NULL);
      My_Pos++;
     } else
     if (Replace_Mode)
     {My_Col+=Char_Rep(B_Replace(Current,' '),NULL);
      My_Pos++;
     } else
     {My_Col+=Char_Rep(B_Del(Current),NULL);
      Adj_Points((long)-1,My_Pos);
      End_Pos--;
      B_End--;
     }
    } else
    {C=' ';
     My_Pos++;
     My_Col++;
    }
    if (My_Col>End_Col)
    {B_Put(&Del,(char)C);
     if (My_Pos<End_Pos)
     {
#if ! (defined(UNIX) | defined(MPE))
      B_Put(&Del,'\r');
#endif
      B_Put(&Del,'\n');
      Lines++;
     }
    } else
     B_Put(&Del,(char)C);
   }
  } else
  {C=My_Pos<B_End?B_Get(Current):' ';
   My_Pos++;
   if (C=='\n')
   {if (My_Col<St_Col)
    {for (My_Col=St_Col;My_Col<=End_Col;My_Col++)
      B_Put(&Del,' ');
#if ! (defined(UNIX) | defined(MPE))
     B_Put(&Del,'\r');
#endif
     B_Put(&Del,'\n');
     Lines++;
    }
    My_Col=1;
   } else
    My_Col+=Char_Rep(C,NULL);
  }
 }
 if (Index>=0)
 {if (Append_It)
  {Adj_Del(Dl_Pointers[Index][1],Del.Pos-Dl_Pointers[Index][0]-
    Dl_Pointers[Index][1]);
  }
  Dl_Pointers[Index][1]=Del.Pos-Dl_Pointers[Index][0];
 }
 My_Col=Sav_Col;
 My_Pos=This_Pos;
 if (Removing)
 {if (Lines>0)
  {if (Lines<Rel_Last)
    Fill_Screen(0,(int)Lines); else
    Fill_Screen(0,Rel_Last);
  } else
   Fill_Rest();
 }
 Right_Margin=Save_Right;
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (Index==Past_String)
  Win_Put_Clip(Lines);
#endif
}

void Fast Rem_It(int Index)
{register long Lines, Chars, This_Pos;
 int C, L;
 long Sel_Pos;
 if (Index>=0)
 {if (!Append_It||Dl_Pointers[Index][0]==0)
   Erase_Del(Dl_Pointers[Index]);
  B_Goto(&Del,Dl_Pointers[Index][0]+Dl_Pointers[Index][1]);
 }
 if (Err_Flag)
  return;
 if (This_Cmd==C_Delete||This_Cmd==C_Center||This_Cmd==C_Undo||This_Cmd==C_Rep)
 {Chars=End_Pos-Beg_Pos;
  Lines=End_Line-Beg_Line;
  if (Index>=0&&!Append_It)
   Dl_Positions[Index]=End_Pos==My_Pos;
  B_Goto(Current,Beg_Pos);
  if (Index>=0)
  {B_Copy(Current,&Del,Chars);
   B_Goto(Current,Beg_Pos);
   if (Append_It)
   {Dl_Pointers[Index][1]+=Chars;
    Adj_Del(Dl_Pointers[Index][0],Chars);
   } else
    Dl_Pointers[Index][1]=Chars;
  }
  if (Replace_Mode)
  {This_Pos=Beg_Pos;
   while (This_Pos!=End_Pos)
   {This_Pos++;
    if ((C=B_Get(Current))!='\n')
    {
#if ! (defined(UNIX) | defined(MPE))
     if (C=='\r'&&This_Pos!=End_Pos)
     {if (B_Get(Current)=='\n')
       This_Pos++; else
      {B_Goto(Current,This_Pos-1);
       B_Replace(Current,' ');
      }
     } else
     {B_Goto(Current,This_Pos-1);
      B_Replace(Current,' ');
     }
#else
     B_Goto(Current,This_Pos-1);
     B_Replace(Current,' ');
#endif
    }
   }
   if (My_Pos==Beg_Pos)
   {if (Lines>0)
     Fill_Screen(0,Lines>Rel_Last?Rel_Last:(int) Lines); else
     Fill_Rest();
   } else
   {if (Lines>0)
    {if (Lines>-Rel_First)
      Fill_Screen(Rel_First,0); else
      Fill_Screen(-(int) Lines,0);
    } else
     Fill_Screen(0,0);
   }
   Move_Entity();
  } else
  {B_Rem(Current,Chars);
   Adj_Points(-Chars,Beg_Pos);
   if (My_Pos==Beg_Pos)
   {if (Lines>0)
    {Adj_Lines(-Lines);
     if (Lines<Rel_Last&&(Max_Scroll?Max_Scroll>=Lines:1))
     {Opt_Scroll((int)Lines,1,Rel_Last);
      Fill_Rest();
      Fill_Screen((int)(Rel_Last-Lines+1),Rel_Last);
     } else
      Fill_Screen(0,Rel_Last);
    } else
     Fill_Rest();
   } else
   {My_Pos-=Chars;
    This_Line-=Lines;
    B_Goto(Current,My_Pos);
    Backto_This(My_Pos);
    if (Lines>0)
    {Adj_Lines(-Lines);
     if (Lines<-Rel_First&&(Max_Scroll?Max_Scroll>=Lines:1))
     {if (Float_Cursor&&This_Line<1-Rel_First)
      {Rel_First+=Lines;
       Rel_Last+=Lines;
       My_Line=Home-=Lines;
       Opt_Scroll((int)Lines,1,Rel_Last);
       Fill_Rest();
       Fill_Screen((int)(Rel_Last-Lines),Rel_Last);
      } else
      {Opt_Scroll((int)-Lines,Rel_First,-1);
       Fill_Screen(0,0);
       Fill_Screen(Rel_First,(int)(Rel_First+Lines-1));
      }
     } else
      Fill_Screen(Rel_First,0);
    } else
     Fill_Rest();
    if (Float_Cursor&&This_Line<1-Rel_First)
    {L=1-Rel_First-This_Line;
     Opt_Scroll(L,Rel_First,Rel_Last);
     Rel_First+=L;
     Rel_Last+=L;
     My_Line=Home-=L;
     Fill_Screen(Rel_Last-L+1,Rel_Last);
    }
   }
  }
 } else
 {Chars=End_Pos-Beg_Pos;
  Lines=End_Line-Beg_Line;
  B_Goto(Current,Beg_Pos);
  B_Copy(Current,&Del,Chars);
  if (Append_It)
  {Dl_Pointers[Index][1]+=Chars;
   Adj_Del(Dl_Pointers[Index][0],Chars);
  } else
  {Dl_Pointers[Index][1]=Chars;
   Dl_Positions[Index]=End_Pos==My_Pos;
  }
 }
 if (Blk_Mode&&(Sel_Pos=Points[0])!=0)
 {if (Sel_Pos<My_Pos)
   Fill_Screen(Rel_First,0);
 }
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (Index==Past_String)
  Win_Put_Clip(Lines);
#endif
}

void Fast Wr_Buffer(char *O_File)
{FILE *F;
#if defined(VMS)
 char Backupfile[80];
#else
 char Backupfile[512];
#endif
#if defined(UNIX) | defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 struct stat S_Buffer;
 int Got_File;
#endif
 if (O_File==NULL)
  return;
#if defined(UNIX)
 if ((Got_File=(stat(O_File,&S_Buffer)==0))&&S_Buffer.st_mode&0040000)
 {Error(2,Dir_Save_Txt);
  return;
 }
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32) 
 if ((Got_File=(stat(O_File,&S_Buffer)==0))&&S_Buffer.st_mode&S_IFDIR)
 {Error(2,Dir_Save_Txt);
  return;
 }
#endif
 if (Err_Flag)
  return;
 if (!Nobak&&(F=fopen(O_File,"r"))!=NULL)
 {fclose(F);
#if defined(MPE)
  strcpy(Backupfile,tmpnam(NULL));
#else
  Filnam(O_File,Backupfile);
#if defined(UNIX)
  strcat(Backupfile,".bak");
#else
  strcat(Backupfile,".BAK");
#endif
#endif
#if defined(VMS)
  delete(Backupfile);
  rename(O_File,Backupfile);
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  unlink(Backupfile);
  rename(O_File,Backupfile);
#if defined(DOS)
  if (Reservation&&File_Reserved[Cur_Buffer])
  {chmod(Backupfile,File_Protection[Cur_Buffer]);
  }
#endif
#endif
#if defined(UNIX)
  unlink(Backupfile);
  link(O_File,Backupfile);
  unlink(O_File);
  if (Reservation&&File_Reserved[Cur_Buffer])
  {chmod(Backupfile,File_Protection[Cur_Buffer]);
  }
#endif
#if defined(ATARI)
  unlink(Backupfile);
  Frename(0,O_File,Backupfile);
#endif
#if defined(MPE)
  remove(Backupfile);
  rename(O_File,Backupfile);
#endif
 }
 B_Save(Current,O_File);
 if (Err_Flag)
 {if (!Nobak)
  {
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(WIN32)
	unlink(O_File);
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
	rename(Backupfile,O_File);
#endif
#if defined(VMS)
	rename(Backupfile,O_File);
#endif
#if defined(UNIX)
	link(Backupfile,O_File);
	unlink(Backupfile);
#endif
#if defined(ATARI)
	Frename(0,O_File,Backupfile);
#endif
#if defined(MPE)
	rename(Backupfile,O_File);
#endif
  }
 } else
 {
#if defined(UNIX) | defined(DOS) | defined(WIN32)
  if (Got_File)
	chmod(O_File,S_Buffer.st_mode);
#endif
#if defined(MPE)
  remove(Backupfile);
#endif
  if (Auto_Saved)
	Rem_Autosave();
  if (Save_Context)
	Save_The_Context(O_File,Save_Context);
 }
}

void Fast Save_Auto(char *O_File)
{
#if defined(VMS)
 char Autofile[80];
#else
 char Autofile[512];
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 struct stat S_Buffer;
 if (stat(O_File,&S_Buffer)==0&&S_Buffer.st_mode&S_IFDIR)
 {Error(2,Dir_Save_Txt);
  return;
 }
#endif
#if defined(UNIX)
 struct stat S_Buffer;
 if (stat(O_File,&S_Buffer)==0&&S_Buffer.st_mode&0040000)
 {Error(2,Dir_Save_Txt);
  return;
 }
#endif
 if (O_File==NULL)
  return;
 Filnam(O_File,Autofile);
#if defined(UNIX)
 strcat(Autofile,".aus");
#else
 strcat(Autofile,".AUS");
#endif
 if (Auto_Saved)
#if defined(VMS)
  delete(Autofile);
#else
  unlink(Autofile);
#endif
 B_Save(Current,Autofile);
}

void Fast Set_Key(long *Where,int Value)
{Rem_Pad(Where);
 Where[0]=0;
 Where[1]=Value;
}

void Fast Undelete(int What)
/*Undeletes from the appropriate buffer*/
{register long Lines, Chars, *P, Sav_Col;
 int C;
 long Sav_Count;
 B_Goto(Current,My_Pos);
 if ((P=Dl_Pointers[What])[0]==0)
  return;
 B_Goto(&Del,P[0]);
 if ((Chars=P[1])==0)
  return;
 if (What==Past_String&&Blk_Mode)
 {Sav_Col=My_Col;
  Points[Maxpoints+1]=My_Pos;
  while (Chars--)
  {if ((C=B_Get(&Del))=='\n')
   {Init_Entity();
    Line();
    Move_Entity();
    if (My_Col!=1)
    {Sav_Count=Count;
     Count=1;
     New_Line((long)0);
     Count=Sav_Count;
     Err_Flag=0;
    }
    while (My_Col<Sav_Col)
    {if (My_Pos>=Points[Maxpoints+4])
      Ins_Char(' '); else
     {B_Goto(Current,My_Pos);
      if (Chk_EL(Current,C=B_Get(Current)))
      {while (My_Col<Sav_Col)
        Ins_Char(' ');
      } else
      {Init_Entity();
       A_Char();
       Move_Entity();
      }
     }
    }
   } else
#if ! (defined(UNIX) | defined(MPE))
   if (C=='\r')
   {if (Chars)
    {if (B_Get(&Del)!='\n')
      Ins_Char(C);
     B_Goto(&Del,Del.Pos-1);
    } else
     Ins_Char(C);
   } else
#endif
    Ins_Char(C);
  }
  Init_Entity();
  Ent_Direction=0;
  Ent_Count=My_Pos-Points[Maxpoints+1];
  A_Char();
  Move_Entity();
 } else
 if (Replace_Mode)
 {Points[Maxpoints+1]=My_Pos;
  while (Chars--)
  {if ((C=B_Get(&Del))=='\n')
   {if (Current->Pos>=Points[Maxpoints+4])
    {Sav_Count=Count;
     Count=1;
     New_Line((long)0);
     Count=Sav_Count;
    } else
    {Init_Entity();
     Ent_Direction=1;
     Line();
     Move_Entity();
     if (My_Col!=1)
     {Sav_Count=Count;
      Count=1;
      New_Line((long)0);
      Count=Sav_Count;
     }
    }
   } else
#if ! (defined(UNIX) | defined(MPE))
   if (C=='\r')
   {if (Chars)
    {if (B_Get(&Del)!='\n')
      Ins_Char(C);
     B_Goto(&Del,Del.Pos-1);
    } else
     Ins_Char(C);
   } else
#endif
    Ins_Char(C);
  }
  if (!Dl_Positions[What])
  {Init_Entity();
   Ent_Direction=0;
   Ent_Count=My_Pos-Points[Maxpoints+1];
   A_Char();
   Move_Entity();
   Lst_Char_Move=Beg_Pos-End_Pos;
   Lst_Line_Move=Beg_Line-End_Line;
  }
 } else
 {Adj_Points(Chars,My_Pos);
  B_Copy(&Del,Current,Chars);
  B_Goto(Current,My_Pos);
  Lines=0;
  while (B_Search(Current,'\n',My_Pos+Chars))
   Lines++;
  B_Goto(Current,My_Pos);
  if (Lines)
  {Adj_Lines(Lines);
   if (Lines<Rel_Last&&(Max_Scroll?Max_Scroll>=Lines:1))
   {Opt_Scroll((int)-Lines,0,Rel_Last);
    Fill_Screen(0,(int)Lines);
   } else
    Fill_Screen(0,Rel_Last);
  } else
   Fill_Rest();
  if (Dl_Positions[What])
  {Init_Entity();
   End_Pos+=Chars;
   End_Line+=Lines;
   Move_Entity();
   Lst_Char_Move=Chars;
   Lst_Line_Move=Lines;
  } else
  {Lst_Char_Move= -Chars;
   Lst_Line_Move= -Lines;
  }
 }
 This_Cmd=C_Movecursor;
 Set_Changes();
}

int Fast Chk_EL(struct Buffer *B,int C)
{if (C=='\n')
  return 1;
#if ! (defined(UNIX) | defined(MPE))
 if (C=='\r')
 {if (B->Pos<=B->Bufsize)
  {C=B_Get(B);
   B_B_Get(B);
   return C=='\n';
  } else
   return 0;
 } else
#endif
  return 0;
}

void Fast Sav_Window(int W)
{if (Dl_Pointers[W+Pointers][0]!=0)
 {B_Goto(&Del,Dl_Pointers[W+Pointers][0]);
  B_Repl_S(&Del,(char *)&My_Col,sizeof(My_Col));
  B_Repl_S(&Del,(char *)&My_Pos,sizeof(My_Pos));
  B_Repl_S(&Del,(char *)&This_Line,sizeof(This_Line));
  B_Repl_S(&Del,(char *)&Indent,sizeof(Indent));
  B_Repl_S(&Del,(char *)&Word_Wrap,sizeof(Word_Wrap));
  B_Repl_S(&Del,(char *)&Left_Margin,sizeof(Left_Margin));
  B_Repl_S(&Del,(char *)&Right_Margin,sizeof(Right_Margin));
  B_Repl_S(&Del,(char *)&Justify,sizeof(Justify));
  B_Repl_S(&Del,(char *)&Auto_Margin,sizeof(Auto_Margin));
  B_Repl_S(&Del,(char *)&Scr_Shift,sizeof(Scr_Shift));
  Offset=Rel_First+Rel_Last;
  B_Repl_S(&Del,(char *)&Offset,sizeof(Offset));
  B_Repl_S(&Del,(char *)&Auto_Count,sizeof(Auto_Count));
  B_Repl_S(&Del,(char *)&Auto_Saved,sizeof(Auto_Saved));
  B_Repl_S(&Del,(char *)&Buf_Lines,sizeof(Buf_Lines));
  B_Repl_S(&Del,(char *)&In_Help,sizeof(In_Help));
  B_Repl_S(&Del,(char *)&Help_Level,sizeof(Help_Level));
  B_Repl_S(&Del,(char *)Help_Levels,sizeof(Help_Levels));
  B_Repl_S(&Del,(char *)&At_End,sizeof(At_End));
  B_Repl_S(&Del,(char *)Points,sizeof(Points));
  B_Repl_S(&Del,(char *)End_Points,sizeof(End_Points));
  B_Repl_S(&Del,(char *)&Infile,sizeof(Infile));
  B_Repl_S(&Del,(char *)&Outfile,sizeof(Outfile));
  B_Repl_S(&Del,(char *)New_File,sizeof(New_File));
  B_Repl_S(&Del,(char *)&Main_Buffer,sizeof(Main_Buffer));
  B_Repl_S(&Del,(char *)&Scr_Width,sizeof(Scr_Width));
  B_Repl_S(&Del,(char *)&Scr_Length,sizeof(Scr_Length));
  B_Repl_S(&Del,(char *)&Home,sizeof(Home));
  B_Repl_S(&Del,(char *)&Rel_First,sizeof(Rel_First));
  B_Repl_S(&Del,(char *)&Rel_Last,sizeof(Rel_Last));
  B_Repl_S(&Del,(char *)&My_Line,sizeof(My_Line));
  B_Repl_S(&Del,(char *)Lst_Col,(Maxlines+1)*sizeof(int));
#if defined(ATARI) & defined(GEM)
  B_Repl_S(&Del,(char *)&TC_Height,sizeof(TC_Height));
  B_Repl_S(&Del,(char *)&TC_Width,sizeof(TC_Width));
  B_Repl_S(&Del,(char *)&T_Baseline,sizeof(T_Baseline));
  B_Repl_S(&Del,(char *)&Buf_Points,sizeof(Buf_Points));
  B_Repl_S(&Del,(char *)My_Screen,(Maxlines+1)*(Maxcol+1));
  B_Repl_S(&Del,(char *)My_Attributes,(Maxlines+1)*(Maxcol+1));
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
  B_Repl_S(&Del,(char *)&Buf_Font,sizeof(Buf_Font));
  B_Repl_S(&Del,(char *)&Char_Width,sizeof(Char_Width));
  B_Repl_S(&Del,(char *)&Char_Height,sizeof(Char_Height));
#endif
#if defined(DOS) & defined(TV)
  B_Repl_S(&Del,(char *)My_Screen,(Maxlines+1)*(Maxcol+1));
  B_Repl_S(&Del,(char *)My_Attributes,(Maxlines+1)*(Maxcol+1));
#endif
  B_Repl_S(&Del,(char *)&Top_Row,sizeof(Top_Row));
  B_Repl_S(&Del,(char *)&Left_Col,sizeof(Left_Col));
  B_Repl_S(&Del,(char *)&Zoomed,sizeof(Zoomed));
  B_Repl_S(&Del,(char *)&Save_Top,sizeof(Save_Top));
  B_Repl_S(&Del,(char *)&Save_Left,sizeof(Save_Left));
  B_Repl_S(&Del,(char *)&Save_Length,sizeof(Save_Length));
  B_Repl_S(&Del,(char *)&Save_Width,sizeof(Save_Width));
  B_Repl_S(&Del,(char *)Tabs,513*sizeof(int));
 } else
 {Dl_Pointers[W+Pointers][0]=Del.Bufsize+1;
  B_Goto(&Del,Dl_Pointers[W+Pointers][0]);
  B_Put_S(&Del,(char *)&My_Col,sizeof(My_Col));
  B_Put_S(&Del,(char *)&My_Pos,sizeof(My_Pos));
  B_Put_S(&Del,(char *)&This_Line,sizeof(This_Line));
  B_Put_S(&Del,(char *)&Indent,sizeof(Indent));
  B_Put_S(&Del,(char *)&Word_Wrap,sizeof(Word_Wrap));
  B_Put_S(&Del,(char *)&Left_Margin,sizeof(Left_Margin));
  B_Put_S(&Del,(char *)&Right_Margin,sizeof(Right_Margin));
  B_Put_S(&Del,(char *)&Justify,sizeof(Justify));
  B_Put_S(&Del,(char *)&Auto_Margin,sizeof(Auto_Margin));
  B_Put_S(&Del,(char *)&Scr_Shift,sizeof(Scr_Shift));
  Offset=Rel_First+Rel_Last;
  B_Put_S(&Del,(char *)&Offset,sizeof(Offset));
  B_Put_S(&Del,(char *)&Auto_Count,sizeof(Auto_Count));
  B_Put_S(&Del,(char *)&Auto_Saved,sizeof(Auto_Saved));
  B_Put_S(&Del,(char *)&Buf_Lines,sizeof(Buf_Lines));
  B_Put_S(&Del,(char *)&In_Help,sizeof(In_Help));
  B_Put_S(&Del,(char *)&Help_Level,sizeof(Help_Level));
  B_Put_S(&Del,(char *)Help_Levels,sizeof(Help_Levels));
  B_Put_S(&Del,(char *)&At_End,sizeof(At_End));
  B_Put_S(&Del,(char *)Points,sizeof(Points));
  B_Put_S(&Del,(char *)End_Points,sizeof(End_Points));
  B_Put_S(&Del,(char *)&Infile,sizeof(Infile));
  B_Put_S(&Del,(char *)&Outfile,sizeof(Outfile));
  B_Put_S(&Del,(char *)New_File,sizeof(New_File));
  B_Put_S(&Del,(char *)&Main_Buffer,sizeof(Main_Buffer));
  B_Put_S(&Del,(char *)&Scr_Width,sizeof(Scr_Width));
  B_Put_S(&Del,(char *)&Scr_Length,sizeof(Scr_Length));
  B_Put_S(&Del,(char *)&Home,sizeof(Home));
  B_Put_S(&Del,(char *)&Rel_First,sizeof(Rel_First));
  B_Put_S(&Del,(char *)&Rel_Last,sizeof(Rel_Last));
  B_Put_S(&Del,(char *)&My_Line,sizeof(My_Line));
  B_Put_S(&Del,(char *)Lst_Col,(Maxlines+1)*sizeof(int));
#if defined(ATARI) & defined(GEM)
  B_Put_S(&Del,(char *)&TC_Height,sizeof(TC_Height));
  B_Put_S(&Del,(char *)&TC_Width,sizeof(TC_Width));
  B_Put_S(&Del,(char *)&T_Baseline,sizeof(T_Baseline));
  B_Put_S(&Del,(char *)&Buf_Points,sizeof(Buf_Points));
  B_Put_S(&Del,(char *)My_Screen,(Maxlines+1)*(Maxcol+1));
  B_Put_S(&Del,(char *)My_Attributes,(Maxlines+1)*(Maxcol+1));
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
  B_Put_S(&Del,(char *)&Buf_Font,sizeof(Buf_Font));
  B_Put_S(&Del,(char *)&Char_Width,sizeof(Char_Width));
  B_Put_S(&Del,(char *)&Char_Height,sizeof(Char_Height));
#endif
#if defined(DOS) & defined(TV)
  B_Put_S(&Del,(char *)My_Screen,(Maxlines+1)*(Maxcol+1));
  B_Put_S(&Del,(char *)My_Attributes,(Maxlines+1)*(Maxcol+1));
#endif
  B_Put_S(&Del,(char *)&Top_Row,sizeof(Top_Row));
  B_Put_S(&Del,(char *)&Left_Col,sizeof(Left_Col));
  B_Put_S(&Del,(char *)&Zoomed,sizeof(Zoomed));
  B_Put_S(&Del,(char *)&Save_Top,sizeof(Save_Top));
  B_Put_S(&Del,(char *)&Save_Left,sizeof(Save_Left));
  B_Put_S(&Del,(char *)&Save_Length,sizeof(Save_Length));
  B_Put_S(&Del,(char *)&Save_Width,sizeof(Save_Width));
  B_Put_S(&Del,(char *)Tabs,513*sizeof(int));
 }
 Clr_Working();
}

void Fast Load_Window(int B)
{B_Goto(&Del,Dl_Pointers[Pointers+B][0]);
 B_Get_N(&Del,(char *)&My_Col,sizeof(My_Col));
 B_Get_N(&Del,(char *)&My_Pos,sizeof(My_Pos));
 B_Get_N(&Del,(char *)&This_Line,sizeof(This_Line));
 B_Get_N(&Del,(char *)&Indent,sizeof(Indent));
 B_Get_N(&Del,(char *)&Word_Wrap,sizeof(Word_Wrap));
 B_Get_N(&Del,(char *)&Left_Margin,sizeof(Left_Margin));
 B_Get_N(&Del,(char *)&Right_Margin,sizeof(Right_Margin));
 B_Get_N(&Del,(char *)&Justify,sizeof(Justify));
 B_Get_N(&Del,(char *)&Auto_Margin,sizeof(Auto_Margin));
 B_Get_N(&Del,(char *)&Scr_Shift,sizeof(Scr_Shift));
 B_Get_N(&Del,(char *)&Offset,sizeof(Offset));
 B_Get_N(&Del,(char *)&Auto_Count,sizeof(Auto_Count));
 B_Get_N(&Del,(char *)&Auto_Saved,sizeof(Auto_Saved));
 B_Get_N(&Del,(char *)&Buf_Lines,sizeof(Buf_Lines));
 B_Get_N(&Del,(char *)&In_Help,sizeof(In_Help));
 B_Get_N(&Del,(char *)&Help_Level,sizeof(Help_Level));
 B_Get_N(&Del,(char *)Help_Levels,sizeof(Help_Levels));
 B_Get_N(&Del,(char *)&At_End,sizeof(At_End));
 B_Get_N(&Del,(char *)Points,sizeof(Points));
 B_Get_N(&Del,(char *)End_Points,sizeof(End_Points));
 B_Get_N(&Del,(char *)&Infile,sizeof(Infile));
 B_Get_N(&Del,(char *)&Outfile,sizeof(Outfile));
 B_Get_N(&Del,(char *)New_File,sizeof(New_File));
 B_Get_N(&Del,(char *)&Main_Buffer,sizeof(Main_Buffer));
 B_Get_N(&Del,(char *)&Scr_Width,sizeof(Scr_Width));
 B_Get_N(&Del,(char *)&Scr_Length,sizeof(Scr_Length));
 B_Get_N(&Del,(char *)&Home,sizeof(Home));
 B_Get_N(&Del,(char *)&Rel_First,sizeof(Rel_First));
 B_Get_N(&Del,(char *)&Rel_Last,sizeof(Rel_Last));
 B_Get_N(&Del,(char *)&My_Line,sizeof(My_Line));
 B_Get_N(&Del,(char *)Lst_Col,(Maxlines+1)*sizeof(int));
#if defined(ATARI) & defined(GEM)
 B_Get_N(&Del,(char *)&TC_Height,sizeof(TC_Height));
 B_Get_N(&Del,(char *)&TC_Width,sizeof(TC_Width));
 B_Get_N(&Del,(char *)&T_Baseline,sizeof(T_Baseline));
 B_Get_N(&Del,(char *)&Buf_Points,sizeof(Buf_Points));
 B_Get_N(&Del,(char *)My_Screen,(Maxlines+1)*(Maxcol+1));
 B_Get_N(&Del,(char *)My_Attributes,(Maxlines+1)*(Maxcol+1));
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 B_Get_N(&Del,(char *)&Buf_Font,sizeof(Buf_Font));
 B_Get_N(&Del,(char *)&Char_Width,sizeof(Char_Width));
 B_Get_N(&Del,(char *)&Char_Height,sizeof(Char_Height));
#endif
#if defined(DOS) & defined(TV)
 B_Get_N(&Del,(char *)My_Screen,(Maxlines+1)*(Maxcol+1));
 B_Get_N(&Del,(char *)My_Attributes,(Maxlines+1)*(Maxcol+1));
#endif
 B_Get_N(&Del,(char *)&Top_Row,sizeof(Top_Row));
 B_Get_N(&Del,(char *)&Left_Col,sizeof(Left_Col));
 B_Get_N(&Del,(char *)&Zoomed,sizeof(Zoomed));
 B_Get_N(&Del,(char *)&Save_Top,sizeof(Save_Top));
 B_Get_N(&Del,(char *)&Save_Left,sizeof(Save_Left));
 B_Get_N(&Del,(char *)&Save_Length,sizeof(Save_Length));
 B_Get_N(&Del,(char *)&Save_Width,sizeof(Save_Width));
 B_Get_N(&Del,(char *)Tabs,513*sizeof(int));
 Clr_Working();
}

#if !((defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))))
void Fast Newwindow()
{
 if (Max_Length-4-Cur_Buffer>=4) 	/* 290a */ 
 {Scr_Length=Max_Length-4-Cur_Buffer;
  Top_Row=1+Cur_Buffer;
 } else
 {Scr_Length=2;
  Top_Row=Max_Length-5;
 }
 if (Max_Width-2-Cur_Buffer>=4)
 {Scr_Width=Max_Width-2-Cur_Buffer;
  Left_Col=1+Cur_Buffer;
 } else
 {Scr_Width=4;
  Left_Col=Max_Width-5;
 }
 Zoomed=0;
 if (Zoom_Windows)
 {Save_Left=Left_Col;
  Save_Top=Top_Row;
  Save_Length=Scr_Length;
  Save_Width=Scr_Width;
  Scr_Length=Max_Length-2; 	/* 290a */ 
  Top_Row=0;
  Scr_Width=Max_Width;
  Left_Col=0;
  Zoomed=1;
 }
 Out_Zap();
 Window_Ref[Cur_Buffer]=1;
 W_D_Border();
 Buffers++;
 {register int I;
  for (I=Buffers-1;I;I--)
   Buffer_Stack[I]=Buffer_Stack[I-1];
 }
 Buffer_Stack[0]=Cur_Buffer;
}
#endif

void Fast Ch_Window(int W)
/*Changes to window W*/
{
#if defined(ATARI) & defined(GEM)
 int Save_Update;
 if (!(Save_Update=Update_Flag))
  Update_On();
#endif
 if (Lst_Current!=Cur_Buffer&&Cur_Buffer!=W)
  Lst_Current=Cur_Buffer;
 W_S_Border();
 Sav_Window(Cur_Buffer);
 Mark_Cursor();
 Out_Zap();
 Cur_Buffer=W;
 Current=(&Main_Buffer);
 if (Dl_Pointers[Pointers+Cur_Buffer][0]==0)
 {Outfile=0;
#if !((defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))))
  Newwindow();
#endif
#if defined(ATARI) & defined(GEM)
  GEM_Newwindow(Cur_Buffer);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
  if (!Win_Newwindow(Cur_Buffer,1))
  {Error(2,No_Window_Txt);
   Cur_Buffer=Lst_Current;
   Current=(&Main_Buffer);
   return;
  }
#endif
#if defined (DOS) & defined(TV)
  if (TV_Newwindow(Cur_Buffer)==NULL)
  {Error(2,No_Window_Txt);
   Cur_Buffer=Lst_Current;
   Current=(&Main_Buffer);
   return;
  }
#endif
  This_Line=My_Col=My_Pos=1;
  Buf_Lines=0;
  Any_Changes[Cur_Buffer]=0;
  In_Help=Auto_Saved=0;
  Infile=Outfile=NULL;
  B_Init(&Main_Buffer);
  {register int I;
   for (I=0;I<Maxpoints+5;I++)
    Points[I]=0;
   for (I=0;I<Maxpoints+1;I++)
    End_Points[I]=0;
  }
#if defined(GEM) | defined(Windows) | defined(TV)
  Rel_First-=Home=((Rel_First=Show_Ruler?3:2)+(Rel_Last=Scr_Length-2)+1)/2;
  Rel_Last=Scr_Length-2-Home;
#else
  Rel_First-=Home=((Rel_First=Show_Ruler?3:2)+(Rel_Last=Scr_Length)+1)/2;
  Rel_Last=Scr_Length-Home;
#endif
  Pos_Cursor();
  Scr_Shift=0;
  Hdr_Changes=1;
  Points[Maxpoints+3]=Points[Maxpoints+4]=1;
  if (Infile!=NULL)
  {Read_File(Infile);
   if (!Err_Flag&&Save_Context)
    Read_Context();
  }
  S_Header();
  if (Show_Ruler)
   S_Ruler();
  Fill_Screen(Rel_First,Rel_Last);
#if defined(DOS) & defined(TV)
  TV_Draw_OK();
#endif
 } else
 {Load_Window(Cur_Buffer);
#if defined(ATARI) & defined(GEM)
  if (wi_handle[Cur_Buffer]==0)
  {GEM_Newwindow(Cur_Buffer);
   Zero();
   S_Header();
   if (Show_Ruler)
    S_Ruler();
   Fill_Screen(Rel_First,Rel_Last);
  } else
   GEM_Topwindow(Cur_Buffer);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
  if (Buf_Wnd[Cur_Buffer]==0)
  {Win_Newwindow(Cur_Buffer,1);
   Zero();
   S_Header();
   if (Show_Ruler)
    S_Ruler();
   Fill_Screen(Rel_First,Rel_Last);
  } else
   Win_Topwindow(Cur_Buffer);
#endif
#if defined(DOS) & defined(TV)
  if (Buf_Wnd[Cur_Buffer]==NULL)
  {TV_Newwindow(Cur_Buffer);
   Zero();
   S_Header();
   if (Show_Ruler)
    S_Ruler();
   Fill_Screen(Rel_First,Rel_Last);
  } else
   TV_Top_Window(Cur_Buffer);
#endif
  W_D_Border();
  {register int I;
   for (I=1;I<Buffers;I++)
    if (Buffer_Stack[I]==Cur_Buffer)
    {register int J;
     for (J=I;J;J--)
      Buffer_Stack[J]=Buffer_Stack[J-1];
     Buffer_Stack[0]=Cur_Buffer;
     break; 
    }
  }
  Hdr_Changes=1;
 }
#if !(((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM)))
 Repaint();
#endif
 Unmark_Cursor();
#if defined(ATARI) & defined(GEM)
 if (!Save_Update)
  Update_Off();
#endif
}

void Fast Com_Error()
{Error(2,Incomplete_Command_Txt);
 Severe_Error=1;
}

void Fast Syntax_Error()
{Error(2,Syntax_Error_Txt);
 Severe_Error=1;
}

void Fast Unk_Err()
{Error(2,Unknown_Command_Txt);
 Severe_Error=1;
}

void Fast Wr_Error()
{Error(2,Write_Error_Txt);
}

void Fast Set_Changes()
{Any_Changes[Cur_Buffer]|=1;
 if (Auto_Save&&Auto_Count==0)
  Auto_Count=1;
}

void Fast Clr_Changes()
{Any_Changes[Cur_Buffer]=0;
 Auto_Count=0;
}

void Fast Window_Position()
{
#if !((defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))))
 int Across, Down;
 switch ((int)Count)
 {
  case 0:
   Across=1;
   Down=1;
   break;
  case 1:
  case 2:
   Across=1;
   Down=2;
   break;
  case 3:
  case 4:
   Across=2;
   Down=1;
   break;
  case 5:
  case 6:
  case 7:
  case 8:
   Across=2;
   Down=2;
   break;
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
   Across=2;
   Down=3;
   break;
  default:
   Error(2,Bad_Argument_Txt);
   return;
 }
 Scr_Length=((Max_Length-2)/Down)-1;
 Scr_Width=(Max_Width/Across)-2;
 Zoomed=0;
 switch ((int)Count)
 {
  case 0:
   Top_Row=1;
   Left_Col=1;
   break;
  case 1:
  case 2:
   Top_Row=((Max_Length-2)/Down)*((Count-1)/Across)+1;
   Left_Col=(Max_Width/Across)*((Count-1)%Across)+1;
   break;
  case 3:
  case 4:
   Top_Row=((Max_Length-2)/Down)*((Count-3)/Across)+1;
   Left_Col=(Max_Width/Across)*((Count-3)%Across)+1;
   break;
  case 5:
  case 6:
  case 7:
  case 8:
   Top_Row=((Max_Length-2)/Down)*((Count-5)/Across)+1;
   Left_Col=(Max_Width/Across)*((Count-5)%Across)+1;
   break;
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
   Top_Row=((Max_Length-2)/Down)*((Count-9)/Across)+1;
   Left_Col=(Max_Width/Across)*((Count-9)%Across)+1;
   break;
 }
 Refresh_Window();
#endif
}

