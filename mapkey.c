#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "vdudef.h"
#include "file.h"

#ifdef WIN32
#define far
#endif

extern int Map_Getc(void);
extern int Scan(void);
extern int Char(void);
extern void Test(char *);
extern void Compile_Keys(char *, char*);
extern void Dump_Keys(char *, char*);
extern void Compile_Functions(char *, char*);
extern void Dump_Functions(char *, char*);
extern int Create(char*);
extern void Edit_Maps(char *, char*);
#define huh extern int Error( char *S, ...);
extern int Read_Type(void);
extern void Readkeys(void);
extern int GETSCAN(int far *);
extern int Getkey(void);
extern int Check_Comb(int);
extern void OURKBD(void);
extern void KEYREL(void);
extern int Get_Character(void);


#if defined(UNIX)
#if !defined(TERMCAP)
#if defined(ULTRIX)
#include <cursesX.h>
#else
#include <curses.h>
#endif
#endif
#endif

#if defined(VMS)
int Got_In, This_In;
char In_Buf[256];
#endif

#if defined(ATARI)
#include <osbind.h>
long Con_Input;
#endif

#if defined(MPE)
int mpe_stdin, mpe_stdout;
#pragma intrinsic FREAD,FOPEN,FCONTROL,FSETMODE,FWRITE
#endif

#if defined(OS2)
#define INCL_DOS 1
#define INCL_SUB 1
#include <os2.h>

struct _GINFOSEG far *Gdt;
struct _LINFOSEG far *Ldt;

struct MonBuff
{				/* generic monitor buffer header */
  unsigned bufflen;
  unsigned ddbufflen;
  unsigned char dispatch[12];
  unsigned char dummy[48];
} InBuff, OutBuff;		/* buffers for monitor read/writes */

struct CharPacket
{				/* substructure of KeyPacket */
  unsigned char asc;
  unsigned char scan;
  unsigned char status;
  unsigned char dshift;
  unsigned shift;
  unsigned long time;
};

struct KeyPacket
{				/* KBD monitor data record */
  unsigned mnflags;
  struct CharPacket cp;
  unsigned ddflags;
} Key_Buff;

struct _KBDINFO Kbd_Status;
struct _MOUEVENTINFO Event;
struct _PTRLOC Ptr_Loc;

int Old_Length, KBD_Handle, Sav_Shift_State;
unsigned Gdt_Descriptor, Ldt_Descriptor, Thread_ID, Mouse_ID, Mouse_Handle;
char Thread_Stack[2048], Mouse_Stack[2048];
struct _VIOMODEINFO Mode_Data;
struct _VIOCURSORINFO Cursor_Data;
HSEM Mouse_Sem = 0;
#endif

struct Scan_Tab_Struct Scan_Tab[Maxscan], *Scan_Pointer;
struct Map_Tab_Struct Map_Tab[Maxmap], *Map_Pointer;
struct Key_Tab_Struct Key_Tab[1024], *Key_Pointer;
struct Comb_Tab_Struct Comb_Tab[1024], *Comb_Pointer;

int I, C, Scancode, Map_Defs, Scan_Defs, Line, Function, Flags, Character,
  Enhanced, Gold, Name_Index, Func_Def_Index, Top_Func, Index,
  Scan_Number, Key_Defs, Key_Number, Key, Version, Comb_Defs, Comb_Level,
  Keyb_Type = 0, Edit, LF_Last = 1, File_Debth = 0, Save_Line[10], CC_Flag;

char *P, *Q, Name[80], Names[1024 * 10], Func_Defs[1024 * 10],
  Dsk_Rec[Blksize + 1], Init_String[256], Reset_String[256];

long Funcs[Maxfunc][3], *FP;

FILE *F;

FILE *Ind_File[10];
char File_Name[80];

void main (Argc, Argv)
     int Argc;
     char *Argv[];
{
  Init_String[0] = 0;
  Reset_String[0] = 0;
  Version = 4;
  Edit = 12;
  printf ("Mapkey version %d(%d)\n", Version, Edit);
  for (I = 0; I < Argc; I++)
    {
      P = Argv[I];
      while ((C = *P) != 0)
	if (C >= 'A' && C <= 'Z')
	  *P++ = tolower (C);
	else
	  P++;
    }
#ifndef WIN32
  if (Argc == 2 && strcmp (Argv[1], "scan") == 0)
    Scan ();
  else if (Argc == 2 && strcmp (Argv[1], "char") == 0)
    Char ();
  else if (Argc == 3 && strcmp (Argv[1], "test") == 0)
    Test (Argv[2]);
  else
#endif

    if (Argc == 5 && strcmp (Argv[1], "compile") == 0
	&& strcmp (Argv[2], "keys") == 0)
    Compile_Keys (Argv[3], Argv[4]);
  else
    if (Argc == 5 && strcmp (Argv[1], "dump") == 0
	&& strcmp (Argv[2], "keys") == 0)
    Dump_Keys (Argv[3], Argv[4]);
  else
    if (Argc == 5 && strcmp (Argv[1], "compile") == 0
	&& strcmp (Argv[2], "functions") == 0)
    Compile_Functions (Argv[3], Argv[4]);
  else
    if (Argc == 5 && strcmp (Argv[1], "dump") == 0
	&& strcmp (Argv[2], "functions") == 0)
    Dump_Functions (Argv[3], Argv[4]);

#ifndef WIN32
  else if (Argc == 3 && strcmp (Argv[1], "create") == 0)
    Create (Argv[2]);
  else if (Argc == 4 && strcmp (Argv[1], "edit") == 0)
    Edit_Maps (Argv[2], Argv[3]);
#endif
  else
    {
#ifndef WIN32
      printf ("MAPKEY SCAN displays keyboard scancodes directly without\n");
      printf ("      firmware intervention\n");
      printf ("\n");
      printf
	("MAPKEY CHAR displays scancodes and characters after firmware\n");
      printf ("      processing\n");
      printf ("\n");
      printf
	("MAPKEY CREATE <ascii mapfile> interactively creates a mapfile\n");
      printf ("\n");
      printf ("MAPKEY EDIT <input ascii mapfile> <output ascii mapfile>\n");
      printf ("      interactively edits an ascii mapfile\n");
      printf ("\n");
      printf ("MAPKEY TEST <ascii mapfile> tests a mapfile interactively\n");
      printf ("\n");
#endif
      printf
	("MAPKEY COMPILE KEYS <ascii mapfile> <binary mapfile> converts an\n");
      printf ("      ASCII mapfile into binary form for Sedt\n");
      printf ("\n");
      printf
	("MAPKEY DUMP KEYS <binary mapfile> <ascii mapfile> converts a\n");
      printf ("      binary mapfile into an ASCII mapfile\n");
      printf ("\n");
      printf
	("MAPKEY COMPILE FUNCTIONS <ascii file> <binary file> converts an\n");
      printf ("      ASCII function file into binary form for Sedt\n");
      printf ("\n");
      printf
	("MAPKEY DUMP FUNCTIONS <binary file> <ascii file> converts a\n");
      printf ("      binary function file into an ASCII function file\n");
      printf ("\n");
      printf ("Use the Escape key to exit");
      printf ("\n");
    }
}

