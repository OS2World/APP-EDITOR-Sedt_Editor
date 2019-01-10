;Copyright (c) Anker Berg-Sonne, 1985,1986,1987,1988
	Title   SEDT Low Level Functions
	Page    66,132
	
	MODEL LARGE
;
; SEDT Low Level Functions
;
;       Version 1.0
;
;       Anker Berg-sonne        July-Nov 1984
;
;       Modified by:
;
;               Reid Brown      Nov 30, 1984    + Converted to MSDOS
;                                                 via LATTICE-C Compiler
;
;               Reid Brown      Feb 19, 1985    + Added fast video support
;
; local macros
clr     macro   reg
	xor     reg,reg
	endm

Preamble        macro
	push    bp
	mov     bp,sp
	push    si
	push    di
	IF @Model EQ 5
	push    ds
	ENDIF
	endm

Postcode        macro
	IF @Model EQ 5
	pop     ds
	ENDIF
	pop     di
	pop     si
	pop     bp
	endm
	
EXTRN   _Video_Seg:WORD
EXTRN   _Mouse_Int:FAR
EXTRN   _Crit_Int:FAR
EXTRN   _Mouse_Stack:BYTE

_BIOS_D SEGMENT AT 40H           ; BIOS data area
	ASSUME DS:_BIOS_D

	ORG     17H
KB_FLAG LABEL   BYTE
RIGHT_SHIFT     EQU     1H
LEFT_SHIFT      EQU     2H
CTL_SHIFT       EQU     4H
ALT_SHIFT       EQU     8H
SCROLL_SHIFT    EQU     10H
NUM_SHIFT       EQU     20h
CAPS_SHIFT      EQU     40H        
	ORG     63H
ADDR_6845       LABEL WORD
	ORG     97H
KB_FLAG_2       LABEL   BYTE
KB_FA   EQU     010H
KB_FE   EQU     020H
	ORG     1AH
BUFFER_HEAD     LABEL   WORD
	ORG     1CH
BUFFER_TAIL     LABEL   WORD
	ORG     49H
VIDEO_MODE      LABEL  BYTE
	ORG     4AH
CRT_COLS        LABEL   WORD
	ORG     4CH
CRT_LEN         LABEL   WORD
	ORG     80H
BUFFER_START    LABEL   WORD
	ORG     82H
BUFFER_END      LABEL   WORD
	
_BIOS_D ENDS

_BIOS_I SEGMENT AT 0H           ; BIOS interrupt vectors
	ASSUME DS:_BIOS_I
	
	ORG 24H*4
IP_24    LABEL WORD              ; IP for interrupt 24
	ORG 24H*4+2
CS_24    LABEL WORD              ; CS for interrupt 24

_BIOS_I ENDS

_TEXT   SEGMENT BYTE PUBLIC 'CODE'
	assume CS:_TEXT
	assume DS:_TEXT


S_IP_24  DW      0               ; The hardware interrupt address we replaced
S_CS_24  DW      0
Mouse_SS        DW      0
Mouse_SP        DW      0
Mouse_AX        DW      0

	PUBLIC  Mouse_SS,Mouse_SP,Mouse_AX
;
; Define Parameter offsets
;

parm1   equ     6
parm2   equ     8
parm3   equ     10
parm4   equ     12
parm5   equ     14
parm6   equ     16

	page
;
; C_Scan - Scan for a character
;
;       Scan forward in memory looking for a specific character
;
;               C_Scan_( start_addr, length, character )
;
;               (returns location of match, or length if not found)
;
;               note: high bits of match character arg must = 0
;

	public  _C_Scan

_C_Scan proc     far
	Preamble
	push    es

	IF @Model  EQ 5
	mov     ax, parm2[bp]
	ELSE
	mov     ax, ds
	ENDIF
	mov     es, ax

	IF @Model EQ 5
	mov     di, parm1[bp]
	mov     cx, parm3[bp]
	mov     ax, parm4[bp]
	ELSE
	mov     di, parm1[bp]
	mov     cx, parm2[bp]
	mov     ax, parm3[bp]
	ENDIF
	cmp     cx, 0
	jle     C_S_2           ; Exit if negative count
	sahf                    ; Reset flags to 0 using high byte
	cld
	repne   scasb           ; Look for CX count=0 or match on AL
	je      C_S_3
	IF @Model EQ 5
	mov     ax,parm3[bp]    ; Return size of string + 1 (not found)
	ELSE
	mov     ax,parm2[bp]    ; Return size of string + 1 (not found)
	ENDIF
	jmp     C_S_2

