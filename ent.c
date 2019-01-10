/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"

regexp *Exp=NULL;

void Fast regerror(char *S)
{Error(2,S);
}

extern long My_Col;

#if ! (defined(UNIX) | defined(MPE))
int Fast Chk_CRLF_F()
{if (End_Pos>Current->Bufsize||End_Pos<2)
  return 0;
 B_Goto(Current,End_Pos-1);
 if (B_Get(Current)=='\r'&&B_Get(Current)=='\n')
 {End_Pos++;
  End_Line++;
  return 1;
 }
 return 0;
}

int Fast Chk_CRLF_FB()
{if (End_Pos>Current->Bufsize||End_Pos<2)
  return 0;
 B_Goto(Current,End_Pos-1);
 if (B_Get(Current)=='\r'&&B_Get(Current)=='\n')
 {End_Pos--;
  return 1;
 }
 return 0;
}

int Fast Chk_CRLF_B()
{if (Beg_Pos>Current->Bufsize||Beg_Pos<2)
  return 0;
 B_Goto(Current,Beg_Pos-1);
 if (B_Get(Current)=='\r'&&B_Get(Current)=='\n')
 {Beg_Pos--;
  return 1;
 }
 return 0;
}

int Fast Chk_CRLF_BF()
{if (Beg_Pos>Current->Bufsize||Beg_Pos<2)
  return 0;
 B_Goto(Current,Beg_Pos-1);
 if (B_Get(Current)=='\r'&&B_Get(Current)=='\n')
 {Beg_Pos++;
  return 1;
 }
 return 0;
}
#endif

void Fast A_Char()
{int Save_Free;
 Save_Free=Cursor_Free;
 Cursor_Free=0;
 Char();
 Cursor_Free=Save_Free;
}

void Fast Char()
/*Goes Ent_Count chars in Ent_Direction*/
{Set_Region();
 if (Cursor_Free)
 {B_Goto(Current,My_Pos);
  if (Ent_Direction)
  {while (Ent_Count--)
   {if (My_Pos==End_Region)
    {Put_A_Char(' ');
     Set_Changes();
     End_Region++;
    } else
    if (Chk_EL(Current,B_Get(Current)))
    {B_B_Get(Current);
     Put_A_Char(' ');
     Set_Changes();
     End_Region++;
    }
    End_Pos++;
   }
  } else
  {while (Ent_Count--)
   {if (My_Pos==Beg_Region)
     ; else
    if (B_B_Get(Current)=='\n')
    {B_Get(Current);
    } else
     Beg_Pos--;
   }
  }
 } else
 {if (Ent_Direction)
  {if ((End_Pos=My_Pos+Ent_Count)>End_Region)
   {End_Error();
    Err_Flag=0;
    Toofar=1;
    End_Pos=End_Region;
   }
   Cnt_F_Lines(End_Pos);
#if ! (defined(UNIX) | defined(MPE))
   Chk_CRLF_F();
#endif
  } else
  {if ((Beg_Pos=My_Pos-Ent_Count)<Beg_Region)
   {Beg_Error();
    Err_Flag=0;
    Toofar=1;
    Beg_Pos=Beg_Region;
   }
   Cnt_B_Lines(Beg_Pos);
#if ! (defined(UNIX) | defined(MPE))
   Chk_CRLF_B();
#endif
  }
 }
 /*Diag("Char:Beg_Line=%D,End_Line=%D,Beg_Pos=%D,End_Pos=%D",
  Beg_Line,End_Line,Beg_Pos,End_Pos);*/
}

void Fast Abs_Char()
{if (Ent_Count>My_Pos)
 {Ent_Count-=My_Pos;
  Ent_Direction=1;
 } else
 {Ent_Count=My_Pos-Ent_Count;
  Ent_Direction=0;
 }
 A_Char();
}

