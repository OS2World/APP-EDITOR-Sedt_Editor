#include <aesbind.h>
#include <obdefs.h>
#include <gemdefs.h>
#include <osbind.h>
#include <ctype.h>
#include "sedtdef.h"
#include "vdudef.h"
#include "bufdef.h"
#include "sedt.h"
#define NULL 0L
#define MDESK 3

#define Txt_Fun(N,T) extern char *N;
#define Int_Fun(N,V) extern int N;
#define Chr_Fun(N,V) extern char N;
#include "text.h"
#include "text1.h"

#define WI_KIND SIZER|MOVER|FULLER|NAME|UPARROW|DNARROW|VSLIDE|CLOSER

#define NO_WINDOW (-1)

long _stksize=4000;

int gl_apid, Curs_On=0, Curs_Line, Curs_Col,
 Rfreshed=0, Sav_Cur, Has_GDOS, Dummy, Hi_Res;
 
char G_String[132], D_String[80], *index();
char *Do_Mapping();

OBJECT *Menu_Addr, *Prompt_Addr, *About_Addr;
OBJECT *P_Object;
TEDINFO *P_Text;
Rect Tempbox={0,0,0,0};
Prect Tempptr={&Tempbox.x,&Tempbox.y,&Tempbox.w,&Tempbox.h};
   
extern int Out_Count, S_Line, S_Col, Home, Scr_Shift,
 Ent_Direction, Err_Flag, Rel_First, Rel_Last, Blk_Mode,
 My_Line, His_Line, His_Col, Cur_Buffer, Screen_Shift, Offset, 
 Indent, Word_Wrap, Left_Margin, Right_Margin, Tabs[], Auto_Margin, Justify,
 Screen_Shift, Offset, His_Attributes, Out_Count,
 Working_Flag, This_Cmd, Not_End, Do_Shift, Count, Err_Display, Float_Cursor,
 Nosub, Nobak, Doreturn, Mem_Entries, First_Line, Any_Changes[], 
 Lst_Current, Def_Font, In_Help, Scr_Width, Scr_Length, Auto_Saved;
extern char *Infile, *Outfile, Lst_Input[][132];
extern long My_Pos, Points[], Beg_Pos, End_Pos, This_Line,
 Buf_Lines, Mem_Limit, Help_Pos, Ent_Count, My_Col;
extern struct Buffer *Current;
extern char *Version, Com_Arg[], New_File[132];
extern char My_Screen[][Maxcol+1], My_Attributes[][Maxcol+1];
extern char Dsk_Rec[];

int	gl_hchar;
int	gl_wchar;
int	gl_wbox;
int	gl_hbox;	/* system sizes */

int 	phys_handle;	/* physical workstation handle */
int 	handle;		/* virtual workstation handle */
int	wi_handle[Maxbuffers];	/* window handle */
int	top_window;	/* handle of topped window */

int	xdesk,ydesk,hdesk,wdesk;
int	xold,yold,hold,wold;
int	xwork,ywork,hwork,wwork;	/* desktop and work areas */

int gr_mkmx, gr_mkmy, gr_mkmstate, gr_mkkstate; /*mouse position params*/

int	msgbuff[8];	/* event message buffer */
int	altbuff[8];	/* alternate event message buffer */
int	keycode;	/* keycode returned by event-keyboard */
int	mx,my, omx, omy;		/* mouse x and y pos. */
int	butdown;	/* button state tested for, UP/DOWN */
int     mobutton;       /* indicator of which button */
int	ret;		/* dummy return variable */

int	hidden;		/* current state of cursor */

int	fulled;		/* current state of window */

int	contrl[12];
int	intin[128];
int	ptsin[128];
int	intout[128];
int	ptsout[128];	/* storage wasted for idiotic bindings */

int work_in[11];	/* Input to GSX parameter array */
int work_out[57];	/* Output from GSX parameter array */
int pxyarray[10];	/* input point array */

int TC_Height, TC_Width, T_Baseline,
 Buf_Points, Cur_Points, Lst_Index;

hide_mouse()
{if (!hidden)
 {graf_mouse(M_OFF,0x0L);
  hidden=1;
 }
}

show_mouse()
{if (hidden)
 {graf_mouse(M_ON,0x0L);
  hidden=0;
 }
}

