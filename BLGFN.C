/*
 * BLGRPFNT
 */

#include <stddef.h>
#include <io.h>
#include <msxbdos.h>
#include <blstd.h>
#include <blstdvdp.h>
#include <blgrp.h>
#include <blgrpcmd.h>
#include <blgrpfnt.h>
#include <blgrpdat.h>

#ifdef NO_BLGRPFNT
void bl_grp_set_font_size(uint8_t w, uint8_t h)
{
	w = h;
}

void bl_grp_set_font_color(uint8_t fg, uint8_t bg)
{
	fg = bg;
}

void bl_grp_setup_font_draw_func(void)
{
}

#else
uint8_t font_2048[] = {				/* w8 x h8 x 256 or w8 x h16 x 128 */
#include "font_e.h"
};

#ifdef BLGRPFNT_KR
#asm
	psect	text
#endasm
uint8_t font_kr[] = {				/* w16 x h16 x 8x4x4 */
#include "font_kr.h"
};
#asm
	psect	data
#endasm
uint16_t ks2kssm[]={
#include "ks2kssm.h"
};
#endif

uint8_t print_kr_mode = 0;			/* 0:English, 1:KS, 2:KSSM */
uint8_t font_text_mode;
uint8_t font_width_byte;
uint8_t font_work[64];
extern uint8_t font_h16;
extern uint8_t *font_asc;
extern void (*font_func)(uint8_t *font);

void draw_font_null(uint8_t *font);
#asm
	psect	text
_draw_font_null:
	ret
#endasm
void (*font_draw_func_table[2][10])(uint8_t *font) = {
	/* non-interlace mode */
	{
		draw_font_null,			/* T1 */
		draw_font_null,			/* T2 */
#ifdef NO_BLGRPFNT_MC
		draw_font_null,			/* MC */
#else
		bl_draw_font_mc,		/* MC */
#endif
		draw_font_null,			/* G1 */
		draw_font_null,			/* G2 */
		draw_font_null,			/* G3 */
#ifdef NO_BLGRPFNT_G4
		draw_font_null,			/* G4 */
#else
		bl_draw_font_g4,		/* G4 */
#endif
#ifdef NO_BLGRPFNT_G5
		draw_font_null,			/* G5 */
#else
		bl_draw_font_g5,		/* G5 */
#endif
#ifdef NO_BLGRPFNT_G6
		draw_font_null,			/* G6 */
#else
		bl_draw_font_g6,		/* G6 */
#endif
#ifdef NO_BLGRPFNT_G7
		draw_font_null,			/* G7 */
#else
		bl_draw_font_g7			/* G7 */
#endif
	},
	/* interlace mode */
	{
		draw_font_null,			/* T1 */
		draw_font_null,			/* T2 */
#ifdef NO_BLGRPFNT_MC
		draw_font_null,			/* MC */
#else
		bl_draw_font_mc,		/* MC */
#endif
		draw_font_null,			/* G1 */
		draw_font_null,			/* G2 */
		draw_font_null,			/* G3 */
#ifdef NO_BLGRPFNT_G4
		draw_font_null,			/* G4 */
#else
		bl_draw_font_g4i,		/* G4 */
#endif
#ifdef NO_BLGRPFNT_G5
		draw_font_null,			/* G5 */
#else
		bl_draw_font_g5i,		/* G5 */
#endif
#ifdef NO_BLGRPFNT_G6
		draw_font_null,			/* G6 */
#else
		bl_draw_font_g6i,		/* G6 */
#endif
#ifdef NO_BLGRPFNT_G7
		draw_font_null,			/* G7 */
#else
		bl_draw_font_g7i		/* G7 */
#endif
	}
};

void bl_grp_set_font(uint8_t *font)
{
	font_asc = font;
}

#ifdef BLGRPFNT_LD
void bl_grp_load_font(char *filename)
{
	uint8_t fh;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	read(fh, font_2048, 2048);

	close(fh);
}

#ifdef BLGRPFNT_KR
void bl_grp_load_font_kr(char *filename)
{
	uint8_t fh;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	read(fh, font_kr, 11520);

	close(fh);
}
#endif
#endif

static void bl_grp_copy_font_to_pattern_gen(uint16_t addr)
{
	bl_vdp_vram_h = (uint8_t)(addr >> 14);
/*	bl_vdp_vram_h |= bl_grp.active_page_a16_a14;*/
	bl_vdp_vram_m = (uint8_t)((addr >> 8) & 0x3F);
	bl_vdp_vram_l = (uint8_t)addr;
	bl_vdp_vram_cnt = 2048;
	bl_copy_to_vram_nn(font_2048);
}

