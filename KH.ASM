Comment $
kh.asm - keyboard interrupt handler for VTxx emulation for SETHOST

 *                      Copyright (c) 1985,1986 by                      *
 *              Digital Equipment Corporation, Maynard, MA              *
 *                      All rights reserved.                            *
 *                                                                      *
 *   This software is furnished under a license and may be used and     *
 *   copied  only  in accordance with the terms of such license and     *
 *   with the  inclusion  of  the  above  copyright  notice.   This     *
 *   software  or  any  other copies thereof may not be provided or     *
 *   otherwise made available to any other person.  No title to and     *
 *   ownership of the software is hereby transferred.                   *
 *                                                                      *
 *   The information in this software is subject to change  without     *
 *   notice  and should not be construed as a commitment by Digital     *
 *   Equipment Corporation.                                             *
 *                                                                      *
 *   Digital assumes no responsibility for the use  or  reliability     *
 *   of its software on equipment which is not supplied by Digital.     *

$
	Page    66,132

	MODEL   LARGE
	
EXTRN   _Scan_Tab:BYTE,_Scan_Defs:WORD,_Keyb_Type:BYTE,_CC_Flag:WORD

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
	ORG     97H
KB_FLAG_2       LABEL   BYTE
KB_FA   EQU     010H
KB_FE   EQU     020H
	ORG     1AH
BUFFER_HEAD     LABEL   WORD
	ORG     1CH
BUFFER_TAIL     LABEL   WORD
	ORG     80H
BUFFER_START    LABEL   WORD
	ORG     82H
BUFFER_END      LABEL   WORD
	ORG     96H
ENH_FLAGS       LABEL   BYTE
	
_BIOS_D ENDS

_TEXT   SEGMENT BYTE PUBLIC 'CODE'
	assume CS:_TEXT
	assume DS:_TEXT

;
; Define Parameter offsets
;

parm1   equ     6
parm2   equ     8
parm3   equ     10
parm4   equ     12
parm5   equ     14
parm6   equ     16


KEYDATA EQU     060H            ; port address of keyboard
KEYCOM  EQU     064H            ; command port of keyboard
;
; response types
;
MACK    EQU     001H            ; acked
MRES    EQU     002H            ; resend
MERR    EQU     003H            ; timeout
;
; misc
;
LK250      EQU  2               ; LK250 keyboard
EPC        EQU  3               ; Enhanced PC keyboard is type 3
CPC        EQU  4               ; PC Convertible keyboard
TANDY      EQU  5               ; TANDY 1400 LT keyboard
TRAP_SHIFT EQU  20H             ; Trapped key, return AL = 0, AH = scancode
EPC_SHIFT  EQU  10H             ; Enhanced key flag
;
; data area (NB: must be in code segment for interrupt code to work)
;

SAVE_KB DB      0               ; saved keyboard status
ENHFLG  DB      0               ; Enhanced key pc flags

SAVEDS  DW      0               ; Saved DS segment
BUFFLG  DB      0               ; Flag to indicate we replace buffer start
KB_VAL  DB      0               ; local keyboard state world
KB_BIN  EQU     1               ; binary mode flag, bypass firmware
ENH_CODE EQU    1               ; Flag in Scan_Tab, Enhanced scancode
NL_FLG  DB      0               ; local flag for num lock handling
;
;*******************************************************************************
;       GETTYP : to determine the kind of PC that we are on
;       RETURNS: system type (FC = IBMAT, FE = IBMXT, FF = IBMPC)
;*******************************************************************************
	PUBLIC  _GETTYP

_GETTYP  PROC    FAR
;                 
	Preamble
	PUSH    ES
	PUSH    DS
	PUSH    CS
	POP     DS
;
; see what kind of system we are working with
;
	MOV     AX,0FFFFH               ; get the magic memory address
	MOV     ES,AX                   ;
	MOV     SI,0DH                  ; and magic offset
	MOV     AL,ES:[SI]              ; and see what's there
	CMP     AL,0D2H                 ; dec machine?
	JZ      DECM                    ;
	MOV     SI,0EH                  ; it's an IBM, see what kind
	MOV     AL,ES:[SI]              ; 
DECM:                                   ;
	XOR     AH,AH                   ; clean up the top byte, to pass back
;
	POP     DS
	POP     ES
	Postcode
	RET
		
_GETTYP  ENDP


