/* gpm_mouse - add gpm support
*/

#include <stdio.h>
#include <linux/keyboard.h>

#include "proto.h"
extern int Mouse_OK;    /* vars.h but can't include */
int Mouse_OK=1;         /* KLUDGE ALERT, should be set when can use mouse!*/

#include <gpm.h>
#include "gpm_mouse.h"

static FILE* gpmin;

static int my_handler(Gpm_Event *event, void *data)
{   
    if (Mouse_OK)
    {
        Abs_Row=event->y -1;
        Abs_Col=event->x -1;

        if (Abs_Row>=0&&Abs_Row<=Phys_Length&&Abs_Col>=0&&Abs_Col<=Phys_Width)
            mouse_1_Down();
    }
    return 0;
}




int gpm_init( FILE *in)
{
    Gpm_Connect conn;

    conn.eventMask      = GPM_DOWN;
    conn.defaultMask    = ~conn.eventMask;
    conn.minMod         = 1<<KG_CTRL;
    conn.maxMod         = (~0);

/* some confusion here
    conn.maxMod     = ~conn.minMod;
    conn.maxMod     = (~0) << (KG_CTRL+1);
*/

    if(Gpm_Open(&conn, 0) == -1)
    {
        printf("Cannot connect to mouse server\n");
        return 0;
    }

    gpmin = in;
    gpm_handler = my_handler;
    return 1;
}

int gpm_fini()
{
    Gpm_Close();
    return 0;
}

int gpm_getc()
{
    return Gpm_Getc(gpmin);
}

