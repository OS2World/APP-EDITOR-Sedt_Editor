/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"
#include "commdlg.h"
#include "print.h"

char szAppName[10];
char szWindowTitle[30];
HANDLE hInst, hCursor;
FARPROC lpprocAbout;
FARPROC lpprocPrompt;
HWND Curr_Wnd, Buf_Wnd[Maxbuffers];
LOGFONT LF;
int Char_Width, Rfreshed=0, Char_Height, Dragging=0, DragX, DragY;
int Ch_Ch, Ch_Count=0, First=1, Cursor_On=0, Sav_Cur, Def_Height,
 Def_Width, Saved_Num_Lock, Lst_Index;
char Work[80];
struct SREGS Rrv;
char Fil_String[128], Sel_Dir[80];
char Keyb_State[256];
int Save_My_Num_Lock, Save_Their_Num_Lock;

extern int __argc;
extern char **__argv;

BOOL FAR PASCAL _export About(HWND hDlg,unsigned message,WORD W,LONG L)
{if (message == WM_COMMAND)
 {EndDialog( hDlg, TRUE );
  return TRUE;
 } else
 if (message == WM_INITDIALOG)
  return TRUE; else
  return FALSE;
}

void SedtPaint(HWND H,PAINTSTRUCT *pps,int Is_Current )
{HBRUSH hbr, hbrOld;
 HDC hDC;
 if (Rfreshed)
 {Rfreshed=0;
  return;
 }
 hDC=pps->hdc;
 if (pps->fErase)
 {hbr = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
  hbrOld = (HBRUSH)SelectObject( hDC, (HANDLE)hbr );
  FillRect( hDC, (LPRECT)&pps->rcPaint, hbr );
  SelectObject( hDC, (HANDLE)hbrOld );
  DeleteObject( (HANDLE)hbr );
 }
 SetBkMode( hDC, OPAQUE);
 if (Out_Count)
  Out_Zap();
 S_Header();
 if (In_Help)
 {Display_Help(Help_Pos);
  Out_Zap();
  Win_On_Curs();
 } else
 {if (Show_Ruler)
   S_Ruler();
  Fill_Screen(Rel_First,Rel_Last);
  if (!Is_Current)
	Mark_Cursor();
  Out_Zap();
 }
}

BOOL SedtInit(HANDLE hInstance )
{WNDCLASS   SedtClass;
 LoadString( hInstance, IDSNAME, (LPSTR)szAppName, 10 );
 LoadString( hInstance, IDSTITLE, (LPSTR)szWindowTitle, 30 );
 SedtClass.hCursor       = LoadCursor( (HINSTANCE) NULL, (LPCSTR)IDC_IBEAM );
 SedtClass.hIcon         = LoadIcon( hInstance, (LPSTR)szAppName );
 SedtClass.lpszMenuName  = (LPSTR)"SedtMenu";
 SedtClass.lpszClassName = (LPSTR)szAppName;
 SedtClass.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
 SedtClass.hInstance     = hInstance;
 SedtClass.style         = CS_VREDRAW | CS_HREDRAW | CS_CLASSDC;
 SedtClass.lpfnWndProc   = SedtWndProc;
 SedtClass.cbClsExtra    = 0;
 SedtClass.cbWndExtra    = 0;
 if (!RegisterClass( &SedtClass ) )
  return FALSE;
 return TRUE;        /* Initialization succeeded */
}

int Make_Window(int B,int Show)
{HWND hWnd;
 HMENU hMenu, hMenu1;
 HDC hDC;
 RECT Rect;
 TEXTMETRIC TM;
 HANDLE Old_Font;
 LOGFONT SLF;
 int I;
 if ((hWnd = CreateWindow((LPSTR)szAppName,
   (LPSTR)szWindowTitle,
   WS_OVERLAPPED|WS_VSCROLL|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|
   WS_THICKFRAME|WS_SYSMENU,
   CW_USEDEFAULT,0,CW_USEDEFAULT,0,
   (HWND)NULL,        /* no parent */
   (HMENU)NULL,       /* use class menu */
   (HANDLE)hInst, /* handle to window instance */
   (LPSTR)NULL        /* no params to pass on */
  ))==(HWND)NULL)
   return 0;
 Curr_Wnd=Buf_Wnd[B]=hWnd;
 hDC = GetDC( hWnd );
 LF.lfWidth=0;
 LF.lfEscapement=0;
 LF.lfOrientation=0;
 LF.lfItalic=0;
 LF.lfUnderline=0;
 LF.lfStrikeOut=0;
 LF.lfCharSet=ANSI_CHARSET;
 LF.lfOutPrecision=OUT_DEFAULT_PRECIS;
 LF.lfClipPrecision=CLIP_DEFAULT_PRECIS;
 LF.lfQuality=DRAFT_QUALITY;
 LF.lfPitchAndFamily=FF_MODERN;
 if (Def_Font!=0)
 {strcpy(LF.lfFaceName,"Courier New");
  LF.lfWeight=FW_BOLD;
  LF.lfHeight= -Def_Font;
 } else
 {strcpy(LF.lfFaceName,"Fixedsys");
  LF.lfWeight=FW_DONTCARE;
  LF.lfHeight=-9;
 }
 SLF=LF;
 SLF.lfHeight=MulDiv(SLF.lfHeight,GetDeviceCaps(hDC,LOGPIXELSY),72);
 Buf_Font=CreateFontIndirect((LPLOGFONT)&SLF);
 Old_Font=SelectObject(hDC,Buf_Font);
 GetTextMetrics( hDC, (LPTEXTMETRIC)&TM );
 SelectObject(hDC,Old_Font);
 GetClientRect(Curr_Wnd,(LPRECT)&Rect);
 Scr_Width=(Rect.right+1)/(Char_Width=TM.tmAveCharWidth);
 Scr_Length=(Rect.bottom+1)/(Char_Height=TM.tmHeight);
 ReleaseDC( hWnd, hDC );
 SetScrollRange(hWnd,SB_VERT,0,100,1);
 if (Zoom_Windows)
  ShowWindow(hWnd,Show|SW_SHOWMAXIMIZED); else
  ShowWindow( hWnd, Show );
 Rfreshed=1;
 UpdateWindow( hWnd );
 hMenu=GetMenu(hWnd);
 for (I=0;I<Maxbuffers;I++)
 if (Buf_Wnd[I]!=0&&I!=B)
 {hMenu1=GetMenu(Buf_Wnd[I]);
  GetMenuString(hMenu1,20+I,(LPSTR)Work,80,MF_BYCOMMAND);
  ChangeMenu(hMenu,20+I,(LPSTR)Work,20+I,MF_CHANGE|MF_STRING|MF_BYCOMMAND);
 }
 for (I=0;I<Maxbuffers;I++)
 if (Buf_Wnd[I]!=0)
 {hMenu=GetMenu(Buf_Wnd[I]);
  ChangeMenu(hMenu,20+B,(LPSTR)(char *)Txt_Untitled,20+B,MF_CHANGE|MF_STRING);
 }
 EnableMenuItem(hMenu,20+B,MF_GRAYED);
 return 1;
}

