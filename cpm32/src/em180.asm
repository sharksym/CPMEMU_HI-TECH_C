;**************************************************************
;*         HD64180(Z80) emulator for WIN32 v0.2               *
;*                      em180.c                               *
;*         Copyleft (C) 2004-2012 by K.Murakami               *
;**************************************************************
	.386
	.model flat

	public _reg
	public _mem
	public _guard
	public _em180

	extern	_io_input:near
	extern	_io_output:near

SF	equ	10000000b
ZF	equ	01000000b
HF	equ	00010000b
PF	equ	00000100b
VF	equ	PF
NF	equ	00000010b
CF	equ	00000001b

_TEXT	segment dword public use32 'CODE'
_TEXT	ends
_DATA	segment dword public use32 'DATA'
_DATA	ends
_BSS	segment dword public use32 'BSS'
_BSS	ends

_BSS	segment dword public use32 'BSS'
	align	4

_reg	label	byte

_BC	label	word
_C	db	?
_B	db	?
_DE	label	word
_E	db	?
_D	db	?
_HL	label	word
_L	db	?
_H	db	?
_AF	label	word
_F	db	?
_A	db	?
_IX	label	word
_IXL	db      ?
_IXH	db	?
_IY	label	word
_IYL	db	?
_IYH	db	?
_BC2	dw	?
_DE2	dw	?
_HL2	dw	?
_AF2	dw	?
_SP	dw	?
_PC	dw	?
_I	db	?
_R	db	?
_IE	db	?
	db	?

_mem	label	byte
_memw	label	word
	db	10000h dup(?)
_guard	label	byte
	db	10h dup(?)

_BSS	ends

_TEXT	segment dword public use32 'CODE'


_em180	proc	near
	push	ebp
	push	ebx
	push	esi
	push	edi
	xor	edi, edi
	movzx	esi, _PC
	movzx	ebp, _SP
	movzx	ebx, _HL
	movzx	ecx, _AF
noop:
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


illegalCB:
	or	ax,0cb00h
	jmp	short illegal
illegalED:
	or	ax,0ed00h
	jmp	short illegal
illegalDD:
;	or	ax,0dd00h
;	jmp	short illegal
	jmp	func[eax*4]
illegalFD:
;	or	ax,0fd00h
;	jmp	short illegal
	jmp	func[eax*4]
illegalDDCB:
	or	eax,0ddcb00h
illegal:
exit:
	mov	_PC, si
	mov	_SP, bp
	mov	_HL, bx
	mov	_AF, cx
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret

rst_xx:	and	al,38h
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	esi,eax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_BC_imm:					; LD BC,imm
	mov	ax,_memw[esi]
	mov	_BC,ax
	add	esi,2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pBC_A:					; LD (BC),A
	movzx	edi,_BC
	mov	_mem[edi],ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_BC:				; INC BC
	inc	_BC
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_B:				; INC B
	shr	cl,1
	inc	_B
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_B:				; DEC B
	shr	cl,1
	dec	_B
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_imm:					; LD B,imm
	mov	al,_mem[esi]
	mov	_B,al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLCA_:				; RLCA
	and	cl,not( HF or NF or CF)
	rol	ch,1
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

EX_AF_AF_:					; EX AF,AF'
	mov	ax,_AF2
	mov	_AF2,cx
	mov	cx,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_HL_BC:				; ADD HL,BC
	and	cl,not(NF or CF)
	add	bx,_BC
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_pBC_:					; LD A,(BC)
	movzx	edi,_BC
	mov	ch,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_BC:				; DEC BC
	dec	_BC
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_C:				; INC C
	shr	cl,1
	inc	_C
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_C:				; DEC C
	shr	cl,1
	dec	_C
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_imm:					; LD C,imm
	mov	al,_mem[esi]
	mov	_C,al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRCA_:				; RRCA
	and	cl,not( HF or NF or CF)
	ror	ch,1
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DJNZ_rel:					; DJNZ rel
	movsx	ax,_mem[ esi]
	inc	esi
	dec	_B
	jz	short l1
	add	si,ax
l1:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_DE_imm:					; LD DE,imm
	mov	ax,_memw[esi]
	mov	_DE,ax
	add	esi,2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pDE_A:					; LD (DE),A
	movzx	edi,_DE
	mov	_mem[edi],ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_DE:				; INC DE
	inc	_DE
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_D:				; INC D
	shr	cl,1
	inc	_D
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_D:				; DEC D
	shr	cl,1
	dec	_D
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_imm:					; LD D,imm
	mov	al,_mem[esi]
	mov	_D,al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLA_:				; RLA
	mov	ah,cl
	and	cl,not( HF or NF or CF)
	sahf
	rcl	ch,1
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JR_rel:					; JR rel
	movsx	ax,_mem[ esi]
	inc	esi
	add	si,ax
l2:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_HL_DE:				; ADD HL,DE
	and	cl,not(NF or CF)
	add	bx,_DE
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_pDE_:					; LD A,(DE)
	movzx	edi,_DE
	mov	ch,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_DE:				; DEC DE
	dec	_DE
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_E:				; INC E
	shr	cl,1
	inc	_E
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_E:				; DEC E
	shr	cl,1
	dec	_E
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_imm:					; LD E,imm
	mov	al,_mem[esi]
	mov	_E,al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRA_:				; RRA
	mov	ah,cl
	and	cl,not( HF or NF or CF)
	sahf
	rcr	ch,1
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JR_NZ:					; JR NZ,rel
	movsx	ax,_mem[ esi]
	inc	esi
	test	cl, ZF
	jnz	short l3
	add	si,ax
l3:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_HL_imm:					; LD HL,imm
	mov	bx,_memw[esi]
	add	esi,2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pabs_HL:					; LD (abs),HL
	movzx	edi,_memw[esi]
	add	esi,2
	mov	_memw[edi],bx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_HL:				; INC HL
	inc	bx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_H:				; INC H
	shr	cl,1
	inc	bh
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_H:				; DEC H
	shr	cl,1
	dec	bh
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_imm:					; LD H,imm
	mov	bh,_mem[esi]
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DAA_:				; DAA
	mov	al,ch
	mov	ah,cl
	sahf
	test	ah,NF
	jz	short da1
	sahf
	das
	lahf
	cmp	al,0fah
	jb	short da2
	or	cl,not CF
	and	ah,cl
	jmp	short da2
da1:	sahf
	daa
	lahf
	and	ah,not NF
da2:
	mov	cl,ah
	mov	ch,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JR_Z:					; JR Z,rel
	movsx	ax,_mem[ esi]
	inc	esi
	test	cl, ZF
	jz	short l4
	add	si,ax
