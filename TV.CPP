/*Copyright (c) Anker Berg-Sonne, 1985-1990*/
#include "proto.h"

#define Uses_TObject
#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TBackground
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TView
#define Uses_TWindow
#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_TDialog
#define Uses_TButton
#define Uses_TLabel
#define Uses_TGroup
#define Uses_MsgBox
#define Uses_TFrame
#define Uses_TScreen
#define Uses_TDisplay
#define Uses_TFileDialog
#define Uses_TInputLine
#define Uses_THistory
#include <tv.h>

extern unsigned _stklen=4*1024U;

class Sedt_Window:public TWindow
{public:
  char Title[61];
  Sedt_Window(const TRect& Bounds,const char *Title,short Number);
  virtual void handleEvent(TEvent& E);
};

class Sedt_Window *Buf_View[Maxbuffers];
class TScrollBar *Buf_Scroll_Bar[Maxbuffers];
int Do_Scroll=0;
const int cmMyFileOpen=200;
const int cmMyFileClear=201;
const int cmMyQuit=202;
const int cmMyFileSave=203;
const int cmMyFileSaveAs=204;
const int cmMyFileInclude=205;
const int cmMyUndo=206;
const int cmMyCut=207;
const int cmMyCopy=208;
const int cmMyPaste=209;
const int cmMyClear=210;
const int cmMyFind=211;
const int cmMyAgain=212;
const int cmMyLine=213;
const int cmNext_Saved=214;
const int cmPrev_Saved=215;

class Sedt_Interior : public TView
{
 public:
  int Window_Buffer;
  Sedt_Interior *Self;
  Sedt_Interior( const TRect& bounds );
  virtual void draw();
  virtual void handleEvent(TEvent& E);
};

class Sedt_App:public TApplication
{public:
  Sedt_App();
  Sedt_Interior *Sedt_Newwindow(int Window);
  void Sedt_Error(char *T);
  virtual void handleEvent(TEvent& E);
  void Fast Suspend(void);
  void Fast Resume(void);
  static TMenuBar *initMenuBar(TRect r);
  static TStatusLine *initStatusLine(TRect r);
  void tile();
  void cascade();
};

class TDialog *This_Key_Box;

Sedt_App::Sedt_App() :
 TProgInit(Sedt_App::initStatusLine,Sedt_App::initMenuBar,
  Sedt_App::initDeskTop)
{
}

