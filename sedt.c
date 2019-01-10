/*Copyright (c) Anker Berg-Sonne, 1985-1990*/
#include "proto.h"

#if defined(VMS)
char *Find_Dir(), *Find_Lib();
#endif

#if defined(VMS) | defined(UNIX) | defined(OS2)
#include <signal.h>
#endif

#if defined(VMS) | defined(OS2) | (defined(UNIX) & defined(TERMCAP) & !defined(XENIX))
void CCint()
{if (CC_Enable)
 {CC_Flag=1;
  Out_Zap();
  signal(SIGINT,CCint);
 }
}
#endif

#if (defined(UNIX) & defined(TERMCAP) & !(defined(XENIX) | defined(HPUX)))
void STPint();

void CONTint()
{USedtenv();
 Setup_Term();
 if (Org_Width>80)
 {if (Scr_Width == 80)
   Narrow_Screen();
 } else
 if (Org_Width == 80)
 {if (Scr_Width>80)
   Wide_Screen();
 }
 Refresh();
 signal(SIGTSTP,STPint);
}

void STPint()
{signal(SIGTSTP,SIG_DFL);
 kill(getpid(),SIGTSTP);
 Set_Scroll(1,Scr_Length);
 Reset_Width();
 Out_Zap();
 USysenv();
 signal(SIGCONT,CONTint);
}
#endif

#if !(defined(Windows) | defined(GEM) | defined(TV))
/*      Main routime
	Setup all variables and then call the main editor routine
	Upon exit from the main editir routine close down in an
	orderly fashion and exit.
*/
int main(int Argc,char * *Argv)
{
#if defined(DOS) & defined(__LARGE__)
#if defined(__BORLANDC__)
 /* EMS overlay memory for over lays, I don't think this is same as GET_Buffering, I think this is for old style program overlays */
#else
 _OvrInitEms(0,0,0);
#endif
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
 Fildir(Argv[0],Run_From);
#endif
#if defined(DOS) & !defined(Windows)
 Set_Crit();
#endif
 Sedt_Args(Argc,Argv);
 Sedt_Init();
 Readruler('0');
 Err_Flag=0;
 Sedt_Files(Argc,Argv);
 if (!(File_Arguments&&Outfile==NULL))
 {if (Remember&&Outfile!=NULL)
	Write_Rem();
  Sedt_Main();
 } else
 {Sedt_Cleanup();
  Sedt_Shutdown();
 }
#if defined(DOS) & !defined(Windows)
 Reset_Crit();
#endif
#if defined(UNIX) | defined(VMS)
 _exit(Return_Value);
#endif
#if defined(DOS) & !(defined(Windows) | defined(TV))
#if defined(DESQVIEW)
 if (DV)
 {ptr_close(Ptr);
  ptr_free(Ptr);
  api_exit();
 }
#endif
 _exit(Return_Value);
#endif
#if defined(ATARI) | defined(MPE)
 _exit(Return_Value);
#endif
#if defined(OS2)
 DosExit(1,Return_Value);
#endif
 return Return_Value;
}
#endif

#if defined(TV)
void Fast Run_Sedt(int Argc,char *Argv[])
{
#if defined(DOS) & defined(__LARGE__)
 /*_OvrInitEms(0,0,0);*/
#endif
 Fildir(Argv[0],Run_From);
 Set_Crit();
 Sedt_Args(Argc,Argv);
 Sedt_Init();
 Readruler('0');
 Err_Flag=0;
 Sedt_Files(Argc,Argv);
 if (!(File_Arguments&&Outfile==NULL))
 {if (Remember&&Outfile!=NULL)
	Write_Rem();
  Sedt_Main();
 } else
 {Sedt_Cleanup();
  Sedt_Shutdown();
 }
 Reset_Crit();
}
#endif

void Fast Write_Rem()
{struct File F;
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(WIN32)
 if (
  (Find_Env("sedt.rem",Dsk_Rec,"SEDTP")!=NULL&&F_open(Dsk_Rec,"w",&F)!=NULL)||
  (Find_Env("sedt.rem",Dsk_Rec,"SEDT")!=NULL&&F_open(Dsk_Rec,"w",&F)!=NULL)||
#if defined(UNIX)
  (Find_Env("sedt.rem",Dsk_Rec,"HOME")!=NULL&&F_open(Dsk_Rec,"w",&F)!=NULL)||
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
  (Find_From("sedt.rem",Dsk_Rec)!=NULL&&F_open(Dsk_Rec,"w",&F)!=NULL)||
#endif
  (Find_Path("sedt.rem",Dsk_Rec)!=NULL&&F_open(Dsk_Rec,"w",&F)!=NULL)||
  F_open("sedt.rem","w",&F)!=NULL)
#endif
#if defined(VMS)
 if (F_open(Find_Dir("sedt.rem",Dsk_Rec),"w",&F)||
  F_open(Find_Lib("sedt.rem",Dsk_Rec),"w",&F)||
  F_open("sedt.rem","w",&F)!=NULL)
#endif
#if defined(MPE)
 if (F_open("rem.sed","w",&F)!=NULL)
#endif
 {F_write(Outfile,1,strlen(Outfile),&F);
  F_close(&F);
 }
}

