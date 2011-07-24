/*
 * BLGRPINI
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

uint8_t mode_0_1[][2] = {
	{ 0x00, 0x10 },	/* T1 */
	{ 0x04, 0x10 },	/* T2 */
	{ 0x00, 0x08 },	/* MC */
	{ 0x00, 0x00 },	/* G1 */
	{ 0x02, 0x00 },	/* G2 */
	{ 0x04, 0x00 },	/* G3 */
	{ 0x06, 0x00 },	/* G4 */
	{ 0x08, 0x00 },	/* G5 */
	{ 0x0A, 0x00 },	/* G6 */
	{ 0x0E, 0x00 }	/* G7 */
};

uint16_t table_addr[][7] = {
/*      pat_name, color, pat_gen,spr_attr,spr_gen, row, bpp_shift */
	{ 0x0000, 0x0000, 0x0800, 0x0000, 0x0000,    40,   3 },	/* T1 */
	{ 0x0000, 0x0800, 0x1000, 0x0000, 0x0000,    80,   3 },	/* T2 */
	{ 0x0800, 0x0400, 0x0000, 0x1B00, 0x3800,    32,   1 },	/* MC */
	{ 0x1800, 0x2000, 0x0000, 0x1B00, 0x3800,    32,   3 },	/* G1 */
	{ 0x1800, 0x2000, 0x0000, 0x1B00, 0x3800,    32,   3 },	/* G2 */
	{ 0x1800, 0x2000, 0x0000, 0x1E00, 0x3800,    32,   3 },	/* G3 */
	{ 0x0000, 0x7680, 0x0000, 0x7600, 0x7800,   128,   1 },	/* G4 */
	{ 0x0000, 0x7680, 0x0000, 0x7600, 0x7800,   128,   2 },	/* G5 */
	{ 0x0000, 0xFA80, 0x0000, 0xFA00, 0xF000,   256,   1 },	/* G6 */
	{ 0x0000, 0x0000, 0x0000, 0xFA00, 0xF000,   256,   0 }	/* G7 */
};

uint16_t table_pattern_name_fix[] = {
	/* T1      T2      MC      G1      G2      G3      G4      G5      G6      G7 */
	0x0000, 0x0C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x7C00, 0x7C00, 0x7C00, 0x7C00
};

uint16_t table_color_fix[] = {
	/* T1      T2      MC      G1      G2      G3      G4      G5      G6      G7 */
	0x0000, 0x001F, 0x0000, 0x0000, 0x1FC0, 0x1FC0, 0x0000, 0x0000, 0x0000, 0x0000
};

uint16_t table_sprite_attr_fix[] = {
	/* T1      T2      MC      G1      G2      G3      G4      G5      G6      G7 */
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0380, 0x0380, 0x0380, 0x0380
};

uint16_t table_sprite_attr_page[][4] = {
/*	  page 0, page 1, page 2, page 3 */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },	/* T1 */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },	/* T2 */
	{ 0x1B00, 0x1B00, 0x1B00, 0x1B00 },	/* MC */
	{ 0x1B00, 0x1B00, 0x1B00, 0x1B00 },	/* G1 */
	{ 0x1B00, 0x1B00, 0x1B00, 0x1B00 },	/* G2 */
	{ 0x1E00, 0x1E00, 0x1E00, 0x1E00 },	/* G3 */
	{ 0x7600, 0x7200, 0x6E00, 0x6A00 },	/* G4 */
	{ 0x7600, 0x7200, 0x6E00, 0x6A00 },	/* G5 */
	{ 0xFA00, 0xEE00, 0xEA00, 0xE600 },	/* G6 */
	{ 0xFA00, 0xEE00, 0xEA00, 0xE600 }	/* G7 */
};

uint8_t init_8_27[] = {
	0x08, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		/*  8 ~ 15 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x05, 0x00,		/* 16 ~ 23 */
	0x00, 0x00, 0x00, 0x00					/* 24 ~ 27 */
};