void Fast Vertical()
/*Vertical movement*/
{register int C;
 static long V_Col, T_Col, V_Line;
 long Temp_Col, Sav_Col;
 Set_Region();
 B_Goto(Current,My_Pos);
 /*Diag("Vertical:My_Pos=%D,Ent_Count=%d,Ent_Direction=%d",My_Pos,Ent_Count,
  Ent_Direction);*/
 if (This_Line!=V_Line||My_Col!=V_Col)
  T_Col=My_Col;
 if (Ent_Direction)
 {while (Ent_Count--&&!Check_CC())
  {if (!B_Search(Current,'\n',End_Region))
   {End_Error();
    return;
   }
   End_Pos=Current->Pos;
   End_Line++;
  }
  Sav_Col=My_Col;
  My_Col=1;
  while (My_Col<T_Col)
  {if (End_Pos>=End_Region)
    break;
   End_Pos++;
   if (Chk_EL(Current,C=B_Get(Current)))
   {End_Pos--;
    break;
   }
   My_Col+=Char_Rep(C,NULL);
  }
  if (Cursor_Free&&My_Col<T_Col)
  {B_Goto(Current,End_Pos);
   Set_Changes();
   while (My_Col<T_Col)
   {Put_A_Char(' ');
    End_Pos++;
    My_Col++;
   }
  }
  Temp_Col=My_Col;
  My_Col=Sav_Col;
  V_Line=End_Line;
 } else
 {while (Ent_Count--&&!Check_CC())
  {if (!B_B_Search(Current,'\n',Beg_Region))
   {Beg_Error();
    return;
   }
   Beg_Pos=Current->Pos;
   Beg_Line--;
  }
  while (Beg_Pos!=Beg_Region)
  {Beg_Pos--;
   if ((C=B_B_Get(Current))=='\n')
   {C=B_Get(Current);
    Beg_Pos++;
    break;
   }
  }
  Sav_Col=My_Col;
  My_Col=1;
  while (My_Col<T_Col)
  {if (Beg_Pos>=End_Region)
    break;
   Beg_Pos++;
   if ((C=B_Get(Current))=='\n')
   {Beg_Pos--;
    break;
   }
   My_Col+=Char_Rep(C,NULL);
  }
#if ! (defined(UNIX) | defined(MPE))
  if (Chk_CRLF_B())
   My_Col-=Char_Rep('\r',NULL);
#endif
  if (Cursor_Free&&T_Col>My_Col)
  {Set_Changes();
   B_Goto(Current,Beg_Pos);
   while (T_Col>My_Col)
   {Put_A_Char(' ');
    My_Pos++;
    End_Region++;
    Beg_Pos++;
    End_Pos++;
    My_Col++;
   }
  }
  Temp_Col=My_Col;
  My_Col=Sav_Col;
  V_Line=Beg_Line;
 }
 V_Col=Temp_Col;
 /*Diag("Vertical:Beg_Line=%D,End_Line=%D,Beg_Pos=%D,End_Pos=%D",
  Beg_Line,End_Line,Beg_Pos,End_Pos);*/
}

void Fast Line()
/*Move a line*/
{Set_Region();
 if (Ent_Direction)
 {while (Ent_Count--&&!Check_CC())
  {if (End_Pos==End_Region)
   {End_Error();
    return;
   }
   if (!B_Search(Current,'\n',End_Region))
   {if (!Ent_Count)
    {End_Pos=Current->Pos;
     return;
    }
    End_Error();
    return;
   }
   End_Pos=Current->Pos;
   End_Line++;
  }
 } else
 {if (Beg_Pos==Beg_Region)
  {Beg_Error();
   return;
  }
  if (B_B_Get(Current)=='\n')
  {if (Ent_Count==0)
    return;
   Beg_Line--;
  }
  if (Ent_Count==0)
   Ent_Count++;
  Beg_Pos--;
  while (Ent_Count--&&!Check_CC())
  {if (!B_B_Search(Current,'\n',Beg_Region))
   {if (Ent_Count)
    {Beg_Error();
     return;
    }
    Beg_Pos=Beg_Region;
    break;
   }
   if (!Ent_Count)
    Beg_Pos=Current->Pos+1; else
    Beg_Line--;
  }
 }
}

void Fast Abs_Line()
{if (Ent_Count>This_Line)
 {Ent_Count-=This_Line;
  Ent_Direction=1;
 } else
 {Ent_Count=This_Line-Ent_Count;
  Ent_Direction=0;
  if (My_Col!=1)
   Ent_Count++;
 }
 Line();
}

void Fast Top()
/*Top of buffer*/
{Set_Region();
 Ent_Direction=0;
 if ((Beg_Pos=Beg_Region)==1)
  Beg_Line=1; else
 {Ent_Count=Beg_Region;
  Abs_Char();
 }
}

void Fast Bottom()
/*Bottom of buffer*/
{Set_Region();
 Cnt_F_Lines(End_Pos=End_Region);
 Ent_Direction=1;
}

void Fast Find()
/*Find a given string*/
{if (!Get_Arg()||Skip)
  return;
 Set_Region();
 if (Com_Arg[0]!=0)
  Ins_Del(Dl_Pointers[Search_String]);
 Find_Next();
}

void Fast Find_Next()
/*Find next occurrence of string*/
{Set_Region();
 while (!Err_Flag&&Ent_Count--&&!Check_CC())
  Find_This(0);
}

char Fast Upper_Case(char C)
/*Converts lower case to upper case*/
{
 return (islower(C)||(C>=224&&C<=255)?C-32:C);
}

char Fast Lower_Case(char C)
/*Converts upper case to lower case*/
{
 return (isupper(C)||(C>=192&&C<=223)?C+32:C);
}

