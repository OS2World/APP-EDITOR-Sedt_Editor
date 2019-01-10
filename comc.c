/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"
#if defined(WIN32)
#include "win32edt.h"
#endif

extern regexp *Exp;

/*      Save the current virtual cursor position and attributes and set them
	to the new values given.
*/
void Fast Sav_Set(int *S_Line,long *S_Col,char *S_Attributes,int N_Line,
 long N_Col,char N_Attributes)
{(*S_Line)=My_Line;
 (*S_Col)=My_Col;
 Opt_Cursor(N_Line,N_Col);
 if (S_Attributes!=NULL)
 {*S_Attributes=Cur_Attributes;
  Cur_Attributes=N_Attributes;
 }
#if defined(VMS) | defined(UNIX)
 if (Shifted_Out)
 {Out_Zap();
  putchar(15);
 }
#endif
}

/*      Restore the virtual cursor position and attributes to the saved values.
*/
void Fast Set_Save(int S_Line,long S_Col,char S_Attributes)
{Opt_Cursor(S_Line,S_Col);
 Cur_Attributes=S_Attributes;
#if defined(VMS) | defined(UNIX)
 if (Shifted_Out)
 {Out_Zap();
  putchar(14);
 }
#endif
}

void Fast Clr_Typeahead()
{
#if defined(DOS) & !defined(Windows)
 while ((bdos(0x0B,0,0)&0x00FF)!=0)
  bdos(0x07,0,0);
#endif
#if defined(OS2)
 if (!Mouse_OK)
  KbdFlushBuffer(0);
#endif
#if defined(WIN32)
   WIN32clrtypeahead();
#endif
}

/*      Print the string given on the bottom line of the display and input
        a Y or a N. Return true if the reponse was a Y.
*/
int Fast Yes_No(char *S)
{
#if defined(ATARI) & defined(GEM)
 return GEM_Yes_No(S);
#elif (defined(DOS) | defined(NT)) & defined(Windows)
 return Win_Yes_No(S);
#elif defined(DOS) & defined(TV)
 return TV_Yes_No(S);
#else
 int Sav_Line, Sav_Update;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;;
#if defined(VMS) | defined(UNIX) 
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#endif
 /*Clr_Typeahead();*/
 Mark_Cursor();
 Set_Physical();
 Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,A_Bold);
 Sav_Shift=Scr_Shift;
 Scr_Shift=0;
 Sav_Update=Update_Flag;
 Update_Flag=1;
 strcpy(Dsk_Rec,S);
 strcat(Dsk_Rec,Yes_No_Txt);
 Opt_String(Dsk_Rec);
 Opt_Erase_Line();
 Err_Display=0;
 Get16();
 while (Character!=Chr_Y&&Character!=tolower(Chr_Y)&&
  Character!=Chr_N&&Character!=tolower(Chr_N)&&Character!=21)
 {Clr_Typeahead();
#if defined(DOS) & !defined(Windows)
  if (!In_Int24)
#endif
   Out_C('\7');
  Get16();
 }
 Opt_Char((char)Character);
 Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
 Set_Logical();
 Scr_Shift=Sav_Shift;
 Update_Flag=Sav_Update;
 Unmark_Cursor();
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Enable_Mouse();
#endif
 Out_Zap();
 Err_Display=1;
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (Working_Flag)
  Win_Clear_Working();
#endif
 Working_Flag=0;
 if (Character==21)
 {Error(3,Txt_Cancelled);
  return 0;
 }
 return(Character==Chr_Y||Character==tolower(Chr_Y));
#endif
}

/*      Save the output file of the current buffer. Set Not_End to false if
        no other buffers contain unsaved changes. If there are unsaved changes
        ask the operator whether he wished to exit sedt and set Not_End to
        false if he responds with a Y.
*/
void Fast Exit_Sedt()
/*Exit from Sedt*/
{int Buffer;
 char *P;
 if ((P=Outfile)==NULL||P[0]==0)
  Error(2,No_File_Txt); else
 {if (!Read_Only[Cur_Buffer]||Yes_No(Txt_Read_Only))
   Wr_Buffer(P);
  if (Err_Flag)
   return;
  Clr_Changes();
#if (defined(DOS) | defined(NT)) & defined(Windows)
  Win_Quit();
  if ((Any_Changes[Cur_Buffer]&1)&&!Read_Only[Cur_Buffer])
   return;
  if (Err_Flag)
   return;
#else
#if defined(ATARI) & defined(GEM)
  GEM_Quit();
#else
  Not_End=0;
  Buffer=Cur_Buffer;
  {register int I;
   Sav_Window(Cur_Buffer);
   for (I=0;I<Maxbuffers;I++)
   {if ((++Buffer)==Maxbuffers)
     Buffer=0;
    if (Dl_Pointers[Pointers+Buffer][0]!=0)
    {if ((Any_Changes[Buffer]&1)&&!Read_Only[Buffer])
     {Not_End=!Yes_No(Unsaved_Chng_Txt);
      if (Not_End)
		{if (Err_Flag)
		  return;
		 Ch_Window(Buffer);
		}
		break;
	  }
	 }
	}
#if defined(UNIX) | defined(DOS) | defined(WIN32) /* ## looks reasonable */
	if (!Not_End)
	 for (I=0;I<Maxbuffers;I++)
	 {if ((++Buffer)==Maxbuffers)
		Buffer=0;
	  if (Reservation&&File_Reserved[Buffer])
	  {Load_Window(Buffer);
		if (chmod(Outfile,File_Protection[Buffer])!=0)
		Plain_Message(No_Reset_Txt); else
		 Plain_Message(Reset_Txt);
	  }
	 }
#endif
  }
#endif
#endif
 }
 Return_Value=Count;
}

void Fast Rem_Autosave()
{
#if defined(VMS)
 char Autofile[80];
#else
 char Autofile[512];
#endif
 Filnam(Outfile,Autofile);
#if defined(UNIX)
 strcat(Autofile,".aus");
#else
 strcat(Autofile,".AUS");
#endif
#if defined(VMS)
 delete(Autofile);
#else
 unlink(Autofile);
#endif
 Auto_Saved=0;
}

/*      
 * VKS Exit: Always save document in buffer 0 and let the user deside
 *           what he wants to do with other changed buffers
 */
void Fast VKS_Exit()
{
	int	i;
	Not_End = 0;
	for(i = 1; i < Maxbuffers; i++)
		if((Any_Changes[i] & 1) && ! Read_Only[i])
    		{
			Ch_Window(i);
			if(Not_End=!Yes_No(Unsaved_Chng_Txt))
				break;

			Any_Changes[i] = 0;
		}


	if(! Not_End)
	{
		Ch_Window(0);
		Exit_Sedt();
	}
	else
		Return_Value = Count;
}

/*      
 * PowerQuit: Don't bother the user who wants to quit. He is a grown up
 *            individual who blames himself if he makes a mistake ;-)
 */
void Fast VKS_Quit()
{
	int	i;
	for(i = 0; i < Maxbuffers; i++)
		Any_Changes[i] = 0;
	
	Quit();
}

/*      Quit from the SEDT session if there are no buffers with unsaved 
        changes or the user confirms quitting.
*/
void Fast Quit()
{int Buffer;
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Win_Quit();
 if ((Any_Changes[Cur_Buffer]&1)&&!Read_Only[Cur_Buffer])
  return;
 if (Err_Flag)
  return;
#else
#if defined(ATARI) & defined(GEM)
 GEM_Quit();
#else
 Not_End=0;
 Buffer=Cur_Buffer;
 {register int I;
  Sav_Window(Cur_Buffer);
  for (I=0;I<Maxbuffers;I++)
  {if ((++Buffer)==Maxbuffers)
    Buffer=0;
   if (Dl_Pointers[Pointers+Buffer][0]!=0)
   {if ((Any_Changes[Buffer]&1)&&!Read_Only[Buffer])
    {Not_End=!Yes_No(Unsaved_Chng_Txt);
     if (Not_End&&Buffer!=Cur_Buffer)
     {if (Err_Flag)
       return;
      Ch_Window(Buffer);
     }
     break;
    }
   }
  }
#if defined(UNIX) | defined(DOS) | defined(WIN32) /* ## looks reasonable */
  if (!Not_End)
	for (I=0;I<Maxbuffers;I++)
	{if ((++Buffer)==Maxbuffers)
	  Buffer=0;
	 if (Reservation&&File_Reserved[Buffer])
	 {Load_Window(Buffer);
		if (chmod(Outfile,File_Protection[Buffer])!=0)
	  Plain_Message(No_Reset_Txt); else
		Plain_Message(Reset_Txt);
    }
   }
#endif
 }
#endif
#endif
 Return_Value=Count;
}

/*      Insert Tab and Space characters until the cursor is at column Col
*/
void Fast Tab_To(long Col)
{int C;
 B_Goto(Current,My_Pos);
 if (Replace_Mode)
 {Set_Region();
  while (My_Col<Col)
  {if (Current->Pos>=End_Region||Chk_EL(Current,C=B_Get(Current)))
   {if (!Tab_Expand)
     while (((My_Col-1)/8+1)*8+1<=Col)
      Ins_Char(9);
    while (My_Col<Col)
     Ins_Char(' ');
    break;
   } else
   {if (C==9&&My_Col+Char_Rep(C,NULL)>Col)
    {while (My_Col<Col)
      Ins_Char(' ');
    } else
    {My_Col+=Char_Rep(C,NULL);
     My_Pos++;
    }
   }
  }
 } else
 {if (!Tab_Expand)
   while (((My_Col-1)/8+1)*8+1<=Col)
    Ins_Char(9);
  while (My_Col<Col)
   Ins_Char(' ');
 }
}

void Fast A_New_Line()
{long Sel_Pos;
 int Sav_Show_Errors, Sav_Replace_Mode;
 Set_Region();
 if (Replace_Mode&&My_Pos<=End_Region)
 {Sav_Show_Errors=Show_Errors;
  Show_Errors=0;
  Init_Entity();
  Line();
  Move_Entity();
  if (Err_Flag||(My_Pos>=End_Region&&My_Col!=1))
  {Err_Flag=0;
   New_Line((long)1);
   Set_Changes();
  }
  Show_Errors=Sav_Show_Errors;
 } else
 {Sav_Replace_Mode=Replace_Mode;
  Replace_Mode=0;
  New_Line((long)1);
  Set_Changes();
  Replace_Mode=Sav_Replace_Mode;
 }
 if (Blk_Mode&&(Sel_Pos=Points[0])!=0)
 {if (Sel_Pos<My_Pos)
   Fill_Screen(Rel_First,0);
 }
}

