/*
 * BLGRPFNT
 */

#include <stdio.h>
#include <string.h>
#include <msxio.h>
#include <msxbdos.h>
#include <blstd.h>
#include <blstdvdp.h>
#include <blgrp.h>
#include <blgrpcmd.h>
#include <blgrpfnt.h>
#include <blgrpdat.h>

static struct bl_grp_var_t *bl_grp = NULL;

uint8_t font_8x8_org[] = {
#include "font_e.h"
};

uint8_t font_8x8_center[] = {
/*#include "font_e.h"*/
0,
};

uint8_t font_8x8_bold[] = {
/*#include "font_e.h"*/
0,
};

uint8_t font_8x8_wide[] = {
/*#include "font_e.h"*/
0,
};

uint8_t *font_8x8;
uint16_t font_8x8_len = sizeof(font_8x8_org);
uint8_t font_8x8_init = 0;

void bl_grp_fnt_init_var(struct bl_grp_var_t *bl_grp_var)
{
#if 0
	uint16_t n;
	uint8_t *org, *center, *bold, *wide;
#endif
	bl_grp = bl_grp_var;
	font_8x8 = font_8x8_org;
#if 0
	if (!font_8x8_init) {
		font_8x8_init = 1;			/* init once! */
		org = font_8x8_org;
		center = font_8x8_center;
		bold = font_8x8_bold;
		wide = font_8x8_wide;
		for (n = 0; n < font_8x8_len; n++) {
			*center >>= 1;
			*bold = *center | (*center >> 1);
			*wide = (*center & 0xF0) | ((*center & 0x1E) >> 1);

			org++;
			center++;
			bold++;
			wide++;
		}
	}
#endif
}

void bl_grp_load_font(char *filename)
{
	FILE *fp;

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return;

	fread(font_8x8_org, 2048, 1, fp);

	fclose(fp);
}

void bl_grp_set_font_style(uint8_t style)
{
	switch (style) {
	case GRP_FONT_CENTER:
		font_8x8 = font_8x8_center;
		break;
	case GRP_FONT_BOLD:
		font_8x8 = font_8x8_bold;
		break;
	case GRP_FONT_WIDE:
		font_8x8 = font_8x8_wide;
		break;
	case GRP_FONT_ORGIN:
	default:
		font_8x8 = font_8x8_org;
		break;
	}
}

void bl_grp_set_font_size(uint8_t w, uint8_t h)
{
	bl_grp->font_width = w & 0xFE;		/* even */
	bl_grp->font_height= h;
	bl_draw_font_w = bl_grp->font_width;
	bl_draw_font_h = bl_grp->font_height;
}

void bl_grp_set_font_color(uint8_t fg, uint8_t bg)
{
	bl_grp->font_fgc = fg;
	bl_grp->font_bgc = bg;
	bl_draw_font_fgc = bl_grp->font_fgc;
	bl_draw_font_bgc = bl_grp->font_bgc;
}

static uint16_t vram_faddr, fcode_idx;
void bl_grp_print_pos(uint16_t x, uint16_t y)
{
	vram_faddr = y * bl_grp->row_byte;
	vram_faddr += x >> (bl_grp->bpp_shift);
	bl_vdp_vram_h = (uint8_t)(vram_faddr >> 14);
	bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((vram_faddr >> 8)& 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_faddr;
}

void bl_grp_print_str(char *str)
{
	switch (bl_grp->screen_mode) {
	case GRP_SCR_G4:
		while (*str) {
			fcode_idx = (uint16_t)(*str) << 3;
			bl_draw_font_g4(font_8x8 + fcode_idx);
			str++;
		}
		break;
	case GRP_SCR_G5:
		while (*str) {
			fcode_idx = (uint16_t)(*str) << 3;
			bl_draw_font_g5(font_8x8 + fcode_idx);
			str++;
		}
		break;
	case GRP_SCR_G6:
		while (*str) {
			fcode_idx = (uint16_t)(*str) << 3;
			bl_draw_font_g6(font_8x8 + fcode_idx);
			str++;
		}
		break;
	case GRP_SCR_G7:
		while (*str) {
			fcode_idx = (uint16_t)(*str) << 3;
			bl_draw_font_g7(font_8x8 + fcode_idx);
			str++;
		}
		break;
	default:
		break;
	}
}

void bl_grp_print(uint16_t x, uint16_t y, char *str)
{
	bl_grp_print_pos(x, y);
	bl_grp_print_str(str);
}

void bl_grp_print_chr(char chr)
{
	fcode_idx = (uint16_t)chr << 3;

	switch (bl_grp->screen_mode) {
	case GRP_SCR_G4:
		bl_draw_font_g4(font_8x8 + fcode_idx);
		break;
	case GRP_SCR_G5:
		bl_draw_font_g5(font_8x8 + fcode_idx);
		break;
	case GRP_SCR_G6:
		bl_draw_font_g6(font_8x8 + fcode_idx);
		break;
	case GRP_SCR_G7:
		bl_draw_font_g7(font_8x8 + fcode_idx);
		break;
	default:
		break;
	}
}

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
	uint8_t addr_delta = (bl_grp->font_width) >> (bl_grp->bpp_shift);
	addr_delta <<= 1;

	bl_grp_print_chr(0x20);

	vram_faddr -= addr_delta;
	bl_vdp_vram_l -= addr_delta;
}

void bl_grp_print_back(char count)
{
	uint8_t addr_delta = (bl_grp->font_width) >> (bl_grp->bpp_shift);

	addr_delta = addr_delta * count;
	vram_faddr -= addr_delta;
	bl_vdp_vram_l -= addr_delta;
}

;