void Fast Sedt_Init()
{char S[80];
 Cur_Buffer=0;
 Current= &Main_Buffer;
#if defined(VMS)
 T$Init();
#endif
#if defined(UNIX) & defined(TERMCAP)
 UInit();
#endif
 Init_Buf();
 Read_Config();
#if defined(DOS) & defined(TV)
 if (EGA43)
  TV_Ega43();
#endif
 if (Mapping== -1)
  Mapping=EDT_Mapping;
 Init_Buffer();
 B_Init(&Pad);
 B_Init(&Com);
 B_Init(&Del);
 Set_Terminal();
 Setup_Term();
 Opt_Init();
#if !((defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))))
 Newwindow();
#endif
 S_Header();
 {register int I;
  for (I=1;I<=512;I++)
   Tabs[I]=I%8==1;
 }
 Rel_First=Show_Ruler?3:2;
#if defined(GEM) | defined(Windows) | defined(TV)
 Rel_Last=Scr_Length-2;
#else
 Rel_Last=Scr_Length; 	/* 290a */ 
#endif
 Home=(Rel_First+Rel_Last+1)/2;
 Rel_First-=Home;
 Rel_Last-=Home;
#if defined(GEM) | defined(Windows) | defined(TV)
 Rel_Last=Scr_Length-2-Home;
#else
 Rel_Last=Scr_Length-Home;	/* 290a */ 
#endif
 Cur_Attributes=0;
 Opt_Cursor(Home,(long)1);
 Opt_String(Version_Txt);
 Opt_String(Version);
#if Usemem==0
 Opt_String(Small_Txt);
#endif
#if !(defined(ATARI) | defined(MPE))
 Opt_Cursor(Home+1,(long)1);
 Opt_String(Device_Txt);
 Opt_String(Term_Names[Term_Type]);
#if (defined(DOS) & !defined(PC100)) | defined(NT) | defined(WIN32)
 Opt_String(Keyboard_Txt);
 if (Keyb_Type<=Keyb_SLT)
  Opt_String(Keyb_Names[Keyb_Type]); else
  Opt_String(Keyb_Name);
 Opt_String(Display_Txt);
 Opt_String(Color?Color_Txt:Mono_Txt);
#endif
#if defined(OS2)
 Opt_String(Keyboard_Txt);
  if (Keyb_Type<=Keyb_SLT)
   Opt_String(Keyb_Names[Keyb_Type]); else
   Opt_String(Keyb_Name);
 Opt_String(Display_Txt);
 Opt_String(Color?Color_Txt:Mono_Txt);
#endif
 Opt_Cursor(Home+2,(long)1);
#else
 Opt_Cursor(Home+1,(long)1);
#endif
 Opt_String(License_Txt);
 Pos_Cursor();
 Opt_Cursor(Home,(long)1);
 My_Col=1;
#if defined(VMS)
 if (!Readmaps(Logical_File(Mapfile()))&&	/* 290a */
  !Readmaps((char *) Mapfile())&&
  !Readmaps(Find_Dir(Mapfile(),S))&&!Readmaps(Find_Lib(Mapfile(),S)))
#endif
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(WIN32)
 if (!(Readmaps(Mapfile()))&&
  !(Find_Env((char *) Mapfile(),S,"SEDTP")!=NULL&&Readmaps(S))&&
  !(Find_Env((char *) Mapfile(),S,"SEDT")!=NULL&&Readmaps(S))&&
#if defined(UNIX)
  !(Find_Env((char *) Mapfile(),S,"HOME")!=NULL&&Readmaps(S))&&
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
  !(Find_From((char *) Mapfile(),S)!=NULL&&Readmaps(S))&&
#endif
  !(Find_Path((char *) Mapfile(),S)!=NULL&&Readmaps(S)))
#endif
#if defined(MPE)
 if (!Readmaps(Mapfile()))
#endif
 {Term_Type=Default;
#if !(defined(VMS) | defined(UNIX) | defined(ATARI) | defined(MPE))
  Keyb_Type=Keyb_Default;
#endif
#if defined(VMS)
  if (!Readmaps(Logical_File(Mapfile()))&&	/* 290a */
   !Readmaps((char *) Mapfile())&&
   !Readmaps(Find_Dir(Mapfile(),S))&&!Readmaps(Find_Lib(Mapfile(),S)))
#endif
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(WIN32)
  if (!(Readmaps(Mapfile()))&&
   !(Find_Env((char *) Mapfile(),S,"SEDTP")!=NULL&&Readmaps(S))&&
   !(Find_Env((char *) Mapfile(),S,"SEDT")!=NULL&&Readmaps(S))&&
#if defined(UNIX)
   !(Find_Env((char *) Mapfile(),S,"HOME")!=NULL&&Readmaps(S))&&
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
   !(Find_From((char *) Mapfile(),S)!=NULL&&Readmaps(S))&&
#endif
   !(Find_Path((char *) Mapfile(),S)!=NULL&&Readmaps(S)))
#endif
  {Sedt_Shutdown();
   Fatalerror(No_Map_Txt,(char *) Mapfile());
#if defined(OS2)
   DosExit(1,1);
#else
#if defined(DOS) & !defined(Windows)
   Reset_Crit();
#if defined(DESQVIEW)
   if (DV)
    api_exit();
#endif
#endif
#if defined(DOS) & defined(TV)
   TV_Exit(1);
#endif
   _exit(1);
#endif
  }
 }
#if defined(VMS)
 if (!Readkeys(Logical_File(Keyfile()))&&	/* 290a */
  !Readkeys((char *) Keyfile())&&
  !Readkeys(Find_Dir(Keyfile(),S))&&!Readkeys(Find_Lib(Keyfile(),S)))
#endif
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(WIN32)
 if (!(Readkeys(Keyfile()))&&
  !(Find_Env((char *) Keyfile(),S,"SEDTP")!=NULL&&Readkeys(S))&&
  !(Find_Env((char *) Keyfile(),S,"SEDT")!=NULL&&Readkeys(S))&&
#if defined(UNIX)
  !(Find_Env((char *) Keyfile(),S,"HOME")!=NULL&&Readkeys(S))&&
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
  !(Find_From((char *) Keyfile(),S)!=NULL&&Readkeys(S))&&
#endif
  !(Find_Path((char *) Keyfile(),S)!=NULL&&Readkeys(S)))
#endif
#if defined(MPE)
 if (!Readkeys((char *) Keyfile()))
#endif
 {Sedt_Shutdown();
  Fatalerror(No_Key_Txt,(char *) Keyfile());
#if defined(OS2)
  DosExit(1,1);
#else
#if defined(DOS) & !defined(Windows)
  Reset_Crit();
#if defined(DESQVIEW)
  if (DV)
   api_exit();
#endif
#endif
#if defined(DOS) & defined(TV)
  TV_Exit(1);
#endif
  _exit(1);
#endif
 }
#if defined(UNIX)
 My_UID=getuid();
 My_GID=getgid();
#endif
}

void Fast Read_Config()
{struct File F;
 char *S, *T;
#if defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(WIN32)
 if (F_open("sedt.cnf","r",&F)!=NULL||
  (Find_Env("sedt.cnf",Dsk_Rec,"SEDTP")!=NULL&&F_open(Dsk_Rec,"r",&F)!=NULL)||
  (Find_Env("sedt.cnf",Dsk_Rec,"SEDT")!=NULL&&F_open(Dsk_Rec,"r",&F)!=NULL)||
#if defined(UNIX)
  (Find_Env("sedt.cnf",Dsk_Rec,"HOME")!=NULL&&F_open(Dsk_Rec,"r",&F)!=NULL)||
#endif
#if defined(DOS) | defined(NT) | defined(WIN32)
  (Find_From("sedt.cnf",Dsk_Rec)!=NULL&&F_open(Dsk_Rec,"r",&F)!=NULL)||
#endif
  (Find_Path("sedt.cnf",Dsk_Rec)!=NULL&&F_open(Dsk_Rec,"r",&F)!=NULL))
#endif
#if defined(VMS)
 if (F_open("Sedt$Cnf","r",&F)!=NULL||	/* 290a */
  F_open("Sedt.Cnf","r",&F)!=NULL||
  F_open(Find_Dir("sedt.cnf",Dsk_Rec),"r",&F)!=NULL||
  F_open(Find_Lib("Sedt.cnf",Dsk_Rec),"r",&F)!=NULL)
#endif
#if defined(MPE)
 if (F_open("cnf.sed","r",&F)!=NULL)
#endif
 {while (F_gets(Dsk_Rec,Blksize,&F)!=NULL)
  if (strlen(Dsk_Rec)!=0)
  {if (Dsk_Rec[0]!=';')
   {register char C;
    S=Dsk_Rec;
    while (isalnum(C=(*S)))
    {if (islower(C))
      (*S)=toupper(C);
     S++;
    }
    if (C!='=')
     Cnf_Error(); else
    {(*S)=0;
     T=(++S);
     {register char C;
      while ((C=(*S))!=0)
      {if (islower(C))
        (*S)=toupper(C);
       S++;
      }
     }
     S=Dsk_Rec;
     Test_Config(S,T);
    }
   }
  }
  F_close(&F);
 }
}

