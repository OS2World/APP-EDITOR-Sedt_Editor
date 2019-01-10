/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"

void Fast Wait_Char()
{
#if (defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV)))
 Mouse_OK=0;
#endif
 Opt_Cursor(Scr_Length,(long)1);
 while (1)
 {Get16();
  if (Character==13||Character==8||isalnum(Character)||Character==' '||
   (Key_Number>=25&&Key_Number<=50)||Key_Number==12)	/* 290a */
   break;
  Out_C('\7');
 }
#if (defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV)))
 Mouse_OK=1;
#endif
}

void Fast Show_Help()
{while (1)
 {Display_Help(Help_Pos=Del.Pos-Dl_Pointers[Help_String][0]);
  Wait_Char();
  if (At_End||(Character!=13&&Key_Number!=48&&Key_Number!=50
              &&Key_Number!=26&&Key_Number!=29))
   break;
 }
}

void Fast Display_Help(long P)
{char C;
 Opt_Cursor(2,(long)1);
 B_Goto(&Del,Dl_Pointers[Help_String][0]+P);
 while (1)
 {if (Del.Pos>=Dl_Pointers[Help_String][0]+Dl_Pointers[Help_String][1])
   break;
  if ((C=B_Get(&Del))!='\n'&&C!='\r'&&C!=' ')
  {if (C==12)
   {B_Search(&Del,'\n',
     Dl_Pointers[Help_String][0]+Dl_Pointers[Help_String][1]);
    At_End=0;
    Wait_Mess(Txt_More);
    return;
   } else
   {B_B_Get(&Del);
    break;
   }
  } else
  {if (My_Line==Scr_Length-1)
   {At_End=0;
    B_B_Get(&Del);
    Wait_Mess(Txt_More);
    return;
   }
   while (C!='\n'&&
    Del.Pos<Dl_Pointers[Help_String][0]+Dl_Pointers[Help_String][1])
   {
#if ! (defined(UNIX) | defined(MPE))
    if (C!='\r')
#endif
     Opt_Char(C);
    C=B_Get(&Del);
   }
   Opt_Erase_Line();
   Opt_Cursor(My_Line+1,(long)1);
  }
 }
 At_End=1;
 Wait_Mess(Txt_End);
}

void Fast Help()
{if (!Get_Arg()||Skip)
  return;
 if (strlen(Com_Arg)==0)
#if defined(UNIX)
  strcpy(Com_Arg,Do_Mapping("help.hlp"));
#else
#if defined(MPE)
  strcpy(Com_Arg,"help.sed");
#else
  strcpy(Com_Arg,Do_Mapping("HELP.HLP"));
#endif
#endif
 Do_Help();
}

