/*Copyright (c) Anker Berg-Sonne, Digital Equipment Corporation, 1985*/
#define Usemem 1
#if defined(VMS) | defined(ATARI) | defined(UNIX) | defined(MPE)
#if defined(VMS)
#define Memmax 30000
#define Blocksize 5120
#endif
#if defined(ATARI)
#define Memmax 4500
#define Blocksize 2048
#endif
#if defined(MPE)
#define Memmax 2000
#define Blocksize 512
#endif
#if defined(UNIX)
#if defined(XENIX)
#define Memmax 4500
#define Blocksize 2048
#else
#define Memmax 30000
#define Blocksize 5120
#endif
#endif
struct fcb_t 
{int F, P;
 char File_Name[13];
};
#endif
#if defined(DOS) | defined(OS2) | defined(NT) | defined(WIN32)
#define Memmax 2500
#define Blocksize 1024
struct fcb_t
{
#if defined(Windows)
 int F;
#else
 FILE *F;
#endif
 int P;
 char File_Name[80];
};
#endif
struct Memory {
 int Next, Last;
 int No;
#if defined(DOS) | defined(NT) | defined(OS2)
#if !defined(Windows)
 int Mem_Segaddr;
#if defined(DOS)
 int EMS_H;
#endif
#endif
#else
 char *Mem_Block;
#endif
 int Disk_Block;
};
struct Buffer {
 long Pos, Bufsize, Memstart;
 int Inno, Outno, Memsize, Memptr, Modified;
 int In_First, In_Last, Out_First, Out_Last, In_This;
 char Membuf[Memmax];
};
