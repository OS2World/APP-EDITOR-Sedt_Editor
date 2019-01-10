/*Message displayed when control C is typed*/
#if defined(DOS) | defined(NT)
Txt_Fun(Cc_Mess,"Control Break typed, error condition set")
#else
Txt_Fun(Cc_Mess,"Control C typed, error condition set")
#endif

/*Header line fields*/
/*Indicator on Windows*/
Txt_Fun(Unsaved_Txt," Unsaved Changes   ")
Txt_Fun(Txt_Changed_Read_Only," Modified Read Only")
/*Value to clear Windows indicator*/
Txt_Fun(Unsaved_Blank_Txt,"                   ")
Txt_Fun(File_Txt,"File: ")
Txt_Fun(Col_Txt,"C=")
Txt_Fun(Line_Txt,"L=")
Txt_Fun(Buffer_Txt,"B=")
/*Column positions for header line fields*/
#if (defined(DOS) | defined(NT)) & defined(Windows)
Int_Fun(Col_Pos,21)
Int_Fun(Col_Arg_Pos,23)
Int_Fun(Line_Pos,30)
Int_Fun(Line_Arg_Pos,32)
Int_Fun(Buffer_Pos,39)
#else
Int_Fun(Col_Pos,58)
Int_Fun(Col_Arg_Pos,60)
Int_Fun(Line_Pos,67)
Int_Fun(Line_Arg_Pos,69)
Int_Fun(Buffer_Pos,76)
#endif

/*Mode line fields*/
Int_Fun(Dir_Pos,2)
Txt_Fun(Forward_Txt,"Fwd")
Txt_Fun(Reverse_Txt,"Rev")
Int_Fun(Replace_Pos,6)
Txt_Fun(Replace_Mode_Txt,"Rep")
Txt_Fun(Insert_Mode_Txt,"Ins")
Int_Fun(Block_Pos,10)
Txt_Fun(Block_Mode_Txt,"Blk")
Txt_Fun(Line_Mode_Txt,"Lin")
Int_Fun(Tab_Pos,14)
Txt_Fun(Tab_Exp_Txt,"Tab:Exp")
Txt_Fun(Tab_Ins_Txt,"Tab:Ins")
Int_Fun(Case_Pos,22)
Txt_Fun(Case_Sen_Txt,"Case:On ")
Txt_Fun(Case_Ins_Txt,"Case:Off")
Int_Fun(Reg_Pos,31)
Txt_Fun(Reg_On_Txt,"Regexp=On ")
Txt_Fun(Reg_Off_Txt,"Regexp=Off")
Int_Fun(Def_Pos,42)
Txt_Fun(Def_Ins_Txt,"Def")
Txt_Fun(Def_Noins_Txt,"Chr")
Int_Fun(Free_Pos,46)
Txt_Fun(Free_Txt,"Free")
Txt_Fun(Nofree_Txt,"Rest")
Int_Fun(Shift_Pos,51)
Int_Fun(Shift_Arg_Pos,57)
Txt_Fun(Screen_Shft_Txt,"Shift=")
Txt_Fun(Off_Txt,"Off")
Int_Fun(Learning_Pos,64)
Txt_Fun(Executing_Txt,"Exe")
Txt_Fun(Learning_Txt,"Lrn")
Int_Fun(Mod_Pos,68)
Int_Fun(Mod_List_Pos,72)
Txt_Fun(Modified_Txt,"Mod=")