void Sedt_App::handleEvent(TEvent& E)
{TApplication::handleEvent(E);
 if (E.what==evCommand)
 {/*Diag("Sedt_App::handleEvent:command=%d",E.message.command);*/
  switch (E.message.command)
  {case cmCascade:
    cascade();
    break;
   case cmTile:
    tile();
    break;
   case cmMyFileOpen:
    {TFileDialog *d=(TFileDialog *)validView(
     new TFileDialog("*.*","Edit File","~N~ame",fdOpenButton,100));
     if (d!=0&&deskTop->execView(d)!=cmCancel)
     {d->getFileName(Dsk_Rec);
      Edit_File(Dsk_Rec);
      Set_Cursor(My_Line,My_Col);
     }
     destroy(d);
    }
    break;
   case cmMyFileClear:
    Zero();
    break;
   case cmMyFileSave:
    {char *S;
     if ((S=Outfile)==NULL||S[0]==0)
      Error(2,No_Output_File_Txt); else
      Wr_Buffer(S);
     if (!Err_Flag)
      Clr_Changes();
    }
    break;
   case cmMyFileSaveAs:
    {TFileDialog *d=(TFileDialog *)validView(
     new TFileDialog("*.*","Edit File","~N~ame",fdOKButton,100));
     if (d!=0&&deskTop->execView(d)!=cmCancel)
     {d->getFileName(New_File);
      Wr_Buffer(Outfile=New_File);
      TV_Name(Outfile);
     }
     destroy(d);
    }
    break;
   case cmMyFileInclude:
    {TFileDialog *d=(TFileDialog *)validView(
     new TFileDialog("*.*","Edit File","~N~ame",fdOpenButton,100));
     if (d!=0&&deskTop->execView(d)!=cmCancel)
     {d->getFileName(Dsk_Rec);
      Include_File(Dsk_Rec,File_Not_Found_Txt);
     }
     destroy(d);
    }
    break;
   case cmMyQuit:
    Quit();
    break;
   case cmMyUndo:
    Undo();
    break;
   case cmMyCut:
    This_Cmd=C_Delete;
    Init_Entity();
    E_Type=E_Paste;
    Sel_Range();
    if (!Err_Flag)
    {if (Blk_Mode)
      Blk_Rem_It(Past_String); else
      Rem_It(Past_String);
    }
    break;
   case cmMyCopy:
    This_Cmd=C_Unknown;
    Init_Entity();
    Sel_Range();
    Clear_Region();
    if (!Err_Flag)
    {if (Blk_Mode)
      Blk_Rem_It(Past_String); else
      Rem_It(Past_String);
    }
    break;
   case cmMyPaste:
    Count=1;
    Paste();
    break;
   case cmMyClear:
    Reset();
    break;
   case cmMyFind:
    Init_Entity();
    strcpy(Dsk_Rec,"Search string:");
    if (!Prompt_Input())
     break;
    strcpy(Com_Arg,Dsk_Rec);
    if (Com_Arg[0]!=0)
    {Arg_Length=strlen(Com_Arg);
     Ins_Del(Dl_Pointers[Search_String]);
    }
    Set_Region();
    Find_Next();
    Move_Entity();
    break;
   case cmMyAgain:
    Com_Arg[0]=0;
    Init_Entity();
    Set_Region();
    Find_Next();
    Move_Entity();
    break;
   case cmMyLine:
    strcpy(Dsk_Rec,"Line number:");
    if (!Prompt_Input())
     break;
    Init_Entity();
    Ent_Count=atol(Dsk_Rec);
    Abs_Line();
    Move_Entity();
    break;
   default:
    return;
  }
  Upd_Header();
  Set_Cursor(My_Line,My_Col);
  Err_Flag=0;
  clearEvent(E);
 }
}

void Sedt_App::tile()
{deskTop->tile(deskTop->getExtent());
}

void Sedt_App::cascade()
{deskTop->cascade(deskTop->getExtent());
}

Sedt_Interior::Sedt_Interior( const TRect& bounds ) : TView( bounds )
{growMode = gfGrowHiX | gfGrowHiY;
 options = options|ofFramed|ofSelectable;
 eventMask|=evBroadcast|evMouseUp|evMouseDown|evMouseMove|evMouseAuto;
 setCursor(0,0);
}

void Sedt_Interior::handleEvent(TEvent& E)
{/*if (E.what&evMessage)
 {if (E.what==evCommand)
   Diag("Sedt_Interior::handleEvent:Command,E.command=%d,infoPtr=%lx",
    E.message.command,E.message.infoPtr); else
  if (E.what==evBroadcast)
   Diag("Sedt_Interior::handleEvent:Broadcast,command=%d,infoPtr=%lx,",
    E.message.command,E.message.infoPtr);
 } else
 if (E.what&evMouse)
 {if (E.what==evMouseDown)
   Diag("Sedt_Interior::handleEvent:MouseDown,Line=%d,Col=%d",
    makeLocal(E.mouse.where).y,makeLocal(E.mouse.where).x); else
  if (E.what==evMouseUp)
   Diag("Sedt_Interior::handleEvent:MouseUp,Line=%d,Col=%d",
    makeLocal(E.mouse.where).y,makeLocal(E.mouse.where).x); else
  if (E.what==evMouseMove)
   Diag("Sedt_Interior::handleEvent:MouseMove,Line=%d,Col=%d",
    makeLocal(E.mouse.where).y,makeLocal(E.mouse.where).x); else
  if (E.what==evMouseAuto)
   Diag("Sedt_Interior::handleEvent:MouseAuto,Line=%d,Col=%d",
    makeLocal(E.mouse.where).y,makeLocal(E.mouse.where).x);
 }*/
 switch (E.what)
 {case evBroadcast:
   switch(E.message.command)
   {case cmReceivedFocus:
     if (E.message.infoPtr==Self&&Window_Buffer!=Cur_Buffer)
     {hideCursor();
      Ch_Window(Window_Buffer);
      showCursor();
     }
     break;
   }
   break;
  case evMouseUp:
  case evMouseDown:
  case evMouseMove:
  case evMouseAuto:
   Mouse_Execute(makeLocal(E.mouse.where).y+1,makeLocal(E.mouse.where).x+1,
    E.mouse.buttons&mbLeftButton,E.mouse.buttons&mbRightButton,
    E.what==evMouseMove||E.what==evMouseAuto);
   break;
 }
 TView::handleEvent(E);
}