void bl_grp_setup_text_font(void)
{
	uint16_t vram_addr = bl_grp.pattern_gen_addr;

	switch (bl_grp.screen_mode) {
	case GRP_SCR_T1:		/* only for Pattern based mode */
	case GRP_SCR_T2:
	case GRP_SCR_G1:
		bl_grp_copy_font_to_pattern_gen(vram_addr);
		break;
	case GRP_SCR_G2:
	case GRP_SCR_G3:
		bl_grp_copy_font_to_pattern_gen(vram_addr);
		vram_addr += 2048;
		bl_grp_copy_font_to_pattern_gen(vram_addr);
		vram_addr += 2048;
		bl_grp_copy_font_to_pattern_gen(vram_addr);
		break;
	default:
		break;
	}
}

void bl_grp_setup_font_draw_func(void)
{
	switch (bl_grp.screen_mode) {
	case GRP_SCR_T1:
	case GRP_SCR_T2:
	case GRP_SCR_G1:
	case GRP_SCR_G2:
	case GRP_SCR_G3:
		font_text_mode = 1;
		break;
	default:
		font_text_mode = 0;
		break;
	}

	font_func = font_draw_func_table[bl_grp.interlace_on][bl_grp.screen_mode];
}

void bl_grp_set_font_size(uint8_t w, uint8_t h)
{
	bl_grp.font_width = w & 0xFE;		/* even */
	bl_grp.font_height= h;
	bl_draw_font_w = bl_grp.font_width;
	bl_draw_font_h = bl_grp.font_height;
	font_width_byte = (bl_grp.font_width) >> (bl_grp.bpp_shift);
	font_h16 = (h > 8) ? 0x29 : 0x00;	/* add hl,hl / nop */

	switch (bl_grp.screen_mode) {
	case GRP_SCR_T1:
		bl_grp.text_width = 40;
		break;
	case GRP_SCR_T2:
		bl_grp.text_width = 80;
		break;
	case GRP_SCR_G1:
	case GRP_SCR_G2:
	case GRP_SCR_G3:
		bl_grp.text_width = 32;
		break;
	case GRP_SCR_MC:
		bl_grp.text_width = (uint8_t)(64 / bl_grp.font_width);
		break;
	case GRP_SCR_G5:
	case GRP_SCR_G6:
		bl_grp.text_width = (uint8_t)(512 / bl_grp.font_width);
		break;
	case GRP_SCR_G4:
	case GRP_SCR_G7:
	default:
		bl_grp.text_width = (uint8_t)(256 / bl_grp.font_width);
		break;
	}
}

void bl_grp_set_font_color(uint8_t fg, uint8_t bg)
{
	bl_draw_font_fgc = bl_grp.font_fgc = fg;
	bl_draw_font_bgc = bl_grp.font_bgc = bg;

	/* pre-calculation for G4,G6 */
#ifndef NO_BLGRPFNT_G4
	bl_draw_font_g4c();
#endif
#ifndef NO_BLGRPFNT_G6
	bl_draw_font_g6c();
#endif
}

void bl_grp_set_font_invert(void)
{
	bl_grp_set_font_color(bl_grp.font_bgc, bl_grp.font_fgc);
}

void bl_grp_set_print_kr(uint8_t mode)
{
	print_kr_mode = mode;
}