;*******************************************************************************
;       OURKBD: keyboard initialization routine
;               Set Xbios in bypass mode
;*******************************************************************************
	PUBLIC  _OURKBD

_OURKBD  PROC    FAR
;                 
				; Grab the vectors for interrupts 9 
				; and substitute ours
	Preamble
	PUSH    ES
	PUSH    DS
	
	CLI                     ; Stop interrupts
	MOV     CS:SAVEDS,DS    ; Save DS for INT9 routine
	
	PUSH    CS              ; Make DS the same as CS
	POP     DS              ;
	
	MOV     NL_FLG,AL       ; and put it where INT9 can get at it

	MOV     AX,SEG _BIOS_D  ; Get segment address of BIOS data area
	MOV     ES,AX           ; Move into ES

				    ; Added for Zenith XT & AT
	MOV     CS:BUFFLG,0         ; default is not to replace the addresses
	TEST    ES:BUFFER_START,0FFH ; Get the keyboard buffer start address
	JNZ     OURKB1               ; Check if it contains an address
	MOV     ES:BUFFER_START,1EH ; Nope, need to setup start and end
	MOV     ES:BUFFER_END,3EH   ; address for the buffer
	MOV     CS:BUFFLG,1         ; set a flag so we can restore it later
OURKB1:
	MOV     AX,SEG _BIOS_I  ; Get segment address of BIOS interrupts
	MOV     ES,AX           ; Move into ES

	MOV     AX,ES:IP_9      ; Save the interrupt 9 vector
	MOV     S_IP_9,AX       ;
	MOV     AX,ES:CS_9      ;
	MOV     S_CS_9,AX       ;
	MOV     AX,OFFSET INT9  ; Shove ours into the BIOS vector
	MOV     ES:IP_9,AX      ;
	MOV     AX,CS           ;
	MOV     ES:CS_9,AX      ;

	STI                     ; Restore interrupts now that we are done
	POP     DS
	POP     ES
	Postcode
	RET
		
_OURKBD  ENDP
;
;************************************************************************
;

	PUBLIC  _GETDIR
;
_GETDIR PROC    FAR
	Preamble
	PUSH    ES
	PUSH    CX

	MOV     AX,040H
	MOV     ES,AX

CLOOP:  MOV     BX,ES:BUFFER_HEAD;Find keyboard buffer head
	CMP     BX,ES:BUFFER_TAIL;Compare with keyboard buffer tail
	JE      CLOOP
	
	MOV     SI,BX
	MOV     AX,ES:[SI]
	INC     BX
	INC     BX
	CMP     BX,ES:BUFFER_END
	JNE     COK
	MOV     BX,ES:BUFFER_START
COK:    MOV     ES:BUFFER_HEAD,BX
	
	MOV     BX,parm1[BP]    ; Get parameter 1 address
	clr     CX              ;
	MOV     CL,AH           ;
	MOV     [BX],CX         ; Return Extended code in parameter 1
	XOR     AH,AH           ; Return character in function return

	POP     CX
	POP     ES
	Postcode
	RET
_GETDIR ENDP

;
;************************************************************************
;

	PUBLIC  _GETSCAN


_GETSCAN PROC    FAR
	Preamble
	PUSH    CX

	MOV     AH,0
	INT     16H             ; Get next character
	IF @Model EQ 5
	mov     bx,parm2[bp]
	mov     ds,bx
	mov     bx,parm1[bp]
	ELSE
	MOV     BX,parm1[BP]    ; Get parameter 1 address
	ENDIF
	clr     CX              ;
	MOV     CL,AH           ;
	MOV     [BX],CX         ; Return Extended code in parameter 1
	XOR     AH,AH           ; Return character in function return

	POP     CX
	Postcode
	RET
_GETSCAN ENDP

	PUBLIC  _GETEXT 


_GETEXT PROC    FAR
	Preamble
	PUSH    CX

	MOV     AH,10H          ; Yes, use new firmware routines
	INT     16H             ; Get next character
	IF @Model EQ 5
	MOV     BX,parm2[bp]
	mov     ds,bx
	mov     BX,parm1[BP]
	ELSE
	MOV     BX,parm1[BP]    ; Get parameter 1 address
	ENDIF
	clr     CX              ;
	MOV     CL,AH           ;
	MOV     [BX],CX         ; Return Extended code in parameter 1
	XOR     AH,AH           ; Return character in function return

	POP     CX
	Postcode
	RET
_GETEXT ENDP
;
;************************************************************************
;
;       ANYCHR: return 1 in AX if the buffer is not empty
;
	PUBLIC  _ANYDIR

_ANYDIR PROC    FAR
	Preamble
	PUSH    ES
	
	MOV     AX,040H
	MOV     ES,AX

	MOV     AH,1            ; Set up function code
	MOV     AX,ES:BUFFER_HEAD;Find keyboard buffer head
	CMP     AX,ES:BUFFER_TAIL;Compare with keyboard buffer tail
	JNE     FINDIR
	MOV     AX,0            ; Bad return
FINDIR:

	POP     ES
	Postcode
	RET
_ANYDIR ENDP

	PUBLIC  _ANYCHR

_ANYCHR PROC    FAR
	Preamble

	MOV     AH,1            ; Set up function code
	INT     16H             ; Call keyboard firmware
	JZ      NOCHAR          ; Too bad, nothing
	MOV     AX,1            ; Set success
	JMP     FINISH          ; And return
NOCHAR: MOV     AX,0            ; Bad return
FINISH:

	Postcode
	RET
_ANYCHR ENDP

	PUBLIC  _ANYEXT

_ANYEXT PROC    FAR
	Preamble

	MOV     AH,11H          ; Use new firmware routines
	INT     16H             ; Call keyboard firmware
	JZ      NOEXT           ; Too bad, nothing
	MOV     AX,1            ; Set success
	JMP     FINEXT          ; And return
NOEXT:  MOV     AX,0            ; Bad return
FINEXT:

	Postcode
	RET
_ANYEXT ENDP
;
;       WAITCMD Wait for the last keyboard command to be accepted
;
WAITCMD PROC FAR
	push    ax                      ; save ax register
W01:
	in      al,KEYCOM               ; get the controller status
	test    al,02                   ; is the input buffer full?
	loopnz  W01                     ; yes, keep trying
	pop     ax                      ; restore ax register
	ret
WAITCMD ENDP
;
;************************************************************************
;
;       KEYREL: this routine must be called before leaving
;               it replaces the original vector address for the keyboard
;
	PUBLIC  _KEYREL

_KEYREL  PROC    FAR
				; Grab the vectors for interrupts 9 
				; and substitute ours
	Preamble
	PUSH    ES
	PUSH    DS
	
	CLI                     ; Stop interrupts

	PUSH    CS              ; Make DS the same as CS
	POP     DS              ;
	
	MOV     AX,SEG _BIOS_D  ; Get segment address of BIOS data area
	MOV     ES,AX           ; Move into ES

	TEST    CS:BUFFLG,1     ; Did we replace the buffer start and end?
	JZ      KR1             ; no, continue
	MOV     ES:BUFFER_START,0H ; yes, set them back to zero
	MOV     ES:BUFFER_END,0H
KR1:
	MOV     AX,SEG _BIOS_I  ; Get segment address of BIOS interrupts
	MOV     ES,AX           ; Move into ES

	MOV     AX,S_IP_9       ; Shove saved vector into the BIOS vector
	MOV     ES:IP_9,AX      ;
	MOV     AX,S_CS_9       ;
	MOV     ES:CS_9,AX      ;

	STI                     ; Restore interrupts now that we are done
	
	POP     DS
	POP     ES
	Postcode
	RET

_KEYREL  ENDP
;
;************************************************************************
;
	PUBLIC _PRINT_SCREEN

_PRINT_SCREEN PROC FAR
	Preamble
	PUSH BP
	INT 5
	NOP
	POP BP
	Postcode
	RET
_PRINT_SCREEN ENDP
;************************************************************************
;
	PUBLIC _GETCAPS
;
;        return 1 if CAPSLOCK flag set, 0 otherwise
;
_GETCAPS PROC FAR
	Preamble
	PUSH    ES
	CLD
	MOV     AX,SEG _BIOS_D          ; Set up ES to point at BIOS data area
	MOV     ES,AX
	clr     AX                      ; zero AX
	TEST    ES:KB_FLAG,CAPS_SHIFT   ; Caps lock set?
	JZ      NOCAPS                  ; no
	INC     AX
NOCAPS:
	POP     ES
	Postcode
	RET
_GETCAPS ENDP
;************************************************************************
;

	PUBLIC  _SENDCMD
;
;       Send a command to an AT or LK250 keyboard
;

_SENDCMD PROC    FAR
	Preamble
	PUSH    ES
	CLD                             ;
	MOV     AX,SEG _BIOS_D          ; Set up Es to point at Bios data area
	MOV     ES,AX                   ;
;
;
;
	mov     bl,3                    ; retry max 3 times
snd0:
	cli                             ; clear interrupts whilt we do this
	AND     ES:KB_FLAG_2, NOT (KB_FE+KB_FA); Clear flags
	OR      CS:KB_VAL,KB_BIN        ; indicate thet we are in binary mode
	sub     cx,cx                   ; clear cx
	CALL    WAITCMD
;
	mov     al,parm1[bp]            ; get the command from the stack
	out     KEYDATA,al              ; give it to the keyboard
	sti                             ; reenable interrupts
;
	mov     cx,0EFFFH               ; set up for 10ms+
snd1:
	TEST    ES:KB_FLAG_2,KB_FE+KB_FA; Test for return
	jnz     snd3                    ; Yup, we are in luck
	loop    snd1                    ; keep looping
	MOV     AX,MERR                 ; Timeout
	jmp     short snd4
snd2:
	dec     bl                      ; we got a resend, decrement the resend counter
	jnz     snd0                    ; go send the command over again
	MOV     AX,MRES                 ; return resend
	jmp     short snd4              ; and return
snd3:
	TEST    ES:KB_FLAG_2,KB_FA      ; was it an ack
	je      snd2                    ; no, must have been an error
	MOV     AX,MACK                 ; Return ACK
snd4:
	AND     CS:KB_VAL,NOT KB_BIN    ; Return to normal mode

	POP     ES
	Postcode
	RET
	
_SENDCMD ENDP
;
;       GETEQ        get the equipment byte from the firmware
;

PUBLIC  _GETEQ

_GETEQ        PROC FAR
	Preamble
	push    es

	clr     ax
	mov     es,ax
	mov     si,410H
	mov     al,es:[si]
	
	pop     es
	Postcode
	ret
	
_GETEQ  ENDP

_BIOS_I SEGMENT AT 0H           ; BIOS interrupt vectors
	ASSUME DS:_BIOS_I
	
	ORG 9H*4
IP_9    LABEL WORD              ; IP for interrupt 9
	ORG 9H*4+2
CS_9    LABEL WORD              ; CS for interrupt 9

	ORG 0CH*4
IP_C    LABEL WORD              ; IP for interrupt A
	ORG 0CH*4+2
CS_C    LABEL WORD              ; CS for interrupt A

_BIOS_I ENDS

	assume CS:_TEXT
	assume DS:_TEXT

P_IP_9  LABEL   DWORD
S_IP_9  DW      0               ; The hardware interrupt address we replaced
S_CS_9  DW      0
S_IP_C  DW      0               ; The hardware interrupt address we replaced
S_CS_C  DW      0


	PUBLIC  INT9

INT9    PROC    FAR             ; This is our private INT 9 handler for LK250
	assume  DS:_BIOS_D

	STI                     ; Enable interrupts
	PUSHF
	PUSH    AX              ; Save a register to work with
	PUSH    DS              ;
	PUSH    ES
	PUSH    BX              ;
	PUSH    CX
	MOV     BX,SEG _BIOS_D  ;
	MOV     DS,BX           ;
	MOV     ES,SAVEDS
	IN      AL,60H          ; See what we have in the buffer
	MOV     AH,AL           ; Save the scan code for later
	CMP     AL,0FEH         ; Resend?
	JE      GOTRES          ; Set flag and return
	CMP     AL,0FAH         ; Ack?
	JE      GOTACK          ; Too

	TEST    CS:KB_VAL,KB_BIN; Are we waiting for a keyboard response?
	JE      NOT_BIN
	JMP     SETTRAP         ; Yes, pass the raw scancode

NOT_BIN:CMP     AL,0E0H         ; EPC code one?
	JE      SETEPC          ; Yes
	CMP     AL,0E1H         ; EPC code two?
	JE      SETEPC          ; Yes
	JMP     CONT

GOTRES: OR      KB_FLAG_2,KB_FE ; Indicate error received
	JMP     ALLDON          ; Call firmware

GOTACK: OR      KB_FLAG_2,KB_FA ; Indicate ACK received
	JMP     ALLDON             ;

SETEPC: CMP     ES:_Keyb_Type,LK250 ; LK250 Keyboard?
	JE      PASS            ; Yes, don't do enhanced keyboard checking
	OR      CS:ENHFLG,EPC_SHIFT ; Flag that we have seen the EPC shift
	JMP     PASS

CONT:   TEST    AL,80H          ; Release code?
	JNZ     PASSON          ; Yes, pass on to firmware

	TEST    CS:ENHFLG,EPC_SHIFT ; Enhanced key?
	JZ      NOTBRK          ; No, forget it
	CMP     AL,046H         ; Break key
	JNE     NOTBRK          ; No
	MOV     ES:_CC_Flag,1   ; Set CC_FLag
	JMP     ALLDON          ; Return

NOTBRK: MOV     BX,0            ; Set starting index
	CMP     ES:_Scan_Defs,0 ; Any entries in table?
	JE      PASSON          ; No, Skip first test
	MOV     CX,ES:_Scan_Defs ; Get number of entries
SLOOP:  CMP     AL,ES:_Scan_Tab[BX]; Same scancode?
	JE      GOTIT           ; Yes, handle
INCIT:  ADD     BX,3            ; Increment pointer
	LOOP    SLOOP           ; Test the next
	JMP     PASSON          ; Not in table, pass on

GOTIT:  TEST    CS:ENHFLG,EPC_SHIFT ; Enhanced key?
	JNZ     GOTEPC          ; Yes, do that test
	TEST    ES:_Scan_Tab+2[BX],ENH_CODE ; Table entry for enhanced key?
	JE      ISOK            ; No, so we are ok
	JMP     INCIT           ; Not OK, go test the next
GOTEPC: TEST    ES:_Scan_Tab+2[BX],ENH_CODE ; Table entry for enhanced key?
	JE      INCIT           ; No, go test the next
	JMP     SETTRAP

ISOK:   JMP     BYPASS          ; Return

PASSON: AND     CS:ENHFLG,NOT EPC_SHIFT ; clear EPC flag
PASS:   POP     CX
	POP     BX              ;
	POP     ES
	POP     DS              ;
	POP     AX              ; Restore the AL register
	POPF                    ; and call firmware
	CLI
	JMP     CS:P_IP_9       ; Call the interrupt routine we saved
				; Go back to where we came from

;
; Bypass firmware to trap the keys the firware won't let us get to
;

SETTRAP:
	OR      CS:ENHFLG,TRAP_SHIFT ; Mark as trapped key

BYPASS: STI                     ; Allow interrupts
	PUSH    DX              ;
	PUSH    SI              ;
	PUSH    DI              ;
	CLD                     ; Set for forward

	TEST    CS:ENHFLG,TRAP_SHIFT ; Trapped key?
	JZ      PUT             ; No
	XOR     AH,AH           ; Zero AL
	AND     CS:ENHFLG,NOT TRAP_SHIFT ; Clear trap flag

PUT:    MOV     BX,BUFFER_TAIL  ; Get the buffer tail
	MOV     SI,BX           ; Save for later store
	INC     BX              ; Increment
	INC     BX              ;
	CMP     BX,BUFFER_END   ; At the end of the buffer?
	JNE     OK              ; No, OK
	MOV     BX,BUFFER_START ; Yes, wrap around
OK:     CMP     BX,BUFFER_HEAD  ; Buffer full?
	JE      DONE            ; try this
	MOV     [SI],AX         ;
	MOV     BUFFER_TAIL,BX  ; Indicate we did it

DONE:   CLI                     ; turn off interrupts
	POP     DI              ;
	POP     SI              ;
	POP     DX              ;
	AND     CS:ENHFLG,NOT EPC_SHIFT ; Clear EPC flag
ALLDON: IN      AL,61H          ; Get control port
	MOV     AH,AL           ; Save it
	OR      AL,80H          ; Reset bit for keyboard
	OUT     61H,AL          ; Do reset
	XCHG    AH,AL           ; Retrieve original control
	OUT     61H,AL          ; Finish reset
	MOV     AX,09102H       ; Wake up any waiters
	INT     15H             ;
	POP     CX              ;
	POP     BX              ;
	POP     ES              ;
	POP     DS              ;
	MOV     AL,20H          ; End of interrupt command
	OUT     20H,AL          ; Send command to int control port
	POP     AX              ; Restore the working register
	POPF                    ; Restore flags
	IRET                    ; Return to where we came from
;
INT9    ENDP