void Sedt_Window::handleEvent(TEvent& E)
{switch (E.what)
 {case evCommand:
   switch(E.message.command)
   {case cmClose:
     break;
   }
   break;
  case evBroadcast:
   switch (E.message.command)
   {case cmScrollBarClicked:
     Do_Scroll=1;
     break;
    case cmScrollBarChanged:
     if (Do_Scroll==0)
      break;
     hideCursor();
     Init_Entity();
     Ent_Count=((class TScrollBar *)E.message.infoPtr)->value;
     Abs_Line();
     Move_Entity();
     Upd_Header();
     Set_Cursor(My_Line,My_Col);
     showCursor();
     clearEvent(E);
     Do_Scroll=0;
     break;
   }
   break;
 }
 TWindow::handleEvent(E);
}

void Sedt_Interior::draw()
{int Sav_Buffer;
 hideCursor();
 if (Buf_Draw_OK[Cur_Buffer])
 {if (Window_Buffer!=Cur_Buffer)
  {Sav_Window(Sav_Buffer=Cur_Buffer);
   Load_Window(Cur_Buffer=Window_Buffer);
  } else
   Sav_Buffer= -1;
  TRect R=getExtent();
  if (R.b.x!=Scr_Width||R.b.y!=Scr_Length)
  {Scr_Width=R.b.x;
   Scr_Length=R.b.y;
   TV_Clear();
   Refresh();
   Set_Cursor(My_Line,My_Col);
  } else
  {S_Header();
   if (Show_Ruler)
    S_Ruler;
   Fill_Screen(Rel_First,Rel_Last);
  }
  if (Sav_Buffer>=0)
  {Sav_Window(Cur_Buffer);
   Load_Window(Cur_Buffer=Sav_Buffer);
  }
 }
 showCursor();
}

Sedt_Window::Sedt_Window(const TRect& Bounds,const char *Title,
 short Number):
  TWindow(Bounds,Title,Number),
  TWindowInit(Sedt_Window::initFrame)
{options|=ofTileable;
 flags&=~wfClose;
 Buf_Scroll_Bar[Cur_Buffer]=standardScrollBar(sbVertical);
}

Sedt_Interior *Sedt_App::Sedt_Newwindow(int Buffer)
{TRect R;
 TPoint Min, Max;
 Sedt_Window *window;
 Sedt_Interior *interior;
 deskTop->sizeLimits(Min,Max);
 Max.y-=2;
 R.a.x=min(Max.x-24,Buffer);
 R.a.y=min(Max.y-6,Buffer);
 R.b=Max;
 if ((Buf_View[Buffer]=window=new Sedt_Window(R,"Untitled",0))==NULL)
  return NULL;
 deskTop->insert(window);
 Scr_Width=R.b.x-3-R.a.x;
 Scr_Length=R.b.y-2-R.a.y;
 R=window->getClipRect();
 R.grow(-1,-1);
 R.b.x-=1;
 window->insert(interior=new Sedt_Interior(R));
 interior->Self=interior;
 Buf_Wnd[Buffer]=interior;
 interior->Window_Buffer=Buffer;
 return interior;
}

void Sedt_App::Sedt_Error(char *T)
{messageBox(T,mfOKButton);
}

Sedt_App *This_Sedt_App;

