#if defined(DOS) & !defined(Windows)
Txt_Fun(Write_Protect_Txt,"DOS error:Write protected disk, retry?")
Txt_Fun(Unknown_Unit_Txt,"DOS error:Unknown unit, retry?")
Txt_Fun(Drive_N_Ready_Txt,"DOS error:Drive not ready, retry?")
Txt_Fun(Unknown_Com_Txt,"DOS error:Unknown command, retry?")
Txt_Fun(Data_Error_Txt,"DOS error:Data error (CRC), retry?")
Txt_Fun(Bad_Request_Txt,"DOS error:Bad request structure length, retry?")
Txt_Fun(Seek_Error_Txt,"DOS error:Seek error, retry?")
Txt_Fun(Unknown_Media_Txt,"DOS error:Unknown media, retry?")
Txt_Fun(Sector_N_Found_Txt,"DOS error:Sector not found, retry?")
Txt_Fun(No_Paper_Txt,"DOS error:Printer out of paper, retry?")
Txt_Fun(Write_Fault_Txt,"DOS error:Write fault, retry?")
Txt_Fun(Read_Fault_Txt,"DOS error:Read fault, retry?")
Txt_Fun(General_Txt,"DOS error:General falure, aborting")
Txt_Fun(Dos_Error_Txt,"DOS error, retry?")
Txt_Fun(Crit_Retry_Txt,"Operation being retried")
Txt_Fun(Crit_Fail_Txt,"Failure status returned to DOS")
#endif

/*Month abbreviations for date display*/
Txt_Fun(January_Txt,"Jan")
Txt_Fun(February_Txt,"Feb")
Txt_Fun(March_Txt,"Mar")
Txt_Fun(April_Txt,"Apr")
Txt_Fun(May_Txt,"May")
Txt_Fun(June_Txt,"Jun")
Txt_Fun(July_Txt,"Jul")
Txt_Fun(August_Txt,"Aug")
Txt_Fun(September_Txt,"Sep")
Txt_Fun(October_Txt,"Oct")
Txt_Fun(November_Txt,"Nov")
Txt_Fun(December_Txt,"Dec")

Txt_Fun(Illegal_Char_Txt,"Illegal character")
Txt_Fun(Writing_Ruler_Txt,"Error closing ruler file")
Txt_Fun(Left_Margin_Txt,"Left margin must be left of right margin")
Txt_Fun(Word_Wrap_Txt,"Word wrap column must be left of right margin")
Txt_Fun(Right_Margin_Txt,
 "Right margin must be right of left margin and word wrap column")
Txt_Fun(No_Right_Margin_Txt,"Right margin not set")
Txt_Fun(Empty_Line_Txt,"Empty line")
Txt_Fun(No_Command_Txt,"No command supplied")
Txt_Fun(No_Compec_Txt,"COMSPEC Environment variable needed for spawn")
Txt_Fun(No_Spawn_Txt,"Error spawning process")
Txt_Fun(Return_To_Sedt_Txt,"\nPress any key to return to Sedt")
Txt_Fun(Spawn_Error_Txt,"Spawned process returned error = %d")
Txt_Fun(No_Stdout_Txt,"Could not open stdout file")
Txt_Fun(No_Stderr_Txt,"Could not open stderr file")
Txt_Fun(Not_Search_Txt,"Last command not search")
Txt_Fun(No_Print_Txt,"Could not open print file")
Txt_Fun(Txt_Printing,"Printing")
Txt_Fun(Abort_Print_Txt,"Print aborted by input")
Txt_Fun(Command_File_Txt,"Could not find command file")
Txt_Fun(Block_Append_Txt,"Cannot append to paste buffer in block mode")
Txt_Fun(Paste_File_Txt,"Could not open paste file")
Txt_Fun(Paste_Create_Txt,"Could not create paste file")
Txt_Fun(Already_Learning_Txt,"Already learning")
Txt_Fun(Null_String_Txt,"Null execute string")
Txt_Fun(Ruler_Syntax_Txt,"Syntax error in ruler definition")
Txt_Fun(Key_Create_Txt,"Could not create key definition file")
Txt_Fun(Learn_Key_Txt,"Key does not contain learned keystrokes")
Txt_Fun(No_Number_Txt,"No number at cursor position")
Txt_Fun(No_Sel_Txt,"No select point")
Txt_Fun(No_Mark_Txt,"Mark not set")
Txt_Fun(Last_Mark_Txt,"No more marks in this direction")
Txt_Fun(No_Match_Txt,"No character to match")
Txt_Fun(Missing_Match_Txt,"No matching character")
Txt_Fun(Edit_Region_End_Txt,"End of edit region reached")
Txt_Fun(Buffer_End_Txt,"End of buffer reached")
Txt_Fun(Edit_Region_Begin_Txt,"Beginning of edit region reached")
Txt_Fun(Buffer_Beginning_Txt,"Beginning of buffer reached")
Txt_Fun(Bad_Entity_Txt,"Unknown entity")
Txt_Fun(No_Buffer_Txt,"No buffer to change to")
Txt_Fun(To_Deep_Ex_Txt,"Too many macros being executed")
Txt_Fun(Replace_This_Txt,"Replace this ocurrence")

