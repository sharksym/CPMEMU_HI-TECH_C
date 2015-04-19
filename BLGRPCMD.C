/*
 * BLGRPCMD
 */

/*#include <stdio.h>*/
#include <string.h>
#include <io.h>
#include <msxbdos.h>
#include <blstd.h>
#include <blstdvdp.h>
#include <blgrp.h>
#include <blgrpcmd.h>
#include <blgrpfnt.h>
#include <blgrpdat.h>

static struct bl_grp_var_t *bl_grp = NULL;

void bl_grp_cmd_init_var(struct bl_grp_var_t *bl_grp_var)
{
	bl_grp = bl_grp_var;
}

static uint8_t pset_cmd[7] = {
	0x00,	/* R36 DX low */
	0x00,	/* R37 DX high */
	0x00,	/* R38 DY low */
	0x00,	/* R39 DY high */
	0x00,	/* R44 Color */
	0x00,	/* R45 Argument */
	0x50	/* R46 Command PSET */
};
void bl_grp_put_pixel(uint16_t x, uint16_t y, uint8_t c, uint8_t op)
{
	uint16_t vram_addr;
	uint8_t vram_data;

	if (bl_grp->screen_mode == GRP_SCR_MC) {	/* MC */
		vram_addr = bl_grp->pattern_gen_addr;
		vram_addr += (y & 0xF8) << 5;		/* (y / 8) * 256 */
		vram_addr += y & 0x07;
		vram_addr += (x & 0xFE) << 2;		/* (x / 2) * 8 */
		bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
		bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
		bl_vdp_vram_m = (uint8_t)((vram_addr >> 8) & 0x3F);
		bl_vdp_vram_l = (uint8_t)vram_addr;
		vram_data = bl_read_vram();

		if (x & 0x01) {				/* low 4bit */
			vram_data &= 0xF0;
			vram_data |= c;
		} else {				/* high 4bit */
			vram_data &= 0x0F;
			vram_data |= c << 4;
		}
		bl_write_vram(vram_data);
	} else {
		pset_cmd[3] = bl_grp->active_page;
		if (bl_grp->interlace_on) {
			pset_cmd[2] = (uint8_t)(y >> 1);
			if ((uint8_t)y & 0x01)		/* for odd page */
				pset_cmd[3]++;
		} else {
			pset_cmd[2] = (uint8_t)y;
		}

		*(uint16_t *)(&pset_cmd[0]) = x;
		pset_cmd[4] = c;
		pset_cmd[6] = 0x50 | op;

		bl_vdp_cmd_pset(pset_cmd);
	}
}

/* This should be called by bl_grp_line(), if interlace mode is on */
static void bl_grp_put_pixel_ext(uint16_t x, uint16_t y)
{
	pset_cmd[3] = bl_grp->active_page;
	if (bl_grp->interlace_on) {
		pset_cmd[2] = (uint8_t)(y >> 1);
		if ((uint8_t)y & 0x01)			/* for odd page */
			pset_cmd[3]++;
	} else {
		pset_cmd[2] = (uint8_t)y;
	}

	*(uint16_t *)(&pset_cmd[0]) = x;
#asm
	global  _bl_vdp_cmd_wait,_bl_vdp_cmd_write
	di
	ld hl,_pset_cmd
	ld bc,00424h		; count = 4, R36 ~ R39
	call _bl_vdp_cmd_write
	inc hl
	inc hl			; skip 2 data (R44,R45)
	ld a,(hl)		; command data
	out (099h),a
	ld a,0AEh		; VDP R46
	out (099h),a
	ei
#endasm
}

