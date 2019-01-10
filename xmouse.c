#if defined(XTERM_MOUSE)


#include "proto.h"

static is_on;

void xmouse_on() 
{
    is_on=1;

 {Out_S("\033[?1002h\033[?9h");
  Out_Zap();
 }
} 

void xmouse_off() 
{
    is_on=0;

 Out_S("\033[?9l\033[?1002l");
 Out_Zap();
} 

void xmouse_toggle() 
{
    if (is_on) xmouse_off();
    else       xmouse_on();
} 

void  mouse_1_Down(void);
void  mouse_3_Down(void);

void xmouse_handler() 
{
    /* <ESC>[M... the ... have still to be read
       each . has a value which is 32 offset from the byte
       so . of '!' gives value of 1
        . = button -1
        . = x
        . = y

    */

    int buttons;
    int x;
    int y;

    buttons = UGetrm();
    x       = UGetrm();
    y       = UGetrm();

    buttons -= 32;
    x       -= 32;
    y       -= 32;

    Abs_Row = y-1;
    Abs_Col = x-1;

    if ( buttons == 0)      mouse_1_Down();
    if ( buttons == 2)      mouse_3_Down();
} 


void  mouse_1_Down()
{
 Ldown=1; 
 Move=Rdown=0; 
  Mouse_Call();
 Abs_Row_Sav=Abs_Row;
 Abs_Col_Sav=Abs_Col;
}

void  mouse_1_Up()
{
 Rdown=0; 
 Ldown=1; 
 {Move=Abs_Row_Sav!=Abs_Row||Abs_Col_Sav!=Abs_Col;
  if (Move)
   Mouse_Call();
 }
}

void  mouse_2_Down()
{
}

void  mouse_2_Up()
{
}

void  mouse_3_Down()
{
 Rdown=1; 
 Move=Ldown=0; 
  Mouse_Call();
}

void  mouse_3_Up()
{
 Move=Ldown=0; 
 Rdown=1; 
}

void  mouse_4_Down()
{
}

void  mouse_4_Up()
{
}

void  mouse_Outside()
{
}


#endif /* defined(XTERM_MOUSE) */

