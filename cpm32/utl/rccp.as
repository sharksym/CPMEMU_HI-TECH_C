;***************************************************************
; rccp -- a resident console command processer
;			Copyright (C) 2004-2005 by Keiji Murakami

BOOT	equ	0
BDOS	equ	5
FCB0	equ	5ch
FCB1	equ	6ch
CBUF	equ	80h
TPA	equ	100h

PUTC	equ	2
PUTS	equ	9
GETS	equ	10
SELDSK	equ	14
OPEN	equ	15
CLOSE	equ	16
FND1ST	equ	17
FNDNXT	equ	18
DELETE	equ	19
READ	equ	20
WRITE	equ	21
CREATE	equ	22
RENAME	equ	23
GETDRV	equ	25
SETDMA	equ	26

SUBDRV	equ	2	; B,A

	psect	text

	jp	init

msg_init:	defm	'RCCP 1.0$'

init:	ld	sp,stack
	ld	de,msg_init
	ld	c,PUTS
	call	BDOS

	ld	hl,(BDOS+1)
	ld	(go_bdos+1),hl
	ld	hl,bdos_hook
	ld	(BDOS+1),hl

	ld	hl,(BOOT+1)
	ld	de,boot_save
	ld	bc,3
	ldir

	ld	de,wboot_hook
	dec	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e
	dec	hl
	ld	(hl),0C3h

	ld	de,cmdbuf+1
	ld	hl,CBUF
	ld	c,(hl)
	ld	b,0
	inc	bc
	ldir

	ld	a,0c9h		; set RET code to top of TPA
	ld	(TPA),a

	jp	cmdexec

; ----------------------------------
bdos_hook:
	ld	a,(xsub)
	or	a
	jr	z,go_bdos

	ld	a,c
	cp	GETS
	jr	z,gets_hook
	cp	SETDMA
	jr	nz,go_bdos
	ld	(dma_save),de
go_bdos:
	jp	BDOS

gets_hook:
	push	de
	call	getsub
	push	af
	ld	de,(dma_save)
	ld	c,SETDMA
	call	go_bdos
	pop	af
	pop	de
	jr	nz,1f
	xor	a
	ld	(xsub),a
	ld	c,GETS
	jr	go_bdos

1:	ld	hl,cmdbuf+1
	ld	c,(hl)
	ld	a,(de)
	inc	de
	cp	c
	jr	c,1f
	ld	c,a
1:	ld	b,0
	inc	bc
	ldir
	xor	a
	ld	h,a
	ld	l,a
	ret

wboot_hook:
	ld	a,SUBDRV	; submit check on
	ld	(subfcb),a
cmdloop:
	ld	sp,stack

	call	crlf
	ld	c,GETDRV
	call	BDOS
	add	a,'a'
	ld	e,a
	call	putch
	ld	e,'>'
	call	putch

	call	getsub
	jr	nz,cmdexec

	ld	hl,cmdbuf	; GET cmd line
	ld	(hl),7fh
	ex	de,hl
	ld	c,GETS
	call	BDOS

cmdexec:
	ld	de,CBUF
	ld	c,SETDMA
	call	BDOS

	ld	hl,cmdbuf
	inc	hl
	ld	c,(hl)
	ld	b,0
	inc	hl

	push	hl
	add	hl,bc
	ld	(hl),0		; set delimiter
	pop	hl

	call	skipsp		; set cmd FCB
	ld	(cmdtop),hl
	push	hl
	ld	de,cmdfcb
	call	setfcb
	call	skipsp
	ld	(argtop),hl
	ld	de,FCB0		; set FCB0
	call	setfcb
	call	skipsp
	ld	de,FCB1		; set FCB1
	call	setfcb
	pop	de

	call	gocmd
	jr	cmdloop

; ---------------------------------------
gocmd:
	ld	hl,cmdtbl
1:	ld	b,(hl)
	inc	b
	dec	b
	jp	z, do_disk

	inc	hl
	push	de
2:	ld	a,(de)
	call	toupper
	cp	(hl)
	jr	nz,7f
	inc	hl
	inc	de
	djnz	2b

	ld	a,(de)
	pop	de
	or	a
	jr	z,9f
	cp	' '
	jr	nz,8f

9:	ld	a,(hl)		; ok .. go cmd proc
	inc	hl
	ld	h,(hl)
	ld	l,a
	jp	(hl)

7:	pop	de
2:	inc	hl
	djnz	2b

8:	inc	hl		; ng .. next 
	inc	hl
	jr	1b

