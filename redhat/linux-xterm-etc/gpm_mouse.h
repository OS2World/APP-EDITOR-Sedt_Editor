/* gpm_mouse.h - add gpm support
*/

#ifndef gpm_mouse_h
#define gpm_mouse_h

int gpm_init( FILE *in);
int gpm_fini(void);
int gpm_getc(void);

#endif /* gpm_mouse_h */
