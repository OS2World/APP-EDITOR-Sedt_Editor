/*Copyright (c) Anker Berg-Sonne, Digital Equipment Corporation, 1985*/
#if defined(Windows)
#ifndef DOS
#define DOS
#endif
#endif

#if defined(GEM)
#define ATARI
#endif

#if defined(XENIX)
#define UNIX
#endif

#if defined(ULTRIX)
#define UNIX
#endif

#if defined(UNIX)
#define Noasm
#endif

#if defined(VMS) | defined(MPE)
#define Out_Size 1024
#else
#define Out_Size 128
#endif

#define Maxbuffers 100
#define Maxpoints 100
#define Search_String 0
#define Char_String 1
#define Line_String 2
#define Unknown_String 3
#define Past_String 4
#define Word_String 5
#define Subs_String 6
#define Paragraph_String 7
#define Page_String 8
#define Sen_String 9
#define Help_String 10
#define Pointers 11
#define E_Char 0
#define E_Line 1
#define E_Paste 2
#define E_Unknown 3
#define E_Word 4
#define E_Par 5
#define E_Pag 6
#define E_Sen 7
#define E_Find 8
#define E_NPaste 9

#define	C_Unknown 	0
#define C_Movecursor	1
#define C_Subs	        2
#define C_Delete	3
#define C_Center        4
#define C_Undo          5
#define C_Rep           6
#define	Change		0
#define Upper		1
#define	Lower		2