; ----------------------------------
do_disk:
	ld	hl,cmdfcb+1
	ld	a,(hl)
	sub	' '
	jr	nz,do_exec

	dec	hl
	or	(hl)
	ret	z
	dec	a
	ld	e,a
	ld	c,SELDSK
	jp	go_bdos

; ----------------------------------
do_exec:
	ld	a,(cmdfcb+1+8)
	cp	' '
	jp	nz,err_cmd

	ld	hl,'C'+'P'*256		; try open fname +".CPM"
	ld	(cmdfcb+1+8),hl
	ld	a,'M'
	ld	(cmdfcb+1+8+2),a

	ld	de,cmdfcb
	ld	c,OPEN
	call	BDOS
	or	a
	jr	z,1f

	ld	a,'O'			; try open fname + ".COM"
	ld	(cmdfcb+1+8+1),a

	ld	de,cmdfcb
	ld	c,OPEN
	call	BDOS
	or	a
	jp	nz,err_cmd
1:	ld	(cmdfcb+32),a

	ld	de,TPA

1:	push	de
	ld	c,SETDMA
	call	go_bdos
	ld	de,cmdfcb
	ld	c,READ
	call	BDOS
	pop	hl

	ld	bc,80h
	add	hl,bc
	ex	de,hl
	ld	hl,bdos_hook - 80h
	sbc	hl,de
	jr	c,1f
	or	a
	jr	z,1b
1:
	ld	de,cmdfcb
	ld	c,CLOSE
	call	BDOS

	ld	de,CBUF
	ld	c,SETDMA
	call	BDOS

do_cont:
	ld	hl,cmdbuf+1
	call	skipsp
	call	skipnsp

	ld	de,CBUF+1	; set CBUF
1:	ld	a,(hl)
	ld	(de),a
	inc	hl
	inc	de
	or	a
	jr	nz,1b

	ld	a,e
	ld	hl,CBUF
	sub	l
	sub	2
	ld	(hl),a		; set CBUF size

	call	crlf
	jp	TPA

; ----------------------------------
do_exit:
	ld	hl,(go_bdos+1)
	ld	(BDOS+1),hl

	ld	hl,boot_save
	ld	de,(BOOT+1)
	ld	bc,3
	ldir

	jp	BOOT

; ----------------------------------
do_xsub:
	ld	a,1
	ld	(xsub),a
	ret

; ----------------------------------
do_era:
	ld	hl,FCB0
	call	ckafn
	cp	11
	jr	nz,1f

	ld	de,msg_all
	ld	c,PUTS
	call	BDOS
	ld	de,cmdbuf
	ld	c,GETS
	call	BDOS
	ld	hl,cmdbuf+1
	ld	a,(hl)
	or	a
	ret	z
	inc	hl
	ld	a,(hl)
	call	toupper
	cp	'Y'
	ret	nz

1:	ld	de,FCB0
	ld	c,DELETE
	call	BDOS
	cp	4
	ret	c
	jr	err_nofile
;
; ----------------------------------
do_ren:
	ld	hl,(argtop)
	ld	de,FCB1
	call	setfcb
	ld	a,(hl)
	cp	'='
	jr	nz,8f
	inc	hl
	ld	de,FCB0
	call	setfcb

	ld	a,(FCB0)
	or	a
	jr	z,1f
	ld	hl,FCB1
	cp	(hl)
	jr	nz,8f
1:
	call	ckafn
8:	ld	hl,(argtop)
	jp	nz,err_arg

	ld	de,FCB1
	ld	c,FND1ST
	call	BDOS
	cp	4
	jr	c,err_exist

	ld	de,FCB0
	ld	c,RENAME
	call	BDOS
	or	a
	ret	z

err_nofile:
	ld	de,no_file
	ld	c,PUTS
	jp	go_bdos

err_exist:
	ld	de,msg_exist
	ld	c,PUTS
	jp	go_bdos

; ----------------------------------
do_type:
	ld	hl,FCB0		; if ( include *,?) error( "arg?");
	call	ckafn
	jr	nz,8b

	ld	de,FCB0		; if ( open FBC0 is fail) error( "NO FILE");
	ld	c,OPEN
	call	BDOS
	or	a
	jr	nz,err_nofile
	ld	(FCB0+32),a

	call	crlf

2:	ld	de,FCB0
	ld	c,READ
	call	BDOS
	or	a
	jr	nz,9f

	ld	hl,CBUF
	ld	b,128

1:	ld	a,(hl)
	inc	hl
	cp	1ah
	jr	z,9f
	push	hl
	push	bc
	ld	e,a
	call	putch
	pop	bc
	pop	hl
	djnz	1b
	jr	2b