PUBLIC  _GETSHIFT

_GETSHIFT       PROC FAR

	Preamble
	PUSH    DS
	MOV     AX,SEG _BIOS_D
	MOV     DS,AX
	MOV     AX,1
	TEST    KB_FLAG,RIGHT_SHIFT+LEFT_SHIFT
	JNZ     SHIFTOK
	MOV     AX,0
SHIFTOK:
	POP     DS
	
	Postcode
	RET
	
_GETSHIFT ENDP

PUBLIC  _GETALT

_GETALT         PROC FAR

	Preamble
	PUSH    DS
	MOV     AX,SEG _BIOS_D
	MOV     DS,AX
	MOV     AX,1
	TEST    KB_FLAG,ALT_SHIFT
	JNZ     ALTOK
	MOV     AX,0
ALTOK:
	POP     DS
	
	Postcode
	RET
	
_GETALT ENDP

PUBLIC  _GETNUM

_GETNUM         PROC FAR

	Preamble
	PUSH    DS
	MOV     AX,SEG _BIOS_D
	MOV     DS,AX
	MOV     AX,1
	TEST    KB_FLAG,NUM_SHIFT
	JNZ     NUMOK
	MOV     AX,0
NUMOK:
	POP     DS
	
	Postcode
	RET
	
_GETNUM ENDP

	PUBLIC  _SETNUMLOCK
_SETNUMLOCK     PROC FAR

	Preamble
	
	PUSH    DS
	MOV     AX,SEG _BIOS_D  ; Get segment address of BIOS data area
	MOV     DS,AX           ; Move into DS
	MOV     AL,KB_FLAG   ; Get the keyboard state
	AND     AL,NUM_SHIFT+SCROLL_SHIFT       ; isolate num and scroll
	MOV     SAVE_KB,AL      ; Save them
	MOV     AL,KB_FLAG   ; Get the flags again
	AND     AL,NOT SCROLL_SHIFT     ; Clear scroll
	OR      AL,NUM_SHIFT    ; and set num lock
	MOV     KB_FLAG,AL   ; and put into BIOS state
	POP     DS
	
	Postcode
	RET

_SETNUMLOCK     ENDP

	PUBLIC  _CLRNUMLOCK
_CLRNUMLOCK     PROC FAR

	Preamble
	
	PUSH    DS
	MOV     AX,SEG _BIOS_D  ; Get segment address of BIOS data area
	MOV     DS,AX           ; Move into DS
	MOV     AL,KB_FLAG   ; Get the keyboard state
	AND     AL,NUM_SHIFT+SCROLL_SHIFT       ; isolate num and scroll
	MOV     SAVE_KB,AL      ; Save them
	MOV     AL,KB_FLAG   ; Get the flags again
	AND     AL,NOT SCROLL_SHIFT+NUM_SHIFT     ; Clear scroll and Num Lock
	MOV     KB_FLAG,AL   ; and put into BIOS state
	POP     DS
	
	Postcode
	RET

_CLRNUMLOCK     ENDP

	PUBLIC  _RESNUMLOCK
_RESNUMLOCK     PROC FAR

	Preamble
	
	PUSH    DS
	MOV     AX,SEG _BIOS_D  ; Get segment address of BIOS data area
	MOV     DS,AX           ; Move into DS
	MOV     AL,KB_FLAG   ; Get the keyboard state
	AND     AL,NOT (NUM_SHIFT+SCROLL_SHIFT); isolate num and scroll
	OR      AL,SAVE_KB      ; set the old values
	MOV     KB_FLAG,AL   ; and put into BIOS state
	POP     DS
	
	Postcode
	RET

_RESNUMLOCK     ENDP

		PUBLIC  _pc_enh_keyb
_pc_enh_keyb    proc far

		push    es
		push    bx
		
		xor     ax,ax
		mov     ax,SEG _BIOS_D
		mov     es,ax
		mov     bl,es:ENH_FLAGS
		test    bl,10H
		je      notenh
		mov     ax,1
		
notenh:
		pop     bx
		pop     es
		ret
		
_pc_enh_keyb    endp                
		
		PUBLIC  _pc_get_cursor
_pc_get_cursor    proc far

		Preamble
		
		mov     ah,03H
		mov     bh,00H
		int     10H
		mov     ax,dx
		
		Postcode
		ret
		
_pc_get_cursor    endp                
		
_TEXT   ENDS

	end