uint8_t bl_grp_get_pixel(uint16_t x, uint16_t y)
{
	uint16_t vram_addr;
	uint8_t vram_data;

	if (bl_grp->screen_mode == GRP_SCR_MC) {	/* MC */
		vram_addr = bl_grp->pattern_gen_addr;
		vram_addr += (y & 0xF8) << 5;		/* (y / 8) * 256 */
		vram_addr += y & 0x07;
		vram_addr += (x & 0xFE) << 2;		/* (x / 2) * 8 */
		bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
		bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
		bl_vdp_vram_m = (uint8_t)((vram_addr >> 8) & 0x3F);
		bl_vdp_vram_l = (uint8_t)vram_addr;
		vram_data = bl_read_vram();

		if (!(x & 0x01))
			vram_data >>= 4;

		return (vram_data & 0x0F);
	}

	vram_addr = y * bl_grp->row_byte;
	vram_addr += x >> (bl_grp->bpp_shift);
	bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
	bl_vdp_vram_h |= bl_grp->active_page_a16_a14;
	bl_vdp_vram_m = (uint8_t)((vram_addr >> 8) & 0x3F);
	bl_vdp_vram_l = (uint8_t)vram_addr;

	vram_data = bl_read_vram();

	switch (bl_grp->bpp_shift) {
	case 1:
		if (!(x & 0x01))
			vram_data >>= 4;

		vram_data &= 0x0F;
		break;
	case 2:
		x &= 0x03;
		if (x == 0)
			vram_data >>= 6;
		else if (x == 1)
			vram_data >>= 4;
		else if (x == 2)
			vram_data >>= 2;

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
void bl_grp_line(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op)
{
	uint16_t line_w, line_h;
	uint8_t sy0, sy1, dy0, dy1;
	int16_t deltax, deltay;
	int16_t error, step, x, y, inc;

	if (bl_grp->screen_mode == GRP_SCR_MC) {	/* MC */
		deltax = dx > sx ? dx - sx : sx - dx;
		deltay = dy > sy ? dy - sy : sy - dy;
		/* first draw using put_pixel() */
		bl_grp_put_pixel(sx, sy, c, op);
		if (deltay > deltax) {
			error = deltay >> 1;
			inc = sy < dy ? 1 : -1;
			step = sx < dx ? 1 : -1;
			for (x = sx, y = sy; y != dy; y += inc) {
				bl_grp_put_pixel(x, y, c, op);
				error -= deltax;
				if (error < 0) {
					x += step;
					error += deltay;
				}
			}
		} else {
			error = deltax >> 1;
			inc = sx < dx ? 1 : -1;
			step = sy < dy ? 1 : -1;
			for (x = sx, y = sy; x != dx; x += inc) {
				bl_grp_put_pixel(x, y, c, op);
				error -= deltay;
				if (error < 0) {
					y += step;
					error += deltax;
				}
			}
		}
		bl_grp_put_pixel(x, y, c, op);

		return;
	}

	if (bl_grp->interlace_on) {
		deltax = dx > sx ? dx - sx : sx - dx;
		deltay = dy > sy ? dy - sy : sy - dy;

		if (!deltay) {				/* hor line */
			*(uint16_t *)(&line_cmd[0]) = sx;
			line_cmd[2] = (uint8_t)(sy >> 1);
			line_cmd[3] = bl_grp->active_page;
			if ((uint8_t)sy & 0x01)
				line_cmd[3]++;

			*(uint16_t *)(&line_cmd[4]) = deltax;
			*(uint16_t *)(&line_cmd[6]) = 0;
			line_cmd[8] = c;
			line_cmd[9] = sx < dx ? 0x00 : 0x04;
			line_cmd[10] = 0x70 | op;
			bl_vdp_cmd_line(line_cmd);
		} else if (!deltax) {			/* ver even & odd line */
			if (sy > dy) {			/* should be top to down direction */
				dy0 = dy;
				dy = sy;
				sy = sy0;
			}
			sy0 = sy1 = sy >> 1;
			dy0 = dy1 = dy >> 1;
			if ((uint8_t)sy & 0x01)		/* for even line */
				sy0++;

			*(uint16_t *)(&line_cmd[0]) = sx;
			*(uint16_t *)(&line_cmd[6]) = 0;
			line_cmd[5] = 0;
			line_cmd[8] = c;
			line_cmd[9] = 0x01;
			line_cmd[10] = 0x70 | op;

			line_cmd[2] = sy0;		/* draw even line */
			line_cmd[3] = bl_grp->active_page;
			line_cmd[4] = dy0 - sy0;
			bl_vdp_cmd_line(line_cmd);

			line_cmd[2] = sy1;		/* draw odd line */
			line_cmd[3]++;
			line_cmd[4] = dy1 - sy0;
			bl_vdp_cmd_line(line_cmd);
		} else {
			/* first draw using put_pixel() */
			bl_grp_put_pixel(sx, sy, c, op);
			if (deltay > deltax) {
				error = deltay >> 1;
				inc = sy < dy ? 1 : -1;
				step = sx < dx ? 1 : -1;
				for (x = sx, y = sy; y != dy; y += inc) {
					bl_grp_put_pixel_ext(x, y);
					error -= deltax;
					if (error < 0) {
						x += step;
						error += deltay;
					}
				}
			} else {
				error = deltax >> 1;
				inc = sx < dx ? 1 : -1;
				step = sy < dy ? 1 : -1;
				for (x = sx, y = sy; x != dx; x += inc) {
					bl_grp_put_pixel_ext(x, y);
					error -= deltay;
					if (error < 0) {
						y += step;
						error += deltax;
					}
				}
			}
			bl_grp_put_pixel_ext(x, y);
		}
	} else {
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
}

void bl_grp_box(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op)
{
	bl_grp_line(sx, sy, dx, sy, c, op);
	bl_grp_line(sx, sy, sx, dy, c, op);
	bl_grp_line(dx, sy, dx, dy, c, op);
	bl_grp_line(sx, dy, dx, dy, c, op);
}

void bl_grp_boxfill(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c, uint8_t op)
{
	uint16_t tmp;

	if (sx > dx) {
		tmp = dx;
		dx = sx;
		sx = tmp;
	}
	if (sy > dy) {
		tmp = dy;
		dy = sy;
		sy = tmp;
	}

	tmp = dy - sy;
	if (bl_grp->interlace_on)
		tmp >>= 1;

	if (dx - sx > tmp) {
		for ( ; sy <= dy; sy++)
			bl_grp_line(sx, sy, dx, sy, c, op);
	} else {
		for ( ; sx <= dx; sx++)
			bl_grp_line(sx, sy, sx, dy, c, op);
	}
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
void bl_grp_boxfill_h(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint8_t c)
{
	uint16_t width, height;

	width = dx - sx + 1;
	height = dy - sy + 1;

	*(uint16_t *)(&hmmv_cmd[0]) = sx;
	hmmv_cmd[3] = bl_grp->active_page;
	*(uint16_t *)(&hmmv_cmd[4]) = width;
	hmmv_cmd[8] = c;

	if (bl_grp->interlace_on) {
		hmmv_cmd[2] = (uint8_t)(sy >> 1);
		*(uint16_t *)(&hmmv_cmd[6]) = height >> 1;
		bl_vdp_cmd_hmmv(hmmv_cmd);		/* even page */
		hmmv_cmd[3]++;
		bl_vdp_cmd_hmmv(hmmv_cmd);		/* odd page */
	} else {
		hmmv_cmd[2] = (uint8_t)(sy);
		*(uint16_t *)(&hmmv_cmd[6]) = height;
		bl_vdp_cmd_hmmv(hmmv_cmd);
	}
}

#define bl_grp_cirle4point(cx, cy, x, y)		\
{							\
	bl_grp_put_pixel(cx + x, cy + y, c, op);	\
	if (x != 0)					\
		bl_grp_put_pixel_ext(cx - x, cy + y);	\
	if (y != 0)					\
		bl_grp_put_pixel_ext(cx + x, cy - y);	\
	if (x != 0 && y != 0)				\
		bl_grp_put_pixel_ext(cx - x, cy - y);	\
}

#define bl_grp_cirle4point_ext(cx, cy, x, y)		\
{							\
	bl_grp_put_pixel_ext(cx + x, cy + y);		\
	if (x != 0)					\
		bl_grp_put_pixel_ext(cx - x, cy + y);	\
	if (y != 0)					\
		bl_grp_put_pixel_ext(cx + x, cy - y);	\
	if (x != 0 && y != 0)				\
		bl_grp_put_pixel_ext(cx - x, cy - y);	\
}

void bl_grp_circle(uint16_t cx, uint16_t cy, uint16_t radius, uint8_t c, uint8_t op)
{
	int16_t error = -radius;
	int16_t x = radius;
	int16_t y = 0;

	while (x >= y) {
		bl_grp_cirle4point(cx, cy, x, y);
		if (x != y)
			bl_grp_cirle4point_ext(cx, cy, y, x);

		error += y;
		++y;
		error += y;

		if (error >= 0) {
			error -= x;
			--x;
			error -= x;
		}
	}
}

;