void Fast Find_This(int First)
/*Find next occurrence of search string*/
{int Got_Match;
 register int Del_C;
 long Sav_Pos, Nxt_Cr, Got_Pos, *P;
 char *S;
 int I, No_Bline;
 if ((P=Dl_Pointers[Search_String])[1]==0)
  return;
 if (Reg_Search)
 {B_Goto(&Del,P[0]);
  B_Get_N(&Del,Find_Str,(int)P[1]);
  Find_Str[P[1]]=0;
  if (Exp!=NULL)
   free((char*)Exp);
  if ((Exp=regcomp(Find_Str))==NULL)
   return;
  if (Ent_Direction)
  {if (Current->Pos>=End_Region)
   {End_Error();
    return;
   }
   B_Goto(Current,My_Pos+(First==0));
   No_Bline=1;
   while (Current->Pos<End_Region)
   {End_Pos=Current->Pos;
    Find_Str[I=(B_Get_S(Current,Find_Str,512)-1)]=0;
#if !(defined(UNIX) | defined(MPE))
    if (Find_Str[I-1]=='\r')
     Find_Str[I-1]=0;
#endif
    if (regexec(Exp,Find_Str,No_Bline))
    {End_Pos=End_Pos+(Exp->startp[0]-Find_Str);
     if (End_Pos<End_Region)
     {B_Goto(Current,Beg_Pos);
      Cnt_F_Lines(End_Pos);
     } else
      End_Error();
     return;
    }
    No_Bline=0;
   }
   End_Error();
  } else
  {Sav_Pos=Current->Pos;
   while (Current->Pos>Beg_Region)
   {B_Goto(Current,Sav_Pos);
    B_B_Search(Current,'\n',(long)1);
    if (Current->Pos==1)
    {Beg_Error();
     return;
    }
    Sav_Pos=Current->Pos;
    B_Get(Current);
    Find_Str[I=(B_Get_S(Current,Find_Str,512)-1)]=0;
#if !(defined(UNIX) | defined(MPE))
    if (Find_Str[I-1]=='\r')
     Find_Str[I-1]=0;
#endif
    S=Find_Str;
    if (regexec(Exp,S,0)&&Sav_Pos+(Exp->startp[0]-Find_Str+1)<End_Pos)
    {S=Exp->startp[0];
     while(*S!=0&&regexec(Exp,S+1,0)&&Sav_Pos+(Exp->startp[0]-Find_Str+1)
      <End_Pos)
      S=Exp->startp[0];
     Beg_Pos=Sav_Pos+(S-Find_Str+1);
     if (Beg_Pos!=End_Pos)
     {if (Beg_Pos<Beg_Region)
      {Beg_Error();
       return;
      }
      B_Goto(Current,My_Pos);
      Cnt_B_Lines(Beg_Pos);
      return;
     }
    }
   }
   Beg_Error();
  }
 } else
 {B_Goto(&Del,P[0]);
  Del_C=B_Get(&Del);
  if (Ent_Direction)
  {B_Goto(Current,End_Pos);
   if (End_Pos>=End_Region)
   {End_Error();
    return;
   }
   if (First==0)
   {End_Pos++;
    if (B_Get(Current)=='\n')
     End_Line++;
   }
   Nxt_Cr=0;
   while (1)
   {Sav_Pos=Current->Pos;
    Got_Pos=0;
    if (Nxt_Cr<=Current->Pos)
    {B_Search(Current,'\n',End_Region);
     Nxt_Cr=Current->Pos;
    }
    B_Goto(Current,Sav_Pos);
    if (B_Search(Current,(char)Del_C,Nxt_Cr))
     Got_Pos=Current->Pos;
    if (!Case_Sensitive&&(islower(Del_C)||(Del_C>=224&&Del_C<=255)))
    {B_Goto(Current,Sav_Pos);
     if (B_Search(Current,Upper_Case((char)Del_C),Nxt_Cr))
      if (Got_Pos!=0)
      {if (Current->Pos<Got_Pos)
        Got_Pos=Current->Pos;
      } else
       Got_Pos=Current->Pos;
    }
    if (!Case_Sensitive&&(isupper(Del_C)||(Del_C>=192&&Del_C<=223)))
    {B_Goto(Current,Sav_Pos);
     if (B_Search(Current,Lower_Case((char)Del_C),Nxt_Cr))
      if (Got_Pos!=0)
      {if (Current->Pos<Got_Pos)
        Got_Pos=Current->Pos;
      } else
       Got_Pos=Current->Pos;
    }
    if (Got_Pos!=0)
    {End_Pos=Got_Pos;
     Got_Match=1;
     B_Goto(Current,Got_Pos);
     Sav_Pos=Got_Pos-1;
     while (1)
     {if (Got_Match==P[1])
      {End_Pos=Current->Pos;
#if ! (defined(UNIX) | defined(MPE))
       Chk_CRLF_F();
#endif
       Other_Pos=End_Pos;
       End_Pos=Sav_Pos;
#if ! (defined(UNIX) | defined(MPE))
       Chk_CRLF_F();
#endif
       return;
      }
      Del_C=Case_Sensitive?B_Get(&Del):Upper_Case((char)B_Get(&Del));
      if (End_Pos>=End_Region)
      {End_Error();
       return;
      }
      End_Pos++;
      if ((Case_Sensitive?B_Get(Current):Upper_Case((char)B_Get(Current)))==
       Del_C)
       Got_Match++; else
      {B_Goto(Current,Sav_Pos);
       End_Pos=Sav_Pos;
       B_Get(Current);
       End_Pos++;
       B_Goto(&Del,P[0]);
       Del_C=Case_Sensitive?B_Get(&Del):Upper_Case((char)B_Get(&Del));
       break;
      }
     }
     if (Del_C=='\n')
      End_Line++;
    } else
    if (Nxt_Cr<End_Region)
    {B_Goto(Current,Nxt_Cr);
     End_Line++;
    } else
    {End_Error();
     break;
    }
   }
#if ! (defined(UNIX) | defined(MPE))
   Chk_CRLF_F();
#endif
  } else
  {B_Goto(Current,First&&Beg_Pos!=End_Region?Beg_Pos+1:Beg_Pos);
   Sav_Pos=Current->Pos;
   if (My_Pos==Beg_Region)
   {Beg_Error();
    return;
   }
   Sav_Pos=Current->Pos;
   if (My_Col==1)
   {Beg_Line--;
    B_B_Get(Current);
   }
   Nxt_Cr=B_B_Search(Current,'\n',Beg_Region)?Current->Pos+1:Beg_Region;
   B_Goto(Current,Sav_Pos);
   while (1)
   {Sav_Pos=Current->Pos;
    Got_Pos=0;
    if (Nxt_Cr==Current->Pos)
    {B_Goto(Current,Nxt_Cr-1);
     Nxt_Cr=B_B_Search(Current,'\n',Beg_Region)?Current->Pos+1:Beg_Region;
     Beg_Line--;
     B_Goto(Current,Sav_Pos);
    }
    if (B_B_Search(Current,(char)Del_C,Nxt_Cr))
     Got_Pos=Current->Pos;
    if (!Case_Sensitive&&(islower(Del_C)||(Del_C>=224&&Del_C<=255)))
    {B_Goto(Current,Sav_Pos);
     if (B_B_Search(Current,Upper_Case((char)Del_C),Nxt_Cr))
      if (Got_Pos!=0)
      {if (Current->Pos>Got_Pos)
        Got_Pos=Current->Pos;
      } else
       Got_Pos=Current->Pos;
    }
    if (!Case_Sensitive&&(isupper(Del_C)||(Del_C>=192&&Del_C<=223)))
    {B_Goto(Current,Sav_Pos);
     if (B_B_Search(Current,Lower_Case((char)Del_C),Nxt_Cr))
      if (Got_Pos!=0)
      {if (Current->Pos>Got_Pos)
        Got_Pos=Current->Pos;
      } else
       Got_Pos=Current->Pos;
    }
    if (Got_Pos!=0)
    {Got_Match=1;
     B_Goto(Current,Got_Pos);
     Sav_Pos=Got_Pos;
     B_Get(Current);
     Beg_Pos=Got_Pos;
     while (1)
     {if (Got_Match==P[1])
      {Beg_Pos=Current->Pos;
#if ! (defined(UNIX) | defined(MPE))
       Chk_CRLF_B();
#endif
       Other_Pos=Beg_Pos;
       Beg_Pos=Sav_Pos;
#if ! (defined(UNIX) | defined(MPE))
       Chk_CRLF_B();
#endif
       return;
      }
      Del_C=Case_Sensitive?B_Get(&Del):Upper_Case((char)B_Get(&Del));
      if (Beg_Pos>=End_Region)
      {End_Error();
       return;
      }
      Beg_Pos++;
      if ((Case_Sensitive?B_Get(Current):Upper_Case((char)B_Get(Current)))==
       Del_C)
       Got_Match++; else
      {B_Goto(Current,Sav_Pos+1);
       Beg_Pos=Sav_Pos+1;
       B_B_Get(Current);
       Beg_Pos--;
       B_Goto(&Del,P[0]);
       Del_C=Case_Sensitive?B_Get(&Del):Upper_Case((char)B_Get(&Del));
       break;
      }
     }
    } else
    if (Nxt_Cr>Beg_Region)
     B_Goto(Current,Nxt_Cr); else
    {Beg_Error();
     break;
    }
   }
#if ! (defined(UNIX) | defined(MPE))
   Chk_CRLF_B();
#endif
  }
 }
}