open_vwork()
{int i;
 Has_GDOS=GDOS_In();
 work_in[0]=Getrez()+2;
 for (i=1;i<10;work_in[i++]=1);
 work_in[10]=2;
 handle=phys_handle;
 v_opnvwk(work_in,&handle,work_out);
 Hi_Res=work_out[1]==399;
 vswr_mode(handle,1);
 vsf_interior(handle,2);
 vsf_style(handle,8);
 vsf_color(handle,0);
 vst_effects(handle,0);
}

set_clip(x,y,w,h)
int x,y,w,h;
{int clip[4];
 clip[0]=x;
 clip[1]=y;
 clip[2]=x+w-1;
 clip[3]=y+h-1;
 vs_clip(handle,1,clip);
}

clear_clip()
{int clip[4];
 clip[0]=xdesk;
 clip[1]=ydesk;
 clip[2]=wdesk;
 clip[3]=hdesk;
 vs_clip(handle,0,clip);
}

open_window(B)
int B;
{top_window=wi_handle[B]=
  wind_create(WI_KIND,xdesk,ydesk,wdesk,hdesk);
 GEM_Name(top_window,Txt_Untitled);
 wind_set(top_window,WF_NAME,Editor_Txt,0,0);
 graf_growbox(xdesk+wdesk/2,ydesk+hdesk/2,gl_wbox,gl_hbox,xdesk,ydesk,wdesk,
  hdesk);
 wind_open(top_window,xdesk,ydesk,wdesk,hdesk);
 wind_get(top_window,WF_WORKXYWH,&xwork,&ywork,&wwork,&hwork);
 Cur_Points=Buf_Points=vst_point(handle,Def_Font?Def_Font:(
  Hi_Res?10:9),&Dummy,&Dummy,&Dummy,&Dummy);
 vqt_attributes(handle,work_out);
 TC_Width=work_out[8];
 TC_Height=work_out[9];
 T_Baseline=TC_Height-work_out[7];
 Scr_Width=wwork/TC_Width;
 Scr_Length=(hwork-1)/TC_Height;
 set_clip(xwork,ywork,wwork,hwork);
}

do_redraw(W,xc,yc,wc,hc)
int W,xc,yc,wc,hc;
{int B, L, C, pxyarray[4], E_L, E_C, S_Hidden, Sav_Current;
 char *Att, *Scr;
 GRECT t1,t2;
 if (Rfreshed&&W==top_window)
 {Rfreshed=0;
  return;
 }
 wind_update(1);
 if (!(S_Hidden=hidden))
  hide_mouse();
 for (B=0;B<Maxbuffers;B++)
  if (wi_handle[B]==W)
   break;
 Sav_Window(Cur_Buffer);
 Load_Window(B);
 t2.g_x=xc;
 t2.g_y=yc;
 t2.g_w=wc;
 t2.g_h=hc;
 vst_point(handle,Buf_Points,&Dummy,&Dummy,&Dummy,&Dummy);
 wind_get(W,WF_WORKXYWH,&xwork,&ywork,&wwork,&hwork);
 Scr_Width=wwork/TC_Width;
 Scr_Length=(hwork-1)/TC_Height;
 wind_get(W,WF_FIRSTXYWH,&t1.g_x,&t1.g_y,&t1.g_w,&t1.g_h);
 while (t1.g_w && t1.g_h) 
 {if (rc_intersect(&t2,&t1)) 
  {set_clip(t1.g_x,t1.g_y,t1.g_w,t1.g_h);
   if (Out_Count)
    Out_Zap();
   pxyarray[0]=t1.g_x;
   pxyarray[1]=t1.g_y;
   pxyarray[2]=t1.g_x+t1.g_w-1;
   pxyarray[3]=t1.g_y+t1.g_h-1;
   if (pxyarray[2]>pxyarray[0]&&pxyarray[3]>pxyarray[1])
    vr_recfl(handle,pxyarray);
   E_L=(t1.g_y+t1.g_h-ywork-1)/TC_Height+1;
   for (L=(t1.g_y-ywork)/TC_Height+1;L<=E_L;L++)
   if (L>=1&&L<=Scr_Length)
   {Scr=My_Screen[L];
    Att=My_Attributes[L];
    E_C=(t1.g_x+t1.g_w-xwork-1)/TC_Width+1;
    for (C=(t1.g_x-xwork)/TC_Width+1;C<=E_C;C++)
    if (C>=1&&C<=Scr_Width)
    {if (Scr[C]==' '&Att[C]==0)
     {if (Out_Count)
       Out_Zap();
     } else
     {if (Out_Count==0)
      {S_Line=L;
       S_Col=C;
       His_Attributes=Att[C];
       Out_C(Scr[C]);
      } else
      {if (His_Attributes!=Att[C])
       {Out_Zap();
        S_Line=L;
        S_Col=C;
        His_Attributes=Att[C];
       }
       Out_C(Scr[C]);
      }
     }
    }
    Out_Zap();
   }
  }
  wind_get(W,WF_NEXTXYWH,&t1.g_x,&t1.g_y,&t1.g_w,&t1.g_h);
 }
 Load_Window(Cur_Buffer);
 vst_point(handle,Buf_Points,&Dummy,&Dummy,&Dummy,&Dummy);
 wind_get(top_window,WF_WORKXYWH,&xwork,&ywork,&wwork,&hwork);
 Scr_Width=wwork/TC_Width;
 Scr_Length=(hwork-1)/TC_Height;
 set_clip(xwork,ywork,wwork,hwork);
 wind_update(0);
 if (!S_Hidden)
  show_mouse();
}