void Readkeys ()
{
  int K;
  char *P;
  Map_Defs = Scan_Defs = Key_Defs = 0;
  Line = 1;
  Name_Index = 0;
  C = Map_Getc ();
  K = Map_Getc ();
  while (K != '\n' && K != EOF)
    K = Map_Getc ();
  if (C == 'I' || C == 'i')
    {
      P = Init_String;
      while ((C = Map_Getc ()) != '\n' && C != EOF)
	*P++ = C;
      *P++ = 0;
      C = Map_Getc ();
      K = Map_Getc ();
      while (K != '\n' && K != EOF)
	K = Map_Getc ();
    }
  if (C == 'R' || C == 'r')
    {
      P = Reset_String;
      while ((C = Map_Getc ()) != '\n' && C != EOF)
	*P++ = C;
      *P++ = 0;
      C = Map_Getc ();
      K = Map_Getc ();
      while (K != '\n' && K != EOF)
	K = Map_Getc ();
    }
  if (C == 'S' || C == 's')
    {
      while (1)
	{
	  Flags = 0;
	  if ((C = Map_Getc ()) == EOF)
	    break;
	  {
	    while (C == ' ' || C == 9)
	      C = Map_Getc ();
	    if (C == 'M' || C == 'm' || C == 'C' || C == 'c' || C == 'K'
		|| C == 'k')
	      {
		K = Map_Getc ();
		while (K != '\n' && K != EOF)
		  K = Map_Getc ();
		break;
	      }
	    if (C != '\n' && C != EOF)
	      {
		if (Scan_Defs == Maxscan)
		  Error
		    ("Maximum number of table entries exceeded on line %d",
		     Line);
		else
		  {
		    K = Read_Type ();
		    if (K == 'S')
		      {
			Scan_Pointer = &Scan_Tab[Scan_Defs];
			Scan_Pointer->Scancode = Scancode;
			Scan_Pointer->Char = Character;
			Scan_Pointer->Flags = 0;
			Scan_Defs++;
		      }
		    else if (K == 'E')
		      {
			Scan_Pointer = &Scan_Tab[Scan_Defs];
			Scan_Pointer->Scancode = Scancode;
			Scan_Pointer->Char = Character;
			Scan_Pointer->Flags = Ext_Code;
			Scan_Defs++;
		      }
		    else if (K == 'W')
		      {
			Scan_Pointer = &Scan_Tab[Scan_Defs];
			Scan_Pointer->Scancode = Scancode;
			Scan_Pointer->Char = Character;
			Scan_Pointer->Flags = Window_Code;
			Scan_Defs++;
		      }
		    else
		      {
			Error ("Error in key type on line %d", Line);
		      }
		    while (C != '\n' && C != EOF)
		      C = Map_Getc ();
		  }
	      }
	  }
	}
    }
  if (C == 'K' || C == 'k')
    {
      while (1)
	{
	  Flags = 0;
	  if ((C = Map_Getc ()) == EOF)
	    break;
	  {
	    while (C == ' ' || C == 9)
	      C = Map_Getc ();
	    if (C == 'M' || C == 'm' || C == 'C' || C == 'c')
	      {
		K = Map_Getc ();
		while (K != '\n' && K != EOF)
		  K = Map_Getc ();
		break;
	      }
	    if (C != '\n' && C != EOF)
	      {
		Flags = 0;
		K = Read_Type ();
		if (K == 'E')
		  {
		    while (C == ' ' || C == 9)
		      C = Map_Getc ();
		    if (C == 'G' || C == 'g')
		      {
			Key_Number = 0;
			C = Map_Getc ();
		      }
		    else if (C >= '0' && C <= '9')
		      {
			Key_Number = C - '0';
			while ((C = Map_Getc ()) >= '0' && C <= '9')
			  Key_Number = Key_Number * 10 + C - '0';
			while (C == ' ' || C == 9)
			  C = Map_Getc ();
			Index = Name_Index + 1;
			while (C != '\n' && C != EOF)
			  {
			    Names[Name_Index++] = C;
			    C = Map_Getc ();
			  }
			Names[Name_Index++] = 0;
			Key_Pointer = &Key_Tab[Key_Defs];
			Key_Pointer->Scancode = Scancode;
			Key_Pointer->Char = Character;
			Key_Pointer->Flags = Flags + Ext_Code;
			Key_Pointer->Key = Key_Number;
			Key_Pointer->Index = Index;
			Key_Defs++;
		      }
		    else
		      {
			Error ("Error in Key_Number key on line %d", Line);
			while (C != '\n' && C != EOF)
			  C = Map_Getc ();
		      }
		  }
		else if (K == 'S')
		  {
		    while (C == ' ' || C == 9)
		      C = Map_Getc ();
		    if (C == 'G' || C == 'g')
		      {
			Key_Number = 0;
			C = Map_Getc ();
		      }
		    else if (C >= '0' && C <= '9')
		      {
			Key_Number = C - '0';
			while ((C = Map_Getc ()) >= '0' && C <= '9')
			  Key_Number = Key_Number * 10 + C - '0';
			while (C == ' ' || C == 9)
			  C = Map_Getc ();
			Index = Name_Index + 1;
			while (C != '\n' && C != EOF)
			  {
			    Names[Name_Index++] = C;
			    C = Map_Getc ();
			  }
			Names[Name_Index++] = 0;
			Key_Pointer = &Key_Tab[Key_Defs];
			Key_Pointer->Scancode = Scancode;
			Key_Pointer->Char = Character;
			Key_Pointer->Flags = Flags;
			Key_Pointer->Key = Key_Number;
			Key_Pointer->Index = Index;
			Key_Defs++;
		      }
		    else
		      {
			Error ("Error in Key_Number key on line %d", Line);
			while (C != '\n' && C != EOF)
			  C = Map_Getc ();
		      }
		  }
		else if (K == 'A')
		  {
		    while (C == ' ' || C == 9)
		      C = Map_Getc ();
		    if (C == 'G' || C == 'g')
		      {
			Key_Number = 0;
			C = Map_Getc ();
		      }
		    else if ((C >= '0' && C <= '9') || C != '\'')
		      {
			if (C == '\'')
			  {
			    C = Map_Getc ();
			    if (C != '\n' && C != EOF)
			      {
				Key_Number = C;
				C = Map_Getc ();
			      }
			    else
			      Key_Number = 0;
			  }
			else
			  {
			    Key_Number = C - '0';
			    while ((C = Map_Getc ()) >= '0' && C <= '9')
			      Key_Number = Key_Number * 10 + C - '0';
			  }
			while (C == ' ' || C == 9)
			  C = Map_Getc ();
			Index = Name_Index + 1;
			while (C != '\n' && C != EOF)
			  {
			    Names[Name_Index++] = C;
			    C = Map_Getc ();
			  }
			Names[Name_Index++] = 0;
			Key_Pointer = &Key_Tab[Key_Defs];
			Key_Pointer->Scancode = 0;
			Key_Pointer->Char = Character;
			Key_Pointer->Flags = Flags + Char_Code;
			Key_Pointer->Key = Key_Number;
			Key_Pointer->Index = Index;
			Key_Defs++;
		      }
		    else
		      {
			Error ("Error in Key_Number key on line %d", Line);
			while (C != '\n' && C != EOF)
			  C = Map_Getc ();
		      }
		  }
		else
		  {
		    Error ("Error in translation type on line %d", Line);
		    while (C != '\n' && C != EOF)
		      C = Map_Getc ();
		  }
	      }
	  }
	}
    }
  if (C == 'C' || C == 'c')
    {
      while (1)
	{
	  Flags = 0;
	  if ((C = Map_Getc ()) == EOF)
	    break;
	  {
	    while (C == ' ' || C == 9)
	      C = Map_Getc ();
	    if (C == 'M' || C == 'm')
	      {
		K = Map_Getc ();
		while (K != '\n' && K != EOF)
		  K = Map_Getc ();
		break;
	      }
	    if (C != '\n' && C != EOF)
	      {
		if (C < '0' || C > '9')
		  {
		    Error ("Error in translation type on line %d", Line);
		    while (C != '\n' && C != EOF)
		      C = Map_Getc ();
		  }
		else
		  {
		    Comb_Level = C - '0';
		    while ((C = Map_Getc ()) >= '0' && C <= '9')
		      Comb_Level = Comb_Level * 10 + C - '0';
		    while (C == ' ' || C == 9)
		      C = Map_Getc ();
		    K = Read_Type ();
		    while (C == ' ' || C == 9)
		      C = Map_Getc ();
		    Function = -1;
		    if (C == 'G' || C == 'g' || (C >= '0' && C <= '9'))
		      {
			if (C == 'G' || C == 'g')
			  {
			    Function = 0;
			    C = Map_Getc ();
			  }
			else if (C >= '0' && C <= '9')
			  {
			    Function = C - '0';
			    while ((C = Map_Getc ()) >= '0' && C <= '9')
			      Function = Function * 10 + C - '0';
			    while (C == ' ' || C == 9)
			      C = Map_Getc ();
			    if (C != '\n' && C != EOF)
			      {
				if (C == '\'' || C == '"')
				  {
				    if (C == '\'')
				      Flags = Prompt_Code;
				    else
				      Flags = Help_Code;
				    C = Map_Getc ();
				  }
				else
				  Flags = Help_Code;
				Index = Name_Index + 1;
				while (C != '\n' && C != EOF)
				  {
				    Names[Name_Index++] = C;
				    C = Map_Getc ();
				  }
				Names[Name_Index++] = 0;
			      }
			    else
			      Index = 0;
			  }
		      }
		    else
		      {
			if (C != '\n' && C != EOF)
			  {
			    if (C == '\'' || C == '"')
			      {
				if (C == '\'')
				  Flags = Prompt_Code;
				else
				  Flags = Help_Code;
				C = Map_Getc ();
			      }
			    else
			      Flags = Help_Code;
			    Index = Name_Index + 1;
			    while (C != '\n' && C != EOF)
			      {
				Names[Name_Index++] = C;
				C = Map_Getc ();
			      }
			    Names[Name_Index++] = 0;
			  }
			else
			  Index = 0;
		      }
		    if (K == 'E')
		      {
			Comb_Pointer = &Comb_Tab[Comb_Defs];
			Comb_Pointer->Level = Comb_Level;
			Comb_Pointer->Scancode = Scancode;
			Comb_Pointer->Char = Character;
			Comb_Pointer->Flags = Flags | Ext_Code;
			if (Function != -1)
			  {
			    Comb_Pointer->Flags |= Final_Code;
			    Comb_Pointer->Key = Function;
			  }
			Comb_Pointer->Index = Index;
			Comb_Defs++;
		      }
		    else if (K == 'S')
		      {
			Comb_Pointer = &Comb_Tab[Comb_Defs];
			Comb_Pointer->Level = Comb_Level;
			Comb_Pointer->Scancode = Scancode;
			Comb_Pointer->Char = Character;
			Comb_Pointer->Flags = Flags;
			if (Function != -1)
			  {
			    Comb_Pointer->Flags |= Final_Code;
			    Comb_Pointer->Key = Function;
			  }
			Comb_Pointer->Index = Index;
			Comb_Defs++;
		      }
		    else if (K == 'A')
		      {
			Comb_Pointer = &Comb_Tab[Comb_Defs];
			Comb_Pointer->Level = Comb_Level;
			Comb_Pointer->Scancode = 0;
			Comb_Pointer->Char = Character;
			Comb_Pointer->Flags = Flags | Char_Code;
			if (Function != -1)
			  {
			    Comb_Pointer->Flags |= Final_Code;
			    Comb_Pointer->Key = Function;
			  }
			Comb_Pointer->Index = Index;
			Comb_Defs++;
		      }
		    else if (K == 'K')
		      {
			Comb_Pointer = &Comb_Tab[Comb_Defs];
			Comb_Pointer->Level = Comb_Level;
			Comb_Pointer->Scancode = 0;
			Comb_Pointer->Char = Key;
			Comb_Pointer->Flags = Flags | Key_Code;
			if (Function != -1)
			  {
			    Comb_Pointer->Flags |= Final_Code;
			    Comb_Pointer->Key = Function;
			  }
			Comb_Pointer->Index = Index;
			Comb_Defs++;
		      }
		    else
		      {
			Error ("Error in translation type on line %d", Line);
			while (C != '\n' && C != EOF)
			  C = Map_Getc ();
		      }
		  }
	      }
	  }
	}
    }
  if (C == 'M' || C == 'm')
    {
      while (1)
	{
	  Flags = 0;
	  if ((C = Map_Getc ()) == EOF)
	    break;
	  {
	    while (C == ' ' || C == 9)
	      C = Map_Getc ();
	    if (C != '\n' && C != EOF)
	      {
		while (C == ' ' || C == 9)
		  C = Map_Getc ();
		if (C < '0' && C > '9')
		  Error ("Map entry not starting with an index on line %d",
			 Line);
		else
		  {
		    Index = C - '0';
		    while ((C = Map_Getc ()) >= '0' && C <= '9')
		      Index = Index * 10 + C - '0';
		    if (Index >= Maxmap)
		      Error
			("Maximum number of table entries exceeded on line %d",
			 Line);
		    else
		      {
			if (Index >= Map_Defs)
			  Map_Defs = Index + 1;
			Map_Pointer = &Map_Tab[Index];
			if (Map_Pointer->Scancode || Map_Pointer->Char
			    || Map_Pointer->Flags || Map_Pointer->F_Code
			    || Map_Pointer->Index)
			  Error ("MAPS index already used on line %d", Line);
			Flags = 0;
			while (C == ' ' || C == 9)
			  C = Map_Getc ();
			if (C == 'G' || C == 'g')
			  {
			    Flags = Gold_Code;
			    C = Map_Getc ();
			    while (C == ' ' || C == 9)
			      C = Map_Getc ();
			  }
			K = Read_Type ();
			if (K == 'E')
			  {
			    while (C == ' ' || C == 9)
			      C = Map_Getc ();
			    if (C == 'G' || C == 'g'
				|| (C >= '0' && C <= '9'))
			      {
				if (C == 'G' || C == 'g')
				  {
				    Function = 0;
				    C = Map_Getc ();
				  }
				else if (C >= '0' && C <= '9')
				  {
				    Function = C - '0';
				    while ((C = Map_Getc ()) >= '0'
					   && C <= '9')
				      Function = Function * 10 + C - '0';
				  }
				Map_Pointer->Scancode = Scancode;
				Map_Pointer->Char = Character;
				Map_Pointer->Flags = Flags + Ext_Code;
				Map_Pointer->F_Code = Function;
				Map_Pointer->Index = Name_Index + 1;
				while (C == ' ' || C == 9)
				  C = Map_Getc ();
				while (C != '\n' && C != EOF)
				  {
				    Names[Name_Index++] = C;
				    C = Map_Getc ();
				  }
				Names[Name_Index++] = 0;
			      }
			    else
			      {
				Error ("Error in function key on line %d",
				       Line);
				while (C != '\n' && C != EOF)
				  C = Map_Getc ();
			      }
			  }
			else if (K == 'S')
			  {
			    while (C == ' ' || C == 9)
			      C = Map_Getc ();
			    if (C == 'G' || C == 'g'
				|| (C >= '0' && C <= '9'))
			      {
				if (C == 'G' || C == 'g')
				  {
				    Function = 0;
				    C = Map_Getc ();
				  }
				else if (C >= '0' && C <= '9')
				  {
				    Function = C - '0';
				    while ((C = Map_Getc ()) >= '0'
					   && C <= '9')
				      Function = Function * 10 + C - '0';
				  }
				Map_Pointer->Scancode = Scancode;
				Map_Pointer->Char = Character;
				Map_Pointer->Flags = Flags;
				Map_Pointer->F_Code = Function;
				Map_Pointer->Index = Name_Index + 1;
				while (C == ' ' || C == 9)
				  C = Map_Getc ();
				while (C != '\n' && C != EOF)
				  {
				    Names[Name_Index++] = C;
				    C = Map_Getc ();
				  }
				Names[Name_Index++] = 0;
			      }
			    else
			      {
				Error ("Error in function key on line %d",
				       Line);
				while (C != '\n' && C != EOF)
				  C = Map_Getc ();
			      }
			  }
			else if (K == 'A')
			  {
			    while (C == ' ' || C == 9)
			      C = Map_Getc ();
			    if (C == 'G' || C == 'g'
				|| (C >= '0' && C <= '9'))
			      {
				if (C == 'G' || C == 'g')
				  {
				    Function = 0;
				    C = Map_Getc ();
				  }
				else if (C >= '0' && C <= '9')
				  {
				    Function = C - '0';
				    while ((C = Map_Getc ()) >= '0'
					   && C <= '9')
				      Function = Function * 10 + C - '0';
				  }
				Map_Pointer->Scancode = 0;
				Map_Pointer->Char = Character;
				Map_Pointer->Flags = Flags + Char_Code;
				Map_Pointer->F_Code = Function;
				Map_Pointer->Index = Name_Index + 1;
				while (C == ' ' || C == 9)
				  C = Map_Getc ();
				while (C != '\n' && C != EOF)
				  {
				    Names[Name_Index++] = C;
				    C = Map_Getc ();
				  }
				Names[Name_Index++] = 0;
			      }
			    else
			      {
				Error ("Error in function key on line %d",
				       Line);
				while (C != '\n' && C != EOF)
				  C = Map_Getc ();
			      }
			  }
			else if (K == 'K')
			  {
			    while (C == ' ' || C == 9)
			      C = Map_Getc ();
			    if (C == 'G' || C == 'g'
				|| (C >= '0' && C <= '9'))
			      {
				if (C == 'G' || C == 'g')
				  {
				    Function = 0;
				    C = Map_Getc ();
				  }
				else if (C >= '0' && C <= '9')
				  {
				    Function = C - '0';
				    while ((C = Map_Getc ()) >= '0'
					   && C <= '9')
				      Function = Function * 10 + C - '0';
				  }
				Map_Pointer->Scancode = 0;
				Map_Pointer->Char = Key;
				Map_Pointer->Flags = Flags + Key_Code;
				Map_Pointer->F_Code = Function;
				Map_Pointer->Index = Name_Index + 1;
				while (C == ' ' || C == 9)
				  C = Map_Getc ();
				while (C != '\n' && C != EOF)
				  {
				    Names[Name_Index++] = C;
				    C = Map_Getc ();
				  }
				Names[Name_Index++] = 0;
			      }
			    else
			      {
				Error ("Error in function key on line %d",
				       Line);
				while (C != '\n' && C != EOF)
				  C = Map_Getc ();
			      }
			  }
			else
			  {
			    Error ("Error in translation type on line %d",
				   Line);
			    while (C != '\n' && C != EOF)
			      C = Map_Getc ();
			  }
		      }
		  }
	      }
	  }
	}
    }
  else
    Error ("Unknown keyword on line %d", Line);
}