uint16_t init_palette[] = {
	/* pal_no << 12, g << 8, r << 4, b */
	0x0000, 0x1000, 0x2611, 0x3733, 0x4117, 0x5327, 0x6151, 0x7617,
	0x8171, 0x9373, 0xA661, 0xB663, 0xC411, 0xD265, 0xE555, 0xF777,
	0xFFFF	/* end mark */
};

static uint16_t vram_addr;
static uint8_t vram_data;

void bl_grp_fnt_init_var(struct bl_grp_var_t *bl_grp_var);
void bl_grp_rc_init_var(struct bl_grp_var_t *bl_grp_var);
int8_t bl_grp_init(void)
{
	uint8_t n;

	bl_grp = (struct bl_grp_var_t *)bl_malloc(sizeof(struct bl_grp_var_t));
	if (bl_grp == NULL)			/* not enough memory? */
		return -1;

	memset(bl_grp, 0x00, sizeof(struct bl_grp_var_t));
	bl_grp->shared_mem = (uint8_t *)bl_malloc(BL_GRP_SHARED_MEM);
	if (bl_grp->shared_mem == NULL) {	/* not enough memory? */
		bl_free(bl_grp);
		return -1;
	}

	bl_grp_fnt_init_var(bl_grp);
	bl_grp_rc_init_var(bl_grp);

	bl_grp->screen_mode = 0xFF;			/* dummy */
	bl_grp->width_L40 = *((uint8_t *)(0xF3AE));	/* LINL40 */
	bl_grp->width_L32 = *((uint8_t *)(0xF3AF));	/* LINL32 */
	bl_grp->width = *((uint8_t *)(0xF3B0));		/* LINLEN */
/*	bl_grp->palette0_on = 0;*/
	bl_grp->line_212 = 1;

	bl_grp->color_text_fg = *((uint8_t *)(0xF3E9));	/* FORCOL */
	bl_grp->color_text_bg = *((uint8_t *)(0xF3EA));	/* BAKCOL */
	bl_grp->color_border = *((uint8_t *)(0xF3EB));	/* BDRCOL */

	bl_grp->font_width = 8;
	bl_grp->font_height = 8;

	/* VDP register shadow */
	bl_grp->reg_shadow_0_7 = (uint8_t *)0xF3DF;
	memcpy(bl_grp->reg_shadow_8_27, init_8_27, sizeof(init_8_27));

	/* Initialize palette */
	memcpy(bl_grp->palette, init_palette, sizeof(init_palette));

	/* Initialize VDP registers */
	for (n = 0; n < 28; n++) {
		if (n < 8)
			bl_write_vdp(n, bl_grp->reg_shadow_0_7[n]);
		else if (n > 24)
			bl_write_vdp(n, bl_grp->reg_shadow_8_27[n - 8]);
	}

	bl_grp->vdp_ver = bl_read_vdp(1) & 0x04 ? GRP_VER_9958 : GRP_VER_9938;

	return bl_grp->vdp_ver;
}

#define CHGMOD		0005FH
#define CALSLT		0001CH
#define EXPTBL		0FCC1H

#asm
;void bl_grp_set_text_mode(void);
		GLOBAL _bl_grp_set_text_mode

_bl_grp_set_text_mode:
		PUSH AF
		PUSH BC
		PUSH DE
		PUSH HL
		PUSH IX
		PUSH IY

		XOR A			; A = Screen Mode 0
		LD IX,CHGMOD
		LD IY,(EXPTBL-1)
		CALL CALSLT
		EI

		POP IY
		POP IX
		POP HL
		POP DE
		POP BC
		POP AF
		RET
#endasm

void bl_grp_deinit(void)
{
	if (bl_grp) {
		bl_free(bl_grp);
		bl_grp_set_text_mode();
	}
}

uint8_t bl_grp_get_vdp_ver(void)
{
	return bl_grp->vdp_ver;
}