void Fast Sel_Range()
/*The select range*/
{register long Point;
 Set_Region();
 if ((Point=Points[0])==0)
 {if (Lst_Cmd==C_Movecursor&&Old_E_Type==E_Find)
   Point=My_Pos+Dl_Pointers[Search_String][1]; else
   {Error(3,No_Sel_Txt);
    return;
   }
 }
 if (E_Type==E_Paste)
  Points[0]=0;
 Cur_Attributes=0;
 B_Goto(Current,My_Pos);
 if (Point>My_Pos)
 {if (Point>End_Region)
   Point=End_Region;
  Cnt_F_Lines(Point);
  End_Pos=Point;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_F();
#endif
  Ent_Direction=1;
 } else
 {if (Point<Beg_Region)
   Point=Beg_Region;
  Cnt_B_Lines(Point);
  Beg_Pos=Point;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_B();
#endif
  Ent_Direction=0;
 }
}

void Fast Word()
{while (Ent_Count--&&!Check_CC())
 {A_Word();
  if (Err_Flag)
   return;
 }
}

void Fast A_Word()
/*A word*/
{register int C;
 Set_Region();
 if (Ent_Direction)
 {if (End_Pos>=End_Region)
  {End_Error();
   return;
  }
  B_Goto(Current,End_Pos);
  End_Pos++;
  if (Chk_EL(Current,C=B_Get(Current)))
  {End_Line++;
#if ! (defined(UNIX) | defined(MPE))
   if (C=='\r')
   {B_Get(Current);
    End_Pos++;
   }
#endif
  }
  if (C==' '||C==9)
  {while (1)
   {if (End_Pos>=End_Region)
     return;
    End_Pos++;
    if ((C=B_Get(Current))!=' '&&C!=9)
     break;
   }
   End_Pos--;
   return;
  }
  if (isupper(C)||islower(C)||isdigit(C)||C=='_'||(C>=192&&C<=255))
  {while (1)
   {if (End_Pos>=End_Region)
     return;
    End_Pos++;
    if (!isupper(C=B_Get(Current))&&!islower(C)&&!isdigit(C)&&C!='_'&&!(C>=192&&C<=255))
     break;
   }
  } else
  {if (End_Pos>=End_Region)
    return;
   C=B_Get(Current);
   End_Pos++;
  }
  if (C==' '||C==9)
  {while (1)
   {if (End_Pos>=End_Region)
     return;
    End_Pos++;
    if ((C=B_Get(Current))!=' '&&C!=9)
     break;
   }
  }
  End_Pos--;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_F();
#endif
 } else
 {if (Beg_Pos==Beg_Region)
  {Beg_Error();
   return;
  }
  B_Goto(Current,Beg_Pos);
  Beg_Pos--;
  if ((C=B_B_Get(Current))==' '||C==9)
  {while (1)
   {if (Beg_Pos==Beg_Region)
     return;
    Beg_Pos--;
    if ((C=B_B_Get(Current))!=' '&&C!=9)
     break;
   }
  }
  if (isupper(C)||islower(C)||isdigit(C)||C=='_'||(C>=192&&C<=255))
  {while (1)
   {if (Beg_Pos==Beg_Region)
     return;
    Beg_Pos--;
    if (!isupper(C=B_B_Get(Current))&&!islower(C)&&!isdigit(C)&&C!='_'&&!(C>=192&&C<=255))
     break;
   }
   Beg_Pos++;
  } else
  {if (C=='\n')
    Beg_Line--;
  }
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_B();
#endif
 }
}