int PASCAL WinMain(HANDLE hInstance,HANDLE hPrevInstance,LPSTR L,int cmdShow )
{Show_First=cmdShow;
 if (!hPrevInstance)
 {if (!SedtInit( hInstance ))
   return FALSE;
 } else
 {GetInstanceData( hPrevInstance, (BYTE *)szAppName, 10 );
  GetInstanceData( hPrevInstance, (BYTE *)szWindowTitle, 30 );
 }
 lpprocAbout = MakeProcInstance( (FARPROC)About, hInstance );
 lpprocPrompt = MakeProcInstance( (FARPROC)Prompt, hInstance );
 hInst = hInstance;
 getcwd(Sel_Dir,80);
 Sedt_Args(__argc,(char * *)__argv);
 Sedt_Init();
 Readruler('0');
 Sedt_Files(__argc,(char * *)__argv);
 if (!(File_Arguments&&Outfile==NULL))
 {Sedt_Main();
 } else
 {Sedt_Cleanup();
  Sedt_Shutdown();
 }
 exit(Return_Value);
 return Return_Value;
}

void Make_File()
{
 if (Fil_String[0]=='\\'||Fil_String[0]=='/'||Fil_String[1]==':')
  strcpy(Com_Arg,Fil_String); else
 {strcpy(Com_Arg,Sel_Dir);
  if (Sel_Dir[strlen(Sel_Dir)-1]!='\\')
  strcat(Com_Arg,"\\");
  strcat(Com_Arg,Fil_String);
 }
}