/*      Insert a line terminator and move to the new line. Advance the cursor 
        to the left margin position.
*/
void Fast New_Line(long Indent)
{while (Count--&&!Check_CC())
 {B_Goto(Current,My_Pos);
#if ! (defined(UNIX) | defined(MPE))
  Put_A_Char('\r');
  Do_New_Line(2,Indent);
#else
  Do_New_Line(1,Indent);
#endif
 }
}

void Fast Do_New_Line(int N,long Indent)
{Put_A_Char('\n');
 Buf_Lines++;
 if (Auto_Margin&&Indent)
  Find_Margin();
 if (Float_Cursor&&Rel_Last!=Margin)
 {Opt_Scroll(-1,1,Rel_Last);
  Fill_Screen(0,1);
  My_Pos+=N;
  This_Line++;
  My_Line=Home= ++Home;
  Rel_First--;
  Rel_Last--;
 } else
 {My_Pos+=N;
  This_Line++;
  B_Goto(Current,My_Pos);
  if (Rel_First!=0)
  {Opt_Scroll(1,Rel_First,0);
   Fill_Screen(-1,0);
  } else
   Fill_Screen(0,0);
 }
 My_Col=1;
 if (Indent)
 {if (Auto_Margin)
   Tab_To(Word_Wrap); else
  if (Left_Margin!=1)
   Tab_To(Left_Margin);
 }
}

/*      Find the column position of the leftmost non-blank character on the 
        line and set the automatic margin to this column.
*/
int Fast Find_Margin()
{register int This_Col;
 register char This_C;
 long B_End, This_Pos;
 B_End=Current->Bufsize+1;
 This_Col=1;
 B_Goto(Current,My_Pos);
 if (B_B_Search(Current,'\n',(long)1))
  B_Get(Current);
 This_Pos=Current->Pos;
 while (This_Pos!=B_End&&((This_C=B_Get(Current))==' '||This_C==9))
 {This_Col=This_C==' '?This_Col+1:((This_Col-1)/8+1)*8+1;
  This_Pos++;
 }
#if ! (defined(UNIX) | defined(MPE))
 if (This_C=='\n'||This_C=='\r')
#else
 if (This_C=='\n')
#endif
 {if (Word_Wrap!=1)
  {Word_Wrap=1;
  if (Show_Ruler)
    S_Ruler();
  }
 } else
 if (Word_Wrap!=This_Col)
 {Word_Wrap=This_Col;
  if (Show_Ruler)
   S_Ruler();
 }
 B_Goto(Current,My_Pos);
 return Word_Wrap!=0;
}

/*      Delete an entity from the buffer.
*/
void Fast Del_It()
{register int Index;
 if (!Get_Entity()||Skip)
  return;
 Index=Get_E_Type();
 if (Blk_Mode&&(E_Type==E_Paste||E_Type==E_NPaste))
  Blk_Rem_It(Index); else
  Rem_It(Index);
 Set_Changes();
}

/*      Copy an entity into a delete buffer.
*/
void Fast Get()
{register int Index;
 if (!Get_Entity()||Skip)
  return;
 Index=Get_E_Type();
 if (E_Type==E_Paste)
  Clear_Region();
 if (Blk_Mode&&(E_Type==E_Paste||E_Type==E_NPaste))
  Blk_Rem_It(Index); else
  Rem_It(Index);
}

/*      Return the appropriate delete buffer for an entity.
*/
int Fast Get_E_Type()
{switch (E_Type)
 {case E_Char:
   return(Char_String);
  case E_Word:
   return(Word_String);
  case E_Line:
   return(Line_String);
  case E_Paste:
  case E_NPaste:
   return(Past_String);
  case E_Par:
   return(Paragraph_String);
  case E_Pag:
   return(Page_String);
  case E_Sen:
   return(Sen_String);
  default:
   return(Unknown_String);
 }
}

/*      Set default direction forward
*/
void Fast Advance()
{Com_Direction=Def_Direction=1;
 Hdr_Changes=1;
}

/*      Set default direction backward
*/
void Fast Backup()
{Com_Direction=Def_Direction=0;
 Hdr_Changes=1;
}

/*      Sets the Select point to the current cursor position
*/
void Fast Select()
{long *P;
 if ((P=Points)[0]!=0)
 {Init_Entity();
  E_Type=E_Paste;
  Sel_Range();
  Clear_Region();
 }
 P[0]=My_Pos;
}

/*      Inserts the contents of the paste buffer at the current cursor 
        position.
*/
void Fast Paste()
{
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Win_Load_Clip();
#endif
 while (Count--&&!Check_CC())
  Undelete(Past_String);
}

/*      Inserts the contents of the character delete buffer at the current 
        cursor position.
*/
void Fast Und_Char()
{while (Count--&&!Check_CC())
  Undelete(Char_String);
}

/*      Inserts the contents of the word delete buffer at the current 
        cursor position.
*/
void Fast Und_Word()
{while (Count--&&!Check_CC())
  Undelete(Word_String);
}

/*      Inserts the contents of the line delete buffer at the current 
        cursor position.
*/
void Fast Und_Line()
{while (Count--&&!Check_CC())
  Undelete(Line_String);
}

/*      Inserts the contents of the paragraph delete buffer at the current 
        cursor position.
*/
void Fast Und_Par()
{while (Count--&&!Check_CC())
  Undelete(Paragraph_String);
}

/*      Inserts the contents of the page delete buffer at the current 
        cursor position.
*/
void Fast Und_Pg()
{while (Count--&&!Check_CC())
  Undelete(Page_String);
}

/*      Inserts the contents of the unknown type delete buffer at the current 
        cursor position.
*/
void Fast Und_Unknown()
{while (Count--&&!Check_CC())
  Undelete(Unknown_String);
}

/*      Inserts the contents of the sentence delete buffer at the current 
        cursor position.
*/
void Fast Und_Sen()
{while (Count--&&!Check_CC())
  Undelete(Sen_String);
}

/*      Switch to a different window.
*/
void Fast Window()
{if (Count>=Maxbuffers)
  Error(2,Illegal_Window_Txt); else
  Ch_Window((int)Count);
}

/*      Insert the Ascii character value Count at the current cursor position.
*/
void Fast Ascii()
{if (Count>255)
 {Error(2,Illegal_ASCII_Txt);
  return;
 }
 Ins_Char((int)Count);
 Set_Changes();
}

/*      Get the delimited command argument into the array Com_Arg.
*/
int Fast Get_Arg()
{register char C;
 char Delim;
 register int I;
 if (Com.Pos>Com.Bufsize)
 {Delim_Error();
  return(0);
 }
 I=0;
 Delim=B_Get(&Com);
 while (1)
 {if (Com.Pos>Com.Bufsize)
  {Delim_Error();
   return 0;
  }
  if ((C=B_Get(&Com))==Delim)
   break;
  Com_Arg[I++]=C;
  if (I>=1024)
  {Error(2,Too_Long_Arg_Txt);
   return(0);
  }
 }
 Com_Arg[I]=0;
 Arg_Length=I;
 return(1);
}

/*      Insert the contents of a file at the current cursor position.
*/
void Fast File_Get()
{int Was_Empty;
 if (!Get_Arg()||Skip)
  return;
 Was_Empty=Current->Bufsize==0&&Outfile==NULL;
 B_Goto(Current,My_Pos);
 Include_File(Com_Arg,File_Not_Found_Txt);
 Fill_Screen(0,Rel_Last);
 if (!Err_Flag&&!Was_Empty)
  Set_Changes();
}

/*      Zero the current buffer. If unsaved changes have been made ask for
        confirmation.
*/
void Fast Zero()
{long *P;
 if ((Any_Changes[Cur_Buffer]&1)&&!Read_Only[Cur_Buffer])
  if (!Yes_No(Unsaved_Chng_Sure_Txt))
   return;
#if defined(UNIX) | defined(DOS) | defined(WIN32) /* ## looks reasonable */
 if (Reservation&&File_Reserved[Cur_Buffer])
 {if (chmod(Outfile,File_Protection[Cur_Buffer])!=0)
	Plain_Message(No_Reset_Txt); else
	Plain_Message(Reset_Txt);
  File_Reserved[Cur_Buffer]=0;
 }
#endif
 B_Reset(Current);
 This_Line=My_Col=My_Pos=1;
 Pos_Cursor();
 P=Points;
 {register int I;
  for (I=0;I<=Maxpoints+2;I++)
   P[I]=0;
 }
 P[Maxpoints+3]=P[Maxpoints+4]=1;
 Buf_Lines=0;
 Read_Only[Cur_Buffer]=Count==0;
#if defined(UNIX)
 File_Protection[Cur_Buffer]= -1;
#endif
 Cur_Attributes=0;
 Clr_Changes();
 Infile=Outfile=NULL;
 Auto_Saved=0;
 Fill_Screen(Rel_First,Rel_Last);
 S_Header();
#if (defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV)))
#if defined(ATARI) & defined(GEM)
 GEM_Name(top_window,NULL);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Win_Name(NULL);
#endif
#if defined(DOS) & defined(TV)
 TV_Name(NULL);
#endif
 W_D_Border();
#endif
 if (Auto_Saved)
  Rem_Autosave();
}

/*      Mark the current cursor position in mark number Count.
*/
void Fast Mark_Pos()
{if (Count<1||Count>Maxpoints)
 {Error(2,Illegal_Mark_Txt);
  return;
 }
 if (Points[Count]!=0)
 {Points[Count]=End_Points[Count]=(Cur_Direction=='-')?0L:My_Pos;
  Fill_Screen(Rel_First,Rel_Last);
 } else
 {Points[Count]=End_Points[Count]=(Cur_Direction=='-')?0L:My_Pos;
  Fill_Rest();
 }
}

/*      Mark the current cursor position as the end of mark number Count.
*/
void Fast End_Mark()
{if (Count<1||Count>Maxpoints)
 {Error(2,Illegal_Mark_Txt);
  return;
 }
 End_Points[Count]=(Cur_Direction=='-')?Points[Count]:My_Pos;
 Fill_Screen(Rel_First,0);
}