void Fast Get_Mark()
/*Go to a set mark*/
{register long Point;
 Set_Region();
 if (Ent_Count>Maxpoints)
 {Error(3,Illegal_Mark_Txt);
  return;
 }
 if ((Point=Points[Ent_Count])==0)
 {Error(3,No_Mark_Txt);
  return;
 }
 if (Point>My_Pos)
 {if (Point>End_Region)
   Point=End_Region;
  Ent_Direction=1;
  Cnt_F_Lines(Point);
  End_Pos=Point;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_F();
#endif
 } else
 {if (Point<Beg_Region)
   Point=Beg_Region;
  Ent_Direction=0;
  Cnt_B_Lines(Point);
  Beg_Pos=Point;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_B();
#endif
 }
}

void Fast Next_Mark()
/*Go to the next mark*/
{register long Point;
 Set_Region();
 Ent_Count=0;
 {register int I;
  for (I=1;I<=Maxpoints;I++)
  {if ((Point=Points[I])!=0)
   {if (Ent_Direction)
    {if (Point>My_Pos)
     {if (Ent_Count==0)
       Ent_Count=Point; else
      if (Point<Ent_Count)
       Ent_Count=Point;
     }
    } else
    {if (Point<My_Pos)
     {if (Ent_Count==0)
       Ent_Count=Point; else
      if (Point>Ent_Count)
       Ent_Count=Point;
     }
    }
   }
  }
 }
 if ((Point=Ent_Count)==0)
 {Error(3,Last_Mark_Txt);
  return;
 }
 if (Point>My_Pos)
 {if (Point>End_Region)
   Point=End_Region;
  Ent_Direction=1;
  Cnt_F_Lines(Point);
  End_Pos=Point;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_F();
#endif
 } else
 {if (Point<Beg_Region)
   Point=Beg_Region;
  Ent_Direction=0;
  Cnt_B_Lines(Point);
  Beg_Pos=Point;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_B();
#endif
 }
}