#if defined(VMS) | defined(UNIX)
Txt_Fun(Ill_Mouse_Mess,"Illegal character in mouse sequence: %c") /* 290a */
#endif

#if defined(DOS) | defined(UNIX) | defined(WIN32)
Txt_Fun(No_Stat_Txt,"Unable to obtain file stat")
Txt_Fun(File_Reserved_Txt,"File is write protected and probably reserved, load anyway?")
Txt_Fun(Out_File_Reserved_Txt,"Output file is write protected and probably reserved, load anyway?")
Txt_Fun(No_Protection_Change_Txt,"Failed to change file protection")
Txt_Fun(Reserved_Txt,"File reserved")
Txt_Fun(Group_Txt,"Cannot reserve file. Group member, but not owner")
Txt_Fun(Other_Txt,"Cannot reserve file. Only other access")
Txt_Fun(No_Reset_Txt,"Unable to reset file protection")
Txt_Fun(Reset_Txt,"File protection reset")
#endif

#if (defined(DOS) | defined(NT)) & defined(Windows)
Txt_Fun(Txt_Untitled,"Untitled")
Txt_Fun(No_Mouse_Txt,"Mouse and menu ignored when displaying help screens")
Txt_Fun(Txt_Warning,"Warning")
Txt_Fun(Txt_Sedt,"Sedt - ")
Txt_Fun(Txt_Untitled_Bracket,"(Untitled)")
Txt_Fun(No_Clipboard_Txt,"Could not open clipboard")
Txt_Fun(Bad_Clipboard_Txt,"Could not empty clipboard")
Txt_Fun(No_Clipboard_Memory_Txt,"Could not allocate memory for clipboard data")
Txt_Fun(Empty_Clipboard_Txt,"Clipboard empty")
Txt_Fun(Txt_Unused,"Unused")
Txt_Fun(Font_Size_Txt,"Current font size=%d")
#endif

#if defined(ATARI) & defined(GEM)
Txt_Fun(Txt_Untitled,"Untitled")
Txt_Fun(Editor_Txt,"Sedt Editor")
Txt_Fun(Resource_Txt,"[3][Resource file not found][OK]")
Txt_Fun(No_Mouse_Txt,"Mouse and menus cannot be used when displaying help")
Txt_Fun(G_Version_Txt,"[0][ Sedt editor, version ")
Txt_Fun(G_OK_Txt," ][OK]")
Txt_Fun(G_Unused_Txt,"Unused")
Txt_Fun(Yes_No_Cancel_Txt,"][Yes|No|Cancel]")
Txt_Fun(G_Continue_Txt,"][Continue]")
Txt_Fun(G_Font_Txt,"Font size=%d")
#endif

Txt_Fun(Not_Implemented_Txt,"Function not implemented in this version")
Txt_Fun(Get_File_Txt,":FE~?/File: /~")
Txt_Fun(Get_Char_Txt,"?/Character position: /@AC")
Txt_Fun(Get_Line_Txt,"?/Line number: /@AL")
Txt_Fun(Get_Buf_Txt,"?/Buffer: /:WI")
Txt_Fun(Not_Enough_Memory_Txt,"Not enough memory")
Txt_Fun(Txt_Auto_Warn,"File about to be checkpointed")
Txt_Fun(Unsupported_Term_Txt,"Unsupported terminal type: ")
Txt_Fun(Txt_Got_Auto,"Autosaved copy exists, load it instead?")
Txt_Fun(Unassigned_Txt,"Unassigned function ")
Txt_Fun(Txt_Zoomed_Error,"Cannot change a zoomed window")