/*      Get the arguments to the Subs command and then substritute
        the next occurrence of the search string with the replace string.
*/
void Fast Subs()
{Get_Arg();
 if (!Skip)
 {if (Com_Arg[0]!=0)
   Ins_Del(Dl_Pointers[Search_String]);
 }
 B_Goto(&Com,Com.Pos-1);
 if (!Get_Arg()||Skip)
  return;
 if (Com_Arg[0]!=0)
  Ins_Del(Dl_Pointers[Subs_String]);
 Subs_Next();
}

/*      Get the arguments to the Subs command and then substritute
        the next occurrence of the search string with the replace string.
*/
void Fast Subs_Conf()
{Get_Arg();
 if (!Skip)
 {if (Com_Arg[0]!=0)
   Ins_Del(Dl_Pointers[Search_String]);
 }
 B_Goto(&Com,Com.Pos-1);
 if (!Get_Arg()||Skip)
  return;
 if (Com_Arg[0]!=0)
  Ins_Del(Dl_Pointers[Subs_String]);
 Subs_Next_Conf();
}

/*      Insert the contents of the command argument into the Del buffer and make
	the del pointer W point to it.
*/
void Fast Ins_Del(long *W)
{Erase_Del(W);
 B_Put_S(&Del,Com_Arg,Arg_Length);
 W[1]=Arg_Length;
}

/*      Search in the current direction for the search string and replace it
        with the replace string.
*/
void Fast Subs_Next()
{int First;
 register long Lines, Chars;
 long *P;
 First=1;
 while (!Err_Flag&&Count--&&!Check_CC())
 {Init_Entity();
  Ent_Direction=Direction;
  if (First)
  {Find_This(Direction);
   First=0;
  } else
   Find_This(0);
  if (!Err_Flag)
  {Move_Entity();
   Init_Entity();
   if (Reg_Search)
    Ent_Count=Exp->endp[0]-Exp->startp[0]; else
    Ent_Count=Dl_Pointers[Search_String][1];
   A_Char();
   Erase_Del(P=Dl_Pointers[Unknown_String]);
   Chars=End_Pos-Beg_Pos;
   Lines=End_Line-Beg_Line;
   B_Goto(Current,Beg_Pos);
   B_Copy(Current,&Del,Chars);
   B_Goto(Current,Beg_Pos);
   B_Rem(Current,Chars);
   Adj_Points(-(P[1]=Chars),Beg_Pos);
   if (Lines>0)
   {Adj_Lines(-Lines);
    if (Lines<Rel_Last&&(Max_Scroll?Max_Scroll>=Lines:1))
    {Opt_Scroll((int)Lines,My_Col==1?0:1,Rel_Last);
     Fill_Screen((int)(Rel_Last-Lines+1),Rel_Last);
    } else
     Fill_Screen(1,Rel_Last);
   }
   B_Goto(Current,My_Pos);
   if ((P=Dl_Pointers[Subs_String])[0]==0)
    Fill_Rest(); else
   {B_Goto(&Del,P[0]);
    if ((Chars=P[1])==0)
     Fill_Rest(); else
    {if (Reg_Search)
     {B_Get_N(&Del,Rep_Str,(int)Chars);
      Rep_Str[Chars]=0;
      regsub(Exp,Rep_Str,Com_Arg);
      Chars=strlen(Com_Arg);
      B_Put_S(Current,Com_Arg,(int)Chars);
     } else
     {Adj_Points(Chars,My_Pos);
      B_Copy(&Del,Current,Chars);
     }
     B_Goto(Current,My_Pos);
     End_Pos=Beg_Pos+Chars;
     Lines=0;
     while (B_Search(Current,'\n',My_Pos+Chars))
      Lines++;
     B_Goto(Current,My_Pos);
     if (Lines)
     {Adj_Lines(Lines);
      if (Lines<Rel_Last&&(Max_Scroll?Max_Scroll>=Lines:1))
      {Opt_Scroll((int) (-Lines),0,Rel_Last);
       Fill_Screen(0,(int)Lines);
      } else
       Fill_Screen(0,Rel_Last);
     } else
      Fill_Rest();
     if (Direction)
     {Init_Entity();
      Ent_Count=Chars;
      A_Char();
      Move_Entity();
     }
    }
   }
  }
 }
 Set_Changes();
}

/*      Search in the current direction for the search string and replace it
        with the replace string. Confirm every substitution
*/
void Fast Subs_Next_Conf()
{int First;
 register long Lines, Chars;
 long *P;
 First=1;
 while (!Err_Flag&&Count--&&!Check_CC())
 {Init_Entity();
  Ent_Direction=Direction;
  if (First)
  {Find_This(Direction);
   First=0;
  } else
   Find_This(0);
  if (!Err_Flag)
  {Move_Entity();
   if (Yes_No(Replace_This_Txt))
   {Init_Entity();
    if (Reg_Search)
     Ent_Count=Exp->endp[0]-Exp->startp[0]; else
     Ent_Count=Dl_Pointers[Search_String][1];
    A_Char();
    Erase_Del(P=Dl_Pointers[Unknown_String]);
    Chars=End_Pos-Beg_Pos;
    Lines=End_Line-Beg_Line;
    B_Goto(Current,Beg_Pos);
    B_Copy(Current,&Del,Chars);
    B_Goto(Current,Beg_Pos);
    B_Rem(Current,Chars);
    Adj_Points(-(P[1]=Chars),Beg_Pos);
    if (Lines>0)
    {Adj_Lines(-Lines);
     if (Lines<Rel_Last&&(Max_Scroll?Max_Scroll>=Lines:1))
     {Opt_Scroll((int)Lines,My_Col==1?0:1,Rel_Last);
      Fill_Screen((int)(Rel_Last-Lines+1),Rel_Last);
     } else
      Fill_Screen(1,Rel_Last);
    }
    B_Goto(Current,My_Pos);
    if ((P=Dl_Pointers[Subs_String])[0]==0)
     Fill_Rest(); else
    {B_Goto(&Del,P[0]);
     if ((Chars=P[1])==0)
      Fill_Rest(); else
     {if (Reg_Search)
      {B_Get_N(&Del,Rep_Str,(int)Chars);
       Rep_Str[Chars]=0;
       regsub(Exp,Rep_Str,Com_Arg);
       Chars=strlen(Com_Arg);
       B_Put_S(Current,Com_Arg,(int)Chars);
      } else
      {Adj_Points(Chars,My_Pos);
       B_Copy(&Del,Current,Chars);
      }
      B_Goto(Current,My_Pos);
      End_Pos=Beg_Pos+Chars;
      Lines=0;
      while (B_Search(Current,'\n',My_Pos+Chars))
       Lines++;
      B_Goto(Current,My_Pos);
      if (Lines)
      {Adj_Lines(Lines);
       if (Lines<Rel_Last&&(Max_Scroll?Max_Scroll>=Lines:1))
       {Opt_Scroll((int) (-Lines),0,Rel_Last);
        Fill_Screen(0,(int)Lines);
       } else
        Fill_Screen(0,Rel_Last);
      } else
       Fill_Rest();
      if (Direction)
      {Init_Entity();
       Ent_Count=Chars;
       A_Char();
       Move_Entity();
      }
     }
    }
   }
  }
 }
 Set_Changes();
}

/*      Insert the representation of character C at the current cursor
        position.
*/
void Fast Put_Rep(int C)
{char S[Maxcol+1]; 
 int Sav_Attributes;
#ifdef VKS
/*
 * DIRTY PATCH: if characters come in negative ring a bell on ASCII 7
 *              else ignore the bell character (as was before)
 *
 * APOLOGY: sorry for introducing hacker methods into your programming,
 *          but it was a way of leaving most of you calls to Put_Rep 
 *          unchanged.
 */
 int do_bell = 0;

 if(C < 0)
 {
   do_bell = 1;
   C = -C;
 }
 if ((C>=' '&&C<='~')||(C>160&&C<255)||(do_bell&&C==7))
#else
#if defined(UTF8_HACK)
 if ((C>=' '&&C<='~'))
#else
 if ((C>=' '&&C<='~')||(C>160&&C<255))
#endif
#endif
  Opt_Char((char)C); else
 if (C>=' '||(C==9&&Expand_Tabs)||!Control_Display)
 {Char_Rep(C,S);
  Opt_String(S);
 } else
 {if ((Sav_Attributes=Cur_Attributes)&A_Reverse)
  {Cur_Attributes&=!A_Reverse;
   Cur_Attributes|=A_Bold;
  } else
   Cur_Attributes|=A_Reverse|A_Bold;
  Opt_Char((char)(C+64));
  Cur_Attributes=Sav_Attributes;
 } 
}

/*      Insert the contents of the command argument at the current cursor 
        position.
*/
void Fast Insert()
{register long Lines, L;
 register char *S;
 B_Goto(Current,My_Pos);
 if (!Get_Arg()||Skip)
  return;
 if (!Replace_Mode)
 {L=Arg_Length;
  B_Put_S(Current,Com_Arg,Arg_Length);
  Adj_Points(L,My_Pos);
  Init_Entity();
  Ent_Count=L;
  A_Char();
  if ((Lines=End_Line-This_Line)!=0)
  {Adj_Lines(Lines);
   if (Lines<Rel_Last&&(Max_Scroll?Max_Scroll>=Lines:1))
   {Opt_Scroll((int)-Lines,0,Rel_Last);
    Fill_Screen(0,(int)Lines);
   } else
    Fill_Screen(0,Rel_Last);
  } else
   Fill_Rest();
  Move_Entity();
 } else
 {L=strlen(S=Com_Arg);
  while (L--)
   Ins_Char(*(S++));
 }
 Set_Changes();
}

/*      Edit the file given in the command argument in the current buffer.
*/
void Fast File_Edit()
{if (!Get_Arg()||Skip)
  return;
 Edit_File(Com_Arg);
}