/*SEDT.CNF content values*/
Txt_Fun(Txt_Screen,"SCREEN")
Txt_Fun(Txt_Color,"COLOR")
Txt_Fun(Txt_Mono,"MONO")
Txt_Fun(Txt_Standard,"STANDARD")
Txt_Fun(Txt_Keyboard,"KEYBOARD")
Txt_Fun(Txt_LK250,"LK250")
Txt_Fun(Txt_New,"NEW")
Txt_Fun(Txt_Enhanced,"ENHANCED")
Txt_Fun(Txt_Convertible,"CONVERTIBLE")
Txt_Fun(Txt_At,"AT")
Txt_Fun(Txt_SLT,"SLT")
Txt_Fun(Txt_System,"SYSTEM")
Txt_Fun(Txt_Ibmpc,"IBMPC")
Txt_Fun(Txt_Ibmxt,"IBMXT")
Txt_Fun(Txt_Ibmat,"IBMAT")
Txt_Fun(Txt_Ibmconvertible,"IBMCONVERTIBLE")
Txt_Fun(Txt_Vaxmate,"VAXMATE")
Txt_Fun(Txt_Pc100,"PC100")
Txt_Fun(Txt_Ibmat339,"IBMAT-339")
Txt_Fun(Txt_Ibmxt286,"IBMXT-286")
Txt_Fun(Txt_Ps250,"PS/2-50")
Txt_Fun(Txt_Ps260,"PS/2-60")
Txt_Fun(Txt_Ibmxtii,"IBMXT-II")
Txt_Fun(Txt_Ps230,"PS/2-30")
Txt_Fun(Txt_Ps280,"PS/2-80")
Txt_Fun(Txt_Tandy,"TANDY1400")
Txt_Fun(Txt_Vt100,"VT100")
Txt_Fun(Txt_Vt200,"VT200")
Txt_Fun(Txt_Vt300,"VT300")
Txt_Fun(Txt_Vt52,"VT52")
Txt_Fun(Txt_Vk100,"VK100")
Txt_Fun(Txt_ANSI,"ANSI")
Txt_Fun(Txt_tcap,"TERMCAP")
Txt_Fun(Txt_Cursorline,"CURSORLINE")
Txt_Fun(Txt_Fixed,"FIXED")
Txt_Fun(Txt_Floating,"FLOATING")
Txt_Fun(Txt_Cursor,"CURSOR")
Txt_Fun(Txt_Free,"FREE")
Txt_Fun(Txt_Restricted,"RESTRICTED")
Txt_Fun(Txt_Journaling,"JOURNALING")
Txt_Fun(Txt_On,"ON")
Txt_Fun(Txt_Off,"OFF")
Txt_Fun(Txt_Controlz,"CONTROLZ")
Txt_Fun(Txt_Backupfile,"BACKUPFILE")
Txt_Fun(Txt_Return,"RETURN")
Txt_Fun(Txt_Screenshift,"SCREENSHIFT")
Txt_Fun(Txt_Ins_Defs,"DEFINITIONS")
Txt_Fun(Txt_Fontsize,"FONTSIZE")
Txt_Fun(Txt_Maxscroll,"MAXSCROLL")
Txt_Fun(Txt_Gold,"GOLD")
Txt_Fun(Txt_Ega43,"EGA43")
Txt_Fun(Txt_Palette,"PALETTE")
Txt_Fun(Txt_Video,"VIDEO")
Txt_Fun(Txt_Bios,"BIOS")
Txt_Fun(Txt_Direct,"DIRECT")
Txt_Fun(Txt_Tabs,"TABS")
Txt_Fun(Txt_Inserted,"INSERTED")
Txt_Fun(Txt_Expanded,"EXPANDED")
Txt_Fun(Txt_Ruler,"RULER")
Txt_Fun(Txt_Mapping,"MAPPING")
Txt_Fun(Txt_Old,"OLD")
Txt_Fun(Txt_Edt,"EDT")
Txt_Fun(Txt_Wps,"WPS")
Txt_Fun(Txt_Scancode,"SCANCODE")
Txt_Fun(Txt_Numlock,"NUMLOCK")
Txt_Fun(Txt_Disabled,"DISABLED")
Txt_Fun(Txt_Enabled,"ENABLED")
Txt_Fun(Txt_Set,"SET")
Txt_Fun(Txt_Cleared,"CLEARED")
Txt_Fun(Txt_Mode,"MODE")
Txt_Fun(Txt_Context,"CONTEXT")
Txt_Fun(Txt_Saved,"SAVED")
Txt_Fun(Txt_Forgotten,"FORGOTTEN")
Txt_Fun(Txt_Check,"CHECK")
Txt_Fun(Txt_Column,"COLUMN")
Txt_Fun(Txt_Displayed,"DISPLAYED")
Txt_Fun(Txt_Mouse,"MOUSE")
Txt_Fun(Txt_Remember,"REMEMBER")
Txt_Fun(Txt_Firmware,"FIRMWARE")
Txt_Fun(Txt_Newint16,"NEWINT16")
Txt_Fun(Txt_Oldint16,"OLDINT16")
Txt_Fun(Txt_Autosave,"AUTOSAVE")
Txt_Fun(Txt_Buffering,"BUFFERING")
Txt_Fun(Txt_Freemem,"FREE")
Txt_Fun(Txt_Nofreemem,"NOFREE")
Txt_Fun(Txt_EMS,"EMS")
Txt_Fun(Txt_NoEMS,"NOEMS")
Txt_Fun(Txt_Share,"SHAREMESSAGE")
Txt_Fun(Txt_Dialog,"DIALOG")
Txt_Fun(Txt_Caps_Lock,"CAPSLOCK")
Txt_Fun(Txt_Normal,"NORMAL")
Txt_Fun(Txt_Debug,"DEBUG")
Txt_Fun(Txt_Windows,"WINDOWS")
Txt_Fun(Txt_Zoomed,"ZOOMED")
Txt_Fun(Txt_Overlapped,"OVERLAPPED")
Txt_Fun(Txt_Searches,"SEARCHES")
Txt_Fun(Txt_Literal,"LITERAL")
Txt_Fun(Txt_Regular,"REGULAR")
Txt_Fun(Txt_Terminal,"TERMINAL")
Txt_Fun(Txt_Files,"FILES")
Txt_Fun(Txt_Reservation,"RESERVATION")
Txt_Fun(Txt_Margin,"MARGIN")