main(Argc,Argv)
int Argc;
char *Argv[];
{int I;
 appl_init();
 phys_handle=graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
 wind_get(0, WF_WORKXYWH, &xdesk, &ydesk, &wdesk, &hdesk);
 open_vwork();
 if (rsrc_load("SEDT.RSC")==0)
  form_alert(1,Resource_Txt); else
 {rsrc_gaddr(0,MENU,&Menu_Addr);
  menu_bar(Menu_Addr,1);
  for (I=1;I<Maxbuffers;I++)
   wi_handle[I]=0;
  graf_mouse(ARROW,0x0L);
  vst_color(1);
  show_mouse();
  hidden=0;
  hide_mouse();
  fulled=0;
  butdown=1;
  Sedt_Args(Argc,Argv);
  Sedt_Init();
  Sedt_Files(Argc,Argv);
  Dgetpath(D_String,0);
  strcat(D_String,"\\*.*");
  Sedt_Main();
  show_mouse();
  for (I=0;I<Maxbuffers;I++)
   if ((top_window=wi_handle[I])!=0)
   {wind_close(top_window);
    wind_delete(top_window);
   }
  menu_bar(Menu_Addr,0);
 }
 v_clsvwk(handle);
 appl_exit();
}

int GEM_char(M)
int M;
{int event, I;
 char *S, *T;
 do 
 {if (Working_Flag)
   Clr_Working();
  Err_Flag=0;
  graf_mkstate(&gr_mkmx,&gr_mkmy,&gr_mkmstate,&gr_mkkstate);
  event = evnt_multi(M?MU_MESAG|MU_BUTTON|MU_KEYBD|MU_TIMER|MU_M1:
   MU_MESAG|MU_KEYBD|MU_TIMER,1,3,butdown,
			1,gr_mkmx,gr_mkmy,1,1,
			0,0,0,0,0,
			msgbuff,512,0,&mx,&my,&mobutton,&ret,&keycode,&ret);
  if (event & MU_MESAG)
  {if (Err_Display)
    Clr_Error();
   switch (msgbuff[0]) 
   {case WM_REDRAW:
     hide_mouse();
     GEM_Off_Curs();
     do_redraw(msgbuff[3],msgbuff[4],msgbuff[5],msgbuff[6],msgbuff[7]);
     break;
    case WM_NEWTOP:
    case WM_TOPPED:
     GEM_Off_Curs();
     for (I=0;I<Maxbuffers;I++)
      if (wi_handle[I]==msgbuff[3])
       break;
     Ch_Window(I);
     GEM_On_Curs();
     break;
    case WM_SIZED:
    case WM_MOVED:
     wind_update(1);
     hide_mouse();
     GEM_Off_Curs();
     if (msgbuff[4]+msgbuff[6]>xdesk+wdesk)
      msgbuff[6]=xdesk+wdesk-msgbuff[4];
     if (msgbuff[6]<TC_Width*10)
      msgbuff[6]=TC_Width*10;
     if (msgbuff[5]+msgbuff[7]-1>ydesk+hdesk)
      msgbuff[7]=ydesk+hdesk-msgbuff[5];
     if (msgbuff[7]<TC_Height*10)
      msgbuff[7]=TC_Height*10;
     wind_set(top_window,WF_CURRXYWH,msgbuff[4],msgbuff[5],msgbuff[6]-1,
      msgbuff[7]-1);
     wind_set(top_window,WF_CURRXYWH,msgbuff[4],msgbuff[5],msgbuff[6],
      msgbuff[7]);
     wind_get(top_window,WF_WORKXYWH,&xwork,&ywork,&wwork,&hwork);
     GEM_Refresh();
     wind_update(0);
     break;
    case WM_FULLED:
     wind_update(1);
     hide_mouse();
     GEM_Off_Curs();
     if (fulled)
     {wind_calc(WC_WORK,WI_KIND,xold,yold,wold,hold,
       &xwork,&ywork,&wwork,&hwork);
      wind_set(top_window,WF_CURRXYWH,xold,yold,wold-1,hold-1);
      wind_set(top_window,WF_CURRXYWH,xold,yold,wold,hold);
     } else
     {wind_calc(WC_BORDER,WI_KIND,xwork,ywork,wwork,hwork,
       &xold,&yold,&wold,&hold);
      wind_calc(WC_WORK,WI_KIND,xdesk,ydesk,wdesk,hdesk,
       &xwork,&ywork,&wwork,&hwork);
      wind_set(top_window,WF_CURRXYWH,xdesk,ydesk,wdesk,hdesk);
     }
     GEM_Refresh();
     fulled ^= 1;
     wind_update(0);
     break;
    case WM_CLOSED:
     GEM_Quit();
     if (!Not_End)
      return;
     break;
    case MN_SELECTED:
     if (In_Help)
     {Error(2,No_Mouse_Txt);
      break;
     }
     wind_update(1);
     hide_mouse();
     GEM_Off_Curs();
     /*Diag("Menu:msgbuff[3]=%d",msgbuff[3]);*/
     switch (msgbuff[3])
     {case MDESK:
       About();
       break;
      case MFILE:
       switch (msgbuff[4])
       {case MFLOAD:
         strcpy(G_String,"");
         show_mouse();
         fsel_input(D_String,G_String,&I);
         evnt_mesag(altbuff);
         do_redraw(altbuff[3],altbuff[4],altbuff[5],altbuff[6],altbuff[7]);
         hide_mouse();
         if (I)
         {strcpy(Com_Arg,D_String);
          S=Com_Arg;
          while ((T=index(S,':'))!=NULL)
           S=T+1;
          while ((T=index(S,'\\'))!=NULL)
           S=T+1;
          strcpy(S,G_String);
          Count=1;
          Edit_File(Com_Arg);
          Upd_Header();
         }
         break;
        case MFSAVE:
         if ((S=Outfile)==NULL||S[0]==0)
          Error(2,No_Output_File_Txt); else
         {Wr_Buffer(S);
          Auto_Saved=0;
         }
         if (!Err_Flag)
          Clr_Changes();
         Upd_Header();
         break;
        case MFSAVAS:
         strcpy(G_String,"");
         show_mouse();
         fsel_input(D_String,G_String,&I);
         evnt_mesag(altbuff);
         do_redraw(altbuff[3],altbuff[4],altbuff[5],altbuff[6],altbuff[7]);
         hide_mouse();
         if (I)
         {strcpy(Com_Arg,D_String);
          S=Com_Arg;
          while ((T=index(S,':'))!=NULL)
           S=T+1;
          while ((T=index(S,'\\'))!=NULL)
           S=T+1;
          strcpy(S,G_String);
          strcpy(New_File,Com_Arg);
          Wr_Buffer(Outfile=New_File);
          Auto_Saved=0;
          S_Header();
          if (!Err_Flag)
           Clr_Changes();
          Upd_Header();
         }
         break;
        case MFINSERT:
         strcpy(G_String,"");
         show_mouse();
         fsel_input(D_String,G_String,&I);
         evnt_mesag(altbuff);
         do_redraw(altbuff[3],altbuff[4],altbuff[5],altbuff[6],altbuff[7]);
         hide_mouse();
         if (I)
         {strcpy(Com_Arg,D_String);
          S=Com_Arg;
          while ((T=index(S,':'))!=NULL)
           S=T+1;
          while ((T=index(S,'\\'))!=NULL)
           S=T+1;
          strcpy(S,G_String);
	  Include_File(Com_Arg,File_Not_Found_Txt);
          Fill_Screen(0,Rel_Last);
          Set_Changes();
          S_Header();
         }
         break;
        case MFQUIT:
         GEM_Quit();
         if (!Not_End)
         {hide_mouse();
          menu_tnormal(Menu_Addr,msgbuff[3],1);
          wind_update(0);
          return;
         }
         break;
       }
       break;
      case MWINDOW:
       switch (msgbuff[4])
       {case MW0:
         Ch_Window(0);
         break;
        case MW1:
         Ch_Window(1);
         break;
        case MW2:
         Ch_Window(2);
         break;
        case MW3:
         Ch_Window(3);
         break;
        case MWSWITCH:
         Other_Window();
         break;
        case MWCLEAR:
         Zero();
         break;
       }
       break;
      case MSELECT:
       switch (msgbuff[4])
       {case MSCLEAR:
         Reset();
         break;
        case MSCUT:
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
        case MSPASTE:
         Count=0;
         Paste();
         break;
       }
       break;
      case MRULER:
       switch (msgbuff[4])
       {case MRULER0:
         Count=0;
         break;
        case MRULER1:
         Count=1;
         break;
        case MRULER2:
         Count=2;
         break;
        case MRULER3:
         Count=3;
         break;
        case MRULER4:
         Count=4;
         break;
        case MRULER5:
         Count=5;
         break;
        case MRULER6:
         Count=6;
         break;
        case MRULER7:
         Count=7;
         break;
        case MRULER8:
         Count=8;
         break;
        case MRULER9:
         Count=9;
         break;
       }
       Get_Ruler();
       break;
      case MHELP:
       switch (msgbuff[4])
       {case MHSHOW:
         hide_mouse();
         strcpy(Com_Arg,Do_Mapping("HELP.HLP"));
         Do_Help();
         break;
       }
       break;
     }
     menu_tnormal(Menu_Addr,msgbuff[3],1);
     GEM_On_Curs();
     wind_update(0);
     break;
    case WM_ARROWED:
     if (In_Help)
     {Error(2,No_Mouse_Txt);
      break;
     }
     hide_mouse();
     GEM_Off_Curs();
     Init_Entity();
     switch (msgbuff[4])
     {case 2:
      case 3:
       Ent_Direction=msgbuff[4]==3;
       Line();
       break;
      case 0:
      case 1:
       Ent_Direction=msgbuff[4]==1;
       Screen();
       break;
     }
     Move_Entity();
     Upd_Header();
     GEM_On_Curs();
     break;
    case WM_VSLID:
     if (In_Help)
     {Error(2,No_Mouse_Txt);
      break;
     }
     hide_mouse();
     GEM_Off_Curs();
     Init_Entity();
     if ((Ent_Count=((msgbuff[4]*Buf_Lines)/1000)+1)>
      Buf_Lines)
      Ent_Count=Buf_Lines;
     Abs_Line();
     Move_Entity();
     Upd_Header();
     GEM_On_Curs();
     break;
   }
  }
  if (event&MU_M1)
  {if (butdown==0)
   {wind_update(1);
    hide_mouse();
    GEM_Off_Curs();
    my=(my-ywork)/TC_Height+1;
    mx=(mx-xwork)/TC_Width+1;
    Mouse_Execute(my,mx,1,0,1);
    wind_update(0);
    GEM_On_Curs();
   }
   show_mouse();
  }
  if (event & MU_BUTTON)
  {if (In_Help)
    Error(2,No_Mouse_Txt); else
   {butdown=butdown?0:1;
    wind_update(1);
    hide_mouse();
    GEM_Off_Curs();
    my=(my-ywork)/TC_Height+1;
    mx=(mx-xwork)/TC_Width+1;
    if (butdown==0)
     Mouse_Execute(my,mx,1,0,0);
    wind_update(0);
    GEM_On_Curs();
   }
  }
  if (event&MU_TIMER)
   GEM_Switch_Curs(Curs_Line,Curs_Col);
  if (event & MU_KEYBD)
  {hide_mouse();
   return keycode;
  }
  /*Diag("GEM_Char:event=%x,msgbuff[0]=%d",event,msgbuff[0]);*/
  Out_Zap();
 } while (1);
}