long FAR PASCAL _export SedtWndProc(HWND hWnd,unsigned message,unsigned wParam,LONG lParam )
{RECT rect;
 HBRUSH hbr, hbrOld;
 PAINTSTRUCT ps;
 int mx, my, Offset, I, B, Is_Current;
 char *S;
 if (hWnd==Curr_Wnd)
 {B=Cur_Buffer;
  Is_Current=1;
 } else
 {Is_Current=0;
  for (B=0;B<Maxbuffers;B++)
   if (Buf_Wnd[B]==hWnd)
    break;
 }
 switch (message)
 {case WM_SYSCOMMAND:
   return DefWindowProc(hWnd, message, wParam, lParam );
  case WM_SETFOCUS:
   if (!Is_Current)
    Ch_Window(B);
   CreateCaret( hWnd, (HBITMAP)NULL,(int)Char_Width,
    (int)Char_Height);
   SetCaretPos((int)(Char_Width*(My_Col-Scr_Shift-1)),
    (int)(Char_Height*(My_Line-1)));
   if (Cursor_On)
    ShowCaret(Curr_Wnd);
	Dragging=0;
   if (Set_Num_Lock!=-1)
	{GetKeyboardState((BYTE far *)Keyb_State);
    Save_Their_Num_Lock=Keyb_State[VK_NUMLOCK];
    Keyb_State[VK_NUMLOCK]=Save_My_Num_Lock;
	 SetKeyboardState((BYTE far *)Keyb_State);
   }
   break;
  case WM_KILLFOCUS:
   if (Cursor_On)
    HideCaret(Curr_Wnd);
   DestroyCaret();
   if (Set_Num_Lock!=-1)
	{GetKeyboardState((BYTE far *)Keyb_State);
    Save_My_Num_Lock=Keyb_State[VK_NUMLOCK];
    Keyb_State[VK_NUMLOCK]=Save_Their_Num_Lock;
	 SetKeyboardState((BYTE far *)Keyb_State);
   }
	break;
  case WM_PAINT:
   if (!Is_Current)
    Swap_Window(B);
   BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
   SedtPaint( hWnd,(PAINTSTRUCT *)&ps ,Is_Current);
   EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
   if (!Is_Current)
    Restore_Window(B);
   break;
  case WM_SIZE:
	if (!Is_Current)
    Swap_Window(B);
   Scr_Width=LOWORD(lParam)/Char_Width;
   Scr_Length=HIWORD(lParam)/Char_Height;
   Scr_Shift=0;
   if (First)
    First=0; else
	{if (Do_Shift)
    {if (My_Col-Scr_Shift>=Scr_Width)
     {while (My_Col-Scr_Shift>=Scr_Width)
       Scr_Shift+=Shift_Screen?Shift_Screen:Scr_Width/2;
     }
    }
    Offset=Rel_First+Rel_Last;
    Rel_First=Show_Ruler?3:2;
    Rel_Last=Scr_Length-2;
    My_Line=Home=(Rel_First+Rel_Last+1)/2;
    Rel_First-=Home;
    Rel_Last-=Home;
    if (Float_Cursor)
     Set_Offset(Offset);
    SetCaretPos((int)(Char_Width*(My_Col-Scr_Shift-1)),
     (int)(Char_Height*(My_Line-1)));
   }
	if (!Is_Current)
	 Restore_Window(B);
   break;
  case WM_ERASEBKGND:
   GetClientRect( hWnd, (LPRECT)&rect );
   hbr = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
   hbrOld = (HBRUSH)SelectObject( (HDC)wParam, (HANDLE)hbr );
   FillRect( (HDC)wParam, (LPRECT)&rect, hbr );
   SelectObject( (HDC)wParam, (HANDLE)hbrOld );
   DeleteObject( (HANDLE)hbr );
   break;
  case WM_QUERYENDSESSION:
   for (I=0;I<Maxbuffers;I++)
   if (Buf_Wnd[I]!=0&&(Any_Changes[Cur_Buffer]&1))
    return Win_Yes_No(Unsaved_Chng_Txt);
   return 1;
  case WM_LBUTTONDOWN:
   if (In_Help)
    break;
	if (!Mouse_OK)
    break;
   Win_Off_Curs();
   DragY=my=(HIWORD(lParam))/Char_Height+1;
   DragX=mx=(LOWORD(lParam))/Char_Width+1;
	Mouse_Execute(my,mx,1,0,0);
   Dragging=1;
   Win_On_Curs();
   break;
  case WM_LBUTTONUP:
   Dragging=0;
   break;
  case WM_RBUTTONDOWN:
  case WM_MOUSEMOVE:
   if (!Mouse_OK||(message==WM_MOUSEMOVE&&!Dragging))
    break;
   Win_Off_Curs();
   my=(HIWORD(lParam))/Char_Height+1;
	mx=(LOWORD(lParam))/Char_Width+1;
   if (DragX!=mx||DragY!=my)
    Mouse_Execute(my,mx,1,0,1);
   Win_On_Curs();
   break;
  case WM_CLOSE:
   if (In_Help)
    break;
   Win_Quit();
   break;
  case WM_VSCROLL:
	if (In_Help)
    break;
   if (wParam!=SB_ENDSCROLL)
   {Win_Off_Curs();
    if (Err_Display)
     Clr_Error();
    Init_Entity();
	 switch (wParam)
    {case SB_LINEUP:
      Ent_Direction=0;
     case SB_LINEDOWN:
      Line();
      break;
     case SB_PAGEUP:
      Ent_Direction=0;
     case SB_PAGEDOWN:
      Screen();
      break;
     case SB_TOP:
      Top();
      break;
     case SB_BOTTOM:
      Bottom();
      break;
	  case SB_THUMBPOSITION:
		if ((Ent_Count=((LOWORD(lParam)*Buf_Lines)/100)+1)>Buf_Lines)
       Ent_Count=Buf_Lines;
      Abs_Line();
      break;
    }
    Move_Entity();
    Upd_Header();
    Win_On_Curs();
   }
   Out_Zap();
   break;
  case WM_COMMAND:
   if (In_Help)
    break;
   if (!Mouse_OK)
    break;
   Win_Off_Curs();
   switch (wParam)
	{case FILE_OPEN:
     if (Open_File_Dialog())
     {Make_File();
      Count=1;
      Edit_File(Com_Arg);
		Upd_Header();
     }
     break;
    case FILE_SAVE:
     if ((S=Outfile)==NULL||S[0]==0)
      Error(2,No_Output_File_Txt); else
     {Wr_Buffer(S);
     }
     if (!Err_Flag)
      Clr_Changes();
     Upd_Header();
     break;
    case FILE_SAVE_AS:
	  if (Save_As_Dialog())
     {Make_File();
      strcpy(New_File,Com_Arg);
      Wr_Buffer(Outfile=New_File);
      S_Header();
      if (!Err_Flag)
       Clr_Changes();
      Upd_Header();
     }
     break;
    case FILE_INSERT:
	  if (Open_File_Dialog())
     {Make_File();
      Include_File(Com_Arg,File_Not_Found_Txt);
      Fill_Screen(0,Rel_Last);
      Set_Changes();
      S_Header();
     }
	  break;
    case FILE_QUIT:
     if (In_Help)
      break;
     Win_Quit();
     break;
    case FILE_ABOUT:
     DialogBox( hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout );
     break;
    case FILE_PRINT:
     Print_Buffer();
     break;
    case WINDOW_0:
    case WINDOW_1:
    case WINDOW_2:
    case WINDOW_3:
     Ch_Window(wParam-20);
	  break;
	 case WINDOW_SWITCH:
     Other_Window();
     break;
    case FILE_NEW:
     Zero();
     break;
    case EDIT_CLEAR:
     Reset();
     break;
    case EDIT_CUT:
     This_Cmd=C_Delete;
     Init_Entity();
     Sel_Range();
     if (!Err_Flag)
     {if (Blk_Mode)
       Blk_Rem_It(Past_String); else
       Rem_It(Past_String);
      Set_Changes();
	  }
     break;
    case EDIT_PASTE:
     Count=1;
     Paste();
	  Upd_Header();
     break;
    case RULER_0:
    case RULER_1:
    case RULER_2:
    case RULER_3:
    case RULER_4:
    case RULER_5:
    case RULER_6:
    case RULER_7:
    case RULER_8:
    case RULER_9:
     Readruler((char)('0'+wParam-40));
	  if (Show_Ruler)
      S_Ruler();
     break;
    case HELP:
     strcpy(Com_Arg,Do_Mapping("HELP.HLP"));
     Do_Help();
     break;
    case FONT_SELECT:
     Win_Font();
     break;
   }
	Out_Zap();
   Win_On_Curs();
   break;
  default:
   return DefWindowProc( hWnd, message, wParam, lParam );
 }
 return(0L);
}