static uint16_t vram_faddr;
void bl_grp_print_pos(uint16_t x, uint16_t y)
{
	if (font_text_mode) {
		vram_faddr = bl_grp.pattern_name_addr;
		vram_faddr += y * bl_grp.row_byte + x;
		bl_vdp_vram_h = 0;
	} else {
		if (bl_grp.screen_mode == GRP_SCR_MC) {	/* MC */
			vram_faddr = bl_grp.pattern_gen_addr;
			vram_faddr += (y & 0xF8) << 5;		/* (y / 8) * 256 */
			vram_faddr += y & 0x07;
			vram_faddr += (x & 0xFE) << 2;		/* (x / 2) * 8 */
		} else {
			if (bl_grp.interlace_on)
				y >>= 1;

			vram_faddr = y * bl_grp.row_byte;
			vram_faddr += x >> (bl_grp.bpp_shift);
		}

		bl_vdp_vram_h = (uint8_t)(vram_faddr >> 14);
		bl_vdp_vram_h |= bl_grp.active_page_a16_a14;
	}

	bl_vdp_vram_m = (uint8_t)((vram_faddr >> 8) & 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_faddr;
}

#asm
;void bl_grp_print_str(char *str)
	global	_bl_grp_print_str
	global	_font_2048
	global  _bl_vdp_cmd_wait
; for BLOPTIM parser
global	_bl_write_vram

_bl_grp_print_str:
	pop bc			; return addr
	pop de			; char *str
	push de
	push bc

	ld a,(_font_text_mode)
	and a
	jp z,_bl_grp_print_bitmap

_bl_grp_print_pattern:		; for T0, T1, G1, G2, G3
	ld a,(de)
	and a
	ret z			; string end?

	ld l,a
; for BLOPTIM parser
push	hl
call	_bl_write_vram
ld	hl,2
add	hl,sp
ld	sp,hl

_bl_grp_print_pattern_lp:
	inc de
	ld a,(de)
	and a
	ret z			; string end?
	out (098h),a		; write vram
	jp _bl_grp_print_pattern_lp

_bl_grp_print_bitmap:		; for bitmap graphic
	call _bl_vdp_cmd_wait	; wait VDP CMD...

	ld a,(_print_kr_mode)
	and a
	jp nz, _bl_grp_print_bitmap_k
_bl_grp_print_bitmap_l:
	ld a,(de)
	and a
	ret z			; string end?

	push de			; backup str
	ld l,a
	call _bl_grp_draw_w8
	pop de			; restore str
	inc de			; str++
	jp _bl_grp_print_bitmap_l

_bl_grp_print_bitmap_k:		; euc-kr
	ld a,(de)
	and a
	ret z			; string end?
	bit 7,a
	jp z,_bl_grp_draw_k1	; ascii area?

	ld h,a
	inc de			; str++
	ld a,(de)
	ld l,a			; hl = character code
	push de			; backup str
	push hl
	call _bl_grp_make_font_k
	pop hl			; cleanup stack

	ld de,_font_work + 32	; target
	ld hl,_font_work
	call _bl_grp_copy_font_k
	ld hl,_font_work + 1
	call _bl_grp_copy_font_k

	ld hl,_font_work + 32	; left part
	call _font_jp_func
	ld hl,_font_work + 48	; right part
	call _font_jp_func
	pop de			; restore str
	inc de			; str++
	jp _bl_grp_print_bitmap_k

_bl_grp_copy_font_k:		; change order
	ld bc,01040h		; b = 16
_bl_grp_copy_font_l:
	ldi
	inc hl
	djnz _bl_grp_copy_font_l
	ret

_bl_grp_draw_k1:
	push de			; backup str
	cp 008H			; BackSpace
	jr z, _bl_grp_draw_k_bs
	ld l,a
	call _bl_grp_draw_w8
	pop de			; restore str
	inc de			; str++
	jp _bl_grp_print_bitmap_k

_bl_grp_draw_k_bs:
	ld bc,(_font_width_byte)
	ld b,0

	ld hl,(_vram_faddr)
	xor a
	sbc hl,bc
	ld (_vram_faddr),hl
	ld (faddr + 1),hl
	ld hl,(_bl_vdp_vram_l)
	xor a
	sbc hl,bc
	ld (_bl_vdp_vram_l),hl
	ld (vram_l + 1),hl

	ld l,020H
	call _bl_grp_draw_w8

faddr:	ld hl, 0
	ld (_vram_faddr),hl
vram_l:	ld hl, 0
	ld (_bl_vdp_vram_l),hl
	pop de			; restore str
	inc de			; str++
	jp _bl_grp_print_bitmap_k
#endasm

#ifdef BLGRPFNT_KR
static uint8_t table_cho_[21] =
{ 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
static uint8_t table_jung[30] =
{ 0, 0, 0, 1, 2, 3, 4, 5, 0, 0, 6, 7, 8, 9, 10, 11, 0, 0, 12, 13, 14, 15, 16, 17, 0, 0, 18, 19, 20, 21};
static uint8_t table_jong[30] =
{ 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

static uint8_t bul_cho1[22] =
{ 0*20, 0*20, 0*20, 0*20, 0*20, 0*20, 0*20, 0*20, 0*20, 1*20, 3*20,
  3*20, 3*20, 1*20, 2*20, 4*20, 4*20, 4*20, 2*20, 1*20, 3*20, 0*20 };
static uint8_t bul_cho2[22] =
{ 0*20, 5*20, 5*20, 5*20, 5*20, 5*20, 5*20, 5*20, 5*20, 6*20, 7*20,
  7*20, 7*20, 6*20, 6*20, 7*20, 7*20, 7*20, 6*20, 6*20, 7*20, 5*20 };
static uint8_t bul_jong[22] =
{ 0*28, 0*28, 2*28, 0*28, 2*28, 1*28, 2*28, 1*28, 2*28, 3*28, 0*28,
  2*28, 1*28, 3*28, 3*28, 1*28, 2*28, 1*28, 3*28, 3*28, 1*28, 1*28 };

void bl_grp_make_font_k(uint16_t k_code)
{
	static uint16_t font_pos;
	static uint16_t code;
	static uint16_t cho__bul, jung_bul, jong_bul;
	uint8_t cho, jung, jong;
	uint8_t n;

	if (print_kr_mode == 1)			/* 1: Convert KS to KSSM */
		code = ks2kssm[(((k_code >> 8) & 0xFF) - 0xB0) * 94 + (uint8_t)k_code - 0xA1];
	else					/* 2: KSSM */
		code = k_code;

	cho  = table_cho_[(code >> 10) & 0x001F];
	jung = table_jung[(code >> 5) & 0x001F];
	jong = table_jong[code & 0x001F];

	if (!jong) {
		if (!jung) {
			cho__bul = 1 * 20;
			jung_bul = 0 * 22;
		} else {
			cho__bul = bul_cho1[jung];

			if ((cho == 1) || (cho == 16))
				jung_bul = 0 * 22;
			else
				jung_bul = 1 * 22;
		}
	} else {
		if (!jung) {
			cho__bul = 6 * 20;
			jung_bul = 0 * 22;
			jong_bul = 1 * 28;
		} else {
			cho__bul = bul_cho2[jung];

			if ((cho == 1) || (cho == 16))
				jung_bul = 2 * 22;
			else
				jung_bul = 3 * 22;

			jong_bul = bul_jong[jung];
		}
	}

	font_pos = (cho__bul + cho) << 5;
	memcpy(font_work, &font_kr[font_pos], 32);

	font_pos = (160 + jung_bul + jung) << 5;
	for (n = 0; n < 32; n++) {
		font_work[n] |= font_kr[font_pos + n];
	}

	if (jong) {
		font_pos = (160 + 88 + jong_bul + jong) << 5;
		for (n = 0; n < 32; n++) {
			font_work[n] |= font_kr[font_pos + n];
		}
	}
}
#else
#ifdef BLGRPFNT_ROM
#asm
	global	_bl_grp_make_font_k, _bl_rom_call_page1
_bl_grp_make_font_k:			; HL <- k_code
	ld de, _font_work		; DE = font_work
	jp _bl_rom_call_page1
#endasm
#else
void bl_grp_make_font_k(uint16_t k_code)
{
	k_code = k_code;
}
#endif
#endif

void bl_grp_print(uint16_t x, uint16_t y, char *str)
{
	bl_grp_print_pos(x, y);
	bl_grp_print_str(str);
}

#asm
;void bl_grp_print_chr(char chr)
	global	_bl_grp_print_chr

_bl_grp_print_chr:
	pop bc			; return addr
	pop hl			; char chr
	push hl
	push bc

_bl_grp_draw_w8:		; draw font (l = code)
	ld h,0
	add hl,hl
	add hl,hl
	add hl,hl		; font idx = (uint16_t)(*str) * 8
_font_h16:			; uint8_t font_h16; 'add hl,hl' or 'nop'
	add hl,hl		; font idx = (uint16_t)(*str) * 16

	defb 001h		; ld bc,_font_2048
_font_asc:			; uint8_t *font_asc;
	defw _font_2048
	add hl,bc		; hl = font addr
_font_jp_func:
	defb 0c3h		; jp (_font_draw_func)
_font_func:			; void *font_func;
	defw _draw_font_null
#endasm

void bl_grp_print_asc(char asc)
{
	if (asc == '\b')
		bl_grp_print_backspace();
	else
		bl_grp_print_chr(asc);
}

void bl_grp_print_cursor(char chr)
{
	uint16_t vram_faddr_bak = vram_faddr;
	uint8_t bl_vdp_vram_l_bak = bl_vdp_vram_l;

	bl_grp_print_chr(chr);

	vram_faddr = vram_faddr_bak;
	bl_vdp_vram_l = bl_vdp_vram_l_bak;
}

void bl_grp_print_cursor_draw(void)
{
	bl_grp_set_font_invert();
	bl_grp_print_cursor(' ');
	bl_grp_set_font_invert();
}

void bl_grp_print_cursor_erase(void)
{
	bl_grp_print_cursor(' ');
}

void bl_grp_print_backspace(void)
{
	uint8_t addr_delta = font_width_byte;

	vram_faddr -= addr_delta;
	bl_vdp_vram_l -= addr_delta;

	bl_grp_print_chr(0x20);

	vram_faddr -= addr_delta;
	bl_vdp_vram_l -= addr_delta;
}

void bl_grp_print_back(char count)
{
	uint8_t addr_delta = font_width_byte * count;

	vram_faddr -= addr_delta;
	bl_vdp_vram_l -= addr_delta;
}
#endif
