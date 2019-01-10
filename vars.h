/*Copyright (c) Anker Berg-Sonne 1991*/
#include <stdio.h>
#include "sedtdef.h"
#ifndef VARS_C
#include "text.h"
#include "text1.h"
#endif
#include "bufdef.h"
#include "vdudef.h"
#include "file.h"
#include "regexp.h"

#if defined(Windows)
#include <windows.h>
#if defined(OWL)
#include "osedt.h"
#else
#include "wsedt.h"
#endif
#endif

#if defined(NT)
#include <windows.h>
#include "ntsedt.h"
#endif

#if defined(WIN32)
#undef WIN32
#include <windows.h>
#undef WIN32
#define WIN32 1
#endif

#if defined(DOS)
extern char Run_From[];
extern int Video_Seg;
#endif

#if defined(NT)
extern char Run_From[];
#endif

#if defined(WIN32)
extern char Run_From[];
extern HANDLE win32ConsoleHnd;
extern HANDLE win32KeybHnd;
#endif


#if defined(DOS) & defined (DESQVIEW)
extern int DV, Buflen, Save_Row, Save_Col;
extern unsigned long Ptr, Handle;
extern struct {unsigned int Row, Column; unsigned short Status, Field;}
 far *Ptr_Buf;
#endif

#if defined(DOS) & !defined(Windows)
extern int Mem_Buffering, EMS_Buffering, Save_Caps, Video_Seg, Got_EMS,
 EMS_Top, EMS_Handle, In_Int24;
#endif

#if (defined(DOS) | defined(NT)) & defined(Windows)
extern int Show_First, Char_Width, Char_Height;
extern HWND Curr_Wnd, Buf_Wnd[];
extern HANDLE Mem_Handle, Buf_Font;
#endif

#if defined(DOS) & defined(TV)
extern void *Buf_Wnd[];
extern short Buf_Draw_OK[];
extern char far My_Screen[Maxlines+1][Maxcol+1],
 far My_Attributes[Maxlines+1][Maxcol+1];
#endif

#if defined(OS2)
#define INCL_DOS
#define INCL_DOSDEVICES
#define INCL_SUB
#include <os2.h>
extern char Thread_Stack[2048], Mouse_Stack[2048];
extern int Old_Length, Sav_Shift_State;
extern unsigned Gdt_Descriptor, Ldt_Descriptor, Thread_ID, Mouse_ID,
 Mouse_Handle, Read_Type;
extern struct _GINFOSEG far *Gdt;
extern struct _LINFOSEG far *Ldt;
extern struct MonBuff 
 {/* generic monitor buffer header */
  unsigned        bufflen;
  unsigned        ddbufflen;
  unsigned char   dispatch[12];
  unsigned char   dummy[48];
 } InBuff, OutBuff;     /* buffers for monitor read/writes */
extern struct CharPacket
 {/* substructure of KeyPacket */
  unsigned char   asc;
  unsigned char   scan;
  unsigned char   status;
  unsigned char   dshift;
  unsigned        shift;
  unsigned long   time;
 };
extern struct KeyPacket
 {/* KBD monitor data record */
  unsigned        mnflags;
  struct CharPacket cp;
  unsigned        ddflags;
 } Key_Buff;
extern struct _KBDINFO Kbd_Status;
extern struct _MOUEVENTINFO Event;
extern struct _PTRLOC Ptr_Loc;
extern struct _VIOMODEINFO Mode_Data;
extern struct _VIOCURSORINFO Cursor_Data;
extern HSEM Mouse_Sem;
#endif

#if defined(DOS) | defined(OS2) | defined(NT) | defined(WIN32)
extern char Keyb_Name[], Keyb_File[];
extern char *Keyb_Names[];
extern int Set_Num_Lock, KBD_Handle, New_Mode, Old_Mode,
 Keyb_Type, DEC_Mode, Color, Gotmouse, Int16_Ext;
#endif

#if (defined(DOS) & !(defined(Windows) | defined(TV))) | defined(OS2)| defined(WIN32)
extern char Bracket[2][12], Output[132], Attr[132], Attab[16];
extern int St_Line, St_Col, Zap_Col, Zap_Line, Zap_Count;
#else
#if defined(DOS) & defined(TV)
extern char Attab[16];
#endif
extern char Bracket[2][10];
#endif

#if ((defined(DOS) | defined(NT)) & (defined(Windows) | defined(TV))) | (defined(ATARI) & defined(GEM))
extern int Dialog, S_Line, S_Col;
#endif

#if defined(VMS)
#include <descrip.h>
extern char TTY_Buffer[Maxcol], Mbx_Mess[100][1024], Today[21];
extern int Save_Mbx, Rms_Sts, VMS_Application, Org_Width, Force_Cursor,
 Got_In, This_In, Do_Echo, Echoed, Echo_Read, Bcast_OK, Mbx_Count, Bcast_Show;
extern long Parent_PID, Sav_Com, Echo_Pos;
extern struct dsc$descriptor_s Today_Desc;
#endif
#if defined(VMS) | defined(UNIX)
extern int Abs_Row, Abs_Col, Ldown, Rdown, Move, Abs_Row_Sav, Abs_Col_Sav;
extern int Gotmouse;
#endif

#if defined(UNIX)
extern char Defined_Terminal[];
extern int Org_Width;
extern int Same_Character;
extern int Saved_Character;
#include <unistd.h>
extern __uid_t My_UID;
extern __gid_t My_GID;
#endif

#if defined(UNIX) | defined(DOS) | defined(WIN32)
extern int File_Protection[];
extern int File_Reserved[];
#endif