void Win_Scroll(int T,int B,int S)
{RECT Rect;
 Rect.top=(T-1)*Char_Height;
 Rect.bottom=B*Char_Height;
 Rect.left=0;
 Rect.right=Scr_Width*Char_Width;
 ScrollWindow(Curr_Wnd,0,-S*Char_Height,&Rect,&Rect);
 Rfreshed=1;
 UpdateWindow(Curr_Wnd);
}

void Win_Clear()
{HDC hDC;
 RECT rect;
 HBRUSH hbr, hbrOld;
 hDC = GetDC(Curr_Wnd);
 GetClientRect( Curr_Wnd, (LPRECT)&rect );
 hbr = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
 hbrOld = (HBRUSH)SelectObject( hDC, (HANDLE)hbr );
 FillRect( hDC, (LPRECT)&rect, hbr );
 SelectObject( hDC, (HANDLE)hbrOld );
 DeleteObject( (HANDLE)hbr );
 ReleaseDC(Curr_Wnd, hDC );
}

void Win_Erase(int L,int C,int W)
{HDC hDC;
 RECT rect;
 HBRUSH hbr, hbrOld;
 hDC = GetDC(Curr_Wnd);
 rect.top=(L-1)*Char_Height;
 rect.left=(C-1)*Char_Width;
 rect.bottom=rect.top+Char_Height;
 rect.right=rect.left+W*Char_Width;
 hbr = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
 hbrOld = (HBRUSH)SelectObject( hDC, (HANDLE)hbr );
 FillRect( hDC, (LPRECT)&rect, hbr );
 SelectObject( hDC, (HANDLE)hbrOld );
 DeleteObject( (HANDLE)hbr );
 ReleaseDC(Curr_Wnd, hDC );
}

void Win_On_Curs()
{
 if (Cursor_On)
  Win_Off_Curs();
 SetCaretPos((int)(Char_Width*(My_Col-Scr_Shift-1)),
  (int)(Char_Height*(My_Line-1)));
 ShowCaret(Curr_Wnd);
 Cursor_On=1;
}

void Win_Off_Curs()
{
 HideCaret(Curr_Wnd );
 Cursor_On=0;
}

void Win_Getmem(HANDLE H,int A,int L)
{LPSTR P;
 P=GlobalLock(H);
 LockData(0);
 segread(&Rrv);
 movedata(HIWORD(P),LOWORD(P),Rrv.ds,A,L);
 UnlockData(0);
 GlobalUnlock(H);
}

void Win_Putmem(int A,HANDLE H,int L)
{LPSTR P;
 P=GlobalLock(H);
 LockData(0);
 segread(&Rrv);
 movedata(Rrv.ds,A,HIWORD(P),LOWORD(P),L);
 UnlockData(0);
 GlobalUnlock(H);
}

HANDLE Win_Alloc(int L)
{
 return GlobalAlloc(GMEM_MOVEABLE,(long)L);
}

void Win_Free(HANDLE H)
{
 GlobalFree(H);
}

void WinText(int L,int C,char *S,int A)
{HDC hDC;
 HANDLE Old_Font;
 hDC = GetDC(Curr_Wnd);
 Old_Font=SelectObject(hDC,Buf_Font);
 SetBkMode( hDC, OPAQUE);
 if (A&A_Reverse)
 {SetTextColor(hDC,GetSysColor(COLOR_WINDOW));
  SetBkColor(hDC,GetSysColor(COLOR_WINDOWTEXT));
 } else
 {SetTextColor( hDC, GetSysColor(COLOR_WINDOWTEXT) );
  SetBkColor(hDC,GetSysColor(COLOR_WINDOW));
 }
 TextOut(hDC,Char_Width*(C-1),Char_Height*(L-1),
  (char FAR *)S,strlen(S));
 SelectObject(hDC,Old_Font);
 ReleaseDC(Curr_Wnd, hDC );
}