l4:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_HL_HL:				; ADD HL,HL
	and	cl,not(NF or CF)
	add	bx,bx
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_HL_pabs_:					; LD HL,(abs)
	movzx	edi,_memw[esi]
	add	esi,2
	mov	bx,_memw[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_HL:				; DEC HL
	dec	bx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_L:				; INC L
	shr	cl,1
	inc	bl
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_L:				; DEC L
	shr	cl,1
	dec	bl
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_imm:					; LD L,imm
	mov	bl,_mem[esi]
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CPL_:				; CPL 
	not	ch
	or	cl,HF or NF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JR_NC:					; JR NC,rel
	movsx	ax,_mem[ esi]
	inc	esi
	test	cl, CF
	jnz	short l5
	add	si,ax
l5:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_SP_imm:					; LD SP,imm
	mov	bp,_memw[esi]
	add	esi,2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pabs_A:					; LD (abs),A
	movzx	edi,_memw[esi]
	add	esi,2
	mov	_mem[edi],ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_SP:				; INC SP
	inc	bp
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_pHL_:				; INC (HL)
	shr	cl,1
	inc	_mem[ebx]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_pHL_:				; DEC (HL)
	shr	cl,1
	dec	_mem[ebx]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pHL_imm:					; LD (HL),imm
	mov	al,_mem[esi]
	mov	_mem[ebx],al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SCF:				; SCF
	and	cl,not(HF or NF)
	or	cl,CF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JR_C:					; JR C,rel
	movsx	ax,_mem[ esi]
	inc	esi
	test	cl, CF
	jz	short l6
	add	si,ax
l6:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_HL_SP:				; ADD HL,SP
	and	cl,not(NF or CF)
	add	bx,bp
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_pabs_:					; LD A,(abs)
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ch,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_SP:				; DEC SP
	dec	bp
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_A:				; INC A
	shr	cl,1
	inc	ch
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_A:				; DEC A
	shr	cl,1
	dec	ch
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_imm:					; LD A,imm
	mov	ch,_mem[esi]
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CCF:				; CCF
	and	cl,not(HF or NF)
	xor	cl,CF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_B:					; LD B,B
	mov	al,_B
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_C:					; LD B,C
	mov	al,_C
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_D:					; LD B,D
	mov	al,_D
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_E:					; LD B,E
	mov	al,_E
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_H:					; LD B,H
	mov	_B,bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_L:					; LD B,L
	mov	_B,bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_pHL_:					; LD B,(HL)
	mov	al,_mem[ebx]
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_A:					; LD B,A
	mov	_B,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_B:					; LD C,B
	mov	al,_B
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_C:					; LD C,C
	mov	al,_C
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_D:					; LD C,D
	mov	al,_D
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_E:					; LD C,E
	mov	al,_E
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_H:					; LD C,H
	mov	_C,bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_L:					; LD C,L
	mov	_C,bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_pHL_:					; LD C,(HL)
	mov	al,_mem[ebx]
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_A:					; LD C,A
	mov	_C,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_B:					; LD D,B
	mov	al,_B
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_C:					; LD D,C
	mov	al,_C
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_D:					; LD D,D
	mov	al,_D
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_E:					; LD D,E
	mov	al,_E
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_H:					; LD D,H
	mov	_D,bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_L:					; LD D,L
	mov	_D,bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_pHL_:					; LD D,(HL)
	mov	al,_mem[ebx]
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_A:					; LD D,A
	mov	_D,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_B:					; LD E,B
	mov	al,_B
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_C:					; LD E,C
	mov	al,_C
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_D:					; LD E,D
	mov	al,_D
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_E:					; LD E,E
	mov	al,_E
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_H:					; LD E,H
	mov	_E,bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_L:					; LD E,L
	mov	_E,bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_pHL_:					; LD E,(HL)
	mov	al,_mem[ebx]
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_A:					; LD E,A
	mov	_E,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_B:					; LD H,B
	mov	bh,_B
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_C:					; LD H,C
	mov	bh,_C
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_D:					; LD H,D
	mov	bh,_D
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_E:					; LD H,E
	mov	bh,_E
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_H:					; LD H,H
	mov	bh,bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_L:					; LD H,L
	mov	bh,bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_pHL_:					; LD H,(HL)
	mov	bh,_mem[ebx]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_A:					; LD H,A
	mov	bh,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_B:					; LD L,B
	mov	bl,_B
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_C:					; LD L,C
	mov	bl,_C
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_D:					; LD L,D
	mov	bl,_D
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_E:					; LD L,E
	mov	bl,_E
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_H:					; LD L,H
	mov	bl,bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_L:					; LD L,L
	mov	bl,bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_pHL_:					; LD L,(HL)
	mov	bl,_mem[ebx]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_A:					; LD L,A
	mov	bl,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pHL_B:					; LD (HL),B
	mov	al,_B
	mov	_mem[ebx],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pHL_C:					; LD (HL),C
	mov	al,_C
	mov	_mem[ebx],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pHL_D:					; LD (HL),D
	mov	al,_D
	mov	_mem[ebx],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pHL_E:					; LD (HL),E
	mov	al,_E
	mov	_mem[ebx],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pHL_H:					; LD (HL),H
	mov	_mem[ebx],bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pHL_L:					; LD (HL),L
	mov	_mem[ebx],bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

HALT_:					; HALT
	mov	al,1
	jmp	exit
LD_pHL_A:					; LD (HL),A
	mov	_mem[ebx],ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_B:					; LD A,B
	mov	ch,_B
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_C:					; LD A,C
	mov	ch,_C
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_D:					; LD A,D
	mov	ch,_D
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_E:					; LD A,E
	mov	ch,_E
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_H:					; LD A,H
	mov	ch,bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_L:					; LD A,L
	mov	ch,bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_pHL_:					; LD A,(HL)
	mov	ch,_mem[ebx]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_A:					; LD A,A
	mov	ch,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_B:				; ADD A,B
	add	ch,_B
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_C:				; ADD A,C
	add	ch,_C
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_D:				; ADD A,D
	add	ch,_D
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_E:				; ADD A,E
	add	ch,_E
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_H:				; ADD A,H
	add	ch,bh
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_L:				; ADD A,L
	add	ch,bl
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_pHL_:				; ADD A,(HL)
	add	ch,_mem[ebx]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_A:				; ADD A,A
	add	ch,ch
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_B:				; ADC A,B
	shr	cl,1
	adc	ch,_B
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_C:				; ADC A,C
	shr	cl,1
	adc	ch,_C
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_D:				; ADC A,D
	shr	cl,1
	adc	ch,_D
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_E:				; ADC A,E
	shr	cl,1
	adc	ch,_E
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_H:				; ADC A,H
	shr	cl,1
	adc	ch,bh
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_L:				; ADC A,L
	shr	cl,1
	adc	ch,bl
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_pHL_:				; ADC A,(HL)
	shr	cl,1
	adc	ch,_mem[ebx]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_A:				; ADC A,A
	shr	cl,1
	adc	ch,ch
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_B:				; SUB A,B
	sub	ch,_B
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_C:				; SUB A,C
	sub	ch,_C
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_D:				; SUB A,D
	sub	ch,_D
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_E:				; SUB A,E
	sub	ch,_E
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_H:				; SUB A,H
	sub	ch,bh
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_L:				; SUB A,L
	sub	ch,bl
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_pHL_:				; SUB A,(HL)
	sub	ch,_mem[ebx]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_A:				; SUB A,A
	sub	ch,ch
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_B:				; SBC A,B
	shr	cl,1
	sbb	ch,_B
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_C:				; SBC A,C
	shr	cl,1
	sbb	ch,_C
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_D:				; SBC A,D
	shr	cl,1
	sbb	ch,_D
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_E:				; SBC A,E
	shr	cl,1
	sbb	ch,_E
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_H:				; SBC A,H
	shr	cl,1
	sbb	ch,bh
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_L:				; SBC A,L
	shr	cl,1
	sbb	ch,bl
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_pHL_:				; SBC A,(HL)
	shr	cl,1
	sbb	ch,_mem[ebx]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_A:				; SBC A,A
	shr	cl,1
	sbb	ch,ch
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_B:				; AND A,B
	and	ch,_B
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_C:				; AND A,C
	and	ch,_C
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_D:				; AND A,D
	and	ch,_D
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_E:				; AND A,E
	and	ch,_E
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_H:				; AND A,H
	and	ch,bh
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_L:				; AND A,L
	and	ch,bl
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_pHL_:				; AND A,(HL)
	and	ch,_mem[ebx]
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_A:				; AND A,A
	and	ch,ch
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_B:				; XOR A,B
	xor	ch,_B
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_C:				; XOR A,C
	xor	ch,_C
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_D:				; XOR A,D
	xor	ch,_D
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_E:				; XOR A,E
	xor	ch,_E
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_H:				; XOR A,H
	xor	ch,bh
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_L:				; XOR A,L
	xor	ch,bl
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_pHL_:				; XOR A,(HL)
	xor	ch,_mem[ebx]
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_A:				; XOR A,A
	xor	ch,ch
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_B:				; OR A,B
	or	ch,_B
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_C:				; OR A,C
	or	ch,_C
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_D:				; OR A,D
	or	ch,_D
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_E:				; OR A,E
	or	ch,_E
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_H:				; OR A,H
	or	ch,bh
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_L:				; OR A,L
	or	ch,bl
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_pHL_:				; OR A,(HL)
	or	ch,_mem[ebx]
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_A:				; OR A,A
	or	ch,ch
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_B:				; CP A,B
	cmp	ch,_B
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_C:				; CP A,C
	cmp	ch,_C
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_D:				; CP A,D
	cmp	ch,_D
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_E:				; CP A,E
	cmp	ch,_E
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_H:				; CP A,H
	cmp	ch,bh
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_L:				; CP A,L
	cmp	ch,bl
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_pHL_:				; CP A,(HL)
	cmp	ch,_mem[ebx]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_A:				; CP A,A
	cmp	ch,ch
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RET_NZ:				; RET NZ
	test	cl, ZF
	jnz	short l7
	movzx	esi, _memw[ ebp]
	add	bp, 2
l7:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


POP_BC:					; POP BC
	mov	di,_memw[ebp]
	add	bp,2
	mov	_BC,di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_NZ:					; JP NZ,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, ZF
	jnz	short l8
	mov	si, di
l8:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_a_:					; JP add
	movzx	edi, _memw[ esi]
	add	esi, 2
	mov	si, di
l9:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CALL_NZ:					; CALL NZ,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, ZF
	jnz	short l10
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l10:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


PUSH_BC:					; PUSH BC
	mov	di, _BC
	sub	bp,2
	mov	_memw[ebp], di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_imm:				; ADD A,imm
	add	ch,_mem[esi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RET_Z:				; RET Z
	test	cl, ZF
	jz	short l11
	movzx	esi, _memw[ ebp]
	add	bp, 2
l11:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


RET_:				; RET 
	movzx	esi, _memw[ ebp]
	add	bp, 2
l12:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


JP_Z:					; JP Z,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, ZF
	jz	short l13
	mov	si, di
l13:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

prebyte_CB:					; prebyte CB
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	funcCB[eax*4]

CALL_Z:					; CALL Z,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, ZF
	jz	short l14
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l14:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


CALL_a_:					; CALL add
	movzx	edi, _memw[ esi]
	add	esi, 2
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l15:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


ADC_A_imm:				; ADC A,imm
	shr	cl,1
	adc	ch,_mem[esi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RET_NC:				; RET NC
	test	cl, CF
	jnz	short l16
	movzx	esi, _memw[ ebp]
	add	bp, 2
l16:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


POP_DE:					; POP DE
	mov	di,_memw[ebp]
	add	bp,2
	mov	_DE,di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_NC:					; JP NC,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, CF
	jnz	short l17
	mov	si, di
l17:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CALL_NC:					; CALL NC,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, CF
	jnz	short l18
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l18:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


PUSH_DE:					; PUSH DE
	mov	di, _DE
	sub	bp,2
	mov	_memw[ebp], di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_imm:				; SUB A,imm
	sub	ch,_mem[esi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RET_C:				; RET C
	test	cl, CF
	jz	short l19
	movzx	esi, _memw[ ebp]
	add	bp, 2
l19:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


EXX:					; EXX 
	mov	ax,_BC
	mov	di,_BC2
	mov	_BC2,ax
	mov	_BC,di
	mov	ax,_DE
	mov	di,_DE2
	mov	_DE2,ax
	mov	_DE,di
	mov	ax,_HL2
	mov	_HL2,bx
	mov	bx,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_C:					; JP C,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, CF
	jz	short l20
	mov	si, di
l20:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CALL_C:					; CALL C,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, CF
	jz	short l21
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l21:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


prebyte_DD:					; prebyte DD
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	funcDD[eax*4]

SBC_A_imm:				; SBC A,imm
	shr	cl,1
	sbb	ch,_mem[esi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RET_PO:				; RET PO
	test	cl, PF
	jnz	short l22
	movzx	esi, _memw[ ebp]
	add	bp, 2
l22:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


POP_HL:					; POP HL
	mov	bx,_memw[ebp]
	add	bp,2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_PO:					; JP PO,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, PF
	jnz	short l23
	mov	si, di
l23:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

EX_pSP_HL:					; EX (SP),HL
	mov	ax,_memw[ebp]
	mov	_memw[ebp],bx
	mov	bx,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CALL_PO:					; CALL PO,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, PF
	jnz	short l24
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l24:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


PUSH_HL:					; PUSH HL
	sub	bp,2
	mov	_memw[ebp], bx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_imm:				; AND A,imm
	and	ch,_mem[esi]
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RET_PE:				; RET PE
	test	cl, PF
	jz	short l25
	movzx	esi, _memw[ ebp]
	add	bp, 2
l25:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


JP_pHL_:					; JP (HL)
	mov	si, bx
l26:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_PE:					; JP PE,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, PF
	jz	short l27
	mov	si, di
l27:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

EX_DE_HL:					; EX DE,HL
	mov	ax,_DE
	mov	_DE,bx
	mov	bx,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CALL_PE:					; CALL PE,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, PF
	jz	short l28
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l28:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


prebyte_ED:					; prebyte ED
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	funcED[eax*4]

XOR_A_imm:				; XOR A,imm
	xor	ch,_mem[esi]
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RET_PL:				; RET PL
	test	cl, SF
	jnz	short l29
	movzx	esi, _memw[ ebp]
	add	bp, 2
l29:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


POP_AF:					; POP AF
	mov	cx,_memw[ebp]
	add	bp,2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_PL:					; JP PL,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, SF
	jnz	short l30
	mov	si, di
l30:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DI_:					; DI
	mov	_IE,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CALL_PL:					; CALL PL,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, SF
	jnz	short l31
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l31:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


PUSH_AF:					; PUSH AF
	sub	bp,2
	mov	_memw[ebp], cx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_imm:				; OR A,imm
	or	ch,_mem[esi]
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RET_MI:				; RET MI
	test	cl, SF
	jz	short l32
	movzx	esi, _memw[ ebp]
	add	bp, 2
l32:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


LD_SP_HL:					; LD SP,HL
	mov	bp,bx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_MI:					; JP MI,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, SF
	jz	short l33
	mov	si, di
l33:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

EI_:					; EI
	mov	_IE,0ffh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CALL_MI:					; CALL MI,add
	movzx	edi, _memw[ esi]
	add	esi, 2
	test	cl, SF
	jz	short l34
	sub	bp, 2
	mov	_memw[ ebp], si
	mov	si, di
l34:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


prebyte_FD:					; prebyte FD
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	funcFD[eax*4]

CP_A_imm:				; CP A,imm
	cmp	ch,_mem[esi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_B:				; RLC
	rol	_B,1
	mov	al,_B
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_C:				; RLC
	rol	_C,1
	mov	al,_C
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_D:				; RLC
	rol	_D,1
	mov	al,_D
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_E:				; RLC
	rol	_E,1
	mov	al,_E
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_H:				; RLC
	rol	bh,1
	inc	bh
	dec	bh
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_L:				; RLC
	rol	bl,1
	inc	bl
	dec	bl
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_pHL_:				; RLC
	rol	_mem[ebx],1
	mov	al,_mem[ebx]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_A:				; RLC
	rol	ch,1
	inc	ch
	dec	ch
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_B:				; RRC
	ror	_B,1
	mov	al,_B
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_C:				; RRC
	ror	_C,1
	mov	al,_C
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_D:				; RRC
	ror	_D,1
	mov	al,_D
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_E:				; RRC
	ror	_E,1
	mov	al,_E
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_H:				; RRC
	ror	bh,1
	inc	bh
	dec	bh
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_L:				; RRC
	ror	bl,1
	inc	bl
	dec	bl
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_pHL_:				; RRC
	ror	_mem[ebx],1
	mov	al,_mem[ebx]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_A:				; RRC
	ror	ch,1
	inc	ch
	dec	ch
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_B:				; RL
	shr	cl,1
	rcl	_B,1
	mov	al,_B
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_C:				; RL
	shr	cl,1
	rcl	_C,1
	mov	al,_C
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_D:				; RL
	shr	cl,1
	rcl	_D,1
	mov	al,_D
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_E:				; RL
	shr	cl,1
	rcl	_E,1
	mov	al,_E
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_H:				; RL
	shr	cl,1
	rcl	bh,1
	inc	bh
	dec	bh
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_L:				; RL
	shr	cl,1
	rcl	bl,1
	inc	bl
	dec	bl
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_pHL_:				; RL
	shr	cl,1
	rcl	_mem[ebx],1
	mov	al,_mem[ebx]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_A:				; RL
	shr	cl,1
	rcl	ch,1
	inc	ch
	dec	ch
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_B:				; RR
	shr	cl,1
	rcr	_B,1
	mov	al,_B
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_C:				; RR
	shr	cl,1
	rcr	_C,1
	mov	al,_C
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_D:				; RR
	shr	cl,1
	rcr	_D,1
	mov	al,_D
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_E:				; RR
	shr	cl,1
	rcr	_E,1
	mov	al,_E
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_H:				; RR
	shr	cl,1
	rcr	bh,1
	inc	bh
	dec	bh
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_L:				; RR
	shr	cl,1
	rcr	bl,1
	inc	bl
	dec	bl
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_pHL_:				; RR
	shr	cl,1
	rcr	_mem[ebx],1
	mov	al,_mem[ebx]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_A:				; RR
	shr	cl,1
	rcr	ch,1
	inc	ch
	dec	ch
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_B:				; SLA
	sal	_B,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_C:				; SLA
	sal	_C,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_D:				; SLA
	sal	_D,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_E:				; SLA
	sal	_E,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_H:				; SLA
	sal	bh,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_L:				; SLA
	sal	bl,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_pHL_:				; SLA
	sal	_mem[ebx],1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_A:				; SLA
	sal	ch,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_B:				; SRA
	sar	_B,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_C:				; SRA
	sar	_C,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_D:				; SRA
	sar	_D,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_E:				; SRA
	sar	_E,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_H:				; SRA
	sar	bh,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_L:				; SRA
	sar	bl,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_pHL_:				; SRA
	sar	_mem[ebx],1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_A:				; SRA
	sar	ch,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_B:				; SL1
	stc
	rcl	_B,1
	mov	al,_B
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_C:				; SL1
	stc
	rcl	_C,1
	mov	al,_C
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_D:				; SL1
	stc
	rcl	_D,1
	mov	al,_D
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_E:				; SL1
	stc
	rcl	_E,1
	mov	al,_E
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_H:				; SL1
	stc
	rcl	bh,1
	inc	bh
	dec	bh
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_L:				; SL1
	stc
	rcl	bl,1
	inc	bl
	dec	bl
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_pHL_:				; SL1
	stc
	rcl	_mem[ebx],1
	mov	al,_mem[ebx]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_A:				; SL1
	stc
	rcl	ch,1
	inc	ch
	dec	ch
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_B:				; SRL
	shr	_B,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_C:				; SRL
	shr	_C,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_D:				; SRL
	shr	_D,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_E:				; SRL
	shr	_E,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_H:				; SRL
	shr	bh,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_L:				; SRL
	shr	bl,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_pHL_:				; SRL
	shr	_mem[ebx],1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_A:				; SRL
	shr	ch,1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_B_0:				; BIT B,0
	test	_B,1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_C_0:				; BIT C,0
	test	_C,1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_D_0:				; BIT D,0
	test	_D,1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_E_0:				; BIT E,0
	test	_E,1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_H_0:				; BIT H,0
	test	bh,1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_L_0:				; BIT L,0
	test	bl,1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pHL_0:				; BIT (HL),0
	test	_mem[ebx],1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_A_0:				; BIT A,0
	test	ch,1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_B_1:				; BIT B,1
	test	_B,1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_C_1:				; BIT C,1
	test	_C,1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_D_1:				; BIT D,1
	test	_D,1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_E_1:				; BIT E,1
	test	_E,1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_H_1:				; BIT H,1
	test	bh,1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_L_1:				; BIT L,1
	test	bl,1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pHL_1:				; BIT (HL),1
	test	_mem[ebx],1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_A_1:				; BIT A,1
	test	ch,1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_B_2:				; BIT B,2
	test	_B,1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_C_2:				; BIT C,2
	test	_C,1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_D_2:				; BIT D,2
	test	_D,1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_E_2:				; BIT E,2
	test	_E,1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_H_2:				; BIT H,2
	test	bh,1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_L_2:				; BIT L,2
	test	bl,1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pHL_2:				; BIT (HL),2
	test	_mem[ebx],1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_A_2:				; BIT A,2
	test	ch,1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_B_3:				; BIT B,3
	test	_B,1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_C_3:				; BIT C,3
	test	_C,1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_D_3:				; BIT D,3
	test	_D,1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_E_3:				; BIT E,3
	test	_E,1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_H_3:				; BIT H,3
	test	bh,1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_L_3:				; BIT L,3
	test	bl,1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pHL_3:				; BIT (HL),3
	test	_mem[ebx],1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_A_3:				; BIT A,3
	test	ch,1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_B_4:				; BIT B,4
	test	_B,1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_C_4:				; BIT C,4
	test	_C,1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_D_4:				; BIT D,4
	test	_D,1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_E_4:				; BIT E,4
	test	_E,1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_H_4:				; BIT H,4
	test	bh,1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_L_4:				; BIT L,4
	test	bl,1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pHL_4:				; BIT (HL),4
	test	_mem[ebx],1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_A_4:				; BIT A,4
	test	ch,1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_B_5:				; BIT B,5
	test	_B,1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_C_5:				; BIT C,5
	test	_C,1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_D_5:				; BIT D,5
	test	_D,1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_E_5:				; BIT E,5
	test	_E,1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_H_5:				; BIT H,5
	test	bh,1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_L_5:				; BIT L,5
	test	bl,1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pHL_5:				; BIT (HL),5
	test	_mem[ebx],1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_A_5:				; BIT A,5
	test	ch,1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_B_6:				; BIT B,6
	test	_B,1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_C_6:				; BIT C,6
	test	_C,1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_D_6:				; BIT D,6
	test	_D,1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_E_6:				; BIT E,6
	test	_E,1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_H_6:				; BIT H,6
	test	bh,1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_L_6:				; BIT L,6
	test	bl,1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pHL_6:				; BIT (HL),6
	test	_mem[ebx],1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_A_6:				; BIT A,6
	test	ch,1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_B_7:				; BIT B,7
	test	_B,1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_C_7:				; BIT C,7
	test	_C,1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_D_7:				; BIT D,7
	test	_D,1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_E_7:				; BIT E,7
	test	_E,1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_H_7:				; BIT H,7
	test	bh,1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_L_7:				; BIT L,7
	test	bl,1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pHL_7:				; BIT (HL),7
	test	_mem[ebx],1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_A_7:				; BIT A,7
	test	ch,1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_B_0:				; RES B,0
	and	_B,not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_C_0:				; RES C,0
	and	_C,not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_D_0:				; RES D,0
	and	_D,not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_E_0:				; RES E,0
	and	_E,not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_H_0:				; RES H,0
	and	bh,not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_L_0:				; RES L,0
	and	bl,not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pHL_0:				; RES (HL),0
	and	_mem[ebx],not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_A_0:				; RES A,0
	and	ch,not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_B_1:				; RES B,1
	and	_B,not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_C_1:				; RES C,1
	and	_C,not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_D_1:				; RES D,1
	and	_D,not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_E_1:				; RES E,1
	and	_E,not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_H_1:				; RES H,1
	and	bh,not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_L_1:				; RES L,1
	and	bl,not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pHL_1:				; RES (HL),1
	and	_mem[ebx],not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_A_1:				; RES A,1
	and	ch,not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_B_2:				; RES B,2
	and	_B,not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_C_2:				; RES C,2
	and	_C,not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_D_2:				; RES D,2
	and	_D,not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_E_2:				; RES E,2
	and	_E,not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_H_2:				; RES H,2
	and	bh,not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_L_2:				; RES L,2
	and	bl,not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pHL_2:				; RES (HL),2
	and	_mem[ebx],not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_A_2:				; RES A,2
	and	ch,not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_B_3:				; RES B,3
	and	_B,not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_C_3:				; RES C,3
	and	_C,not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_D_3:				; RES D,3
	and	_D,not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_E_3:				; RES E,3
	and	_E,not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_H_3:				; RES H,3
	and	bh,not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_L_3:				; RES L,3
	and	bl,not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pHL_3:				; RES (HL),3
	and	_mem[ebx],not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_A_3:				; RES A,3
	and	ch,not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_B_4:				; RES B,4
	and	_B,not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_C_4:				; RES C,4
	and	_C,not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_D_4:				; RES D,4
	and	_D,not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_E_4:				; RES E,4
	and	_E,not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_H_4:				; RES H,4
	and	bh,not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_L_4:				; RES L,4
	and	bl,not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pHL_4:				; RES (HL),4
	and	_mem[ebx],not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_A_4:				; RES A,4
	and	ch,not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_B_5:				; RES B,5
	and	_B,not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_C_5:				; RES C,5
	and	_C,not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_D_5:				; RES D,5
	and	_D,not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_E_5:				; RES E,5
	and	_E,not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_H_5:				; RES H,5
	and	bh,not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_L_5:				; RES L,5
	and	bl,not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pHL_5:				; RES (HL),5
	and	_mem[ebx],not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_A_5:				; RES A,5
	and	ch,not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_B_6:				; RES B,6
	and	_B,not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_C_6:				; RES C,6
	and	_C,not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_D_6:				; RES D,6
	and	_D,not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_E_6:				; RES E,6
	and	_E,not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_H_6:				; RES H,6
	and	bh,not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_L_6:				; RES L,6
	and	bl,not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pHL_6:				; RES (HL),6
	and	_mem[ebx],not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_A_6:				; RES A,6
	and	ch,not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_B_7:				; RES B,7
	and	_B,not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_C_7:				; RES C,7
	and	_C,not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_D_7:				; RES D,7
	and	_D,not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_E_7:				; RES E,7
	and	_E,not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_H_7:				; RES H,7
	and	bh,not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_L_7:				; RES L,7
	and	bl,not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pHL_7:				; RES (HL),7
	and	_mem[ebx],not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_A_7:				; RES A,7
	and	ch,not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_B_0:				; SET B,0
	or	_B,1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_C_0:				; SET C,0
	or	_C,1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_D_0:				; SET D,0
	or	_D,1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_E_0:				; SET E,0
	or	_E,1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_H_0:				; SET H,0
	or	bh,1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_L_0:				; SET L,0
	or	bl,1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pHL_0:				; SET (HL),0
	or	_mem[ebx],1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_A_0:				; SET A,0
	or	ch,1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_B_1:				; SET B,1
	or	_B,1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_C_1:				; SET C,1
	or	_C,1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_D_1:				; SET D,1
	or	_D,1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_E_1:				; SET E,1
	or	_E,1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_H_1:				; SET H,1
	or	bh,1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_L_1:				; SET L,1
	or	bl,1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pHL_1:				; SET (HL),1
	or	_mem[ebx],1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_A_1:				; SET A,1
	or	ch,1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_B_2:				; SET B,2
	or	_B,1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_C_2:				; SET C,2
	or	_C,1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_D_2:				; SET D,2
	or	_D,1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_E_2:				; SET E,2
	or	_E,1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_H_2:				; SET H,2
	or	bh,1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_L_2:				; SET L,2
	or	bl,1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pHL_2:				; SET (HL),2
	or	_mem[ebx],1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_A_2:				; SET A,2
	or	ch,1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_B_3:				; SET B,3
	or	_B,1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_C_3:				; SET C,3
	or	_C,1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_D_3:				; SET D,3
	or	_D,1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_E_3:				; SET E,3
	or	_E,1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_H_3:				; SET H,3
	or	bh,1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_L_3:				; SET L,3
	or	bl,1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pHL_3:				; SET (HL),3
	or	_mem[ebx],1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_A_3:				; SET A,3
	or	ch,1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_B_4:				; SET B,4
	or	_B,1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_C_4:				; SET C,4
	or	_C,1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_D_4:				; SET D,4
	or	_D,1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_E_4:				; SET E,4
	or	_E,1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_H_4:				; SET H,4
	or	bh,1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_L_4:				; SET L,4
	or	bl,1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pHL_4:				; SET (HL),4
	or	_mem[ebx],1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_A_4:				; SET A,4
	or	ch,1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_B_5:				; SET B,5
	or	_B,1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_C_5:				; SET C,5
	or	_C,1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_D_5:				; SET D,5
	or	_D,1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_E_5:				; SET E,5
	or	_E,1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_H_5:				; SET H,5
	or	bh,1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_L_5:				; SET L,5
	or	bl,1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pHL_5:				; SET (HL),5
	or	_mem[ebx],1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_A_5:				; SET A,5
	or	ch,1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_B_6:				; SET B,6
	or	_B,1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_C_6:				; SET C,6
	or	_C,1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_D_6:				; SET D,6
	or	_D,1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_E_6:				; SET E,6
	or	_E,1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_H_6:				; SET H,6
	or	bh,1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_L_6:				; SET L,6
	or	bl,1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pHL_6:				; SET (HL),6
	or	_mem[ebx],1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_A_6:				; SET A,6
	or	ch,1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_B_7:				; SET B,7
	or	_B,1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_C_7:				; SET C,7
	or	_C,1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_D_7:				; SET D,7
	or	_D,1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_E_7:				; SET E,7
	or	_E,1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_H_7:				; SET H,7
	or	bh,1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_L_7:				; SET L,7
	or	bl,1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pHL_7:				; SET (HL),7
	or	_mem[ebx],1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_A_7:				; SET A,7
	or	ch,1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_B:				; TST A,B
	test	ch,_B
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_C:				; TST A,C
	test	ch,_C
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_D:				; TST A,D
	test	ch,_D
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_E:				; TST A,E
	test	ch,_E
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_H:				; TST A,H
	test	ch,bh
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_L:				; TST A,L
	test	ch,bl
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_pHL_:				; TST A,(HL)
	test	ch,_mem[ebx]
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_A:				; TST A,A
	test	ch,ch
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_HL_BC:				; SBC HL,BC
	shr	cl,1
	sbb	bx,_BC
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pabs_BC:					; LD (abs),BC
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ax,_BC
	mov	_memw[edi],ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

NEG_A:				; NEG A
	neg	ch
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RETN_:				; RETN 
	movzx	esi, _memw[ ebp]
	add	bp, 2
l35:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


LD_I_A:					; LD I,A
	mov	_I,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_HL_BC:				; ADC HL,BC
	shr	cl,1
	adc	bx,_BC
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_BC_pabs_:					; LD BC,(abs)
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ax,_memw[edi]
	mov	_BC,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

MLT_BC:				; MLT BC
	mov	ax,_BC
	mul	ah
	mov	_BC,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RETI_:				; RETI 
	movzx	esi, _memw[ ebp]
	add	bp, 2
l36:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


LD_R_A:					; LD R,A
	mov	_R,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_HL_DE:				; SBC HL,DE
	shr	cl,1
	sbb	bx,_DE
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pabs_DE:					; LD (abs),DE
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ax,_DE
	mov	_memw[edi],ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_I:					; LD A,I
	mov	ch,_I
	and	cl, not(HF or PF or NF)
	mov	al, _IE
	and	al, PF
	or	cl, al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_HL_DE:				; ADC HL,DE
	shr	cl,1
	adc	bx,_DE
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_DE_pabs_:					; LD DE,(abs)
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ax,_memw[edi]
	mov	_DE,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

MLT_DE:				; MLT DE
	mov	ax,_DE
	mul	ah
	mov	_DE,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_R:					; LD A,R
	mov	al,19
	imul	_R
	inc	al
	mov	ch,al
	mov	_R,al
	and	cl, not(HF or PF or NF)
	mov	al, _IE
	and	al, PF
	or	cl, al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_HL_HL:				; SBC HL,HL
	shr	cl,1
	sbb	bx,bx
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

TST_A_imm:				; TST A,imm
	test	ch,_mem[esi]
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRD_:				; RRD
	mov	al,_mem[ebx]
	mov	ah,ch
	ror	ax,4
	rol	ah,4
	mov	_mem[ebx],al
	mov	ch,ah
	or	ah,ah
	lahf
	and	ah, not(HF or NF or CF)
	and	cl, CF
	or	cl, ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_HL_HL:				; ADC HL,HL
	shr	cl,1
	adc	bx,bx
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

MLT_HL:				; MLT HL
	mov	ax,bx
	mul	ah
	mov	bx,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLD_:				; RLD
	mov	al,_mem[ebx]
	mov	ah,ch
	rol	ah,4
	rol	ax,4
	mov	_mem[ebx],al
	mov	ch,ah
	or	ah,ah
	lahf
	and	ah, not(HF or NF or CF)
	and	cl, CF
	or	cl, ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_HL_SP:				; SBC HL,SP
	shr	cl,1
	sbb	bx,bp
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pabs_SP:					; LD (abs),SP
	movzx	edi,_memw[esi]
	add	esi,2
	mov	_memw[edi],bp
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_HL_SP:				; ADC HL,SP
	shr	cl,1
	adc	bx,bp
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_SP_pabs_:					; LD SP,(abs)
	movzx	edi,_memw[esi]
	add	esi,2
	mov	bp,_memw[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

MLT_SP:				; MLT SP
	mov	ax,bp
	mul	ah
	mov	bp,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LDI:				; LDI
	movzx	edx,_DE
	and	cl,not (HF or PF or NF)
	mov	al,_mem[ebx]
	inc	bx
	mov	_mem[edx],al
	inc	dx
	dec	_BC
	setnz	al
	shl	al,2
	or	cl,al
	mov	_DE,dx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CPI:				; CPI
	and	cl, CF
	cmp	ch,_mem[ebx]
	lahf
	inc	bx
	and	ah, not(PF or CF)
	dec	_BC
	setnz	al
	shl	al,2
	or	cl,ah
	or	cl,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LDD:				; LDD
	movzx	edx,_DE
	and	cl,not (HF or PF or NF)
	mov	al,_mem[ebx]
	dec	bx
	mov	_mem[edx],al
	dec	dx
	dec	_BC
	setnz	al
	shl	al,2
	or	cl,al
	mov	_DE,dx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CPD:				; CPD
	and	cl, CF
	cmp	ch,_mem[ebx]
	lahf
	dec	bx
	and	ah, not(PF or CF)
	dec	_BC
	setnz	al
	shl	al,2
	or	cl,ah
	or	cl,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LDIR:				; LDIR
	movzx	edx,_DE
	and	cl,not (HF or PF or NF)
	mov	di,cx
	movzx	ecx,_BC
l37:
	mov	al,_mem[ebx]
	inc	bx
	mov	_mem[edx],al
	inc	dx
	loop	l37
	mov	_BC,cx
	mov	cx,di
	mov	_DE,dx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CPIR:				; CPIR
	and	cl, CF
	mov	al,ch
	mov	di,cx
	movzx	ecx,_BC
l38:
	cmp	al,_mem[ebx]
	lea	ebx,[ebx+1]
	loopnz	l38
	lahf
	and	ah,not (PF or CF)
	mov	_BC,cx
	add	cx,-1
	mov	cx,di
	sbb	al,al
	and	al,PF
	or	cl,ah
	or	cl,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LDDR:				; LDDR
	movzx	edx,_DE
	and	cl,not (HF or PF or NF)
	mov	di,cx
	movzx	ecx,_BC
l39:
	mov	al,_mem[ebx]
	dec	bx
	mov	_mem[edx],al
	dec	dx
	loop	l39
	mov	_BC,cx
	mov	cx,di
	mov	_DE,dx
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CPDR:				; CPDR
	and	cl, CF
	mov	al,ch
	mov	di,cx
	movzx	ecx,_BC
l40:
	cmp	al,_mem[ebx]
	lea	ebx,[ebx-1]
	loopnz	l40
	lahf
	and	ah,not (PF or CF)
	mov	_BC,cx
	add	cx,-1
	mov	cx,di
	sbb	al,al
	and	al,PF
	or	cl,ah
	or	cl,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CALLN:					; CALLN v
	mov	al,_mem[ esi]
	inc	si
	mov	ah,1
	jmp	exit

RETEM:					; RETEM 
	xor	eax,eax
	jmp	exit

ADD_IX_BC:				; ADD IX,BC
	and	cl,not(NF or CF)
	mov	ax,_BC
	add	_IX,ax
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_IX_DE:				; ADD IX,DE
	and	cl,not(NF or CF)
	mov	ax,_DE
	add	_IX,ax
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IX_imm:					; LD IX,imm
	mov	ax,_memw[esi]
	mov	_IX,ax
	add	esi,2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pabs_IX:					; LD (abs),IX
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ax,_IX
	mov	_memw[edi],ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_IX:				; INC IX
	inc	_IX
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_IXH:				; INC IXH
	shr	cl,1
	inc	_IXH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_IXH:				; DEC IXH
	shr	cl,1
	dec	_IXH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXH_imm:					; LD IXH,imm
	mov	al,_mem[esi]
	mov	_IXH,al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_IX_IX:				; ADD IX,IX
	and	cl,not(NF or CF)
	mov	ax,_IX
	add	_IX,ax
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IX_pabs_:					; LD IX,(abs)
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ax,_memw[edi]
	mov	_IX,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_IX:				; DEC IX
	dec	_IX
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_IXL:				; INC IXL
	shr	cl,1
	inc	_IXL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_IXL:				; DEC IXL
	shr	cl,1
	dec	_IXL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXL_imm:					; LD IXL,imm
	mov	al,_mem[esi]
	mov	_IXL,al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_pIX_:				; INC (IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	shr	cl,1
	inc	_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_pIX_:				; DEC (IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	shr	cl,1
	dec	_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIX_imm:					; LD (IX+d),imm
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_mem[esi]
	mov	_mem[edi],al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_IX_SP:				; ADD IX,SP
	and	cl,not(NF or CF)
	add	_IX,bp
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_IXH:					; LD B,IXH
	mov	al,_IXH
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_IXL:					; LD B,IXL
	mov	al,_IXL
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_pIX_:					; LD B,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_mem[edi]
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_IXH:					; LD C,IXH
	mov	al,_IXH
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_IXL:					; LD C,IXL
	mov	al,_IXL
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_pIX_:					; LD C,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_mem[edi]
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_IXH:					; LD D,IXH
	mov	al,_IXH
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_IXL:					; LD D,IXL
	mov	al,_IXL
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_pIX_:					; LD D,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_mem[edi]
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_IXH:					; LD E,IXH
	mov	al,_IXH
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_IXL:					; LD E,IXL
	mov	al,_IXL
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_pIX_:					; LD E,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_mem[edi]
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXH_B:					; LD IXH,B
	mov	al,_B
	mov	_IXH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXH_C:					; LD IXH,C
	mov	al,_C
	mov	_IXH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXH_D:					; LD IXH,D
	mov	al,_D
	mov	_IXH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXH_E:					; LD IXH,E
	mov	al,_E
	mov	_IXH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXH_IXH:					; LD IXH,IXH
	mov	al,_IXH
	mov	_IXH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXH_IXL:					; LD IXH,IXL
	mov	al,_IXL
	mov	_IXH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_pIX_:					; LD H,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	bh,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXH_A:					; LD IXH,A
	mov	_IXH,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXL_B:					; LD IXL,B
	mov	al,_B
	mov	_IXL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXL_C:					; LD IXL,C
	mov	al,_C
	mov	_IXL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXL_D:					; LD IXL,D
	mov	al,_D
	mov	_IXL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXL_E:					; LD IXL,E
	mov	al,_E
	mov	_IXL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXL_IXH:					; LD IXL,IXH
	mov	al,_IXH
	mov	_IXL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXL_IXL:					; LD IXL,IXL
	mov	al,_IXL
	mov	_IXL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_pIX_:					; LD L,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	bl,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IXL_A:					; LD IXL,A
	mov	_IXL,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIX_B:					; LD (IX+d),B
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_B
	mov	_mem[edi],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIX_C:					; LD (IX+d),C
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_C
	mov	_mem[edi],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIX_D:					; LD (IX+d),D
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_D
	mov	_mem[edi],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIX_E:					; LD (IX+d),E
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	al,_E
	mov	_mem[edi],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIX_H:					; LD (IX+d),H
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	_mem[edi],bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIX_L:					; LD (IX+d),L
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	_mem[edi],bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIX_A:					; LD (IX+d),A
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	_mem[edi],ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_IXH:					; LD A,IXH
	mov	ch,_IXH
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_IXL:					; LD A,IXL
	mov	ch,_IXL
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_pIX_:					; LD A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	mov	ch,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_IXH:				; ADD A,IXH
	add	ch,_IXH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_IXL:				; ADD A,IXL
	add	ch,_IXL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_pIX_:				; ADD A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	add	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_IXH:				; ADC A,IXH
	shr	cl,1
	adc	ch,_IXH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_IXL:				; ADC A,IXL
	shr	cl,1
	adc	ch,_IXL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_pIX_:				; ADC A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	shr	cl,1
	adc	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_IXH:				; SUB A,IXH
	sub	ch,_IXH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_IXL:				; SUB A,IXL
	sub	ch,_IXL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_pIX_:				; SUB A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	sub	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_IXH:				; SBC A,IXH
	shr	cl,1
	sbb	ch,_IXH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_IXL:				; SBC A,IXL
	shr	cl,1
	sbb	ch,_IXL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_pIX_:				; SBC A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	shr	cl,1
	sbb	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_IXH:				; AND A,IXH
	and	ch,_IXH
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_IXL:				; AND A,IXL
	and	ch,_IXL
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_pIX_:				; AND A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	and	ch,_mem[edi]
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_IXH:				; XOR A,IXH
	xor	ch,_IXH
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_IXL:				; XOR A,IXL
	xor	ch,_IXL
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_pIX_:				; XOR A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	xor	ch,_mem[edi]
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_IXH:				; OR A,IXH
	or	ch,_IXH
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_IXL:				; OR A,IXL
	or	ch,_IXL
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_pIX_:				; OR A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	or	ch,_mem[edi]
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_IXH:				; CP A,IXH
	cmp	ch,_IXH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_IXL:				; CP A,IXL
	cmp	ch,_IXL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_pIX_:				; CP A,(IX+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX
	cmp	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

prebyte_DDCB:					; prebyte CB
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IX

	movzx	eax,_mem[ esi]
	inc	esi
	jmp	funcDDCB[eax*4]

POP_IX:					; POP IX
	mov	di,_memw[ebp]
	add	bp,2
	mov	_IX,di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

EX_pSP_IX:					; EX (SP),IX
	mov	ax,_memw[ebp]
	mov	di,_IX
	mov	_IX,ax
	mov	_memw[ebp],di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

PUSH_IX:					; PUSH IX
	mov	di, _IX
	sub	bp,2
	mov	_memw[ebp], di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_pIX_:					; JP (IX)
	mov	si, _IX
l41:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

EX_DE_IX:					; EX DE,IX
	mov	ax,_DE
	mov	di,_IX
	mov	_IX,ax
	mov	_DE,di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_SP_IX:					; LD SP,IX
	mov	bp,_IX
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_IY_BC:				; ADD IY,BC
	and	cl,not(NF or CF)
	mov	ax,_BC
	add	_IY,ax
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_IY_DE:				; ADD IY,DE
	and	cl,not(NF or CF)
	mov	ax,_DE
	add	_IY,ax
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IY_imm:					; LD IY,imm
	mov	ax,_memw[esi]
	mov	_IY,ax
	add	esi,2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pabs_IY:					; LD (abs),IY
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ax,_IY
	mov	_memw[edi],ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_IY:				; INC IY
	inc	_IY
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_IYH:				; INC IYH
	shr	cl,1
	inc	_IYH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_IYH:				; DEC IYH
	shr	cl,1
	dec	_IYH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYH_imm:					; LD IYH,imm
	mov	al,_mem[esi]
	mov	_IYH,al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_IY_IY:				; ADD IY,IY
	and	cl,not(NF or CF)
	mov	ax,_IY
	add	_IY,ax
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IY_pabs_:					; LD IY,(abs)
	movzx	edi,_memw[esi]
	add	esi,2
	mov	ax,_memw[edi]
	mov	_IY,ax
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_IY:				; DEC IY
	dec	_IY
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_IYL:				; INC IYL
	shr	cl,1
	inc	_IYL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_IYL:				; DEC IYL
	shr	cl,1
	dec	_IYL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYL_imm:					; LD IYL,imm
	mov	al,_mem[esi]
	mov	_IYL,al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

INC_pIY_:				; INC (IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	shr	cl,1
	inc	_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

DEC_pIY_:				; DEC (IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	shr	cl,1
	dec	_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIY_imm:					; LD (IY+d),imm
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_mem[esi]
	mov	_mem[edi],al
	inc	esi
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_IY_SP:				; ADD IY,SP
	and	cl,not(NF or CF)
	add	_IY,bp
	adc	cl,0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_IYH:					; LD B,IYH
	mov	al,_IYH
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_IYL:					; LD B,IYL
	mov	al,_IYL
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_B_pIY_:					; LD B,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_mem[edi]
	mov	_B,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_IYH:					; LD C,IYH
	mov	al,_IYH
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_IYL:					; LD C,IYL
	mov	al,_IYL
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_C_pIY_:					; LD C,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_mem[edi]
	mov	_C,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_IYH:					; LD D,IYH
	mov	al,_IYH
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_IYL:					; LD D,IYL
	mov	al,_IYL
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_D_pIY_:					; LD D,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_mem[edi]
	mov	_D,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_IYH:					; LD E,IYH
	mov	al,_IYH
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_IYL:					; LD E,IYL
	mov	al,_IYL
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_E_pIY_:					; LD E,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_mem[edi]
	mov	_E,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYH_B:					; LD IYH,B
	mov	al,_B
	mov	_IYH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYH_C:					; LD IYH,C
	mov	al,_C
	mov	_IYH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYH_D:					; LD IYH,D
	mov	al,_D
	mov	_IYH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYH_E:					; LD IYH,E
	mov	al,_E
	mov	_IYH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYH_IYH:					; LD IYH,IYH
	mov	al,_IYH
	mov	_IYH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYH_IYL:					; LD IYH,IYL
	mov	al,_IYL
	mov	_IYH,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_H_pIY_:					; LD H,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	bh,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYH_A:					; LD IYH,A
	mov	_IYH,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYL_B:					; LD IYL,B
	mov	al,_B
	mov	_IYL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYL_C:					; LD IYL,C
	mov	al,_C
	mov	_IYL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYL_D:					; LD IYL,D
	mov	al,_D
	mov	_IYL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYL_E:					; LD IYL,E
	mov	al,_E
	mov	_IYL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYL_IYH:					; LD IYL,IYH
	mov	al,_IYH
	mov	_IYL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYL_IYL:					; LD IYL,IYL
	mov	al,_IYL
	mov	_IYL,al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_L_pIY_:					; LD L,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	bl,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_IYL_A:					; LD IYL,A
	mov	_IYL,ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIY_B:					; LD (IY+d),B
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_B
	mov	_mem[edi],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIY_C:					; LD (IY+d),C
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_C
	mov	_mem[edi],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIY_D:					; LD (IY+d),D
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_D
	mov	_mem[edi],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIY_E:					; LD (IY+d),E
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	al,_E
	mov	_mem[edi],al
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIY_H:					; LD (IY+d),H
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	_mem[edi],bh
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIY_L:					; LD (IY+d),L
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	_mem[edi],bl
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_pIY_A:					; LD (IY+d),A
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	_mem[edi],ch
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_IYH:					; LD A,IYH
	mov	ch,_IYH
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_IYL:					; LD A,IYL
	mov	ch,_IYL
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_A_pIY_:					; LD A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	mov	ch,_mem[edi]
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_IYH:				; ADD A,IYH
	add	ch,_IYH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_IYL:				; ADD A,IYL
	add	ch,_IYL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADD_A_pIY_:				; ADD A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	add	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_IYH:				; ADC A,IYH
	shr	cl,1
	adc	ch,_IYH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_IYL:				; ADC A,IYL
	shr	cl,1
	adc	ch,_IYL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

ADC_A_pIY_:				; ADC A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	shr	cl,1
	adc	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not(PF or NF)
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_IYH:				; SUB A,IYH
	sub	ch,_IYH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_IYL:				; SUB A,IYL
	sub	ch,_IYL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SUB_A_pIY_:				; SUB A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	sub	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_IYH:				; SBC A,IYH
	shr	cl,1
	sbb	ch,_IYH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_IYL:				; SBC A,IYL
	shr	cl,1
	sbb	ch,_IYL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SBC_A_pIY_:				; SBC A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	shr	cl,1
	sbb	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_IYH:				; AND A,IYH
	and	ch,_IYH
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_IYL:				; AND A,IYL
	and	ch,_IYL
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

AND_A_pIY_:				; AND A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	and	ch,_mem[edi]
	lahf
	xor  ah,HF or NF
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_IYH:				; XOR A,IYH
	xor	ch,_IYH
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_IYL:				; XOR A,IYL
	xor	ch,_IYL
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

XOR_A_pIY_:				; XOR A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	xor	ch,_mem[edi]
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_IYH:				; OR A,IYH
	or	ch,_IYH
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_IYL:				; OR A,IYL
	or	ch,_IYL
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

OR_A_pIY_:				; OR A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	or	ch,_mem[edi]
	lahf
	and  ah,not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_IYH:				; CP A,IYH
	cmp	ch,_IYH
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_IYL:				; CP A,IYL
	cmp	ch,_IYL
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

CP_A_pIY_:				; CP A,(IY+d)
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY
	cmp	ch,_mem[edi]
	seto	cl
	lahf
	shl	cl,2
	and	ah,not PF
	or	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

prebyte_FDCB:					; prebyte CB
	movsx	di,_mem[ esi]
	inc	esi
	add	di,_IY

	movzx	eax,_mem[ esi]
	inc	esi
	jmp	funcDDCB[eax*4]

POP_IY:					; POP IY
	mov	di,_memw[ebp]
	add	bp,2
	mov	_IY,di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

EX_pSP_IY:					; EX (SP),IY
	mov	ax,_memw[ebp]
	mov	di,_IY
	mov	_IY,ax
	mov	_memw[ebp],di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

PUSH_IY:					; PUSH IY
	mov	di, _IY
	sub	bp,2
	mov	_memw[ebp], di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

JP_pIY_:					; JP (IY)
	mov	si, _IY
l42:	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

EX_DE_IY:					; EX DE,IY
	mov	ax,_DE
	mov	di,_IY
	mov	_IY,ax
	mov	_DE,di
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

LD_SP_IY:					; LD SP,IY
	mov	bp,_IY
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RLC_pIX_:				; RLC
	rol	_mem[edi],1
	mov	al,_mem[edi]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RRC_pIX_:				; RRC
	ror	_mem[edi],1
	mov	al,_mem[edi]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RL_pIX_:				; RL
	shr	cl,1
	rcl	_mem[edi],1
	mov	al,_mem[edi]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RR_pIX_:				; RR
	shr	cl,1
	rcr	_mem[edi],1
	mov	al,_mem[edi]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SLA_pIX_:				; SLA
	sal	_mem[edi],1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRA_pIX_:				; SRA
	sar	_mem[edi],1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SL1_pIX_:				; SL1
	stc
	rcl	_mem[edi],1
	mov	al,_mem[edi]
	inc	al
	dec	al
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SRL_pIX_:				; SRL
	shr	_mem[edi],1
	lahf
	and	ah, not(HF or NF)
	mov	cl,ah
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pIX_0:				; BIT (IX+d),0
	test	_mem[edi],1 SHL 0
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pIX_1:				; BIT (IX+d),1
	test	_mem[edi],1 SHL 1
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pIX_2:				; BIT (IX+d),2
	test	_mem[edi],1 SHL 2
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pIX_3:				; BIT (IX+d),3
	test	_mem[edi],1 SHL 3
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pIX_4:				; BIT (IX+d),4
	test	_mem[edi],1 SHL 4
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pIX_5:				; BIT (IX+d),5
	test	_mem[edi],1 SHL 5
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pIX_6:				; BIT (IX+d),6
	test	_mem[edi],1 SHL 6
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

BIT_pIX_7:				; BIT (IX+d),7
	test	_mem[edi],1 SHL 7
	lahf
	and	cl,SF or PF or CF
	and	ah,ZF
	or	cl,ah
	or	cl,HF
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pIX_0:				; RES (IX+d),0
	and	_mem[edi],not( 1 SHL 0)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pIX_1:				; RES (IX+d),1
	and	_mem[edi],not( 1 SHL 1)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pIX_2:				; RES (IX+d),2
	and	_mem[edi],not( 1 SHL 2)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pIX_3:				; RES (IX+d),3
	and	_mem[edi],not( 1 SHL 3)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pIX_4:				; RES (IX+d),4
	and	_mem[edi],not( 1 SHL 4)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pIX_5:				; RES (IX+d),5
	and	_mem[edi],not( 1 SHL 5)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pIX_6:				; RES (IX+d),6
	and	_mem[edi],not( 1 SHL 6)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

RES_pIX_7:				; RES (IX+d),7
	and	_mem[edi],not( 1 SHL 7)
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pIX_0:				; SET (IX+d),0
	or	_mem[edi],1 SHL 0
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pIX_1:				; SET (IX+d),1
	or	_mem[edi],1 SHL 1
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pIX_2:				; SET (IX+d),2
	or	_mem[edi],1 SHL 2
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pIX_3:				; SET (IX+d),3
	or	_mem[edi],1 SHL 3
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pIX_4:				; SET (IX+d),4
	or	_mem[edi],1 SHL 4
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pIX_5:				; SET (IX+d),5
	or	_mem[edi],1 SHL 5
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pIX_6:				; SET (IX+d),6
	or	_mem[edi],1 SHL 6
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]

SET_pIX_7:				; SET (IX+d),7
	or	_mem[edi],1 SHL 7
	movzx	eax,_mem[ esi]
	inc	esi
	jmp	func[eax*4]


func		label	dword
	dd	noop, LD_BC_imm, LD_pBC_A, INC_BC
	dd	INC_B, DEC_B, LD_B_imm, RLCA_
	dd	EX_AF_AF_, ADD_HL_BC, LD_A_pBC_, DEC_BC
	dd	INC_C, DEC_C, LD_C_imm, RRCA_
	dd	DJNZ_rel, LD_DE_imm, LD_pDE_A, INC_DE
	dd	INC_D, DEC_D, LD_D_imm, RLA_
	dd	JR_rel, ADD_HL_DE, LD_A_pDE_, DEC_DE
	dd	INC_E, DEC_E, LD_E_imm, RRA_
	dd	JR_NZ, LD_HL_imm, LD_pabs_HL, INC_HL
	dd	INC_H, DEC_H, LD_H_imm, DAA_
	dd	JR_Z, ADD_HL_HL, LD_HL_pabs_, DEC_HL
	dd	INC_L, DEC_L, LD_L_imm, CPL_
	dd	JR_NC, LD_SP_imm, LD_pabs_A, INC_SP
	dd	INC_pHL_, DEC_pHL_, LD_pHL_imm, SCF
	dd	JR_C, ADD_HL_SP, LD_A_pabs_, DEC_SP
	dd	INC_A, DEC_A, LD_A_imm, CCF
	dd	LD_B_B, LD_B_C, LD_B_D, LD_B_E
	dd	LD_B_H, LD_B_L, LD_B_pHL_, LD_B_A
	dd	LD_C_B, LD_C_C, LD_C_D, LD_C_E
	dd	LD_C_H, LD_C_L, LD_C_pHL_, LD_C_A
	dd	LD_D_B, LD_D_C, LD_D_D, LD_D_E
	dd	LD_D_H, LD_D_L, LD_D_pHL_, LD_D_A
	dd	LD_E_B, LD_E_C, LD_E_D, LD_E_E
	dd	LD_E_H, LD_E_L, LD_E_pHL_, LD_E_A
	dd	LD_H_B, LD_H_C, LD_H_D, LD_H_E
	dd	LD_H_H, LD_H_L, LD_H_pHL_, LD_H_A
	dd	LD_L_B, LD_L_C, LD_L_D, LD_L_E
	dd	LD_L_H, LD_L_L, LD_L_pHL_, LD_L_A
	dd	LD_pHL_B, LD_pHL_C, LD_pHL_D, LD_pHL_E
	dd	LD_pHL_H, LD_pHL_L, HALT_, LD_pHL_A
	dd	LD_A_B, LD_A_C, LD_A_D, LD_A_E
	dd	LD_A_H, LD_A_L, LD_A_pHL_, LD_A_A
	dd	ADD_A_B, ADD_A_C, ADD_A_D, ADD_A_E
	dd	ADD_A_H, ADD_A_L, ADD_A_pHL_, ADD_A_A
	dd	ADC_A_B, ADC_A_C, ADC_A_D, ADC_A_E
	dd	ADC_A_H, ADC_A_L, ADC_A_pHL_, ADC_A_A
	dd	SUB_A_B, SUB_A_C, SUB_A_D, SUB_A_E
	dd	SUB_A_H, SUB_A_L, SUB_A_pHL_, SUB_A_A
	dd	SBC_A_B, SBC_A_C, SBC_A_D, SBC_A_E
	dd	SBC_A_H, SBC_A_L, SBC_A_pHL_, SBC_A_A
	dd	AND_A_B, AND_A_C, AND_A_D, AND_A_E
	dd	AND_A_H, AND_A_L, AND_A_pHL_, AND_A_A
	dd	XOR_A_B, XOR_A_C, XOR_A_D, XOR_A_E
	dd	XOR_A_H, XOR_A_L, XOR_A_pHL_, XOR_A_A
	dd	OR_A_B, OR_A_C, OR_A_D, OR_A_E
	dd	OR_A_H, OR_A_L, OR_A_pHL_, OR_A_A
	dd	CP_A_B, CP_A_C, CP_A_D, CP_A_E
	dd	CP_A_H, CP_A_L, CP_A_pHL_, CP_A_A
	dd	RET_NZ, POP_BC, JP_NZ, JP_a_
	dd	CALL_NZ, PUSH_BC, ADD_A_imm, rst_xx
	dd	RET_Z, RET_, JP_Z, prebyte_CB
	dd	CALL_Z, CALL_a_, ADC_A_imm, rst_xx
	dd	RET_NC, POP_DE, JP_NC, illegal
	dd	CALL_NC, PUSH_DE, SUB_A_imm, rst_xx
	dd	RET_C, EXX, JP_C, illegal
	dd	CALL_C, prebyte_DD, SBC_A_imm, rst_xx
	dd	RET_PO, POP_HL, JP_PO, EX_pSP_HL
	dd	CALL_PO, PUSH_HL, AND_A_imm, rst_xx
	dd	RET_PE, JP_pHL_, JP_PE, EX_DE_HL
	dd	CALL_PE, prebyte_ED, XOR_A_imm, rst_xx
	dd	RET_PL, POP_AF, JP_PL, DI_
	dd	CALL_PL, PUSH_AF, OR_A_imm, rst_xx
	dd	RET_MI, LD_SP_HL, JP_MI, EI_
	dd	CALL_MI, prebyte_FD, CP_A_imm, rst_xx

funcCB	label	dword
	dd	RLC_B, RLC_C, RLC_D, RLC_E
	dd	RLC_H, RLC_L, RLC_pHL_, RLC_A
	dd	RRC_B, RRC_C, RRC_D, RRC_E
	dd	RRC_H, RRC_L, RRC_pHL_, RRC_A
	dd	RL_B, RL_C, RL_D, RL_E
	dd	RL_H, RL_L, RL_pHL_, RL_A
	dd	RR_B, RR_C, RR_D, RR_E
	dd	RR_H, RR_L, RR_pHL_, RR_A
	dd	SLA_B, SLA_C, SLA_D, SLA_E
	dd	SLA_H, SLA_L, SLA_pHL_, SLA_A
	dd	SRA_B, SRA_C, SRA_D, SRA_E
	dd	SRA_H, SRA_L, SRA_pHL_, SRA_A
	dd	SL1_B, SL1_C, SL1_D, SL1_E
	dd	SL1_H, SL1_L, SL1_pHL_, SL1_A
	dd	SRL_B, SRL_C, SRL_D, SRL_E
	dd	SRL_H, SRL_L, SRL_pHL_, SRL_A
	dd	BIT_B_0, BIT_C_0, BIT_D_0, BIT_E_0
	dd	BIT_H_0, BIT_L_0, BIT_pHL_0, BIT_A_0
	dd	BIT_B_1, BIT_C_1, BIT_D_1, BIT_E_1
	dd	BIT_H_1, BIT_L_1, BIT_pHL_1, BIT_A_1
	dd	BIT_B_2, BIT_C_2, BIT_D_2, BIT_E_2
	dd	BIT_H_2, BIT_L_2, BIT_pHL_2, BIT_A_2
	dd	BIT_B_3, BIT_C_3, BIT_D_3, BIT_E_3
	dd	BIT_H_3, BIT_L_3, BIT_pHL_3, BIT_A_3
	dd	BIT_B_4, BIT_C_4, BIT_D_4, BIT_E_4
	dd	BIT_H_4, BIT_L_4, BIT_pHL_4, BIT_A_4
	dd	BIT_B_5, BIT_C_5, BIT_D_5, BIT_E_5
	dd	BIT_H_5, BIT_L_5, BIT_pHL_5, BIT_A_5
	dd	BIT_B_6, BIT_C_6, BIT_D_6, BIT_E_6
	dd	BIT_H_6, BIT_L_6, BIT_pHL_6, BIT_A_6
	dd	BIT_B_7, BIT_C_7, BIT_D_7, BIT_E_7
	dd	BIT_H_7, BIT_L_7, BIT_pHL_7, BIT_A_7
	dd	RES_B_0, RES_C_0, RES_D_0, RES_E_0
	dd	RES_H_0, RES_L_0, RES_pHL_0, RES_A_0
	dd	RES_B_1, RES_C_1, RES_D_1, RES_E_1
	dd	RES_H_1, RES_L_1, RES_pHL_1, RES_A_1
	dd	RES_B_2, RES_C_2, RES_D_2, RES_E_2
	dd	RES_H_2, RES_L_2, RES_pHL_2, RES_A_2
	dd	RES_B_3, RES_C_3, RES_D_3, RES_E_3
	dd	RES_H_3, RES_L_3, RES_pHL_3, RES_A_3
	dd	RES_B_4, RES_C_4, RES_D_4, RES_E_4
	dd	RES_H_4, RES_L_4, RES_pHL_4, RES_A_4
	dd	RES_B_5, RES_C_5, RES_D_5, RES_E_5
	dd	RES_H_5, RES_L_5, RES_pHL_5, RES_A_5
	dd	RES_B_6, RES_C_6, RES_D_6, RES_E_6
	dd	RES_H_6, RES_L_6, RES_pHL_6, RES_A_6
	dd	RES_B_7, RES_C_7, RES_D_7, RES_E_7
	dd	RES_H_7, RES_L_7, RES_pHL_7, RES_A_7
	dd	SET_B_0, SET_C_0, SET_D_0, SET_E_0
	dd	SET_H_0, SET_L_0, SET_pHL_0, SET_A_0
	dd	SET_B_1, SET_C_1, SET_D_1, SET_E_1
	dd	SET_H_1, SET_L_1, SET_pHL_1, SET_A_1
	dd	SET_B_2, SET_C_2, SET_D_2, SET_E_2
	dd	SET_H_2, SET_L_2, SET_pHL_2, SET_A_2
	dd	SET_B_3, SET_C_3, SET_D_3, SET_E_3
	dd	SET_H_3, SET_L_3, SET_pHL_3, SET_A_3
	dd	SET_B_4, SET_C_4, SET_D_4, SET_E_4
	dd	SET_H_4, SET_L_4, SET_pHL_4, SET_A_4
	dd	SET_B_5, SET_C_5, SET_D_5, SET_E_5
	dd	SET_H_5, SET_L_5, SET_pHL_5, SET_A_5
	dd	SET_B_6, SET_C_6, SET_D_6, SET_E_6
	dd	SET_H_6, SET_L_6, SET_pHL_6, SET_A_6
	dd	SET_B_7, SET_C_7, SET_D_7, SET_E_7
	dd	SET_H_7, SET_L_7, SET_pHL_7, SET_A_7

funcED	label	dword
	dd	illegalED, illegalED, illegalED, illegalED
	dd	TST_A_B, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	TST_A_C, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	TST_A_D, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	TST_A_E, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	TST_A_H, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	TST_A_L, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	TST_A_pHL_, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	TST_A_A, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, SBC_HL_BC, LD_pabs_BC
	dd	NEG_A, RETN_, noop, LD_I_A
	dd	illegalED, illegalED, ADC_HL_BC, LD_BC_pabs_
	dd	MLT_BC, RETI_, illegalED, LD_R_A
	dd	illegalED, illegalED, SBC_HL_DE, LD_pabs_DE
	dd	illegalED, illegalED, noop, LD_A_I
	dd	illegalED, illegalED, ADC_HL_DE, LD_DE_pabs_
	dd	MLT_DE, illegalED, noop, LD_A_R
	dd	illegalED, illegalED, SBC_HL_HL, LD_pabs_HL
	dd	TST_A_imm, illegalED, illegalED, RRD_
	dd	illegalED, illegalED, ADC_HL_HL, LD_HL_pabs_
	dd	MLT_HL, illegalED, illegalED, RLD_
	dd	illegalED, illegalED, SBC_HL_SP, LD_pabs_SP
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, ADC_HL_SP, LD_SP_pabs_
	dd	MLT_SP, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	LDI, CPI, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	LDD, CPD, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	LDIR, CPIR, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	LDDR, CPDR, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, CALLN, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, illegalED, illegalED, illegalED
	dd	illegalED, RETEM, illegalED, illegalED

funcDD	label	dword
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, ADD_IX_BC, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, ADD_IX_DE, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, LD_IX_imm, LD_pabs_IX, INC_IX
	dd	INC_IXH, DEC_IXH, LD_IXH_imm, illegalDD
	dd	illegalDD, ADD_IX_IX, LD_IX_pabs_, DEC_IX
	dd	INC_IXL, DEC_IXL, LD_IXL_imm, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	INC_pIX_, DEC_pIX_, LD_pIX_imm, illegalDD
	dd	illegalDD, ADD_IX_SP, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	LD_B_IXH, LD_B_IXL, LD_B_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	LD_C_IXH, LD_C_IXL, LD_C_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	LD_D_IXH, LD_D_IXL, LD_D_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	LD_E_IXH, LD_E_IXL, LD_E_pIX_, illegalDD
	dd	LD_IXH_B, LD_IXH_C, LD_IXH_D, LD_IXH_E
	dd	LD_IXH_IXH, LD_IXH_IXL, LD_H_pIX_, LD_IXH_A
	dd	LD_IXL_B, LD_IXL_C, LD_IXL_D, LD_IXL_E
	dd	LD_IXL_IXH, LD_IXL_IXL, LD_L_pIX_, LD_IXL_A
	dd	LD_pIX_B, LD_pIX_C, LD_pIX_D, LD_pIX_E
	dd	LD_pIX_H, LD_pIX_L, illegalDD, LD_pIX_A
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	LD_A_IXH, LD_A_IXL, LD_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	ADD_A_IXH, ADD_A_IXL, ADD_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	ADC_A_IXH, ADC_A_IXL, ADC_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	SUB_A_IXH, SUB_A_IXL, SUB_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	SBC_A_IXH, SBC_A_IXL, SBC_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	AND_A_IXH, AND_A_IXL, AND_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	XOR_A_IXH, XOR_A_IXL, XOR_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	OR_A_IXH, OR_A_IXL, OR_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	CP_A_IXH, CP_A_IXL, CP_A_pIX_, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, prebyte_DDCB
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, POP_IX, illegalDD, EX_pSP_IX
	dd	illegalDD, PUSH_IX, illegalDD, illegalDD
	dd	illegalDD, JP_pIX_, illegalDD, EX_DE_IX
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD
	dd	illegalDD, LD_SP_IX, illegalDD, illegalDD
	dd	illegalDD, illegalDD, illegalDD, illegalDD

funcFD	label	dword
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, ADD_IY_BC, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, ADD_IY_DE, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, LD_IY_imm, LD_pabs_IY, INC_IY
	dd	INC_IYH, DEC_IYH, LD_IYH_imm, illegalFD
	dd	illegalFD, ADD_IY_IY, LD_IY_pabs_, DEC_IY
	dd	INC_IYL, DEC_IYL, LD_IYL_imm, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	INC_pIY_, DEC_pIY_, LD_pIY_imm, illegalFD
	dd	illegalFD, ADD_IY_SP, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	LD_B_IYH, LD_B_IYL, LD_B_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	LD_C_IYH, LD_C_IYL, LD_C_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	LD_D_IYH, LD_D_IYL, LD_D_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	LD_E_IYH, LD_E_IYL, LD_E_pIY_, illegalFD
	dd	LD_IYH_B, LD_IYH_C, LD_IYH_D, LD_IYH_E
	dd	LD_IYH_IYH, LD_IYH_IYL, LD_H_pIY_, LD_IYH_A
	dd	LD_IYL_B, LD_IYL_C, LD_IYL_D, LD_IYL_E
	dd	LD_IYL_IYH, LD_IYL_IYL, LD_L_pIY_, LD_IYL_A
	dd	LD_pIY_B, LD_pIY_C, LD_pIY_D, LD_pIY_E
	dd	LD_pIY_H, LD_pIY_L, illegalFD, LD_pIY_A
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	LD_A_IYH, LD_A_IYL, LD_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	ADD_A_IYH, ADD_A_IYL, ADD_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	ADC_A_IYH, ADC_A_IYL, ADC_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	SUB_A_IYH, SUB_A_IYL, SUB_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	SBC_A_IYH, SBC_A_IYL, SBC_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	AND_A_IYH, AND_A_IYL, AND_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	XOR_A_IYH, XOR_A_IYL, XOR_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	OR_A_IYH, OR_A_IYL, OR_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	CP_A_IYH, CP_A_IYL, CP_A_pIY_, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, prebyte_FDCB
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, POP_IY, illegalFD, EX_pSP_IY
	dd	illegalFD, PUSH_IY, illegalFD, illegalFD
	dd	illegalFD, JP_pIY_, illegalFD, EX_DE_IY
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD
	dd	illegalFD, LD_SP_IY, illegalFD, illegalFD
	dd	illegalFD, illegalFD, illegalFD, illegalFD

funcDDCB	label	dword
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RLC_pIX_, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RRC_pIX_, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RL_pIX_, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RR_pIX_, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SLA_pIX_, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SRA_pIX_, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SL1_pIX_, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SRL_pIX_, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, BIT_pIX_0, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, BIT_pIX_1, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, BIT_pIX_2, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, BIT_pIX_3, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, BIT_pIX_4, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, BIT_pIX_5, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, BIT_pIX_6, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, BIT_pIX_7, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RES_pIX_0, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RES_pIX_1, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RES_pIX_2, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RES_pIX_3, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RES_pIX_4, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RES_pIX_5, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RES_pIX_6, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, RES_pIX_7, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SET_pIX_0, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SET_pIX_1, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SET_pIX_2, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SET_pIX_3, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SET_pIX_4, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SET_pIX_5, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SET_pIX_6, illegalDDCB
	dd	illegalDDCB, illegalDDCB, illegalDDCB, illegalDDCB
	dd	illegalDDCB, illegalDDCB, SET_pIX_7, illegalDDCB

_em180	endp

_TEXT	ends

	end