void Fast Edit_File(char *S)
{long *P;
#if defined(UNIX) | defined(DOS) | defined(WIN32)
 struct stat File_Stat;
#endif
 if ((Any_Changes[Cur_Buffer]&1)&&!Read_Only[Cur_Buffer])
 {if (Outfile==NULL)
  {if (!Yes_No(Forget_Buffer_Txt))
    return;
  } else
  if (Yes_No(Save_Changes_Txt))
   Wr_Buffer(Outfile);
 }
 if (Err_Flag)
  return;
#if defined(UNIX) | defined(DOS) | defined(WIN32)
 if (Reservation&&File_Reserved[Cur_Buffer])
 {if (chmod(Outfile,File_Protection[Cur_Buffer])!=0)
	Plain_Message(No_Reset_Txt); else
	Plain_Message(Reset_Txt);
  File_Reserved[Cur_Buffer]=0;
 }
#endif
 if (Auto_Saved)
  Rem_Autosave();
 Read_Only[Cur_Buffer]=Count==0;
#if defined (UNIX)
 File_Protection[Cur_Buffer]= -1;
 if (stat(S,&File_Stat)!=0)
 {Plain_Message(No_Stat_Txt);
  File_Reserved[Cur_Buffer]=0;
  File_Protection[Cur_Buffer]= -1;
 } else
 {File_Protection[Cur_Buffer]=File_Stat.st_mode;
  if (Reservation&!Read_Only[Cur_Buffer])
  {File_Reserved[Cur_Buffer]=0;
	if ((File_Stat.st_mode&(S_IWUSR+S_IWGRP+S_IWOTH))==0)
	{if (Yes_No(File_Reserved_Txt)==1)
	 {Read_Only[Cur_Buffer]=1;
	 } else
	 {return;
	 }
	}
	{if (File_Stat.st_uid==My_UID)
	 {if (chmod(S,File_Stat.st_mode&~(S_IWUSR+S_IWGRP+S_IWOTH))!=0)
	  {Plain_Message(No_Protection_Change_Txt);
		Read_Only[Cur_Buffer]=1;
	  } else
	  {Plain_Message(Reserved_Txt);
		File_Reserved[Cur_Buffer]=1;
		File_Protection[Cur_Buffer]=File_Stat.st_mode;
	  }
	 } else
	 if (File_Stat.st_gid==My_GID)
	 {Plain_Message(Group_Txt);
	  Read_Only[Cur_Buffer]=1;
	 } else
	 {Plain_Message(Other_Txt);
	  Read_Only[Cur_Buffer]=1;
	 }
	}
  }
 }
#endif
#if defined (DOS) | defined(WIN32)
 File_Protection[Cur_Buffer]= -1;
 if (stat(S,&File_Stat)!=0)
 {Plain_Message(No_Stat_Txt);
  File_Reserved[Cur_Buffer]=0;
  File_Protection[Cur_Buffer]= -1;
 } else
 {File_Protection[Cur_Buffer]=File_Stat.st_mode;
  if (Reservation&!Read_Only[Cur_Buffer])
  {File_Reserved[Cur_Buffer]=0;
	if ((File_Stat.st_mode&(S_IWRITE))==0)
	{if (Yes_No(File_Reserved_Txt)==1)
	 {Read_Only[Cur_Buffer]=1;
	 } else
	 {return;
	 }
	}
	{if (chmod(S,File_Stat.st_mode&~(S_IWRITE))!=0)
	 {Plain_Message(No_Protection_Change_Txt);
	  Read_Only[Cur_Buffer]=1;
	 } else
	 {Plain_Message(Reserved_Txt);
	  File_Reserved[Cur_Buffer]=1;
	  File_Protection[Cur_Buffer]=File_Stat.st_mode;
	 }
	}
  }
 }
#endif
 {register int I;
  I=0;
  New_File[I]=S[I];
  while (New_File[I])
  {I++;
   New_File[I]=S[I];
  }
 }
 B_Reset(Current);
 This_Line=My_Col=My_Pos=1;
 Pos_Cursor();
 P=Points;
 {register int I;
  for (I=0;I<=Maxpoints+2;I++)
   P[I]=0;
 }
 P[Maxpoints+3]=P[Maxpoints+4]=1;
 Buf_Lines=0;
 Cur_Attributes=0;
 Clr_Changes();
 Read_File(Outfile=Infile=New_File);
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
 Fill_Screen(Rel_First,Rel_Last);
 S_Header();
 if (!Err_Flag&&Save_Context)
  Read_Context();
}

void Fast C_Read_L(struct File *F,long *D)
{if (F_gets(Dsk_Rec,Blksize,F)==NULL)
  return;
 *D=atol(Dsk_Rec);
}

void Fast C_Read_I(struct File *F,int *D)
{if (F_gets(Dsk_Rec,Blksize,F)==NULL)
  return;
 *D=atol(Dsk_Rec);
}

void Fast Read_Context()
{struct File F;
 long Save_Pos;
 Filnam(Infile,Dsk_Rec);
#if defined(UNIX)
 strcat(Dsk_Rec,".ctx");
#else
 strcat(Dsk_Rec,".CTX");
#endif
 if (F_open(Dsk_Rec,"r",&F)==NULL)
 {if (Save_Context==1)
  Plain_Message(No_Context_Txt);
 } else
 {if (Err_Flag)
   return;
  if (F_gets(Dsk_Rec,132,&F)==NULL)
  {Plain_Message(Read_Context_Txt);
   F_close(&F);
   return;
  }
  Filjnam(Infile,Com_Arg);
  if (strcmp(Dsk_Rec,Com_Arg)!=0)
  {Plain_Message(Context_Match_Txt);
   F_close(&F);
   return;
  }
  C_Read_L(&F,&Save_Pos);
  {register int I;
   for (I=0;I<=Maxpoints;I++)
   {C_Read_L(&F,&Points[I]);
    C_Read_L(&F,&End_Points[I]);
   }
  }
  C_Read_L(&F,&Indent);
  C_Read_L(&F,&Word_Wrap);
  C_Read_L(&F,&Left_Margin);
  C_Read_L(&F,&Right_Margin);
  C_Read_I(&F,&Justify);
  C_Read_I(&F,&Auto_Margin);
  {register int I;
   for (I=0;I<512;I++)
    C_Read_I(&F,&Tabs[I]);
  }
  F_close(&F);
  if (Save_Pos>0&&Save_Pos<=Current->Bufsize+1)
  {Init_Entity();
   Ent_Count=Save_Pos-1;
   A_Char();
   Move_Entity();
  } else
   Plain_Message(Bad_Context_Cursor_Txt);
  Points[0]=0;
 }
}

/*      Save the contents of the current buffer in the file given in the
        command argument. If the command argument is empty the file is saved
        in the output file of the buffer.
*/
void Fast File_Save()
{char *P;
 if (!Get_Arg()||Skip)
  return;
 if (Read_Only[Cur_Buffer]&&!Yes_No(Txt_Read_Only))
  return;
 if (Err_Flag)
  return;
 if (Com_Arg[0]==0)
 {if ((P=Outfile)==NULL||P[0]==0)
   Error(2,No_Output_File_Txt); else
  {Wr_Buffer(P);
  }
 } else
 {if (Auto_Saved)
   Rem_Autosave();
  strcpy(New_File,Com_Arg);
  Wr_Buffer(Outfile=New_File);
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
  S_Header();
 }
 if (!Err_Flag)
  Clr_Changes();
}

/*      Write the contents of the current buffer to the file given in the
        command argument. 
*/
void Fast File_Write()
{if (!Get_Arg()||Skip)
  return;
 if (Com_Arg[0]==0)
  Error(2,No_Output_File_Txt); else
 {Wr_Buffer(Com_Arg);
 }
}

/*      Check whether we have reached the end of the command buffer.
*/
int Fast End_Com()
{return(Com.Pos>Com.Bufsize);
}

/*      Define the contents of the key given in the first command argument as
        the string given as the second command argument.
*/
void Fast Def_Key_Function()
{long Sav_Pos, St_Pos;
 int Length;
 Gold=Function=0;
 Get_Key();
 if (End_Com())
 {Com_Error();
  return;
 }
 St_Pos=Com.Pos+1;
 if (!Get_Arg()||Skip)
  return;
 if ((Function=Map_Tab[Map_Index].F_Code)<=0||Function<=Top_Func)
 {if (Next_Func==Maxfunc-1)
  {Error(2,Max_Func_Txt);
   return;
  } else
   Map_Tab[Map_Index].F_Code=Function= ++Next_Func;
 }
 Length=Arg_Length;
 Sav_Pos=Com.Pos;
 Move_Key(Key_Addr(Function),St_Pos,(long)Length);
 B_Goto(&Com,Sav_Pos);
}

void Fast Def_Key_Alpha()
{Gold=0;
 Key_Number= -1;
 Def_Key();
}

void Fast Def_Key_Key()
{Gold=0;
 Key_Number=0;
 Def_Key();
}

void Fast Def_Key_G_Alpha()
{Gold=1;
 Key_Number= -1;
 Def_Key();
}

void Fast Def_Key_G_Key()
{Gold=1;
 Key_Number=0;
 Def_Key();
}

void Fast Def_Key()
{long Sav_Pos, St_Pos;
 int Length;
 int C;
 Function=Character=0;
 {register int I;
  for (I=1;I<=3;I++)
  {if (Com.Pos>Com.Bufsize)
   {Syntax_Error();
    return;
   }
   if ((C=B_Get(&Com))<'0'||C>'9')
   {Syntax_Error();
    return;
   }
   Character=Character*10+C-'0';
  }
 }
 St_Pos=Com.Pos+1;
 if (!Get_Arg()||Skip)
  return;
 {register int I;
  for (I=0;I<=Map_Defs;I++)
  {if (I==Map_Defs)
   {Map_Defs++;
    Map_Tab[I].Scancode=0;
    Map_Tab[I].Char=Character;
    Map_Tab[I].Flags=0;
    if (Gold)
     Map_Tab[I].Flags|=Gold_Code;
    if (Key_Number>=0)
     Map_Tab[I].Flags|=Key_Code; else
     Map_Tab[I].Flags|=Char_Code;
    Map_Tab[I].F_Code= -1;
    Map_Tab[I].Index=0;
    break;
   } else
   {if (Map_Tab[I].Flags&Gold_Code?Gold:!Gold)
    {if ((Key_Number==0&&(Map_Tab[I].Flags&Key_Code)&&
       Map_Tab[I].Char==Character)||
      (Key_Number==-1&&(Map_Tab[I].Flags&Char_Code)&&
       Map_Tab[I].Char==Character))
      break;
    }
   }
  }
  if ((Function=Map_Tab[I].F_Code)<=0||Function<=Top_Func)
  {if (Next_Func==Maxfunc-1)
   {Error(2,Max_Func_Txt);
    return;
   } else
    Map_Tab[I].F_Code=Function= ++Next_Func;
  }
 }
 Length=Arg_Length;
 Sav_Pos=Com.Pos;
 Move_Key(Key_Addr(Function),St_Pos,(long)Length);
 B_Goto(&Com,Sav_Pos);
}

