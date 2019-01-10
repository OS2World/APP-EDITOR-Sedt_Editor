;Copyright (c) Anker Berg-Sonne, 1985,1986,1987
        Title   SEDT Low Level Functions
	Page	66,132
;
; SEDT Low Level Functions
;
;	Version 1.0
;
;	Anker Berg-sonne	July-Nov 1984
;
;	Modified by:
;
;		Reid Brown	Nov 30, 1984	+ Converted to MSDOS
;						  via LATTICE-C Compiler
;
;		Reid Brown	Feb 19, 1985	+ Added fast video support
;
; local macros
clr	macro	reg
	xor	reg,reg
        endm

Preamble        macro
        push    bp
        mov     bp,sp
        push    si
        push    di
        endm

Postcode        macro
        pop     di
        pop     si
        pop     bp
        endm
        
_TEXT   SEGMENT BYTE PUBLIC 'CODE'
        assume CS:_TEXT
        assume DS:_TEXT


;
; Define Parameter offsets
;

parm1	equ	6
parm2	equ	8
parm3	equ	10
parm4	equ	12
parm5   equ     14
parm6   equ     16

	page
;
; C_Scan - Scan for a character
;
;	Scan forward in memory looking for a specific character
;
;		C_Scan_( start_addr, length, character )
;
;		(returns location of match, or length if not found)
;
;		note: high bits of match character arg must = 0
;

        public  _C_Scan

_C_Scan proc     far
        Preamble
	push	es

	mov	ax, ds		; Overmap ES with DS
	mov	es, ax

	mov	di, parm1[bp]
	mov	cx, parm2[bp]
	mov	ax, parm3[bp]
	cmp	cx, 0
	jle	C_S_2		; Exit if negative count
	sahf			; Reset flags to 0 using high byte
	cld
	repne	scasb		; Look for CX count=0 or match on AL
	je	C_S_3
	mov	ax,parm2[bp]	; Return size of string + 1 (not found)
	jmp	C_S_2

C_S_3:	inc	cx
	mov	ax,parm2[bp]	; Subtract decremented count from full count,
	sub	ax,cx		; giving the location where char was found

C_S_2:  pop     es
        Postcode
        ret     

_C_Scan endp
	page
;
; C_Bscan - Backward Scan for character
;
;	Scan Backwards in memory for a character
;
;		C_Bscan_( start_address, length, character )
;
;		(returns location of found character in string)
;
;		note: insure high byte of character arg = 0
;		      start_address is loc to start searching bwd from
;		      length must be negative (e.g. -60)
;

        public  _C_Bscan

_C_Bscan proc    far
        Preamble
	push	es

	mov	ax, ds		; Overmap ES with DS
	mov	es, ax

	mov	di, parm1[bp]
	mov	cx, parm2[bp]
	neg	cx
	mov	ax, parm3[bp]
	cmp	cx, 0
	jle	C_B_2
	sahf
	std
	repne	scasb
	je	C_B_3
	mov	ax,parm2[bp]
	jmp	C_B_2

C_B_3:	inc	cx
	mov	ax,parm2[bp]
	add	ax,cx

C_B_2:  cld
	pop     es
        Postcode
        ret     

_C_Bscan endp


_TEXT   ENDS

        end