void Fast Line_End()
{register char C;
 Set_Region();
 if (Ent_Direction)
 {if (My_Pos>=End_Region)
  {End_Error();
   return;
  }
#if ! (defined(UNIX) | defined(MPE))
  if ((C=B_Get(Current))=='\r')
  {if (Current->Pos>=End_Region)
   {End_Error();
    return;
   }
   C=B_Get(Current);
  }
  if (C=='\n')
#else
  if ((C=B_Get(Current))=='\n')
#endif
  {if (Ent_Count==0)
    return;
   End_Line++;
  }
  if (Ent_Count==0)
   Ent_Count++;
  while (Ent_Count--&&!Check_CC())
  {if (!B_Search(Current,'\n',End_Region))
   {if (!Ent_Count)
     End_Pos=Current->Pos; else
     End_Error();
    return;
   }
   End_Line++;
  }
  End_Line--;
  End_Pos=Current->Pos-1;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_FB();
#endif
 } else
 {if (My_Pos==Beg_Region)
  {Beg_Error();
   return;
  }
  while (Ent_Count--&&!Check_CC())
  {if (!B_B_Search(Current,'\n',Beg_Region))
   {Beg_Error();
    return;
   }
   Beg_Line--;
  }
  Beg_Pos=Current->Pos;
#if ! (defined(UNIX) | defined(MPE))
  Chk_CRLF_B();
#endif
 }
}

char Fast Skip_Spaces(char C)
{if (C==' '||C==9||C==12||C=='\r')
 while (1)
 {if (End_Pos>=End_Region)
   return ' ';
  End_Pos++;
  if ((C=B_Get(Current))!=' '&&C!=9&&C!=12&&C!='\r')
   break;
 }
 return C;
}

char Fast B_Skip_Spaces(char C)
{if (C==' '||C==9||C==12||C=='\r')
 while (1)
 {if (Current->Pos==Beg_Region)
   return ' ';
  Beg_Pos--;
  if ((C=B_B_Get(Current))!=' '&&C!=9&&C!=12&&C!='\r')
   break;
 }
 return(C);
}

void Fast Par()
{Set_Region();
 while (Ent_Count--&&!Check_CC())
 {A_Par();
  if (Err_Flag)
   return;
 }
}

void Fast A_Par()
/*A Par*/
{register char C;
 register long Sav_Pos;
 if (Ent_Direction)
 {if (End_Pos>=End_Region)
  {End_Error();
   return;
  }
  B_Goto(Current,End_Pos);
  while (1)
  {if (End_Pos>=End_Region)
    return;
   End_Pos++;
   if ((C=B_Get(Current))=='\n')
   {End_Line++;
    if (End_Pos>=End_Region)
     return;
    End_Pos++;
    C=Skip_Spaces((C=B_Get(Current)));
    if (End_Pos>=End_Region)
    {if (C=='\n')
      End_Line++;
     return;
    }
    if (C=='\n')
    {while (C=='\n')
     {End_Line++;
      if (End_Pos>=End_Region)
       return;
      End_Pos++;
      C=Skip_Spaces((C=B_Get(Current)));
      if (End_Pos>=End_Region)
      {if (C=='\n')
        End_Line++;
       return;
      }
     }
     End_Pos--;
     return;
    }
   }
  }
 } else
 {if (Beg_Pos==Beg_Region)
  {Beg_Error();
   return;
  }
  B_Goto(Current,Beg_Pos);
  Beg_Pos--;
  C=B_Skip_Spaces(C=B_B_Get(Current));
  if (Current->Pos==Beg_Region)
  {Beg_Error();
   return;
  }
  if (C=='\n')
  {while (C=='\n')
   {Beg_Line--;
    if (Current->Pos==Beg_Region)
    {Beg_Error();
     return;
    }
    C=B_Skip_Spaces((C=B_B_Get(Current)));
    if (Current->Pos==Beg_Region)
    {Beg_Error();
     return;
    }
   }
  }
  Sav_Pos=Current->Pos;
  while (1)
  {if (Current->Pos==Beg_Region)
   {Beg_Pos=Sav_Pos;
    return;
   }
   if ((C=B_B_Get(Current))=='\n')
   {if (Current->Pos==Beg_Region)
    {Beg_Pos=Sav_Pos;
     return;
    }
    C=B_Skip_Spaces((C=B_B_Get(Current)));
    if (Current->Pos==Beg_Region)
    {Beg_Pos=Sav_Pos;
     return;
    }
    if (C=='\n')
    {Beg_Pos=Sav_Pos;
     return;
    }
    Beg_Line--;
   } else
   if (C!=' '&&C!=9&&C!=12&&C!='\r')
    Sav_Pos=Current->Pos;
  }
 }
}