/*      Define the contents of the key given in the first command argument as
        the string given as the second command argument.
*/
void Fast Def_Key_Number()
{Gold=Function=0;
 Get_Key();
 if (Err_Flag||Skip)
  return;
 Map_Tab[Map_Index].F_Code=Count;
}

void Fast Def_Function()
{long St_Pos, Sav_Pos, Length;
 St_Pos=Com.Pos+1;
 if (!Get_Arg()||Skip)
  return;
 Sav_Pos=Com.Pos;
 Length=Arg_Length;
 Move_Key(Key_Addr(Function=Count),St_Pos,(long)Length);
 if (Function>Top_Func)
  Top_Func=Function;
 B_Goto(&Com,Sav_Pos);
}

/*      Reset the select point.
*/
void Fast Reset()
{Init_Entity();
 E_Type=E_Paste;
 Sel_Range();
 Clear_Region();
}

/*      Attempt to undo the effect of the last command executed.
*/
void Fast Undo()
{if (Lst_Cmd==C_Movecursor)
 {Init_Entity();
  if (Lst_Char_Move>0)
  {Ent_Direction=0;
   Beg_Pos-=Lst_Char_Move;
   Beg_Line-=Lst_Line_Move;
  } else
  {End_Pos-=Lst_Char_Move;
   End_Line-=Lst_Line_Move;
  }
  Lst_Char_Move=(-Lst_Char_Move);
  Lst_Line_Move=(-Lst_Line_Move);
  Lst_Cmd=This_Cmd;
  This_Cmd=C_Movecursor;
  Move_Entity();
 } else
 if (Lst_Cmd==C_Subs)
 {if (My_Pos!=Beg_Pos)
  {Ent_Direction=0;
   Move_Entity();
  }
  End_Pos=My_Pos+Dl_Pointers[Subs_String][1];
  Beg_Line=This_Line;
  Rem_It(-1);
  Undelete(Unknown_String);
 } else
 if (Lst_Cmd==C_Delete)
 {switch (E_Type)
  {case E_Char:
    {Und_Char();
     break;
    }
   case E_Word:
    {Und_Word();
     break;
    }
   case E_Line:
    {Und_Line();
     break;
    }
   case E_Paste:
   case E_NPaste:
    {Paste();
     break;
    }
   case E_Par:
    {Und_Par();
     break;
    }
   case E_Pag:
    {Und_Pg();
     break;
    }
   case E_Sen:
    {Und_Sen();
     break;
    }
   default:
    Und_Unknown();
  }
 } else
 {Error(2,Cannot_Undo_Txt);
  return;
 }
}

/*      Determine whether there are any characters waiting in the typeahead 
        buffer.
*/
int Fast Input_Test()
{
#if defined(VMS)
 return T$Typeahead()!=0;
#endif
#if defined(WIN32)
 { /* scope */
   int WIN32istypeahead(void);
   return WIN32istypeahead();
 }
#endif
#if defined(DOS) | defined(NT)
#if !defined(Windows)
#if defined(PC100)
 return (bdos(11,0)&0x00FF)!=0;
#else
 if (Firmware&1)
  return ANYEXT(); else
  return ANYCHR();
#endif
#endif
#endif
#if defined(OS2)
 struct _KBDKEYINFO Key_Data;
 KbdPeek((PKBDKEYINFO)&Key_Data,0);
 return Key_Data.fbStatus!=0;
#endif
#if defined(ATARI) & !defined(GEM)
 return Cconis()||Key_Count;
#endif
/* ## TODO WIN32  ANYCHR or how else to handle this, does this even matter for win32? */
#if ((defined(DOS) | defined(NT)) & defined(Windows)) | (defined(ATARI) & defined(GEM)) | defined(UNIX) | defined(WIN32)
 return 1;
#endif
}

void Fast Test_Input()
{Err_Flag=Input_Test();
}

/*      Move to the next tab position on the ruler.
*/
void Fast Tab()
{register long New_Col;
 while (Count--&&!Check_CC())
 {if (Indent>My_Col)
   New_Col=Indent; else
  {New_Col=My_Col;
   while (1)
    if ((++New_Col)>512)
    {if (New_Col%8==1)
      break;
    } else
    if (Tabs[New_Col])
     break;
    if (Word_Wrap>My_Col&&Word_Wrap<New_Col)
     New_Col=Word_Wrap;
  }
  Tab_To(New_Col);
 }
 Set_Changes();
}

/*      Toggle between insert and replace mode.
*/
void Fast Overstrike_Mode()
{Replace_Mode=(Replace_Mode?0:1);
 Hdr_Changes=1;
}

/*      Toggle between insertion and expansion of tab characters.
*/
void Fast Tab_Toggle()
{Tab_Expand=(Tab_Expand?0:1);
 Hdr_Changes=1;
}

/*      Display the current column on the bottom line of the display.
*/
void Fast Show_Column()
{char Display[15];
 sprintf(Display,(char *)Show_Column_Txt,My_Col);
 Plain_Message(Display);
}

/*      Load the ruler from the saved ruler given in Count.
*/
void Fast Get_Ruler()
{if (Count>9)
 {Error(2,Illegal_Ruler_Txt);
  return;
 }
 Readruler((char)(Count+'0'));
 if (Show_Ruler)
  S_Ruler();
}

/*      Toggle between automatic screen shifting and no screen shifting.
*/
void Fast Shift_Toggle()
{Do_Shift=(Do_Shift?0:1);
 Hdr_Changes=1;
}

void Fast Case_Toggle()
{Case_Sensitive=(Case_Sensitive?0:1);
 Hdr_Changes=1;
}

void Fast Ins_Defs_Toggle()
{Ins_Defs=Ins_Defs?0:1;
 Hdr_Changes=1;
}

void Fast Cursor_Free_Toggle()
{Cursor_Free=Cursor_Free?0:1;
 Hdr_Changes=1;
}

void Fast Change_Window()
{
  Other_Window();
}

/*      Set the error level that will ring the terminal bell.
*/
void Fast Err_Level()
{Bell_Level=Count;
}

void Fast Tog_Marks()
{Show_Marks=Show_Marks?0:1;
 Fill_Screen(Rel_First,Rel_Last);
}

void Fast Tog_Block()
{register int Sel_Pos;
 Blk_Mode=Blk_Mode?0:1;
 Hdr_Changes=1;
 if ((Sel_Pos=Points[0])!=0)
 {if (Sel_Pos<My_Pos)
   Fill_Screen(Rel_First,0); else
   Fill_Screen(0,Rel_Last);
 }
}

void Fast Tog_Control()
{Control_Display=Control_Display?0:1;
 Fill_Screen(Rel_First,Rel_Last);
 if (My_Col!=1)
 {Init_Entity();
  Backto_This(My_Pos);
 }
}

void Fast Tog_Direction()
{Com_Direction=Def_Direction=Def_Direction?0:1;
 Hdr_Changes=1;
}

void Fast Show_Version()
{Plain_Message(Version);
}

void Fast Tog_Cursor()
{Float_Cursor=Float_Cursor?0:1;
 Pos_Cursor();
 Fill_Screen(Rel_First,Rel_Last);
}

void Fast Tog_Reg_Search()
{Reg_Search=Reg_Search?0:1;
 Hdr_Changes=1;
}

void Fast Tog_Ruler()
{Show_Ruler=Show_Ruler?0:1;
 if (Show_Ruler)
 {Rel_First++;
  S_Ruler();
 } else
 {Rel_First--;
  Fill_Screen(Rel_First,Rel_First);
 }
 Pos_Cursor();
 Fill_Screen(Rel_First,Rel_Last);
}

void Fast Curs_Top()
{register int I;
 if (Float_Cursor)
 {if (Rel_First<-1)
  {I=Rel_First+1;
   if (Max_Scroll?Max_Scroll>=-I:1)
   {Opt_Scroll(-I,Rel_First,Rel_Last);
    Home=My_Line+=I;
    Rel_First-=I;
    Rel_Last-=I;
    Fill_Screen(Rel_Last+I-1,Rel_Last);
   } else
   {Home=My_Line+=I;
    Rel_First-=I;
    Rel_Last-=I;
    Fill_Screen(Rel_First,Rel_Last);
   }
  }
 }
}

void Fast Curs_Bottom()
{register int I;
 if (Float_Cursor)
 {if (Rel_Last>1)
  {I=Rel_Last-1;
   if (I>This_Line+Rel_First-1)
    I=This_Line+Rel_First-1;
   if (I>0)
   {if (Max_Scroll?Max_Scroll>=I:1)
    {Opt_Scroll(-I,Rel_First,Rel_Last);
     Home=My_Line+=I;
     Rel_First-=I;
     Rel_Last-=I;
     Fill_Screen(Rel_First,Rel_First+I-1);
    } else
    {Home=My_Line+=I;
     Rel_First-=I;
     Rel_Last-=I;
     Fill_Screen(Rel_First,Rel_Last);
    }
   }
  }
 }
}

void Fast Curs_Middle()
{register int I;
 if (Float_Cursor)
 {I=(Rel_First+Rel_Last+1/*mdj +allow offset from middle*/+Count)/2;
  if (I)
  {if (I>This_Line+Rel_First-1)
    I=This_Line+Rel_First-1;
   if (I>0)
   {if (Max_Scroll?Max_Scroll>=I:1)
    {Opt_Scroll(-I,Rel_First,Rel_Last);
     Home=My_Line+=I;
     Rel_First-=I;
     Rel_Last-=I;
     Fill_Screen(Rel_First,Rel_First+I-1);
    } else
    {Home=My_Line+=I;
     Rel_First-=I;
     Rel_Last-=I;
     Fill_Screen(Rel_First,Rel_Last);
    }
   } else
   if (I<0)
   {if (Max_Scroll?Max_Scroll>=-I:1)
    {Opt_Scroll(-I,Rel_First,Rel_Last);
     Home=My_Line+=I;
     Rel_First-=I;
     Rel_Last-=I;
     Fill_Screen(Rel_Last+I-1,Rel_Last);
    } else
    {Home=My_Line+=I;
     Rel_First-=I;
     Rel_Last-=I;
     Fill_Screen(Rel_First,Rel_Last);
    }
   }
  }
 }
}

