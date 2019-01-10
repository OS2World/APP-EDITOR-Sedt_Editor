/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"

#if (defined(DOS) & !defined(Windows))
struct SREGS Seg_Regs;
union REGS In_Regs, Out_Regs;
#endif

#if defined(DOS) | defined(NT) | defined(OS2)
struct Memory far *Mem_List=NULL;
#else
struct Memory *Mem_List=NULL;
#endif

/*
			     Logic

	Buf implements an  "infinitely"  long string and routines 
for accessing the string.  The  implementation takes advantage of
all available memory before overflowing to disk files.

				--------------------------------
				|	Input			|
				|	File			|
			----------------------------------------
			|Memory	|
			|/Buffer	|
--------------------------------
|	Output		|
|	File		|
-------------------------

	The input file and output  file can either be a true disk
file or linked lists of records in memory.
*/

#define Flash_Count 3

#if defined(Noasm) | defined(DOS) | defined(NT) | defined(ATARI) | defined(OS2) | defined(UNIX) | defined(WIN32)
/*      Copies L characters from F to T and handles overlapping copies
	correctly.
*/
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
void Fast C_Move(char *F,char *T,int L)
{
 memmove(T,F,L);
#endif
#if defined(ATARI)
void Fast C_Move(char *F,char *T,int L,int Overlap)
{
 if (!Overlap)
  memcpy(T,F,L); else
  movmem(F,T,L);
#endif
#if (defined(Noasm) & !(defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)))| defined(UNIX) | defined(MPE)
void Fast C_Move(char *F,char *T,int L,int Overlap)
{
 if (!Overlap)
  memcpy(T,F,L); else
 {if (F<T)
  {F+=L;
   T+=L;
   L++;
   while (--L)
    (*--T)=(*--F);
  } else
  {L++;
   while (--L)
    (*T++)=(*F++);
  }
 }
#endif
}
#endif

/*      Copies the string F to T and pads the end of T with spaces to the
	length L.
*/
void Fast S_Copy(char *F,char *T,int L)
{while (((*T)=(*F++))!=0)
 {T++;
  L--;}
 while (L--)
  *(T++)=' ';
}

/*      Displays a fatal error message, cleans up the buffer database and
	exits from SEDT.
*/
#if defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(WIN32)
void Fatalerror(char *S,...)
#else
void Fatalerror(char *S,int P1,int P2,int P3,int P4,int P5,int P6,int P7, int P8,int P9)
#endif
{
#if (defined(DOS) | defined(NT)) & defined(Windows)
 va_list Ap;
 static char T[128];
#if !defined(WIN95)
 if (Curr_Wnd==0)
  Make_Window(0,SHOW_OPENWINDOW);
#endif
 va_start(Ap,S);
 vsprintf(T,S,Ap);
 va_end(Ap);
 Win_Error(T);
#elif defined(DOS) & defined(TV)
 static char T[128];
 va_list Ap;
 va_start(Ap,S);
 vsprintf(T,S,Ap);
 va_end(Ap);
 TV_Error(T);
#elif defined(DOS) | defined(NT) | defined(OS2) | defined(UNIX) | defined(VMS) | defined(MPE) | defined(WIN32)
 va_list Ap;
 printf("\7");
 va_start(Ap,S);
 vprintf(S,Ap);
 va_end(Ap);
 printf("\n");
#else
 printf(S,P1,P2,P3,P4,P5,P6,P7,P8,P9);
 printf("\n");
#endif
}

#if defined(DOS) & !defined(Windows)
int Crit_Int(int Type)
{char *Crit_Txt;
 if (Err_Flag)
  return 3;
 In_Int24=1;
 switch (Type)
 {case 0:
   Crit_Txt=Write_Protect_Txt;
   break;
  case 1:
   Crit_Txt=Unknown_Unit_Txt;
   break;
  case 2:
   Crit_Txt=Drive_N_Ready_Txt;
   break;
  case 3:
   Crit_Txt=Unknown_Com_Txt;
   break;
  case 4:
   Crit_Txt=Data_Error_Txt;
   break;
  case 5:
   Crit_Txt=Bad_Request_Txt;
   break;
  case 6:
   Crit_Txt=Seek_Error_Txt;
   break;
  case 7:
   Crit_Txt=Unknown_Media_Txt;
   break;
  case 8:
   Crit_Txt=Sector_N_Found_Txt;
   break;
  case 9:
   Crit_Txt=No_Paper_Txt;
   break;
  case 10:
   Crit_Txt=Write_Fault_Txt;
   break;
  case 11:
   Crit_Txt=Read_Fault_Txt;
   break;
  case 12:
   Sedt_Shutdown();
   Fatalerror(General_Txt);
#if defined(OS2)
   DosExit(1,1);
#else
#if defined(DESQVIEW)
   if (DV)
    api_exit();
#endif
#if defined(DOS) & defined(TV)
   TV_Exit(1);
#endif
   _exit(1);
#endif
  default:
   Crit_Txt=Dos_Error_Txt;
   break;
 }
 if (Yes_No(Crit_Txt))
 {Err_Flag=In_Int24=0;
  Plain_Message(Crit_Retry_Txt);
  return 1;
 } else
 {Plain_Message(Crit_Fail_Txt);
  Err_Flag=1;
  In_Int24=0;
  return 3;
 }
}
#endif

/*      Read a fixed length record number P into B from the buffer file F.
        If an error occurs a fatal error message is displayed.
*/
void Fast Blockread(struct fcb_t *F,char *B,int P)
{
 /*Diag("Blockread:P=%d,F->P%d",P,F->P);*/
#if defined(VMS) | defined(DOS) | defined(NT) | defined(ATARI) | defined(UNIX) | defined(OS2) | defined(MPE) | defined(WIN32)
 if (F->P!=P)
#if defined(VMS) | defined(UNIX) | defined(MPE)
  lseek(F->F,(long)P*Blocksize,0);
 if (read(F->F,B,Blocksize)<=0)
#endif
#if defined(DOS) | defined(WIN32)
#if defined(Windows)
  lseek(F->F,(long)P*Blocksize,0);
 if (read(F->F,B,Blocksize)<=0)
#else
  fseek(F->F,(long)P*Blocksize,0);
 if (fread(B,Blocksize,1,F->F)<=0)
#endif
#endif
#if defined(NT)
  _lseek(F->F,(long)P*Blocksize,0);
 if (_read(F->F,B,Blocksize)<=0)
#endif
#if defined(OS2)
  fseek(F->F,(long)P*Blocksize,0);
 if (fread(B,Blocksize,1,F->F)<=0)
#endif
#if defined(ATARI)
  Fseek((long)P*Blocksize,F->F,0);
 if (Fread(F->F,(long)Blocksize,B)<=0)
#endif
 {Sedt_Shutdown();
  Fatalerror(Buffer_Err_Txt);
#if defined(OS2)
  DosExit(1,1);
#else
#if defined(DESQVIEW)
  if (DV)
   api_exit();
#endif
#if defined(DOS) & defined(TV)
   TV_Exit(1);
#endif
  _exit(1);
#endif
 }
 F->P=P+1;
#endif
}

/*      Writes a fixed length record from B into the buffer file F at record
        number P. If an error occurs an fatal error message is displayed.
*/
void Fast Blockwrite(struct fcb_t *F,char *B,int P)
{
 /*Diag("Blockwrite:P=%d,F->P%d",P,F->P);*/
#if defined(VMS) | defined(DOS) | defined(NT) | defined(ATARI) | defined(UNIX) | defined(OS2) | defined(MPE) | defined(WIN32)
 if (F->P!=P)
#if defined(VMS) | defined(UNIX) | defined(MPE)
  lseek(F->F,(long)P*Blocksize,0);
 if (write(F->F,B,Blocksize)<=0)
#endif
#if defined(DOS) | defined(WIN32)
#if defined(Windows)
  lseek(F->F,(long)P*Blocksize,0);
 if (write(F->F,B,Blocksize)<=0)
#else
  fseek(F->F,(long)P*Blocksize,0);
 if (fwrite(B,Blocksize,1,F->F)<=0)
#endif
#endif
#if defined(NT)
  _lseek(F->F,(long)P*Blocksize,0);
 if (_write(F->F,B,Blocksize)<=0)
#endif
#if defined(OS2)
  fseek(F->F,(long)P*Blocksize,0);
 if (fwrite(B,Blocksize,1,F->F)<=0)
#endif
#if defined(ATARI)
  Fseek((long)P*Blocksize,F->F,0);
 if (Fwrite(F->F,(long)Blocksize,B)<=0)
#endif
 {Sedt_Shutdown();
  Fatalerror(Buffer_Wr_Err_Txt);
#if defined(OS2)
  DosExit(1,1);
#else
#if defined(DESQVIEW)
  if (DV)
   api_exit();
#endif
#if defined(DOS) & defined(TV)
   TV_Exit(1);
#endif
  _exit(1);
#endif
 }
 F->P=P+1;
#endif
}

/*      Outputs a record from the bottom of the memory window of the buffer B
        to a virtual file in memory if possible. If there is no more room in
        memory a disk file is created and all the contents of the virtual file
        written to it. All subsequent writes until the buffer is rewound will
        be to the disk file.
*/
void Fast Out_Buf(struct Buffer *B)
{int M;
 char *P;
#if defined(DOS) | defined(NT) | defined(OS2)
 struct Memory far *M_P;
#else
 struct Memory *M_P;
#endif
#if defined(DOS)
 char far *Membuf_Ptr;
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Mem_List=(struct Memory far *) GlobalLock(Mem_Handle);
#endif
 if (B->Modified)
 {if ((M=Free_List)==-1)
  {if (M_Next==Mem_Entries)
   {Sedt_Shutdown();
    Fatalerror(Max_Err_Txt);
#if defined(OS2)
    DosExit(1,1);
#else
#if defined(DESQVIEW)
    if (DV)
     api_exit();
#endif
#if defined(DOS) & defined(TV)
    TV_Exit(1);
#endif
    _exit(1);
#endif
   }
   M=M_Next++;
   M_P= &Mem_List[M];
#if defined(ATARI)
   if ((M_P->Mem_Block=(char *)malloc(Blocksize))==NULL)
#endif
#if defined(VMS) | defined(UNIX) | defined(MPE)
   if ((M_P->Mem_Block=(char *)malloc(Blocksize))==NULL)
#endif
#if defined(WIN32)
   if ((M_P->Mem_Block=(char *)calloc(1,Blocksize))==NULL)
#endif
#if defined(DOS) & !defined(Windows)
   if ((M_P->Mem_Segaddr=Mem_Alloc(Blocksize))==0&&EMS_Handle==0)
#endif
#if defined(OS2)
   if ((M_P->Mem_Segaddr=Mem_Alloc(Blocksize))==0)
#endif
   {if (Overflow.F==0)
    {
#if (defined(DOS) | defined(NT)) & defined(Windows)
     Win_Tempfile(Overflow.File_Name);
#else
#if defined(VMS) | defined(UNIX)
#if defined(VMS)
     strcpy(Overflow.File_Name,"Sys$Scratch:");
#else
     strcpy(Overflow.File_Name,"");
#endif
     strcat(Overflow.File_Name,"seXXXXXX");
     mktemp(Overflow.File_Name);
#else
#if defined(MPE)
     strcpy(Overflow.File_Name,"temp.sed");
#else
     if ((P=getenv("TEMP"))!=NULL)
     {strcpy(Overflow.File_Name,P);
      if (P[strlen(P)-1]!='\\')
       strcat(Overflow.File_Name,"\\");
      strcat(Overflow.File_Name,"seXXXXXX");
     } else
      strcpy(Overflow.File_Name,"seXXXXXX");
     mktemp(Overflow.File_Name);
#endif
#endif
#endif
     Overflow.P=0;
#if defined(ATARI)
     Fdelete(Overflow.File_Name);
     if ((Overflow.F=Fcreate(Overflow.File_Name,0))<0)
#endif
#if defined(VMS) 
     if ((Overflow.F=creat(Overflow.File_Name,0,"fop=tmd"))<0)
#endif
#if (defined(DOS) & defined(Windows))
     if ((Overflow.F=open(Overflow.File_Name,O_CREAT|O_RDWR|O_BINARY))==-1)
#endif
#if defined(NT)
     if ((Overflow.F=_open(Overflow.File_Name,O_CREAT|O_RDWR|O_BINARY))==-1)
#endif
#if (defined(DOS) & !defined(Windows)) | defined(WIN32)
     if ((Overflow.F=fopen(Overflow.File_Name,"w+b"))==NULL)
#endif
#if defined(OS2)
     if ((Overflow.F=fopen(Overflow.File_Name,"w+b"))==NULL)
#endif
#if defined(UNIX) | defined(MPE)
     if ((Overflow.F=open(Overflow.File_Name,O_RDWR|O_CREAT,00400+00200))<0)
#endif
     {Sedt_Shutdown();
      Fatalerror(Dsk_Full_Err_Txt);
#if defined(OS2)
      DosExit(1,1);
#else
#if defined(DESQVIEW)
      if (DV)
       api_exit();
#endif
#if defined(DOS) & defined(TV)
      TV_Exit(1);
#endif
      _exit(1);
#endif
     }
     D_Next=0;
    }
    M_P->Disk_Block=D_Next++;
   }
#if defined(DOS) & ! defined(Windows)
   M_P->EMS_H=EMS_Handle;
#endif
  } else
   M_P= &Mem_List[M];
  if (M==Free_List)
   Free_List=M_P->Next;
  if (B->Out_First==-1)
  {B->Out_First=B->Out_Last=M;
   M_P->Last= -1;
  } else
  {Mem_List[B->Out_Last].Next=M;
   M_P->Last=B->Out_Last;
   B->Out_Last=M;
  }
  Mem_List[B->Out_Last].Next= -1;
  Mem_List[B->Out_Last].No=B->Outno;
#if !((defined(DOS) | defined(NT)) & defined(Windows))
#if defined(DOS) | defined(OS2)
#if defined(DOS)
  if (M_P->Mem_Segaddr!=0||M_P->EMS_H!=0)
#else
  if (M_P->Mem_Segaddr!=0)
#endif
#else
  if (M_P->Mem_Block!=NULL)
#endif
#if defined(DOS) | defined(OS2)
  {
#if defined(DOS)
   if (M_P->EMS_H!=0)
    Move_To_EMS(M_P->EMS_H,M_P->Mem_Segaddr,B->Membuf); else
#if defined(__BORLANDC__)
    movedata(FP_SEG(B->Membuf),FP_OFF(B->Membuf),M_P->Mem_Segaddr,0, Blocksize);
#else
   {Membuf_Ptr=B->Membuf;
    _movedata(FP_SEG(Membuf_Ptr),FP_OFF(Membuf_Ptr),M_P->Mem_Segaddr,0, Blocksize);
   }
#endif
#else
   _fmemcpy(MAKEP(M_P->Mem_Segaddr,0),B->Membuf,Blocksize);
#endif
  } else
#endif
#if defined(WIN32)
   C_Move(B->Membuf,M_P->Mem_Block,Blocksize); else
#endif
#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE)
   C_Move(B->Membuf,M_P->Mem_Block,Blocksize,0); else
#endif
#endif
  {
#if defined(VMS) | defined(DOS) | defined(NT) | defined(ATARI) | defined(UNIX) | defined(OS2) | defined(MPE) | defined(WIN32)
   Blockwrite(&Overflow,B->Membuf,M_P->Disk_Block);
#endif
  }
 }
#if (defined(DOS) | defined(NT)) & defined(Windows)
 GlobalUnlock(Mem_Handle);
#endif
 B->Outno++;
 if (B->Memsize>Blocksize)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move((B->Membuf)+Blocksize,B->Membuf,B->Memsize-Blocksize);
#else
  C_Move((B->Membuf)+Blocksize,B->Membuf,B->Memsize-Blocksize,1);
#endif
 B->Memsize-=Blocksize;
 B->Memptr-=Blocksize;
 B->Memstart+=Blocksize;
}

/*      Read a record from the virtual or real input file into the top of
	the memory window of buffer B. This routine also displays the
	"Working..." indicator if more than a certain number of buffer reads
	have been issued since the working flag was last cleared.
*/
void Fast In_Buf(struct Buffer *B)
{
#if !(defined(DOS) & defined(TV))
 int Sav_Line, Sav_Update;
 long Sav_Col, Sav_Shift;
 char Sav_Attributes;
#endif
 int M;
#if defined(DOS) | defined(NT) | defined(OS2)
 struct Memory far *M_P;
#else
 struct Memory *M_P;
#endif
#if defined(DOS)
 char far *Membuf_Ptr;
#endif
#if !(defined(DOS) & defined(TV))
 if ((Wr_Count++)==Flash_Count)
 {Set_Physical();
  Sav_Set(&Sav_Line,&Sav_Col,&Sav_Attributes,Scr_Length,(long)1,0);
  Sav_Shift=Scr_Shift;
  Scr_Shift=0;
  Sav_Update=Update_Flag;
  Update_Flag=1;
  Opt_String(Txt_Working);
  Opt_Erase_Line();
  Err_Display=0;
  Out_Zap();
  Set_Save(Sav_Line,Sav_Col,Sav_Attributes);
  Set_Logical();
  Scr_Shift=Sav_Shift;
  Working_Flag=1;
  Update_Flag=Sav_Update;
 }
#endif
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Mem_List=(struct Memory far *)GlobalLock(Mem_Handle);
#endif
 if (B->Inno==0)
  M=B->In_First; else
 {M=B->In_This==-1?(B->In_This=B->In_First):B->In_This;
  while (Mem_List[M].No<B->Inno)
   M=Mem_List[M].Next;
  while (Mem_List[M].No>B->Inno)
   M=Mem_List[M].Last;
 }
 M_P= &Mem_List[M];
 B->In_This=M;
#if !((defined(DOS) | defined(NT)) & defined(Windows))
#if defined(DOS) | defined(OS2)
#if defined(DOS)
 if (M_P->Mem_Segaddr!=0||M_P->EMS_H!=0)
#else
 if (M_P->Mem_Segaddr!=0)
#endif
#else
 if (M_P->Mem_Block!=NULL)
#endif
#if defined(DOS) | defined(OS2)
 {
#if defined(DOS)
  if (M_P->EMS_H!=0)
   Move_From_EMS(M_P->EMS_H,M_P->Mem_Segaddr,B->Membuf+B->Memsize); else
#if defined(__BORLANDC__)
    movedata(M_P->Mem_Segaddr,0,FP_SEG(Membuf_Ptr),FP_OFF(Membuf_Ptr)+B->Memsize,Blocksize);
#else
  {Membuf_Ptr=B->Membuf;
   _movedata(M_P->Mem_Segaddr,0,FP_SEG(Membuf_Ptr),FP_OFF(Membuf_Ptr)+B->Memsize,Blocksize);
  }
#endif
#else
  _fmemcpy(B->Membuf+B->Memsize,MAKEP(M_P->Mem_Segaddr,0),Blocksize);
#endif
 } else
#endif
#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE)
  C_Move(M_P->Mem_Block,B->Membuf+B->Memsize,Blocksize,0); else
#endif
#if defined(WIN32)
  C_Move(M_P->Mem_Block,B->Membuf+B->Memsize,Blocksize); else
#endif
#endif
#if defined(VMS) | defined(DOS) | defined(NT) | defined(ATARI) | defined(UNIX) | defined(OS2) | defined(MPE) | defined(WIN32)
  Blockread(&Overflow,B->Membuf+B->Memsize,M_P->Disk_Block);
#endif
 if (B->Modified)
 {B->In_First=M_P->Next;
  M_P->Next= -1;
  Mem_Return(M);
  B->In_This=M=B->In_First;
  if (M!=-1)
   M_P->Last= -1;
 }
#if (defined(DOS) | defined(NT)) & defined(Windows)
 GlobalUnlock(Mem_Handle);
#endif
 B->Inno++;
 B->Memsize+=Blocksize;
 if (B->Memsize+B->Memstart-1>B->Bufsize)
  B->Memsize=B->Bufsize-B->Memstart+1;
}

/*      Writes all of the contents of the memory window and the input file
        to the output file of a buffer.
*/
void Fast B_Flush(struct Buffer *B)
{while (B->Memstart+B->Memsize<=B->Bufsize)
 {if (B->Memsize<Blocksize)
   In_Buf(B);
  Out_Buf(B);
 }
 while (B->Memsize>0)
  Out_Buf(B);
 /*Diag("B_Flush:End");*/
}

/*      Flushes the memory window and input file of a buffer to the output
        file. Then removes the input file and changes the former output file
        into the input file and positions the buffer at the beginning.
*/
void Fast B_Rewind(struct Buffer *B)
{/*Diag("B_Rewind");*/
 if (B->Modified)
 {B_Flush(B);
  if (B->In_First!=-1)
   Mem_Return(B->In_First);
  B->In_First=B->Out_First;
  B->In_This=B->Out_First=B->Out_Last= -1;
  B->Modified=0;
 }
 B->Inno=B->Outno=B->Memsize=B->Memptr=0;
 B->Memstart=B->Pos=1;
}

/*      Repositions the buffer B to the point Point.
*/
void Fast B_Goto(struct Buffer *B,long Point)
{if (Point<B->Memstart)
  B_Rewind(B);
 if (Point>=B->Memstart+B->Memsize)
 {if (Point>B->Bufsize)
  {if (Point!=1)
   {B_Goto(B,Point-1);
    B->Pos++;
    B->Memptr++;
   }
  } else
  {if ((!B->Modified)&&Point>B->Memstart+Memmax)
   {B->Inno=(Point-1)/Blocksize;
    B->Memstart=(long) B->Inno*Blocksize+1;
    B->Memsize=B->Memptr=B->Pos=0;
   }
   while (Point>=B->Memstart+B->Memsize)
    Out_In_Buf(B);
  }
 }
 B->Memptr=Point-B->Memstart;
 B->Pos=Point;
}

/*      Reads a record from the input file of buffer B into the memory window.
        If the window has no room for the record a record is first written out
        to the output file.
*/
void Fast Out_In_Buf(struct Buffer *B)
{
 if (Memmax-B->Memsize<Blocksize)
  Out_Buf(B);
 In_Buf(B);
}

/*      Initializes all buffer variables including the file names for the
        input and output files of a buffer.
*/
void Fast B_Init(struct Buffer *B)
{Zero_Buffer(B);
}

/*      Removes all the contents of a buffer and reinitializes it's database.
*/
void Fast B_Reset(struct Buffer *B)
{if (B->In_First!=-1)
  Mem_Return(B->In_First);
 if (B->Out_First!=-1)
  Mem_Return(B->Out_First);
 Zero_Buffer(B);
}

/*      Intializes all the variables associated with a buffer.
*/
void Fast Zero_Buffer(struct Buffer *B)
{B->Inno=B->Outno=B->Memsize=B->Memptr=B->Bufsize=B->Modified=0;
 B->In_First=B->In_Last=B->Out_First=B->Out_Last=B->In_This= -1;
 B->Memstart=B->Pos=1;
}

/*      Makes a buffer ready for modification by writing all data behind
        the memory window to the output file. It then sets a flag to indicate
        that this process has taken place.
*/
void Fast B_Modify(struct Buffer *B)
{register long Sav_Pos;
 Sav_Pos=B->Pos;
 B_Rewind(B);
 B->Modified=1;
 B_Goto(B,Sav_Pos);
}

/*      Ensures that a buffer is ready for insertion of data into the
        window.
*/
void Fast Make_Room(struct Buffer *B)
{long Saveptr;
 if (B->Memptr<Blocksize)
 {Saveptr=B->Pos;
  B_Rewind(B);
  B_Modify(B);
  B_Goto(B,Saveptr);
  if (B->Memsize==Memmax)
   Out_Buf(B);
 } else
  Out_Buf(B);
}

/*      Inserts the character C into the buffer B.
*/
void Fast B_Put(struct Buffer *B,char C)
{if (!B->Modified)
  B_Modify(B);
 /*Diag("B_Put:Memsize=%d,Memptr=%d",B->Memsize,B->Memptr);*/
 if (B->Memsize==Memmax)
  Make_Room(B);
 if (B->Memsize>B->Memptr)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move((B->Membuf)+B->Memptr,(B->Membuf)+B->Memptr+1,
   B->Memsize-B->Memptr);
#else
  C_Move((B->Membuf)+B->Memptr,(B->Membuf)+B->Memptr+1,
   B->Memsize-B->Memptr,1);
#endif
 B->Memsize++;
 B->Membuf[B->Memptr++]=C;
 B->Pos++;
 B->Bufsize++;
}

/*      Inserts the string S of length L into the buffer T.
*/
void Fast B_Put_S(struct Buffer *T,char *S,int L)
{register int I;
 if (!T->Modified)
  B_Modify(T);
 while (L!=0)
 {if (T->Memsize==Memmax)
   Make_Room(T);
  if ((I=L)>Memmax-T->Memsize)
   I=Memmax-T->Memsize;
  if (T->Memsize>T->Memptr)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
   C_Move(T->Membuf+T->Memptr,T->Membuf+T->Memptr+I,
    T->Memsize-T->Memptr);
#else
   C_Move(T->Membuf+T->Memptr,T->Membuf+T->Memptr+I,
    T->Memsize-T->Memptr,1);
#endif
  T->Memsize+=I;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move(S,T->Membuf+T->Memptr,I);
#else
  C_Move(S,T->Membuf+T->Memptr,I,0);
#endif
  T->Memptr+=I;
  T->Pos+=I;
  T->Bufsize+=I;
  S+=I;
  L-=I;
 }
}

/*      Replaces the string S of length L into the buffer T replacing exisiting
        text.
*/
void Fast B_Repl_S(struct Buffer *B,char *S,int L)
{register int I;
 if (!B->Modified)
  B_Modify(B);
 while (L!=0)
 {if (B->Memptr>=B->Memsize)
   Out_In_Buf(B);
  if ((I=L)>B->Memsize-B->Memptr)
   I=B->Memsize-B->Memptr;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move(S,B->Membuf+B->Memptr,I);
#else
  C_Move(S,B->Membuf+B->Memptr,I,0);
#endif
  B->Memptr+=I;
  B->Pos+=I;
  S+=I;
  L-=I;
 }
}

/*      Replaces the character at the current position of the buffer B by the
        character C.
*/
int Fast B_Replace(struct Buffer *B,int C)
{int Old_C;
 if (!B->Modified)
  B_Modify(B);
 if (B->Memptr>=B->Memsize)
  Out_In_Buf(B);
 Old_C=B->Membuf[B->Memptr];
 B->Membuf[B->Memptr++]=(char /*mdj*/ )C;
 B->Pos++;
 return Old_C&255;
}

/*      Removes the character at the current position of the buffer B and
        returns the removed character.
*/
int Fast B_Del(struct Buffer *B)
{register char C;
 if (!B->Modified)
  B_Modify(B);
 if (B->Memptr>=B->Memsize)
  B_Goto(B,B->Pos);
 C=B->Membuf[B->Memptr];
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
 C_Move(B->Membuf+B->Memptr+1,B->Membuf+B->Memptr,
  B->Memsize-B->Memptr-1);
#else
 C_Move(B->Membuf+B->Memptr+1,B->Membuf+B->Memptr,
  B->Memsize-B->Memptr-1,1);
#endif
 B->Memsize--;
 B->Bufsize--;
 /*Diag("B_Del:Inno=%d Outno=%d Memsize=%d Memstart=%ld Memptr=%d",B->Inno,
  B->Outno,B->Memsize,B->Memstart,B->Memptr);*/
 return(C&255);
}

/*      Returns the character at the current position of buffer B and advances
        the position by one character.
*/
int Fast B_Get(struct Buffer *B)
{if (B->Memptr>=B->Memsize)
  Out_In_Buf(B);
 B->Pos++;
 return((B->Membuf[B->Memptr++])&255);
}

/*      Copies L characters from buffer to pointer P and advances the pointer.
*/
void Fast B_Get_N(struct Buffer *B,char *S,int M)
{register int I /* mdj , L */;
/* mdj   L=0; */
 while (M)
 {if (B->Memptr>=B->Memsize)
   B_Goto(B,B->Pos);
  if ((I=M)>B->Memsize-B->Memptr)
   I=B->Memsize-B->Memptr;
  if (I>B->Bufsize-B->Pos+1)
   I=B->Bufsize-B->Pos+1;
  M-=I;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move(B->Membuf+B->Memptr,S,I);
#else
  C_Move(B->Membuf+B->Memptr,S,I,0);
#endif
  S+=I;
/* mdj  L+=I; */
  if ((B->Pos=B->Memstart+(B->Memptr+=I))>B->Bufsize)
   break;
 }
}

/*      Gets a NL terminated string from the buffer and advances the pointer
*/
int Fast B_Get_S(struct Buffer *B,char *S,int M)
{register int I, L, J;
 L=0;
 while (M)
 {if (B->Memptr>=B->Memsize)
   B_Goto(B,B->Pos);
  if ((I=M)>B->Memsize-B->Memptr)
   I=B->Memsize-B->Memptr;
  if (I>B->Bufsize-B->Pos+1)
   I=B->Bufsize-B->Pos+1;
  M-=I;
#if (defined(DOS) & ! defined(Windows)) | defined(NT) | defined(OS2)
  if (I==(J=C_Scan((char far *)B->Membuf+B->Memptr,I,'\n')))
#else
  if (I==(J=C_Scan(B->Membuf+B->Memptr,I,'\n')))
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  {C_Move(B->Membuf+B->Memptr,S,J);
#else
  {C_Move(B->Membuf+B->Memptr,S,J,0);
#endif
   S+=J;
   L+=J;
   B->Memptr+=J;
  } else
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  {C_Move(B->Membuf+B->Memptr,S,++J);
#else
  {C_Move(B->Membuf+B->Memptr,S,++J,0);
#endif
   B->Memptr+=J;
   B->Pos=B->Memstart+B->Memptr;
   return L+J;
  }
  if ((B->Pos=B->Memstart+B->Memptr)>B->Bufsize)
   break;
 }
 return L;
}

/*      Removes L characters from the buffer B.
*/
void Fast B_Rem(struct Buffer *B,long L)
{register int I;
 /*Diag("B_Rem:L=%D,A=%D",L,B->Bufsize-B->Pos);*/
 if (!B->Modified)
  B_Modify(B);
 while (L!=0)
 {if (B->Memptr>=B->Memsize)
   B_Goto(B,B->Pos);
  if ((I=B->Memsize-B->Memptr)>L)
   I=L;
  if (B->Memsize-B->Memptr-I)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
   C_Move(B->Membuf+B->Memptr+I,B->Membuf+B->Memptr,
    B->Memsize-B->Memptr-I);
#else
   C_Move(B->Membuf+B->Memptr+I,B->Membuf+B->Memptr,
    B->Memsize-B->Memptr-I,1);
#endif
  B->Memsize-=I;
  B->Bufsize-=I;
  L-=I;
 }
}

/*      Copies L characters from the buffer F to the buffer T. The positions
        of both buffers are advanced to the first character after the copied
        text.
*/
void Fast B_Copy(struct Buffer *F,struct Buffer *T,long L)
{long I;
 if (!T->Modified)
  B_Modify(T);
 while (L!=0)
 {if (F->Memptr>=F->Memsize)
   B_Goto(F,F->Pos);
  if ((I=L)>F->Memsize-F->Memptr)
   I=F->Memsize-F->Memptr;
  if (T->Memsize==Memmax)
   Make_Room(T);
  if (I>Memmax-T->Memsize)
   I=Memmax-T->Memsize;
  if (T->Memsize>T->Memptr)
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
   C_Move(T->Membuf+T->Memptr,T->Membuf+T->Memptr+I,
    T->Memsize-T->Memptr);
#else
   C_Move(T->Membuf+T->Memptr,T->Membuf+T->Memptr+I,
    T->Memsize-T->Memptr,1);
#endif
  T->Memsize+=I;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move(F->Membuf+F->Memptr,T->Membuf+T->Memptr,(int)I);
#else
  C_Move(F->Membuf+F->Memptr,T->Membuf+T->Memptr,(int)I,1);
#endif
  T->Memptr+=I;
  T->Pos+=I;
  T->Bufsize+=I;
  F->Memptr+=I;
  F->Pos+=I;
  L-=I;
 }
}

/*      Moves the position of the buffer B one character back and returns the
        character at the new position.
*/
int Fast B_B_Get(struct Buffer *B)
{if (B->Memptr==0)
  if (B->Pos!=1)
  {B_Goto(B,B->Pos-1);
   B_Get(B);
  } else
  {Sedt_Shutdown();
   Fatalerror(Rel_Bad_Txt);
#if defined(OS2)
   DosExit(1,1);
#else
#if defined(DESQVIEW)
   if (DV)
    api_exit();
#endif
#if defined(DOS) & defined(TV)
   TV_Exit(1);
#endif
   _exit(1);
#endif
  }
 B->Pos--;
 return((B->Membuf[--B->Memptr])&255);
}

/*      Searches for the character C in the buffer B from the character after
        the current position up to the position E-1. Returns the position of
        the first occurrence of the character if found and NULL if the 
        character is not found.
*/
int Fast B_Search(struct Buffer *B,char C,long E)
{register int L_Memptr, L_Memsize;
 register char *M;
 register long L, I, J;
 M=B->Membuf;
 L_Memptr=B->Memptr;
 L_Memsize=B->Memsize;
 L=E-B->Pos;
 while (L>0)
 {if (L_Memptr>=L_Memsize)
  {B->Memptr=L_Memptr;
   Out_In_Buf(B);
   L_Memptr=B->Memptr;
   L_Memsize=B->Memsize;
  }
  if ((I=L)>L_Memsize-L_Memptr)
   I=L_Memsize-L_Memptr;
#if (defined(DOS) & !defined(Windows)) | defined(NT) | defined(OS2)
  L-=J=C_Scan((char far *)M+L_Memptr,(int) I,C);
#else
  L-=J=C_Scan(M+L_Memptr,(int) I,C);
#endif
  L_Memptr+=J;
  if (I!=J)
  {B->Pos=E-L+1;
   B->Memptr=L_Memptr+1;
   return(1);
  }
 }
 B->Pos=E;
 B->Memptr=L_Memptr;
 return(0);
}

/*      Searches for the character C in the buffer B from the character before
	the current position back to the position E. Returns the position of
	the first occurrence of the character if found. Returns NULL if the
	character is not found.
*/
int Fast B_B_Search(struct Buffer *B,char C,long E)
{register int L_Memptr;
 register char *M;
 register long L, I, J;
 M=B->Membuf;
 L=B->Pos-E;
 L_Memptr=B->Memptr;
 while (L>0)
 {if (L_Memptr==0)
  {B->Memptr=L_Memptr;
   B_Goto(B,E+L-1);
   B_Get(B);
   L_Memptr=B->Memptr;
  }
  if ((I=L)>L_Memptr)
   I=L_Memptr;
#if (defined(DOS) & !defined(Windows)) | defined(NT) | defined(OS2)
  L+=J=C_Bscan((char far *)M+L_Memptr-1,(int) -I,C);
#else
  L+=J=C_Bscan(M+L_Memptr-1,(int) -I,C);
#endif
  L_Memptr+=J;
  if (-J!=I)
  {B->Pos=E+L-1;
   B->Memptr=L_Memptr-1;
   return(1);
  }
 }
 B->Pos=E;
 B->Memptr=L_Memptr;
 return(0);
}

/*      Returns a record from a virtual file to the free list of records.
*/
void Fast Mem_Return(int M)
{
 int N;
 N=M;
#if (defined(DOS) | defined(NT)) & defined(Windows)
 Mem_List=(struct Memory far *)GlobalLock(Mem_Handle);
#endif
 while (Mem_List[N].Next!=-1)
  N=Mem_List[N].Next;
 Mem_List[N].Next=Free_List;
 Free_List=M;
#if (defined(DOS) | defined(NT)) & defined(Windows)
 GlobalUnlock(Mem_Handle);
#endif
}

#if defined(Noasm) | (defined(UNIX) & !defined(SCO))
/*      Scans for the character C in the first N characters of the character 
        string B. Returns N if the character is not found and the position
        within B (zero origin) if it is found.
*/
int C_Scan(char *B, int N, char C)
{
#if defined(UNIX)
 char *P;
 if ((P=memchr(B,C,N))==NULL)
  return N; else
  return P-B;
#else
 register int I;
 I=N;
 do
 {if ((*B++)==C)
   return(N-I);
 } while ((--I)!=0);
 return(N);
#endif
}

/*      Scans backwards for N characters from the position B for the character
        C. If the character is found it returns the relative position from
        the start otherwise returns -N.
*/
int C_Bscan(char *B, int N, char C)
{register int I;
 I=(-N);
 do
 {if ((*B--)==C)
   return(N+I);
 } while ((--I)!=0);
 return(N);
}
#endif

void Fast Init_Buf()
{
#if (defined(DOS) & !defined(Windows))
 segread(&Seg_Regs);
#endif
#if defined(DOS) | defined(NT) | defined(OS2)
#if defined(Windows)
 if ((Mem_Handle=GlobalAlloc(GMEM_MOVEABLE,
  (DWORD) (sizeof(struct Memory)*Mem_Entries)))==(HANDLE) NULL)
#else
#if defined(__LARGE__)
#if defined(__BORLANDC__)
 if ((Mem_List=
 (struct Memory *)malloc(sizeof(struct Memory)*Mem_Entries))==NULL)
#else
 if ((Mem_List=new struct Memory[Mem_Entries])==NULL)
#endif
#else
 if (((*((unsigned *)&(Mem_List) + 1))=
  Mem_Alloc(sizeof(struct Memory)*Mem_Entries))==0)
#endif
#endif
#else
#if defined(ATARI)
 if ((Mem_List=
  (struct Memory *)malloc(sizeof(struct Memory)*Mem_Entries))==NULL)
#else
 if ((Mem_List=
  (struct Memory *)malloc(sizeof(struct Memory)*Mem_Entries))==NULL)
#endif
#endif
  Initerror(No_Memory_Txt);
#if defined(DOS) & !defined(Windows)
 Got_EMS=Check_EMS();
#endif
 Overflow.F=0;
}

void Fast Reset_Buf()
{
#if defined(DOS) & !defined(Windows)
 {register int I;
  for (I=M_Next-1;I>=0;I--)
  {if (Mem_List[I].EMS_H!=0&&Mem_List[I].Mem_Segaddr==0)
    Return_EMS(Mem_List[I].EMS_H); else
   if (Mem_List[I].EMS_H==0&&Mem_List[I].Mem_Segaddr!=0)
    M_Free(Mem_List[I].Mem_Segaddr);
  }
 }
 M_Free(*((unsigned *)&(Mem_List) + 1));
#endif
 if (Overflow.F!=0)
 {
#if defined(ATARI)
  Fclose(Overflow.F);
  Fdelete(Overflow.File_Name);
#endif
#if defined(VMS) | defined(UNIX) | defined(MPE)
  close(Overflow.F);
#if defined(UNIX)
  unlink(Overflow.File_Name);
#endif
#endif
#if defined(DOS) | defined(WIN32)
#if defined(Windows)
  close(Overflow.F);
#else
  fclose(Overflow.F);
#endif
  unlink(Overflow.File_Name);
#endif
#if defined(NT)
 _close(Overflow.F);
 unlink(Overflow.File_Name);
#endif
#if defined(OS2)
  fclose(Overflow.F);
  unlink(Overflow.File_Name);
#endif
 }
#if (defined(DOS) | defined(NT)) & defined(Windows)
 GlobalFree(Mem_Handle);
#endif
}

#if defined(DOS) & !defined(Windows)
/*      Attempts to allocate memory for a virtual record. Returns the position
	of the record if succesful and 0 if not.
*/
int Fast Mem_Alloc(unsigned Size)
{int Mem_Seg, Para_Alloc;
 if (Mem_Limit==0)
  return 0;
 if (Mem_Limit>0)
 {if ((Mem_Limit-=Size)<0)
   Mem_Limit=0;
 }
 if (Got_EMS&&EMS_Buffering)
 {if (EMS_Top==0)
   EMS_Handle=Get_EMS();
  if (EMS_Handle!=0)
  {Mem_Seg=EMS_Top;
   if ((EMS_Top+=Blocksize)+Blocksize>=16382)
    EMS_Top=0;
   return Mem_Seg;
  }
 }
 EMS_Handle=0;
#if defined(DOS) & defined(__LARGE__)
 return 0;
#else
 if (!Mem_Buffering)
  return 0;
 Para_Alloc=(Size/16)+(Size%16?1:0);
 if ((Mem_Seg=M_Alloc(Para_Alloc))==0)
  return 0;
 return(Mem_Seg);
#endif
}

void Fast Move_To_EMS(int H,int B,char *A)
{char far *A_Ptr;
 /*Diag("Move_To_EMS:H=%d,B=%d",H,B);*/
 In_Regs.h.ah=0x44;
 In_Regs.h.al=0;
 In_Regs.x.bx=0;
 In_Regs.x.dx=H;
 int86x(0x67,&In_Regs,&Out_Regs,&Seg_Regs);
 In_Regs.h.ah=0x41;
 int86x(0x67,&In_Regs,&Out_Regs,&Seg_Regs);
#if defined(__BORLANDC__)
 movedata(FP_SEG(A),FP_OFF(A),Out_Regs.x.bx,B,Blocksize);
#else
 A_Ptr=A;
 _movedata(FP_SEG(A_Ptr),FP_OFF(A_Ptr),Out_Regs.x.bx,B,Blocksize);
#endif
}

void Fast Move_From_EMS(int H,int B,char *A)
{char far *A_Ptr;
 /*Diag("Move_From_EMS:H=%d,B=%d",H,B);*/
 In_Regs.h.ah=0x44;
 In_Regs.h.al=0;
 In_Regs.x.bx=0;
 In_Regs.x.dx=H;
 int86x(0x67,&In_Regs,&Out_Regs,&Seg_Regs);
 In_Regs.h.ah=0x41;
 int86x(0x67,&In_Regs,&Out_Regs,&Seg_Regs);
#if defined(__BORLANDC__)
 movedata(Out_Regs.x.bx,B,FP_SEG(A),FP_OFF(A),Blocksize);
#else
 A_Ptr=A;
 _movedata(Out_Regs.x.bx,B,FP_SEG(A_Ptr),FP_OFF(A_Ptr),Blocksize);
#endif
}

int Fast Check_EMS()
{char Int_67_Name[8];
 char far *Int_67_Ptr;
 Int_67_Ptr=Int_67_Name;
 In_Regs.h.ah=0x35;
 In_Regs.h.al=0x67;
 intdosx(&In_Regs,&Out_Regs,&Seg_Regs);
#if defined(__BORLANDC__)
 movedata(Seg_Regs.es,10,FP_SEG(Int_67_Name),FP_OFF(Int_67_Name),8);
#else
 _movedata(Seg_Regs.es,10,FP_SEG(Int_67_Ptr),FP_OFF(Int_67_Ptr),8);
#endif
 return memcmp("EMMXXXX0",Int_67_Name,8)==0;
}

int Fast Get_EMS()
{
 /*Diag("Get_EMS");*/
 In_Regs.h.ah=0x42;
 int86x(0x67,&In_Regs,&Out_Regs,&Seg_Regs);
 if (Out_Regs.h.ah!=0||Out_Regs.x.bx==0)
  return 0;
 In_Regs.h.ah=0x43;
 In_Regs.x.bx=1;
 int86x(0x67,&In_Regs,&Out_Regs,&Seg_Regs);
 if (Out_Regs.h.ah!=0)
  return 0;
 /*Diag("Get_EMS:H=%d",Out_Regs.x.dx);*/
 return Out_Regs.x.dx;
}

void Fast Return_EMS(int H)
{In_Regs.h.ah=0x45;
 In_Regs.x.dx=H;
 int86x(0x67,&In_Regs,&Out_Regs,&Seg_Regs);
}
#endif

#if defined(OS2)
int Mem_Alloc(Size)
unsigned Size;
{int Selector;
 if (DosAllocSeg(Size,(PSEL)&Selector,0))
  return 0; else
  return Selector;
}
#endif

