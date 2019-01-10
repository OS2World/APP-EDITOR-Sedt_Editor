/*Copyright (c) Anker Berg-Sonne 1991*/

#define VARS_C
#include "vars.h"
#undef VARS_C

#if defined(WIN95)
#define DOS
#define Windows
#define ANSIC
#endif
/*
#include <stdio.h>
#include "sedtdef.h"
#include "vdudef.h"
#include "bufdef.h"
#include "file.h"
*/

#if defined(UNIX)
#include <sys/stat.h>
#include <unistd.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#if defined(DECOSF)
#include <sys/ioctl.h>
#endif
#endif

#if defined(Windows)
#include <windows.h>
#endif

#if defined(DOS) & defined(__LARGE__)
unsigned _ovrbuffer=0x2000;
#endif

#if defined(DOS) | defined(NT)
char Run_From[80];
	/*Directory  that Sedt was run from*/
int Video_Seg;
	/*Segment address of the video buffer for direct screen output*/
#endif

#if defined(WIN32)
char Run_From[300];
	/*Directory  that Sedt was run from*/
HANDLE win32ConsoleHnd;
	/*e.g. GetStdHandle(STD_OUTPUT_HANDLE); */
HANDLE win32KeybHnd;
	/*e.g. GetStdHandle(STD_INPUT_HANDLE); */
#endif

#if defined (DOS) & defined(DESQVIEW)
int DV, 
        /*1 if Sedt is running under DESQview*/
 Buflen,
	/*Argument for ptr_read*/
 Save_Row, 
	/*Row number where mouse movement started*/
 Save_Col;
	/*Column number where mouse movement started*/
unsigned long Ptr;
	/*Pointer object handle*/
unsigned long Handle;
	/*I think unused*/
struct {unsigned int Row, Column; unsigned short Status, Field;} far *Ptr_Buf;
	/*Pointer message buffer*/
#endif

#if defined(DOS) & !defined(Windows)
int Mem_Buffering=1,
	/*Use unused DOS memory for buffering*/
 EMS_Buffering=1,
	/*Use EMS memory for buffering*/
 Save_Caps,
	/*State of caps lock at time of last screen update*/
 Got_EMS=0,
	/*1 if EMS memory is available*/
 EMS_Top=0,
	/*Amount of memory used in current EMS overflow block*/
 EMS_Handle,
	/*EMS memory handle*/
 In_Int24=0;
	/*1 if Sedt is handling an INT24 (critical DOS error) interrupt*/
#endif

#if defined(DOS) & defined(TV)
void *Buf_Wnd[Maxbuffers];
short Buf_Draw_OK[Maxbuffers];
char far My_Screen[Maxlines+1][Maxcol+1],
	/*Image of characters on screen*/
 far My_Attributes[Maxlines+1][Maxcol+1];
	/*Attributes of characters on screen*/
#endif

#if (defined(DOS) | defined(NT)) & defined(Windows)
int Show_First;
        /*1 if the first window should be shown, 0 if iconized*/
int Mem_Entries=1024;
	/*Number of entries in buffer overflow table*/
HANDLE Mem_Handle;
HANDLE Buf_Font;
#else
int Mem_Entries=4096;
	/*Number of entries inbuffer overflow table*/
#endif

#if defined(DOS) & defined(Windows) & defined(OWL)
HWND Curr_Wnd, Buf_Wnd[Maxbuffers];
int Char_Width, Char_Height;
#endif

#if defined(OS2)
#define INCL_DOS
#define INCL_DOSDEVICES
#define INCL_SUB
#include <os2.h>
char Thread_Stack[2048];
	/*Stack area for keyboard monitor thread*/
int Old_Length, 
	/*Screen length when Sedt was called*/
 Sav_Shift_State;
	/*Shift state when Sedt was called*/
unsigned Gdt_Descriptor, 
	/*GDT descriptor pointer*/
 Ldt_Descriptor, 
	/*Ldt descriptor pointer*/
 Thread_ID, 
	/*Thread ID for keyboard monitor thread*/
 Mouse_ID, 
	/*Thread ID for mouse thread*/
 Mouse_Handle,
	/*Handle for mouse*/
 Read_Type;
	/*Type for mouse event*/
struct _GINFOSEG far *Gdt;
	/*Gdt information block*/
struct _LINFOSEG far *Ldt;
	/*Ldt information block*/
struct MonBuff 
 {/* generic monitor buffer header */
  unsigned        bufflen;
  unsigned        ddbufflen;
  unsigned char   dispatch[12];
  unsigned char   dummy[48];
 } InBuff, OutBuff;     /* buffers for monitor read/writes */
