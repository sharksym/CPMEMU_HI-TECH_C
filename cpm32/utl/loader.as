;***************************************************************
; loadhi -- hi-memory loader
;			Copyright (C) 2004-2005 by Keiji Murakami

BDOS	equ	5

	psect	loader

	global	__Lbss,__Ltext,__Htext

start:	ld	hl,(BDOS+1)
	ld	sp,hl

	ld	de,__Htext	; BC = relocation gap
	and	a
	sbc	hl,de
	ld	b,h
	ld	c,l

	ld	hl,__Lbss	; HL = relocation data top

	ld	e,(hl)		; DE = relocation data count
	inc	hl
	ld	d,(hl)
	inc	hl
1:
	push	de		; for ( DE times) {
	ld	e,(hl)		;   DE2 = *HL++;
	inc	hl
	ld	d,(hl)
	inc	hl

	push	hl
	ld	hl,__Ltext-1	;   if ( program top <= label addr.) {
	and	a
	sbc	hl,de
	jr	nc,2f

	ex	de,hl
	ld	e,(hl)		;     *DE2 += BC;
	inc	hl
	ld	d,(hl)
	ex	de,hl
	add	hl,bc
	ex	de,hl
	ld	(hl),d
	dec	hl
	ld	(hl),e
2:
	pop	hl		;   }
	pop	de

	dec	de
	ld	a,e
	or	d
	jr	nz,1b		; }

	ld	hl,__Htext-1
	push	hl		; source
	add	hl,bc
	push	hl		; destination

	ld	hl,__Htext
	ld	de,__Ltext
	and	a
	sbc	hl,de
	ld	b,h		; code size
	ld	c,l

	pop	de
	pop	hl
	lddr			; block move

	ex	de,hl
	inc	hl
	jp	(hl)		; jump to top of relocated code

	psect	bss

	end	start