void WinChar()
{static MSG msg;
 int I;
 static Last_Character, Last_Scancode;
 if (Ch_Count>0)
 {Ch_Count--;
  Character=Last_Character;
  Scancode=Last_Scancode;
  return;
 }
 Rfreshed=0;
 while (1)
 {if (Working_Flag)
   Clr_Working();
  Err_Flag=0;
  if (!GetMessage((LPMSG)&msg,(HWND)NULL,0,0))
  {Not_End=0;
   return;
  }
  if (msg.message==WM_CHAR)
  {Ch_Count=(int)(msg.lParam&0xFFFF)-1;
   Last_Character=Character=msg.wParam;
   if (Do_Debug)
    Diag("Character:Scancode=%d,Character=%d",Scancode,Character);
   return;
  } else
  if (msg.message==WM_KEYDOWN)
  {Ch_Count=(int)(msg.lParam&0xFFFF)-1;
	Last_Scancode=Scancode=(int)(msg.lParam>>16)&0xFF;
   if (Set_Num_Lock!=-1&&Scancode==0x45)
	{GetKeyboardState((BYTE far *)Keyb_State);
    Keyb_State[VK_NUMLOCK]=Set_Num_Lock;
	 SetKeyboardState((BYTE far *)Keyb_State);
   }
   if (Do_Debug)
    Diag("Scancode=%d,Window code=%d",Scancode,msg.wParam);
   for (I=0;I<Scan_Defs;I++)
   {if ((Last_Scancode=Scancode)==Scan_Tab[I].Scancode&&
	  !Scan_Tab[I].Flags&Ext_Code)
    {
     /*Diag("Scan code");*/
     Last_Character=Character=Scan_Tab[I].Char;
     return;
	 }
    if (msg.wParam==Scan_Tab[I].Scancode&&Scan_Tab[I].Flags&Window_Code)
    {
     /*Diag("Window Code");*/
     Last_Character=Character=Scan_Tab[I].Char;
     Last_Scancode=Scancode=msg.wParam;
     return;
    }
   }
	TranslateMessage((LPMSG)&msg);
   DispatchMessage((LPMSG)&msg);
  } else
  {TranslateMessage((LPMSG)&msg);
	DispatchMessage((LPMSG)&msg);
  }
  if (!Not_End)
   return;
 }
}

int Win_Yes_No(char *S)
{int Res;
  Res=MessageBox(Curr_Wnd,(LPSTR)S,(LPSTR)(char *)Txt_Warning,MB_YESNOCANCEL|
   MB_ICONEXCLAMATION|MB_APPLMODAL);
  UpdateWindow(Curr_Wnd);
  if (Res==IDCANCEL)
  {Error(3,Txt_Cancelled);
   return 0;
  }
  return Res==IDYES;
}

void Win_Error(char *S)
{
 MessageBox(Curr_Wnd,(LPSTR)S,NULL,MB_OK|MB_ICONEXCLAMATION|MB_APPLMODAL);
 UpdateWindow(Curr_Wnd);
}

void Win_P_Bar(long H,long L)
{if (L==0||H==0)
  SetScrollPos(Curr_Wnd,SB_VERT,0,1); else
  SetScrollPos(Curr_Wnd,SB_VERT,(int)((100L*(L-1))/H),1);
}

int Prompt_Input()
{int Res;
 Res=DialogBox(hInst,MAKEINTRESOURCE(PROMPTBOX),Curr_Wnd,lpprocPrompt);
 UpdateWindow(Curr_Wnd);
 return Res;
}

void Prompt_Init(HWND hDlg)
{
 SetDlgItemText(hDlg,PROMPTTEXT,(LPSTR)Dsk_Rec);
 Lst_Index=-1;
}

void Prompt_Exit(HWND hDlg)
{
 GetDlgItemText(hDlg,PROMPTINPUT,(LPSTR)Dsk_Rec,Blksize);
 if (Dsk_Rec[0]!=0)
 {C_Move(Lst_Input[0],Lst_Input[1],132*9);
  strcpy(Lst_Input[0],Dsk_Rec);
 }
 EndDialog(hDlg,TRUE);
}

BOOL FAR PASCAL _export Prompt(HWND hDlg,unsigned message,WORD wParam,LONG lParam)
{switch (message)
 {case WM_COMMAND:
   switch (wParam)
   {case PROMPTOK:
     Prompt_Exit(hDlg);
     break;
    case PROMPTCANCEL:
     EndDialog( hDlg, FALSE );
     break;
    case PROMPTINPUT:
     if (HIWORD(lParam)==0)
      Prompt_Exit(hDlg);
	  break;
	 case PROMPTNEXT:
     if (Lst_Index>0)
      SetDlgItemText(hDlg,PROMPTINPUT,(LPSTR)Lst_Input[--Lst_Index]);
     break;
    case PROMPTPREV:
     if (Lst_Index<9)
      SetDlgItemText(hDlg,PROMPTINPUT,(LPSTR)Lst_Input[++Lst_Index]);
     break;
   }
   break;
  case WM_INITDIALOG:
   Prompt_Init(hDlg);
   break;
  default:
   return FALSE;
 }
 return TRUE;
}

int Win_Newwindow(int B,int Show)
{
 return Make_Window(B,Show);
}

void Win_Topwindow(int B)
{
 SetFocus(Curr_Wnd=Buf_Wnd[B]);
}

void Swap_Window(int B)
{
 Out_Zap();
 Sav_Window(Cur_Buffer);
 Sav_Cur=Cur_Buffer;
 Cur_Buffer=B;
 Current=(&Main_Buffer);
 Load_Window(Cur_Buffer);
 Curr_Wnd=Buf_Wnd[B];
}

void Restore_Window(int I)
{
 Out_Zap();
 Sav_Window(Cur_Buffer);
 Cur_Buffer=Sav_Cur;
 Current=(&Main_Buffer);
 Load_Window(Cur_Buffer);
 Curr_Wnd=Buf_Wnd[Cur_Buffer];
}

