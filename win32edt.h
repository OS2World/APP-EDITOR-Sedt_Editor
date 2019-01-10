#ifndef win32edt_h
#define win32edt_h

extern HANDLE win32ConsoleHnd;
extern HANDLE win32KeybHnd;

void WIN32scrup( int Scroll, int top, int left, int bottom, int right, char Attab);
void WIN32scrdown( int Scroll, int top, int left, int bottom, int right, char Attab);
void WIN32cursor(int row, int col);
void WIN32scrclear(void);
void WIN32cleartoeol(int row, int col, int width);
void WIN32init(void);
void WIN32clrtypeahead(void);
void WIN32fini(void);
void WIN32attrib(int Attrib);
int  WIN32istypeahead(void);

#endif