void Fast Test_Config(char *S,char *T)
{if (strcmp(S,Txt_Screen)==0)
 {if (Call_Color==0)
  {if (strcmp(T,Txt_Color)==0)
    Call_Color=2; else
   if (strcmp(T,Txt_Mono)==0)
    Call_Color=1; else
   Cnf_Error();
  }
 } else
 if (strcmp(S,Txt_Keyboard)==0)
  Test_Keyboard(T); else
 if (strcmp(S,Txt_System)==0||strcmp(S,Txt_Terminal)==0)
  Test_System(T); else
 if (strcmp(S,Txt_Cursorline)==0)
 {if (strcmp(T,Txt_Fixed)==0)
   Float_Cursor=0; else
  if (strcmp(T,Txt_Floating)==0)
   Float_Cursor=1; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Cursor)==0)
 {if (strcmp(T,Txt_Free)==0)
   Cursor_Free=1; else
  if (strcmp(T,Txt_Restricted)==0)
   Cursor_Free=0; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Controlz)==0)
 {if (strcmp(T,Txt_On)==0)
   Nosub=0; else
  if (strcmp(T,Txt_Off)==0)
   Nosub=1; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Backupfile)==0)
 {if (strcmp(T,Txt_On)==0)
   Nobak=0; else
  if (strcmp(T,Txt_Off)==0)
   Nobak=1; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Return)==0)
 {if (strcmp(T,Txt_On)==0)
   Doreturn=1; else
  if (strcmp(T,Txt_Off)==0)
   Doreturn=0; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Screenshift)==0)
  Test_Screenshift(T); else
 if (strcmp(S,Txt_Ins_Defs)==0)
 {if (strcmp(T,Txt_On)==0)
   Ins_Defs=1; else
  if (strcmp(T,Txt_Off)==0)
   Ins_Defs=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Fontsize)==0)
 {Def_Font=atoi(T);
 } else
 if (strcmp(S,Txt_Maxscroll)==0)
 {Max_Scroll=atoi(T);
 } else
 if (strcmp(S,Txt_Ega43)==0)
 {if (strcmp(T,Txt_On)==0)
   EGA43=1; else
  if (strcmp(T,Txt_Off)==0)
   EGA43=0; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Palette)==0)
  Test_Palette(T); else
 if (strcmp(S,Txt_Video)==0)
 {if (strcmp(T,Txt_Bios)==0)
   Dir_Video=0; else
  if (strcmp(T,Txt_Direct)==0)
   Dir_Video=1; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Tabs)==0)
 {if (strcmp(T,Txt_Inserted)==0)
   Tab_Expand=0; else
  if (strcmp(T,Txt_Expanded)==0)
   Tab_Expand=1; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Ruler)==0)
 {if (strcmp(T,Txt_Off)==0)
   Show_Ruler=0; else
  if (strcmp(T,Txt_On)==0)
   Show_Ruler=1; else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Mapping)==0)
  Test_Mapping(T); else
 if (strcmp(S,Txt_Numlock)==0)
 {
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  if (strcmp(T,Txt_Set)==0)
   Set_Num_Lock=1; else
  if (strcmp(T,Txt_Cleared)==0)
   Set_Num_Lock=0; else
   Cnf_Error();
#endif
 } else
 if (strcmp(S,Txt_Mode)==0)
 {
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  if (New_Mode==0)
  {register char C;
   while ((C= *(T++))!=0)
   {New_Mode*=16;
    if (C>='0'&&C<='9')
     New_Mode+=C-'0'; else
    if (C>='A'&&C<='F')
     New_Mode+=C-'A'+10; else
    Cnf_Error();
   }
  }
#endif
 } else
 if (strcmp(S,Txt_Context)==0)
 {if (strcmp(T,Txt_Saved)==0)
   Save_Context=1; else
  if (strcmp(T,Txt_Forgotten)==0)
   Save_Context=0; else
  if (strcmp(T,Txt_Check)==0)
   Save_Context=2; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Column)==0)
 {if (strcmp(T,Txt_Displayed)==0)
   Display_Column=1; else
  if (strcmp(T,Txt_Off)==0)
   Display_Column=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Mouse)==0)
 {if (strcmp(T,Txt_Enabled)==0)
   Use_Mouse=1; else
  if (strcmp(T,Txt_Disabled)==0)
   Use_Mouse=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Remember)==0)
 {if (strcmp(T,Txt_On)==0)
   Remember=1; else
  if (strcmp(T,Txt_Off)==0)
   Remember=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Autosave)==0)
  Test_Auto(T); else
 /*if (strcmp(S,Txt_Firmware)==0)
 {
#if defined(DOS) & ! (defined(Windows) | defined(TV))
  if (strcmp(T,Txt_Newint16)==0)
   Firmware=1; else
  if (strcmp(T,Txt_Oldint16)==0)
   Firmware=0; else
   Cnf_Error();
#endif
 } else*/
 if (strcmp(S,Txt_Dialog)==0)
 {
#if ((defined(DOS) | defined(NT)) & defined(Windows)) | (defined(ATARI) & defined(GEM))
  if (strcmp(T,Txt_On)==0)
   Dialog=1; else
  if (strcmp(T,Txt_Off)==0)
   Dialog=0; else
   Cnf_Error();
#endif
 } else
 if (strcmp(S,Txt_Buffering)==0)
 {
#if defined(DOS) & ! defined(Windows)
  if (strcmp(T,Txt_Freemem)==0)
   Mem_Buffering=1; else
  if (strcmp(T,Txt_Nofreemem)==0)
   Mem_Buffering=0; else
  if (strcmp(T,Txt_EMS)==0)
   EMS_Buffering=1; else
  if (strcmp(T,Txt_NoEMS)==0)
   EMS_Buffering=0; else
   Cnf_Error();
#endif
 } else
 if (strcmp(S,Txt_Share)==0)
 {if (strcmp(T,Txt_On)==0)
   Sharemess=1; else
  if (strcmp(T,Txt_Off)==0)
   Sharemess=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Caps_Lock)==0)
 {if (strcmp(T,Txt_Displayed)==0)
   Caps_Lock=1; else
  if (strcmp(T,Txt_Normal)==0)
   Caps_Lock=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Debug)==0)
 {if (strcmp(T,Txt_On)==0)
   Do_Debug=1; else
  if (strcmp(T,Txt_Off)==0)
   Do_Debug=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Windows)==0)
 {if (strcmp(T,Txt_Zoomed)==0)
   Zoom_Windows=1; else
  if (strcmp(T,Txt_Overlapped)==0)
   Zoom_Windows=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Reservation)==0)
 {if (strcmp(T,Txt_On)==0)
   Reservation=1; else
  if (strcmp(T,Txt_Off)==0)
   Reservation=0; else
   Cnf_Error();
 } else
 if (strcmp(S,Txt_Margin)==0)
 {register char C;
  if ((C= *(T++))>='0'&&C<='9')
  {Requested_Margin=C-'0';
   while ((C= *(T++))>='0'&&C<='9')
    Requested_Margin=Requested_Margin*10+C-'0';
  } else
  Cnf_Error();
 } else
 if (strcmp(S,Txt_Searches)==0)
  Test_Searches(T); else
 if (strcmp(S,Txt_Files)==0)
  Test_Files(T); else
 if (strcmp(S,Txt_Journaling)==0)
  ; else
  Cnf_Error();
}

void Fast Test_Files(char *T)
{
 if (strcmp(T,Txt_Literal)==0)
  Reg_Search=0; else
 if (strcmp(T,Txt_Regular)==0)
  Reg_Search=1; else
  Cnf_Error();
}

void Fast Test_Searches(char *T)
{
 if (strcmp(T,Txt_Literal)==0)
  Reg_Search=0; else
 if (strcmp(T,Txt_Regular)==0)
  Reg_Search=1; else
  Cnf_Error();
}