Txt_Fun(Arg_Error_Txt,"Illegal argument")
Txt_Fun(Conf_Synt_Txt,"Syntax error in configuration file")
Txt_Fun(Bel_Conf_Synt_Txt,"\7Syntax error in configuration file")
Txt_Fun(Fil_Buff_Txt,"More files than buffers")
Txt_Fun(Txt_Done,"Done")
Txt_Fun(Severe_Txt,"Severe error|Check bottom line of window")
Txt_Fun(Version_Txt,"Sedt (Screen EDiTor) Version ")
Txt_Fun(Small_Txt," (Small memory configuration)")
Txt_Fun(Device_Txt,"Device: ")
Txt_Fun(Keyboard_Txt,", Keyboard: ")
Txt_Fun(Display_Txt,", Display: ")
Txt_Fun(Color_Txt,"Color")
Txt_Fun(Mono_Txt,"Monochrome")
Txt_Fun(No_Key_Txt,"No key definition file: %s")
Txt_Fun(No_Map_Txt,"No key map file: %s")
Txt_Fun(Copyright_Txt,"Copyright (c) Anker Berg-Sonne, 1985-1991")
Txt_Fun(No_Init_Txt,"Could not open initialization file %s")
Txt_Fun(No_Ruler_Txt,"No ruler file: %s")
Txt_Fun(Ill_Ruler_Txt,"Illegal contents of ruler file")
Txt_Fun(Txt_Eof,"Eof")
Txt_Fun(Txt_Bof,"Bof")
Txt_Fun(Txt_New_File,"New file")
Txt_Fun(Txt_Loading,"Loading...")
Txt_Fun(Dir_Load_Txt,"Cannot load from directory")
Txt_Fun(Spec_Load_Txt,"Cannot load from special file")
Txt_Fun(No_Delimiter_Txt,"Missing delimiter")
Txt_Fun(No_Token_Txt,"Missing token")
Txt_Fun(Illegal_Token_Txt,"Illegal token")
Txt_Fun(Txt_Cancelled,"Cancelled")
Txt_Fun(Unprogrammed_Txt,"Unprogrammed Key")
Txt_Fun(Bad_Key_Txt,"Bad key definition: %s")
Txt_Fun(Bad_Key_Num_Txt,"Bad key number: %s")
Txt_Fun(Double_Def_Txt,"Key defined twice in key definition file: %s")
Txt_Fun(Buffer_Err_Txt,"Error reading data from buffer")
Txt_Fun(Buffer_Wr_Err_Txt,"Error writing data from buffer")
Txt_Fun(Max_Err_Txt,"Exceeded maximum size, use -s switch")
Txt_Fun(Dsk_Full_Err_Txt,"Could not create overflow file on disk")
Txt_Fun(Txt_Working,"Working...")
Txt_Fun(Rel_Bad_Txt,"Attempt to get before beginning")
Txt_Fun(No_Memory_Txt,"Not enough memory")
Txt_Fun(Mouse_Toggle_Txt,"Click mouse on toggle displays")
Txt_Fun(Mouse_Window_Txt,"Click mouse within window")
Txt_Fun(No_Termcap_Txt,"No termcap entry for %s\n")
Txt_Fun(Unrecognized_Terminal_Txt,"Unrecognized terminal. Set to VT2XX")
Txt_Fun(Illegal_Keystroke_Txt,"Illegal keystroke")
Txt_Fun(Undefined_Escape_Txt,"Undefined escape sequence")
Txt_Fun(Yes_No_Txt," <Y/N>: ")
Txt_Fun(No_File_Txt,"No file to save")
Txt_Fun(Unsaved_Chng_Txt,"Unsaved changes. Quit?")
Txt_Fun(This_Unsaved_Txt,"Unsaved changes in this buffer. Save?")
Txt_Fun(Illegal_Window_Txt,"Illegal window number")
Txt_Fun(Illegal_ASCII_Txt,"Illegal ASCII character for insert")
Txt_Fun(Too_Long_Arg_Txt,"Too long command argument")
Txt_Fun(File_Not_Found_Txt,"Could not find file")
Txt_Fun(Unsaved_Chng_Sure_Txt,"Unsaved changes. Are you sure?")
Txt_Fun(Illegal_Mark_Txt,"Illegal mark number")
Txt_Fun(Help_Menu_Txt," Type Space, Return, Backspace or Menu selection")
Txt_Fun(Txt_More,"More..")
Txt_Fun(Txt_End,"End...")
Txt_Fun(No_Help_File_Txt,"No help file: %s")
Txt_Fun(Forget_Buffer_Txt,"Forget contents of buffer?")
Txt_Fun(Save_Changes_Txt,"Save changes to current file?")
Txt_Fun(No_Context_Txt,"Could not open context file")
Txt_Fun(Read_Context_Txt,"Could not read context file")
Txt_Fun(Context_Match_Txt,"Context file does not match input file")
Txt_Fun(Bad_Context_Cursor_Txt,"Bad cursor position in context file")
Txt_Fun(No_Output_File_Txt,"No output file")
Txt_Fun(Cannot_Undo_Txt,"Last command cannot be undone")
Txt_Fun(Show_Column_Txt,"Column=%ld")
Txt_Fun(Illegal_Ruler_Txt,"Illegal ruler number")
Txt_Fun(No_Contxt_Name_Txt,
 "Buffer needs to have a file name for a context save")