C_S_3:  inc     cx
	IF @Model EQ 5
	mov     ax,parm3[bp]    ; Subtract decremented count from full count,
	ELSE
	mov     ax,parm2[bp]    ; Subtract decremented count from full count,
	ENDIF
	sub     ax,cx           ; giving the location where char was found

C_S_2:  pop     es
	Postcode
	ret     

_C_Scan endp
	page
;
; C_Bscan - Backward Scan for character
;
;       Scan Backwards in memory for a character
;
;               C_Bscan_( start_address, length, character )
;
;               (returns location of found character in string)
;
;               note: insure high byte of character arg = 0
;                     start_address is loc to start searching bwd from
;                     length must be negative (e.g. -60)
;

	public  _C_Bscan

_C_Bscan proc    far
	Preamble
	push    es

	IF @Model EQ 5
	mov     ax, parm2[bp]
	ELSE
	mov     ax, ds
	ENDIF
	mov     es, ax

	IF @Model EQ 5
	mov     di, parm1[bp]
	mov     cx, parm3[bp]
	neg     cx
	mov     ax, parm4[bp]
	ELSE
	mov     di, parm1[bp]
	mov     cx, parm2[bp]
	neg     cx
	mov     ax, parm3[bp]
	ENDIF
	cmp     cx, 0
	jle     C_B_2
	sahf
	std
	repne   scasb
	je      C_B_3
	IF @Model EQ 5
	mov     ax,parm3[bp]
	ELSE
	mov     ax,parm2[bp]
	ENDIF
	jmp     C_B_2

C_B_3:  inc     cx
	IF @Model EQ 5
	mov     ax,parm3[bp]
	ELSE
	mov     ax,parm2[bp]
	ENDIF
	add     ax,cx

C_B_2:  cld
	pop     es
	Postcode
	ret     

_C_Bscan endp

;
; M_Alloc - Allocate memory
;
;       Allocate a block of memory (size specified in paragraphs)
;
;               ptr = _alloc( size )
;                       size (short) = requested size in paragraphs, must
;                               be 0-32767
;                       ptr  (short) = error code if negative, otherwise
;                               the segment address if positive.  Error
;                               codes are:
;                               zero no memory available
;
	public  _M_Alloc

_M_Alloc proc    far                                           
	Preamble

	mov     bx, parm1[bp]           ; Argument is size in paragraphs
	and     bx, 7fffh               ; Insure 0-32767
	mov     ah, 48h                 ; Function code = 48h
	int     21h                     ; Call MS-DOS
	jnc     _m_ret                  ; Done if no carry

	mov     ax, 0h                  ; Error, return 0

_m_ret: Postcode                      ; Restore bp
	ret     

_M_Alloc endp

;
; M_Free - Free memory
;
;
	public  _M_Free

_M_Free proc    far                                           
	Preamble

	mov     es, parm1[bp]           ; Argument is size in paragraphs
	mov     ah, 49h                 ; Function code = 48h
	int     21h                     ; Call MS-DOS
	jnc     _f_ret                  ; Done if no carry

	mov     ax, 0h                  ; Error, return 0

_f_ret: Postcode                      ; Restore bp
	ret     

_M_Free endp

	public  _IBMpage

_IBMpage      proc    far
	Preamble
		 
	mov     ah,5h
	mov     al,parm1[bp]
	int     10h
		
	Postcode
	ret
_IBMpage      endp

	public  _IBMcursor

_IBMcursor    proc far
	Preamble

	mov     ah,2h
	mov     bh,0h
	mov     dh,parm1[bp]
	mov     dl,parm2[bp]
	int     10h
		
	Postcode
	ret
_IBMcursor    endp

	public  _IBMscrup

_IBMscrup       proc far
	Preamble
		  
	mov     ah,6h
	mov     al,parm1[bp]
	mov     ch,parm2[bp]
	mov     cl,parm3[bp]
	mov     dh,parm4[bp]
	mov     dl,parm5[bp]
	mov     bh,parm6[bp]
	int     10h
		
	Postcode
	ret

_IBMscrup       endp

	public  _IBMscrdown