int
Map_Getc ()
{
  int C;
  char *P;
  while (1)
    {
      if (File_Debth == 0)
	{
	  C = fgetc (F);
	  if (LF_Last && C == '@')
	    {
	      P = File_Name;
	      while ((C = fgetc (F)) != '\n' && C != EOF)
		*(P++) = C;
	      Line++;
	      *P = 0;
	      if (File_Debth == 10)
		Error ("Too many levels of indirect files");
	      else
		{
		  if ((Ind_File[File_Debth] = fopen (File_Name, "r")) == NULL)
		    Error ("Indirect file not found: %s", File_Name);
		  else
		    {
		      File_Debth++;
		      Save_Line[File_Debth - 1] = Line;
		      Line = 1;
		    }
		}
	    }
	  else if (LF_Last && C == ';')
	    {
	      while ((C = fgetc (F)) != '\n' && C != EOF)
		;
	      Line++;
	    }
	  else
	    {
         LF_Last = (C == '\n');
	      if (LF_Last)
		Line++;
	      return C;
	    }
	}
      else if ((C = fgetc (Ind_File[File_Debth - 1])) != EOF)
	{
	  if (LF_Last && C == '@')
	    {
	      P = File_Name;
	      while ((C = fgetc (Ind_File[File_Debth - 1])) != '\n'
		     && C != EOF)
		*(P++) = C;
	      Line++;
	      *P = 0;
	      if (File_Debth == 10)
		Error ("Too many levels of indirect files");
	      else
		{
		  if ((Ind_File[File_Debth] = fopen (File_Name, "r")) == NULL)
		    Error ("Indirect file not found: %s", File_Name);
		  else
		    {
		      File_Debth++;
		      Save_Line[File_Debth - 1] = Line;
		      Line = 1;
		    }
		}
	    }
	  else if (LF_Last && C == ';')
	    {
	      while ((C = fgetc (Ind_File[File_Debth - 1])) != '\n'
		     && C != EOF)
		;
	      Line++;
	    }
	  else
	    {
	      LF_Last = C == '\n';
	      if (LF_Last )
		Line++;
	      return C;
	    }
	}
      else
	{
	  Line = Save_Line[File_Debth - 1];
	  fclose (Ind_File[--File_Debth]);
	}
    }
}