Txt_Fun(Cannot_Create_Context_Txt,"Could not create context file")
Txt_Fun(Learned_Input_Txt,"Contents of this key learned input")
Txt_Fun(Txt_Saving,"Saving...")
Txt_Fun(Cannot_Create_File_Txt,"Could not create output file")
Txt_Fun(No_Window_Txt,"Could not create new window")
Txt_Fun(End_Region_Txt,"End point must be edit region")
Txt_Fun(Dir_Save_Txt,"Cannot save into directory")
Txt_Fun(Incomplete_Command_Txt,"Incomplete command")
Txt_Fun(Syntax_Error_Txt,"Command syntax error")
Txt_Fun(Unknown_Command_Txt,"Unknown command")
Txt_Fun(Write_Error_Txt,"Error writing file")
Txt_Fun(Block_Sel_Txt,
 "Select point must be upper left or lower right corner in block mode")
Txt_Fun(Txt_Read_Only,"Buffer is read only, do you really want to save?")
Txt_Fun(No_Messages_Txt,"No more broadcast messages")
Txt_Fun(Max_Func_Txt,"No more room for function definitions")
Txt_Fun(Txt_Bad_Version,"Version incompatibility with key map file")
Txt_Fun(Reading_Help_Txt,"Reading help files")
Txt_Fun(Gold_Txt,"Gold ")
Txt_Fun(Map_Create_Error,"Error creating map file")
Txt_Fun(Fun_Create_Error,"Error creating function file")
Txt_Fun(Ascii_Value_Txt,"ASCII value %d")
Txt_Fun(Col_Ascii_Value_Txt,"Column=%ld, ASCII value=%d")
Txt_Fun(Bad_Argument_Txt,"Illegal argument value")
Txt_Fun(Bad_Window_Pos_Txt,"Illegal window position")
Txt_Fun(Last_Not_Search_Txt,"Last command not search")