void bl_grp_update_reg_bit(uint8_t no, uint8_t mask, uint8_t bits)
{
	uint8_t val;

	if (no < 8) {
		val  = bl_grp->reg_shadow_0_7[no] & (~mask);
		val |= bits;
		bl_write_vdp(no, val);
		bl_grp->reg_shadow_0_7[no] = val;
	} else if (no < 28) {
		val  = bl_grp->reg_shadow_8_27[no - 8] & (~mask);
		val |= bits;
		bl_write_vdp(no, val);
		bl_grp->reg_shadow_8_27[no - 8] = val;
	} else {
		bl_write_vdp(no, bits);		/* write all 8bits */
	}
}

void bl_grp_set_pattern_name_addr(uint16_t addr)
{
	bl_grp->pattern_name_addr = addr;
	addr |= table_pattern_name_fix[bl_grp->screen_mode];
	bl_grp_update_reg_bit( 2, 0x3F, (uint8_t)(addr >> 10));
}

void bl_grp_set_color_addr(uint16_t addr)
{
	bl_grp->color_addr = addr;
	addr |= table_color_fix[bl_grp->screen_mode];
	bl_grp_update_reg_bit( 3, 0xFF, (uint8_t)(addr >> 6));
	bl_grp_update_reg_bit(10, 0x03, (uint8_t)(addr >> 14));
}

void bl_grp_set_pattern_gen_addr(uint16_t addr)
{
	bl_grp->pattern_gen_addr = addr;
	bl_grp_update_reg_bit( 4, 0x1F, (uint8_t)(addr >> 11));
}

void bl_grp_set_sprite_attr_view_addr(uint16_t addr)
{
	bl_grp->sprite_attr_view_addr = addr;
	bl_grp->sprite_color_view_addr = addr - 0x200;
	addr |= table_sprite_attr_fix[bl_grp->screen_mode];
	bl_grp_update_reg_bit( 5, 0xFF, (uint8_t)(addr >> 7));
	bl_grp_update_reg_bit(11, 0x01, (uint8_t)(addr >> 15));
}

void bl_grp_set_sprite_attr_active_addr(uint16_t addr)
{
	bl_grp->sprite_attr_active_addr = addr;
	bl_grp->sprite_color_active_addr = addr - 0x200;
}

void bl_grp_set_sprite_gen_addr(uint16_t addr)
{
	bl_grp->sprite_gen_addr = addr;
	bl_grp_update_reg_bit( 6, 0x1F, (uint8_t)(addr >> 11));
}

void bl_grp_set_screen_mode(uint8_t mode)
{
	bl_grp->screen_mode = mode;

	bl_grp_update_reg_bit( 0, 0x0E, mode_0_1[mode][0]);
	bl_grp_update_reg_bit( 1, 0x18, mode_0_1[mode][1]);

	bl_grp_set_pattern_name_addr(table_addr[mode][0]);
	bl_grp_set_color_addr(table_addr[mode][1]);
	bl_grp_set_pattern_gen_addr(table_addr[mode][2]);
	bl_grp_set_sprite_view_page(0);
	bl_grp_set_sprite_active_page(0);
	bl_grp_set_sprite_gen_addr(table_addr[mode][4]);

	bl_grp_update_palette(bl_grp->palette);

	bl_grp->row_byte = table_addr[mode][5];
	bl_grp->bpp_shift = table_addr[mode][6];

	bl_grp_set_sprite_mode(bl_grp->sprite_mode);
	bl_grp_set_sprite_on(bl_grp->sprite_on);

	bl_grp_set_palette0_on(bl_grp->palette0_on);
	bl_grp_set_line_212(bl_grp->line_212);
	bl_grp_set_display_mode(bl_grp->display_mode);

	bl_grp_set_scroll_mode(GRP_SCROLL_P1);
	bl_grp_set_scroll_hor(0);
	bl_grp_set_scroll_ver(0);

	bl_grp_set_view_page(0);
	bl_grp_set_active_page(0);
	bl_grp_erase_page(0, 0);
}

