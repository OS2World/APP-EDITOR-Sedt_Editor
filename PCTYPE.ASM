;
; Program NETDRIVER,  Module PCTYPE.ASM - Determine PC system type
;
; Copyright (C) 1985, 1986, 1987 by
; Digital Equipment Corporation, Maynard, Mass.
;
; This software is furnished under a license and may be used and copied
; only in accordance with the terms of such license. 
;
;
; Networks & Communications Software Engineering
;
; IDENT HISTORY:
;
; V1.0  29-Jul-85
;       DECnet-DOS  V1.0
;
; V1.1  30-Mar-86
;       DECnet-DOS  V1.1 
;
; V1.2  30-Mar-87
;       DECnet-DOS  V1.2
;
;
;       05-Aug-87   DJM
;       created new module
;       15-Oct-87   DJM
;       clarify AH=C0h in code and comments
;

;	Static Name Aliases
;

_TEXT	SEGMENT  BYTE PUBLIC 'CODE'
_TEXT	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
DGROUP	GROUP	_BSS,	_DATA
	ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP


_TEXT	SEGMENT

;+
;   _pc_sys_type - Determine PC system and model type
;
;   call BIOS Int 15, AH Func=C0h to determine system & model type
;
; C call: type = int pc_sys_type();
; Inputs: none
; Outputs: 
; Returns:
;       al = IBM System type
;       ah = IBM Model type
;          = 0 if unknown
;-

                PUBLIC  _pc_sys_type
_pc_sys_type    proc far

                push    es
                push    bx
                
                mov ah, 0C0h    ; set function code for system type
                int     15h     ; Call BIOS
                                ; returns: far pointer to table in ES:BX
                jc      nogo
                mov     ax, es:[bx+2]  ; get system and model type
                jmp     exit

nogo:           xor     ax,ax
           
exit:           pop     bx
                pop     es
                ret
                
_pc_sys_type    endp



_TEXT	ENDS
	END