void Fast Sentence()
{Set_Region();
 while (Ent_Count--&&!Check_CC())
 {A_Sentence();
  if (Err_Flag)
   return;
 }
}

void Fast A_Sentence()
/*A sentence*/
{register char C;
 register long Sav_Pos;
 if (Ent_Direction)
 {if (End_Pos>=End_Region)
  {End_Error();
   return;
  }
  B_Goto(Current,End_Pos);
  while (1)
  {if (End_Pos>=End_Region)
    return;
   End_Pos++;
   if ((C=B_Get(Current))=='\n'||C=='.'||C==';'||C=='?'||C=='!'||C==':')
   {if (C=='\n')
     End_Line++;
    if (End_Pos>=End_Region)
     return;
    End_Pos++;
    C=Skip_Spaces((C=B_Get(Current)));
    if (End_Pos>=End_Region)
    {if (C=='\n')
      End_Line++;
     return;
    }
    if (C=='\n')
    {while (C=='\n')
     {End_Line++;
      if (End_Pos>=End_Region)
       return;
      End_Pos++;
      C=Skip_Spaces((C=B_Get(Current)));
      if (End_Pos>=End_Region)
      {if (C=='\n')
        End_Line++;
       return;
      }
     }
     End_Pos--;
     return;
    }
    End_Pos--;
    return;
   }
  }
 } else
 {if (Beg_Pos==Beg_Region)
  {Beg_Error();
   return;
  }
  B_Goto(Current,Beg_Pos);
  Beg_Pos--;
  C=B_Skip_Spaces((C=B_B_Get(Current)));
  if (Beg_Pos==Beg_Region)
  {Beg_Error();
   return;
  }
  if (C=='\n')
  {while (C=='\n')
   {Beg_Line--;
    if (Beg_Pos==Beg_Region)
    {Beg_Error();
     return;
    }
    Beg_Pos--;
    C=B_Skip_Spaces((C=B_B_Get(Current)));
    if (Beg_Pos==Beg_Region)
    {Beg_Error();
     return;
    }
   }
  }
  Sav_Pos=Beg_Pos;
  while (1)
  {if (Beg_Pos==Beg_Region)
   {Beg_Pos=Sav_Pos;
    return;
   }
   Beg_Pos--;
   if ((C=B_B_Get(Current))=='\n'||C=='.'||C==';'||C=='?'||C=='!'||C==':')
   {Beg_Pos=Sav_Pos;
    return;
   } else
   if (C!=' '&&C!=9&&C!=12&&C!='\r')
    Sav_Pos=Beg_Pos;
  }
 }
}

void Fast Page()
{Set_Region();
 if (Ent_Direction)
 {if (End_Pos>=End_Region)
  {End_Error();
   return;
  }
  while (Ent_Count--&&!Check_CC())
  {if (!B_Search(Current,'\f',End_Region)&&Ent_Count)
   {End_Error();
    return;
   };
  }
  End_Pos=Current->Pos;
  B_Goto(Current,Beg_Pos);
  Cnt_F_Lines(End_Pos);
 } else
 {if (Beg_Pos==Beg_Region)
  {Beg_Error();
   return;
  }
  B_Goto(Current,--Beg_Pos);
  while (Ent_Count--&&!Check_CC())
  {if (!B_B_Search(Current,'\f',Beg_Region))
   {if (Ent_Count)
    {Beg_Error();
     return;
    } else
     Beg_Pos=Beg_Region;
   } else
    Beg_Pos=Current->Pos+1;
  }
  B_Goto(Current,End_Pos);
  Cnt_B_Lines(Beg_Pos);
 }
}

void Fast Screen()
{Ent_Count*=(Rel_Last-Rel_First);
 Line();
 if (Err_Flag)
 {Err_Flag=0;
  if (Ent_Direction)
   Bottom(); else
   Top();
 }
}

