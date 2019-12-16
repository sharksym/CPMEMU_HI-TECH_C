/*
 * BLGRPFNT
 */

#include <stddef.h>
#include <stdio.h>
/*#include <string.h>*/
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
uint8_t font_kr[] = {				/* w16 x h16 x 8x4x4 */
#include "font_kr.h"
};
#endif

void draw_font_null(uint8_t *font)
{
}

uint8_t font_text_mode;
uint8_t font_width_byte;
extern uint8_t font_h16_1, font_h16_2;
extern uint8_t *font_asc_1, *font_asc_2;
extern void (*font_func_1)(uint8_t *font), (*font_func_2)(uint8_t *font);

void (*font_draw_func_table[2][10])(uint8_t *font) = {
	/* non-interlace mode */
	{
		draw_font_null,			/* T1 */
		draw_font_null,			/* T2 */
		bl_draw_font_mc,		/* MC */
		draw_font_null,			/* G1 */
		draw_font_null,			/* G2 */
		draw_font_null,			/* G3 */
		bl_draw_font_g4,		/* G4 */
		bl_draw_font_g5,		/* G5 */
		bl_draw_font_g6,		/* G6 */
		bl_draw_font_g7			/* G7 */
	},
	/* interlace mode */
	{
		draw_font_null,			/* T1 */
		draw_font_null,			/* T2 */
		bl_draw_font_mc,		/* MC */
		draw_font_null,			/* G1 */
		draw_font_null,			/* G2 */
		draw_font_null,			/* G3 */
		bl_draw_font_g4i,		/* G4 */
		bl_draw_font_g5i,		/* G5 */
		bl_draw_font_g6i,		/* G6 */
		bl_draw_font_g7i		/* G7 */
	}
};

void bl_grp_set_font(uint8_t *font)
{
	font_asc_1 = font;
	font_asc_2 = font;
}

void bl_grp_load_font(char *filename)
{
	uint8_t fh;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	read(fh, font_2048, 2048);

	close(fh);
}

void bl_grp_load_font_kr(char *filename)
{
#ifdef BLGRPFNT_KR
	uint8_t fh;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	read(fh, font_kr, 11520);

	close(fh);
#else
	filename = filename;
#endif
}

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

	font_func_1 = font_func_2 =
		font_draw_func_table[bl_grp.interlace_on][bl_grp.screen_mode];
}

void bl_grp_set_font_size(uint8_t w, uint8_t h)
{
	bl_grp.font_width = w & 0xFE;		/* even */
	bl_grp.font_height= h;
	bl_draw_font_w = bl_grp.font_width;
	bl_draw_font_h = bl_grp.font_height;
	font_width_byte = (bl_grp.font_width) >> (bl_grp.bpp_shift);

	if (h > 8) {				/* add hl,hl */
		font_h16_1 = 0x29;
		font_h16_2 = 0x29;
	} else {				/* nop */
		font_h16_1 = 0x00;
		font_h16_2 = 0x00;
	}

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
	bl_draw_font_g4c();
	bl_draw_font_g6c();
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
	ld a,(de)
	and a
	ret z			; string end?
	push de			; backup str

	ld h,0
	ld l,a
	add hl,hl
	add hl,hl
	add hl,hl		; font idx = (uint16_t)(*str) * 8
_font_h16_1:			; uint8_t font_h16_1; 'add hl,hl' or 'nop'
	add hl,hl		; font idx = (uint16_t)(*str) * 16

	defb 001h		; ld bc,_font_2048
_font_asc_1:			; uint8_t *font_asc_1;
	defw _font_2048
	add hl,bc		; hl = font addr

	defb 0cdh		; call (_font_draw_func)
_font_func_1:			; void *font_func_1;
	defw _draw_font_null

	pop de			; restore str
	inc de			; str++
	jp _bl_grp_print_bitmap
#endasm

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

	ld h,0
	add hl,hl
	add hl,hl
	add hl,hl		; font idx = (uint16_t)(*str) * 8
_font_h16_2:			; uint8_t font_h16_2; 'add hl,hl' or 'nop'
	add hl,hl		; font idx = (uint16_t)(*str) * 16

	defb 001h		; ld bc,_font_2048
_font_asc_2:			; uint8_t *font_asc_2;
	defw _font_2048
	add hl,bc		; hl = font addr

	defb 0c3h		; jp (_font_draw_func)
_font_func_2:			; void *font_func_2;
	defw _draw_font_null
#endasm

void bl_grp_print_cursor(void)
{
	uint16_t vram_faddr_bak = vram_faddr;
	uint8_t bl_vdp_vram_l_bak = bl_vdp_vram_l;

	bl_grp_print_chr(0xDB);

	vram_faddr = vram_faddr_bak;
	bl_vdp_vram_l = bl_vdp_vram_l_bak;
}

void bl_grp_print_backspace(void)
{
	uint8_t addr_delta = font_width_byte << 1;

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
;
