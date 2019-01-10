#if (defined(DOS) & !defined(Windows))
#if defined (__cplusplus)
#define Fast _fastcall
#else
#define Fast
#endif
#else
#define Fast
#endif
/*SEDT.C*/
void Fast VKS_Quit(void);
void Fast VKS_Exit(void);
void  CCint();
int  main(int  Argc,char  * *Argv);
void  Fast Write_Rem(void);
void  Fast Sedt_Init(void);
void  Fast Init_Buffer(void);
#if (defined(DOS) | defined(NT)) & defined(Windows)
void  Sedt_Main(void);
#else
void  Fast Sedt_Main(void);
#endif
void  Fast Sedt_Cleanup(void);
void  Fast Sedt_Shutdown(void);
void  Fast S_Header(void);
void  Fast Upd_Header(void);
void  Fast Skip_To(int  C);
void  Fast Put_I(long  I);
void  Fast Rec_I(long  I);
void  Fast S_Ruler(void);
void  Fast Argerror(void);
void  Fast Read_Config(void);
void  Fast Test_Config(char *S, char *T);
void  Fast Test_Files(char *T);
void  Fast Test_Searches(char *T);
void  Fast Test_Mode(char *T);
void  Fast Test_Screenshift(char *T);
void  Fast Test_Mapping(char *T);
void  Fast Test_System(char *T);
void  Fast Test_Keyboard(char *T);
void  Fast Test_Palette(char *T);
void  Fast Test_Auto(char *T);
void  Fast Cnf_Error(void);
void  Fast Sedt_Args(int  Argc,char  * *Argv);
void  Fast Test_Args(char *P);
void  Fast Sedt_Files(int  Argc,char  * *Argv);
void  Fast Initerror(char  *S);
#if defined(DOS) & !defined(Windows) & !defined(PC100)
#if defined(__cplusplus)
extern "C" void OURKBD(void);
extern "C" void KEYREL(void);
extern "C" int GETCAPS(void);
#else
void OURKBD(void);
void KEYREL(void);
int GETCAPS(void);
#endif
#endif
/*EDIT.C*/
void  Fast Reset_Width(void);
void  Fast Editor(void);
void  Fast Readruler(char  C);
void  Fast Fill_Rest(void);
void  Fast Fill_Screen(int  First,int  Last);
void  Fast Read_File(char  *N);
void  Fast Filnam(char  *N,char  *T);
void  Fast Filtyp(char  *N,char  *T);
void  Fast Filjnam(char  *N,char  *T);
void  Fast Fildir(char  *N,char  *T);
void  Fast Include_File(char  *N,char  *E);
#if defined(DOS) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(NT) | defined(WIN32)
void  Error(int T, char *S,...);
#else
void  Error(int T, char *S,int P1,int P2,int P3,int P4,int P5,int P6,int P7,\
int P8,int P9);
#endif
void  Fast Plain_Error(int  L,char  *E);
#if defined(DOS) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(NT) | defined(WIN32)
void  Message(char *S,...);
#else
void  Message(char *S,int P1,int P2,int P3,int P4,int P5,int P6,int P7,\
int P8,int P9);
#endif
void  Fast Plain_Message(char  *T);
#if defined(DOS) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(NT) | defined(WIN32)
void  Diag(char *F,...);
#else
void  Diag(char *F,int P1,int P2,int P3,int P4,int P5,int P6,int P7,int P8,\
int P9);
#endif
int  Fast Pad_Put(int  What);
int  Fast Pad_Get(void);
void  Fast Get16(void);
void  Fast Clr_Error(void);
void  Fast Do_Command(void);
void  Fast Clr_Working(void);
void  Fast Ins_Char(int  C);
long  Fast Read_Count(int  C);
void  Fast Put_Count(long  Count);
char  *Find_From(char  *N,char  *S);
char  *Find_Env(char  *N,char  *S,char  *V);
char  *Find_Path(char  *N,char  *S);
#if defined(VMS)
char  *Find_Dir(char  *S1,char  *S2);
char  *Find_Lib(char  *S1,char  *S2);
char  *Logical_File(char *S);
void  Strip_Version(void);
#endif
/*KEY.C*/
void  Fast Rem_Pad(long  *Where);
void  Fast Rem_Del(long  *Where);
void  Fast Adj_Pad(long  Start,long  Chars);
void  Fast Adj_Del(long  Start,long  Chars);
void  Fast Exec_Key(int  K);
void  Fast Expand_Com(long  P);
void  Fast Set_Physical(void);
void  Fast Set_Logical(void);
void  Fast Push(int  What);
int  Fast Pop(void);
void  Fast Mark_Cursor(void);
void  Fast Unmark_Cursor(void);
int  Fast Test_Dir_Key(long  (*A)[3],int  I);
int  Fast Ins_Key(long  (*A)[3],int  I);
void  Fast Key_Get(int  I,long  (*A)[3]);
void  Fast Ins_A_Key(long  Start,long  Length);
int  Fast Readkeys(char  *N);
int  Fast Readmaps(char  *N);
int  Fast Char_Rep(int  C,char  *S);
void  Fast Erase_Del(long  *W);
long  *Key_Addr(int  Function);
#if defined(ATARI) | defined(VMS) | (defined(UNIX) & !defined(HPUX)) | defined(MPE)
void ltoa(long L,char *S,int R);
#endif
/*BUF.C*/
#if defined(DOS) | defined(OS2) | defined(NT) | defined(WIN32)
void  Fast C_Move(char  *F,char  *T,int  L);
#else
void  Fast C_Move(char  *F,char  *T,int  L,int  Overlap);
#endif
void  Fast S_Copy(char  *F,char  *T,int  L);
#if defined(Noasm) | (defined(UNIX) & !defined(SCO))
int C_Scan(char *B, int N, char C);
#else
#if defined(__cplusplus)
extern "C" C_Scan(char far *B, int N, char C);
#else
#if defined(DOS) | defined(OS2)
int C_Scan(char far *B, int N, char C);
#else
int C_Scan(char *B, int N, char C);
#endif
#endif
#endif
#if defined(DOS) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(NT) | defined(WIN32)
void  Fatalerror(char *S,...);
#else
void  Fatalerror(char *S,int P1,int P2,int P3,int P4,int P5,int P6,int P7,\
int P8,int P9);
#endif
#if defined(DOS)
#if defined(__cplusplus)
extern "C" int  Crit_Int(int  Type);
#else
int  Crit_Int(int  Type);
#endif
#else
int  Crit_Int(int  Type);
#endif
void  Fast Blockread(struct  fcb_t *F,char  *B,int  P);
void  Fast Blockwrite(struct  fcb_t *F,char  *B,int  P);
void  Fast Out_Buf(struct  Buffer *B);
void  Fast In_Buf(struct  Buffer *B);
void  Fast B_Flush(struct  Buffer *B);
void  Fast B_Rewind(struct  Buffer *B);
void  Fast B_Goto(struct  Buffer *B,long  Point);
void  Fast Out_In_Buf(struct  Buffer *B);
void  Fast B_Init(struct  Buffer *B);
void  Fast B_Reset(struct  Buffer *B);
void  Fast Zero_Buffer(struct  Buffer *B);
void  Fast B_Modify(struct  Buffer *B);
void  Fast Make_Room(struct  Buffer *B);
void  Fast B_Put(struct  Buffer *B,char  C);
void  Fast B_Put_S(struct  Buffer *T,char  *S,int  L);
void  Fast B_Repl_S(struct  Buffer *B,char  *S,int  L);
int  Fast B_Replace(struct  Buffer *B,int  C);
int  Fast B_Del(struct  Buffer *B);
int  Fast B_Get(struct  Buffer *B);
void  Fast B_Get_N(struct  Buffer *B,char  *S,int  M);
int  Fast B_Get_S(struct  Buffer *B,char  *S,int  M);
void  Fast B_Rem(struct  Buffer *B,long  L);
void  Fast B_Copy(struct  Buffer *F,struct  Buffer *T,long  L);
int  Fast B_B_Get(struct  Buffer *B);
int  Fast B_Search(struct  Buffer *B,char  C,long  E);
int  Fast B_B_Search(struct  Buffer *B,char  C,long  E);
void  Fast Mem_Return(int  M);
void  Fast Init_Buf(void);
void  Fast Reset_Buf(void);
int  Fast Mem_Alloc(unsigned  Size);
void  Fast Move_To_EMS(int  H,int  B,char  *A);
void  Fast Move_From_EMS(int  H,int  B,char  *A);
int  Fast Check_EMS(void);
int  Fast Get_EMS(void);
void  Fast Return_EMS(int  H);
#if defined(Noasm) | (defined(UNIX) & !defined(SCO))
int C_Bscan(char *B, int N, char C);
#else
#if defined(__cplusplus)
extern "C" C_Bscan(char far *B, int N, char C);
#else
#if defined(DOS) | defined(OS2)
C_Bscan(char far *B, int N, char C);
#else
C_Bscan(char *B, int N, char C);
#endif
#endif
#endif
#if defined(DOS) & !defined(Windows)
#if defined(__cplusplus)
extern "C" void M_Free(int S);
extern "C" int M_Alloc(int L);
#else
void M_Free(int S);
int M_Alloc(int L);
#endif
#endif
/*VDU.C*/
void  Fast Out_C(char  C);
void  Fast Out_I(int  I);
void  Fast Out_S(char  *S);
void  Fast Out_Zap(void);
void  Fast Opt_Init(void);
void  Fast Space_Fill(char  *Cp,int  N);
void  Fast Zero_Fill(char  *Cp,int  N);
void  Fast Opt_Cursor(int  Line,long  Col);
void  Fast Put_Char(char  C);
void  Fast Opt_Move_Cursor(int  Line,long  Col);
void  Fast Opt_Char(char  C);
void  Fast Opt_String(char  *S);
void  Fast Opt_Erase_Line(void);
void  Fast Get_Type(void);
#if defined(DOS) & !defined(Windows) & !defined(PC100)
#if defined(__cplusplus)
extern "C" void Dir_Out(int X,int Y,char far *S,char A,int L);
extern "C" void IBMpage(int P);
extern "C" int GETTYP(void);
extern "C" int pc_sys_type(void);
#else
void Dir_Out(int X,int Y,char far *S,char A,int L);
void IBMpage(int P);
int GETTYP(void);
int pc_sys_type(void);
#endif
#endif
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
#if defined(DOS)
#if defined(TV) | defined(PC100)
#if defined(__cplusplus)
extern "C" void Mouse_Int(void);
#else
void Mouse_Int(void);
#endif
#else
#if defined(__cplusplus)
extern "C" void  Mouse_Int(int  P1,int  P2,int  P3,int  P4);
#else
void  Mouse_Int(int  P1,int  P2,int  P3,int  P4);
#endif
#endif
#else
void  Mouse_Int(int  P1,int  P2,int  P3,int  P4);
#endif
#if defined(VMS) | defined(UNIX)
void Fast Mouse_Call(void);
#else
void  Fast Mouse_Call(int  Abs_Row,int  Abs_Col,int  Ldown,int  Rdown,\
int  Move);
#endif
void  Fast Mouse_Execute(int  Line,int  Col,int  Left,int  Right,int  Move);
void  Fast Set_Terminal(void);
void  Fast Sub_Set(int  H,int  W);
void  Fast Set_Cursor(int  Line,int  Col);
void  Fast Opt_Scroll(int  Scroll,int  First,int  Last);
void  Fast Scl_Up(int  Scroll);
void  Fast Scl_Down(int  Scroll);
void  Fast Clr_Screen(void);
void  Fast Reset_Screen(void);
void  Fast Set_Scroll(int  Top,int  Bottom);
void  Fast Move_Right(int  Col);
void  Fast Move_Left(int  Col);
void  Fast Opt_Force_Attributes(char  A);
void  Fast Del_To_End_Line(void);
char  *Do_Mapping(char  *S);
char  *Keyfile(void);
char  *Mapfile(void);
char  *Helpfile(void);
void  Fast Conin(void);
void  Fast Getkbd(void);
void  Fast Getkey(void);
int  Fast Next_Comb(int  Index,int  Comb_Level);
int  Fast Check_Comb(int  Index,int  Comb_Level);
void  Fast Setup_Term(void);
void  Fast Reset_Term(void);
void  Fast Wide_Screen(void);
void  Fast Narrow_Screen(void);
void  Fast Term_Error(void);
void  Fast Key_Error(void);
void  Fast Esc_Error(void);
void  Fast Mouse(int  *P1,int  *P2,int  *P3,int  *P4);
#if defined(DOS) & !defined(Windows) & !defined(PC100)
#if defined(__cplusplus)
extern "C" void Clear_Mouse(void);
extern "C" void Set_Mouse(void);
extern "C" int GETEQ(void);
extern "C" int pc_enh_keyb(void);
extern "C" void Get_Mode(int far *L,int far *W);
extern "C" void IBMcursor(int X,int Y);
extern "C" void Dir_Set(int X,int Y,char C,char A,int W);
extern "C" void IBMscrup(int S,int T,int L,int B,int R,char A);
extern "C" void IBMscrdown(int S,int T,int L,int B,int R,char A);
extern "C" void IBMcout(int W,char C,char A);
extern "C" int GETEXT(int far *S);
extern "C" int GETSCAN(int far *S);
extern "C" void Set43(void);
extern "C" int Set_Mode(int M);
extern "C" int SENDCMD(int C);
extern "C" int ANYCHR(void);
extern "C" int ANYEXT(void);
extern "C" void SETNUMLOCK(void);
extern "C" void CLRNUMLOCK(void);
extern "C" void Clr43(void);
extern "C" void RESNUMLOCK(void);
#else
void Clear_Mouse(void);
void Set_Mouse(void);
int GETEQ(void);
int pc_enh_keyb(void);
void Get_Mode(int far *L,int far *W);
void IBMcursor(int X,int Y);
void Dir_Set(int X,int Y,char C,char A,int W);
void IBMscrup(int S,int T,int L,int B,int R,char A);
void IBMscrdown(int S,int T,int L,int B,int R,char A);
void IBMcout(int W,char C,char A);
int GETEXT(int far *S);
int GETSCAN(int far *S);
void Set43(void);
Set_Mode(int M);
int SENDCMD(int C);
int ANYCHR(void);
int ANYEXT(void);
void SETNUMLOCK(void);
void CLRNUMLOCK(void);
void Clr43(void);
void RESNUMLOCK(void);
#endif
#endif
/*COM.C*/
void  Fast Sav_Set(int  *S_Line,long  *S_Col,char  *S_Attributes,int  N_Line,
 long  N_Col,char  N_Attributes);