void Fast Shift_In()
{
#if defined(VMS) | defined(UNIX)
 Out_Zap();
 putchar(15);
 Shifted_Out=0;
 Repaint();
#endif
#if defined(DOS) & !(defined(Windows) | defined(TV)) & defined(PC100)
 Out_Zap();
 putchar(15);
 Shifted_Out=0;
 Repaint();
#endif
}

void Fast Shift_Out()
{
#if defined(VMS) | defined(UNIX)
 Out_Zap();
 putchar(14);
 Shifted_Out=1;
 Repaint();
#endif
#if defined(DOS) & !(defined(Windows) | defined(TV)) & defined(PC100)
 Out_Zap();
 putchar(14);
 Shifted_Out=1;
 Repaint();
#endif
}

void Fast Font_Larger()
{
#if defined(ATARI) & defined(GEM)
 GEM_Larger();
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Win_Larger();
#endif
}

void Fast Font_Smaller()
{
#if defined(ATARI) & defined(GEM)
 GEM_Smaller();
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Win_Smaller();
#endif
}

void Fast Update_Off()
{Update_Flag=0;
}

void Fast Update_On()
{if (!Update_Flag)
 {Update_Flag=1;
#if !((defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))))
  if (Zoomed)
  {S_Header();
   Fill_Screen(Rel_First,Rel_Last);
   if (Show_Ruler)
    S_Ruler();
  } else
   Refresh_Window();
#else
  S_Header();
  Fill_Screen(Rel_First,Rel_Last);
  if (Show_Ruler)
   S_Ruler();
#endif
 }
}

void Fast Remove_Char()
{Erase_Del(Dl_Pointers[Char_String]);
}

void Fast Remove_Help()
{Erase_Del(Dl_Pointers[Help_String]);
}

void Fast Remove_Line()
{Erase_Del(Dl_Pointers[Line_String]);
}

void Fast Remove_Page()
{Erase_Del(Dl_Pointers[Page_String]);
}

void Fast Remove_Par()
{Erase_Del(Dl_Pointers[Paragraph_String]);
}

void Fast Remove_Paste()
{Erase_Del(Dl_Pointers[Past_String]);
}

void Fast Remove_Search()
{Erase_Del(Dl_Pointers[Search_String]);
}

void Fast Remove_Sent()
{Erase_Del(Dl_Pointers[Sen_String]);
}

void Fast Remove_Subs()
{Erase_Del(Dl_Pointers[Subs_String]);
}

void Fast Remove_Unknown()
{Erase_Del(Dl_Pointers[Unknown_String]);
}

void Fast Remove_Word()
{Erase_Del(Dl_Pointers[Word_String]);
}

void Fast Set_Block()
{Blk_Mode=Count;
 Hdr_Changes=1;
}

void Fast Set_Replace()
{Replace_Mode=Count;
 Hdr_Changes=1;
}

void Fast Set_Direction()
{Com_Direction=Def_Direction=Count;
 Hdr_Changes=1;
}

void Fast Set_Tab_Expand()
{Tab_Expand=Count;
 Hdr_Changes=1;
}

void Fast Set_Case_Sensitive()
{Case_Sensitive=Count;
 Hdr_Changes=1;
}

void Fast Set_Reg_Search()
{Reg_Search=Count;
 Hdr_Changes=1;
}

void Fast Set_Ins_Defs()
{Ins_Defs=Count;
 Hdr_Changes=1;
}

void Fast Set_Cursor_Free()
{Cursor_Free=Count;
 Hdr_Changes=1;
}

void Fast Counter_Set()
{if (!Get_Counter_Index()||Skip)
  return;
 if (Direction)
  Counter[Counter_Index]=Count; else
  Counter[Counter_Index]= -Count;
}

void Fast Counter_Put()
{char *P, C;
 if (!Get_Counter_Index()||Skip)
  return;
#if defined(HPUX)
 strcpy(Dsk_Rec,ltoa(Counter[Counter_Index]));
#else
 ltoa(Counter[Counter_Index],Dsk_Rec,10);
#endif
 P=Dsk_Rec;
 while ((C= *(P++))!=0)
  Ins_Char(C);
 Set_Changes();
}

void Fast Counter_Increment()
{if (!Get_Counter_Index()||Skip)
  return;
 if (Direction)
 Counter[Counter_Index]+=Count; else
 Counter[Counter_Index]-=Count;
}

void Fast Counter_Absolute_Value()  /* 290a */
{
if (!Get_Counter_Index()||Skip)
  return;
 if (Counter[Counter_Index]<0)
  Counter[Counter_Index]=Counter[Counter_Index]*(-1); 
}

void Fast End_Learn()
{if (Learning)
 {Learning=0;
  Hdr_Changes=1;
#if defined(VMS) | defined(UNIX)
  if (Gotmouse)	/* 290a */
   Enable_Mouse();
#endif
  Upd_Header();
  Out_Zap();
 }
}

void Fast Context_Save()
{if (Outfile==NULL||(Outfile)[0]==0)
  Error(2,No_File_Txt); else
  Save_The_Context(Outfile,1);
}

void Fast Save_The_Context(char *N,int Option)
{struct File F;
 if (strlen(N)==0)
  Error(2,No_Contxt_Name_Txt); else
 {Filnam(N,Dsk_Rec);
#if defined(UNIX)
  strcat(Dsk_Rec,".ctx");
#else
  strcat(Dsk_Rec,".CTX");
#endif
  if (Option==2)
  {if (F_open(Dsk_Rec,"r",&F)==NULL)
    return;
   if (Err_Flag)
    return;
   F_close(&F);
  }
  if (F_open(Dsk_Rec,"w",&F)==NULL)
   Plain_Message(Cannot_Create_Context_Txt); else
  {if (Err_Flag)
    return;
   Filjnam(N,Dsk_Rec);
#if ! (defined(UNIX) | defined(MPE))
   strcat(Dsk_Rec,"\r\n");
#else
   strcat(Dsk_Rec,"\n");
#endif
   F_write(Dsk_Rec,1,strlen(Dsk_Rec),&F);
   C_Write(My_Pos,&F);
   {register int I;
    for (I=0;I<=Maxpoints;I++)
    {C_Write(Points[I],&F);
     C_Write(End_Points[I],&F);
    }
   }
   C_Write(Indent,&F);
   C_Write(Word_Wrap,&F);
   C_Write(Left_Margin,&F);
   C_Write(Right_Margin,&F);
   C_Write((long)Justify,&F);
   C_Write((long)Auto_Margin,&F);
   {register int I;
    for (I=0;I<512;I++)
     C_Write((long)Tabs[I],&F);
   }
   F_close(&F);
  }
 }
}

void Fast Delete_The_Context(char *N)
{Filnam(N,Dsk_Rec);
#if defined(UNIX)
 strcat(Dsk_Rec,".ctx");
#else
 strcat(Dsk_Rec,".CTX");
#endif
#if defined(VMS)
 delete(Dsk_Rec);
#else
 unlink(Dsk_Rec);
#endif
}

void Fast C_Write(long N,struct File *F)
{
#if defined(HPUX)
 strcpy(Dsk_Rec,ltoa(N));
#else
 ltoa(N,Dsk_Rec,10);
#endif
#if ! (defined(UNIX) | defined(MPE))
 strcat(Dsk_Rec,"\r\n");
#else
 strcat(Dsk_Rec,"\n");
#endif
 F_write(Dsk_Rec,1,strlen(Dsk_Rec),F);
}

void Fast Check_Point()
{if(!Executing&&Outfile!=0&&(Any_Changes[Cur_Buffer]&1)&&
  !Read_Only[Cur_Buffer])
 {Save_Auto(Outfile);
  if (!Err_Flag)
   Auto_Saved++;
  Auto_Count=0;
 }
}

void Fast Push_Changes()
{Any_Changes[Cur_Buffer]<<=1;
 Hdr_Changes=1;
}

void Fast Pop_Changes()
{Any_Changes[Cur_Buffer]>>=1;
 Hdr_Changes=1;
}

void Fast Show_Bcast()
{
#if defined(VMS)
 if (Mbx_Count>0)
 {Plain_Message(Mbx_Mess[0]);
  Err_Display=0;
  Mbx_Count--;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move(Mbx_Mess[1],Mbx_Mess[0],99*1024);
#else
  C_Move(Mbx_Mess[1],Mbx_Mess[0],99*1024,1);
#endif
 } else
  Error(3,No_Messages_Txt);
#endif
}

void Fast Window_Used()
{Sav_Window(Cur_Buffer);
 if (Direction)
 {register int I;
  for (I=Cur_Buffer+1;I!=Cur_Buffer;I++)
  {if (I==Maxbuffers)
   {I=0;
    if (I==Cur_Buffer)
     break;
   }
   if (Dl_Pointers[Pointers+I][0]!=0)
   {Ch_Window(I);
    return;
   }
  }
 } else
 {register int I;
  for (I=Cur_Buffer-1;I!=Cur_Buffer;I--)
  {if (I==-1)
   {I=Maxbuffers-1;
    if (I==Cur_Buffer)
     break;
   }
   if (Dl_Pointers[Pointers+I][0]!=0)
   {Load_Window(I);
    if (Main_Buffer.Bufsize!=0||Outfile!=NULL)
    {Load_Window(Cur_Buffer);
     Ch_Window(I);
     return;
    }
   }
  }
 }
 Load_Window(Cur_Buffer);
 Error(3,No_Buffer_Txt);
}

void Fast Window_Changed()
{Sav_Window(Cur_Buffer);
 if (Direction)
 {register int I;
  for (I=Cur_Buffer+1;I!=Cur_Buffer;I++)
  {if (I==Maxbuffers)
   {I=0;
    if (I==Cur_Buffer)
     break;
   }
   if (Dl_Pointers[Pointers+I][0]!=0)
   {/*Load_Window(I);*/
    if (Any_Changes[I])	 	/* 290a */ 
    {/*Load_Window(Cur_Buffer);*/
     Ch_Window(I);
     return;
    }
   }
  }
 } else
 {register int I;
  for (I=Cur_Buffer-1;I!=Cur_Buffer;I--)
  {if (I==-1)
   {I=Maxbuffers-1;
    if (I==Cur_Buffer)
     break;
   }
   if (Dl_Pointers[Pointers+I][0]!=0)
   {/*Load_Window(I);*/
    if (Any_Changes[I]) 	/* 290a */ 
    {/*Load_Window(Cur_Buffer);*/
     Ch_Window(I);
     return;
    }
   }
  }
 }
 Load_Window(Cur_Buffer);
 Error(3,No_Buffer_Txt);
}