void bl_grp_set_display_on(uint8_t on)
{
	if (on) {
		bl_grp->display_on = 1;
		on = 0x40;
	} else {
		bl_grp->display_on = 0;
		on = 0x00;
	}
	bl_grp_update_reg_bit(1, 0x40, on);
}

uint8_t bl_grp_get_screen_mode(void)
{
	return bl_grp->screen_mode;
}

uint8_t bl_grp_get_display_on(void)
{
	return bl_grp->display_on;
}

void bl_grp_set_sprite_mode(uint8_t mode)
{
/*	mode &= 0x03;*/
	bl_grp->sprite_mode = mode;
	bl_grp_update_reg_bit(1, 0x03, mode);
}

void bl_grp_set_sprite_on(uint8_t on)
{
	if (on) {
		bl_grp->sprite_on = 1;
		on = 0x00;
	} else {
		bl_grp->sprite_on = 0;
		on = 0x02;
	}
	bl_grp_update_reg_bit(8, 0x02, on);
}

uint8_t bl_grp_get_sprite_mode(void)
{
	return bl_grp->sprite_mode;
}

uint8_t bl_grp_get_sprite_on(void)
{
	return bl_grp->sprite_on;
}

void bl_grp_set_palette0_on(uint8_t on)
{
	if (on) {
		bl_grp->palette0_on = 1;
		on = 0x20;
	} else {
		bl_grp->palette0_on = 0;
		on = 0x00;
	}
	bl_grp_update_reg_bit(8, 0x20, on);
}

void bl_grp_set_line_212(uint8_t on)
{
	if (on) {
		bl_grp->line_212 = 1;
		on = 0x80;
	} else {
		bl_grp->line_212 = 0;
		on = 0x00;
	}
	bl_grp_update_reg_bit(9, 0x80, on);
}

void bl_grp_set_display_mode(uint8_t mode)
{
/*	mode &= 0x0C;*/
	bl_grp->display_mode = mode;
	bl_grp_update_reg_bit(9, 0x0C, mode);
}

uint8_t bl_grp_get_palette0_on(void)
{
	return bl_grp->palette0_on;
}

uint8_t bl_grp_get_line_212(void)
{
	return bl_grp->line_212;
}

uint8_t bl_grp_get_display_mode(void)
{
	return bl_grp->display_mode;
}

void bl_grp_set_color_text_fg(uint8_t color)
{
	bl_grp->color_text_fg = color & 0x0F;
}

void bl_grp_set_color_text_bg(uint8_t color)
{
	bl_grp->color_text_bg = color & 0x0F;
}

void bl_grp_set_color_border(uint8_t color)
{
	if (bl_grp->screen_mode < GRP_SCR_G7) {
		bl_grp->color_border = color & 0x0F;
		bl_grp_update_reg_bit(7, 0x0F, bl_grp->color_border);
	} else {
		bl_grp->color_border = color;
		bl_grp_update_reg_bit(7, 0xFF, bl_grp->color_border);
	}
}

uint8_t bl_grp_get_color_text_fg(void)
{
	return bl_grp->color_text_fg;
}

uint8_t bl_grp_get_color_text_bg(void)
{
	return bl_grp->color_text_bg;
}

uint8_t bl_grp_get_color_border(void)
{
	return bl_grp->color_border;
}

void bl_grp_set_width(uint8_t width)
{
}

void bl_grp_set_view_page(uint8_t page)
{
	if (bl_grp->screen_mode < GRP_SCR_G6)
		page &= 0x03;
	else
		page &= 0x01;

	bl_grp->view_page = page;
	page <<= 5;
	bl_grp_update_reg_bit(2, 0x60, page);
}