_IBMscrdown     proc far
	Preamble

	mov     ah,7h
	mov     al,parm1[bp]
	mov     ch,parm2[bp]
	mov     cl,parm3[bp]
	mov     dh,parm4[bp]
	mov     dl,parm5[bp]
	mov     bh,parm6[bp]
	int     10h
		
	Postcode
	ret
_IBMscrdown     endp

	public  _IBMsout

_IBMsout        proc far
	Preamble

	mov     ah,19
	mov     al,0h
	mov     bh,0h
	mov     cx,parm1[bp]
	push    ds
	pop     es
	mov     bl,parm3[bp]
	mov     dh,parm4[bp]
	mov     dl,parm5[bp]
	mov     bp,parm2[bp]
	int     10h
		
	Postcode
	ret
_IBMsout        endp

	public  _IBMcout

_IBMcout        proc far
	Preamble

	mov     ah,9
	mov     bh,0h
	mov     cx,parm1[bp]
	mov     al,parm2[bp]
	mov     bl,parm3[bp]
	int     10h
		
	Postcode
	ret
_IBMcout        endp

	public  _Set43

_Set43        proc far
	Preamble

	MOV     AX,1112H
	MOV     BL,00
	INT     10H
	PUSH    DS
	SUB     AX,AX
	MOV     DS,AX
	MOV     BP,0487H
	PUSH    DS:[BP]
	OR      BYTE PTR DS:[BP],01H
	MOV     CX,0606H
	MOV     AH,01H
	INT     10H
	POP     DS:[BP]
	POP     DS
	MOV     DX,03B4H
	MOV     AX,0714H
	OUT     DX,AX
		
	Postcode
	ret
_Set43        endp

	public  _Clr43

_Clr43        proc far
	Preamble

	MOV     AX,1111H
	MOV     BL,00
	INT     10H
	PUSH    DS
	SUB     AX,AX
	MOV     DS,AX
	MOV     BP,0487H
	PUSH    DS:[BP]
	OR      BYTE PTR DS:[BP],01
	MOV     CX,0708H
	MOV     AH,01H
	INT     10H
	POP     DS:[BP]
	POP     DS
	MOV     DX,03B4H
	MOV     AX,0D14H
	OUT     DX,AX
		
	Postcode
	ret
_Clr43        endp

	public  _Set_Mode

_Set_Mode       proc far
	Preamble

	mov     ax,040H
	mov     es,ax
	mov     bh,0
	mov     bl,es:VIDEO_MODE
	mov     ah,0
	mov     al,parm1[bp]
	int     10h
	mov     ax,bx
		
	Postcode
	ret
_Set_Mode        endp

	public  _Get_Mode

_Get_Mode       proc far
	Preamble

	mov     ax,cs
	mov     es,ax
	mov     ah,1BH
	mov     bx,0
	lea     di,video_data
	int     10h
	cmp     al,1BH
	jne     nosup
	mov     ax,word ptr es:video_data+5H
	IF @Model EQ 5
	mov     bx,parm4[bp]
	mov     ds,bx
	mov     di,parm3[bp]
	ELSE
	mov     di,parm2[bp]
	ENDIF
	mov     [di],ax
	mov     al,es:video_data+22H
	mov     ah,0
	IF @Model EQ 5
	mov     bx,parm2[bp]
	mov     ds,bx
	mov     di,parm1[bp]
	ELSE
	mov     di,parm1[bp]
	ENDIF
	mov     [di],ax
nosup:

	Postcode
	ret

video_data      LABEL BYTE
	DW      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DW      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DW      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	DW      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		
_Get_Mode        endp

	ASSUME DS:_BIOS_D
	
	public  _Dir_Out

_Dir_Out        proc far
	Preamble
	push    es
	
	mov     ax,_Video_Seg
Dir_Col:
	mov     es,ax
	mov     ax,parm1[bp]
	push    ds
	mov     bx,040H
	mov     ds,bx
	imul    CRT_COLS
;        call    V_Trace
	pop     ds
	add     ax,parm2[bp]
	shl     ax,1
	mov     di,ax
	IF @Model EQ 5
	mov     ax,parm4[bp]
	mov     ds,ax
	mov     si,parm3[bp]
	mov     cx,parm6[bp]
	mov     ah,parm5[bp]
	ELSE
	mov     si,parm3[bp]
	mov     cx,parm5[bp]
	mov     ah,parm4[bp]
	ENDIF