void Fast Window_Not_Used()
{if (Direction)
 {register int I;
  for (I=Cur_Buffer+1;I!=Cur_Buffer;I++)
  {if (I==Maxbuffers)
   {I=0;
    if (I==Cur_Buffer)
     break;
   }
   if (Dl_Pointers[Pointers+I][0]==0)
   {Ch_Window(I);
    return;
   }
  }
 } else
 {register int I;
  for (I=Cur_Buffer-1;I!=Cur_Buffer;I--)
  {if (I==-1)
   {I=Maxbuffers-1;
    if (I==Cur_Buffer)
     break;
   }
   if (Dl_Pointers[Pointers+I][0]==0)
   {Ch_Window(I);
    return;
   }
  }
 }
 Error(3,No_Buffer_Txt);
}

void Fast Window_Cycle()
{int I;
 if (Direction)
 {I=Cur_Buffer+1;
  if (I==Maxbuffers)
   I=0;
 } else
 {I=Cur_Buffer-1;
  if (I==-1)
   I=Maxbuffers-1;
 }
 Ch_Window(I);
 return;
}

/*      Define the contents of the key given in the first command argument as
        the value of the command.
*/
void Fast Redefine_Key()
{Gold=Function=0;
 Get_Key();
 if (End_Com())
 {Com_Error();
  return;
 }
 if (Skip)
  return;
 if ((Function=Map_Tab[Map_Index].F_Code)<=0||Function<=Top_Func)
 {if (Next_Func==Maxfunc-1)
  {Error(2,Max_Func_Txt);
   return;
  } else
   Map_Tab[Map_Index].F_Code=Function= ++Next_Func;
 }
 Set_Key(Key_Addr(Function),(int)Count);
}

#if defined(VMS) | defined(UNIX)
/*      Display the current column and the ASCII code 
      	on the bottom line of the display.
*/
void Fast Show_Ascii()
{Set_Region();
 if (My_Pos>=End_Region)
 {Message(Show_Column_Txt,My_Col);  	/* 290a */ 
 }else
 {B_Goto(Current,My_Pos);
  Message(Col_Ascii_Value_Txt,My_Col,B_Get(Current));  	/* 290a */ 
 }
}
#else
/*      Display the ASCII code on the bottom line of the display.
*/
void Fast Show_Ascii()
{Set_Region();
 if (My_Pos>=End_Region)
 {Error(2,End_Region_Txt);
  return;
 }
 B_Goto(Current,My_Pos);
 Message(Ascii_Value_Txt,B_Get(Current));
}
#endif

void Fast W_D_Border()
{int Save_Line, Save_Att;
 long Save_Col, Save_Shift;
 register int I;
 if (Zoomed)
  return;
 Out_Zap();
 Save_Line=My_Line;
 Save_Col=My_Col;
 Save_Att=Cur_Attributes;
 Save_Shift=Scr_Shift;
 Scr_Shift=0;
 Top_Row--;
 Left_Col--;
 Scr_Length+=2;
 Scr_Width+=2;
 Cur_Attributes=A_Bold+A_Reverse;
 if (Top_Row>=0)
 {Opt_Cursor(1,(long)1);
  if (Left_Col>=0)
   Opt_Char((char)D_Border[0]); else
   My_Col++;
  for (I=2;I<=Scr_Width-1;I++)
   Opt_Char((char)D_Border[1]);
  if (Left_Col+Scr_Width<=Max_Width)
   Opt_Char((char)D_Border[2]); else
   My_Col++;
 }
 if (Left_Col+Scr_Width<=Max_Width)
#if defined(GEM) | defined(Windows) | defined(TV)
  for (I=2;I<=Scr_Length-2;I++)
#else
  for (I=2;I<=Scr_Length-1;I++) 	/* 290a */ 
#endif
  {Opt_Cursor(I,(long)Scr_Width);
   Opt_Char((char)D_Border[3]);
  }
#if defined(GEM) | defined(Windows) | defined(TV)
 if (Top_Row+Scr_Length<=Max_Length)
 {Opt_Cursor(Scr_Length-1,(long)Scr_Width);
  if (Left_Col+Scr_Width<=Max_Width)
   Opt_Char((char)D_Border[4]);
  Opt_Cursor(Scr_Length-1,(long)1);
  if (Left_Col>=0)
   Opt_Char((char)D_Border[6]);
  Opt_Cursor(Scr_Length-1,(long)2);
  for (I=2;I<=Scr_Width-1;I++)
   Opt_Char((char)D_Border[5]);
 }
 if (Left_Col>=0)
  for (I=Scr_Length-2;I>1;I--)
#else
 if (Top_Row+Scr_Length<Max_Length-1) 	/* 290a */ 
 {Opt_Cursor(Scr_Length,(long)Scr_Width);
  if (Left_Col+Scr_Width<=Max_Width)
   Opt_Char((char)D_Border[4]);
  Opt_Cursor(Scr_Length,(long)1);
  if (Left_Col>=0)
   Opt_Char((char)D_Border[6]);
  Opt_Cursor(Scr_Length,(long)2);
  for (I=2;I<=Scr_Width-1;I++)
   Opt_Char((char)D_Border[5]);
 }
 if (Left_Col>=0)
  for (I=Scr_Length-1;I>1;I--) 	/* 290a */ 
#endif
  {Opt_Cursor(I,(long)1);
   Opt_Char((char)D_Border[7]);
  }
 if (Top_Row>=0)
 {Opt_Cursor(1,(long)2);
  Put_I((long)Cur_Buffer);
  /*Opt_Char(':');
  Opt_String(Outfile);*/ 	/* 290a */ 
 }
 Out_Zap();
 Top_Row++;
 Left_Col++;
 Scr_Length-=2;
 Scr_Width-=2;
 Cur_Attributes=Save_Att;
 Opt_Cursor(Save_Line,Save_Col);
 Scr_Shift=Save_Shift;
#if defined(VMS) | defined(UNIX) | defined(ATARI)
 His_Line=His_Col= -1;
#endif
}

void Fast W_S_Border()
{int Save_Line, Save_Att;
 long Save_Col, Save_Shift;
 register int I;
 if (Zoomed)
  return;
 Out_Zap();
 Save_Line=My_Line;
 Save_Col=My_Col;
 Save_Att=Cur_Attributes;
 Save_Shift=Scr_Shift;
 Scr_Shift=0;
 Top_Row--;
 Left_Col--;
 Scr_Length+=2;
 Scr_Width+=2;
 Cur_Attributes=0;
 if (Top_Row>=0)
 {Opt_Cursor(1,(long)1);
  if (Left_Col>=0)
   Opt_Char((char)S_Border[0]); else
   My_Col++;
  for (I=2;I<=Scr_Width-1;I++)
   Opt_Char((char)S_Border[1]);
  if (Left_Col+Scr_Width<=Max_Width)
   Opt_Char((char)S_Border[2]); else
   My_Col++;
 }
 if (Left_Col+Scr_Width<=Max_Width)
#if defined(GEM) | defined(Windows) | defined(TV)
  for (I=2;I<=Scr_Length-2;I++)
#else
  for (I=2;I<=Scr_Length-1;I++) 	/* 290a */ 
#endif
  {Opt_Cursor(I,(long)Scr_Width);
   Opt_Char((char)S_Border[3]);
  }
#if defined(GEM) | defined(Windows) | defined(TV)
 if (Top_Row+Scr_Length<=Max_Length)
 {Opt_Cursor(Scr_Length-1,(long)Scr_Width);
  if (Left_Col+Scr_Width<=Max_Width)
   Opt_Char((char)D_Border[4]);
  Opt_Cursor(Scr_Length-1,(long)1);
  if (Left_Col>=0)
   Opt_Char((char)D_Border[6]);
  Opt_Cursor(Scr_Length-1,(long)2);
  for (I=2;I<=Scr_Width-1;I++)
   Opt_Char((char)D_Border[5]);
 }
 if (Left_Col>=0)
  for (I=Scr_Length-2;I>1;I--)
#else
 if (Top_Row+Scr_Length<Max_Length-1) 	/* 290a */ 
 {Opt_Cursor(Scr_Length,(long)Scr_Width);
  if (Left_Col+Scr_Width<=Max_Width)
   Opt_Char((char)S_Border[4]);
  Opt_Cursor(Scr_Length,(long)2);
  for (I=2;I<=Scr_Width-1;I++)
   Opt_Char((char)S_Border[5]);
  Opt_Cursor(Scr_Length,(long)1);
  if (Left_Col>=0)
   Opt_Char((char)S_Border[6]);
 }
 if (Left_Col>=0)
  for (I=Scr_Length-1;I>1;I--) 	/* 290a */ 
#endif
  {Opt_Cursor(I,(long)1);
   Opt_Char((char)S_Border[7]);
  }
 if (Top_Row>=0)
 {Opt_Cursor(1,(long)2);
  Put_I((long)Cur_Buffer);
  if (Top_Row<Scr_Length/2) 	/* 290a */ 
  {Opt_Char(':');
   Opt_String(Outfile);
  }
 }
 Out_Zap();
 Top_Row++;
 Left_Col++;
 Scr_Length-=2;
 Scr_Width-=2;
 Cur_Attributes=Save_Att;
 Opt_Cursor(Save_Line,Save_Col);
 Scr_Shift=Save_Shift;
#if defined(VMS) | defined(UNIX) | defined(ATARI)
 His_Line=His_Col= -1;
#endif
}

void Fast Open_Window()
{Out_Zap();
 W_D_Border();
 Repaint();
}

void Fast Refresh_Window()
{register int I;
 Out_Zap();
 Down_Type=0;
 if (!Zoomed) 	/* 290a */
 {
  Sav_Window(Cur_Buffer);
  Reset_Screen();
  for (I=1;I<Buffers-1;I++)	/* 290a search next zoomed window in stack */
  {Load_Window(Buffer_Stack[I]);
   if (Zoomed) break;
  }
  for (;I>=0;I--)
  {
   Load_Window(Buffer_Stack[I]);
   Cur_Buffer=Buffer_Stack[I];
   if (I==0)
    W_D_Border(); else
    W_S_Border();
   Repaint();
   Mark_Cursor();
   Out_Zap();
  }
  Unmark_Cursor();
 } else
 Repaint();
}