void Win_Name(char *S)
{int I;
 HMENU hMenu;
 strcpy(Com_Arg,(char *)Txt_Sedt);
 if (S==NULL)
  strcat(Com_Arg,(char *)Txt_Untitled); else
  strcat(Com_Arg,S);
 SetWindowText(Curr_Wnd,Com_Arg);
 for (I=0;I<Maxbuffers;I++)
 if (Buf_Wnd[I]!=0)
 {hMenu=GetMenu(Buf_Wnd[I]);
  ChangeMenu(hMenu,20+Cur_Buffer,(LPSTR)(S==NULL?(char *)Txt_Untitled:S),
    20+Cur_Buffer,MF_CHANGE|MF_STRING);
 }
 hMenu=GetMenu(Buf_Wnd[Cur_Buffer]);
 EnableMenuItem(hMenu,20+Cur_Buffer,MF_GRAYED);
}

void Win_Put_Clip(long L)
{HANDLE M;
 LPSTR P;
 long I;
 int C;
 if (!OpenClipboard(Curr_Wnd))
 {Error(2,No_Clipboard_Txt);
  return;
 }
 if (!EmptyClipboard())
 {Error(2,Bad_Clipboard_Txt);
  CloseClipboard();
  return;
 }
 if ((M=GlobalAlloc(GMEM_MOVEABLE,(long)((I=Dl_Pointers[Past_String][1])+L+1)))
  ==(HGLOBAL)NULL)
 {Error(2,No_Clipboard_Memory_Txt);
  CloseClipboard();
  return;
 }
 P=GlobalLock(M);
 B_Goto(&Del,Dl_Pointers[Past_String][0]);
 while (I--)
 {if ((C=B_Get(&Del))=='\n')
   *(P++)=13;
  *(P++)=C;
 }
 *(P++)=0;
 GlobalUnlock(M);
 SetClipboardData(CF_TEXT,M);
 CloseClipboard();
}

void Win_Load_Clip()
{long L=0;
 HANDLE M;
 LPSTR P;
 int C, L_C=0;
 Erase_Del(Dl_Pointers[Past_String]);
 if (!OpenClipboard(Curr_Wnd))
 {Error(2,No_Clipboard_Txt);
  return;
 }
 if ((M=GetClipboardData(CF_TEXT))==(HANDLE)NULL)
 {Error(2,Empty_Clipboard_Txt);
  CloseClipboard();
  return;
 }
 P=GlobalLock(M);
 while ((C=*(P++))!=0)
 {if (L_C==13)
  {if (C!='\n')
   {B_Put(&Del,13);
    L++;
   }
  }
  if (C!=13)
  {B_Put(&Del,(char)C);
   L++;
  }
  L_C=C;
 }
 if (L_C==13)
 {B_Put(&Del,13);
  L++;
 }
 Dl_Pointers[Past_String][1]=L;
 GlobalUnlock(M);
 CloseClipboard();
}

void Win_Tempfile(char *F)
{
 GetTempFileName(0,(LPSTR)"SEDT",0,(LPSTR) F);
}

void Win_Del_Window()
{int I, B;
 HMENU hMenu;
 B=Cur_Buffer;
 Any_Changes[Cur_Buffer]=0;
 Zero();
 Not_End=0;
 My_Col=0;
 for (I=0;I<Maxbuffers;I++)
  if (I!=B&&Buf_Wnd[I]!=0)
  {Not_End=1;
   hMenu=GetMenu(Buf_Wnd[I]);
   ChangeMenu(hMenu,20+B,(LPSTR)(char *)Txt_Unused,20+B,MF_CHANGE|MF_STRING);
  }
 DestroyWindow(Curr_Wnd);
 Buf_Wnd[B]=0;
}

void Win_Quit()
{int I, B;
 HMENU hMenu;
 B=Cur_Buffer;
 if (Any_Changes[Cur_Buffer]&1)
  if (!Yes_No(Unsaved_Chng_Txt))
   return;
 Any_Changes[Cur_Buffer]=0;
 Zero();
 Not_End=0;
 My_Col=0;
 for (I=0;I<Maxbuffers;I++)
  if (I!=B&&Buf_Wnd[I]!=0)
  {Not_End=1;
   hMenu=GetMenu(Buf_Wnd[I]);
   ChangeMenu(hMenu,20+B,(LPSTR)(char *)Txt_Unused,20+B,MF_CHANGE|MF_STRING);
  }
 if (Not_End)
 {/*DeleteObject(Buf_Font);*/
  DestroyWindow(Curr_Wnd);
  Buf_Wnd[B]=0;
 } else
  PostQuitMessage(0);
 Not_End=1;
}

void Win_Font()
{RECT Rect;
 HDC hDC;
 CHOOSEFONT Choose;
 TEXTMETRIC TM;
 HANDLE Old_Font;
 LOGFONT SLF;
 hDC = GetDC(Curr_Wnd);
 DeleteObject(Buf_Font);
 SLF=LF;
 SLF.lfHeight=MulDiv(SLF.lfHeight,GetDeviceCaps(hDC,LOGPIXELSY),72);
 memset(&Choose,0,sizeof(CHOOSEFONT));
 Choose.lStructSize=sizeof(CHOOSEFONT);
 Choose.hwndOwner=Curr_Wnd;
 Choose.lpLogFont=&SLF;
 Choose.Flags=CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT|CF_ANSIONLY|CF_FIXEDPITCHONLY;
 Choose.nFontType=SCREEN_FONTTYPE;
 ChooseFont(&Choose);
 Buf_Font=CreateFontIndirect(&SLF);
 LF=SLF;
 LF.lfHeight=MulDiv(LF.lfHeight,72,GetDeviceCaps(hDC,LOGPIXELSY));
 Old_Font=SelectObject(hDC,Buf_Font);
 GetClientRect(Curr_Wnd,(LPRECT)&Rect);
 GetTextMetrics( hDC, (LPTEXTMETRIC)&TM );
 Scr_Width=(Rect.right+1)/(Char_Width=TM.tmAveCharWidth);
 Scr_Length=(Rect.bottom+1)/(Char_Height=TM.tmHeight);
 if (Cursor_On)
  HideCaret(Curr_Wnd);
 DestroyCaret();
 CreateCaret( Curr_Wnd, (HBITMAP)NULL,(int)Char_Width,
  (int)Char_Height);
 SetCaretPos((int)(Char_Width*(My_Col-Scr_Shift-1)),
  (int)(Char_Height*(My_Line-1)));
 if (Cursor_On)
  ShowCaret(Curr_Wnd);
 SelectObject(hDC,Old_Font);
 ReleaseDC(Curr_Wnd,hDC);
 Refresh();
}