Read_Type ()
{
  while (C == ' ' || C == 9)
    C = Map_Getc ();
  if (C == 'E' || C == 'e')
    {
      if ((C = Map_Getc ()) < '0' || C > '9')
	{
	  Error ("Error in scan code on line %d", Line);
	  return 0;
	}
      else
	{
	  Scancode = C - '0';
	  while ((C = Map_Getc ()) >= '0' && C <= '9')
	    Scancode = Scancode * 10 + C - '0';
	  while (C == ' ' || C == 9)
	    C = Map_Getc ();
	  if (C == '\\')
	    {
	      if ((C = Map_Getc ()) <= ' ' || C >= 127)
		{
		  Error ("Bad character literal on line %d", Line);
		  return 0;
		}
	      Character = C;
	      C = Map_Getc ();
	      return 'E';
	    }
	  else if (C < '0' || C > '9')
	    {
	      Error ("Error in character value on line %d", Line);
	      return 0;
	    }
	  else
	    {
	      Character = C - '0';
	      while ((C = Map_Getc ()) >= '0' && C <= '9')
		Character = Character * 10 + C - '0';
	      return 'E';
	    }
	}
    }
  else if (C == 'W' || C == 'w')
    {
      if ((C = Map_Getc ()) < '0' || C > '9')
	{
	  Error ("Error in windows key code on line %d", Line);
	  return 0;
	}
      else
	{
	  Scancode = C - '0';
	  while ((C = Map_Getc ()) >= '0' && C <= '9')
	    Scancode = Scancode * 10 + C - '0';
	  while (C == ' ' || C == 9)
	    C = Map_Getc ();
	  if (C == '\\')
	    {
	      if ((C = Map_Getc ()) <= ' ' || C >= 127)
		{
		  Error ("Bad character literal on line %d", Line);
		  return 0;
		}
	      Character = C;
	      C = Map_Getc ();
	      return 'W';
	    }
	  else if (C < '0' || C > '9')
	    {
	      Error ("Error in character value on line %d", Line);
	      return 0;
	    }
	  else
	    {
	      Character = C - '0';
	      while ((C = Map_Getc ()) >= '0' && C <= '9')
		Character = Character * 10 + C - '0';
	      return 'W';
	    }
	}
    }
  else if (C == 'S' || C == 's')
    {
      if ((C = Map_Getc ()) < '0' || C > '9')
	{
	  Error ("Error in scan code on line %d", Line);
	  return 0;
	}
      else
	{
	  Scancode = C - '0';
	  while ((C = Map_Getc ()) >= '0' && C <= '9')
	    Scancode = Scancode * 10 + C - '0';
	  while (C == ' ' || C == 9)
	    C = Map_Getc ();
	  if (C == '\\')
	    {
	      if ((C = Map_Getc ()) <= ' ' || C >= 127)
		{
		  Error ("Bad character literal on line %d", Line);
		  return 0;
		}
	      Character = C;
	      C = Map_Getc ();
	      return 'S';
	    }
	  else if (C < '0' || C > '9')
	    {
	      Error ("Error in character value on line %d", Line);
	      return 0;
	    }
	  else
	    {
	      Character = C - '0';
	      while ((C = Map_Getc ()) >= '0' && C <= '9')
		Character = Character * 10 + C - '0';
	      return 'S';
	    }
	}
    }
  else if (C == 'A' || C == 'a')
    {
      if ((C = Map_Getc ()) == '\\')
	{
	  if ((C = Map_Getc ()) <= ' ' || C >= 127)
	    {
	      Error ("Bad character literal on line %d", Line);
	      return 0;
	    }
	  Character = C;
	  C = Map_Getc ();
	  return 'A';
	}
      else if (C < '0' || C > '9')
	{
	  Error ("Error in character value on line %d", Line);
	  return 0;
	}
      else
	{
	  Character = C - '0';
	  while ((C = Map_Getc ()) >= '0' && C <= '9')
	    Character = Character * 10 + C - '0';
	  return 'A';
	}
    }
  else if (C == 'K' || C == 'k')
    {
      if ((C = Map_Getc ()) < '0' || C > '9')
	{
	  Error ("Error in key value on line %d", Line);
	  return 0;
	}
      else
	{
	  Key = C - '0';
	  while ((C = Map_Getc ()) >= '0' && C <= '9')
	    Key = Key * 10 + C - '0';
	  return 'K';
	}
    }
  else
    {
      Error ("Error in translation type on line %d", Line);
      return 0;
    }
}

Error (S, P1, P2, P3, P4, P5, P6, P7, P8, P9)
     char *S;
     int P1, P2, P3, P4, P5, P6, P7, P8, P9;
{
  printf (S, P1, P2, P3, P4, P5, P6, P7, P8, P9);
  if (File_Debth != 0)
    printf (" in file %s", File_Name);
  printf ("\n");
  return 0;
}

Getkey ()
{
#if defined(DOS) | defined(NT)
  Character = GETSCAN ((int far *) &Scancode);
#endif
#if defined(VMS)
  Character = Conin ();
  Scancode = -1;
#endif
#if defined(UNIX)
#if defined(TERMCAP)
  Character = UGetrm ();
#else
  refresh ();
  Character = getch ();
#endif
  Scancode = -1;
#endif
#if defined(OS2)
  struct _KBDKEYINFO Key_Data;
  struct _NOPTRRECT No_Pointer;
  do
    {
      KbdCharIn ((PKBDKEYINFO) & Key_Data, 0, 0);
      Character = Key_Data.chChar;
      Scancode = Key_Data.chScan;
      if (Scancode == 0x45)
	{
	  Kbd_Status.cb = 10;
	  KbdGetStatus ((PKBDINFO) & Kbd_Status, 0);
	  Kbd_Status.fsMask = 1 << 4;
	  Kbd_Status.fsState |= (1 << 5);
	  KbdSetStatus ((PKBDINFO) & Kbd_Status, 0);
	}
      else if (Scancode == 0x46)
	{
	  Kbd_Status.cb = 10;
	  KbdGetStatus ((PKBDINFO) & Kbd_Status, 0);
	  Kbd_Status.fsMask = 1 << 4;
	  Kbd_Status.fsState &= ~(1 << 4);
	  KbdSetStatus ((PKBDINFO) & Kbd_Status, 0);
	}
    }
  while (Scancode == 0);
#endif
#if defined(ATARI)
  Con_Input = Crawcin ();
  Character = Con_Input & 0xFF;
  Scancode = (Con_Input >> 16) & 0xFF;
#endif
#if defined(MPE)
  {
    static char data[10];
    static int len, index;
    int dummy;
    while (index == len)
      {
	len = FREAD (mpe_stdin, (unsigned short *) data, -1);
	index = 0;
      }
    Character = data[index++];
  }
#endif
#if !(defined(UNIX) | defined(VMS))
  if (Scancode == 0)
    {
      Enhanced = 1;
      Scancode = Character;
      printf ("Enhanced key:Scancode=%d\n", Scancode);
    }
  else
#endif
    {
      Enhanced = 0;
#if defined(UNIX)
      printf ("Normal key:Character=%d\n\r", Character);
#else
      printf ("Normal key:Scancode=%d,Character=%d\n", Scancode, Character);
#endif
    }
  Scan_Number = -1;
  if (Scancode == Character)
    {
      for (I = 0; I < Scan_Defs; I++)
	{
	  if (Scan_Tab[I].Scancode == Scancode)
	    {
	      if (Enhanced && (Scan_Tab[I].Flags & Ext_Code))
		{
		  Character = Scan_Tab[I].Char;
		  printf ("Character from table=%d\n", Character);
		  break;
		}
	      else if (!Enhanced && !(Scan_Tab[I].Flags & Ext_Code))
		{
		  Character = Scan_Tab[I].Char;
		  printf ("Character from table=%d\n", Character);
		  break;
		}
	    }
	}
    }
  Key_Number = -1;
  for (I = 0; I < Key_Defs; I++)
    {
      if (Key_Tab[I].Scancode == Scancode && Key_Tab[I].Char == Character)
	{
	  if (Enhanced && (Key_Tab[I].Flags & Ext_Code))
	    {
	      Key_Number = Key_Tab[I].Key;
	      printf ("Key=%d\n", Key_Number);
	      break;
	    }
	  else if (!Enhanced && !(Key_Tab[I].Flags & Ext_Code))
	    {
	      Key_Number = Key_Tab[I].Key;
	      printf ("Key=%d\n", Key_Number);
	      break;
	    }
	}
      else if (Key_Tab[I].Char == Character && (Key_Tab[I].Flags & Char_Code))
	{
	  Key_Number = Key_Tab[I].Key;
	  printf ("Key=%d\n", Key_Number);
	  break;
	}
    }
   return 0;
}

int
Check_Comb (Index)
{
  Comb_Level++;
  Getkey ();
  for (; Index < Comb_Defs; Index++)
    {
      if (Comb_Tab[Index].Level < Comb_Level)
	return 0;
      if (Comb_Tab[Index].Level == Comb_Level)
	{
	  if (Comb_Tab[Index].Scancode == Scancode
	      && Comb_Tab[Index].Char == Character)
	    {
	      if (Enhanced && (Map_Tab[Index].Flags & Ext_Code))
		{
		  if (Comb_Tab[Index].Flags & Final_Code)
		    {
		      Key_Number = Comb_Tab[Index].Key;
#if defined(UNIX)
		      printf ("Final character in combination:Key=%d\n\r",
			      Key_Number);
#else
		      printf ("Final character in combination:Key=%d\n",
			      Key_Number);
#endif
		      return 1;
		    }
		  else
#if defined(UNIX)
		    {
		      printf ("Character combination:Count=%d\n\r",
			      Comb_Level);
#else
		    {
		      printf ("Character combination:Count=%d\n", Comb_Level);
#endif
		      return Check_Comb (Index + 1);
		    }
		}
	      else if (!Enhanced && !(Map_Tab[Index].Flags & Ext_Code))
		{
		  if (Comb_Tab[Index].Flags & Final_Code)
		    {
		      Key_Number = Comb_Tab[Index].Key;
#if defined(UNIX)
		      printf ("Final character in combination:Key=%d\n\r",
			      Key_Number);
#else
		      printf ("Final character in combination:Key=%d\n",
			      Key_Number);
#endif
		      return 1;
		    }
		  else
#if defined(UNIX)
		    {
		      printf ("Character combination:Count=%d\n\r",
			      Comb_Level);
#else
		    {
		      printf ("Character combination:Count=%d\n", Comb_Level);
#endif
		      return Check_Comb (Index + 1);
		    }
		}
	    }
	  else
	    if (Comb_Tab[Index].Char == Character
		&& (Comb_Tab[Index].Flags & Char_Code))
	    {
	      if (Comb_Tab[Index].Flags & Final_Code)
		{
		  Key_Number = Comb_Tab[Index].Key;
#if defined(UNIX)
		  printf ("Final character in combination:Key=%d\n\r",
			  Key_Number);
#else
		  printf ("Final character in combination:Key=%d\n",
			  Key_Number);
#endif
		  return 1;
		}
	      else
#if defined(UNIX)
		{
		  printf ("Character combination:Count=%d\n\r", Comb_Level);
#else
		{
		  printf ("Character combination:Count=%d\n", Comb_Level);
#endif
		  return Check_Comb (Index + 1);
		}
	    }
	  else
	    if (Map_Tab[Index].Char == Key_Number
		&& (Map_Tab[Index].Flags & Key_Code))
	    {
	      if (Comb_Tab[Index].Flags & Final_Code)
		{
		  Key_Number = Comb_Tab[Index].Key;
#if defined(UNIX)
		  printf ("Final character in combination:Key=%d\n\r",
			  Key_Number);
#else
		  printf ("Final character in combination:Key=%d\n",
			  Key_Number);
#endif
		  return 1;
		}
	      else
#if defined(UNIX)
		{
		  printf ("Character combination:Count=%d\n\r", Comb_Level);
#else
		{
		  printf ("Character combination:Count=%d\n", Comb_Level);
#endif
		  return Check_Comb (Index + 1);
		}
	    }
	}
    }
  return 0;
}

#if defined(OS2)
void
Mon_Thread ()
{
  unsigned Key_Count;
  int I;
  while (1)
    {
      Key_Count = sizeof (Key_Buff);
      DosMonRead ((PBYTE) & InBuff, 0, (PBYTE) & Key_Buff,
		  (PUSHORT) & Key_Count);
      if (Key_Buff.ddflags & 0x37)
	{
	  if (Key_Buff.cp.scan == 0)
	    {
	      if (Key_Buff.mnflags == 17792)
		{
		  Key_Buff.ddflags &= 0x7740;
		  Key_Buff.cp.scan = 0x45;
		}
	      else if (Key_Buff.mnflags == 18048)
		{
		  Key_Buff.ddflags &= 0x7740;
		  Key_Buff.cp.scan = 0x46;
		}
	    }
	}
      for (I = 0; I < Scan_Defs; I++)
	{
	  if (Key_Buff.cp.scan == Scan_Tab[I].Scancode)
	    {
	      Key_Buff.ddflags &= 0x7740;
	      Key_Buff.cp.asc = Scan_Tab[I].Char;
	    }
	}
      DosMonWrite ((PBYTE) & OutBuff, (PBYTE) & Key_Buff, Key_Count);
    }
}

OURKBD ()
{
  DosGetInfoSeg ((PSEL) & Gdt_Descriptor, (PSEL) & Ldt_Descriptor);
  Gdt = MAKEPGINFOSEG (Gdt_Descriptor);
  Ldt = MAKEPLINFOSEG (Ldt_Descriptor);
  DosMonOpen ("KBD$", (PHMONITOR) & KBD_Handle);
  InBuff.bufflen = 64;
  OutBuff.bufflen = 64;
  DosMonReg (KBD_Handle, (PBYTE) & InBuff, (PBYTE) & OutBuff, 0,
	     Gdt->sgCurrent);
  DosCreateThread ((void (far *) (void)) Mon_Thread, (PTID) & Thread_ID,
		   (PBYTE) Thread_Stack + 2048);
}

KEYREL ()
{
}
#endif

#if defined(VMS)
OURKBD ()
{
  T$Init ();
}

KEYREL ()
{
  T$Cancel ();
}
#endif

#if defined(UNIX)
OURKBD ()
{
#if defined(TERMCAP)
  UInit ();
#else
  initscr ();
  nonl ();
  cbreak ();
  noecho ();
#endif
}

KEYREL ()
{
#if defined(TERMCAP)
  UEnd ();
#else
  refresh ();
  endwin ();
#endif
}
#endif

#if defined(ATARI)
OURKBD ()
{
}

KEYREL ()
{
}
#endif

#if defined(WIN32)
void OURKBD ()
{
}

void KEYREL ()
{
}
#endif

#if defined(MPE)
OURKBD ()
{
  int dummy;
  mpe_stdin = FOPEN ("$STDIN");
  FCONTROL (mpe_stdin, 27, &dummy);
  FCONTROL (mpe_stdin, 13, &dummy);
  FSETMODE (mpe_stdin, 4);
  mpe_stdout = FOPEN ("$STDLIST");
  FWRITE (mpe_stdout, "\033&s1A", 5, 0);
}

KEYREL ()
{
  int dummy;
  FWRITE (mpe_stdout, "\033&s0A", 5, 0);
  FCONTROL (mpe_stdin, 26, &dummy);
  FCONTROL (mpe_stdin, 12, &dummy);
  FCLOSE (mpe_stdin, 0, 0);
  FCLOSE (mpe_stdout, 0, 0);
}
#endif

Scan ()
{
#if defined(OS2)
  struct _KBDKEYINFO Key_Data;
  struct _NOPTRRECT No_Pointer;
#endif
#if defined(UNIX) | defined(VMS)
  printf ("Scan not a valid command in this system\n");
#else
  for (I = 0; I < 256; I++)
    {
      Scan_Pointer = Scan_Tab + I;
      Scan_Pointer->Scancode = I + 1;
      Scan_Pointer->Flags = 0;
    }
  for (I = 0; I < 256; I++)
    {
      Scan_Pointer = Scan_Tab + I + 255;
      Scan_Pointer->Scancode = I + 1;
      Scan_Pointer->Flags = Ext_Code;
    }
  Scan_Defs = I + 256;
  OURKBD ();
  do
    {
#if defined(DOS) | defined(NT)
      C = GETSCAN ((int far *) &Scancode);
#endif
#if defined(VMS)
      C = Conin ();
      Scancode = -1;
#endif
#if defined(UNIX)
#if defined(TERMCAP)
      C = UGetrm ();
#else
      refresh ();
      C = getch ();
#endif
      Scancode = -1;
#endif
#if defined(ATARI)
      Con_Input = Crawcin ();
      C = Con_Input & 0xFF;
      Scancode = (Con_Input >> 16) & 0xFF;
#endif
#if defined(MPE)
      {
	static char data[10];
	static int len, index;
	int dummy;
	while (index == len)
	  {
	    len = FREAD (mpe_stdin, (unsigned short *) data, -1);
	    index = 0;
	  }
	C = data[index++];
      }
#endif
#if defined(OS2)
      do
	{
	  KbdCharIn ((PKBDKEYINFO) & Key_Data, 0, 0);
	  C = Key_Data.chChar;
	  Scancode = Key_Data.chScan;
	  if (Scancode == 0x45)
	    {
	      Kbd_Status.cb = 10;
	      KbdGetStatus ((PKBDINFO) & Kbd_Status, 0);
	      Kbd_Status.fsMask = 1 << 4;
	      Kbd_Status.fsState |= (1 << 5);
	      KbdSetStatus ((PKBDINFO) & Kbd_Status, 0);
	    }
	  else if (Scancode == 0x46)
	    {
	      Kbd_Status.cb = 10;
	      KbdGetStatus ((PKBDINFO) & Kbd_Status, 0);
	      Kbd_Status.fsMask = 1 << 4;
	      Kbd_Status.fsState &= ~(1 << 4);
	      KbdSetStatus ((PKBDINFO) & Kbd_Status, 0);
	    }
	}
      while (Scancode == 0);
#endif
#if defined(DOS) | defined(NT)
      if (C && C == Scancode)
	printf ("Normal key:Scancode=%d\n", Scancode);
      else if (Scancode == 0)
	{
	  printf ("Enhanced key:Scancode=%d\n", C);
	}
      else
	printf ("Bug:Scancode=%d,Character=%d\n", Scancode, C);
#endif
#if defined(OS2) | defined(ATARI) | defined(UNIX) | defined(MPE)
      printf ("Scancode=%d\n", Scancode);
#endif
    }
  while (Scancode != 1);
  KEYREL ();
#endif
return 0;
}

Char ()
{
#if defined(OS2)
  struct _KBDKEYINFO Key_Data;
  struct _NOPTRRECT No_Pointer;
#endif
#if defined(UNIX)
#if defined(TERMCAP)
  UInit ();
#else
  initscr ();
  nonl ();
  cbreak ();
  noecho ();
#endif
#endif
#if defined(VMS)
  T$Init ();
#endif
#if defined(MPE)
  OURKBD ();
#endif
  Scan_Tab[0].Scancode = 0;
  do
    {
#if defined(DOS) | defined(NT)
      C = GETSCAN ((int far *) &Scancode);
#endif
#if defined(VMS)
      C = Conin ();
      Scancode = -1;
#endif
#if defined(UNIX)
#if defined(TERMCAP)
      C = UGetrm ();
#else
      refresh ();
      C = getch ();
#endif
      Scancode = -1;
#endif
#if defined(OS2)
      do
	{
	  KbdCharIn ((PKBDKEYINFO) & Key_Data, 0, 0);
	  C = Key_Data.chChar;
	  Scancode = Key_Data.chScan;
	  if (Scancode == 0x45)
	    {
	      Kbd_Status.cb = 10;
	      KbdGetStatus ((PKBDINFO) & Kbd_Status, 0);
	      Kbd_Status.fsMask = 1 << 4;
	      Kbd_Status.fsState |= (1 << 5);
	      KbdSetStatus ((PKBDINFO) & Kbd_Status, 0);
	    }
	  else if (Scancode == 0x46)
	    {
	      Kbd_Status.cb = 10;
	      KbdGetStatus ((PKBDINFO) & Kbd_Status, 0);
	      Kbd_Status.fsMask = 1 << 4;
	      Kbd_Status.fsState &= ~(1 << 4);
	      KbdSetStatus ((PKBDINFO) & Kbd_Status, 0);
	    }
	}
      while (Scancode == 0);
#endif
#if defined(ATARI)
      Con_Input = Crawcin ();
      C = Con_Input & 0xFF;
      Scancode = (Con_Input >> 16) & 0xFF;
#endif
#if defined(MPE)
      {
	static char data[10];
	static int len, index;
	int dummy;
	while (index == len)
	  {
	    len = FREAD (mpe_stdin, (unsigned short *) data, -1);
	    index = 0;
	  }
	C = data[index++];
      }
#endif
#if defined(UNIX) | defined(MPE) | defined(VMS)
      printf ("Character=%d\n\r", C);
#if defined(MPE)
      FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
    }
  while (C != 26);
#else
      printf ("Scancode=%d,Character=%d\n", Scancode, C);
    }
  while (Scancode != 1);
#endif
#if defined(UNIX)
#if defined(TERMCAP)
  UEnd ();
#else
  refresh ();
  endwin ();
#endif
#endif
#if defined(VMS)
  T$Cancel ();
#endif
#if defined(MPE)
  KEYREL ();
#endif
return 0;
}

Get_Character ()
{
#if defined(DOS) | defined(NT)
  Character = GETSCAN ((int far *) &Scancode);
#endif
#if defined(VMS)
  Character = Conin ();
  Scancode = -1;
#endif
#if defined(UNIX)
#if defined(TERMCAP)
  fflush (stdout);
  Character = UGetrm ();
#else
  refresh ();
  Character = getch ();
#endif
  Scancode = -1;
#endif
#if defined(OS2)
  struct _KBDKEYINFO Key_Data;
  struct _NOPTRRECT No_Pointer;
  do
    {
      KbdCharIn ((PKBDKEYINFO) & Key_Data, 0, 0);
      Character = Key_Data.chChar;
      Scancode = Key_Data.chScan;
      if (Scancode == 0x45)
	{
	  Kbd_Status.cb = 10;
	  KbdGetStatus ((PKBDINFO) & Kbd_Status, 0);
	  Kbd_Status.fsMask = 1 << 4;
	  Kbd_Status.fsState |= (1 << 5);
	  KbdSetStatus ((PKBDINFO) & Kbd_Status, 0);
	}
      else if (Scancode == 0x46)
	{
	  Kbd_Status.cb = 10;
	  KbdGetStatus ((PKBDINFO) & Kbd_Status, 0);
	  Kbd_Status.fsMask = 1 << 4;
	  Kbd_Status.fsState &= ~(1 << 4);
	  KbdSetStatus ((PKBDINFO) & Kbd_Status, 0);
	}
    }
  while (Scancode == 0);
#endif
#if defined(ATARI)
  Con_Input = Crawcin ();
  Character = Con_Input & 0xFF;
  Scancode = (Con_Input >> 16) & 0xFF;
#endif
#if defined(MPE)
  {
    static char data[10];
    static int len, index;
    int dummy;
    while (index == len)
      {
	len = FREAD (mpe_stdin, (unsigned short *) data, -1);
	index = 0;
      }
    Character = data[index++];
  }
#endif
return 0;
}

void Write_Maps (F1)
     char *F1;
{
  FILE *F;
  if ((F = fopen (F1, "w")) == NULL)
    {
      printf ("Error creating map file\n");
      return;
    }
  fprintf (F, "INITIALIZE\n");
  fprintf (F, "%s\n", Init_String);
  fprintf (F, "RESET\n");
  fprintf (F, "%s\n", Reset_String);
  fprintf (F, "SPECIAL\n");
  for (I = 0; I < Scan_Defs; I++)
    {
      if (Scan_Tab[I].Char > ' ' && Scan_Tab[I].Char < 127)
	{
	  if (Scan_Tab[I].Flags & Ext_Code)
	    fprintf (F, "E%d \\%c\n", Scan_Tab[I].Scancode, Scan_Tab[I].Char);
	  else if (Scan_Tab[I].Flags & Window_Code)
	    fprintf (F, "W%d \\%c\n", Scan_Tab[I].Scancode, Scan_Tab[I].Char);
	  else
	    fprintf (F, "S%d \\%c\n", Scan_Tab[I].Scancode, Scan_Tab[I].Char);
	}
      else
	{
	  if (Scan_Tab[I].Flags & Ext_Code)
	    fprintf (F, "E%d %d\n", Scan_Tab[I].Scancode, Scan_Tab[I].Char);
	  else if (Scan_Tab[I].Flags & Window_Code)
	    fprintf (F, "W%d %d\n", Scan_Tab[I].Scancode, Scan_Tab[I].Char);
	  else
	    fprintf (F, "S%d %d\n", Scan_Tab[I].Scancode, Scan_Tab[I].Char);
	}
    }
  fprintf (F, "KEYS\n");
  for (I = 0; I < Key_Defs; I++)
    {
      if (Key_Tab[I].Char > ' ' && Key_Tab[I].Char < 127)
	{
	  if (Key_Tab[I].Flags & Char_Code)
	    fprintf (F, "A\\%c", Key_Tab[I].Char);
	  else if (Key_Tab[I].Flags & Ext_Code)
	    fprintf (F, "E%d \\%c", Key_Tab[I].Scancode, Key_Tab[I].Char);
	  else
	    fprintf (F, "S%d \\%c", Key_Tab[I].Scancode, Key_Tab[I].Char);
	  fprintf (F, " %d %s\n", Key_Tab[I].Key,
		   &Names[Key_Tab[I].Index - 1]);
	}
      else
	{
	  if (Key_Tab[I].Flags & Char_Code)
	    fprintf (F, "A%d", Key_Tab[I].Char);
	  else if (Key_Tab[I].Flags & Ext_Code)
	    fprintf (F, "E%d %d", Key_Tab[I].Scancode, Key_Tab[I].Char);
	  else
	    fprintf (F, "S%d %d", Key_Tab[I].Scancode, Key_Tab[I].Char);
	  fprintf (F, " %d %s\n", Key_Tab[I].Key,
		   &Names[Key_Tab[I].Index - 1]);
	}
    }
  fprintf (F, "COMBINATIONS\n");
  for (I = 0; I < Comb_Defs; I++)
    {
      if (Comb_Tab[I].Char > ' ' && Comb_Tab[I].Char < 127)
	{
	  if (Comb_Tab[I].Flags & Key_Code)
	    fprintf (F, "%d K%d", Comb_Tab[I].Level, Comb_Tab[I].Char);
	  else if (Comb_Tab[I].Flags & Char_Code)
	    fprintf (F, "%d A\\%c", Comb_Tab[I].Level, Comb_Tab[I].Char);
	  else if (Comb_Tab[I].Flags % Ext_Code)
	    fprintf (F, "%d E%d \\%c", Comb_Tab[I].Level,
		     Comb_Tab[I].Scancode, Comb_Tab[I].Char);
	  else
	    fprintf (F, "%d S%d \\%c", Comb_Tab[I].Level,
		     Comb_Tab[I].Scancode, Comb_Tab[I].Char);
	}
      else
	{
	  if (Comb_Tab[I].Flags & Key_Code)
	    fprintf (F, "%d K%d", Comb_Tab[I].Level, Comb_Tab[I].Char);
	  else if (Comb_Tab[I].Flags & Char_Code)
	    fprintf (F, "%d A%d", Comb_Tab[I].Level, Comb_Tab[I].Char);
	  else if (Comb_Tab[I].Flags % Ext_Code)
	    fprintf (F, "%d E%d %d", Comb_Tab[I].Level, Comb_Tab[I].Scancode,
		     Comb_Tab[I].Char);
	  else
	    fprintf (F, "%d S%d %d", Comb_Tab[I].Level, Comb_Tab[I].Scancode,
		     Comb_Tab[I].Char);
	}
      if (Comb_Tab[I].Flags & Final_Code)
	fprintf (F, " %d '%s", Comb_Tab[I].Key,
		 &Names[Comb_Tab[I].Index - 1]);
      else if (Comb_Tab[I].Index != 0)
	if (Comb_Tab[I].Flags & Help_Code)
	  fprintf (F, " \"%s", &Names[Comb_Tab[I].Index - 1]);
	else
	  fprintf (F, " '%s", &Names[Comb_Tab[I].Index - 1]);
      fprintf (F, "\n");
    }
  fprintf (F, "MAPS\n");
  for (I = 0; I < Map_Defs; I++)
    {
      if (Map_Tab[I].Char > ' ' && Map_Tab[I].Char < 127)
	{
	  fprintf (F, "%d ", I);
	  if (Map_Tab[I].Flags & Gold_Code)
	    fprintf (F, "G ");
	  if (Map_Tab[I].Flags & Key_Code)
	    fprintf (F, "K%d", Map_Tab[I].Char);
	  else if (Map_Tab[I].Flags & Char_Code)
	    fprintf (F, "A\\%c", Map_Tab[I].Char);
	  else if (Map_Tab[I].Flags % Ext_Code)
	    fprintf (F, "E%d \\%c", Map_Tab[I].Scancode, Map_Tab[I].Char);
	  else
	    fprintf (F, "S%d \\%c", Map_Tab[I].Scancode, Map_Tab[I].Char);
	  if (Map_Tab[I].Index != 0)
	    {
	      if (Map_Tab[I].F_Code != 0)
		fprintf (F, " %d %s\n", Map_Tab[I].F_Code,
			 &Names[Map_Tab[I].Index - 1]);
	      else
		fprintf (F, " G %s\n", &Names[Map_Tab[I].Index - 1]);
	    }
	  else
	    {
	      if (Map_Tab[I].F_Code != 0)
		fprintf (F, " %d\n", Map_Tab[I].F_Code);
	      else
		fprintf (F, " G\n");
	    }
	}
      else
	{
	  fprintf (F, "%d ", I);
	  if (Map_Tab[I].Flags & Gold_Code)
	    fprintf (F, "G ");
	  if (Map_Tab[I].Flags & Key_Code)
	    fprintf (F, "K%d", Map_Tab[I].Char);
	  else if (Map_Tab[I].Flags & Char_Code)
	    fprintf (F, "A%d", Map_Tab[I].Char);
	  else if (Map_Tab[I].Flags % Ext_Code)
	    fprintf (F, "E%d %d", Map_Tab[I].Scancode, Map_Tab[I].Char);
	  else
	    fprintf (F, "S%d %d", Map_Tab[I].Scancode, Map_Tab[I].Char);
	  if (Map_Tab[I].Index != 0)
	    {
	      if (Map_Tab[I].F_Code != 0)
		fprintf (F, " %d %s\n", Map_Tab[I].F_Code,
			 &Names[Map_Tab[I].Index - 1]);
	      else
		fprintf (F, " G %s\n", &Names[Map_Tab[I].Index - 1]);
	    }
	  else
	    {
	      if (Map_Tab[I].F_Code != 0)
		fprintf (F, " %d\n", Map_Tab[I].F_Code);
	      else
		fprintf (F, " G\n");
	    }
	}
    }
  fclose (F);
}

Do_Edit ()
{
  int Term_Char, Term_Scan, Term_Enh, Last_Char, Last_Scan, Last_Enh, Level,
    Key, Comb_Index;
  OURKBD ();
  printf ("Enter a key to terminate input with:");
  Get_Character ();
#if UNIX
  printf ("\n\r");
#else
  printf ("\n");
#endif
  Term_Char = Character;
  Term_Scan = Scancode;
  Term_Enh = Enhanced;
  for (Key = 1; Key <= 52; Key++)
    {
      printf ("Enter sequence for key number %d (", Key);
      switch (Key)
	{
	case 1:
	  printf ("F1");
	  break;
	case 2:
	  printf ("F2");
	  break;
	case 3:
	  printf ("F3");
	  break;
	case 4:
	  printf ("F4");
	  break;
	case 5:
	  printf ("F5");
	  break;
	case 6:
	  printf ("F6");
	  break;
	case 7:
	  printf ("F7");
	  break;
	case 8:
	  printf ("F8");
	  break;
	case 9:
	  printf ("F9");
	  break;
	case 10:
	  printf ("F10");
	  break;
	case 11:
	  printf ("F11");
	  break;
	case 12:
	  printf ("F12");
	  break;
	case 13:
	  printf ("F13");
	  break;
	case 14:
	  printf ("F14");
	  break;
	case 15:
	  printf ("F15");
	  break;
	case 16:
	  printf ("F16");
	  break;
	case 17:
	  printf ("F17");
	  break;
	case 18:
	  printf ("F18");
	  break;
	case 19:
	  printf ("F19");
	  break;
	case 20:
	  printf ("F20");
	  break;
	case 21:
	  printf ("Find");
	  break;
	case 22:
	  printf ("Insert");
	  break;
	case 23:
	  printf ("Remove");
	  break;
	case 24:
	  printf ("Select");
	  break;
	case 25:
	  printf ("Prev Screen/Pg Up");
	  break;
	case 26:
	  printf ("Next Screen/Pg Dn");
	  break;
	case 27:
	  printf ("Up Arrow");
	  break;
	case 28:
	  printf ("Left Arrow");
	  break;
	case 29:
	  printf ("Down Arrow");
	  break;
	case 30:
	  printf ("Right Arrow");
	  break;
	case 31:
	  printf ("Keypad 0");
	  break;
	case 32:
	  printf ("Keypad 1");
	  break;
	case 33:
	  printf ("Keypad 2");
	  break;
	case 34:
	  printf ("Keypad 3");
	  break;
	case 35:
	  printf ("Keypad 4");
	  break;
	case 36:
	  printf ("Keypad 5");
	  break;
	case 37:
	  printf ("Keypad 6");
	  break;
	case 38:
	  printf ("Keypad 7");
	  break;
	case 39:
	  printf ("Keypad 8");
	  break;
	case 40:
	  printf ("Keypad 9");
	  break;
	case 41:
	  printf ("Keypad ,");
	  break;
	case 42:
	  printf ("Keypad -");
	  break;
	case 43:
	  printf ("Keypad .");
	  break;
	case 44:
	  printf ("PF1 - Gold in EDT/WPS");
	  break;
	case 45:
	  printf ("PF2");
	  break;
	case 46:
	  printf ("PF3");
	  break;
	case 47:
	  printf ("PF4");
	  break;
	case 48:
	  printf ("Keypad Enter");
	  break;
	case 49:
	  printf ("Backspace");
	  break;
	case 50:
	  printf ("Return");
	  break;
	case 51:
	  printf ("Home");
	  break;
	case 52:
	  printf ("End");
	  break;
	}
      printf ("):");
      Get_Character ();
      if (Character != Term_Char || Scancode != Term_Scan
	  || Enhanced != Term_Enh)
	{
	  Last_Char = Character;
	  Last_Scan = Scancode;
	  Last_Enh = Enhanced;
	  Level = 1;
	  Get_Character ();
	  if (Character == Term_Char && Scancode == Term_Scan
	      && Enhanced == Term_Enh)
	    {
	      Key_Pointer = &Key_Tab[Key_Defs];
	      Key_Pointer->Scancode = Last_Scan;
	      Key_Pointer->Char = Last_Char;
	      Key_Pointer->Key = Key;
	      Key_Pointer->Index = 0;
	      if (Last_Enh)
		Key_Pointer->Flags = Ext_Code;
	      if (Last_Scan == -1)
		Key_Pointer->Flags = Char_Code;
	      Key_Defs++;
	    }
	  else
	    {
	      Comb_Index = 0;
	      while (1)
		{
		  while (Comb_Index < Comb_Defs
			 && Comb_Tab[Comb_Index].Level >= Level
			 && (Comb_Tab[Comb_Index].Level != Level
			     || (Comb_Tab[Comb_Index].
				 Flags & Char_Code ? Last_Scan !=
				 -1 : Comb_Tab[Comb_Index].Scancode !=
				 Last_Scan)
			     || Comb_Tab[Comb_Index].Char != Last_Char
			     || (Comb_Tab[Comb_Index].
				 Flags & Ext_Code ? Last_Enh ==
				 0 : Last_Enh != 0)))
		    Comb_Index++;
		  if (Comb_Index == Comb_Defs)
		    {
		      Comb_Tab[Comb_Index].Level = Level;
		      Comb_Tab[Comb_Index].Char = Last_Char;
		      Comb_Tab[Comb_Index].Scancode = Last_Scan;
		      if (Last_Enh)
			Comb_Tab[Comb_Index].Flags = Ext_Code;
		      else if (Last_Scan == -1)
			Comb_Tab[Comb_Index].Flags = Char_Code;
		      Comb_Defs++;
		    }
		  else if (Comb_Tab[Comb_Index].Level != Level)
		    {
		      register int I;
		      for (I = Comb_Defs; I > Comb_Index; I--)
			Comb_Tab[I] = Comb_Tab[I - 1];
		      Comb_Tab[Comb_Index].Level = Level;
		      Comb_Tab[Comb_Index].Char = Last_Char;
		      Comb_Tab[Comb_Index].Scancode = Last_Scan;
		      if (Last_Enh)
			Comb_Tab[Comb_Index].Flags = Ext_Code;
		      else if (Last_Scan == -1)
			Comb_Tab[Comb_Index].Flags = Char_Code;
		      Comb_Defs++;
		    }
		  Comb_Index++;
		  Last_Char = Character;
		  Last_Scan = Scancode;
		  Last_Enh = Enhanced;
		  if (Last_Char == Term_Char && Last_Scan == Term_Scan
		      && Last_Enh == Term_Enh)
		    {
		      Comb_Tab[Comb_Index - 1].Flags |= Final_Code;
		      Comb_Tab[Comb_Index - 1].Key = Key;
		      break;
		    }
		  else
		    {
		      Get_Character ();
		      Level++;
		    }
		}
	    }
	}
#if UNIX
      printf ("\n\r");
#else
      printf ("\n");
#endif
    }
  KEYREL ();
return 0;
}

void Edit_Maps (F1, F2)
     char *F1, *F2;
{
  if ((F = fopen (F1, "r")) == NULL)
    {
      printf ("Error opening map file\n");
      return;
    }
  Readkeys ();
  fclose (F);
  Scan_Tab[Scan_Defs].Scancode = 0;
  Do_Edit ();
  Write_Maps (F2);
}

Create (F1)
     char *F1;
{
  Map_Defs = Scan_Defs = Key_Defs = Comb_Defs = 0;
  Do_Edit ();
  Write_Maps (F1);
  return 0;
}

void Test (F1)
     char *F1;
{
#if defined(MPE)
  if ((F = fopen (F1, "rTm")) == NULL)
#else
  if ((F = fopen (F1, "r")) == NULL)
#endif
    {
      printf ("Error opening map file\n");
      return;
    }
  Readkeys ();
  fclose (F);
  Scan_Tab[Scan_Defs].Scancode = 0;
  OURKBD ();
  Gold = 0;
  do
    {
      while (1)
	{
	  Comb_Level = 0;
	  if (Check_Comb (0))
	    break;
	  if (Comb_Level == 1)
	    break;
#if defined(UNIX) | defined(MPE)
	  printf ("Bad key sequence\n\r");
#if defined(MPE)
	  FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
	  printf ("Bad key sequence\n");
#endif
	}
      for (I = 0; I < Map_Defs; I++)
	{
	  if (Map_Tab[I].Flags & Gold_Code ? Gold : !Gold)
	    {
	      if (Map_Tab[I].Scancode == Scancode
		  && Map_Tab[I].Char == Character)
		{
		  if (Enhanced && (Map_Tab[I].Flags & Ext_Code))
		    {
		      if (Gold)
#if defined(UNIX) | defined(MPE)
			printf ("Gold ");
#if defined(MPE)
		      FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
			printf ("Gold ");
#endif
		      if (Map_Tab[I].F_Code == 0)
#if defined(UNIX) | defined(MPE)
			{
			  printf ("Gold key\n\r");
#if defined(MPE)
			  FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
			}
		      else
#else
			printf ("Gold key\n\r");
		      else
#endif
#if defined(UNIX) | MPE
			printf ("Function=%d\n\r", Map_Tab[I].F_Code);
#if defined(MPE)
		      FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
			printf ("Function=%d\n", Map_Tab[I].F_Code);
#endif
		      Gold = Map_Tab[I].F_Code == 0;
		      break;
		    }
		  else if (!Enhanced && !(Map_Tab[I].Flags & Ext_Code))
		    {
		      if (Gold)
#if defined(UNIX) | defined(MPE)
			printf ("Gold ");
#if defined(MPE)
		      FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
			printf ("Gold ");
#endif
		      if (Map_Tab[I].F_Code == 0)
#if defined(UNIX) | defined(MPE)
			{
			  printf ("Gold key\n\r");
#if defined(MPE)
			  FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
			}
		      else
#else
			printf ("Gold key\n\r");
		      else
#endif
#if defined(UNIX) | defined(MPE)
			printf ("Function=%d\n\r", Map_Tab[I].F_Code);
#if defined(MPE)
		      FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
			printf ("Function=%d\n", Map_Tab[I].F_Code);
#endif
		      Gold = Map_Tab[I].F_Code == 0;
		      break;
		    }
		}
	      else
		if (Map_Tab[I].Char == Character
		    && (Map_Tab[I].Flags & Char_Code))
		{
		  if (Gold)
#if defined(UNIX) | defined(MPE)
		    printf ("Gold ");
#if defined(MPE)
		  FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
		    printf ("Gold ");
#endif
		  if (Map_Tab[I].F_Code == 0)
#if defined(UNIX) | defined(MPE)
		    {
		      printf ("Gold key\n\r");
#if defined(MPE)
		      FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
		    }
		  else
#else
		    printf ("Gold key\n\r");
		  else
#endif
#if defined(UNIX) | defined(MPE)
		    printf ("Function=%d\n\r", Map_Tab[I].F_Code);
#if defined(MPE)
		  FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
		    printf ("Function=%d\n", Map_Tab[I].F_Code);
#endif
		  Gold = Map_Tab[I].F_Code == 0;
		  break;
		}
	      else
		if (Map_Tab[I].Char == Key_Number
		    && (Map_Tab[I].Flags & Key_Code))
		{
		  if (Gold)
#if defined(UNIX) | defined(MPE)
		    printf ("Gold ");
#if defined(MPE)
		  FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
		    printf ("Gold ");
#endif
		  if (Map_Tab[I].F_Code == 0)
#if defined(UNIX) | defined(MPE)
		    {
		      printf ("Gold key\n\r");
#if defined(MPE)
		      FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
		    }
		  else
#else
		    printf ("Gold key\n\r");
		  else
#endif
#if defined(UNIX) | defined(MPE)
		    printf ("Function=%d\n\r", Map_Tab[I].F_Code);
#if defined(MPE)
		  FWRITE (mpe_stdout, "\n\r", 2, 0);
#endif
#else
		    printf ("Function=%d\n", Map_Tab[I].F_Code);
#endif
		  Gold = Map_Tab[I].F_Code == 0;
		  break;
		}
	    }
	  if (I == Map_Defs - 1)
	    {
	      Gold = 0;
	    }
	}
#if defined(UNIX) | defined(VMS) | defined(MPE)
    }
  while (Character != 26);
#else
    }
  while (Scancode != 1);
#endif
  KEYREL ();
}

void Compile_Keys (F1, F2)
     char *F1, *F2;
{
  int I;
#if defined(MPE)
  if ((F = fopen (F1, "rTm")) == NULL)
#else
  if ((F = fopen (F1, "r")) == NULL)
#endif
    {
      printf ("Error opening map file\n");
      return;
    }
  Readkeys ();
  fclose (F);
#if defined(UNIX)
  if ((F = fopen (F2, "w")) == NULL)
#else
  if ((F = fopen (F2, "wb")) == NULL)
#endif
    {
      printf ("Error creating map file\n");
      return;
    }
  fwrite (&Version, sizeof (int), 1, F);
  I = strlen (Init_String);
  fwrite (&I, sizeof (int), 1, F);
  if (I)
    fwrite (Init_String, sizeof (char), I, F);
  I = strlen (Reset_String);
  fwrite (&I, sizeof (int), 1, F);
  if (I)
    fwrite (Reset_String, sizeof (char), I, F);
  fwrite (&Scan_Defs, sizeof (int), 1, F);
  fwrite (Scan_Tab, sizeof (struct Scan_Tab_Struct), Scan_Defs, F);
  fwrite (&Key_Defs, sizeof (int), 1, F);
  fwrite (Key_Tab, sizeof (struct Key_Tab_Struct), Key_Defs, F);
  fwrite (&Comb_Defs, sizeof (int), 1, F);
  fwrite (Comb_Tab, sizeof (struct Comb_Tab_Struct), Comb_Defs, F);
  fwrite (&Map_Defs, sizeof (int), 1, F);
  fwrite (Map_Tab, sizeof (struct Map_Tab_Struct), Map_Defs, F);
  fwrite (Names, 1, Name_Index, F);
  fclose (F);
}

void Dump_Keys (F1, F2)
     char *F1, *F2;
{
  int I;
#if defined(UNIX) | defined(MPE)
#if defined(MPE)
  if ((F = fopen (F1, "rTm")) == NULL)
#else
  if ((F = fopen (F1, "r")) == NULL)
#endif
#else
  if ((F = fopen (F1, "rb")) == NULL)
#endif
    {
      printf ("Error opening map file\n");
      return;
    }
  fread (&Version, sizeof (int), 1, F);
  if (Version != 3 && Version != 2)
    {
      printf ("Version incompatibility with key map file\n");
      return;
    }
  if (Version > 2)
    {
      fread (&I, sizeof (int), 1, F);
      if (I)
	{
	  fread (Init_String, sizeof (char), I, F);
	  Init_String[I] = 0;
	}
      fread (&I, sizeof (int), 1, F);
      if (I)
	{
	  fread (Reset_String, sizeof (char), I, F);
	  Reset_String[I] = 0;
	}
    }
  fread (&Scan_Defs, sizeof (int), 1, F);
  fread (Scan_Tab, sizeof (struct Scan_Tab_Struct), Scan_Defs, F);
  fread (&Key_Defs, sizeof (int), 1, F);
  fread (Key_Tab, sizeof (struct Key_Tab_Struct), Key_Defs, F);
  fread (&Comb_Defs, sizeof (int), 1, F);
  fread (Comb_Tab, sizeof (struct Comb_Tab_Struct), Comb_Defs, F);
  fread (&Map_Defs, sizeof (int), 1, F);
  fread (Map_Tab, sizeof (struct Map_Tab_Struct), Map_Defs, F);
  fread (Names, 1, 1024 * 5, F);
  fclose (F);
  Write_Maps (F2);
}

void Compile_Functions (F1, F2)
     char *F1, *F2;
{
#if defined(MPE)
  if ((F = fopen (F1, "rTm")) == NULL)
#else
  if ((F = fopen (F1, "r")) == NULL)
#endif
    {
      printf ("Error opening function file\n");
      return;
    }
  Top_Func = Func_Def_Index = Name_Index = 0;
  Line = 0;
  while (fgets (Dsk_Rec, Blksize, F) != NULL)
    {
      Line++;
      if (Dsk_Rec[0] != ';')
	{
	  P = Dsk_Rec;
	  Q = NULL;
	  while ((C = *P++) != 0)
	    {
	      if (C == ' ' && Q == NULL)
		Q = P - 1;
	      else if (C != ' ' && Q != NULL)
		Q = NULL;
	    }
	  if (Q != NULL)
	    (*Q) = 0;
	  P = Dsk_Rec;
	  while ((C = *(P++)) == ' ' || C == 9)
	    ;
	  if (isdigit (C))
	    {
	      I = C - '0';
	      while ((C = *(P++)) >= '0' && C <= '9')
		I = I * 10 + C - '0';
	      while (C == ' ' || C == 9)
		C = *(P++);
	      P--;
	      if (I >= Maxfunc)
		printf ("Bad key number on line %d\n", Line);
	      else if ((FP = Funcs[I])[1] != 0)
		printf ("Function already defined on line %d\n", Line);
	      else
		{
		  FP[0] = Func_Def_Index + 1;
		  while ((C = (*P++)) && C != '\n')
		    Func_Defs[Func_Def_Index++] = C;
		  FP[1] = Func_Def_Index + 1 - FP[0];
		  if (I > Top_Func)
		    Top_Func = I;
		}
	    }
	  else
	    printf ("Bad key number on line %d\n", Line);
	  FP[2] = Name_Index + 1;
	  while ((C = fgetc (F)) != '\n' && C != EOF)
	    Names[Name_Index++] = C;
	  Names[Name_Index++] = 0;
	  Line++;
	}
    }
  fclose (F);
  for (I = 0; I <= Top_Func; I++)
    if (Funcs[I][0] != 0)
      Funcs[I][0] += Name_Index;
#if defined(UNIX)
  if ((F = fopen (F2, "w")) == NULL)
#else
  if ((F = fopen (F2, "wb")) == NULL)
#endif
    {
      printf ("Error creating function file\n");
      return;
    }
  fwrite (&Top_Func, sizeof (int), 1, F);
  fwrite (Funcs, 3 * sizeof (long), Top_Func + 1, F);
  fwrite (Names, 1, Name_Index, F);
  fwrite (Func_Defs, 1, Func_Def_Index, F);
  fclose (F);
}

void Dump_Functions (F1, F2)
     char *F1, *F2;
{
#if defined(UNIX) | defined(MPE)
#if defined(MPE)
  if ((F = fopen (F1, "rTm")) == NULL)
#else
  if ((F = fopen (F1, "r")) == NULL)
#endif
#else
  if ((F = fopen (F1, "rb")) == NULL)
#endif
    {
      printf ("Error opening function file\n");
      return;
    }
  fread (&Top_Func, sizeof (int), 1, F);
  fread (Funcs, 3 * sizeof (long), Top_Func + 1, F);
  Name_Index = fread (Names, 1, 10 * 1024, F);
  fclose (F);
  if ((F = fopen (F2, "w")) == NULL)
    {
      printf ("Error creating function file\n");
      return;
    }
  for (I = 0; I <= Top_Func; I++)
    {
      if (Funcs[I][0] != 0)
	{
	  fprintf (F, "%d %.*s\n", I, (int) Funcs[I][1],
		   &Names[Funcs[I][0] - 1]);
	  if (Funcs[I][2] != 0)
	    fprintf (F, "%s\n", &Names[Funcs[I][2] - 1]);
	  else
	    fprintf (F, "\n");
	}
    }
  fclose (F);
}

#if defined(VMS)
Conin ()
{
  int Character;
  if (Got_In == This_In)
    {
      do
	Got_In = Get_Input ();
      while (Got_In == 0);
      This_In = 0;
    }
  Character = In_Buf[This_In++] & 255;
  Scancode = -1;
  return Character;
}

Get_Input ()
{
  return Got_In = T$Startone (In_Buf, Maxcol, "", 0);
}

PRINT_MESS ()
{
}
#endif