void Fast Window_Grow()
{
 int Done;
 if (Zoomed)
 {Error(2,Txt_Zoomed_Error);
  return;
 }
#if defined(DOS) | defined(NT) | defined(OS2) | (defined(ATARI) & defined(GEM)) | defined(WIN32)
 Mouse_OK=Done=0;
#else
 Done=0;
#endif
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#endif
 while (!Done)
 {Out_Zap();
  Get16();
  if (Gold)
   Error(2,Illegal_Keystroke_Txt); else
  switch (Key_Number)
  {case 27: /*Up Arrow*/
    if (Top_Row>0)
    {Top_Row--;
     Scr_Length++;
     Open_Window();
    }
    break;
   case 28: /*Left Arrow*/
    if (Left_Col>0)
    {Left_Col--;
     Scr_Width++;
     Open_Window();
    }
    break;
   case 29: /*Down Arrow*/
#if defined(GEM) | defined(Windows) | defined(TV)
    if (Top_Row+Scr_Length<Max_Length)
#else
    if (Top_Row+Scr_Length<Max_Length-2) 	/* 290a */ 
#endif
    {Scr_Length++;
     Open_Window();
    }
    break;
   case 30: /*Right Arrow*/
    if (Left_Col+Scr_Width<Max_Width)
    {Scr_Width++;
     Open_Window();
    }
    break;
   case 48:
   case 50: /*Return,Enter*/
    Refresh_Window();
    Done=1;
    break;
   default:
    Error(2,Illegal_Keystroke_Txt);
    break;
  }
 }
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Enable_Mouse();
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | (defined(ATARI) & defined(GEM)) | defined(WIN32)
 Mouse_OK=1;
#endif
}

void Fast Window_Shrink()
{
 int Done;
 if (Zoomed)
 {Error(2,Txt_Zoomed_Error);
  return;
 }
#if defined(DOS) | defined(NT) | defined(OS2) | (defined(ATARI) & defined(GEM)) | defined(WIN32)
 Mouse_OK=Done=0;
#else
 Done=0;
#endif
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#endif
 while (!Done)
 {Out_Zap();
  Get16();
  if (Gold)
   Error(2,Illegal_Keystroke_Txt); else
  switch (Key_Number)
  {case 27: /*Up Arrow*/
    if (Scr_Length>4)
    {Scr_Length--;
     Open_Window();
    }
    break;
   case 28: /*Left Arrow*/
    if (Scr_Width>1)
    {Scr_Width--;
     Open_Window();
    }
    break;
   case 29: /*Down Arrow*/
    if (Scr_Length>4)
    {Top_Row++;
     Scr_Length--;
     Open_Window();
    }
    break;
   case 30: /*Right Arrow*/
    if (Scr_Width>1)
    {Left_Col++;
     Scr_Width--;
     Open_Window();
    }
    break;
   case 48:
   case 50: /*Return,Enter*/
    Refresh_Window();
    Done=1;
    break;
   default:
    Error(2,Illegal_Keystroke_Txt);
    break;
  }
 }
#if defined(DOS) | defined(NT) | defined(OS2) | (defined(ATARI) & defined(GEM)) | defined(WIN32)
 Mouse_OK=1;
#endif
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Enable_Mouse();
#endif
}

void Fast Window_Move()
{
 int Done;
 if (Zoomed)
 {Error(2,Txt_Zoomed_Error);
  return;
 }
#if defined(DOS) | defined(NT) | defined(OS2) | (defined(ATARI) & defined(GEM)) | defined(WIN32)
 Mouse_OK=Done=0;
#else
 Done=0;
#endif
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Disable_Mouse();
#endif
 while (!Done)
 {Out_Zap();
  Get16();
  if (Gold)
   Error(2,Illegal_Keystroke_Txt); else
  switch (Key_Number)
  {case 27: /*Up Arrow*/
    if (Top_Row>0)
    {Top_Row--;
     Open_Window();
    }
    break;
   case 28: /*Left Arrow*/
    if (Left_Col>0)
    {Left_Col--;
     Open_Window();
    }
    break;
   case 29: /*Down Arrow*/
#if defined(GEM) | defined(Windows) | defined(TV)
    if (Top_Row+Scr_Length<Max_Length)
#else
    if (Top_Row+Scr_Length<Max_Length-2) 	/* 290a */ 
#endif
    {Top_Row++;
     Open_Window();
    }
    break;
   case 30: /*Right Arrow*/
    if (Left_Col+Scr_Width<Max_Width)
    {Left_Col++;
     Open_Window();
    }
    break;
   case 48:
   case 50: /*Return,Enter*/
    Refresh_Window();
    Done=1;
    break;
   default:
    Error(2,Illegal_Keystroke_Txt);
    break;
  }
 }
#if defined(DOS) | defined(NT) | defined(OS2) | (defined(ATARI) & defined(GEM)) | defined(WIN32)
 Mouse_OK=1;
#endif
#if defined(VMS) | defined(UNIX)
 if (Gotmouse)	/* 290a */
  Enable_Mouse();
#endif
}

void Fast Window_Zoom()
{
#if !(((defined(DOS) | defined(NT)) & ((defined(Windows) | defined(TV)) | defined(TV))) | (defined(ATARI) & defined(GEM)))
 if ((Count==2&&Zoomed)||(Count==3&&!Zoomed))
  ; else
 if (Zoomed)
 {Zoomed=0;
  Top_Row=Save_Top;
  Left_Col=Save_Left;
  Scr_Length=Save_Length;
  Scr_Width=Save_Width;
  Refresh_Window();
 } else
 {Zoomed=1;
  Save_Top=Top_Row;
  Save_Left=Left_Col;
  Save_Length=Scr_Length;
  Save_Width=Scr_Width;
  Top_Row=0;
  Left_Col=0;
#if defined(GEM) | defined(Windows) | defined(TV)
  Scr_Length=Max_Length;
#else
  Scr_Length=Max_Length-2;
#endif
  Scr_Width=Max_Width;
  Open_Window();
 }
 Set_Cursor(My_Line,My_Col);
#endif
#if defined(DOS) & defined(TV)
 TV_Zoom();
#endif
}

void Fast Window_Tile()
{
#if !(((defined(DOS) | defined(NT)) & ((defined(Windows) | defined(TV)) | defined(TV))) | (defined(ATARI) & defined(GEM)))
 int Across, Down, W, I;
 if (Buffers==1)
  Across=Down=W=1; else
 if (Buffers==2)
 {Across=1;
  Down=2;
  W=2;
 } else
 if (Buffers<=4)
 {Across=2;
  Down=2;
  W=4;
 } else
 if (Buffers<=6)
 {Across=2;
  Down=3;
  W=6;
 } else
 if (Buffers<=9)
 {Across=3;
  Down=3;
  W=9;
 } else
 if (Buffers<=12)
 {Across=3;
  Down=4;
  W=12;
 } else
 {Across=4;
  Down=4;
  W=16;
 }
 if (W>Buffers)
  W=Buffers;
 /*Diag("Window_Tile:Across=%d,Down=%d,Buffers=%d,W=%d",
  Across,Down,Buffers,W);*/
 Sav_Window(Cur_Buffer);
 for (I=W-1;I>=0;I--)
 {Load_Window(Buffer_Stack[I]);
  Top_Row=((Max_Length-2)/Down)*(I/Across)+1;
  Left_Col=(Max_Width/Across)*(I%Across)+1;
  Scr_Length=((Max_Length-3)/Down)-1;
  Scr_Width=(Max_Width/Across)-2;
  Zoomed=0;
  Sav_Window(Buffer_Stack[I]);
 }
 Load_Window(Cur_Buffer);
 Refresh_Window();
#endif
#if defined(DOS) & defined(TV)
 TV_Tile();
#endif
}

void Fast Window_Cascade()
{
#if defined(DOS) & defined(TV)
 TV_Cascade();
#endif
}

void Fast Window_Fix_Bottom()
{int Smaller;
 if (Direction==0)
#if defined(GEM) | defined(Windows) | defined(TV)
  Count=Max_Length/2;
 if (Count<Top_Row+4||Count>Max_Length+1)
  Error(2,Bad_Window_Pos_Txt); else
 {Zoomed=0;
  Smaller=Count<Top_Row+Scr_Length+1;
#else
  Count=Max_Length/2-2;	/* 290a */
 if (Count<Top_Row+4||Count>=Max_Length-1)	/* 290a */
  Error(2,Bad_Window_Pos_Txt); else
 {Zoomed=0;
  Smaller=Count<=Top_Row+Scr_Length;	/* 290a */
#endif
  Scr_Length=Count-Top_Row;
  if (Smaller)
   Refresh_Window(); else
   Repaint();
 }
}

void Fast Window_Fix_Left()
{int Smaller;
 if (Direction==0)
  Count=Max_Width/2;
 if (Count<0||Count>Left_Col+Scr_Width)
  Error(2,Bad_Window_Pos_Txt); else
 {Zoomed=0;
  Smaller=Count>Left_Col;
  Scr_Width-=Count-Left_Col;
  Left_Col=Count;
  if (Smaller)
   Refresh_Window(); else
   Repaint();
 }
}

void Fast Window_Fix_Right()
{int Smaller;
 if (Direction==0)
  Count=Max_Width/2;
 if (Count<Left_Col+4||Count>Max_Width+1)
  Error(2,Bad_Window_Pos_Txt); else
 {Zoomed=0;
  Smaller=Count<Left_Col+Scr_Width+1;
  Scr_Width=Count-Left_Col-1;
  if (Smaller)
   Refresh_Window(); else
   Repaint();
 }
}

void Fast Window_Fix_Top()
{int Smaller;
 if (Direction==0)
#if defined(GEM) | defined(Windows) | defined(TV)
  Count=Max_Length/2;
#else
  Count=Max_Length/2-1;	/* 290a */
#endif
 if (Count<0||Count>Top_Row+Scr_Length)
  Error(2,Bad_Window_Pos_Txt); else
 {Zoomed=0;
  Smaller=Count>Top_Row;
  Scr_Length-=Count-Top_Row;
  Top_Row=Count;
  if (Smaller)
   Refresh_Window(); else
   Repaint();
 }
}