void Win_Larger()
{RECT Rect;
 HDC hDC;
 TEXTMETRIC TM;
 HANDLE Old_Font;
 LOGFONT SLF;
 hDC = GetDC(Curr_Wnd);
 DeleteObject(Buf_Font);
 if (LF.lfHeight>0)
  LF.lfHeight*=-1;
 LF.lfHeight--;
 LF.lfWidth=0;
 SLF=LF;
 SLF.lfHeight=MulDiv(SLF.lfHeight,GetDeviceCaps(hDC,LOGPIXELSY),72);
 Buf_Font=CreateFontIndirect(&SLF);
 Old_Font=SelectObject(hDC,Buf_Font);
 GetClientRect(Curr_Wnd,(LPRECT)&Rect);
 GetTextMetrics( hDC, (LPTEXTMETRIC)&TM );
 Scr_Width=(Rect.right+1)/(Char_Width=TM.tmAveCharWidth);
 Scr_Length=(Rect.bottom+1)/(Char_Height=TM.tmHeight);
 if (Cursor_On)
  HideCaret(Curr_Wnd);
 DestroyCaret();
 CreateCaret( Curr_Wnd, (HBITMAP)NULL,(int)Char_Width,
  (int)Char_Height);
 SetCaretPos((int)(Char_Width*(My_Col-Scr_Shift-1)),
  (int)(Char_Height*(My_Line-1)));
 if (Cursor_On)
  ShowCaret(Curr_Wnd);
 SelectObject(hDC,Old_Font);
 ReleaseDC(Curr_Wnd,hDC);
 Refresh();
}

void Win_Smaller()
{RECT Rect;
 HDC hDC;
 TEXTMETRIC TM;
 HANDLE Old_Font;
 LOGFONT SLF;
 hDC = GetDC(Curr_Wnd);
 DeleteObject(Buf_Font);
 if (LF.lfHeight>0)
  LF.lfHeight*=-1;
 LF.lfHeight++;
 LF.lfWidth=0;
 SLF=LF;
 SLF.lfHeight=MulDiv(SLF.lfHeight,GetDeviceCaps(hDC,LOGPIXELSY),72);
 Buf_Font=CreateFontIndirect(&SLF);
 Old_Font=SelectObject(hDC,Buf_Font);
 GetClientRect(Curr_Wnd,(LPRECT)&Rect);
 GetTextMetrics( hDC, (LPTEXTMETRIC)&TM );
 Scr_Width=(Rect.right+1)/(Char_Width=TM.tmAveCharWidth);
 Scr_Length=(Rect.bottom+1)/(Char_Height=TM.tmHeight);
 if (Cursor_On)
  HideCaret(Curr_Wnd);
 DestroyCaret();
 CreateCaret( Curr_Wnd, (HBITMAP)NULL,(int)Char_Width,
  (int)Char_Height);
 SetCaretPos((int)(Char_Width*(My_Col-Scr_Shift-1)),
  (int)(Char_Height*(My_Line-1)));
 if (Cursor_On)
  ShowCaret(Curr_Wnd);
 SelectObject(hDC,Old_Font);
 ReleaseDC(Curr_Wnd,hDC);
 Refresh();
}

void Win_Set_Num_Lock(void)
{Save_My_Num_Lock=1;
 GetKeyboardState((BYTE far *)Keyb_State);
 Save_Their_Num_Lock=Keyb_State[VK_NUMLOCK];
 Keyb_State[VK_NUMLOCK]=1;
 SetKeyboardState((BYTE far *)Keyb_State);
}

void Win_Clear_Num_Lock(void)
{Save_My_Num_Lock=0;
 GetKeyboardState((BYTE far *)Keyb_State);
 Save_Their_Num_Lock=Keyb_State[VK_NUMLOCK];
 Keyb_State[VK_NUMLOCK]=0;
 SetKeyboardState((BYTE far *)Keyb_State);
}

void Win_Reset_Num_Lock(void)
{GetKeyboardState((BYTE far *)Keyb_State);
 Keyb_State[VK_NUMLOCK]=Save_Their_Num_Lock;
 SetKeyboardState((BYTE far *)Keyb_State);
}

int Open_File_Dialog()
{OPENFILENAME Ofn;
 Ofn.lStructSize=sizeof(OPENFILENAME);
 Ofn.hwndOwner=Curr_Wnd;
 Ofn.lpstrFilter=NULL;
 Ofn.lpstrCustomFilter=NULL;
 strcpy(Fil_String,"*.*");
 Ofn.lpstrFile=Fil_String;
 Ofn.nMaxFile=128;
 Ofn.lpstrFileTitle=NULL;
 Ofn.lpstrInitialDir=Sel_Dir;
 Ofn.lpstrTitle=NULL;
 Ofn.Flags=OFN_PATHMUSTEXIST|OFN_HIDEREADONLY;
 Ofn.lpstrDefExt=NULL;
 if (GetOpenFileName(&Ofn))
 {strcpy(Com_Arg,Fil_String);
  strncpy(Sel_Dir,Fil_String,Ofn.nFileOffset);
  Sel_Dir[Ofn.nFileOffset]=0;
  return 1;
 } else
  return 0;
}