void Fast Test_Screenshift(char *T)
{if (strcmp(T,Txt_On)==0)
  Do_Shift=1; else
 if (strcmp(T,Txt_Off)==0)
  Do_Shift=0; else
 {register char C;
  if ((C= *(T++))>='0'&&C<='9')
  {Shift_Screen=C-'0';
   while ((C= *(T++))>='0'&&C<='9')
    Shift_Screen=Shift_Screen*10+C-'0';
  } else
  Cnf_Error();
 }
}

void Fast Test_Mapping(char *T)
{if (Mapping== -1)
 {if (strcmp(T,Txt_Edt)==0)
   Mapping=EDT_Mapping; else
  if (strcmp(T,Txt_Wps)==0)
   Mapping=WPS_Mapping; else
  {Mapping=Other_Mapping;
#if defined(UNIX)
   {register char *Q, *R, C;
    Q=Map_Ext;
    R=T;
    while ((C= *R++)!=0)
     *(Q++)=tolower(C);
    *Q=0;
   }
#else
   strcpy(Map_Ext,T);
#endif
  }
 }
}

void Fast Test_System(char *T)
{
#if (defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)) & !defined(PC100)
 if (Call_Type==0)
 {if (strcmp(T,Txt_Ibmpc)==0)
   Call_Type=0xFF; else
  if (strcmp(T,Txt_Ibmxt)==0)
   Call_Type=0xFE; else
  if (strcmp(T,Txt_Ibmat)==0)
   Call_Type=0xFC; else
  if (strcmp(T,Txt_Ibmconvertible)==0)
   Call_Type=0xF9; else
  if (strcmp(T,Txt_Vaxmate)==0)
   Call_Type=0xD2; else
  if (strcmp(T,Txt_Ibmat339)==0)
  {Call_Type=0xFC;
   Call_Model=1;
  } else
  if (strcmp(T,Txt_Ibmxt286)==0)
  {Call_Type=0xFC;
   Call_Model=2;
  } else
  if (strcmp(T,Txt_Ps250)==0)
  {Call_Type=0xFC;
   Call_Model=4;
  } else
  if (strcmp(T,Txt_Ps260)==0)
  {Call_Type=0xFC;
   Call_Model=5;
  } else
  if (strcmp(T,Txt_Ibmxtii)==0)
   Call_Type=0xFB; else
  if (strcmp(T,Txt_Ps230)==0)
   Call_Type=0xFA; else
  if (strcmp(T,Txt_Ps280)==0)
   Call_Type=0xF8; else
  if (strcmp(T,Txt_Tandy)==0)
   Call_Type=2; else
  Cnf_Error();
 }
#endif
#if defined(VMS) | defined(UNIX)
 if (Call_Type==0)
 {if (strcmp(T,Txt_Vt100)==0)
   Call_Type=1; else
  if (strcmp(T,Txt_Vt200)==0)
   Call_Type=2; else
  if (strcmp(T,Txt_Vt52)==0)
   Call_Type=3; else
  if (strcmp(T,Txt_Vk100)==0)
   Call_Type=4; else
  if (strcmp(T,Txt_ANSI)==0)
   Call_Type=5; else
  if (strcmp(T,Txt_Vt300)==0)
   Call_Type=6; else
  if (strcmp(T,Txt_tcap)==0)
   Call_Type=7; else
#if defined(UNIX)
  {Call_Type=8;
   strcpy(Defined_Terminal,T);
  }
#else
   Cnf_Error();
#endif
 }
#endif
}

void Fast Test_Keyboard(char *T)
{
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 if (Keyb_Type==0)
 {if (strcmp(T,Txt_Standard)==0)
  {Keyb_Type=Keyb_PC;
  } else
  if (strcmp(T,Txt_LK250)==0)
  {Keyb_Type=Keyb_250;
  } else
  if (strcmp(T,Txt_New)==0)
  {Keyb_Type=Keyb_New;
  } else
  if (strcmp(T,Txt_Enhanced)==0)
  {Keyb_Type=Keyb_New;
  } else
  if (strcmp(T,Txt_Convertible)==0)
  {Keyb_Type=Keyb_Conv;
  } else
  if (strcmp(T,Txt_At)==0)
  {Keyb_Type=Keyb_AT;
  } else
  if (strcmp(T,Txt_SLT)==0)
  {Keyb_Type=Keyb_SLT;
  } else
  {Keyb_Type=Keyb_Special;
   strcpy(Keyb_Name,T);
  }
 }
#endif
}

void Fast Test_Palette(char *T)
{
#if (defined(DOS) & !(defined(Windows) | defined(TV))) | defined(OS2)
 register int I, J;
 I=0;
 {register char C;
  while ((C=(*(T++)))!=0)
  {if (islower(C))
    C=toupper(C);
   if (C>='A'&&C<='F')
    J=10+C-'A'; else
   if (C>='0'&&C<='9')
    J=C-'0'; else
   {Cnf_Error();
    break;
   }
   J=J<<4;
   C=(*(T++));
   if (islower(C))
    C=toupper(C);
   if (C>='A'&&C<='F')
    J+=10+C-'A'; else
   if (C>='0'&&C<='9')
    J+=C-'0'; else
	{Cnf_Error();
	 break;
	}
	Attab[I++]=J;
  }
 }
#endif
}

void Fast Test_Auto(char *T)
{if (strcmp(T,Txt_On)==0)
 {Auto_Save=200;
  Auto_Warn=20;
 } else
 if (strcmp(T,Txt_Off)==0)
 {Auto_Save=0;
 } else
 if (T[0]>='0'&&T[1]<='9')
 {register char C;
  Auto_Save=0;
  Auto_Warn=20;
  while ((C= *(T++))>='0'&&C<='9')
   Auto_Save=Auto_Save*10+C-'0';
  if (C!=0)
  {if (C==',')
   {if ((C= *T)>='0'&&C<='9')
    {Auto_Warn=0;
     while ((C= *(T++))>='0'&&C<='9')
      Auto_Warn=Auto_Warn*10+C-'0';
    } else
     Cnf_Error();
    if (C!=0)
     Cnf_Error();
   } else
    Cnf_Error();
  }
 } else
  Cnf_Error();
}

void Fast Init_Buffer()
{
 B_Init(&Main_Buffer);
#if defined(ATARI) & defined(GEM)
 GEM_Newwindow(Cur_Buffer);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (!Win_Newwindow(Cur_Buffer,Show_First))
 {Error(2,No_Window_Txt);
  Cur_Buffer=Lst_Current;
  Current=(&Main_Buffer);
  return;
 }
#endif
#if defined(DOS) & defined(TV)
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
 In_Help=0;
 {register int I;
  for (I=0;I<Maxpoints+5;I++)
   Points[I]=0;
  for (I=0;I<Maxpoints+1;I++)
   End_Points[I]=0;
 }
#if (defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV)))
 Opt_Init();
 Rel_First-=Home=((Rel_First=Show_Ruler?3:2)+(Rel_Last=Scr_Length-2)+1)/2;
 Rel_Last=Scr_Length-2-Home;
 Pos_Cursor();
#endif
 Scr_Shift=0;
 Hdr_Changes=1;
 Points[Maxpoints+3]=Points[Maxpoints+4]=1;
 Auto_Saved=0;
 Infile=NULL;
}

