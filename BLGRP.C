/*
 * BLGRP
 */

#include <stddef.h>
/*#include <stdio.h>*/
#include <string.h>
#include <io.h>
#include <cpumode.h>
#include <msxbdos.h>
#include <blstd.h>
#include <blstdvdp.h>
#include <blgrp.h>
#include <blgrpcmd.h>
#include <blgrpfnt.h>
#include <blgrpdat.h>

#define WORK_SCRMOD	*((uint8_t *)(0xFCAF))
#define WORK_FORCOL	*((uint8_t *)(0xF3E9))
#define WORK_BAKCOL	*((uint8_t *)(0xF3EA))
#define WORK_BDRCOL	*((uint8_t *)(0xF3EB))

#define YAE_YJK_MODE	0x18	/* bit mask for yae yjk mode */
#define SPR_MODE	0x03	/* bit mask for sprite mode */
#define DISP_MODE	0x0E	/* bit mask for display timing mode */
#define DISP_IL_E0	0x0C	/* bit mask for interlaced two field */
#define SCROLL_MODE	0x03	/* bit mask for scroll mode */

static uint8_t scrmode[] = {
	0,	/* T1 */
	0,	/* T2 */
	3,	/* MC */
	1,	/* G1 */
	2,	/* G2 */
	4,	/* G3 */
	5,	/* G4 */
	6,	/* G5 */
	7,	/* G6 */
	8	/* G7 */
};

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
	{ 0x1B00, 0x1B80, 0x1C00, 0x1C80, 0x1D00, 0x1D80, 0x1E00, 0x1E80 },	/* G1 */
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
	0x0000, 0x1000, 0x2611, 0x3733, 0x4117, 0x5327, 0x6151, 0x7627,
	0x8171, 0x9373, 0xA661, 0xB664, 0xC411, 0xD265, 0xE555, 0xF777,
	0xFFFF	/* end mark */
};

static uint8_t adj_to_reg[16] = {
	/* -7, -6 ... -1, 0, 1, ... 7, 8 */
	0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0, 0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9, 0x8
};

static int8_t adj_from_reg[16] = {
	0, -1, -2, -3, -4, -5, -6, -7, 8, 7, 6, 5, 4, 3, 2, 1
};

static uint8_t bl_grp_bak[256];
static uint8_t bl_grp_suspended = 0;
static uint8_t vdp_reg_org[28];
static uint8_t scrmod_org;
static uint16_t addr;
static char msx_ver;

static void vdp_sync_regs_shadow(void);
static void vdp_restore_regs(void);
void bl_vdp_cmd_init(void);