struct CharPacket
 {/* substructure of KeyPacket */
  unsigned char   asc;
  unsigned char   scan;
  unsigned char   status;
  unsigned char   dshift;
  unsigned        shift;
  unsigned long   time;
 };
struct KeyPacket 
 {/* KBD monitor data record */
  unsigned        mnflags;
  struct CharPacket cp;
  unsigned        ddflags;
 } Key_Buff;
struct _KBDINFO Kbd_Status;
	/*Keyboard status block*/
struct _MOUEVENTINFO Event;
	/*Mouse event block*/
struct _PTRLOC Ptr_Loc;
	/*Pointer location block*/
struct _VIOMODEINFO Mode_Data;
	/*Video mode information block*/
struct _VIOCURSORINFO Cursor_Data;
	/*Cursor information block*/
HSEM Mouse_Sem=0;
	/*Semaphor to synchronize mouse and main thread*/
#endif

#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
char Keyb_Name[80], 
	/*User supplied keyboard name used to select mapping files*/
 Keyb_File[80], 
	/*Name of key mapping file*/
 *Keyb_Names[]=
  {NULL,"IBM","LK250","Enhanced IBM","IBM Convertible","Tandy 1400","AT",
  "SLT","Default"};
	/*Translation of keyboard types into keyboard names*/
int Set_Num_Lock= -1, 
	/*Flag to determine if the PC keyboard num lock state should be set*/
 KBD_Handle, 
	/*Keyboard monitor handle*/
 New_Mode=0, 
	/*Video mode specified in call to Sedt*/
 Old_Mode=0, 
	/*Video mode when Sedt was called*/
 Keyb_Type=0, 
	/*Keyboard type*/
 DEC_Mode, 
	/*1 if LK250 keyboard is in DEC mode*/
 Color=0, 
	/*1 if a color adapter is being used*/
#if defined(UNIX) | defined(VMS)
 Gotmouse=0,
#else
 Gotmouse=1,
#endif
	/*1 if a mouse is present*/
 Int16_Ext=0;
	/*1 if the PC type supports enhanced keyboard BIOS*/
#endif

#if (defined(DOS) & !(defined(Windows) | defined(TV))) | defined(OS2) | defined(WIN32)
char Bracket[2][12]={ { '\'','"','[',']','{','}','(',')','<','>',174,175 },
                      { '\'','"',']','[','}','{',')','(','>','<',175,174 }};
	/*Matched bracket characters*/
char Output[132], Attr[132], Attab[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	/*Table of chracter attributes*/
 Mouse_Stack[2048];
	/*Stack area for mouse thread stack*/
int St_Line, 
	/*Starting line of current output string*/
 St_Col, 
	/*Starting column of current output string*/
 Zap_Col, 
	/*Current column that current output string is at*/
 Zap_Line, 
	/*Current line that current output string is at*/
 Zap_Count;
	/*Length of current output string*/
#else
#if defined(DOS) & defined(TV)
char Attab[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, Mouse_Stack[2048];
#endif
char Bracket[2][10]={'\'','"','[',']','{','}','(',')','<','>',
		     '\'','"',']','[','}','{',')','(','>','<'};
	/*Matched bracket characters*/
#endif

#if ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM))
int Dialog=1, 
	/*1 if dialog boxes are used to prompt for input*/
 S_Line, 
	/*Line for current output string*/
 S_Col;
	/*Column for current output string*/
#endif

#if defined(VMS)
#include <descrip.h>
char TTY_Buffer[Maxcol], 
	/*Input character buffer*/
 Mbx_Mess[100][1024], 
	/*Last 100 broadcast messages*/
 Today[21];
	/*Todays date from VMS*/
int Nobak=1, 
	/*1 if .bak files should not be created*/
 Save_Mbx=0, 
	/*Number of VMS broadcast messages saved*/
 VMS_Application=0, 
	/*Keypad application mode supplied by VMS*/
 Org_Width=0, 
	/*Original screen width supplied by VMS*/
 Force_Cursor=0,
	/*1 if VMS implementation should force the cursor position*/
 Bcast_OK=0, 
	/*1 if its OK to display a broadcast message*/
 Mbx_Count=0, 
	/*Number of save broadcast messages*/
 Bcast_Show=0, 
	/*1 if broadcast messages should be shown*/
 Got_In, 
	/*Number of characters read*/
 This_In, 
	/*Number of characters processed by Sedt*/
 Do_Echo=0,
	/*1 if characters should be read with echo on*/
 Echoed=0, 
	/*1 if the characters being processed have been echoed*/
 Echo_Read=0;
	/*Number of echoed characters read*/
long Parent_PID,
	/*PID of parent process supplied on command line*/
 Sav_Com,
	/*Position in Com where echoed characters are saved*/
 Echo_Pos;
	/*Position in buffer where echoed charcters are to be inserted*/
struct dsc$descriptor_s Today_Desc=
        { 20, DSC$K_DTYPE_T, DSC$K_CLASS_S, Today};
	/*Descriptor for today's date*/
#else
int Nobak=0;
	/*1 if no .bak files are to be created when saving*/
#endif

#if defined(VMS) | defined(UNIX)
int Abs_Row, Abs_Col, Ldown, Rdown, Move, Abs_Row_Sav, Abs_Col_Sav;
	/*Mouse parameter*/
int Gotmouse=0; 
	/*1 if a mouse is present*/
#endif

#if defined(UNIX)
char Defined_Terminal[80];
	/*User supplied terminal type*/
int Org_Width=0;
	/*Original width of terminal*/
int Same_Character=0;
int Saved_Character=0;
__uid_t My_UID;
 /*UID of current process*/
__gid_t My_GID;
 /*GID of current process*/
#endif

#if defined(UNIX) | defined(DOS) | defined(WIN32)
int File_Protection[Maxbuffers];
 /*Saved file protections*/
int File_Reserved[Maxbuffers];
 /*1 if the file has been reserved*/
#endif

#if defined(UNIX) & !(defined(SCO) | defined(HPUX))
struct winsize ws;
	/*Structure for terminal height and width*/
#endif

#if defined(UNIX) & defined(TERMCAP)
char tcapbuf[1024], tcap[1024],
	/*Terminal capability buffers*/
       *SO,                     /* standout */
       *SE,                     /* end standout */
       *CE,			/* del to end of line */
       *CH,			/* horizontal Cursor motion */
       *CL,			/* clear screen */
       *CM,			/* cursor motion */
       *CS,			/* change scrolling region */
       *IS,			/* initialize terminal */
       *KE,			/* reset keypad */
       *KS,			/* enable keypad */
       *MB,			/* bold */
       *MD,			/* blink */
       *ME,			/* reset */
       *MR,			/* reverse */
       *SR,			/* scroll reverse */
       *UL,			/* underline */
       *SF;			/* upline */
#endif

#if defined(ATARI) & !defined(GEM)
long Key_Buffer[20];
	/*Terminal typeahead buffer*/
int Key_Count=0;
	/*Number of character in typeahead buffer*/
#endif

#if defined(ATARI) & defined(GEM)
int top_window;
	/*Handle of the current window*/
#endif

#if defined(VMS) | defined(UNIX)
int Display_Column=0;
#else
int Display_Column=1;
#endif
	/*1 if the current column nuumber is to be displayed on banner line*/

#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE) | defined(WIN32)
char My_Screen[Maxlines+1][Maxcol+1],
	/*Image of characters on screen*/
 My_Attributes[Maxlines+1][Maxcol+1];
	/*Attributes of characters on screen*/
#endif

#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE) 
int D_Border[8]={'-','-','-','|','-','-','-','|'},
 S_Border[8]={'-','-','-','|','-','-','-','|'};
	/*Characters used to draw window borders*/
#else
int D_Border[8]={201,205,187,186,188,205,200,186},
 S_Border[8]={218,196,191,179,217,196,192,179};
	/*Characters used to draw window borders*/
#endif


#if defined(DOS) | defined(NT) | defined(OS2) | defined(ATARI) | defined(WIN32)
int Mouse_OK=0;
	/*1 if Sedt can handle mouse commands*/
#endif

char *Infile, *Outfile, *Version="5.0 (303+mdj)", Map_Ext[80], *Rep[]=
 {"<NUL>","<SOH>","<STX>","<ETX>","<EOT>","<ENQ>","<ACK>","<BEL>",
  "<BS>","<HT>","<LF>","<VT>","<FF>","<CR>","<SO>","<SI>",
  "<DLE>","<DC1>","<DC2>","<DC3>","<DC4>","<NAK>","<SYN>","<ETB>",
  "<CAN>","<EM>","<SUB>","<ESC>","<FS>","<GS>","<RS>","<US>"},
	/*Mnemonics for control characters*/
 Lst_Input[10][132],
	/*Last 10 responses to prompts, for recall*/
 Init_T_String[256],
	/*User supplied string to initialize terminal*/
 Reset_T_String[256],
	/*User supplied string to restore terminal*/
 *Term_Names[]={"VT100","VT200","VT52","VK100","VAXmate",
  "IBM PC/AT","IBM PC/XT","IBM PC","PC 100","MS Windows","IBM Convertible",
  "Dummy","AT 339","XT 286","PS/2 Model 50","PS/2 Model 60",
  "PS/2 Model 30","PS/2 Model 80","XT/2","VT300","Tandy 1400","ANSI","Termcap",
  "VT400","Default","Defined"},
	/*Names of systems and terminals*/
  Out_String[Out_Size],
	/*Buffer for characters to be output to terminal*/
  Format_Line[Maxcol+1],
	/*Ruler display line*/
  Init_File[80],
	/*Name of user supplied initialization file*/
  Dsk_Rec[Blksize+1],
	/*Buffer for disk I/O*/
  Com_Char,
	/*Current character being parsed*/
  Com_Arg[512],
	/*Contents of delimited command argument*/
  Rep_Str[512],
	/*String to be used in regular expression replace*/
  Find_Str[512],
	/*String to be used for regular expression searches*/
  New_File[512],
	/*Name of file supplied by user*/
  Help_File[80],
	/*Name of current help file*/
  Help_Levels[10];
	/*Help menu choices that got us here*/
	
int Tabs[513],
	/*Tabs[I] 1 if a tab stop is set in column I*/
 First_Line=1,
	/*Line number supplied in command argument*/
 Hdr_Mods,
	/*Modified status from last time banner line was written*/
 Hdr_Direction,
	/*Direction flag from last time status line was written*/
 Hdr_Learning,
	/*Learning status from last time status line was written*/
 Justify,
	/*1 if justification (stright right margin), 0 if jagged*/
 Nosub=1,
	/*0 if ^Z should be inserted at end of file, 1 otherwise*/
 Auto_Margin=0,
	/*1 if automatic margin*/
 Hdr_Replace,
	/*Replace mode from last time status line was written*/
 Hdr_Executing,
	/*Executing status from last time status line was written*/
 CC_Enable=1,
	/*1 if OK to intercept control C*/
 Hdr_Xpand,
	/*Expand tabs mode from last time status line was written*/
 Doreturn=0,
	/*1 if files must be terminates with <CR>*/
 Call_Type=0,
	/*User supplied system or terminal type, 0 if auto-detection*/
 Hdr_Shift,
	/*Horizontal screen shift from last time status line was written*/
 Hdr_Do_Shift,
	/*Horizontal shift flag from last time status line was written*/
 Call_Color=0,
	/*1 if user specified mono, 2 if user specified color, otherwise 0*/
 Hdr_Block,
	/*Block mode from last time status line was written*/
 Hdr_Changes=0,
	/*1 if the banner and status lines need to be rewritten*/
 Def_Font=0,
	/*Font size requested by user; 0 if none requested*/
 Max_Scroll=0,
	/*Maximum number of lines user want screen to scroll, 0 if any*/
 EGA43=0,
	/*1 if user has requested 43 line EGA mode*/
 Dir_Video=1,
	/*1 if PC output should go directly to video memory; 1 for BIOS*/
 Call_Model=0,
	/*PC model provided by user, 0 if none*/
 Show_Ruler=1,
	/*1 if ruler should be shown on display*/
 Mapping= -1,
	/*Keyboard mapping supplied by user*/
 Auto_Save=200,
	/*Number of keystrokes to make before auto save after a modification*/
 Auto_Warn=20,
	/*Number of keystrokes before autosave to issue warning*/
 Return_Value,
	/*Return status to return to caller of Sedt*/
 Save_Context=0,
	/*1 if context should be saved with file*/
#if defined(VMS) | defined(UNIX)
 Use_Mouse=0,
#else
 Use_Mouse=1,
#endif
	/*1 if Sedt should use mouse*/
 Remember=0,
	/*1 if Sedt should load previous file edited if called with no args*/
 Firmware=0,
	/*1 if enhanced keyboard firmware calls should be used*/
 Sub_Window=0,
	/*1 if user has requested using a part of full screen*/
 Sub_X=0,
	/*Line number where Sedt display should start*/
 Sub_Y=0,
	/*Column where Sedt display should start*/
 Sub_H,
	/*Height of subscreen that Sedt should use*/
 Sub_W,
	/*Width of subscreen that Sedt should use*/
 CC_Flag=0,
	/*1 if a control C has been typed*/
 Auto_Count,
 Auto_Saved,
 Case_Sensitive=0,
 Hdr_Case,
 Sharemess=1,
 Conf_Error=0,
 Caps_Lock=0,
 Shift_Screen=0,
 Ins_Defs=0,
 Hdr_Ins_Defs,
 Cursor_Free=0,
 Hdr_Free=0,
 Do_Debug=0,
 Hdr_Reg,
 List_Files,
 Batch_Mode=0,
 Zoom_Windows=1,
 Ent_Char,
 E_Type=E_Unknown,
 Old_E_Type=E_Unknown,
 Reg_Search=0,
 Err_Flag,
 Control_Display=1,
 Com_End,
 Expand_Tabs=1,
 In_Prompt=0,
 Save_Stack[20],
 Buffer_Stack[Maxbuffers],
 Buffers,
 Down_Type=0,
 Save_Top,
 Save_Left,
 Save_Length,
 Save_Width,
 Window_Ref[Maxbuffers],
 Top_Row=0,
 Left_Col=0,
 Zoomed=1,
 Max_Length,
 Max_Width,
 Phys_Length,
 Phys_Width,
 Key_Number,
 Map_Index,
 Term_Type= -1,
 Scl_Regions,
 Scancode,
 Key_Defs=0,
 Map_Defs=0,
 Scan_Defs=0,
 Enhanced,
 Comb_Defs=0,
 Top_Func=0,
 Next_Func=0,
 His_Line=1,
 His_Col=1,
 My_Line=1,
 Scr_Width=80,
 Scl_Top=0,
 Scl_Bottom=0,
 Scr_Col,
 Scr_Length=24,
 Lst_Col[Maxlines+1],
 Out_Count,
 Cur_Attributes,
 His_Attributes,
 Not_End=1,
 Function,
 Gold,
 Err_Display,
 Show_Errors=1,
 Learning=0,
 Ln_Function,
 Executing=0,
 Character,
 Ex_Function[10],
 Ex_Index[10],
 Replace_Mode=0,
 Ex_Count[10],
 Com_Direction,
 Ex_Debth=0,
 Cur_Buffer,
 Lst_Current= -1,
 Rel_First,
	/*Negative of the number of lines between the cursor and the top of the window*/
 Rel_Last,
 	/*Number of lines between the cursor and the bottom of the window*/
 Home,
 Dl_Positions[Pointers],
 Direction,
 Def_Direction=1,
 Ent_Direction,
 Cur_Direction,
 Tab_Expand=1,
 Severe_Error=0,
 Do_Shift=1,
 Bell_Level=2,
 Show_Marks=0,
 Blk_Mode=0,
 Float_Cursor=1,
 Update_Flag=1,
 Counter_Index,
 Arg_Length,
 Shifted_Out=0,
 Skip=0,
 Append_It=0,
 At_End,
 In_Help,
 Help_Level,
 Offset,
 Wr_Count=0,
 Working_Flag=0,
 Free_List= -1,
 D_Next=0,
 M_Next=0,
 This_Cmd,
 Lst_Cmd=C_Unknown,
 What_Kind,
 Toofar=0,
 Reservation=0,
  /*Reserve files by setting them read-only*/
 File_Arguments=0,
  /*the user supplied file arguments when calling Sedt*/
 Margin=1,
  /*Number of lines to leave between the cursor and the top and bottom of the window*/
 Requested_Margin=1;
  /*Number of lines user requested between the cursor and top and bottom of window*/
short int Any_Changes[Maxbuffers], Read_Only[Maxbuffers];
long This_Line=1, Hdr_Line, Mem_Limit= -1, Hdr_Lines, Word_Wrap=1,
 Indent=1, Left_Margin=1, Right_Margin=1, Hdr_Col, Scr_Shift=0,
 Funcs[Maxfunc][3], Beg_Pos, End_Pos, Beg_Line, End_Line,
 Dl_Pointers[Pointers+Maxbuffers][2], Beg_Region, End_Region, Other_Pos,
 Local_Count, My_Col=1, My_Pos=1, Sel_Col, Points[Maxpoints+5], Lst_Char_Move,
 Lst_Line_Move, Buf_Lines, Count, Ent_Count, Counter[26],
 End_Points[Maxpoints+1], Help_Pos;
struct Key_Tab_Struct *Key_Tab, *Key_Pointer;
struct Comb_Tab_Struct *Comb_Tab;
struct Scan_Tab_Struct Scan_Tab[Maxscan];
struct Map_Tab_Struct Map_Tab[Maxmap];
struct Buffer *Current, Main_Buffer, Pad, Com, Del;
struct fcb_t Overflow;
