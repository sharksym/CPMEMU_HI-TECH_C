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
#include <blgrpdat.h>

static struct bl_grp_var_t *bl_grp = NULL;

uint8_t font_8x8[] = {
#include "FONT_E.H"
};

void bl_grp_fnt_init_var(struct bl_grp_var_t *bl_grp_var)
{
	bl_grp = bl_grp_var;
}

static uint16_t vram_faddr, fcode_idx;
void bl_grp_print(uint16_t x, uint16_t y, char *str)
{
	bl_draw_font_w = bl_grp->font_width;
	bl_draw_font_h = bl_grp->font_height;
	bl_draw_font_fgc = bl_grp->font_fgc;
	bl_draw_font_bgc = bl_grp->font_bgc;

	vram_faddr = y * bl_grp->row_byte;
	vram_faddr += x >> (bl_grp->bpp_shift);
	bl_vdp_vram_h = (uint8_t)(vram_faddr >> 14);
	bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((vram_faddr >> 8)& 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_faddr;

	bl_vdp_cmd_wait();

	switch (bl_grp->screen_mode) {
	case GRP_SCR_G4:
		while (*str) {
			fcode_idx = (uint16_t)(*str) << 3;
			bl_draw_font_g4(&font_8x8[fcode_idx]);
			str++;
		}
		break;
	case GRP_SCR_G5:
		while (*str) {
			fcode_idx = (uint16_t)(*str) << 3;
			bl_draw_font_g5(&font_8x8[fcode_idx]);
			str++;
		}
		break;
	case GRP_SCR_G6:
		while (*str) {
			fcode_idx = (uint16_t)(*str) << 3;
			bl_draw_font_g6(&font_8x8[fcode_idx]);
			str++;
		}
		break;
	case GRP_SCR_G7:
		while (*str) {
			fcode_idx = (uint16_t)(*str) << 3;
			bl_draw_font_g7(&font_8x8[fcode_idx]);
			str++;
		}
		break;
	default:
		break;
	}
}

;