int8_t bl_grp_init(void)
{
	/* MSX2/2+/tR only */
	msx_ver = get_msx_version();
	if (msx_ver == MSX1)
		return -1;

	memset(&bl_grp, 0x00, sizeof(struct bl_grp_var_t));
	bl_grp.shared_mem = (uint8_t *)bl_malloc(BL_GRP_SHARED_MEM);
	if (bl_grp.shared_mem == NULL)			/* not enough memory? */
		return -1;

	/* VDP version */
	bl_grp.vdp_ver = bl_read_vdp(1) & 0x04 ? GRP_VER_9958 : GRP_VER_9938;

	/* VDP register shadow */
	memcpy(&bl_grp.reg_shadow[0], (uint8_t *)0xF3DF, 8);	/* 0 ~ 7 */
	memcpy(&bl_grp.reg_shadow[8], (uint8_t *)0xFFE7, 16);	/* 8 ~ 23 */
	bl_grp.reg_shadow[24] = 0;				/* 24 dummy */

	if (bl_grp.vdp_ver) {					/* 25 ~ 27 */
		if (msx_ver == MSX2) {				/* MSX2 with V9958 */
			bl_grp.reg_shadow[25] = 0;
			bl_grp.reg_shadow[26] = 0;
			bl_grp.reg_shadow[27] = 0;
			bl_write_vdp(25, 0);
			bl_write_vdp(26, 0);
			bl_write_vdp(27, 0);
		} else {
			bl_grp.reg_shadow[25] = *(uint8_t *)0xFFFA;
			bl_grp.reg_shadow[26] = *(uint8_t *)0xFFFB;
			bl_grp.reg_shadow[27] = *(uint8_t *)0xFFFC;
		}
	}

	memcpy(vdp_reg_org, bl_grp.reg_shadow, sizeof(vdp_reg_org));
	scrmod_org = WORK_SCRMOD;

	bl_grp.screen_mode  = 0xFF;			/* dummy */
	bl_grp.yae_yjk_mode = 0;
	bl_grp.display_on   = 0;
	bl_grp.sprite_mode  = 0;
	bl_grp.sprite_on    = 1;
	bl_grp.text_width   = 40;
	bl_grp.palette0_on  = 0;
	bl_grp.line_212     = 1;
	bl_grp.display_mode = bl_grp.reg_shadow[9] & DISP_MODE;
	bl_grp.interlace_on = (bl_grp.reg_shadow[9] & DISP_IL_E0) == DISP_IL_E0 ? 1 : 0;

	bl_grp.color_text_fg = WORK_FORCOL;
	bl_grp.color_text_bg = WORK_BAKCOL;
	bl_grp.color_border  = WORK_BDRCOL;

	bl_grp.adjust_h = adj_from_reg[bl_grp.reg_shadow[18] & 0xF];
	bl_grp.adjust_v = adj_from_reg[(bl_grp.reg_shadow[18] >> 4) & 0xF];

	bl_grp.scroll_mode = 0;
	bl_grp.scroll_h    = 0;
	bl_grp.scroll_v    = 0;

	bl_grp.width  = 256;
	bl_grp.height = 212;

	bl_grp_set_font_size(8, 8);
	bl_grp_set_font_color(bl_grp.color_text_fg, bl_grp.color_text_bg);

	/* Initialize palette */
	memcpy(bl_grp.palette, init_palette, sizeof(init_palette));

	bl_vdp_cmd_init();

	return bl_grp.vdp_ver;
}

void bl_grp_deinit(void)
{
	bl_grp_reset_palette();

	vdp_reg_org[1] &= ~0x40;		/* display off */
	memcpy(bl_grp.reg_shadow, vdp_reg_org, sizeof(vdp_reg_org));
	vdp_restore_regs();
	vdp_sync_regs_shadow();

	bl_free(bl_grp.shared_mem);
#asm
	PUSH	IX
	PUSH	IY

	LD	A, (_scrmod_org)
	LD	IX, 0005FH			; CHGMOD
	LD	IY, (0FCC1H - 1)		; EXPTBL - 1
	CALL	0001CH				; CALSLT

	POP	IY
	POP	IX
	EI
#endasm
}

uint8_t bl_grp_get_vdp_ver(void)
{
	return bl_grp.vdp_ver;
}

static void vdp_sync_regs_shadow(void)
{
	memcpy((uint8_t *)0xF3DF, &bl_grp.reg_shadow[0], 8);	/* 0 ~ 7 */
	memcpy((uint8_t *)0xFFE7, &bl_grp.reg_shadow[8], 16);	/* 8 ~ 23 */

	if (msx_ver >= MSX2P) {					/* 25 ~ 27 */
		*(uint8_t *)0xFFFA = bl_grp.reg_shadow[25];
		*(uint8_t *)0xFFFB = bl_grp.reg_shadow[26];
		*(uint8_t *)0xFFFC = bl_grp.reg_shadow[27];
	}
}

static void vdp_restore_regs(void)
{
	uint8_t n;

	for (n = 0; n <= 23; n++)				/* 0 ~ 23 */
		bl_write_vdp(n, bl_grp.reg_shadow[n]);

	if (bl_grp.vdp_ver) {					/* 25 ~ 27 */
		bl_write_vdp(25, bl_grp.reg_shadow[25]);
		bl_write_vdp(26, bl_grp.reg_shadow[26]);
		bl_write_vdp(27, bl_grp.reg_shadow[27]);
	}
}

void bl_grp_suspend(void)
{
	if (bl_grp_suspended)
		return;

	bl_grp_suspended = 1;

	bl_free(bl_grp.shared_mem);
	memcpy(&bl_grp_bak, &bl_grp, sizeof(bl_grp_bak));

	vdp_sync_regs_shadow();
}

void bl_grp_resume(void)
{
	if (!bl_grp_suspended)
		return;

	bl_grp_suspended = 0;

	memcpy(&bl_grp, &bl_grp_bak, sizeof(bl_grp_bak));
	bl_grp.shared_mem = (uint8_t *)bl_malloc(BL_GRP_SHARED_MEM);

	vdp_restore_regs();
}

extern uint8_t update_bits;
#define bl_grp_update_reg_bit(no, mask, bits)	\
{ update_bits = bits; bl_grp_update_reg(((uint16_t)no << 8) | (uint8_t)(~mask)); }
#asm
;void bl_grp_update_reg(uint16_t no_mask)
		GLOBAL _bl_grp_update_reg_hl
_bl_grp_update_reg:
_bl_grp_update_reg_hl:			; fastcall for BLOPTIM
		DI
					; HL = no_mask
		LD D,GRP_REG_SHADOW_HI	; VDP shadow register addr high
		LD E,H			; VDP shadow register addr low
		LD A,(DE)		; Read old value
		AND L			; Mask bits

		DEFB 0F6H		; OR bits
_update_bits:	DEFB 0

		OUT (099H),A		; Write value
		LD (DE),A		; Update shadow register
		LD A,H
		OR 080H
		OUT (099H),A		; Write register no.

		EI
		RET
#endasm

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

void bl_grp_set_pattern_name_addr(uint16_t addr_)
{
	addr = addr_;
	bl_grp.pattern_name_addr = addr;
	addr |= table_pattern_name_fix[bl_grp.screen_mode];
	bl_grp_update_reg_bit( 2, 0x3F, (uint8_t)(addr >> 10));
}

void bl_grp_set_color_addr(uint16_t addr_)
{
	addr = addr_;
	bl_grp.color_addr = addr;
	addr |= table_color_fix[bl_grp.screen_mode];
	bl_grp_update_reg_bit( 3, 0xFF, (uint8_t)(addr >> 6));
	bl_grp_update_reg_bit(10, 0x03, (uint8_t)(addr >> 14));
}

void bl_grp_set_pattern_gen_addr(uint16_t addr_)
{
	addr = addr_;
	bl_grp.pattern_gen_addr = addr;
	bl_grp_update_reg_bit( 4, 0x1F, (uint8_t)(addr >> 11));
}

void bl_grp_set_sprite_attr_view_addr(uint16_t addr_)
{
	addr = addr_;
	bl_grp.sprite_attr_view_addr = addr;
	bl_grp.sprite_color_view_addr = addr - 0x200;
	addr |= table_sprite_attr_fix[bl_grp.screen_mode];
	bl_grp_update_reg_bit( 5, 0xFF, (uint8_t)(addr >> 7));
	bl_grp_update_reg_bit(11, 0x01, (uint8_t)(addr >> 15));
}

void bl_grp_set_sprite_attr_active_addr(uint16_t addr_)
{
	addr = addr_;
	bl_grp.sprite_attr_active_addr = addr;
	bl_grp.sprite_color_active_addr = addr - 0x200;
}

void bl_grp_set_sprite_gen_view_addr(uint16_t addr_)
{
	addr = addr_;
	bl_grp.sprite_gen_view_addr = addr;
	bl_grp_update_reg_bit( 6, 0x1F, (uint8_t)(addr >> 11));
}

void bl_grp_set_sprite_gen_active_addr(uint16_t addr)
{
	bl_grp.sprite_gen_active_addr = addr;
}

void bl_grp_setup_mc_pattern(void)
{
	uint8_t row_data[32], n, m, data_begin;

	bl_vdp_vram_h = 0;
	bl_vdp_vram_m = 0;
	bl_vdp_vram_l = 0;
	bl_vdp_vram_cnt = 2048;
	memset(bl_grp.shared_mem, 0x00, BL_GRP_SHARED_MEM);
	bl_copy_to_vram_nn(bl_grp.shared_mem);

	addr = bl_grp.pattern_name_addr;
	data_begin = 0;
	for (n = 0; n < 6; n++, data_begin += 32) {
		for (m = 0; m < 32; m++)
			row_data[m] = data_begin + m;

		bl_vdp_vram_h = (uint8_t)(addr >> 14);
		for (m = 0; m < 4; m++, addr += 32) {
			bl_vdp_vram_m = (uint8_t)((addr >> 8) & 0x3F);
			bl_vdp_vram_l = (uint8_t)addr;
			bl_copy_to_vram_32(row_data);
		}
	}
}

void bl_grp_set_screen_mode(uint8_t mode)
{
	bl_grp.screen_mode = mode;

	bl_grp_update_reg_bit( 0, 0x0E, mode_0_1[mode][0]);
	bl_grp_update_reg_bit( 1, 0x18, mode_0_1[mode][1]);

	bl_grp_set_yae_yjk_mode(bl_grp.yae_yjk_mode);
	bl_grp_set_color_border(bl_grp.color_border);
	bl_grp_set_pattern_name_addr(table_addr[mode][0]);
	bl_grp_set_color_addr(table_addr[mode][1]);
	bl_grp_set_pattern_gen_addr(table_addr[mode][2]);
	bl_grp_set_sprite_view(0);
	bl_grp_set_sprite_active(0);
	bl_grp_set_sprite_gen_view(0);
	bl_grp_set_sprite_gen_active(0);
	bl_grp_update_palette(bl_grp.palette);

	bl_grp.row_byte = table_addr[mode][5];
	bl_grp.bpp_shift = table_addr[mode][6];
	bl_grp.width = (mode == GRP_SCR_G5) || (mode == GRP_SCR_G6) ? 512 : 256;

	bl_grp_set_sprite_mode(bl_grp.sprite_mode);
	bl_grp_set_sprite_on(bl_grp.sprite_on);

	bl_grp_set_palette0_on(bl_grp.palette0_on);
	bl_grp_set_line_212(bl_grp.line_212);
	bl_grp_set_display_mode(bl_grp.display_mode);

	bl_grp_set_scroll_mode(bl_grp.scroll_mode);
	bl_grp_set_scroll_h(bl_grp.scroll_h);
	bl_grp_set_scroll_v(bl_grp.scroll_v);

	bl_grp_set_view(0);
	bl_grp_set_active(0);
	bl_grp_erase(0, 0);

	switch (mode) {
	case GRP_SCR_T1:
	case GRP_SCR_T2:
		bl_grp_set_font_size(6, 8);
		break;
	case GRP_SCR_G1:
	case GRP_SCR_G2:
	case GRP_SCR_G4:
		bl_grp_set_font_size(8, 8);
		break;
	case GRP_SCR_MC:
		bl_grp_setup_mc_pattern();
		break;
	default:
		break;
	}

	WORK_SCRMOD = scrmode[mode];		/* for compatibility */
}

void bl_grp_set_yae_yjk_mode(uint8_t mode)
{
	bl_grp.yae_yjk_mode = mode;

	if (bl_grp.vdp_ver) {
		bl_grp_update_reg_bit(25, YAE_YJK_MODE, mode);
	}
}

void bl_grp_set_display_on(uint8_t on)
{
	if (on) {
		bl_grp.display_on = 1;
		on = 0x40;
	} else {
		bl_grp.display_on = 0;
		on = 0x00;
	}
	bl_grp_update_reg_bit(1, 0x40, on);
}

uint8_t bl_grp_get_screen_mode(void)
{
	return bl_grp.screen_mode;
}

uint8_t bl_grp_get_yae_yjk_mode(void)
{
	return bl_grp.yae_yjk_mode;
}

uint8_t bl_grp_get_display_on(void)
{
	return bl_grp.display_on;
}

void bl_grp_set_sprite_mode(uint8_t mode)
{
	bl_grp.sprite_mode = mode;
	bl_grp_update_reg_bit(1, SPR_MODE, mode);
}

void bl_grp_set_sprite_on(uint8_t on)
{
	if (on) {
		bl_grp.sprite_on = 1;
		on = 0x00;
	} else {
		bl_grp.sprite_on = 0;
		on = 0x02;
	}
	bl_grp_update_reg_bit(8, 0x02, on);
}

uint8_t bl_grp_get_sprite_mode(void)
{
	return bl_grp.sprite_mode;
}

uint8_t bl_grp_get_sprite_on(void)
{
	return bl_grp.sprite_on;
}

void bl_grp_set_palette0_on(uint8_t on)
{
	if (on) {
		bl_grp.palette0_on = 1;
		on = 0x20;
	} else {
		bl_grp.palette0_on = 0;
		on = 0x00;
	}
	bl_grp_update_reg_bit(8, 0x20, on);
}

void bl_grp_set_line_212(uint8_t on)
{
	if (on) {
		bl_grp.line_212 = 1;
		bl_grp.height = 212;
		on = 0x80;
	} else {
		bl_grp.line_212 = 0;
		bl_grp.height = 192;
		on = 0x00;
	}
	bl_grp_update_reg_bit(9, 0x80, on);
}

extern void bl_grp_setup_font_draw_func(void);	/* from BLGRPFNT.C */
void bl_grp_set_display_mode(uint8_t mode)
{
	bl_grp.display_mode = mode;

	bl_grp_update_reg_bit(9, DISP_MODE, bl_grp.display_mode);
	bl_grp.interlace_on = (bl_grp.display_mode & DISP_IL_E0) == DISP_IL_E0 ? 1 : 0;
	bl_grp_setup_font_draw_func();
}

uint8_t bl_grp_get_palette0_on(void)
{
	return bl_grp.palette0_on;
}

uint8_t bl_grp_get_line_212(void)
{
	return bl_grp.line_212;
}

uint8_t bl_grp_get_display_mode(void)
{
	return bl_grp.display_mode;
}

uint8_t bl_grp_get_interlace_on(void)
{
	return bl_grp.interlace_on;
}

static void bl_grp_fill_g1_color_table(void)
{
	uint8_t color_table[32], n, c;

	c = bl_grp.reg_shadow[7];
	for (n = 0; n < 32; n++)
		color_table[n] = c;

	bl_vdp_vram_h = (uint8_t)(bl_grp.color_addr >> 14);
/*	bl_vdp_vram_h |= bl_grp.active_page_a16_a14;*/
	bl_vdp_vram_m = (uint8_t)((bl_grp.color_addr >> 8) & 0x3F);
	bl_vdp_vram_l = (uint8_t)bl_grp.color_addr;
	bl_copy_to_vram_32(color_table);
}

void bl_grp_set_color_text_fg(uint8_t color)
{
	bl_grp.color_text_fg = color & 0x0F;

	switch (bl_grp.screen_mode) {
	case GRP_SCR_T1:
	case GRP_SCR_T2:
	case GRP_SCR_G1:
		bl_grp_update_reg_bit(7, 0xF0, (bl_grp.color_text_fg) << 4);
		if (bl_grp.screen_mode == GRP_SCR_G1)
			bl_grp_fill_g1_color_table();
		break;
	default:
		break;
	}
}

void bl_grp_set_color_text_bg(uint8_t color)
{
	bl_grp.color_text_bg = color & 0x0F;

	switch (bl_grp.screen_mode) {
	case GRP_SCR_T1:
	case GRP_SCR_T2:
	case GRP_SCR_G1:
		bl_grp_update_reg_bit(7, 0x0F, bl_grp.color_text_bg);
		if (bl_grp.screen_mode == GRP_SCR_G1)
			bl_grp_fill_g1_color_table();
		break;
	default:
		break;
	}
}

void bl_grp_set_color_border(uint8_t color)
{
	if (bl_grp.screen_mode < GRP_SCR_G7) {
		bl_grp.color_border = color & 0x0F;
		bl_grp_update_reg_bit(7, 0x0F, bl_grp.color_border);
	} else {
		bl_grp.color_border = color;
		bl_grp_update_reg_bit(7, 0xFF, bl_grp.color_border);
	}
}

uint8_t bl_grp_get_color_text_fg(void)
{
	return bl_grp.color_text_fg;
}