void Fast Match_Bracket()
{int Level=0, Index;
 unsigned char C;
 if (Current->Pos>=End_Region)
 {Error(3,No_Match_Txt);
  return;
 }
 C=B_Get(Current);
 B_B_Get(Current);
#if (defined(DOS) & !defined(Windows)) | defined(OS2) | defined(WIN32)
 for (Index=0;Index<12;Index++)
#else
 for (Index=0;Index<10;Index++)
#endif
  if (Bracket[0][Index]==C)
   break;
#if (defined(DOS) & !defined(Windows)) | defined(OS2) | defined(WIN32)
 if (Index==12)
#else
 if (Index==10)
#endif
 {Error(3,No_Match_Txt);
  return;
 }
#if (defined(DOS) & !defined(Windows)) | defined(OS2) | defined(WIN32)
 if (C=='('||C=='['||C=='{'||C=='<'||C==174)
  Ent_Direction=1; else
 if (C==')'||C==']'||C=='}'||C=='>'||C==175)
  Ent_Direction=0;
#else
 if (C=='('||C=='['||C=='{'||C=='<')
  Ent_Direction=1; else
 if (C==')'||C==']'||C=='}'||C=='>')
  Ent_Direction=0;
#endif
 if (Ent_Direction)
 {B_Get(Current);
  while (1)
  {if (++End_Pos>=End_Region)
   {Error(3,Missing_Match_Txt);
    return;
   }
   if ((C=B_Get(Current))==Bracket[1][Index])
   {if (Level--==0)
     break;
   } else
   if (C==Bracket[0][Index])
    Level++;
  }
  B_Goto(Current,Beg_Pos);
  Cnt_F_Lines(End_Pos);
 } else
 {while (1)
  {if (--Beg_Pos<Beg_Region)
   {Error(3,Missing_Match_Txt);
    return;
   }
   if ((C=B_B_Get(Current))==Bracket[1][Index])
   {if (Level--==0)
     break;
   } else
   if (C==Bracket[0][Index])
    Level++;
  }
  B_Goto(Current,End_Pos);
  Cnt_B_Lines(Beg_Pos);
 }
}

void Fast Search_End()
{B_Goto(Current,My_Pos);
 End_Pos=Other_Pos;
 Cnt_F_Lines(End_Pos);
}

#if defined(ANSIC)
struct Ent_Rec {char *Name; int Type, Skipok;void (Fast *Function)(void);}
#else
struct Ent_Rec {char *Name; int Type, Skipok;void (Fast *Function)();}
#endif

 Ent_Tab[]=
  {
   {"AC",E_Unknown,1,Abs_Char},
   {"AL",E_Unknown,1,Abs_Line},
   {"BB",E_Unknown,1,Top},
   {"BM",E_Unknown,1,Match_Bracket},
   {"EB",E_Unknown,1,Bottom},
   {"EL",E_Line,1,Line_End},
   {"ES",E_Unknown,1,Search_End},
   {"C",E_Char,1,Char},
   {"F",E_Find,0,Find},
   {"L",E_Line,1,Line},
   {"M",E_Unknown,1,Get_Mark},
   {"N",E_Find,1,Find_Next},
   {"PAG",E_Pag,1,Page},
   {"PAR",E_Par,1,Par},
   {"SCR",E_Line,1,Screen},
   {"SEN",E_Sen,1,Sentence},
   {"SR",E_Paste,1,Sel_Range},
   {"V",E_Unknown,1,Vertical},
   {"W",E_Word,1,Word},
   {"XNM",E_Unknown,1,Next_Mark},
   {"XSR",E_NPaste,1,Sel_Range}
  };

#define Entities (sizeof(Ent_Tab)/sizeof(struct Ent_Rec))

void Fast Ex_Entity()
/*Gets and executes a entity*/
{register int L=0, Done=0, Entity;
 char S[10];
 struct Ent_Rec *P;
 Beg_Line=End_Line=This_Line;
 B_Goto(Current,Beg_Pos=End_Pos=My_Pos);
 while (!Done)
 {if (Com.Pos>Com.Bufsize||L==8)
  {Ent_Error();
   return;
  }
  if (islower(Ent_Char=B_Get(&Com))||(Ent_Char>=224&&Ent_Char<=255))
   Ent_Char=_toupper(Ent_Char);
  if (!isupper(Ent_Char)&&(Ent_Char<192&&Ent_Char>255))
  {Ent_Error();
   return;
  }
  S[L++]=Ent_Char;
  S[L]=0;
  for (Entity=0;Entity<Entities;Entity++)
   {if (S[0]==(*(P=(&Ent_Tab[Entity]))).Name[0]&&!strcmp(S,(*(P)).Name))
    {Old_E_Type=E_Type;
     E_Type=(*P).Type;
     if (!(Skip&&(*P).Skipok))
     {
      (*((*P).Function))();
     }
     Done=1;
     break;
   }
  }
 }
}

void Fast Cnt_F_Lines(long Pos)
{while (B_Search(Current,'\n',Pos))
  End_Line++;
}

void Fast Cnt_B_Lines(long Pos)
{while (B_B_Search(Current,'\n',Pos))
  Beg_Line--;
}

void Fast Set_Region()
{Beg_Region=Points[Maxpoints+3];
 End_Region=Points[Maxpoints+4];
}

void Fast End_Error()
{if (End_Region!=Current->Bufsize+1)
  Error(3,Edit_Region_End_Txt); else
  Error(3,Buffer_End_Txt);
}

void Fast Beg_Error()
{if (Beg_Region!=1)
  Error(3,Edit_Region_Begin_Txt); else
  Error(3,Buffer_Beginning_Txt);
}

void Fast Delim_Error()
{Error(3,No_Delimiter_Txt);
}

void Fast Ent_Error()
{Error(2,Bad_Entity_Txt);
}