void Fast Sedt_Main()
{
#if defined(VMS) | defined(OS2) | (defined(UNIX) & defined(TERMCAP) & !defined(XENIX))
 signal(SIGINT,CCint);
#endif
#if (defined(UNIX) & defined(TERMCAP) & !(defined(XENIX) | defined(HPUX)))
 signal (SIGTSTP,STPint);
#endif
 Editor();
 Sedt_Cleanup();
 Sedt_Shutdown();
}

/*      Cleanup the buffers removing all work files and on IBM compatibles
		  restore the normal keyboard interrupt.
*/
void Fast Sedt_Cleanup()
/*Remove all buffers*/
{Sav_Window(Cur_Buffer);
 {register int I;
  for (I=0;I<Maxbuffers;I++)
  if (Dl_Pointers[Pointers+I][0]!=0)
  {Load_Window(I);
   if (Auto_Saved)
    Rem_Autosave();
   B_Reset(&Main_Buffer);
  }
 }
}

void Fast Sedt_Shutdown()
{
#if defined(DOS) & !defined(Windows) & !defined(PC100)
 KEYREL();
#endif
  Top_Row=Left_Col=0;
  Set_Cursor(1,1);
#if !((defined(ATARI) & defined(GEM)) | ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | defined(MPE) | defined(WIN32))
 Reset_Width();
#else
 Reset_Term();
#endif
 B_Reset(&Pad);
 B_Reset(&Com);
 B_Reset(&Del);
 Reset_Buf();
#if defined(OS2)
 DosMonClose(KBD_Handle);
 MouClose(Mouse_Handle);
#endif
#if defined(VMS)
 T$Cancel();
 Dump_Messages();
#endif
#if defined(UNIX) & defined(TERMCAP)
 UEnd();
#endif
#if !((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) & !(defined(ATARI) & defined(GEM)) & !defined(MPE)
 if (Sharemess)
 {printf("%s\n",Share_1_Txt);
  printf("%s\n",Share_2_Txt);
  printf("%s\n",Share_3_Txt);
  printf("%s\n",Share_4_Txt);
  printf("%s\n",Share_5_Txt);
  printf("%s\n",Share_6_Txt);
 }
#endif
}

/*      Write the header on line 1 of the display. Save the current value
	of the variables displayed in the header so that we can regognize
	when they have changed.
*/
void Fast S_Header()
/*Print the First Line*/
{int Sav_Line;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;
 if (!Update_Flag)
  return;
 Sav_Shift=Scr_Shift;
 Scr_Shift=0;
 Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,1,(long)1,A_Reverse+A_Bold);
#if defined(GEM)
 GEM_S_Bar((Hdr_Lines=Buf_Lines)+1,This_Line);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Win_P_Bar((Hdr_Lines=Buf_Lines)+1,This_Line);
#endif
#if defined(DOS) & defined(TV)
 TV_P_Bar((Hdr_Lines=Buf_Lines)+1,This_Line);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if ((Hdr_Mods=(Any_Changes[Cur_Buffer]&1))!=0)
 {if (Read_Only[Cur_Buffer])
   Opt_String(Txt_Changed_Read_Only); else
   Opt_String(Unsaved_Txt);
 }
#else
 Opt_Char((char)(((Hdr_Mods=(Any_Changes[Cur_Buffer]&1))!=0)?
  (Read_Only[Cur_Buffer]?'#':'*'):' '));
 Opt_String(File_Txt);
 if (Outfile!=0)
  Opt_String(Outfile);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (Display_Column)
 {Skip_To(Col_Pos);
  Opt_String(Col_Txt);
  Put_I((long) (Hdr_Col=Sav_Col));
 }
 Skip_To(Line_Pos);
 Opt_String(Line_Txt);
 Put_I(Hdr_Line=This_Line);
 Skip_To(Buffer_Pos);
 Opt_String(Buffer_Txt);
 Put_I((long) Cur_Buffer);
 Skip_To(Scr_Width+1);
#else
 if (Display_Column)
 {Skip_To(Col_Pos);
  Opt_String(Col_Txt);
  Put_I((long) (Hdr_Col=Sav_Col));
 }
 Skip_To(Line_Pos);
 Opt_String(Line_Txt);
 Put_I(Hdr_Line=This_Line);
 Skip_To(Buffer_Pos);
 Opt_String(Buffer_Txt);
 Put_I((long) Cur_Buffer);
#if defined(DOS) | defined(NT)
 if (Caps_Lock)
 {Skip_To(Scr_Width);
  Opt_Char((char)(((Save_Caps=GETCAPS())!=0)?'C':' '));
 }
#endif
#if defined(VMS)
 Skip_To(Scr_Width);
 Opt_Char((Save_Mbx=Mbx_Count)?'M':' ');
#endif
 Skip_To(Scr_Width+1);
#endif
#if !defined(GEM) & !defined(Windows)
 Set_Physical();	/* 290a */ 
#endif
 Cur_Attributes=A_Reverse;
 Opt_Cursor(Scr_Length-1,(long)1);
 Skip_To(Dir_Pos);
 Opt_String((Hdr_Direction=Def_Direction)==1?Forward_Txt:Reverse_Txt);
 Skip_To(Replace_Pos);
 Opt_String((Hdr_Replace=Replace_Mode)!=0?Replace_Mode_Txt:Insert_Mode_Txt);
 Skip_To(Block_Pos);
 Opt_String((Hdr_Block=Blk_Mode)!=0?Block_Mode_Txt:Line_Mode_Txt);
 Skip_To(Tab_Pos);
 Opt_String((Hdr_Xpand=Tab_Expand)!=0?Tab_Exp_Txt:Tab_Ins_Txt);
 Skip_To(Case_Pos);
 Opt_String((Hdr_Case=Case_Sensitive)!=0?Case_Sen_Txt:Case_Ins_Txt);
 Skip_To(Reg_Pos);
 Opt_String((Hdr_Reg=Reg_Search)!=0?Reg_On_Txt:Reg_Off_Txt);
 Skip_To(Def_Pos);
 Opt_String((Hdr_Ins_Defs=Ins_Defs)!=0?Def_Ins_Txt:Def_Noins_Txt);
 Skip_To(Free_Pos);
 Opt_String((Hdr_Free=Cursor_Free)!=0?Free_Txt:Nofree_Txt);
 Skip_To(Shift_Pos);
 Opt_String(Screen_Shft_Txt);
 if ((Hdr_Do_Shift=Do_Shift)!=0)
  Put_I((long) (Hdr_Shift=Sav_Shift)); else
  Opt_String(Off_Txt);
 Skip_To(Learning_Pos);
 if ((Hdr_Executing=Executing)!=0)
  Opt_String(Executing_Txt); else
 if ((Hdr_Learning=Learning)!=0)
  Opt_String(Learning_Txt);
 Skip_To(Mod_Pos);
 Opt_String(Modified_Txt);
 {register int I;
  for (I=0;I<Maxbuffers;I++)
  {if (Any_Changes[I]&1)
   {Put_I((long)I);
    if (Read_Only[I])
     Opt_Char('R');
	 Opt_Char(' ');
   }
  }
 }
 Skip_To(Scr_Width+1);
#if !defined(GEM) & !defined(Windows)
 Set_Logical();		/* 290a */ 
#endif
 Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
 Scr_Shift=Sav_Shift;
}

