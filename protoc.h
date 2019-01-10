#if (defined(DOS) & !defined(Windows))
#define Fast _fastcall
#else
#define Fast
#endif
/*SEDT.C*/
void  CCint();
int  main();
void  Fast Write_Rem();
void  Fast Sedt_Init();
void  Fast Init_Buffer();
#if (defined(DOS) | defined(NT)) & defined(Windows)
void  Sedt_Main();
#else
void  Fast Sedt_Main();
#endif
void  Fast Sedt_Cleanup();
void  Fast Sedt_Shutdown();
void  Fast S_Header();
void  Fast Upd_Header();
void  Fast Skip_To();
void  Fast Put_I();
void  Fast Rec_I();
void  Fast S_Ruler();
void  Fast Argerror();
void  Fast Read_Config();
void  Fast Test_Config();
void  Fast Test_Files();
void  Fast Test_Searches();
void  Fast Test_Mode();
void  Fast Test_Screenshift();
void  Fast Test_Mapping();
void  Fast Test_System();
void  Fast Test_Keyboard();
void  Fast Test_Palette();
void  Fast Test_Auto();
void  Fast Cnf_Error();
void  Fast Sedt_Args();
void  Fast Test_Args();
void  Fast Sedt_Files();
void  Fast Initerror();
/*EDIT.C*/
void  Fast Reset_Width();
void  Fast Editor();
void  Fast Readruler();
void  Fast Fill_Rest();
void  Fast Fill_Screen();
void  Fast Read_File();
void  Fast Filnam();
void  Fast Filtyp();
void  Fast Filjnam();
void  Fast Fildir();
void  Fast Include_File();
void  Error();
void  Fast Plain_Error();
void  Message();
void  Fast Plain_Message();
void  Diag();
int  Fast Pad_Put();
int  Fast Pad_Get();
void  Fast Get16();
void  Fast Clr_Error();
void  Fast Do_Command();
void  Fast Clr_Working();
void  Fast Ins_Char();
long  Fast Read_Count();
void  Fast Put_Count();
char  *Find_From();
char  *Find_Env();
char  *Find_Path();
#if defined(VMS)
char  *Find_Dir();
char  *Find_Lib();
char  *Logical_File();
void  Strip_Version();
#endif
/*KEY.C*/
void  Fast Rem_Pad();
void  Fast Rem_Del();
void  Fast Adj_Pad();
void  Fast Adj_Del();
void  Fast Exec_Key();
void  Fast Expand_Com();
void  Fast Set_Physical();
void  Fast Set_Logical();
void  Fast Push();
int  Fast Pop();
void  Fast Mark_Cursor();
void  Fast Unmark_Cursor();
int  Fast Test_Dir_Key();
int  Fast Ins_Key();
void  Fast Key_Get();
void  Fast Ins_A_Key();
int  Fast Readkeys();
int  Fast Readmaps();
int  Fast Char_Rep();
void  Fast Erase_Del();
long  *Key_Addr();
/*BUF.C*/
void  Fast C_Move();
void  Fast S_Copy();
int C_Scan();
void  Fatalerror();
int  Crit_Int();
void  Fast Blockread();
void  Fast Blockwrite();
void  Fast Out_Buf();
void  Fast In_Buf();
void  Fast B_Flush();
void  Fast B_Rewind();
void  Fast B_Goto();
void  Fast Out_In_Buf();
void  Fast B_Init();
void  Fast B_Reset();
void  Fast Zero_Buffer();
void  Fast B_Modify();
void  Fast Make_Room();
void  Fast B_Put();
void  Fast B_Put_S();
void  Fast B_Repl_S();
int  Fast B_Replace();
int  Fast B_Del();
int  Fast B_Get();
void  Fast B_Get_N();
int  Fast B_Get_S();
void  Fast B_Rem();
void  Fast B_Copy();
int  Fast B_B_Get();
int  Fast B_Search();
int  Fast B_B_Search();
void  Fast Mem_Return();
void  Fast Init_Buf();
void  Fast Reset_Buf();
int  Fast Mem_Alloc();
void  Fast Move_To_EMS();
void  Fast Move_From_EMS();
int  Fast Check_EMS();
int  Fast Get_EMS();
void  Fast Return_EMS();
/*VDU.C*/
void  Fast Out_C();
void  Fast Out_I();
void  Fast Out_S();
void  Fast Out_Zap();
void  Fast Opt_Init();
void  Fast Space_Fill();
void  Fast Zero_Fill();
void  Fast Opt_Cursor();
void  Fast Put_Char();
void  Fast Opt_Move_Cursor();
void  Fast Opt_Char();
void  Fast Opt_String();
void  Fast Opt_Erase_Line();
void  Fast Get_Type();
/*TERM.C*/
#if defined(VMS) | defined(UNIX)
void  Reposition_Input();
void  Mouse_1_Down();
void  Mouse_1_Up();
void  Mouse_2_Down();
void  Mouse_2_Up();
void  Mouse_3_Down();
void  Mouse_3_Up();
void  Mouse_4_Down();
void  Mouse_4_Up();
void  Mouse_Outside();
void  Enable_Mouse();
void  Disable_Mouse();
int  Mouse_Get_Rest();
#endif
void  Mouse_Int();
void  Fast Mouse_Call();
void  Fast Mouse_Execute();
void  Fast Set_Terminal();
void  Fast Sub_Set();
void  Fast Set_Cursor();
void  Fast Opt_Scroll();
void  Fast Scl_Up();
void  Fast Scl_Down();
void  Fast Clr_Screen();
void  Fast Reset_Screen();
void  Fast Set_Scroll();
void  Fast Move_Right();
void  Fast Move_Left();
void  Fast Opt_Force_Attributes();
void  Fast Del_To_End_Line();
char  *Do_Mapping();
char  *Keyfile();
char  *Mapfile();
char  *Helpfile();
void  Fast Conin();
void  Fast Getkbd();
void  Fast Getkey();
int  Fast Next_Comb();
int  Fast Check_Comb();
void  Fast Setup_Term();
void  Fast Reset_Term();
void  Fast Wide_Screen();
void  Fast Narrow_Screen();
void  Fast Term_Error();
void  Fast Key_Error();
void  Fast Esc_Error();
void  Fast Mouse();
/*COM.C*/
void  Fast Sav_Set();
void  Fast Set_Save();
void  Fast Clr_Typeahead();
int  Fast Yes_No();
void  Fast Exit_Sedt();
void  Fast Rem_Autosave();
void  Fast Quit();
void  Fast Tab_To();
void  Fast A_New_Line();
void  Fast New_Line();
void  Fast Do_New_Line();
int  Fast Find_Margin();
void  Fast Del_It();
void  Fast Get();
int  Fast Get_E_Type();
void  Fast Advance();
void  Fast Backup();
void  Fast Select();
void  Fast Paste();
void  Fast Und_Char();
void  Fast Und_Word();
void  Fast Und_Line();
void  Fast Und_Par();
void  Fast Und_Pg();
void  Fast Und_Unknown();
void  Fast Und_Sen();
void  Fast Window();
void  Fast Ascii();
int  Fast Get_Arg();
void  Fast File_Get();
void  Fast Zero();
void  Fast Mark_Pos();
void  Fast End_Mark();
void  Fast Subs();
void  Fast Subs_Conf();
void  Fast Ins_Del();
void  Fast Subs_Next();
void  Fast Subs_Next_conf();
void  Fast Put_Rep();
void  Fast Insert();
void  Fast File_Edit();
void  Fast Edit_File();
void  Fast C_Read_L();
void  Fast C_Read_I();
void  Fast Read_Context();
void  Fast File_Save();
void  Fast File_Write();
int  Fast End_Com();
void  Fast Def_Key_Function();
void  Fast Def_Key_Alpha();
void  Fast Def_Key_Key();
void  Fast Def_Key_G_Alpha();
void  Fast Def_Key_G_Key();
void  Fast Def_Key();
void  Fast Def_Key_Number();
void  Fast Def_Function();
void  Fast Reset();
void  Fast Undo();
int  Fast Input_Test();
void  Fast Test_Input();
void  Fast Tab();
void  Fast Overstrike_Mode();
void  Fast Tab_Toggle();
void  Fast Show_Column();
void  Fast Get_Ruler();
void  Fast Shift_Toggle();
void  Fast Case_Toggle();
void  Fast Ins_Defs_Toggle();
void  Fast Cursor_Free_Toggle();
void  Fast Change_Window();
void  Fast Err_Level();
void  Fast Tog_Marks();
void  Fast Tog_Block();
void  Fast Tog_Control();
void  Fast Tog_Direction();
void  Fast Show_Version();
void  Fast Tog_Cursor();
void  Fast Tog_Reg_Search();
void  Fast Tog_Ruler();
void  Fast Curs_Top();
void  Fast Curs_Bottom();
void  Fast Curs_Middle();
void  Fast Shift_In();
void  Fast Shift_Out();
void  Fast Font_Larger();
void  Fast Font_Smaller();
void  Fast Update_Off();
void  Fast Update_On();
void  Fast Remove_Char();
void  Fast Remove_Help();
void  Fast Remove_Line();
void  Fast Remove_Page();
void  Fast Remove_Par();
void  Fast Remove_Paste();
void  Fast Remove_Search();
void  Fast Remove_Sent();
void  Fast Remove_Subs();
void  Fast Remove_Unknown();
void  Fast Remove_Word();
void  Fast Set_Block();
void  Fast Set_Replace();
void  Fast Set_Direction();
void  Fast Set_Tab_Expand();
void  Fast Set_Case_Sensitive();
void  Fast Set_Reg_Search();
void  Fast Set_Ins_Defs();
void  Fast Set_Cursor_Free();
void  Fast Counter_Set();
void  Fast Counter_Put();
void  Fast Counter_Increment();
void  Fast Counter_Absolute_Value();  /* 290a */
void  Fast End_Learn();
void  Fast Context_Save();
void  Fast Save_The_Context();
void  Fast Delete_The_Context();
void  Fast C_Write();
void  Fast Check_Point();
void  Fast Push_Changes();
void  Fast Pop_Changes();
void  Fast Show_Bcast();
void  Fast Window_Used();
void  Fast Window_Changed();
void  Fast Window_Not_Used();
void  Fast Window_Cycle();
void  Fast Redefine_Key();
void  Fast Show_Ascii();
void  Fast W_D_Border();
void  Fast W_S_Border();
void  Fast Open_Window();
void  Fast Refresh_Window();
void  Fast Window_Grow();
void  Fast Window_Shrink();
void  Fast Window_Move();
void  Fast Window_Zoom();
void  Fast Window_Cascade();
void  Fast Window_Tile();
void  Fast Window_Fix_Bottom();
void  Fast Window_Fix_Left();
void  Fast Window_Fix_Right();
void  Fast Window_Fix_Top();
long  Fast Get_Count();
void  Fast Com_Parse();
int  Fast Check_CC();
void  Fast Signal_CC();
void  Fast Ex_Command();
void  Fast Move_Entity();
void  Fast Move_Forward();
void  Fast Move_Back();
void  Fast Goto_This();
void  Fast Paint_To_This();
void  Fast Backto_This();
void  Fast B_Save();
int  Fast Put_Out();
int  Fast Get_Entity();
void  Fast Adj_Points();
/*COMA.C*/
void  Fast Ruler();
void  Fast Get_Spaces();
char  Fast Get_Word();
void  Fast Fill();
void  Fast Put_A_Char();
void  Fast Blind_Tab_To();
void  Fast Do_Fill();
void  Fast Adj_Tabs();
void  Fast Center_Line();
void  Fast Refresh();
void  Fast Repaint();
void  Fast Width();
void  Fast Spawn();
void  Fast Q_Spawn();
void  Fast R_Spawn();
void  Fast Ch_Case();
void  Fast Ch_Lower();
void  Fast Ch_Upper();
void  Fast Ch_It();
void  Fast Repl();
void  Fast Print();
void  Fast File_Execute();
void  Fast Ex_File();
void  Fast Ind_Set();
void  Fast Ind_Decrease();
void  Fast Ind_Increase();
void  Fast Key_Load();
void  Fast Get_Key();
void  Fast Load_Key();
void  Fast Key_Save();
void  Fast Sav_Key();
void  Fast Del_Append();
void  Fast Get_Append();
void  Fast Ch_Edt_Style();
void  Fast Past_Load();
void  Fast Past_Save();
void  Fast Open_Line();
void  Fast Upd_Screen();
void  Fast Load_Learn();
void  Fast Ex_Learn();
void  Fast Search_Load();
void  Fast Out_Message();
void  Fast Put_Date();
void  Fast Put_Time();
void  Fast Out_Error();
void  Fast Other_Window();
void  Fast Rul_Load();
void  Fast Ins_String();
void  Fast Rul_Save();
void  Fast Save_Keys();
void  Fast Save_Functions();
void  Fast f_puti();
void  Fast Strip_Trailing();
void  Fast Set_Tabs();
void  Fast Remove_Tabs();
void  Fast Set_Edit_Region();
void  Fast Cancel_Edit_Region();
void  Fast Key_Mess();
void  Fast Clear_Region();
void  Fast Counter_Get();
void  Fast Counter_Add();
int  Fast Get_Counter_Index();
void  Fast Query();
void  Fast Execute_Key();
void  Fast Execute_Function();
void  Fast Diag_Error();
void  Fast Pos_Cursor();
void  Fast Set_Offset();
void  Fast Adj_Lines();
void  Fast Move_Key();
void  Fast Init_Entity();
void  Fast Blk_Rem_It();
void  Fast Rem_It();
void  Fast Wr_Buffer();
void  Fast Save_Auto();
void  Fast Set_Key();
void  Fast Undelete();
int  Fast Chk_EL();
void  Fast Sav_Window();
void  Fast Load_Window();
void  Fast Newwindow();
void  Fast Ch_Window();
void  Fast Com_Error();
void  Fast Syntax_Error();
void  Fast Unk_Err();
void  Fast Wr_Error();
void  Fast Set_Changes();
void  Fast Clr_Changes();
void  Fast Window_Position();
/*COMB.C*/
void  Fast Wait_Char();
void  Fast Show_Help();
void  Fast Display_Help();
void  Fast Help();
void  Fast Do_Help();
void  Fast Wait_Mess();
/*ENT.C*/
void  Fast regerror();
int  Fast Chk_CRLF_F();
int  Fast Chk_CRLF_FB();
int  Fast Chk_CRLF_B();
int  Fast Chk_CRLF_BF();
void  Fast A_Char();
void  Fast Char();
void  Fast Abs_Char();
void  Fast Vertical();
void  Fast Line();
void  Fast Abs_Line();
void  Fast Top();
void  Fast Bottom();
void  Fast Find();
void  Fast Find_Next();
char  Fast Upper_Case();
char  Fast Lower_Case();
void  Fast Find_This();
void  Fast Sel_Range();
void  Fast Word();
void  Fast A_Word();
void  Fast Get_Mark();
void  Fast Next_Mark();
void  Fast Line_End();
char  Fast Skip_Spaces();
char  Fast B_Skip_Spaces();
void  Fast Par();
void  Fast A_Par();
void  Fast Sentence();
void  Fast A_Sentence();
void  Fast Page();
void  Fast Screen();
void  Fast Match_Bracket();
void  Fast Ex_Entity();
void  Fast Cnt_F_Lines();
void  Fast Cnt_B_Lines();
void  Fast Set_Region();
void  Fast End_Error();
void  Fast Beg_Error();
void  Fast Delim_Error();
void  Fast Ent_Error();
/*FILE.C*/
struct  File *F_open();
int  Fast F_close();
char  *F_gets();
int  Fast F_read();
int  Fast F_getc();
int  Fast F_write();
int  Fast F_putc();
int  Fast F_seek();
/*REGEXP.C*/
struct  regexp  *regcomp();
char  *reg();
char  *regbranch();
char  *regpiece();
char  *regatom();
char  *regnode();
void  Fast regc();
void  Fast reginsert();
void  Fast regtail();
void  Fast regoptail();
int  Fast regexec();
int  Fast regtry();
int  Fast regmatch();
int  Fast regrepeat();
char  *regnext();
char  *reg();
char  *regbranch();
char  *regpiece();
char  *regatom();
char  *regnode();
void  Fast regc();
void  Fast reginsert();
void  Fast regtail();
void  Fast regoptail();
int  Fast regexec();
int  Fast regtry();
int  Fast regmatch();
int  Fast regrepeat();
char  *regnext();
/*REGSUB.C*/
void  Fast regsub();
/*WSEDT.C*/
#if defined(DOS) & defined(Windows)
#if !defined(WIN95)
int  _pascal _far _export About();
int  _pascal WinMain();
long  _pascal _far _export SedtWndProc();
int  _pascal _far _export Prompt();
#endif
void  SedtPaint();
int  SedtInit();
int  Make_Window();
void  Make_File();
void  Win_Scroll();
void  Win_Clear();
void  Win_Erase();
void  Win_On_Curs();
void  Win_Off_Curs();
void  Win_Getmem();
void  Win_Putmem();
unsigned short  Win_Alloc();
void  Win_Free();
void  WinText();
void  WinChar();
int  Win_Yes_No();
void  Win_Error();
void  Win_P_Bar();
int  Prompt_Input();
void  Prompt_Init();
void  Prompt_Exit();
int  Win_Newwindow();
void  Win_Topwindow();
void  Swap_Window();
void  Restore_Window();
void  Win_Name();
void  Win_Put_Clip();
void  Win_Load_Clip();
void  Win_Tempfile();
void  Win_Quit();
void  Win_Smaller();
void  Win_Larger();
void  Win_Font();
#endif
/*NTSEDT.C*/
#if defined(NT)
int WINAPI About();
void  SedtPaint();
int  SedtInit();
int  Make_Window();
int WINAPI WinMain();
void  Make_File();
long WINAPI SedtWndProc();
void  Win_Scroll();
void  Win_Clear();
void  Win_Erase();
void  Win_On_Curs();
void  Win_Off_Curs();
void  Win_Getmem();
void  Win_Putmem();
unsigned short  Win_Alloc();
void  Win_Free();
void  WinText();
void  WinChar();
int  Win_Yes_No();
void  Win_Error();
void  Win_P_Bar();
int  Prompt_Input();
void  Prompt_Init();
void  Prompt_Exit();
int WINAPI Prompt();
int  Win_Newwindow();
void  Win_Topwindow();
void  Swap_Window();
void  Restore_Window();
void  Win_Name();
void  Win_Put_Clip();
void  Win_Load_Clip();
void  Win_Tempfile();
void  Win_Quit();
void  Win_Smaller();
void  Win_Larger();
void  Win_Font();
#endif