void* Fast TV_Newwindow(int Buffer)
{
 /*Diag("TV_Newwindow:Buffer=%d",Buffer);*/
 Buf_Draw_OK[Cur_Buffer]=0;
 Buf_Wnd[Buffer]=This_Sedt_App->Sedt_Newwindow(Buffer);
 TV_Top_Window(Buffer);
 return Buf_Wnd[Buffer];
}

void Fast TV_Draw_OK(void)
{Buf_Draw_OK[Cur_Buffer]=1;
}

void Fast TV_Top_Window(int Buffer)
{class Sedt_Interior *W;
 W=(class Sedt_Interior *)Buf_Wnd[Buffer];
 Buf_View[Buffer]->select();
 W->select();
}

void Fast TV_Error(char *T)
{/*Diag("TV_Error:T=%s",T);*/
 This_Sedt_App->Sedt_Error(T);
}

void Fast TV_Text(int Line,int Col,char *String,int Attributes)
{class Sedt_Interior *W;
 /*Diag("TV_Text:Line=%d,Col=%d,String=%s",Line,Col,String);*/
 if ((W=(class Sedt_Interior *)Buf_Wnd[Cur_Buffer])!=NULL)
  W->writeStr(Col-1,Line-1,String,Attributes);
}

void Fast TV_Erase(int Line,int Col,int Length)
{class Sedt_Interior *W;
 /*Diag("TV_Erase:Line=%d,Col=%d,Length=%d",Line,Col,Length)*/
 if ((W=(class Sedt_Interior *)Buf_Wnd[Cur_Buffer])!=NULL)
  W->writeChar(Col-1,Line-1,' ',6,Length);
}

void Fast TV_Clear()
{class Sedt_Interior *W;
 TRect R;
 /*Diag("TV_Clear");*/
 if ((W=(class Sedt_Interior *)Buf_Wnd[Cur_Buffer])!=NULL)
 {R=W->getExtent();
  {int L;
   for (L=0;L<R.b.y;L++)
    W->writeChar(0,L,' ',6,R.b.x);
  }
 }
}

void Fast TV_Cursor(int Line,int Col)
{class Sedt_Interior *W;
 if ((W=(class Sedt_Interior *)Buf_Wnd[Cur_Buffer])!=NULL)
  W->setCursor(Col-1,Line-1);
}

void Fast TV_Char()
{if (!Replace_Mode)
  ((class Sedt_Interior *)Buf_Wnd[Cur_Buffer])->blockCursor(); else
  ((class Sedt_Interior *)Buf_Wnd[Cur_Buffer])->normalCursor();
 ((class Sedt_Interior *)Buf_Wnd[Cur_Buffer])->showCursor();
 do
 {TEvent E;
  This_Sedt_App->getEvent(E);
  if ((E.what&evKeyDown)!=0/*&&Mouse_OK*/)
  {Scancode=E.keyDown.charScan.scanCode;
   Character=E.keyDown.charScan.charCode;
   /*Diag("TV_Char:Character=%d,Scancode=%d",Character,Scancode);*/
   if ((Scancode>=16&&Scancode<=25)||(Scancode>=30&&Scancode<=38)||
    (Scancode>=44&&Scancode<=50))
    This_Sedt_App->handleEvent(E);
   if (E.what!=evNothing)
   {This_Sedt_App->clearEvent(E);
    ((class Sedt_Interior *)Buf_Wnd[Cur_Buffer])->hideCursor();
    return;
   }
  } else
  {This_Sedt_App->handleEvent(E);
   if (E.what!=evNothing)
    This_Sedt_App->eventError(E);
  }
  if( E.what != evNothing )
   This_Sedt_App->eventError(E);
  /*Diag("TV_Char:Not_End=%d",Not_End);*/
 } while (Not_End);
}

class Prompt_Dialog:public TDialog
{public:
  int Lst_Index;
  TView *Input_Box;
  Prompt_Dialog(const TRect& bounds, const char *aTitle);
  virtual void handleEvent(TEvent& E);
};

Prompt_Dialog::Prompt_Dialog(const TRect& bounds, const char *aTitle):
 TDialog(bounds,aTitle),TWindowInit(Prompt_Dialog::initFrame )
{Lst_Index=-1;
}