9:	ld	de,FCB0
	ld	c,CLOSE
	jp	go_bdos

; ----------------------------------
do_save:
	ld	hl,0
	ld	de,FCB0+1
1:	ld	a,(de)
	inc	de
	cp	' '
	jr	z,9f
	cp	'9'+1
	jr	nc,8f
	sub	'0'
	jr	c,8f
	ld	b,h
	ld	c,l
	add	hl,hl
	add	hl,hl
	add	hl,bc
	add	hl,hl
	ld	b,0
	ld	c,a
	add	hl,bc
	jr	1b

8:	ld	hl,CBUF+2
1:	ld	a,(hl)
	inc	hl
	cp	' '
	jr	z,1f
	or	a
	jr	z,1f
	ld	e,a
	push	hl
	call	putch
	pop	hl
	jr	1b
1:
	ld	e,'?'
	jp	putch

9:	add	hl,hl
	push	hl
	ld	hl,FCB1
	ld	de,FCB0
	ld	bc,12
	ldir
	ld	de,FCB0
	ld	c,DELETE
	call	BDOS
	ld	de,FCB0
	ld	c,CREATE
	call	BDOS
	pop	bc
	or	a
	jr	nz,7f
	ld	(FCB0+32),a

	ld	hl,TPA
1:	ld	a,b
	or	c
	jr	z,1f
	dec	bc
	push	hl
	push	bc
	ex	de,hl
	ld	c,SETDMA
	call	go_bdos
	ld	de,FCB0
	ld	c,WRITE
	call	BDOS
	pop	bc
	pop	hl
	ld	de,80h
	add	hl,de
	or	a
	jr	z,1b
	call	7f
1:
	ld	de,FCB0
	ld	c,CLOSE
	jp	go_bdos

7:	ld	de,no_space
	ld	c,PUTS
	jp	go_bdos

; ----------------------------------
do_dir:
	ld	de,FCB0+1	; if (no arg) fcb = "*.*"
	ld	a,(de)
	cp	' '
	jr	nz,1f
	ld	hl,FCB0+9
	ld	a,(hl)
	cp	' '
	jr	nz,1f
	ld	a,'*'
	ld	(de),a
	ld	(hl),a
1:	dec	de

	ld	c,FND1ST	; FIND 1ST
	call	BDOS
	cp	4		; if (not found)
	jp	nc,err_nofile	;   error( "NO FILE");

	ld	b,a
2:
	ld	c,0
	push	bc
	call	crlf
	ld	a,(FCB0)
	sub	1
	jr	nc,1f
	ld	c,GETDRV
	call	BDOS
1:	add	a,'a'
	ld	e,a
	call	putch
	pop	bc

3:	push	bc
	ld	e,':'
	call	putch
	ld	e,' '
	call	putch
	pop	bc

	ld	a,b
	add	a,a		;
	add	a,a		; HL = fname (CBUF + A*32)
	add	a,a		
	add	a,a
	add	a,a
	ld	hl,CBUF
	ld	e,a
	ld	d,0
	add	hl,de
	inc	hl

	ld	b,8		; loop 11 times { putch( *HL++);}
	call	putnsp
	ld	b,3
	call	putnsp

	ld	de,FCB0
	push	bc
	ld	c,FNDNXT
	call	BDOS
	pop	bc
	or	a
	ret	m
	ld	b,a
	inc	c
	ld	a,c
	cp	5
	jr	nz,3b
	jr	2b

; ----------------------------------
getsub:	ld	hl,subfcb
	inc	(hl)

1:	ld	hl,subfcb
	dec	(hl)		; check B:$$$.SUB, A:$$$.SUB
	ret	z

	ex	de,hl
	ld	c,OPEN
	call	BDOS
	or	a
	jr	nz,1b

	ld	de,cmdbuf+1
	ld	c,SETDMA
	call	go_bdos

	ld	a,(subfcb+15)	; read last block
	dec	a
	ld	(subfcb+32),a
	ld	de,subfcb
	ld	c,READ
	call	BDOS
	or	a
	push	af
	jr	nz,9f

	ld	hl,cmdbuf+1
	ld	b,(hl)
	inc	hl
	call	putnch

	ld	hl,subfcb+14	; S1 = 0
	ld	(hl),a
	inc	hl		; RC--
	dec	(hl)

9:	ld	de,subfcb
	ld	c,CLOSE
	call	BDOS
	pop	af
	sub	1
	sbc	a,a
	ret	nz

brksub:
	ld	hl,subfcb
	inc	(hl)
1:
	ld	hl,subfcb
	dec	(hl)
	ret	z
	ex	de,hl
	ld	c,DELETE
	call	BDOS
	jr	1b

