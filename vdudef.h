/*Copyright (c) Anker Berg-Sonne, Digital Equipment Corporation, 1985*/
#if defined(DOS) | defined(OS2) | defined(NT)
#if defined(Windows)
#define Maxlines 66
#define Maxcol 132
#else
#if defined(TV)
#define Maxlines 55
#define Maxcol 80
#else
#define Maxlines 60
#define Maxcol 132
#endif
#endif
#endif
#if defined(ATARI)
#if defined(GEM)
#define Maxlines 57
#define Maxcol 102
#else
#define Maxlines 25
#define Maxcol 80
#endif
#endif
#if defined(MPE)
#define Maxlines 24
#define Maxcol 80
#endif
#if defined(VMS)
#define Maxlines 80
#define Maxcol 512
#endif
#if defined(UNIX)
#define Maxlines 100
#define Maxcol 512
#endif
#if defined(WIN32)
#define Maxlines 120
#define Maxcol 512
#endif
#define Maxfunc 512
#define Maxmap 512
#define Maxscan 128
#define A_Bold 1
#define A_Blink 2
#define A_Underline 4
#define A_Reverse 8
#define Keyb_PC 1
#define Keyb_250 2
#define Keyb_New 3
#define Keyb_Conv 4
#define Keyb_TC 5
#define Keyb_AT 6
#define Keyb_SLT 7
#define Keyb_Special 8
#define Keyb_Default 9
#define VT1XX 0
#define VT2XX 1
#define VT52 2
#define VK100 3
#define PCXX 4
#define IBM_AT 5
#define IBM_XT 6
#define IBM_PC 7
#define Windev 9
#define IBM_Conv 10
#define AT_339 12
#define XT_286 13
#define PS2_50 14
#define PS2_60 15
#define PS2_30 16
#define PS2_80 17
#define XT_2 18
#define VT3XX 19
#define TANDY_1400 20
#define ANSI 21
#define Tcap 22
#define VT4XX 23
#define Default 24
#define Defined 25
#define EDT_Mapping 1
#define WPS_Mapping 2
#define Other_Mapping 3
struct Scan_Tab_Struct
 {unsigned char Scancode, Char, Flags;};
#define Ext_Code 1
#if defined(WIN95)
struct Key_Tab_Struct
 {unsigned char Scancode, Char, Flags;short Key, Index;};
struct Comb_Tab_Struct
 {unsigned char Level, Scancode, Flags; short Char, Key, Index;};
struct Map_Tab_Struct
 {unsigned char Scancode, Flags; short Char, F_Code, Index;};
#else
struct Key_Tab_Struct
 {unsigned char Scancode, Char, Flags;int Key, Index;};
struct Comb_Tab_Struct
 {unsigned char Level, Scancode, Flags; int Char, Key, Index;};
struct Map_Tab_Struct
 {unsigned char Scancode, Flags; int Char, F_Code, Index;};
#endif
#define Gold_Code 2
#define Char_Code 4
#define Key_Code 8
#define Final_Code 16
#define Window_Code 32
#define Help_Code 64
#define Prompt_Code 128