void Prompt_Dialog::handleEvent(TEvent& E)
{TDialog::handleEvent(E);
 if (E.what==evCommand)
 {/*Diag("Prompt_Dialog::handleEvent:command=%d",E.message.command);*/
  switch (E.message.command)
  {case cmNext_Saved:
    if (Lst_Index>0)
     setData(Lst_Input[--Lst_Index]);
    Input_Box->select();
    clearEvent(E);
    break;
   case cmPrev_Saved:
    if (Lst_Index<9)
     setData(Lst_Input[++Lst_Index]);
    Input_Box->select();
    clearEvent(E);
    break;
  }
 } else
 if ((E.what&evKeyDown)!=0)
 {switch (E.keyDown.keyCode)
  {case kbUp:
    if (Lst_Index<9)
     setData(Lst_Input[++Lst_Index]);
    clearEvent(E);
    break;
   case kbDown:
    if (Lst_Index>0)
     setData(Lst_Input[--Lst_Index]);
    clearEvent(E);
    break;
  }
 }
}

int Fast Prompt_Input(void)
{int Result;
 TView *control;
 TRect R(0,0,46,8);
 R.move((TProgram::deskTop->size.x-R.b.x)/2,
  (TProgram::deskTop->size.y-R.b.y)/2);
 Prompt_Dialog* dlg = new Prompt_Dialog(R,"Input");
 if (!dlg) return 0;
 dlg->Input_Box=control=new TInputLine(TRect(2,3,43,4), Blksize+1);
 dlg->insert(control);
 control=new TButton(TRect(1,5,11,7),"Prev.",cmPrev_Saved,bfNormal);
 dlg->insert(control);
 control=new TButton(TRect(12,5,22,7),"Next",cmNext_Saved,bfNormal);
 dlg->insert(control);
 control=new TButton(TRect(23,5,33,7),"OK",cmOK,bfDefault);
 dlg->insert(control);
 control=new TButton(TRect(34,5,44,7),"Cancel",cmCancel,bfNormal);
 dlg->insert(control);
 dlg->insert(new TStaticText(TRect(2,2,42,3),Dsk_Rec));
 dlg->selectNext(False);
 Result=This_Sedt_App->execView(dlg);
 if (Result!=cmCancel)
  dlg->getData(Dsk_Rec);
 if (Dsk_Rec[0]!=0&&strcmp(Dsk_Rec,Lst_Input[0])!=0)
 {C_Move(Lst_Input[0],Lst_Input[1],132*9);
  strcpy(Lst_Input[0],Dsk_Rec);
 }
 TObject::destroy(dlg);
 return Result==cmOK;
}

int Fast TV_Yes_No(char *String)
{int Result;
 Result=messageBox(String,mfYesButton|mfNoButton|mfCancelButton);
 if (Result==cmYes)
  return 1; else
 if (Result==cmCancel)
 {Err_Flag=1;
  return 0;
 } else
  return 0;
}

void Fast TV_Name(char *String)
{if (String==NULL)
  strcpy(Buf_View[Cur_Buffer]->Title,"Untitled"); else
  strncpy(Buf_View[Cur_Buffer]->Title,String,60);
 Buf_View[Cur_Buffer]->title=Buf_View[Cur_Buffer]->Title;
 Buf_View[Cur_Buffer]->frame->draw();
}

void Fast TV_Open_Key_Box()
{/*This_Key_Box=new TDialog(TRect(0,0,40,5),"Type a Key");*/
}

void Fast TV_Close_Key_Box()
{/*Sedt_App::destroy(This_Key_Box);*/
}

void Fast Sedt_App::Suspend(void)
{suspend();
 system("cls");
}

void Fast TV_Suspend(void)
{This_Sedt_App->Suspend();
}

void Fast Sedt_App::Resume(void)
{resume();
 redraw();
}

void Fast TV_Resume(void)
{This_Sedt_App->Resume();
}

int main(int Argc,char **Argv)
{_OvrInitEms(0,0,0);
 This_Sedt_App=new Sedt_App();
 Run_Sedt(Argc,Argv);
 gotoxy(1,1);
 return Return_Value;
}

void Fast TV_P_Bar(long H,long L)
{if (L==0||H==0)
  Buf_Scroll_Bar[Cur_Buffer]->setParams(1,1,1,1,1); else
  Buf_Scroll_Bar[Cur_Buffer]->setParams((int)L,1,(int)H,Rel_Last-Rel_First,1);
}

TMenuBar *Sedt_App::initMenuBar( TRect r )
{r.b.y = r.a.y + 1;
 return new TMenuBar( r,
   *new TSubMenu("~F~ile",kbAltF)+
    *new TMenuItem("~O~pen..",cmMyFileOpen,cmMyFileOpen,hcNoContext,"")+
    *new TMenuItem("~C~lear",cmMyFileClear,cmMyFileClear,hcNoContext,"")+
    *new TMenuItem("~S~ave",cmMyFileSave,cmMyFileSave,hcNoContext,"")+
    *new TMenuItem("Save ~A~s...",cmMyFileSaveAs,cmMyFileSaveAs,hcNoContext,"")+
    *new TMenuItem("~I~nclude...",cmMyFileInclude,cmMyFileInclude,hcNoContext,"")+
    newLine()+
    *new TMenuItem("~E~xit",cmMyQuit,cmMyQuit,hcNoContext,"")+
   *new TSubMenu("~E~dit",kbAltE)+
    *new TMenuItem("~U~ndo",cmMyUndo,cmMyUndo,hcNoContext,"")+
    newLine()+
    *new TMenuItem("~C~ut",cmMyCut,cmMyCut,hcNoContext,"")+
    *new TMenuItem("C~o~py",cmMyCopy,cmMyCopy,hcNoContext,"")+
    *new TMenuItem("~P~aste",cmMyPaste,cmMyPaste,hcNoContext,"")+
    *new TMenuItem("C~l~ear",cmMyClear,cmMyClear,hcNoContext,"")+
   *new TSubMenu("~S~earch",kbAltS)+
    *new TMenuItem("~F~ind...",cmMyFind,cmMyFind,hcNoContext,"")+
    *new TMenuItem("Search ~A~gain",cmMyAgain,cmMyAgain,hcNoContext,"")+
    newLine()+
    *new TMenuItem("~L~ine number...",cmMyLine,cmMyLine,hcNoContext,"")+
   *new TSubMenu("~W~indow",kbAltW)+
    *new TMenuItem("~N~ext",cmNext,cmNext,hcNoContext,"")+
    *new TMenuItem("~Z~oom",cmZoom,cmZoom,hcNoContext,"")+
    *new TMenuItem("~T~ile",cmTile,cmTile,hcNoContext,"")+
    *new TMenuItem("~C~ascade",cmCascade,cmCascade,hcNoContext,"")
  );
}

TStatusLine *Sedt_App::initStatusLine(TRect r)
{r.a.y=r.b.y-1;
 return new TStatusLine(r,*new TStatusDef(0,0xFFFF)+
  *new TStatusItem("~F1~ Help",0,0));
}

void Fast TV_Zoom(void)
{TEvent E;
 E.what=evCommand;
 E.message.command=cmZoom;
 E.message.infoPtr=Buf_View[Cur_Buffer];
 Buf_View[Cur_Buffer]->putEvent(E);
}

void Fast TV_Tile(void)
{TEvent E;
 E.what=evCommand;
 E.message.command=cmTile;
 E.message.infoPtr=Buf_View[Cur_Buffer];
 Buf_View[Cur_Buffer]->putEvent(E);
}

void Fast TV_Cascade(void)
{TEvent E;
 E.what=evCommand;
 E.message.command=cmCascade;
 E.message.infoPtr=Buf_View[Cur_Buffer];
 Buf_View[Cur_Buffer]->putEvent(E);
}

void Fast TV_Ega43(void)
{This_Sedt_App->setScreenMode(TDisplay::smCO80|TDisplay::smFont8x8);
}

void Fast TV_Exit(int Result)
{This_Sedt_App->destroy(This_Sedt_App);
 exit(Result);
}