GEM_Text(L,C,S,A)
int L, C, A;
char *S;
{int pxyarray[4], N;
 N=strlen(S);
 wind_update(1);
 v_gtext(handle,xwork+(C-1)*TC_Width,ywork+L*TC_Height-
  T_Baseline,S);
 if (A&A_Reverse)
 {pxyarray[0]=xwork+(C-1)*TC_Width;
  pxyarray[1]=ywork+(L-1)*TC_Height;
  pxyarray[2]=pxyarray[0]+TC_Width*N-1;
  pxyarray[3]=pxyarray[1]+TC_Height-1;
  vswr_mode(handle,3);
  vr_recfl(handle,pxyarray);
  vswr_mode(handle,1);
 }
 wind_update(0);
}

GEM_Clear()
{int pxyarray[4];
 wind_update(1);
 pxyarray[0]=xwork;
 pxyarray[1]=ywork;
 pxyarray[2]=xwork+wwork-1;
 pxyarray[3]=ywork+hwork-1;
 vr_recfl(handle,pxyarray);
 wind_update(0);
}

GEM_Erase(Line,Col,Length)
int Line, Col, Length;
{int pxyarray[4];
 wind_update(1);
 pxyarray[0]=xwork+(Col-1)*TC_Width;
 pxyarray[1]=ywork+(Line-1)*TC_Height;
 pxyarray[2]=pxyarray[0]+Length*TC_Width-1;
 pxyarray[3]=pxyarray[1]+TC_Height-1;
 vr_recfl(handle,pxyarray);
 wind_update(0);
}

GEM_Up_Scroll(T,B,Scroll)
int T, B, Scroll;
{FDB S_FDB, D_FDB;
 int pxyarray[8];
 wind_update(1);
 pxyarray[0]=xwork;
 pxyarray[1]=ywork+(T+Scroll-1)*TC_Height;
 pxyarray[2]=pxyarray[0]+wwork-1;
 pxyarray[3]=ywork+B*TC_Height-1;
 pxyarray[4]=xwork;
 pxyarray[5]=ywork+(T-1)*TC_Height;
 pxyarray[6]=pxyarray[4]+wwork-1;
 pxyarray[7]=ywork+(B-Scroll)*TC_Height-1;
 S_FDB.fd_addr=0;
 D_FDB.fd_addr=0;
 vro_cpyfm(handle,3,pxyarray,&S_FDB,&D_FDB);
 wind_update(0);
}

GEM_Down_Scroll(T,B,Scroll)
int T, B, Scroll;
{FDB S_FDB, D_FDB;
 int pxyarray[8];
 wind_update(1);
 pxyarray[0]=xwork;
 pxyarray[1]=ywork+(T-1)*TC_Height;
 pxyarray[2]=pxyarray[0]+wwork-1;
 pxyarray[3]=ywork+(B-Scroll)*TC_Height-1;
 pxyarray[4]=xwork;
 pxyarray[5]=ywork+(T+Scroll-1)*TC_Height;
 pxyarray[6]=pxyarray[4]+wwork-1;
 pxyarray[7]=ywork+B*TC_Height-1;
 S_FDB.fd_addr=0;
 D_FDB.fd_addr=0;
 vro_cpyfm(handle,3,pxyarray,&S_FDB,&D_FDB);
 wind_update(0);
}

GEM_Switch_Curs(L,C)
{int pxyarray[8], Sav_hidden;
 Sav_hidden=hidden;
 wind_update(1);
 if (!Sav_hidden)
  hide_mouse();
 pxyarray[0]=xwork+(C-1)*TC_Width;
 pxyarray[1]=ywork+(L-1)*TC_Height;
 pxyarray[2]=pxyarray[0]+TC_Width-1;
 pxyarray[3]=pxyarray[1]+TC_Height-1;
 vswr_mode(handle,3);
 vr_recfl(handle,pxyarray);
 vswr_mode(handle,1);
 Curs_On=Curs_On?0:1;
 if (!Sav_hidden)
  show_mouse();
 wind_update(0);
}

GEM_On_Curs()
{Curs_Line=My_Line;
 Curs_Col=My_Col-Scr_Shift;
 GEM_Switch_Curs(Curs_Line,Curs_Col);
}

GEM_Off_Curs()
{if (Curs_On)
  GEM_Switch_Curs(Curs_Line,Curs_Col);
}

GEM_Name(W,S)
int W;
char *S;
{char T[33];
 strcpy(T,"  ");
 if (S==NULL)
  strcpy(T+2,G_Unused_Txt); else 
  strncpy(T+2,S,30);
 menu_text(Menu_Addr,MW0+Cur_Buffer,T);
}

GEM_Newwindow(B)
int B;
{open_window(B);
}

GEM_Topwindow(B)
int B;
{wind_set(top_window=wi_handle[B],WF_TOP,0,0,0,0);
 wind_get(top_window,WF_WORKXYWH,&xwork,&ywork,&wwork,&hwork);
 set_clip(xwork,ywork,wwork,hwork);
 vst_point(handle,Buf_Points,&Dummy,&Dummy,&Dummy,&Dummy);
}

GEM_Yes_No(S)
char *S;
{int Sav_hidden, Ret;
 if (Sav_hidden=hidden)
  show_mouse();
 strcpy(G_String,"[2][");
 strcat(G_String,S);
 strcat(G_String,Yes_No_Cancel_Txt);
 Ret=form_alert(1,G_String);
 if (Sav_hidden)
  hide_mouse();
 if (Ret==3)
 {Error(3,Txt_Cancelled);
  return 0;
 }
 return Ret==1;
}

GEM_Error(S)
char *S;
{int Sav_hidden;
 if (Sav_hidden=hidden)
  show_mouse();
 strcpy(G_String,"[3][");
 strcat(G_String,S);
 strcat(G_String,G_Continue_Txt);
 form_alert(1,G_String);
 if (Sav_hidden)
  hide_mouse();
}

GEM_Refresh()
{set_clip(xwork,ywork,wwork,hwork);
 Scr_Width=wwork/TC_Width;
 Scr_Length=(hwork-1)/TC_Height;
 Scr_Shift=0;
 if (Do_Shift)
 {if (My_Col-Scr_Shift>=Scr_Width)
  {while (My_Col-Scr_Shift>=Scr_Width)
    Scr_Shift+=Scr_Width/2;
  }
 }
 if (In_Help)
 {S_Header();
  Display_Help(Help_Pos);
 } else
  Refresh();
 Rfreshed=1;
 GEM_On_Curs(My_Line,(int)My_Col-Scr_Shift);
}

GEM_S_Bar(H,L)
long H, L;
{int S;
 S=Scr_Length-5;
 wind_set(top_window,WF_VSLSIZE,(int)((1000L*S)/(H<S?S:H)));
 wind_set(top_window,WF_VSLIDE,(int)((1000L*L-1)/H));
}

GEM_P_Bar(H,L)
long H, L;
{int S;
 S=Scr_Length-5;
 wind_set(top_window,WF_VSLIDE,(int)((1000L*L-1)/H));
}

GEM_Quit()
{int I, Old_Buffer;
 if ((Any_Changes[Cur_Buffer]&1))
  if (!Yes_No(Unsaved_Chng_Txt))
   return;
 Any_Changes[Cur_Buffer]=0;
 Zero();
 GEM_Name(top_window,G_Unused_Txt);
 Not_End=0;
 for (I=0;I<Maxbuffers;I++)
  if (I!=Cur_Buffer&&wi_handle[I]!=0)
  {Not_End=1;
   Lst_Current=I;
   break;
  }
 I=Cur_Buffer;
 if (Not_End)
  Ch_Window(Lst_Current);
 wind_close(wi_handle[I]);
 wind_delete(wi_handle[I]);
 wi_handle[I]=0;
}

GEM_Smaller()
{int Old_Size;
 Old_Size=Buf_Points;
 if ((Buf_Points=vst_point(handle,Old_Size-1,&Dummy,&Dummy,
  &Dummy,&Dummy))<Old_Size)
 {vqt_attributes(handle,work_out);
  TC_Width=work_out[8];
  TC_Height=work_out[9];
  T_Baseline=TC_Height-work_out[7];
  Scr_Width=wwork/TC_Width;
  Scr_Length=(hwork-1)/TC_Height;
  GEM_Refresh();
  Rfreshed=0;
 }
 Message(G_Font_Txt,Buf_Points);
}

GEM_Larger()
{int Old_Size, New_Size;
 New_Size=Old_Size=Buf_Points;
 while (++New_Size<=999)
 if ((Buf_Points=vst_point(handle,New_Size,&Dummy,&Dummy,
  &Dummy,&Dummy))>Old_Size)
 {vqt_attributes(handle,work_out);
  TC_Width=work_out[8];
  TC_Height=work_out[9];
  T_Baseline=TC_Height-work_out[7];
  Scr_Width=wwork/TC_Width;
  Scr_Length=(hwork-1)/TC_Height;
  GEM_Refresh();
  Rfreshed=0;
  break;
 }
 Message(G_Font_Txt,Buf_Points);
}

int Prompt_Input()
{struct text_edinfo *P;
 char *S;
 int Done=0, Result;
 Lst_Index=-1;
 show_mouse();
 rsrc_gaddr(0,PROMPT,&Prompt_Addr);
 form_center(Prompt_Addr,Tempptr);
 (Prompt_Addr+PTEXT)->ob_spec=(long) Dsk_Rec;
 form_dial(0,1,1,1,1,Tempbox);
 form_dial(1,1,1,1,1,Tempbox);
 objc_draw(Prompt_Addr,ROOT,MAX_DEPTH,Tempbox);
 while (!Done)
 {switch (form_do(Prompt_Addr,PINPUT))
  {case PENTER:
    (Prompt_Addr+PENTER)->ob_state=NORMAL;
    P=(struct text_edinfo *) (Prompt_Addr+PINPUT)->ob_spec;
    if (strncmp(P->te_ptext,"@@@@@",5)==0)
     P->te_ptext[0]=0;
    strcpy(Dsk_Rec,S=P->te_ptext);
    while(*S)
     *(S++)='@';
    *S='@';
    if (Dsk_Rec[0]!=0)
    {C_Move(Lst_Input[0],Lst_Input[1],132*9,1);
     strcpy(Lst_Input[0],Dsk_Rec);
    }
    Done=1;
    /*Diag("Prompt_Input:Dsk+Rec=%s",Dsk_Rec);*/
    Result=1;
    break;
   case PCANCEL:
    (Prompt_Addr+PCANCEL)->ob_state=NORMAL;
    Error(3,Txt_Cancelled);
    P=(struct text_edinfo *) (Prompt_Addr+PINPUT)->ob_spec;
    S=P->te_ptext;
    while(*S)
     *(S++)='@';
    *S='@';
    Done=1;
    Result=0;
    break;
   case PPREV:
    (Prompt_Addr+PPREV)->ob_state=NORMAL;
    objc_draw(Prompt_Addr,PPREV,1,Tempbox);
    S=P->te_ptext;
    if (Lst_Index<9)
    {while(*S)
      *(S++)='@';
     *S='@';
     P=(struct text_edinfo *) (Prompt_Addr+PINPUT)->ob_spec;
     strcpy(P->te_ptext,Lst_Input[++Lst_Index]);
     objc_draw(Prompt_Addr,PINPUT,1,Tempbox);
    }
    break;
   case PNEXT:
    (Prompt_Addr+PNEXT)->ob_state=NORMAL;
    objc_draw(Prompt_Addr,PNEXT,1,Tempbox);
    S=P->te_ptext;
    if (Lst_Index>0)
    {while(*S)
      *(S++)='@';
     *S='@';
     P=(struct text_edinfo *) (Prompt_Addr+PINPUT)->ob_spec;
     strcpy(P->te_ptext,Lst_Input[--Lst_Index]);
     objc_draw(Prompt_Addr,PINPUT,1,Tempbox);
    }
    break;
  }
 }
 form_dial(2,1,1,1,1,Tempbox);
 form_dial(3,1,1,1,1,Tempbox);
 hide_mouse();
 evnt_mesag(msgbuff);
 do_redraw(msgbuff[3],msgbuff[4],msgbuff[5],msgbuff[6],msgbuff[7]);
 return Result;
}

About()
{struct text_edinfo *P;
 char *S;
 Lst_Index=-1;
 show_mouse();
 rsrc_gaddr(0,ABOUT,&About_Addr);
 form_center(About_Addr,Tempptr);
 (About_Addr+PVERSION)->ob_spec=(long) Version;
 form_dial(0,1,1,1,1,Tempbox);
 form_dial(1,1,1,1,1,Tempbox);
 objc_draw(About_Addr,ROOT,MAX_DEPTH,Tempbox);
 form_do(About_Addr,0);
 (About_Addr+POK)->ob_state=NORMAL;
 form_dial(2,1,1,1,1,Tempbox);
 form_dial(3,1,1,1,1,Tempbox);
 hide_mouse();
 evnt_mesag(msgbuff);
 do_redraw(msgbuff[3],msgbuff[4],msgbuff[5],msgbuff[6],msgbuff[7]);
}