/*      Display the appropriate help screens for the keyboard being used.
*/
void Fast Do_Help()
{char Sav_Attributes;
 int I, J, K, L, OK, Key, H_Col, Gold_Txt, Key_Txt;
 struct File F, G;
 int Sav_Line, C;
 long Sav_Col, Sav_Shift;
 Update_On();
 if (Dl_Pointers[Help_String][1]==0||strcmp(Help_File,Com_Arg)!=0)
 {strcpy(Help_File,Com_Arg);
  if (Dl_Pointers[Help_String][1]!=0)
   Remove_Help();
#if defined(MPE)
  if (F_open(Help_File,"r",&F)==NULL)
#else
#if defined(VMS)
  if (F_open(Logical_File(Com_Arg),"r",&F)==NULL&&	/* 290a */
   F_open(Help_File,"r",&F)==NULL&&
   F_open(Find_Dir(Help_File,Dsk_Rec),"r",&F)==NULL&&
   F_open(Find_Lib(Help_File,Dsk_Rec),"r",&F)==NULL)
#else
  if (F_open(Help_File,"r",&F)==NULL&&
   (Find_Env(Help_File,Dsk_Rec,"SEDTP")==NULL||
    F_open((char *) Dsk_Rec,"r",&F)==NULL)&&
   (Find_Env(Help_File,Dsk_Rec,"SEDT")==NULL||
    F_open((char *) Dsk_Rec,"r",&F)==NULL)&&
#if defined(UNIX)
   (Find_Env(Help_File,Dsk_Rec,"HOME")==NULL||
    F_open((char *) Dsk_Rec,"r",&F)==NULL)&&
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
   (Find_From(Help_File,Dsk_Rec)==NULL||
    F_open((char *) Dsk_Rec,"r",&F)==NULL)&&
#endif
   (Find_Path(Help_File,Dsk_Rec)==NULL||
    F_open((char *) Dsk_Rec,"r",&F)==NULL))
#endif
#endif
  {Error(2,No_Help_File_Txt,Dsk_Rec);
   return;
  }
  Plain_Message(Reading_Help_Txt);
  Upd_Header();
  B_Goto(&Del,Del.Bufsize+1);
  Dl_Pointers[Help_String][0]=Del.Pos;
  Gold_Txt=0;
  while ((I=F_read(Dsk_Rec,1,Blksize,&F))!=0)
  {H_Col=1;
   J=0;
   while(J<I)
   {if ((C=Dsk_Rec[J])=='\n'||C=='\r')
     H_Col=1;
    if (C=='%')
    {if ((C=Dsk_Rec[J+1])=='F'||C=='f')
     {if ((C=Dsk_Rec[J+2])>='0'&&C<='9')
      {Key=0;
       J+=2;
       while (J<I&&(C=Dsk_Rec[J])>='0'&&C<='9')
       {Key=Key*10+C-'0';
        J++;
       }
       J--;
       for (K=0;K<Map_Defs;K++)
       {if (Map_Tab[K].F_Code==Key)
        {if (Map_Tab[K].Flags&Gold_Code)
         {if (Gold_Txt==0)
          {for (L=0;L<Key_Defs;L++)
            if (Key_Tab[L].Key==44)
            {Gold_Txt=Key_Tab[L].Index;
             break;
            }
          }
          if (Gold_Txt==0)
          {for (L=0;L<Comb_Defs;L++)
            if (Comb_Tab[L].Key==44&&Comb_Tab[L].Flags&Help_Code)
            {Gold_Txt=Comb_Tab[L].Index;
             break;
            }
          }
          if (Gold_Txt==0||Gold_Txt==-1)
          {Gold_Txt= -1;
           B_Put_S(&Del,"Gold ",5);
          } else
	  {B_Goto(&Pad,(long) Gold_Txt);
	   while ((C=B_Get(&Pad))!=0)
           {H_Col++;
	    B_Put(&Del,(char)C);
	   }
           B_Put(&Del,' ');
           H_Col++;
          }
         }
         if (Map_Tab[K].Flags&Key_Code)
         {Key_Txt=0;
          for (L=0;L<Key_Defs;L++)
          {if ((Key_Tab[L].Key)==(Map_Tab[K].Char))
           {Key_Txt=Key_Tab[L].Index;
            break;
           }
          }
          if (Key_Txt==0)
          {for (L=0;L<Comb_Defs;L++)
           {if ((Comb_Tab[L].Key)==(Map_Tab[K].Char)&&
             Comb_Tab[L].Flags&Help_Code)
            {Key_Txt=Comb_Tab[L].Index;
             break;
            }
           }
          }
          if(Key_Txt!=0)
	  {B_Goto(&Pad,(long) Key_Txt);
	   while ((C=B_Get(&Pad))!=0)
	   {H_Col++;
	    B_Put(&Del,(char)C);
	   }
          } else
          {B_Put_S(&Del,"Undefined key",13);
           H_Col+=13;
          }
         } else
	 {B_Goto(&Pad,(long) Map_Tab[K].Index);
	  while ((C=B_Get(&Pad))!=0)
          {H_Col++;
	   B_Put(&Del,(char)C);
	  }
         }
         break;
        } else
        if (K==Map_Defs-1)
        {B_Put_S(&Del,Unassigned_Txt,strlen(Unassigned_Txt));
         H_Col+=strlen(Unassigned_Txt);
         if (Key>=1000)
         {H_Col++;
          B_Put(&Del,(char)((Key/1000)%10+'0'));
         }
         if (Key>=100)
         {H_Col++;
          B_Put(&Del,(char)((Key/100)%10+'0'));
         }
         if (Key>=10)
         {H_Col++;
          B_Put(&Del,(char)((Key/10)%10+'0'));
         }
         B_Put(&Del,(char)(Key%10+'0'));
         H_Col++;
        }
       }
      } else
      {H_Col++;
       B_Put(&Del,'%');
      }
     } else
     if ((C=Dsk_Rec[J+1])=='A'||C=='a')
     {if ((C=Dsk_Rec[J+2])>='0'&&C<='9')
      {Key=0;
       J+=2;
       while (J<I&&(C=Dsk_Rec[J])>='0'&&C<='9')
       {Key=Key*10+C-'0';
        J++;
       }
       J--;
       OK=0;
       for (K=0;K<Map_Defs;K++)
       {if (Map_Tab[K].F_Code==Key)
        {if (OK)
         {B_Put_S(&Del,", ",2);
          H_Col+=2;
         }
         if (Map_Tab[K].Flags&Gold_Code)
         {if (Gold_Txt==0)
          {for (L=0;L<Key_Defs;L++)
            if (Key_Tab[L].Key==44)
            {Gold_Txt=Key_Tab[L].Index;
             break;
            }
          }
          if (Gold_Txt==0)
          {for (L=0;L<Comb_Defs;L++)
            if (Comb_Tab[L].Key==44&&Comb_Tab[L].Flags&Help_Code)
            {Gold_Txt=Comb_Tab[L].Index;
             break;
            }
          }
          if (Gold_Txt==0||Gold_Txt==-1)
          {Gold_Txt= -1;
           B_Put_S(&Del,Txt_Gold,strlen(Txt_Gold));
           H_Col+=strlen(Txt_Gold);
          } else
	  {B_Goto(&Pad,(long) Gold_Txt);
	   while ((C=B_Get(&Pad))!=0)
           {H_Col++;
	    B_Put(&Del,(char)C);
	   }
           B_Put(&Del,' ');
           H_Col++;
          }
         }
         if (Map_Tab[K].Flags&Key_Code)
         {Key_Txt=0;
          for (L=0;L<Key_Defs;L++)
          {if ((Key_Tab[L].Key)==(Map_Tab[K].Char))
           {Key_Txt=Key_Tab[L].Index;
            break;
           }
          }
          if (Key_Txt==0)
          {for (L=0;L<Comb_Defs;L++)
           {if ((Comb_Tab[L].Key)==(Map_Tab[K].Char)&&
             Comb_Tab[L].Flags&Help_Code)
            {Key_Txt=Comb_Tab[L].Index;
             break;
            }
           }
          }
          if(Key_Txt!=0)
	  {B_Goto(&Pad,(long) Key_Txt);
	   while ((C=B_Get(&Pad))!=0)
	   {H_Col++;
	    B_Put(&Del,(char)C);
	   }
          } else
          {B_Put_S(&Del,Unassigned_Txt,strlen(Unassigned_Txt));
           H_Col+=strlen(Unassigned_Txt);
          }
         } else
	 {B_Goto(&Pad,(long) Map_Tab[K].Index);
	  while ((C=B_Get(&Pad))!=0)
          {H_Col++;
	   B_Put(&Del,(char)C);
	  }
         }
         OK=1;
        } else
        if (K==Map_Defs-1&&!OK)
        {B_Put_S(&Del,Unassigned_Txt,strlen(Unassigned_Txt));
         H_Col+=strlen(Unassigned_Txt);
         if (Key>=1000)
         {H_Col++;
          B_Put(&Del,(char)((Key/1000)%10+'0'));
         }
         if (Key>=100)
         {H_Col++;
          B_Put(&Del,(char)((Key/100)%10+'0'));
         }
         if (Key>=10)
         {H_Col++;
          B_Put(&Del,(char)((Key/10)%10+'0'));
         }
         B_Put(&Del,(char)(Key%10+'0'));
         H_Col++;
        }
       }
      } else
      {H_Col++;
       B_Put(&Del,'%');
      }
     } else
     if (C=='C'||C=='c')
     {if ((C=Dsk_Rec[J+2])>='0'&&C<='9')
      {Key=0;
       J+=2;
       while (J<I&&(C=Dsk_Rec[J])>='0'&&C<='9')
       {Key=Key*10+C-'0';
        J++;
       }
       J--;
       if (Key<H_Col)
       {H_Col=2;
#if !(defined(UNIX) | defined(MPE))
        B_Put(&Del,'\r');
#endif
        B_Put(&Del,'\n');
       }
       while (Key>H_Col)
       {H_Col++;
        B_Put(&Del,' ');
       }
      } else
      {H_Col++;
       B_Put(&Del,'%');
      }
     } else
     if (C=='T'||C=='t')
     {if ((C=Dsk_Rec[J+2])>='0'&&C<='9')
      {Key=0;
       J+=2;
       while (J<I&&(C=Dsk_Rec[J])>='0'&&C<='9')
       {Key=Key*10+C-'0';
        J++;
       }
       J--;
       if (Funcs[Key][2]!=0)
       {B_Goto(&Pad,Funcs[Key][2]);
	while ((C=B_Get(&Pad))!=0)
        {H_Col++;
	 B_Put(&Del,(char)C);
	}
       }
      } else
      {H_Col++;
       B_Put(&Del,'%');
      }
     } else
     if (C=='K'||C=='k')
#if defined(MPE)
     {if (F_open((char *) Helpfile(),"r",&G)==NULL)
#else
#if defined(VMS)
     {if (F_open(Logical_File(Helpfile()),"r",&G)==NULL&&	/* 290a */
       F_open((char *) Helpfile(),"r",&G)==NULL&&
       F_open(Find_Dir(Helpfile(),Dsk_Rec),"r",&G)==NULL&&
       F_open(Find_Lib(Helpfile(),Dsk_Rec),"r",&G)==NULL)
#else
     {if (F_open((char *) Helpfile(),"r",&G)==NULL&&
       (Find_Env(Helpfile(),Dsk_Rec,"SEDTP")==NULL||
        F_open((char *) Dsk_Rec,"r",&G)==NULL)&&
       (Find_Env(Helpfile(),Dsk_Rec,"SEDT")==NULL||
        F_open((char *) Dsk_Rec,"r",&G)==NULL)&&
#if defined(UNIX)
       (Find_Env(Helpfile(),Dsk_Rec,"HOME")==NULL||
        F_open((char *) Dsk_Rec,"r",&G)==NULL)&&
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
       (Find_From(Helpfile(),Dsk_Rec)==NULL||
        F_open((char *) Dsk_Rec,"r",&G)==NULL)&&
#endif
       (Find_Path(Helpfile(),Dsk_Rec)==NULL||
        F_open((char *) Dsk_Rec,"r",&G)==NULL))
#endif
#endif
       Error(2,No_Help_File_Txt,Helpfile()); else
      {while ((C=F_getc(&G))!=EOF)
        B_Put(&Del,(char)C);
       F_close(&G);
      }
     } else
     {H_Col++;
      B_Put(&Del,'%');
      if (C=='%')
       J++;
     }
    } else
    {H_Col++;
     B_Put(&Del,(char)C);
    }
    J++;
   }
  }
  F_close(&F);
  Dl_Pointers[Help_String][1]=Del.Pos-Dl_Pointers[Help_String][0];
 }
 In_Help=1;
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#endif
#if !(((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM)))
 Set_Physical();
 Clr_Screen();
#endif
 Set_Scroll(1,Scr_Length-1);
 Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,1,(long)1,0);
 Cur_Attributes=A_Reverse;
 Skip_To(Scr_Width+1);
 Opt_Cursor(Scr_Length-1,(long)1);
 Skip_To(Scr_Width+1);
 Cur_Attributes=0;
 Opt_Cursor(2,(long)1);
 Sav_Shift=Scr_Shift;
 Scr_Shift=0;
 Help_Level=0;
 B_Goto(&Del,Dl_Pointers[Help_String][0]);
 while (1)
 {Show_Help();
  if (Character==13||Character==8||Key_Number==48||Key_Number==50
      ||Key_Number==12) 	/* 290a */
  {Help_Level--;
   if (Help_Level<0)
    break;
  } else  /* 290a */
  if (Key_Number>=31&&Key_Number<=40)
  {Help_Level++;
   Help_Levels[Help_Level-1]=Key_Number-31+'0';
  } else
  if (isalnum(Character))
  {Help_Level++;
   Help_Levels[Help_Level-1]=Character;
  } else
  if (Character==27)
   break; else
  if (Character==' '&&!Function)
   break;
  B_Goto(&Del,Dl_Pointers[Help_String][0]);
  if (Help_Level!=0)
  {while (1)
   {if (Del.Pos>=Dl_Pointers[Help_String][0]+Dl_Pointers[Help_String][1])
    {B_Goto(&Del,Dl_Pointers[Help_String][0]);
     if ((--Help_Level)==0)
      break;
    } else
    {OK=1;
     for (I=0;OK&&I<Help_Level;I++)
     {if ((C=B_Get(&Del))=='\n')
      {OK=0;
       break;
      }
      OK&=Help_Levels[I]==C;
     }
     if (C!='\n')
      B_Search(&Del,'\n',
       Dl_Pointers[Help_String][0]+Dl_Pointers[Help_String][1]);
     if (OK)
      break;
    }
   }
  }
 }
 In_Help=0;
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Enable_Mouse();
#endif
#if !(((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM)))
 Set_Logical();
 Clr_Screen();
 if (!Zoomed)
  Refresh_Window(); else
#endif
 {S_Header();
  if (Show_Ruler)
   S_Ruler();
  Fill_Screen(Rel_First,Rel_Last);
 }
 Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
 Scr_Shift=Sav_Shift;
 Plain_Message("");
}

/*      Utility procedure for Help. Erase to the bottom of the screen and
        prompt for a command character. Return the character entered.
*/
void Fast Wait_Mess(char *S)
{while (My_Line!=Scr_Length-1)
 {Opt_Erase_Line();
  Opt_Cursor(My_Line+1,(long)1);
 }
 Opt_Cursor(Scr_Length,(long)1);
 Cur_Attributes=A_Bold;
 Opt_String(S);
 Opt_String(Help_Menu_Txt);
 Opt_Erase_Line();
 Cur_Attributes=0;
}