; ----------------------------------
err_cmd:
	ld	hl,(cmdtop)
err_arg:
	push	hl
	call	crlf
	pop	hl
1:	ld	a,(hl)
	inc	hl
	or	a
	jr	z,9f
	cp	' '
	jr	z,9f
	ld	e,a
	push	hl
	call	putch
	pop	hl
	jr	1b
9:
	ld	e,'?'
	call	putch
	jr	brksub

; ----------------------------------
crlf:	ld	e,0Dh
	call	putch
	ld	e,0Ah
putch:	ld	c,PUTC
	jp	go_bdos

; ----------------------------------
putnch:	ld	e,(hl)
	inc	hl
	push	hl
	push	bc
	call	putch
	pop	bc
	pop	hl
	djnz	putnch
	ret
; ----------------------------------
putnsp:	call	putnch
	ld	e,' '
	push	hl
	push	bc
	call	putch
	pop	bc
	pop	hl
	ret
; ----------------------------------
toupper:
	cp	'a'
	ret	c
	cp	'z'+1
	ret	nc
	sub	'a'-'A'
	ret
; ----------------------------------
skipsp:	ld	a,(hl)
	cp	' '
	ret	nz
	inc	hl
	jr	skipsp

; ----------------------------------
skipnsp:
	ld	a,(hl)
	or	a
	ret	z
	cp	' '
	ret	z
	inc	hl
	jr	skipnsp

; ----------------------------------
fillsp:	ld	a,' '		; memset( de, ' ', b);
fill:	inc	b		; memset( de, a, b);
	dec	b
	ret	z
1:	ld	(de),a
	inc	de
	djnz	1b
	ret

; ----------------------------------
setfcb:
	ld	a,(hl)
	or	a
	jr	z,2f
	inc	hl
	call	toupper
	sub	'@'
	ld	(de),a
	inc	de
	ld	a,(hl)
	inc	hl
	cp	':'
	jr	z,1f

	dec	de
	dec	hl
	dec	hl
2:	xor	a
	ld	(de),a
	inc	de

1:	ld	b,8
	call	cpfcb
	ld	a,(hl)
	cp	'.'
	jr	nz,1f
	inc	hl
1:	call	fillsp

	ld	b,3
	call	cpfcb
1:	call	fillsp

	ld	b,4		; clear ex,s1,s2,rc
	xor	a
	jp	fill

; ----------------------------------
cpfcb:	ld	a,(hl)
	or	a
	ret	z
	cp	' '
	ret	z
	cp	':'
	ret	z
	cp	';'
	ret	z
	cp	'='
	ret	z
	cp	'.'
	ret	z
	inc	hl
	ld	(de),a
	inc	de
	djnz	cpfcb
	ret
; ----------------------------------
ckafn:	inc	hl
	ld	bc,800h
	call	1f
	ld	c,b
	ld	b,0
	add	hl,bc
	ld	c,a
	ld	b,3
1:
	ld	a,(hl)
	cp	' '
	jr	z,9f
	cp	'?'
	jr	z,2f
	cp	'*'
	jr	nz,3f
	set	7,c
2:	inc	c
3:	inc	hl
	djnz	1b
9:	ld	a,c
	or	a
	ret	p
	add	a,b
	and	7fh
	ret

; ----------------------------------

no_file: 	defb	0dh,0ah
		defm	'NO FILE$'
no_space:	defb	0dh,0ah
		defm	'NO SPACE$'
msg_all: 	defb	0dh,0ah
		defm	'ALL (Y/N)?$'
msg_exist:	defb	0dh,0ah
		defm	'FILE EXISTS$'

cmdtbl:	defb	3
	defm	'DIR'
	defw	do_dir

	defb	3
	defm	'ERA'
	defw	do_era

	defb	3
	defm	'REN'
	defw	do_ren

	defb	4
	defm	'TYPE'
	defw	do_type

	defb	4
	defm	'SAVE'
	defw	do_save

	defb	4
	defm	'CONT'
	defw	do_cont

	defb	4
	defm	'XSUB'
	defw	do_xsub

	defb	4
	defm	'EXIT'
	defw	do_exit

	defb	1,1ah
	defw	do_exit

	defb	0

boot_save:	defb	0,0,0

xsub:		defb	0
dma_save:	defw	CBUF

cmdtop:	defw	0
argtop:	defw	0

subfcb:	defb	SUBDRV
	defm	'$$$     SUB'
	defs	4+16+1

cmdfcb:	defs	33

cmdbuf:	defs	80h

	defs	32
stack:
