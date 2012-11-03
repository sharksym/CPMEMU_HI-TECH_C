/*
 * BLGRP
 */

/*#include <stdio.h>*/
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

uint16_t table_sprite_attr_page[][8] = {
/*	  page 0, page 1, page 2, page 3, page 4, page 5, page 6, page 7 */
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },	/* T1 */
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },	/* T2 */
	{ 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00 },	/* MC */
	{ 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00 },	/* G1 */
	{ 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00, 0x1B00 },	/* G2 */
	{ 0x1E00, 0x1E00, 0x1E00, 0x1E00, 0x1E00, 0x1E00, 0x1E00, 0x1E00 },	/* G3 */
	{ 0x7600, 0x7200, 0x6E00, 0x6A00, 0x6600, 0x6200, 0x5E00, 0x5A00 },	/* G4 */
	{ 0x7600, 0x7200, 0x6E00, 0x6A00, 0x6600, 0x6200, 0x5E00, 0x5A00 },	/* G5 */
	{ 0xFA00, 0xEE00, 0xEA00, 0xE600, 0xE200, 0xDE00, 0xDA00, 0xD600 },	/* G6 */
	{ 0xFA00, 0xEE00, 0xEA00, 0xE600, 0xE200, 0xDE00, 0xDA00, 0xD600 }	/* G7 */
};

uint16_t table_sprite_gen_page[][8] = {
/*	  page 0, page 1, page 2, page 3, page 4, page 5, page 6, page 7 */
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },	/* T1 */
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },	/* T2 */
	{ 0x3800, 0x3000, 0x2800, 0x2000, 0x1800, 0x1000, 0x0800, 0x0000 },	/* MC */
	{ 0x3800, 0x3000, 0x2800, 0x2000, 0x1800, 0x1000, 0x0800, 0x0000 },	/* G1 */
	{ 0x3800, 0x3000, 0x2800, 0x2000, 0x1800, 0x1000, 0x0800, 0x0000 },	/* G2 */
	{ 0x3800, 0x3000, 0x2800, 0x2000, 0x1800, 0x1000, 0x0800, 0x0000 },	/* G3 */
	{ 0x7800, 0x7000, 0x6800, 0x6000, 0x5800, 0x5000, 0x4800, 0x4000 },	/* G4 */
	{ 0x7800, 0x7000, 0x6800, 0x6000, 0x5800, 0x5000, 0x4800, 0x4000 },	/* G5 */
	{ 0xF000, 0xE800, 0xE000, 0xD800, 0xD000, 0xC800, 0xC000, 0xB800 },	/* G6 */
	{ 0xF000, 0xE800, 0xE000, 0xD800, 0xD000, 0xC800, 0xC000, 0xB800 }	/* G7 */
};

uint16_t init_palette[] = {
	/* pal_no << 12, g << 8, r << 4, b */
	0x0000, 0x1000, 0x2611, 0x3733, 0x4117, 0x5327, 0x6151, 0x7617,
	0x8171, 0x9373, 0xA661, 0xB663, 0xC411, 0xD265, 0xE555, 0xF777,
	0xFFFF	/* end mark */
};

void bl_grp_cmd_init_var(struct bl_grp_var_t *bl_grp_var);
void bl_grp_fnt_init_var(struct bl_grp_var_t *bl_grp_var);
void bl_grp_rc_init_var(struct bl_grp_var_t *bl_grp_var);
int8_t bl_grp_init(void)
{
	bl_grp = (struct bl_grp_var_t *)bl_malloc(sizeof(struct bl_grp_var_t));
	if (bl_grp == NULL)			/* not enough memory? */
		return -1;

	memset(bl_grp, 0x00, sizeof(struct bl_grp_var_t));
	bl_grp->shared_mem = (uint8_t *)bl_malloc(BL_GRP_SHARED_MEM);
	if (bl_grp->shared_mem == NULL) {	/* not enough memory? */
		bl_free(bl_grp);
		return -1;
	}

	bl_grp_cmd_init_var(bl_grp);
	bl_grp_fnt_init_var(bl_grp);
	bl_grp_rc_init_var(bl_grp);

	bl_grp->screen_mode = 0xFF;			/* dummy */
	bl_grp->width_L40 = *((uint8_t *)(0xF3AE));	/* LINL40 */
	bl_grp->width_L32 = *((uint8_t *)(0xF3AF));	/* LINL32 */
	bl_grp->width = *((uint8_t *)(0xF3B0));		/* LINLEN */
	bl_grp->palette0_on = 0;
	bl_grp->line_212 = 1;
	bl_grp->display_mode = GRP_DISP_240P;
	bl_grp->interlace_on = 0;

	bl_grp->color_text_fg = *((uint8_t *)(0xF3E9));	/* FORCOL */
	bl_grp->color_text_bg = *((uint8_t *)(0xF3EA));	/* BAKCOL */
	bl_grp->color_border = *((uint8_t *)(0xF3EB));	/* BDRCOL */

	bl_grp->adjust_h = 0;
	bl_grp->adjust_v = 0;

	bl_grp->scroll_mode = GRP_SCROLL_P1;
	bl_grp->scroll_h = 0;
	bl_grp->scroll_v = 0;

	bl_grp_set_font_size(8, 8);
	bl_grp_set_font_color(15, 0);

	/* VDP register shadow */
	bl_grp->reg_shadow = (uint8_t *)GRP_REG_SHADOW_ADDR;
	memcpy(&bl_grp->reg_shadow[0], (uint8_t *)0xF3DF, 8);	/* 0 ~ 7 */
	memcpy(&bl_grp->reg_shadow[8], (uint8_t *)0xFFE7, 16);	/* 8 ~ 23 */
	bl_grp->reg_shadow[24] = 0;				/* 24 */
	memcpy(&bl_grp->reg_shadow[25], (uint8_t *)0xFFFA, 3);	/* 25 ~ 27 */

	/* Initialize palette */
	memcpy(bl_grp->palette, init_palette, sizeof(init_palette));

	/* VDP version */
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
		bl_grp_set_palette0_on(0);		/* disable palette 0 */
		bl_grp_set_line_212(1);
		bl_grp_set_adjust_hor(0);
		bl_grp_set_adjust_ver(0);
		bl_grp_set_scroll_hor(0);
		bl_grp_set_scroll_ver(0);
		bl_grp_set_view(0);
		bl_grp_set_active(0);
		bl_grp_reset_palette();

		bl_grp_set_text_mode();
		bl_free(bl_grp);
	}
}

uint8_t bl_grp_get_vdp_ver(void)
{
	return bl_grp->vdp_ver;
}

#if 1	/* ASM version */
#define bl_grp_update_reg_bit(no, mask, bits)	\
	update_reg_bit(((uint16_t)no << 8) | (uint8_t)(~mask), bits)
#asm
;void _update_reg_bit(uint16_t no_mask, uint8_t bits)
		GLOBAL _update_reg_bit
_update_reg_bit:
		POP HL			; return address
		POP DE			; D = no, E = mask (inverted)
		POP BC			; C = bits
		PUSH BC
		PUSH DE
		PUSH HL

		LD H,083H		; VDP shadow register addr high
		LD L,D			; VDP shadow register addr low
		LD A,(HL)		; Read old value
		AND E			; Mask bits
		OR C			; Set value

		DI
		OUT (099H),A		; Write value
		LD (HL),A		; Update shadow register
		LD A,D
		OR 080H
		OUT (099H),A		; Write register no.
		EI

		RET
#endasm
#else	/* C version */
void bl_grp_update_reg_bit(uint8_t no, uint8_t mask, uint8_t bits)
{
	uint8_t val;

	val  = bl_grp->reg_shadow[no] & (~mask);
	val |= bits;
	bl_write_vdp(no, val);
	bl_grp->reg_shadow[no] = val;
}
#endif

void bl_grp_set_irq_vblank(uint8_t on)
{
	bl_grp_update_reg_bit( 1, 0x20, on ? 0x20 : 0x00);
}

void bl_grp_set_irq_hblank(uint8_t on)
{
	bl_grp_update_reg_bit( 0, 0x10, on ? 0x10 : 0x00);
}

void bl_grp_set_irq_hblank_line(uint8_t line)
{
	bl_grp_update_reg_bit(19, 0xFF, line);
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

void bl_grp_set_sprite_gen_view_addr(uint16_t addr)
{
	bl_grp->sprite_gen_view_addr = addr;
	bl_grp_update_reg_bit( 6, 0x1F, (uint8_t)(addr >> 11));
}

void bl_grp_set_sprite_gen_active_addr(uint16_t addr)
{
	bl_grp->sprite_gen_active_addr = addr;
}

void bl_grp_set_screen_mode(uint8_t mode)
{
	bl_grp->screen_mode = mode;

	bl_grp_update_reg_bit( 0, 0x0E, mode_0_1[mode][0]);
	bl_grp_update_reg_bit( 1, 0x18, mode_0_1[mode][1]);

	bl_grp_set_pattern_name_addr(table_addr[mode][0]);
	bl_grp_set_color_addr(table_addr[mode][1]);
	bl_grp_set_pattern_gen_addr(table_addr[mode][2]);
	bl_grp_set_sprite_view(0);
	bl_grp_set_sprite_active(0);
	bl_grp_set_sprite_gen_view(0);
	bl_grp_set_sprite_gen_active(0);

	bl_grp_update_palette(bl_grp->palette);

	bl_grp->row_byte = table_addr[mode][5];
	bl_grp->bpp_shift = table_addr[mode][6];

	bl_grp_set_sprite_mode(bl_grp->sprite_mode);
	bl_grp_set_sprite_on(bl_grp->sprite_on);

	bl_grp_set_palette0_on(bl_grp->palette0_on);
	bl_grp_set_line_212(bl_grp->line_212);
	bl_grp_set_display_mode(bl_grp->display_mode);

	bl_grp_set_adjust_hor(bl_grp->adjust_h);
	bl_grp_set_adjust_ver(bl_grp->adjust_v);

	bl_grp_set_scroll_mode(bl_grp->scroll_mode);
	bl_grp_set_scroll_hor(bl_grp->scroll_h);
	bl_grp_set_scroll_ver(bl_grp->scroll_v);

	bl_grp_set_view(0);
	bl_grp_set_active(0);
	bl_grp_erase(0, 0);
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
/*	mode &= 0x0E;*/
	bl_grp->display_mode = mode;
	bl_grp_update_reg_bit(9, 0x0E, mode);

	if ((bl_grp->display_mode & GRP_DISP_IL_E0) == GRP_DISP_IL_E0)
		bl_grp->interlace_on = 1;
	else
		bl_grp->interlace_on = 0;
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

void bl_grp_set_view(uint8_t page)
{
	if (bl_grp->interlace_on) {
		page &= 0x02;
		bl_grp->view_page = page;
		page++;					/* page 1 or 3 */
	} else {
		bl_grp->view_page = page;
	}
	page <<= 5;
	bl_grp_update_reg_bit(2, 0x60, page);
}

void bl_grp_set_active(uint8_t page)
{
	if (bl_grp->interlace_on)
		page &= 0x02;				/* page 0 or 2 */

	bl_grp->active_page = page;
	if (bl_grp->screen_mode < GRP_SCR_G6)
		bl_grp->active_page_a16_a14 = page << 1;	/* 32Kbytes */
	else
		bl_grp->active_page_a16_a14 = page << 2;	/* 64Kbytes */
}

static uint8_t hmmv_cmd[11] = {
	0x00,	/* R36 DX low */
	0x00,	/* R37 DX high */
	0x00,	/* R38 DY low */
	0x00,	/* R39 DY high */
	0x00,	/* R40 Width low */
	0x00,	/* R41 Width high */
	0x00,	/* R42 Height low */
	0x00,	/* R43 Height high */
	0x00,	/* R44 Byte Color */
	0x00,	/* R45 Argument */
	0xC0	/* R46 Command HMMV */
};
void bl_grp_erase(uint8_t page, uint8_t c)
{
	uint16_t n, block;
	uint16_t width, height;

	if (bl_grp->interlace_on)
		page &= 0x02;				/* page 0 or 2 */

	if (bl_grp->screen_mode < GRP_SCR_G4) {		/* page size = 16kbytes */
		block = 1;
		bl_vdp_vram_h = page;
		for (n = 0; n < block; n++) {
			bl_erase_vram(c);
			bl_vdp_vram_h++;
		}
		return;
	}

	/* use HMMV for bitmap mode */
	if ((bl_grp->screen_mode == GRP_SCR_G5) ||
		(bl_grp->screen_mode == GRP_SCR_G6))
		width = 512;
	else
		width = 256;

	if (bl_grp->interlace_on)			/* erase double page */
		height = 512;
	else
		height = 256;

	*(uint16_t *)(&hmmv_cmd[0]) = 0;
	hmmv_cmd[2] = 0;
	hmmv_cmd[3] = page;
	*(uint16_t *)(&hmmv_cmd[4]) = width;
	*(uint16_t *)(&hmmv_cmd[6]) = height;
	hmmv_cmd[8] = c;

	bl_vdp_cmd_hmmv(hmmv_cmd);
	bl_vdp_cmd_wait();
}

void bl_grp_set_sprite_view(uint8_t page)
{
/*	page &= 0x07;*/
	bl_grp->sprite_view_page = page;
	bl_grp_set_sprite_attr_view_addr(table_sprite_attr_page[bl_grp->screen_mode][page]);
}

void bl_grp_set_sprite_active(uint8_t page)
{
/*	page &= 0x07;*/
	bl_grp->sprite_active_page = page;
	bl_grp_set_sprite_attr_active_addr(table_sprite_attr_page[bl_grp->screen_mode][page]);
}

void bl_grp_set_sprite_gen_view(uint8_t page)
{
/*	page &= 0x07;*/
	bl_grp->sprite_gen_view_page = page;
	bl_grp_set_sprite_gen_view_addr(table_sprite_gen_page[bl_grp->screen_mode][page]);
}

void bl_grp_set_sprite_gen_active(uint8_t page)
{
/*	page &= 0x07;*/
	bl_grp->sprite_gen_active_page = page;
	bl_grp_set_sprite_gen_active_addr(table_sprite_gen_page[bl_grp->screen_mode][page]);
}

static uint8_t adj_h[16] =
	{ 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08 };
void bl_grp_set_adjust_hor(int8_t h)
{
	if (h < -7)
		h = -7;
	else if (h > 8)
		h = 8;

	bl_grp->adjust_h = h;
	h += 7;
	bl_grp_update_reg_bit(18, 0x0F, adj_h[h]);
}

static uint8_t adj_v[16] =
	{ 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00, 0xF0, 0xE0, 0xD0, 0xC0, 0xB0, 0xA0, 0x90, 0x80 };
void bl_grp_set_adjust_ver(int8_t v)
{
	if (v < -7)
		v = -7;
	else if (v > 8)
		v = 8;

	bl_grp->adjust_v = v;
	v += 7;
	bl_grp_update_reg_bit(18, 0xF0, adj_v[v]);
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

uint16_t bl_grp_get_vramaddr_spr_gen(uint16_t no)
{
	return (bl_grp->sprite_gen_active_addr + (no << 3));
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

	n = 212 + 16 + bl_grp->scroll_v;
	if (n != 217)
		n--;

	spr_attr[0] = n;
	spr_attr[1] = 0;
	spr_attr[2] = 0;
	spr_attr[3] = 0;

	for (n = 0; n < 32; n++, vram_addr += 4) {
		bl_set_vram_addr16(vram_addr);
		bl_copy_to_vram_4(spr_attr);
	}
}

void bl_grp_put_sprite(uint16_t layer, uint8_t x, uint8_t y, uint8_t c, uint8_t no)
{
	y += bl_grp->scroll_v;
	if (y != 217)
		y--;

	spr_attr[0] = y;
	spr_attr[1] = x;
	spr_attr[2] = (bl_grp->sprite_mode < GRP_SPR_16) ? no : no << 2;
	spr_attr[3] = c;

	bl_set_vram_addr16(bl_grp->sprite_attr_active_addr + (layer << 2));
	bl_copy_to_vram_4(spr_attr);
}

void bl_grp_write_vram(uint8_t *src, uint16_t y, uint16_t size)
{
	uint16_t vram_addr = y * bl_grp->row_byte;

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
	uint16_t vram_addr = y * bl_grp->row_byte;

/*	vram_addr += x >> (bl_grp->bpp_shift);*/

	bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
	bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_addr;
	bl_vdp_vram_cnt = size;
	bl_copy_from_vram_nn(dest);
}

;