uint8_t bl_grp_get_color_text_bg(void)
{
	return bl_grp.color_text_bg;
}

uint8_t bl_grp_get_color_border(void)
{
	return bl_grp.color_border;
}

void bl_grp_set_width(uint8_t width)
{
}

void bl_grp_set_view(uint8_t page)
{
	if (bl_grp.interlace_on) {
		page &= 0x02;
		bl_grp.view_page = page;
		page++;					/* page 1 or 3 */
	} else {
		bl_grp.view_page = page;
	}
	page <<= 5;
	bl_grp_update_reg_bit(2, 0x60, page);
}

void bl_grp_set_active(uint8_t page)
{
	if (bl_grp.interlace_on)
		page &= 0x02;				/* page 0 or 2 */

	bl_grp.active_page = page;
	if (bl_grp.screen_mode < GRP_SCR_G6)
		bl_grp.active_page_a16_a14 = page << 1;	/* 32Kbytes */
	else
		bl_grp.active_page_a16_a14 = page << 2;	/* 64Kbytes */
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
	static uint16_t n, block;
	uint16_t lines;

	if (bl_grp.interlace_on) {
		page &= 0x02;				/* page 0 or 2 */
		lines = 512;				/* erase double page */
	} else {
		lines = 256;
	}

	if (bl_grp.screen_mode < GRP_SCR_G4) {		/* page size = 16kbytes */
		block = 1;
		bl_vdp_vram_h = page;
		for (n = 0; n < block; n++) {
			bl_erase_vram(c);
			bl_vdp_vram_h++;
		}
		return;
	}

	/* use HMMV for bitmap mode */
	*(uint16_t *)(&hmmv_cmd[0]) = 0;
	hmmv_cmd[2] = 0;
	hmmv_cmd[3] = page;
	*(uint16_t *)(&hmmv_cmd[4]) = bl_grp.width;
	*(uint16_t *)(&hmmv_cmd[6]) = lines;
	hmmv_cmd[8] = c;

	bl_vdp_cmd_hmmv(hmmv_cmd);
	bl_vdp_cmd_wait();
}

extern uint16_t clear_size;
extern uint8_t clear_val;
static uint16_t table_fill_size[] = { 960 - 1, 1920 - 1, 2048 - 1, 768 - 1, 768 - 1, 768 - 1 };
static void bl_grp_clear_screen_fill(void)
{
	if (bl_grp.screen_mode == GRP_SCR_MC) {
		bl_set_vram_addr16(bl_grp.pattern_gen_addr);
		clear_val = ' ';
	} else {
		bl_set_vram_addr16(bl_grp.pattern_name_addr);
		clear_val = bl_grp.font_bgc | ((bl_grp.font_bgc) << 4);
	}

	bl_write_vram(clear_val);		/* First byte */
	clear_size = table_fill_size[bl_grp.screen_mode];
#asm
		DI
		DEFB	011H			; LD DE, nn
_clear_size:	DEFW	00000H
		DEFB	021H			; LD HL, nn
_clear_val:	DEFW	00000H

		LD	C, 098H
clear_screen_fill_lp:
		OUT	(C), L			; write vram
		DEC	DE
		LD	A, D
		OR	E
		JP	NZ, clear_screen_fill_lp
		EI
#endasm
}

void bl_grp_clear_screen(void)
{
	uint8_t val, page;

	switch (bl_grp.screen_mode) {
	case GRP_SCR_G4:
		val = bl_grp.font_bgc;
		val |= (bl_grp.font_bgc) << 4;
		break;
	case GRP_SCR_G5:
		val = bl_grp.font_bgc;
		val |= (bl_grp.font_bgc) << 2;
		val |= (bl_grp.font_bgc) << 4;
		val |= (bl_grp.font_bgc) << 6;
		break;
	case GRP_SCR_G6:
		val = bl_grp.font_bgc;
		val |= (bl_grp.font_bgc) << 4;
		break;
	case GRP_SCR_G7:
		val = bl_grp.font_bgc;
		break;
	default:					/* for pattern mode */
		bl_grp_clear_screen_fill();
		return;					/* return */
	}

	page = bl_grp.active_page;
	if (bl_grp.interlace_on)
		page &= 0x02;				/* page 0 or 2 */

	do {
		*(uint16_t *)(&hmmv_cmd[0]) = 0;
		hmmv_cmd[2] = 0;
		hmmv_cmd[3] = page++;
		*(uint16_t *)(&hmmv_cmd[4]) = bl_grp.width;
		*(uint16_t *)(&hmmv_cmd[6]) = bl_grp.height;	/* 192, 212 */
		hmmv_cmd[8] = val;

		bl_vdp_cmd_hmmv(hmmv_cmd);
	} while (bl_grp.interlace_on && (page & 0x01));

	bl_vdp_cmd_wait();
}