#if defined(DOS) & !defined(Windows)
Txt_Fun(Menu_Command_Txt,"Command")
Txt_Fun(Menu_Move_Txt,"Move")
Txt_Fun(Menu_Advance_Txt,"Default move forward")
Txt_Fun(Menu_Backup_Txt,"Default move reverse")
Txt_Fun(Menu_Curs_Bottom_Txt,"Cursor at bottom of screen")
Txt_Fun(Menu_Abs_Char,"Absolute character position")
Txt_Fun(Menu_Ask_Abs_Char,"Position:")
Txt_Fun(Menu_Abs_Line,"Absolute line number")
Txt_Fun(Menu_Ask_Abs_Line,"Line number:")
Txt_Fun(Menu_Top,"Top of buffer")
Txt_Fun(Menu_Match_Bracket,"Matching bracket character")
Txt_Fun(Menu_Bottom,"Bottom of buffer")
Txt_Fun(Menu_Line_End,"End of line")
Txt_Fun(Menu_Ask_Line_End,"Lines:")
Txt_Fun(Menu_Char,"Character")
Txt_Fun(Menu_Ask_Char,"Characters:")
Txt_Fun(Menu_Find,"Find string")
Txt_Fun(Menu_Ask_Find,"Occurrences:")
Txt_Fun(Menu_Line,"Line")
Txt_Fun(Menu_Ask_Line,"Lines:")
Txt_Fun(Menu_Get_Mark,"Mark")
Txt_Fun(Menu_Ask_Mark,"Number:")
Txt_Fun(Menu_Find_Next,"Find same string")
Txt_Fun(Menu_Ask_Find_Next,"Occurrences:")
Txt_Fun(Menu_Page,"Page")
Txt_Fun(Menu_Ask_Page,"Pages:")
Txt_Fun(Menu_Par,"Paragraph")
Txt_Fun(Menu_Ask_Par,"Paragraphs:")
Txt_Fun(Menu_Screen,"Screen")
Txt_Fun(Menu_Ask_Screen,"Screens:")
Txt_Fun(Menu_Sentence,"Sentence")
Txt_Fun(Menu_Ask_Sentence,"Sentences:")
Txt_Fun(Menu_Sel_Range,"Select range")
Txt_Fun(Menu_Vertical,"Vertical")
Txt_Fun(Menu_Ask_Vertical,"Lines:")
Txt_Fun(Menu_Word,"Word")
Txt_Fun(Menu_Ask_Word,"Words:")
Txt_Fun(Menu_Next_Mark,"Next mark")
Txt_Fun(Menu_Ch_Case,"Switch case")
Txt_Fun(Menu_Ch_Lower,"Change to lower case")
Txt_Fun(Menu_Ascii,"Insert Ascii character")
Txt_Fun(Menu_Check_Point,"Checkpoint file")
Txt_Fun(Menu_Center_Line,"Center text")
Txt_Fun(Menu_Curs_Middle,"Cursor in middle of screen")
Txt_Fun(Menu_Counter_Add,"Add value from text to counter")
Txt_Fun(Menu_Counter_Get,"Get counter from text")
Txt_Fun(Menu_Counter_Increment,"Increment counter")
Txt_Fun(Menu_Counter_Put,"Insert counter value")
Txt_Fun(Menu_Counter_Set,"Set counter value")
Txt_Fun(Menu_Context_Save,"Save file context")
Txt_Fun(Menu_Curs_Top,"Cursor at top of screen")
Txt_Fun(Menu_Ch_Upper,"Change to upper case")
Txt_Fun(Menu_Change_Window,"Switch to previous window")
Txt_Fun(Menu_Del_It,"Delete")
Txt_Fun(Menu_Del_Append,"Delete and append to delete buffer")
Txt_Fun(Menu_Execute_Function,"Execute function")
Txt_Fun(Menu_Execute_Key,"Execute key")
Txt_Fun(Menu_Err_Level,"Set error level")
Txt_Fun(Menu_Cancel_Edit_Region,"Cancel edit region")
Txt_Fun(Menu_Set_Edit_Region,"Set edit region")
Txt_Fun(Menu_Def_Function,"Define function")
Txt_Fun(Menu_File_Edit,"Edit file")
Txt_Fun(Menu_File_Get,"Insert file contents")
Txt_Fun(Menu_Font_Larger,"Increase font size")
Txt_Fun(Menu_Font_Smaller,"Decrease font size")
Txt_Fun(Menu_File_Save,"Save file")
Txt_Fun(Menu_Fill,"Adjust margins")
Txt_Fun(Menu_File_Write,"Write buffer to file")
Txt_Fun(Menu_File_Execute,"Execute Sedt commands in file")
Txt_Fun(Menu_Get_Append,"Append to delete buffer")
Txt_Fun(Menu_Get,"Copy to delete buffer")
Txt_Fun(Menu_Get_Ruler,"Read ruler from disk")
Txt_Fun(Menu_Help,"Help")
Txt_Fun(Menu_Insert,"Insert text")
Txt_Fun(Menu_Def_Key_Alpha,"Define alphanumeric key")
Txt_Fun(Menu_Def_Key_Function,"Define function key")
Txt_Fun(Menu_Def_Key_G_Alpha,"Define gold alphanumeric key")
Txt_Fun(Menu_Def_Key_G_Key,"Define gold numbered key")
Txt_Fun(Menu_Def_Key_Key,"Define numbered key")
Txt_Fun(Menu_Def_Key_Number,"Map key to function")
Txt_Fun(Menu_Ind_Decrease,"Increase indentation level")
Txt_Fun(Menu_Ind_Increase,"Decrease indentation level")
Txt_Fun(Menu_Load_Key,"Load key definition")
Txt_Fun(Menu_Ex_Learn,"Execute learned keystrokes")
Txt_Fun(Menu_Load_Learn,"Learn keystrokes")
Txt_Fun(Menu_End_Learn,"End learning")
Txt_Fun(Menu_Ind_Set,"Set indentation level")
Txt_Fun(Menu_Mark_Pos,"Set mark")
Txt_Fun(Menu_A_New_Line,"New line")
Txt_Fun(Menu_Subs_Next,"Replace next occurrence")
Txt_Fun(Menu_Window_Used,"Next used window")
Txt_Fun(Menu_Window_Changed,"Next modified window")
Txt_Fun(Menu_Window_Cycle,"Next window")
Txt_Fun(Menu_Window_Not_Used,"Next unused window")
Txt_Fun(Menu_Out_Error,"Display error message")
Txt_Fun(Menu_Open_Line,"Open line")
Txt_Fun(Menu_Out_Message,"Display message")
Txt_Fun(Menu_Overstrike_Mode,"Toggle overstrike mode")
Txt_Fun(Menu_Other_Window,"Previous window")
Txt_Fun(Menu_Put_Date,"Insert date")
Txt_Fun(Menu_Past_Load,"Load paste buffer from file")
Txt_Fun(Menu_Pop_Changes,"Pop change flag")
Txt_Fun(Menu_Print,"Print buffer")
Txt_Fun(Menu_Past_Save,"Save paste buffer in file")
Txt_Fun(Menu_Put_Time,"Insert time")
Txt_Fun(Menu_Push_Changes,"Push change flag")
Txt_Fun(Menu_Quit,"Quit")
Txt_Fun(Menu_Repl,"Replace search string with paste buffer")
Txt_Fun(Menu_Redefine_Key,"Redefine key")
Txt_Fun(Menu_Refresh,"Refresh screen")
Txt_Fun(Menu_Ruler,"Enter ruler mode")
Txt_Fun(Menu_Remove_Char,"Clear character delete buffer")
Txt_Fun(Menu_Remove_Help,"Clear help text buffer")
Txt_Fun(Menu_Remove_Line,"Clear line delete buffer")
Txt_Fun(Menu_Remove_Page,"Clear page delete buffer")
Txt_Fun(Menu_Remove_Par,"Clear paragraph delete buffer")
Txt_Fun(Menu_Remove_Paste,"Clear paste buffer")
Txt_Fun(Menu_Remove_Search,"Clear search string")
Txt_Fun(Menu_Remove_Sent,"Clear sentence delete buffer")
Txt_Fun(Menu_Remove_Subs,"Clear replace string")
Txt_Fun(Menu_Remove_Unknown,"Clear other delete buffer")
Txt_Fun(Menu_Remove_Word,"Clear word delete buffer")
Txt_Fun(Menu_Reset,"Reset select region")
Txt_Fun(Menu_Remove_Tabs,"Convert tab characters to spaces")
Txt_Fun(Menu_Rul_Load,"Load ruler from buffer")
Txt_Fun(Menu_Rul_Save,"Save ruler in buffer")
Txt_Fun(Menu_Show_Ascii,"Insert ASCII character")
Txt_Fun(Menu_Save_Keys,"Save key definitions in file")
Txt_Fun(Menu_Save_Functions,"Save function definitions in file")
Txt_Fun(Menu_Show_Bcast,"Show VMS broadcast messages")
Txt_Fun(Menu_Show_Column,"Show current column")
Txt_Fun(Menu_Select,"Set end of select region")
Txt_Fun(Menu_Set_Block,"Set block/line mode")
Txt_Fun(Menu_Set_Direction,"Set default direction")
Txt_Fun(Menu_Set_Cursor_Free,"Set free/restricted cursor movement")
Txt_Fun(Menu_Set_Replace,"Set insert/replace mode")
Txt_Fun(Menu_Set_Tab_Expand,"Set tab expand/insert mode")
Txt_Fun(Menu_Set_Case_Sensitive,"Set search case sensitivity")
Txt_Fun(Menu_Set_Ins_Defs,"Set key expansion in prompts")
Txt_Fun(Menu_Shift_In,"Shift in")
Txt_Fun(Menu_Sav_Key,"Insert key definition in buffer")
Txt_Fun(Menu_Search_Load,"Load search string from file")
Txt_Fun(Menu_Shift_Out,"Shift out")
Txt_Fun(Menu_Set_Tabs,"Convert spaces to tab characters")
Txt_Fun(Menu_Strip_Trailing,"Strip trailing spaces")
Txt_Fun(Menu_Subs,"Replace string by another")
Txt_Fun(Menu_Show_Version,"Show Sedt version")
Txt_Fun(Menu_Spawn,"Execute an operating system command")
Txt_Fun(Menu_Q_Spawn,"Operating system command with no output")
Txt_Fun(Menu_R_Spawn,"Operating system command with redirected output")
Txt_Fun(Menu_Tab,"Insert a tab character")
Txt_Fun(Menu_Adj_Tabs,"Shift text tab stops")
Txt_Fun(Menu_Tog_Block,"Toggle block/line mode")
Txt_Fun(Menu_Tog_Control,"Toggle control character display")
Txt_Fun(Menu_Tog_Direction,"Toggle default direction")
Txt_Fun(Menu_Cursor_Free_Toggle,"Toggle free/restricted cursor movement")
Txt_Fun(Menu_Test_Input,"Test for typeahead")
Txt_Fun(Menu_Tog_Marks,"Toggle eof/bof display")
Txt_Fun(Menu_Tog_Cursor,"Toggle cursor in center of window")
Txt_Fun(Menu_Tog_Ruler,"Toggle display of ruler")
Txt_Fun(Menu_Shift_Toggle,"Toggle automatic horizontal scrolling")
Txt_Fun(Menu_Tab_Toggle,"Toggle tab character expansion/deletion")
Txt_Fun(Menu_Case_Toggle,"Toggle search case sensitivity")
Txt_Fun(Menu_Ins_Defs_Toggle,"Toggle key expansion in prompts")
Txt_Fun(Menu_Und_Char,"Insert character delete buffer")
Txt_Fun(Menu_Und_Line,"Insert line delete buffer")
Txt_Fun(Menu_Undo,"Undo command")
Txt_Fun(Menu_Update_On,"Update screen")
Txt_Fun(Menu_Update_Off,"Suppress screen updates")
Txt_Fun(Menu_Und_Pg,"Insert page delete buffer")
Txt_Fun(Menu_Und_Par,"Insert paragraph delete buffer")
Txt_Fun(Menu_Upd_Screen,"Update window header")
Txt_Fun(Menu_Und_Sen,"Insert sentence delete buffer")
Txt_Fun(Menu_Paste,"Insert paste buffer")
Txt_Fun(Menu_Und_Unknown,"Insert other delete buffer")
Txt_Fun(Menu_Und_Word,"Insert word delete buffer")
Txt_Fun(Menu_Window_Grow,"Increase window size")
Txt_Fun(Menu_Window,"Switch to window")
Txt_Fun(Menu_Window_Move,"Move window")
Txt_Fun(Menu_Window_Shrink,"Shrink window")
Txt_Fun(Menu_Exit_Sedt,"Save file and exit")
Txt_Fun(Menu_Query,"Query")
Txt_Fun(Menu_Zero,"Clear buffer")
Txt_Fun(Menu_Options,"Options")
Txt_Fun(Menu_Display,"Display")
Txt_Fun(Menu_Delete,"Delete")
Txt_Fun(Menu_Formatting,"Text format")
Txt_Fun(Menu_Insertion,"Insert")
Txt_Fun(Menu_Counters,"Counters")
Txt_Fun(Menu_File,"File")
Txt_Fun(Grow_Txt,"Use arrow keys to change window, type return when done")
#endif

Chr_Fun(Chr_Y,'Y')
Chr_Fun(Chr_N,'N')

Txt_Fun(License_Txt,
 "Shareware program, proper license required. Read LICENSE.TXT")
Txt_Fun(Share_1_Txt,
"                           Sedt is a shareware program")
Txt_Fun(Share_2_Txt,
"")
Txt_Fun(Share_3_Txt,
"   Continued support and development depends on your $25 license fee sent to:")
Txt_Fun(Share_4_Txt,
"           Anker Berg-Sonne, 8 Middlemost Way, Stow, MA 01775, U.S.A.")
Txt_Fun(Share_5_Txt,
"")
Txt_Fun(Share_6_Txt,
"                           Thank you for your support")
