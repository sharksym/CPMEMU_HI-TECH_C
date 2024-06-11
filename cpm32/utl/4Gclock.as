;*****************************************************************
; 4Gclock -- Benchmark program : 4,000,019,200 clocks @ Z80
;			Copyright (C) 2004-2005 by Keiji Murakami

	psect	data
	psect	bss
	psect	text

	ld	de,800		; 10
loop1:
	ld	hl,50000	; 10
loop0:
	push	hl		; 11
	push	de		; 11

	ld	a,(hl)		; 7
	ld	l,a		; 4
	rla			; 4
	sbc	a,a		; 4
	ld	h,a		; 4
	add	a,30h		; 7
	inc	a		; 4

	pop	de		; 10
	pop	hl		; 10

	dec	hl		; 6
	ld	a,l		; 4
	or	h		; 4
	jp	nz,loop0	; 10

	dec	de		; 6
	ld	a,e		; 4
	or	d		; 4
	jp	nz,loop1	; 10
	jp	0

	end