void  Fast Set_Save(int  S_Line,long  S_Col,char  S_Attributes);
void  Fast Clr_Typeahead(void);
int  Fast Yes_No(char  *S);
void  Fast Exit_Sedt(void);
void  Fast Rem_Autosave(void);
void  Fast Quit(void);
void  Fast Tab_To(long  Col);
void  Fast A_New_Line(void);
void  Fast New_Line(long  Indent);
void  Fast Do_New_Line(int  N,long  Indent);
int  Fast Find_Margin(void);
void  Fast Del_It(void);
void  Fast Get(void);
int  Fast Get_E_Type(void);
void  Fast Advance(void);
void  Fast Backup(void);
void  Fast Select(void);
void  Fast Paste(void);
void  Fast Und_Char(void);
void  Fast Und_Word(void);
void  Fast Und_Line(void);
void  Fast Und_Par(void);
void  Fast Und_Pg(void);
void  Fast Und_Unknown(void);
void  Fast Und_Sen(void);
void  Fast Window(void);
void  Fast Ascii(void);
int  Fast Get_Arg(void);
void  Fast File_Get(void);
void  Fast Zero(void);
void  Fast Mark_Pos(void);
void  Fast End_Mark(void);
void  Fast Subs(void);
void  Fast Subs_Conf(void);
void  Fast Ins_Del(long  *W);
void  Fast Subs_Next(void);
void  Fast Subs_Next_Conf(void);
void  Fast Put_Rep(int  C);
void  Fast Insert(void);
void  Fast File_Edit(void);
void  Fast Edit_File(char  *S);
void  Fast C_Read_L(struct  File *F,long  *D);
void  Fast C_Read_I(struct  File *F,int  *D);
void  Fast Read_Context(void);
void  Fast File_Save(void);
void  Fast File_Write(void);
int  Fast End_Com(void);
void  Fast Def_Key_Function(void);
void  Fast Def_Key_Alpha(void);
void  Fast Def_Key_Key(void);
void  Fast Def_Key_G_Alpha(void);
void  Fast Def_Key_G_Key(void);
void  Fast Def_Key(void);
void  Fast Def_Key_Number(void);
void  Fast Def_Function(void);
void  Fast Reset(void);
void  Fast Undo(void);
int  Fast Input_Test(void);
void  Fast Test_Input(void);
void  Fast Tab(void);
void  Fast Overstrike_Mode(void);
void  Fast Tab_Toggle(void);
void  Fast Show_Column(void);
void  Fast Get_Ruler(void);
void  Fast Shift_Toggle(void);
void  Fast Case_Toggle(void);
void  Fast Ins_Defs_Toggle(void);
void  Fast Cursor_Free_Toggle(void);
void  Fast Change_Window(void);
void  Fast Err_Level(void);
void  Fast Tog_Marks(void);
void  Fast Tog_Block(void);
void  Fast Tog_Control(void);
void  Fast Tog_Direction(void);
void  Fast Show_Version(void);
void  Fast Tog_Cursor(void);
void  Fast Tog_Reg_Search(void);
void  Fast Tog_Ruler(void);
void  Fast Curs_Top(void);
void  Fast Curs_Bottom(void);
void  Fast Curs_Middle(void);
void  Fast Shift_In(void);
void  Fast Shift_Out(void);
void  Fast Font_Larger(void);
void  Fast Font_Smaller(void);
void  Fast Update_Off(void);
void  Fast Update_On(void);
void  Fast Remove_Char(void);
void  Fast Remove_Help(void);
void  Fast Remove_Line(void);
void  Fast Remove_Page(void);
void  Fast Remove_Par(void);
void  Fast Remove_Paste(void);
void  Fast Remove_Search(void);
void  Fast Remove_Sent(void);
void  Fast Remove_Subs(void);
void  Fast Remove_Unknown(void);
void  Fast Remove_Word(void);
void  Fast Set_Block(void);
void  Fast Set_Replace(void);
void  Fast Set_Direction(void);
void  Fast Set_Tab_Expand(void);
void  Fast Set_Case_Sensitive(void);
void  Fast Set_Reg_Search(void);
void  Fast Set_Ins_Defs(void);
void  Fast Set_Cursor_Free(void);
void  Fast Counter_Set(void);
void  Fast Counter_Put(void);
void  Fast Counter_Increment(void);
void  Fast Counter_Absolute_Value(void);
void  Fast End_Learn(void);
void  Fast Context_Save(void);
void  Fast Save_The_Context(char  *N,int  Option);
void  Fast Delete_The_Context(char  *N);
void  Fast C_Write(long  N,struct  File *F);
void  Fast Check_Point(void);
void  Fast Push_Changes(void);
void  Fast Pop_Changes(void);
void  Fast Show_Bcast(void);
void  Fast Window_Used(void);
void  Fast Window_Changed(void);
void  Fast Window_Not_Used(void);
void  Fast Window_Cycle(void);
void  Fast Redefine_Key(void);
void  Fast Show_Ascii(void);
void  Fast W_D_Border(void);
void  Fast W_S_Border(void);
void  Fast Open_Window(void);
void  Fast Refresh_Window(void);
void  Fast Window_Grow(void);
void  Fast Window_Shrink(void);
void  Fast Window_Move(void);
void  Fast Window_Zoom(void);
void  Fast Window_Tile(void);
void  Fast Window_Cascade(void);
void  Fast Window_Fix_Bottom(void);
void  Fast Window_Fix_Left(void);
void  Fast Window_Fix_Right(void);
void  Fast Window_Fix_Top(void);
long  Fast Get_Count(void);
void  Fast Com_Parse(long  S,int  Skipall);
int  Fast Check_CC(void);
void  Fast Signal_CC(void);
void  Fast Ex_Command(void);
void  Fast Move_Entity(void);
void  Fast Move_Forward(void);
void  Fast Move_Back(void);
void  Fast Goto_This(long  P);
void  Fast Paint_To_This(long  P);
void  Fast Backto_This(long  P);
void  Fast B_Save(struct  Buffer *B,char  *N);
int  Fast Put_Out(struct  File *F,char  *B,int  L);
int  Fast Get_Entity(void);
void  Fast Adj_Points(long  N,long  W);
/*COMA.C*/
void  Fast Ruler(void);
void  Fast Get_Spaces(int  C);
char  Fast Get_Word(int  C);
void  Fast Fill(void);
void  Fast Put_A_Char(int  C);
void  Fast Blind_Tab_To(long  Col);
void  Fast Do_Fill(void);
void  Fast Adj_Tabs(void);
void  Fast Center_Line(void);
void  Fast Refresh(void);
void  Fast Repaint(void);
void  Fast Width(void);
void  Fast Width80(void);
void  Fast Width132(void);
void  Fast Spawn(void);
void  Fast Q_Spawn(void);
void  Fast R_Spawn(void);
void  Fast Ch_Case(void);
void  Fast Ch_Lower(void);
void  Fast Ch_Upper(void);
void  Fast Ch_It(void);
void  Fast Repl(void);
void  Fast Print(void);
void  Fast File_Execute(void);
void  Fast Ex_File(struct  File *F);
void  Fast Ind_Set(void);
void  Fast Ind_Decrease(void);
void  Fast Ind_Increase(void);
void  Fast Key_Load(long  *Where);
void  Fast Get_Key(void);
void  Fast Load_Key(void);
void  Fast Key_Save(long  *Where);
void  Fast Sav_Key(void);
void  Fast Del_Append(void);
void  Fast Get_Append(void);
void  Fast Ch_Edt_Style(void);
void  Fast Past_Load(void);
void  Fast Past_Save(void);
void  Fast Open_Line(void);
void  Fast Upd_Screen(void);
void  Fast Load_Learn(void);
void  Fast Ex_Learn(void);
void  Fast Search_Load(void);
void  Fast Out_Message(void);
void  Fast Put_Date(void);
void  Fast Put_Time(void);
void  Fast Out_Error(void);
void  Fast Other_Window(void);
void  Fast Rul_Load(void);
void  Fast Ins_String(char  *S);
void  Fast Rul_Save(void);
void  Fast Save_Keys(void);
void  Fast Save_Functions(void);
void  Fast f_puti(FILE *F,int  I);
void  Fast Strip_Trailing(void);
void  Fast Set_Tabs(void);
void  Fast Remove_Tabs(void);
void  Fast Set_Edit_Region(void);
void  Fast Cancel_Edit_Region(void);
void  Fast Key_Mess(void);
void  Fast Clear_Region(void);
void  Fast Counter_Get(void);
void  Fast Counter_Add(void);
int  Fast Get_Counter_Index(void);
void  Fast Query(void);
void  Fast Execute_Key(void);
void  Fast Execute_Function(void);
void  Fast Diag_Error(void);
void  Fast Pos_Cursor(void);
void  Fast Set_Offset(int  I);
void  Fast Adj_Lines(long  L);
void  Fast Move_Key(long  *Where,long  Start,long  Length);
void  Fast Init_Entity(void);
void  Fast Blk_Rem_It(int  Index);
void  Fast Rem_It(int  Index);
void  Fast Wr_Buffer(char  *O_File);
void  Fast Save_Auto(char  *O_File);
void  Fast Set_Key(long  *Where,int  Value);
void  Fast Undelete(int  What);
int  Fast Chk_EL(struct  Buffer *B,int  C);
void  Fast Sav_Window(int  W);
void  Fast Load_Window(int  B);
void  Fast Newwindow(void);
void  Fast Ch_Window(int  W);
void  Fast Com_Error(void);
void  Fast Syntax_Error(void);
void  Fast Unk_Err(void);
void  Fast Wr_Error(void);
void  Fast Set_Changes(void);
void  Fast Clr_Changes(void);
void  Fast Window_Position(void);
#if defined(DOS) & !defined(Windows) & !defined(PC100)
#if defined(__cplusplus)
extern "C" void Reset_Crit(void);
extern "C" void Set_Crit(void);
#else
void Reset_Crit(void);
void Set_Crit(void);
#endif
#endif
/*COMB.C*/
void  Fast Wait_Char(void);
void  Fast Show_Help(void);
void  Fast Display_Help(long  P);
void  Fast Help(void);
void  Fast Do_Help(void);
void  Fast Wait_Mess(char  *S);
/*ENT.C*/
void  Fast regerror(char  *S);
int  Fast Chk_CRLF_F(void);
int  Fast Chk_CRLF_FB(void);
int  Fast Chk_CRLF_B(void);
int  Fast Chk_CRLF_BF(void);
void  Fast A_Char(void);
void  Fast Char(void);
void  Fast Abs_Char(void);
void  Fast Vertical(void);
void  Fast Line(void);
void  Fast Abs_Line(void);
void  Fast Top(void);
void  Fast Bottom(void);
void  Fast Find(void);
void  Fast Find_Next(void);
char  Fast Upper_Case(char  C);
char  Fast Lower_Case(char  C);
void  Fast Find_This(int  First);
void  Fast Sel_Range(void);
void  Fast Word(void);
void  Fast A_Word(void);
void  Fast Get_Mark(void);
void  Fast Next_Mark(void);
void  Fast Line_End(void);
char  Fast Skip_Spaces(char  C);
char  Fast B_Skip_Spaces(char  C);
void  Fast Par(void);
void  Fast A_Par(void);
void  Fast Sentence(void);
void  Fast A_Sentence(void);
void  Fast Page(void);
void  Fast Screen(void);
void  Fast Match_Bracket(void);
void  Fast Ex_Entity(void);
void  Fast Cnt_F_Lines(long  Pos);
void  Fast Cnt_B_Lines(long  Pos);
void  Fast Set_Region(void);
void  Fast End_Error(void);
void  Fast Beg_Error(void);
void  Fast Delim_Error(void);
void  Fast Ent_Error(void);
/*FILE.C*/
struct  File *F_open(char  *N,char  *A,struct  File *F);
int  Fast F_close(struct  File *F);
char  *F_gets(char  *S,int  N,struct  File *F);
int  Fast F_read(char  *S,int  L,int  N,struct  File *F);
int  Fast F_getc(struct  File *F);
int  Fast F_write(char  *S,int  L,int  N,struct  File *F);
int  Fast F_putc(char  C,struct  File *F);
int  Fast F_seek(struct  File *F,long  R,int  S);
/*REGEXP.C*/
struct  regexp  *regcomp(char  *exp);
char  *reg(int  paren,int  *flagp);
char  *regbranch(int  *flagp);
char  *regpiece(int  *flagp);
char  *regatom(int  *flagp);
char  *regnode(char  op);
void  Fast regc(char  b);
void  Fast reginsert(char  op,char  *opnd);
void  Fast regtail(char  *p,char  *val);
void  Fast regoptail(char  *p,char  *val);
int  Fast regexec(struct  regexp *prog,char  *string,int  No_Bline);
int  Fast regtry(struct  regexp *prog,char  *string);
int  Fast regmatch(char  *prog);
int  Fast regrepeat(char  *p);
char  *regnext(char  *p);
/*REGSUB.C*/
void  Fast regsub(struct  regexp *prog,char  *source,char  *dest);
/*WSEDT.C*/
#if defined(DOS) & defined(Windows)
#if !defined(WIN95)
int  _pascal _far _export About(HWND  hDlg,unsigned message,WORD wParam,\
LONG  lParam);
long  _pascal _far _export SedtWndProc(HWND hWnd,unsigned message,\
unsigned wParam,LONG lParam);
int  _pascal _far _export Prompt(HWND  hDlg,unsigned  message,WORD  wParam,\
LONG  lParam);
#endif
void  SedtPaint(HWND  hWnd,PAINTSTRUCT *pps,int  Is_Current);
int  SedtInit(HANDLE  hInstance);
int  Make_Window(int  B,int  Show);
void  Make_File(void);
void  Win_Scroll(int  T,int  B,int  S);
void  Win_Clear(void);
void  Win_Erase(int  L,int  C,int  W);
void  Win_On_Curs(void);
void  Win_Off_Curs(void);
void  Win_Getmem(HANDLE H,int  A,int  L);
void  Win_Putmem(int  A,HANDLE  H,int  L);
HANDLE  Win_Alloc(int  L);
void  Win_Free(HANDLE  H);
void  WinText(int  L,int  C,char  *S,int  A);
void  WinChar(void);
int  Win_Yes_No(char  *S);
void  Win_Error(char  *S);
void  Win_P_Bar(long  H,long  L);
int  Prompt_Input(void);
void  Prompt_Init(HWND  hDlg);
void  Prompt_Exit(HWND  hDlg);
int  _pascal _far _export Prompt(HWND  hDlg,unsigned  message,WORD  wParam,\
LONG  lParam);
int  Win_Newwindow(int  B, int Show);
void  Win_Topwindow(int  B);
void  Swap_Window(int  B);
void  Restore_Window(int  B);
void  Win_Name(char  *S);
void  Win_Put_Clip(long  L);
void  Win_Load_Clip(void);
void  Win_Tempfile(char  *F);
void  Win_Quit(void);
void  Win_Smaller(void);
void  Win_Larger(void);
void  Win_Font(void);
void Win_Set_Num_Lock(void);
void Win_Clear_Num_Lock(void);
void Win_Reset_Num_Lock(void);
int Open_File_Dialog(void);
int Save_As_Dialog(void);
void Print_Buffer(void);
void Print_Setup(void);
void Win_Del_Window(void);
#endif
/*NTSEDT.C*/
#if defined(NT)
int  WINAPI About(HWND  hDlg,unsigned message,WORD wParam,\
LONG  lParam);
void  SedtPaint(HWND  hWnd,PAINTSTRUCT *pps,int  Is_Current);
int  SedtInit(HANDLE  hInstance);
int  Make_Window(int  B,int  Show);
void  Make_File(void);
long  WINAPI SedtWndProc(HWND hWnd,unsigned message,\
unsigned wParam,LONG lParam);
void  Win_Scroll(int  T,int  B,int  S);
void  Win_Clear(void);
void  Win_Erase(int  L,int  C,int  W);
void  Win_On_Curs(void);
void  Win_Off_Curs(void);
void  Win_Getmem(HANDLE H,int  A,int  L);
void  Win_Putmem(int  A,HANDLE  H,int  L);
HANDLE  Win_Alloc(int  L);
void  Win_Free(HANDLE  H);
void  WinText(int  L,int  C,char  *S,int  A);
void  WinChar(void);
int  Win_Yes_No(char  *S);
void  Win_Error(char  *S);
void  Win_P_Bar(long  H,long  L);
int  Prompt_Input(void);
void  Prompt_Init(HWND  hDlg);
void  Prompt_Exit(HWND  hDlg);
int  WINAPI Prompt(HWND  hDlg,unsigned  message,WORD  wParam,\
LONG  lParam);
int  Win_Newwindow(int  B, int Show);
void  Win_Topwindow(int  B);
void  Swap_Window(int  B);
void  Restore_Window(int  B);
void  Win_Name(char  *S);
void  Win_Put_Clip(long  L);
void  Win_Load_Clip(void);
void  Win_Tempfile(char  *F);
void  Win_Quit(void);
void  Win_Smaller(void);
void  Win_Larger(void);
void  Win_Font(void);
void Win_Set_Num_Lock(void);
void Win_Clear_Num_Lock(void);
void Win_Reset_Num_Lock(void);
#endif
/*TV.C*/
#if defined(DOS) & defined(TV)
void Fast Run_Sedt(int Argc,char *Argv[]);
void* Fast TV_Newwindow(int Buffer);
void Fast TV_Top_Window(int Buffer);
void Fast TV_Error(char *T);
void Fast TV_Text(int Line,int Col,char *String,int Attributes);
void Fast TV_Clear();
void Fast TV_Erase(int Line,int Col,int Width);
void Fast TV_Char(void);
void Fast TV_Cursor(int Line,int Col);
void Fast TV_Draw_OK(void);
int Fast Prompt_Input(void);
int Fast TV_Yes_No(char *String);
void Fast TV_Name(char *String);
void Fast TV_Open_Key_Box(void);
void Fast TV_Close_Key_Box(void);
void Fast TV_Suspend(void);
void Fast TV_Resume(void);
void Fast TV_P_Bar(long H,long L);
void Fast TV_Zoom(void);
void Fast TV_Tile(void);
void Fast TV_Cascade(void);
void Fast TV_Ega43(void);
void Fast TV_Exit(int Result);
#endif