void bl_grp_set_active_page(uint8_t page)
{
	bl_grp->active_page = page;
	if (bl_grp->screen_mode < GRP_SCR_G6)
		bl_grp->active_page_a16_a14 = page << 1;	/* 32Kbytes */
	else
		bl_grp->active_page_a16_a14 = page << 2;	/* 64Kbytes */
}

void bl_grp_erase_page(uint8_t page, uint8_t c)
{
	uint16_t n, block;

	if (bl_grp->screen_mode < GRP_SCR_G4) {		/* 16kbytes */
		block = 1;
		bl_vdp_vram_h = page;
	} else if (bl_grp->screen_mode < GRP_SCR_G6) {	/* 32kbytes */
		block = 2;
		bl_vdp_vram_h = page << 1;
	} else {					/* 64kbytes */
		block = 4;
		bl_vdp_vram_h = page << 2;
	}

	for (n = 0; n < block; n++) {
		bl_erase_vram(c);
		bl_vdp_vram_h++;
	}
}

void bl_grp_set_sprite_view_page(uint8_t page)
{
/*	page &= 0x03;*/
	bl_grp->sprite_view_page = page;
	bl_grp_set_sprite_attr_view_addr(table_sprite_attr_page[bl_grp->screen_mode][page]);
}

void bl_grp_set_sprite_active_page(uint8_t page)
{
/*	page &= 0x03;*/
	bl_grp->sprite_active_page = page;
	bl_grp_set_sprite_attr_active_addr(table_sprite_attr_page[bl_grp->screen_mode][page]);
}

void bl_grp_set_scroll_mode(uint8_t mode)
{
	bl_grp->scroll_mode = mode & 0x03;
	bl_grp_update_reg_bit(25, 0x03, bl_grp->scroll_mode);
}

void bl_grp_set_scroll_hor(uint16_t h)
{
	uint8_t r26, r27;

	bl_grp->scroll_h = h;

	r26 = (h >> 3) & 0x3F;
	r27 = h & 0x07;
	if (r27) {
		r26++;
		r27 = 8 - r27;
	}

	bl_grp_update_reg_bit(26, 0xFF, r26);
	bl_grp_update_reg_bit(27, 0xFF, r27);
}

void bl_grp_set_scroll_ver(uint8_t v)
{
	bl_grp->scroll_v = v;
	bl_grp_update_reg_bit(23, 0xFF, v);
}

static uint16_t pal_data;
void bl_grp_set_palette(uint8_t no, uint8_t r, uint8_t g, uint8_t b)
{
	pal_data = (uint16_t)no << 12;
	pal_data |= (uint16_t)g << 8;
	pal_data |= r << 4;
	pal_data |= b;
	bl_grp->palette[no] = pal_data;

	bl_set_palette_(pal_data);
}

static uint16_t pal_r, pal_g, pal_b;
uint16_t bl_grp_get_palette(uint8_t no)
{
	pal_data = bl_grp->palette[no];
	pal_r = (pal_data << 4) & 0x0F00;
	pal_g = (pal_data >> 4) & 0x00F0;
	pal_b = pal_data & 0x000F;

	return (pal_r | pal_g | pal_b);
}

void bl_grp_reset_palette(void)
{
	/* Initialize default palette */
	memcpy(bl_grp->palette, init_palette, sizeof(init_palette));
	bl_grp_update_palette(bl_grp->palette);
}

uint16_t mute_palette[] = {
	/* pal_no << 12, g << 8, r << 4, b */
	0x0000, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000,
	0x8000, 0x9000, 0xA000, 0xB000, 0xC000, 0xD000, 0xE000, 0xF000,
	0xFFFF	/* end mark */
};
void bl_grp_set_palette_mute(uint8_t on)
{
	if (on)
		bl_grp_update_palette(mute_palette);
	else
		bl_grp_update_palette(bl_grp->palette);
}