void bl_grp_set_sprite_view(uint8_t page)
{
/*	page &= 0x07;*/
	bl_grp.sprite_view_page = page;
	bl_grp_set_sprite_attr_view_addr(table_sprite_attr_page[bl_grp.screen_mode][page]);
}

void bl_grp_set_sprite_active(uint8_t page)
{
/*	page &= 0x07;*/
	bl_grp.sprite_active_page = page;
	bl_grp_set_sprite_attr_active_addr(table_sprite_attr_page[bl_grp.screen_mode][page]);
}

void bl_grp_set_sprite_gen_view(uint8_t page)
{
/*	page &= 0x07;*/
	bl_grp.sprite_gen_view_page = page;
	bl_grp_set_sprite_gen_view_addr(table_sprite_gen_page[bl_grp.screen_mode][page]);
}

void bl_grp_set_sprite_gen_active(uint8_t page)
{
/*	page &= 0x07;*/
	bl_grp.sprite_gen_active_page = page;
	bl_grp_set_sprite_gen_active_addr(table_sprite_gen_page[bl_grp.screen_mode][page]);
}

void bl_grp_set_adjust_h(int8_t h)
{
	if (h < -7)
		h = -7;
	else if (h > 8)
		h = 8;

	bl_grp.adjust_h = h;
	bl_grp_update_reg_bit(18, 0x0F, adj_to_reg[h + 7]);
}

void bl_grp_set_adjust_v(int8_t v)
{
	if (v < -7)
		v = -7;
	else if (v > 8)
		v = 8;

	bl_grp.adjust_v = v;
	bl_grp_update_reg_bit(18, 0xF0, adj_to_reg[v + 7] << 4);
}

void bl_grp_set_scroll_mode(uint8_t mode)
{
	bl_grp.scroll_mode = mode;

	if (bl_grp.vdp_ver) {
		bl_grp_update_reg_bit(25, SCROLL_MODE, bl_grp.scroll_mode);
	}
}

void bl_grp_set_scroll_h(uint16_t h)
{
	uint8_t r26, r27;

	bl_grp.scroll_h = h;

	if (bl_grp.vdp_ver) {
		r26 = (h >> 3) & 0x3F;
		r27 = h & 0x07;
		if (r27) {
			r26++;
			r27 = 8 - r27;
		}

		bl_grp_update_reg_bit(26, 0xFF, r26);
		bl_grp_update_reg_bit(27, 0xFF, r27);
	}
}

void bl_grp_set_scroll_v(uint8_t v)
{
	bl_grp.scroll_v = v;
	bl_grp_update_reg_bit(23, 0xFF, v);
}

static uint16_t pal_data, pal_r, pal_g, pal_b;
void bl_grp_set_palette(uint8_t no, uint8_t r, uint8_t g, uint8_t b)
{
	*((uint8_t *)&pal_data + 1) = (no << 4) | g;
	*((uint8_t *)&pal_data + 0) = (r  << 4) | b;

	bl_grp.palette[no] = pal_data;

	bl_set_palette_(pal_data);
}

uint16_t bl_grp_get_palette(uint8_t no)
{
	pal_data = bl_grp.palette[no];
	pal_r = (pal_data << 4) & 0x0F00;
	pal_g = (pal_data >> 4) & 0x00F0;
	pal_b = pal_data & 0x000F;

	return (pal_r + pal_g + pal_b);
}