int Save_As_Dialog()
{OPENFILENAME Ofn;
 Ofn.lStructSize=sizeof(OPENFILENAME);
 Ofn.hwndOwner=Curr_Wnd;
 Ofn.lpstrFilter=NULL;
 Ofn.lpstrCustomFilter=NULL;
 strcpy(Fil_String,Outfile);
 Ofn.lpstrFile=Fil_String;
 Ofn.nMaxFile=128;
 Ofn.lpstrFileTitle=NULL;
 Ofn.lpstrInitialDir=NULL;
 Ofn.lpstrTitle=NULL;
 Ofn.Flags=OFN_PATHMUSTEXIST|OFN_HIDEREADONLY;
 Ofn.lpstrDefExt=NULL;
 if (GetSaveFileName(&Ofn))
 {strcpy(Com_Arg,Fil_String);
  strncpy(Sel_Dir,Fil_String,Ofn.nFileOffset);
  Sel_Dir[Ofn.nFileOffset]=0;
  return 1;
 } else
  return 0;
}

void Print_Buffer()
{PRINTDLG Dlg;
 memset(&Dlg,0,sizeof(PRINTDLG));
 Dlg.lStructSize=sizeof(PRINTDLG);
 Dlg.hwndOwner=Curr_Wnd;
 Dlg.Flags=PD_RETURNDC|PD_NOPAGENUMS|PD_NOSELECTION;
 if (PrintDlg(&Dlg))
 {int X, Y, MaxX, MaxY, Char_Width, Char_Height;
  long Save_Col;
  char Output[100];
  int Out_Length=0, Out_X, Out_Y, I;
  TEXTMETRIC TM;
  DOCINFO DI;
  HANDLE Font;
  LOGFONT PLF;
  SetCursor(LoadCursor((HINSTANCE)NULL,IDC_WAIT));
  PLF=LF;
  PLF.lfHeight=MulDiv(PLF.lfHeight,GetDeviceCaps(Dlg.hDC,LOGPIXELSY),72);
  Font=CreateFontIndirect(&PLF);
  MaxX=GetDeviceCaps(Dlg.hDC,HORZRES);
  MaxY=GetDeviceCaps(Dlg.hDC,VERTRES);
  SelectObject(Dlg.hDC,Font);
  GetTextMetrics( Dlg.hDC, (LPTEXTMETRIC)&TM );
  Char_Width=TM.tmAveCharWidth;
  Char_Height=TM.tmHeight;
  DI.cbSize=sizeof(DOCINFO);
  DI.lpszDocName=Outfile;
  DI.lpszOutput=NULL;
  StartDoc(Dlg.hDC,&DI);
  StartPage(Dlg.hDC);
  B_Goto(Current,1);
  X=0;
  Y=Char_Height*4;
  Save_Col=My_Col;
  My_Col=1;
  while (Current->Pos<=Current->Bufsize)
  {char C, Rep[10];
   int L;
	C=B_Get(Current);
	if (C==12)
	{if (Out_Length>0)
	 {TextOut(Dlg.hDC,Out_X,Out_Y,Output,Out_Length);
	  Out_Length=0;
	 }
	 EndPage(Dlg.hDC);
	 StartPage(Dlg.hDC);
	 SelectObject(Dlg.hDC,Font);
	 X=0;
	 My_Col=1;
	 Y=Char_Height*4;
	} else
	if (C==10)
	{if (Out_Length>0)
	 {TextOut(Dlg.hDC,Out_X,Out_Y,Output,Out_Length);
	  Out_Length=0;
	 }
	 X=0;
	 My_Col=1;
	 Y+=Char_Height;
	 if (Y>MaxY-Char_Height*4&&MaxY!=0)
	 {EndPage(Dlg.hDC);
	  StartPage(Dlg.hDC);
	  SelectObject(Dlg.hDC,Font);
	  Y=Char_Height*4;
	 }
	} else
	if (C==13)
	 ; else
	if (X<=MaxX||MaxX==0)
	{L=Char_Rep(C,Rep);
	 if (Out_Length+L>100)
	 {TextOut(Dlg.hDC,Out_X,Out_Y,Output,Out_Length);
	  Out_Length=0;
	 }
	 if (Out_Length==0)
	 {Out_X=X;
	  Out_Y=Y;
    }
    for (I=0;I<=L;I++)
     Output[Out_Length+I]=Rep[I];
    Out_Length+=L;
	 X+=Char_Width*L;
    My_Col+=L;
   }
  }
  if (Out_Length>0)
  {TextOut(Dlg.hDC,Out_X,Out_Y,Output,Out_Length);
   Out_Length=0;
  }
  My_Col=Save_Col;
  EndPage(Dlg.hDC);
  EndDoc(Dlg.hDC);
  DeleteDC(Dlg.hDC);
  DeleteObject(Font);
  if (Dlg.hDevMode!=(HGLOBAL)NULL)
   GlobalFree(Dlg.hDevMode);
  if (Dlg.hDevNames!=(HGLOBAL)NULL)
   GlobalFree(Dlg.hDevNames);
  SetCursor(LoadCursor((HINSTANCE)NULL,IDC_IBEAM));
 }
}