void bl_grp_set_font_size(uint8_t w, uint8_t h)
{
	bl_grp->font_width = w & 0xFE;		/* even */
	bl_grp->font_height= h;
}

void bl_grp_set_font_color(uint8_t fg, uint8_t bg)
{
	bl_grp->font_fgc = fg;
	bl_grp->font_bgc = bg;
}

void bl_grp_put_pixel(uint16_t x, uint16_t y, uint8_t c)
{
	vram_addr = y * bl_grp->row_byte;
	vram_addr += x >> (bl_grp->bpp_shift);
	bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
	bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_addr;

	switch (bl_grp->bpp_shift) {
	case 1:
		vram_data = bl_read_vram();
		if (x & 0x01) {
			vram_data &= 0xF0;
			vram_data |= c & 0x0F;
		} else {
			vram_data &= 0x0F;
			vram_data |= c << 4;
		}
		bl_write_vram(vram_data);
		break;
	case 2:
		vram_data = bl_read_vram();
		x &= 0x03;
		c &= 0x03;
		if (x == 0) {
			vram_data &= 0x3F;
			c <<= 6;
		} else if (x == 1) {
			vram_data &= 0xCF;
			c <<= 4;
		} else if (x == 2) {
			vram_data &= 0xF3;
			c <<= 2;
		} else {
			vram_data &= 0xFC;
		}
		vram_data |= c;
		bl_write_vram(vram_data);
		break;
	case 3:	/* not yet... */
		vram_data = bl_read_vram();
		bl_write_vram(c);
		break;
	case 0:
	default:
		bl_write_vram(c);
		break;
	}
}

uint8_t bl_grp_get_pixel(uint16_t x, uint16_t y)
{
	vram_addr = y * bl_grp->row_byte;
	vram_addr += x >> (bl_grp->bpp_shift);
	bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
	bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_addr;

	vram_data = bl_read_vram();

	switch (bl_grp->bpp_shift) {
	case 1:
		if (!(x & 0x01)) {
			vram_data >>= 4;
		}
		vram_data &= 0x0F;
		break;
	case 2:
		x &= 0x03;
		if (x == 0) {
			vram_data >>= 6;
		} else if (x == 1) {
			vram_data >>= 4;
		} else if (x == 2) {
			vram_data >>= 2;
		}
		vram_data &= 0x03;
		break;
	case 3:	/* not yet... */
		break;
	case 0:
	default:
		break;
	}

	return vram_data;
}

uint16_t bl_grp_get_vramaddr_spr_gen(uint16_t no)
{
	return (bl_grp->sprite_gen_addr + (no << 3));
}

uint16_t bl_grp_get_vramaddr_spr_col(uint16_t no)
{
	return (bl_grp->sprite_color_active_addr + (no << 4));
}

uint16_t bl_grp_get_vramaddr_spr_attr(uint16_t layer)
{
	return (bl_grp->sprite_attr_active_addr + (layer << 2));
}

static uint8_t spr_attr[4];
void bl_grp_clear_sprite(void)
{
	uint16_t vram_addr = bl_grp->sprite_attr_active_addr;
	uint8_t n;

	spr_attr[0] = 212 + bl_grp->scroll_v;
	spr_attr[1] = 0;
	spr_attr[2] = 0xFF;
	spr_attr[3] = 0;

	for (n = 0; n < 32; n++, vram_addr += 4) {
		bl_set_vram_addr16(vram_addr);
		bl_copy_to_vram_4(spr_attr);
	}
}

void bl_grp_put_sprite(uint16_t layer, uint8_t x, uint8_t y, uint8_t c, uint8_t no)
{
	spr_attr[0] = y + bl_grp->scroll_v - 1;
	spr_attr[1] = x;
	spr_attr[2] = (bl_grp->sprite_mode < GRP_SPR_16) ? no : no << 2;
	spr_attr[3] = c;

	bl_set_vram_addr16(bl_grp->sprite_attr_active_addr + (layer << 2));
	bl_copy_to_vram_4(spr_attr);
}