Dir_Loop:
	mov     al,[si]
	mov     es:[di],ax
	inc     si
	inc     di
	inc     di
	loop    Dir_Loop
	
	pop     es
	Postcode
	ret

_Dir_Out        endp

	public  _Dir_Set

_Dir_Set        proc far
	Preamble
	push    es
	
	mov     ax,_Video_Seg
Dir_Col1:
	mov     es,ax
	mov     ax,parm1[bp]
	push    ds
	mov     bx,040H
	mov     ds,bx
	imul    CRT_COLS
;        call    V_Trace
	pop     ds
	add     ax,parm2[bp]
	shl     ax,1
	mov     di,ax
	mov     al,parm3[bp]
	mov     ah,parm4[bp]
	mov     cx,parm5[bp]
Dir_Loop1:
	mov     es:[di],ax
	inc     di
	inc     di
	loop    Dir_Loop1
	
	
	pop     es
	Postcode
	ret

_Dir_Set        endp

	public  _M_Int
_M_Int      proc    far

	cli
	mov     cs:Mouse_AX,ax
	mov     ax,ss
	mov     cs:Mouse_SS,ax
	mov     ax,sp
	mov     cs:Mouse_SP,ax
	mov     ax,SEG _Video_Seg
	mov     ss,ax
	lea     ax,_Mouse_Stack
	add     ax,2048D
	mov     sp,ax
	mov     ax,cs:Mouse_AX
	sti

	Preamble
	
	push    ds
	push    es
	push    dx
	push    cx
	push    bx
	push    ax
	mov     ax,SEG _Video_Seg
	mov     ds,ax
	call    _Mouse_Int
	add     sp,8
	pop     es
	pop     ds
	
	Postcode

	cli
	mov     cs:Mouse_AX,ax
	mov     ax,cs:Mouse_SP
	mov     sp,ax
	mov     ax,cs:Mouse_SS
	mov     ss,ax
	mov     ax,cs:Mouse_AX
	sti
	
	ret
_M_Int      endp

	public  _Set_Mouse
_Set_Mouse      proc    far
	Preamble
	
	mov     ax,cs
	mov     es,ax
	lea     dx,_M_Int
	mov     ax,12
	mov     cx,1+2+4+8
	int     51
	
	Postcode
	ret
_Set_Mouse      endp

	public  _Clear_Mouse
_Clear_Mouse    proc    far
	Preamble
	
	mov     ax,cs
	mov     es,ax
	lea     dx,_M_Int
	mov     ax,12
	mov     cx,0
	int     51

	Postcode
	ret
_Clear_Mouse    endp

	public  _Set_Crit
_Set_Crit       proc    far
	Preamble
	
	CLI
	MOV     AX,SEG _BIOS_I  ; Get segment address of BIOS interrupts
	MOV     ES,AX           ; Move into ES

	MOV     AX,ES:IP_24      ; Save the interrupt 24 vector
	MOV     S_IP_24,AX       ;
	MOV     AX,ES:CS_24      ;
	MOV     S_CS_24,AX       ;
	MOV     AX,OFFSET INT24  ; Shove ours into the BIOS vector
	MOV     ES:IP_24,AX      ;
	MOV     AX,CS           ;
	MOV     ES:CS_24,AX      ;
	STI

	Postcode
	ret
_Set_Crit       endp

	public  _Reset_Crit
_Reset_Crit       proc    far
	Preamble
	
	CLI
	MOV     AX,SEG _BIOS_I  ; Get segment address of BIOS interrupts
	MOV     ES,AX           ; Move into ES

	MOV     AX,S_IP_24       ; Shove saved vector into the BIOS vector
	MOV     ES:IP_24,AX      ;
	MOV     AX,S_CS_24       ;
	MOV     ES:CS_24,AX      ;
	STI                     ; Restore interrupts now that we are done

	Postcode
	ret
_Reset_Crit       endp

	PUBLIC  INT24

INT24   PROC FAR

	push    bx
	push    cx
	push    dx
	push    si
	push    bp
	push    ds
	push    es
	push    di
	mov     ax,SEG _Video_Seg
	mov     ds,ax
	call    _Crit_Int
	pop     di
	pop     es
	pop     ds
	pop     bp
	pop     si
	pop     dx
	pop     cx
	pop     bx        

	iret
	
INT24   ENDP

_TEXT   ENDS

	end