void bl_grp_reset_palette(void)
{
	/* Initialize default palette */
	memcpy(bl_grp.palette, init_palette, sizeof(init_palette));
	bl_grp_update_palette(bl_grp.palette);
}

static uint16_t mute_palette[] = {
	/* pal_no << 12, g << 8, r << 4, b */
	0x0000, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000,
	0x8000, 0x9000, 0xA000, 0xB000, 0xC000, 0xD000, 0xE000, 0xF000,
	0xFFFF	/* end mark */
};
void bl_grp_set_palette_mute(uint8_t on)
{
	bl_grp_update_palette(on ? mute_palette : bl_grp.palette);
}

void bl_grp_set_palette_gray(uint8_t on)
{
	uint16_t gray_palette[17];
	static uint16_t n, y, v;

	if (on) {
		for (n = 0; n < 16; n++) {
			/* Y = 0.299R + 0.587G + 0.114B */
			v = bl_grp.palette[n];
			y = ((((v >> 4) & 7) * 299) +
			     (((v >> 8) & 7) * 587) +
			     (((v     ) & 7) * 114)) / 1000;
			/* gray_palette[n] = ((uint16_t)n << 12) | (y << 8) | (y << 4) | y; */
			gray_palette[n] = ((uint16_t)n << 12) + y * (256 + 16 + 1);
		}
		gray_palette[16] = 0xFFFF;
		bl_grp_update_palette(gray_palette);
	} else {
		bl_grp_update_palette(bl_grp.palette);
	}
}

uint16_t bl_grp_get_vramaddr_spr_gen(uint16_t no)
{
	return (bl_grp.sprite_gen_active_addr + (no << 3));
}

uint16_t bl_grp_get_vramaddr_spr_col(uint16_t no)
{
	return (bl_grp.sprite_color_active_addr + (no << 4));
}

uint16_t bl_grp_get_vramaddr_spr_attr(uint8_t layer)
{
	return (bl_grp.sprite_attr_active_addr + (layer << 2));
}

static uint8_t spr_attr0[4] = { 0, 0, 0, 0 };
void bl_grp_clear_sprite(void)
{
	uint8_t n;

	n = 212 + 16 + bl_grp.scroll_v;
	if (n != 217)
		n--;

	spr_attr0[0] = n;

	addr = bl_grp.sprite_attr_active_addr;
	for (n = 0; n < 32; n++, addr += 4) {
		bl_set_vram_addr16(addr);
		bl_copy_to_vram_4(spr_attr0);
	}
}

static uint8_t spr_attr[4];
void bl_grp_put_sprite(uint8_t layer, uint8_t x, uint8_t y, uint8_t c, uint8_t no)
{
	y += bl_grp.scroll_v;
	if (y != 217)
		y--;

	spr_attr[0] = y;
	spr_attr[1] = x;
	spr_attr[2] = (bl_grp.sprite_mode < GRP_SPR_16) ? no : no << 2;
	spr_attr[3] = c;

	bl_set_vram_addr16(bl_grp.sprite_attr_active_addr + (layer << 2));
	bl_copy_to_vram_4(spr_attr);
}

void bl_grp_write_vram(uint8_t *src, uint16_t y, uint16_t size)
{
	addr = y * bl_grp.row_byte;
/*	addr += x >> (bl_grp.bpp_shift);*/

	bl_vdp_vram_h = (uint8_t)(addr >> 14);
	bl_vdp_vram_h |= bl_grp.active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((addr >> 8) & 0x3F);
	bl_vdp_vram_l = (uint8_t)addr;
	bl_vdp_vram_cnt = size;
	bl_copy_to_vram_nn(src);
}

void bl_grp_read_vram(uint8_t *dest, uint16_t y, uint16_t size)
{
	addr = y * bl_grp.row_byte;
/*	addr += x >> (bl_grp.bpp_shift);*/

	bl_vdp_vram_h = (uint8_t)(addr >> 14);
	bl_vdp_vram_h |= bl_grp.active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((addr >> 8) & 0x3F);
	bl_vdp_vram_l = (uint8_t)addr;
	bl_vdp_vram_cnt = size;
	bl_copy_from_vram_nn(dest);
}