static uint8_t hmmm_cmd[15] = {
	0x00,	/* R32 SX low */
	0x00,	/* R33 SX high */
	0x00,	/* R34 SY low */
	0x00,	/* R35 SY high */
	0x00,	/* R36 DX low */
	0x00,	/* R37 DX high */
	0x00,	/* R38 DY low */
	0x00,	/* R39 DY high */
	0x00,	/* R40 Width low */
	0x00,	/* R41 Width high */
	0x00,	/* R42 Height low */
	0x00,	/* R43 Height high */
	0x00,	/* R44 Color */
	0x00,	/* R45 Argument */
	0xD0	/* R46 Command HMMM */
};

void bl_grp_hcopy_v2v(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t dx, uint16_t dy)
{
	*(uint16_t *)(&hmmm_cmd[0]) = sx;
	hmmm_cmd[2] = (uint8_t)sy;
	hmmm_cmd[3] = bl_grp->active_page;

	*(uint16_t *)(&hmmm_cmd[4]) = dx;
	hmmm_cmd[6] = (uint8_t)dy;
	hmmm_cmd[7] = bl_grp->active_page;

	*(uint16_t *)(&hmmm_cmd[8]) = w;
	*(uint16_t *)(&hmmm_cmd[10]) = h;

	bl_vdp_cmd_mmm(hmmm_cmd);
}

void bl_grp_hcopy_v2v_p(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t dx, uint16_t dy, uint8_t src)
{
	*(uint16_t *)(&hmmm_cmd[0]) = sx;
	hmmm_cmd[2] = (uint8_t)sy;
	hmmm_cmd[3] = src;

	*(uint16_t *)(&hmmm_cmd[4]) = dx;
	hmmm_cmd[6] = (uint8_t)dy;
	hmmm_cmd[7] = bl_grp->active_page;

	*(uint16_t *)(&hmmm_cmd[8]) = w;
	*(uint16_t *)(&hmmm_cmd[10]) = h;

	bl_vdp_cmd_mmm(hmmm_cmd);
}

static uint8_t lmmm_cmd[15] = {
	0x00,	/* R32 SX low */
	0x00,	/* R33 SX high */
	0x00,	/* R34 SY low */
	0x00,	/* R35 SY high */
	0x00,	/* R36 DX low */
	0x00,	/* R37 DX high */
	0x00,	/* R38 DY low */
	0x00,	/* R39 DY high */
	0x00,	/* R40 Width low */
	0x00,	/* R41 Width high */
	0x00,	/* R42 Height low */
	0x00,	/* R43 Height high */
	0x00,	/* R44 Color */
	0x00,	/* R45 Argument */
	0x90	/* R46 Command LMMM */
};

void bl_grp_lcopy_v2v(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t dx, uint16_t dy, uint8_t op)
{
	*(uint16_t *)(&lmmm_cmd[0]) = sx;
	lmmm_cmd[2] = (uint8_t)sy;
	lmmm_cmd[3] = bl_grp->active_page;

	*(uint16_t *)(&lmmm_cmd[4]) = dx;
	lmmm_cmd[6] = (uint8_t)dy;
	lmmm_cmd[7] = bl_grp->active_page;

	*(uint16_t *)(&lmmm_cmd[8]) = w;
	*(uint16_t *)(&lmmm_cmd[10]) = h;

	lmmm_cmd[14] = 0x90 | op;

	bl_vdp_cmd_mmm(lmmm_cmd);
}

void bl_grp_lcopy_v2v_p(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t dx, uint16_t dy, uint8_t src, uint8_t op)
{
	*(uint16_t *)(&lmmm_cmd[0]) = sx;
	lmmm_cmd[2] = (uint8_t)sy;
	lmmm_cmd[3] = src;

	*(uint16_t *)(&lmmm_cmd[4]) = dx;
	lmmm_cmd[6] = (uint8_t)dy;
	lmmm_cmd[7] = bl_grp->active_page;

	*(uint16_t *)(&lmmm_cmd[8]) = w;
	*(uint16_t *)(&lmmm_cmd[10]) = h;

	lmmm_cmd[14] = 0x90 | op;

	bl_vdp_cmd_mmm(lmmm_cmd);
}