/*      Update the header line of the display to reflect any changed made.
        Only changed data is written.
*/
void Fast Upd_Header()
{int Sav_Line;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;
 /*Diag("Upd_Header:Top_Row=%d",Top_Row);*/
#if defined(DOS) & !defined(Windows)
 if ((!Update_Flag&&(!Caps_Lock||Save_Caps==GETCAPS()))||In_Prompt)
#else
 if (!Update_Flag||In_Prompt)
#endif
  return;
#if defined(GEM)
 if (Hdr_Lines!=Buf_Lines)
  GEM_S_Bar((Hdr_Lines=Buf_Lines)+1,This_Line);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (Hdr_Lines!=Buf_Lines)
  Win_P_Bar((Hdr_Lines=Buf_Lines)+1,This_Line);
#endif
#if defined(DOS) & defined(TV)
 if (Hdr_Lines!=Buf_Lines)
  TV_P_Bar((Hdr_Lines=Buf_Lines)+1,This_Line);
#endif
 if (Hdr_Changes||Hdr_Mods!=(Any_Changes[Cur_Buffer]&1)||
  Hdr_Line!=This_Line||Hdr_Col!=My_Col)
 {Hdr_Changes=0;
  Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,1,(long)1,A_Reverse+A_Bold);
  Sav_Shift=Scr_Shift;
  Scr_Shift=0;
  if (Hdr_Mods!=(Any_Changes[Cur_Buffer]&1))
#if (defined(DOS) | defined(NT)) & defined(Windows)
   if (Any_Changes[Cur_Buffer]&1)
   {if (Read_Only[Cur_Buffer])
	  Opt_String(Txt_Changed_Read_Only); else
     Opt_String(Unsaved_Txt);
   } else
    Opt_String(Unsaved_Blank_Txt);
#else
   Opt_Char((char)(Any_Changes[Cur_Buffer]&1?(Read_Only[Cur_Buffer]?'#':'*'):
    ' '));
#endif
  if (Display_Column)
  {My_Col=Col_Arg_Pos;
   if (Hdr_Col!=Sav_Col)
   {Put_I((long) (Hdr_Col=Sav_Col));
    Skip_To(Line_Pos);
   }
  }
  My_Col=Line_Arg_Pos;
  if (Hdr_Line!=This_Line)
  {
#if defined(GEM)
  GEM_P_Bar((Hdr_Lines)+1,This_Line);
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
  Win_P_Bar((Hdr_Lines)+1,This_Line);
#endif
#if defined(DOS) & defined(TV)
  TV_P_Bar((Hdr_Lines)+1,This_Line);
#endif
   Put_I(Hdr_Line=This_Line);
   Skip_To(Buffer_Pos);
  }
#if defined(DOS) & !defined(Windows)
  if (Caps_Lock)
  {if (Save_Caps!=GETCAPS())
   {My_Col=Scr_Width;
    Opt_Char((char)((Save_Caps=GETCAPS())!=0?'C':' '));
   }
  }
#endif
#if defined(VMS)
  if (Save_Mbx!=Mbx_Count)
  {My_Col=Scr_Width;
   Opt_Char((Save_Mbx=Mbx_Count)?'M':' ');
  }
#endif
  Cur_Attributes=A_Reverse;
#if !defined(GEM) & !defined(Windows)
  Set_Physical();	/* 290a */ 
#endif
  Opt_Cursor(Scr_Length-1,(long)1);
  Skip_To(Dir_Pos);
  if (Hdr_Direction!=Def_Direction)
   Opt_String((Hdr_Direction=Def_Direction)==1?Forward_Txt:Reverse_Txt);
  if (Hdr_Replace!=Replace_Mode)
  {My_Col=Replace_Pos;
   Opt_String((Hdr_Replace=Replace_Mode)!=0?Replace_Mode_Txt:Insert_Mode_Txt);
  }
  if (Hdr_Block!=Blk_Mode)
  {My_Col=Block_Pos;
   Opt_String((Hdr_Block=Blk_Mode)!=0?Block_Mode_Txt:Line_Mode_Txt);
  }
  if (Hdr_Xpand!=Tab_Expand)
  {My_Col=Tab_Pos;
   Opt_String((Hdr_Xpand=Tab_Expand)!=0?Tab_Exp_Txt:Tab_Ins_Txt);
  }
  if (Hdr_Case!=Case_Sensitive)
  {My_Col=Case_Pos;
   Opt_String((Hdr_Case=Case_Sensitive)!=0?Case_Sen_Txt:Case_Ins_Txt);
  }
  if (Hdr_Reg!=Reg_Search)
   {My_Col=Reg_Pos;
    Opt_String((Hdr_Reg=Reg_Search)!=0?Reg_On_Txt:Reg_Off_Txt);
  }
  if (Hdr_Ins_Defs!=Ins_Defs)
  {My_Col=Def_Pos;
   Opt_String((Hdr_Ins_Defs=Ins_Defs)!=0?Def_Ins_Txt:Def_Noins_Txt);
  }
  if (Hdr_Free!=Cursor_Free)
  {My_Col=Free_Pos;
   Opt_String((Hdr_Free=Cursor_Free)!=0?Free_Txt:Nofree_Txt);
  }
  if (Hdr_Shift!=Sav_Shift||Hdr_Do_Shift!=Do_Shift)
  {My_Col=Shift_Arg_Pos;
   if ((Hdr_Do_Shift=Do_Shift)!=0)
    Put_I((long) (Hdr_Shift=Sav_Shift)); else
    Opt_String(Off_Txt);
   Skip_To(Learning_Pos);
  }
  if (Hdr_Learning!=Learning)
  {My_Col=Learning_Pos;
   if ((Hdr_Learning=Learning)!=0)
    Opt_String(Learning_Txt);
   Skip_To(Mod_Pos);
  } else
  if (Hdr_Executing!=Executing)
  {My_Col=Learning_Pos;
   if ((Hdr_Executing=Executing)!=0)
	 Opt_String(Executing_Txt);
   Skip_To(Mod_Pos);
  }
  if (Hdr_Mods!=(Any_Changes[Cur_Buffer]&1))
  {register int I;
   My_Col=Mod_List_Pos;
   for (I=0;I<Maxbuffers;I++)
   {if (Any_Changes[I]&1)
    {Put_I((long)I);
     if (Read_Only[I])
      Opt_Char('R');
     Opt_Char(' ');
    }
   }
   Skip_To(Scr_Width+1);
   Hdr_Mods=Any_Changes[Cur_Buffer]&1;
  }
#if !defined(GEM) & !defined(Windows)
  Set_Logical();		/* 290a */ 
#endif
  Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
  Scr_Shift=Sav_Shift;
 }
}

/*      Skip to the column given in argument C writing spaces along the way.
*/
void Fast Skip_To(int C)
{if (My_Col>C)
  My_Col=C; else
 while (My_Col!=C)
  Opt_Char(' ');
}

/*      Write the value of the positive integer given in I on the display
	starting at the current column.
*/
void Fast Put_I(long I)
{if (I==0)
 Opt_Char('0'); else
 Rec_I(I);
}

/*      Recursive procedure used by Put_I to write a single digit and call
        itself if any more digits need to be written.
*/
void Fast Rec_I(long I)
{if (I>=10)
 {Rec_I(I/10);
  Opt_Char((char)('0'+I%10));
 } else
  Opt_Char((char)('0'+I));
}

/*      Write the ruler line on the second line of the display. This procedure
        uses the current value of Scr_Shift to determine the shoft of the
        displayed ruler.
*/
void Fast S_Ruler()
/*Print the ruler*/
{int Sav_Line;
 long Sav_Col;
 char Sav_Attributes;
 if (!Update_Flag)
  return;
 Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,2,(long)Scr_Shift+1,A_Reverse);
 {register int I;
  for (I=1;I<=Scr_Width;I++)
  if (Scr_Shift+I>512)
   Format_Line[I-1]=(I+Scr_Shift)%8==1?'T':' '; else
   Format_Line[I-1]=(Tabs[I+Scr_Shift]==1)?'T':' ';
 }
 if (Indent>Scr_Shift&&Indent-Scr_Shift<=Scr_Width)
  Format_Line[Indent-1-Scr_Shift]='I';
 if (Right_Margin>Scr_Shift&&Right_Margin-Scr_Shift<=Scr_Width)
  Format_Line[Right_Margin-1-Scr_Shift]=Justify?'J':'R';
 if (Left_Margin>Scr_Shift&&Left_Margin-Scr_Shift<=Scr_Width)
  Format_Line[Left_Margin-1-Scr_Shift]='L';
 if (Word_Wrap>Scr_Shift&&Word_Wrap-Scr_Shift<=Scr_Width)
  Format_Line[Word_Wrap-1-Scr_Shift]=Auto_Margin?'A':'W';
 Format_Line[Scr_Width]=0;
 Opt_String(Format_Line);
 Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
}

void Fast Argerror()
{Initerror(Arg_Error_Txt);
}

void Fast Cnf_Error()
{Conf_Error=1;
}

#if defined(VMS) | defined(ATARI) | defined(SCO) | defined(ULTRIX) | defined(CONVEX) | defined(MPE) | defined (HPUX) | defined(LINUX)
int stricmp(S,T)
char *S, *T;
{char A, B;
 while ((A= *(S++))==(B= *(T++))&&A!=0&&B!=0)
  ;
 if (A==B)
  return 0;
 if (A<B)
  return -1;
 return 1;
}
#endif

void Fast Sedt_Args(int Argc,char * *Argv)
{
 char *P;
 {register int I;
  for (I=1;I<Argc;I++)
  if ((P=Argv[I])[0]!=13)
  {if (P[0]=='-')
   {
    if (tolower(P[1])=='i')
    {if (P[2]!=0)
      strcpy(Init_File,P+2); else
     {if (++I==Argc)
       Argerror(); else
		 strcpy(Init_File,Argv[I]);
     }
	 } else
     Test_Args(P);
   }
  }
 }
}

void Fast Test_Args(char *P)
{
#if defined(VMS)
 if (tolower(P[1])=='p')
 {register int J;
  register char C;
  J=2;
  Parent_PID=0;
  while (C=P[J++])
  {if (isdigit(C))
    Parent_PID=Parent_PID*16+C-'0'; else
   if (C>='A'&&C<='F')
    Parent_PID=Parent_PID*16+C-'A'+10; else
   if (C>='a'&&C<='f')
    Parent_PID=Parent_PID*16+C-'a'+10; else
	 Argerror();
  }
  if (!Parent_PID)
   Argerror();
 } else
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 if (tolower(P[1])=='k'&&P[2]==0)
  Keyb_Type=Keyb_New; else
 if (tolower(P[1])=='k')
 {Keyb_Type=Keyb_Special;
  strcpy(Keyb_Name,P+2);
 } else
 if (tolower(P[1])=='v'&&P[2]==0)
  Dir_Video=0; else
#endif
 if (tolower(P[1])=='e'&&tolower(P[2])=='d'&&tolower(P[3])=='t'&&P[4]==0)
  Mapping=EDT_Mapping; else
 if (tolower(P[1])=='w'&&tolower(P[2])=='p'&&tolower(P[3])=='s'&&P[4]==0)
  Mapping=WPS_Mapping; else
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 if (tolower(P[1])=='m'&&P[2]>='0'&&P[2]<='9')
 {register char C;
  P+=2;
  New_Mode=0;
  while ((C= *(P++))!=0)
  {New_Mode*=16;
   if (C>='0'&&C<='9')
    New_Mode+=C-'0'; else
   if (C>='A'&&C<='F')
    New_Mode+=C-'A'+10; else
   Argerror();
  }
 } else
#endif
 if (tolower(P[1])=='w'&&tolower(P[2])=='i')
 {Sub_X=(P[3]-'0')*10+P[4]-'0';
  Sub_Y=(P[5]-'0')*10+P[6]-'0';
  Sub_H=(P[7]-'0')*10+P[8]-'0';
  Sub_W=(P[9]-'0')*10+P[10]-'0';
  Sub_Window=1;
 } else
 if (tolower(P[1])=='l')
 {register int J;
  register char C;
  Mem_Limit=0;
  for (J=2;(C=P[J])!=0;J++)
  {if (!isdigit(C))
	 Argerror();
   Mem_Limit=Mem_Limit*10+C-'0';
  }
 } else
 if (tolower(P[1])=='a'&&P[2]==0)
  Auto_Save=Auto_Save?0:200; else
 if (tolower(P[1])=='e'&&P[2]==0)
  Nosub=1; else
 if (tolower(P[1])=='b'&&P[2]==0)
#if defined(VMS)
  Nobak=0; else
#else
  Nobak=1; else
#endif
 if (tolower(P[1])=='b'&&tolower(P[2])=='a'&&P[3]==0)
  Batch_Mode=1; else
 if (tolower(P[1])=='r'&&P[2]==0)
  Doreturn=1; else
 if (tolower(P[1])=='p'&&tolower(P[2])=='c'&&P[3]==0)
  Call_Type=0xFF; else
 if (tolower(P[1])=='a'&&tolower(P[2])=='t'&&P[3]==0)
  Call_Type=0xFC; else
 if (tolower(P[1])=='m'&&tolower(P[2])=='o'&&P[3]==0)
  Call_Color=1; else
 if (tolower(P[1])=='c'&&tolower(P[2])=='o'&&P[3]==0)
  Call_Color=2; else
 if (tolower(P[1])=='e'&&tolower(P[2])=='g'&&tolower(P[3])=='a'&&P[4]==0)
  EGA43=1; else
 if (tolower(P[1])=='r'&&tolower(P[2])=='o'&&P[3]==0)
  ; else
 if (tolower(P[1])=='s')
 {register int J;
  register char C;
  J=2;
  Mem_Entries=0;
  while ((C=P[J++])!=0)
  {if (!isdigit(C))
    Argerror();
	Mem_Entries=Mem_Entries*10+C-'0';
  }
  if (!Mem_Entries)
   Argerror();
  Mem_Entries=(long)Mem_Entries*1024L*1024L/Blocksize;
 } else
 if (tolower(P[1])=='y')
 {strcpy(Map_Ext,P+2);
  if (stricmp(Map_Ext,"EDT")==0)
   Mapping=EDT_Mapping; else
  if (stricmp(Map_Ext,"WPS")==0)
   Mapping=WPS_Mapping; else
	Mapping=Other_Mapping;
 } else
#if defined(UNIX)
 if (tolower(P[1])=='t')
 {strcpy(Defined_Terminal,P+2);
  Call_Type=8;
 } else
#endif
 if (tolower(P[1])=='o')
  ; else
 {register int J;
  register char C;
  J=1;
  First_Line=0;
  while ((C=P[J++])!=0)
  {if (!isdigit(C))
	 Argerror();
	First_Line=First_Line*10+C-'0';
  }
  if (!First_Line)
	Argerror();
 }
}