#if defined(UNIX) & !defined(SCO)
extern struct winsize ws;
#endif

#if defined(UNIX) & defined(TERMCAP)
extern char tcapbuf[1024], tcap[1024],
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
extern long Key_Buffer[20];
extern int Key_Count;
#endif

#if defined(ATARI) & defined(GEM)
extern int top_window;
#endif

#if defined(MPE)
extern int mpe_stdin, mpe_stdout;
#pragma intrinsic FREAD,FWRITE,FOPEN,FCLOSE,FCONTROL,FSETMODE
#endif

#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE) | defined(WIN32)
extern char My_Screen[Maxlines+1][Maxcol+1],
 My_Attributes[Maxlines+1][Maxcol+1];
#endif

#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE)
 extern int D_Border[8], S_Border[8];
#else
extern int D_Border[8], S_Border[8];
#endif


#if defined(DOS) | defined(OS2) | defined(ATARI) | defined(NT) | defined(WIN32)
extern int Mouse_OK;
#endif

extern char *Infile, *Outfile, *Version, Map_Ext[], *Rep[],
 Lst_Input[10][132], Init_T_String[256], Reset_T_String[256], *Term_Names[],
 Out_String[Out_Size], Format_Line[], Init_File[], Dsk_Rec[Blksize+1],
 Com_Char, Com_Arg[512], Rep_Str[512], Find_Str[512], New_File[512],
 Help_File[80], Help_Levels[10];
extern int Nobak, Display_Column, Tabs[513], First_Line, Hdr_Mods,
 Hdr_Direction, Hdr_Learning, Justify, Nosub, Auto_Margin, Hdr_Replace,
 Hdr_Executing, CC_Enable, Hdr_Xpand, Doreturn, Call_Type, Hdr_Shift,
 Hdr_Do_Shift, Call_Color, Hdr_Block, Hdr_Changes, Def_Font, Max_Scroll,
 EGA43, Dir_Video, Call_Model, Show_Ruler, Mapping, Auto_Save, Auto_Warn,
 Return_Value, Save_Context, Use_Mouse, Remember, Firmware, Sub_Window,
 Sub_X, Sub_Y, Sub_H, Sub_W, CC_Flag, Auto_Count, Auto_Saved, Case_Sensitive,
 Hdr_Case, Sharemess, Conf_Error, Caps_Lock, Shift_Screen, Ins_Defs,
 Hdr_Ins_Defs, Cursor_Free, Hdr_Free, Do_Debug, Hdr_Reg, List_Files,
 Batch_Mode, Zoom_Windows, Ent_Char, E_Type, Old_E_Type, Reg_Search,
 Err_Flag, Com_End, Control_Display, Err_Flag, Expand_Tabs, In_Prompt,
 Save_Stack[20], Buffer_Stack[Maxbuffers], Buffers, Down_Type, Save_Top,
 Save_Left, Save_Length, Save_Width, Window_Ref[Maxbuffers], Top_Row,
 Left_Col, Zoomed, Max_Length, Max_Width, Phys_Length, Phys_Width,
 Key_Number, Map_Index, Term_Type, Scl_Regions, Scancode, Key_Defs, Map_Defs,
 Scan_Defs, Enhanced, Comb_Defs, Top_Func, Next_Func, His_Line, His_Col,
 My_Line, Scr_Width, Scl_Top, Scl_Bottom, Scr_Col, Scr_Length,
 Lst_Col[Maxlines+1], Out_Count, Cur_Attributes, His_Attributes, Not_End,
 Function, Gold, Err_Display, Show_Errors, Learning, Ln_Function, Executing,
 Character, Ex_Function[10], Ex_Index[10], Replace_Mode, Ex_Count[10],
 Com_Direction, Ex_Debth, Cur_Buffer, Lst_Current, Rel_First, Rel_Last,
 Home, Dl_Positions[Pointers], Direction, Def_Direction, Ent_Direction,
 Cur_Direction, Tab_Expand, Severe_Error, Do_Shift, Bell_Level, Show_Marks,
 Blk_Mode, Float_Cursor, Update_Flag, Counter_Index, Arg_Length, Shifted_Out,
 Skip, Append_It, At_End, In_Help, Help_Level, Offset, Wr_Count, Working_Flag,
 Free_List, D_Next, M_Next, Mem_Entries, This_Cmd, Lst_Cmd, What_Kind, Toofar,
 Reservation, File_Arguments, Margin, Requested_Margin;
extern short int Any_Changes[Maxbuffers], Read_Only[Maxbuffers];
extern long This_Line, Hdr_Line, Mem_Limit, Hdr_Lines, Word_Wrap, Indent,
 Left_Margin, Right_Margin, Hdr_Col, Scr_Shift, Funcs[Maxfunc][3], Beg_Pos,
 End_Pos, Beg_Line, End_Line, Dl_Pointers[Pointers+Maxbuffers][2], Beg_Region,
 End_Region, Other_Pos, Local_Count, My_Col, My_Pos, Sel_Col,
 Points[Maxpoints+5], Lst_Char_Move, Lst_Line_Move, Buf_Lines, Count,
 Ent_Count, Counter[26], End_Points[Maxpoints+1], Help_Pos;
extern struct Key_Tab_Struct *Key_Tab, *Key_Pointer;
extern struct Comb_Tab_Struct *Comb_Tab;
extern struct Scan_Tab_Struct Scan_Tab[Maxscan];
extern struct Map_Tab_Struct Map_Tab[Maxmap];
extern struct Buffer *Current, Main_Buffer, Pad, Com, Del;
extern struct fcb_t Overflow;