static uint8_t line_cmd[11] = {
	0x00,	/* R36 DX low */
	0x00,	/* R37 DX high */
	0x00,	/* R38 DY low */
	0x00,	/* R39 DY high */
	0x00,	/* R40 Major low */
	0x00,	/* R41 Major high */
	0x00,	/* R42 Minor low */
	0x00,	/* R43 Minor high */
	0x00,	/* R44 Color */
	0x00,	/* R45 Argument */
	0x70	/* R46 Command LINE */
};
static uint16_t line_w, line_h;
void bl_grp_line(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op)
{
	*(uint16_t *)(&line_cmd[0]) = sx;
	line_cmd[2] = (uint8_t)sy;
	line_cmd[3] = bl_grp->active_page;

	if (sx < dx) {			/* to right */
		line_cmd[9] = 0x00;
		line_w = dx - sx;
	} else {			/* to left */
		line_cmd[9] = 0x04;
		line_w = sx - dx;
	}

	if (sy < dy) {			/* to down */
		line_h = dy - sy;
	} else {			/* to up */
		line_cmd[9] |= 0x08;
		line_h = sy - dy;
	}

	if (line_w > line_h) {
		*(uint16_t *)(&line_cmd[4]) = line_w;
		*(uint16_t *)(&line_cmd[6]) = line_h;
	} else {
		line_cmd[9] |= 0x01;
		*(uint16_t *)(&line_cmd[4]) = line_h;
		*(uint16_t *)(&line_cmd[6]) = line_w;
	}

	line_cmd[8] = c;
	line_cmd[10] = 0x70 | op;

	bl_vdp_cmd_line(line_cmd);
}

static uint16_t box_w, box_h, box_cnt, box_x, box_y;
void bl_grp_box(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op)
{
	bl_grp_line(sx, sy, dx, sy, c, op);
	bl_grp_line(sx, sy, sx, dy, c, op);
	bl_grp_line(dx, sy, dx, dy, c, op);
	bl_grp_line(sx, dy, dx, dy, c, op);
}

void bl_grp_boxfill(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op)
{
	if (sx < dx) {			/* to right */
		box_w = dx - sx;
		box_x = sx;
	} else {			/* to left */
		box_w = sx - dx;
		box_x = dx;
	}

	if (sy < dy) {			/* to down */
		box_h = dy - sy;
		box_y = sy;
	} else {			/* to up */
		box_h = sy - dy;
		box_y = dy;
	}

	if (box_w >= box_h) {
		for (box_cnt = 0; box_cnt <= box_h; box_cnt++, box_y++)
			bl_grp_line(sx, box_y, dx, box_y, c, op);
	} else {
		for (box_cnt = 0; box_cnt <= box_w; box_cnt++, box_x++)
			bl_grp_line(box_x, sy, box_x, dy, c, op);
	}
}

void bl_grp_write_vram(uint8_t *src, uint16_t y, uint16_t size)
{
	vram_addr = y * bl_grp->row_byte;
/*	vram_addr += x >> (bl_grp->bpp_shift);*/

	bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
	bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_addr;
	bl_vdp_vram_cnt = size;
	bl_copy_to_vram_nn(src);
}

void bl_grp_read_vram(uint8_t *dest, uint16_t y, uint16_t size)
{
	vram_addr = y * bl_grp->row_byte;
/*	vram_addr += x >> (bl_grp->bpp_shift);*/

	bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
	bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_addr;
	bl_vdp_vram_cnt = size;
	bl_copy_from_vram_nn(dest);
}

;