void Fast Sedt_Files(int Argc,char * *Argv)
{
 char *P;
#if defined(DOS) | defined(UNIX) | defined(WIN32)
 struct stat File_Stat;
#endif
 int Load_It, Make_Read_Only;
 {register int I;
  for (I=1;I<Argc;I++)
  if ((P=Argv[I])[0]!=13)
  {if (P[0]=='-')
	{
#if defined(DOS) | defined(UNIX) | defined(WIN32)
	 if ((tolower(P[1])=='r'&&tolower(P[2])=='o'&&P[3]==0) || tolower(P[1])=='o')
	 {if (File_Reserved[Cur_Buffer])
	  {chmod(New_File,File_Protection[Cur_Buffer]);
	  }
	 }
#endif
	 if (tolower(P[1])=='r'&&tolower(P[2])=='o'&&P[3]==0)
	  Read_Only[Cur_Buffer]=1; else
	 if (tolower(P[1])=='o')
	 {if (P[2]!=0)
		strcpy(Outfile=New_File,P+2); else
	  {if (++I==Argc)
		 Argerror(); else
		 strcpy(Outfile=New_File,Argv[I]);
	  }
#if defined (UNIX)
	  Read_Only[Cur_Buffer]=0;
	  if (stat(New_File,&File_Stat)!=0)
	  {Plain_Message(No_Stat_Txt);
	  } else
	  if (Reservation)
	  {File_Reserved[Cur_Buffer]=0;
		if ((File_Stat.st_mode&(S_IWUSR+S_IWGRP+S_IWOTH))==0)
		{if (Yes_No(Out_File_Reserved_Txt)!=1)
		 {Zero();
		  if (Cur_Buffer!=0)
			Ch_Window(Cur_Buffer-1);
		 } else
		  Read_Only[Cur_Buffer]=1;
		} else
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
#if defined (DOS) | defined(WIN32)
	  Read_Only[Cur_Buffer]=0;
	  if (stat(New_File,&File_Stat)!=0)
	  {Plain_Message(No_Stat_Txt);
	  } else
	  if (Reservation)
	  {File_Reserved[Cur_Buffer]=0;
		if ((File_Stat.st_mode&(S_IWRITE))==0)
		{if (Yes_No(Out_File_Reserved_Txt)!=1)
		 {
#if defined(Windows)
		  Win_Del_Window();
#else
		  Zero();
#endif
		  if (Cur_Buffer!=0)
			Ch_Window(Cur_Buffer-1);
		 } else
		  Read_Only[Cur_Buffer]=1;
		} else
		if (chmod(New_File,File_Stat.st_mode&~(S_IWRITE))!=0)
		 Plain_Message(No_Protection_Change_Txt); else
		{Plain_Message(Reserved_Txt);
		 File_Reserved[Cur_Buffer]=1;
		 File_Protection[Cur_Buffer]=File_Stat.st_mode;
		}
	  }
#endif
#if defined(VMS)
	  Strip_Version();
#endif
	 } else
	 if (P[1]=='i')
	 {if (P[2]==0)
		I++;
	 }
	} else
	{File_Arguments=1;
#if defined(UNIX) | defined(DOS) | defined(WIN32)
	 Load_It=0;
#else
	 Load_It=1;
#endif
			Make_Read_Only=0;
	 strcpy(Com_Arg,Argv[I]);
#if defined (UNIX)
	 if (stat(Com_Arg,&File_Stat)!=0)
	 {Plain_Message(No_Stat_Txt);
			 Load_It=1;
	 } else
	 if (Reservation)
	 {File_Reserved[Cur_Buffer]=0;
	  if ((File_Stat.st_mode&(S_IWUSR+S_IWGRP+S_IWOTH))==0)
	  {if (Yes_No(File_Reserved_Txt)==1)
		{Load_It=1;
		 Make_Read_Only=1;
		}
	  } else
	  {if (File_Stat.st_uid==My_UID)
		{if (chmod(Com_Arg,File_Stat.st_mode&~(S_IWUSR+S_IWGRP+S_IWOTH))!=0)
		 Plain_Message(No_Protection_Change_Txt); else
		 {Plain_Message(Reserved_Txt);
		  File_Reserved[Cur_Buffer]=1;
		  File_Protection[Cur_Buffer]=File_Stat.st_mode;
		 }
		} else
		{if (File_Stat.st_gid==My_GID)
		 {Plain_Message(Group_Txt);
		 } else
		 {Plain_Message(Other_Txt);
		 }
		}
		Load_It=1;
	  }
	 } else
	  Load_It=1;
#endif
#if defined (DOS) | defined(WIN32)
	 if (stat(Com_Arg,&File_Stat)!=0)
	 {Plain_Message(No_Stat_Txt);
			 Load_It=1;
	 } else
	 if (Reservation)
	 {File_Reserved[Cur_Buffer]=0;
	  if ((File_Stat.st_mode&(S_IWRITE))==0)
	  {if (Yes_No(File_Reserved_Txt)==1)
		{Load_It=1;
		 Make_Read_Only=1;
		}
	  } else
	  {if (chmod(Com_Arg,File_Stat.st_mode&~(S_IWRITE))!=0)
		Plain_Message(No_Protection_Change_Txt); else
		{Plain_Message(Reserved_Txt);
		 File_Reserved[Cur_Buffer]=1;
		 File_Protection[Cur_Buffer]=File_Stat.st_mode;
		}
		Load_It=1;
	  }
	 } else
	  Load_It=1;
#endif
	 if (Load_It)
	 {if (Infile!=NULL)
		Ch_Window(Cur_Buffer+1);
#if (defined(DOS) | defined(NT)) & defined(Windows)
	  if (!Update_Flag)
		Update_On();
	  Mark_Cursor();
	  Out_Zap();
#endif
 	  strcpy(Infile=Outfile=New_File,Com_Arg);
	  Read_File(Infile);
          if (Make_Read_Only)
           Read_Only[Cur_Buffer]=1;
	  W_D_Border();
	  if (!Err_Flag&&Save_Context)
		Read_Context();
#if (defined(ATARI) & defined(GEM)) | defined(DOS) | defined(NT) | defined(WIN32)
	  S_Header();
	  if (Show_Ruler)
		S_Ruler();
	  Fill_Screen(Rel_First,Rel_Last);
#endif
	 }
	 Err_Flag=0;
	}
  }
 }
 if (Cur_Buffer!=0)
  Ch_Window(0);
}

/*      Displays a fatal error message, cleans up the buffer database and
        exits from SEDT.
*/
void Fast Initerror(char *S)
{
#if (defined(DOS) | defined(NT)) & defined(Windows)
 if (Curr_Wnd==0)
  Make_Window(0,SHOW_OPENWINDOW);
 Win_Error(S);
#elif defined(DOS) & defined(TV)
 TV_Error(S);
#else
 printf("%s",S);
#endif
#if defined(OS2)
 DosExit(1,1);
#else
#if defined(DOS) & ! defined(Windows)
 Reset_Crit();
#if defined(DESQVIEW)
 if (DV)
  api_exit();
#endif
#endif
#if defined(DOS) & defined(TV)
 TV_Exit(1);
#endif
 _exit(1);
#endif
}
