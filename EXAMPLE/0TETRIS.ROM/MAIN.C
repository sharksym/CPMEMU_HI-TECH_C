/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  0TETRIS.ROM (BLGRP, BLSND DEMO)
 *  R800 DRAM mode supported
 *
 *  app-mode = 0
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 *  BL_ROM=48
 *  BLBGM_T
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <msxbdos.h>
#include <cpumode.h>
#include <keyio.h>
#include <blstd.h>
#include <blstdint.h>
#include <blgrp.h>
#include <blgrpcmd.h>

/*#define SHOW_CPU_IDLE*/

void resource_init(void);
uint8_t title(void);
void game_loop(void);
void resource_deinit(void);

int main(int argc, char* argv[])
{
	if (get_msx_version() == MSXTR)
		set_cpu_mode_tr(CPU_TR_R800_DRAM);

	while (bl_grp_init() < 0);

	bl_disable_bios_timi();
	resource_init();

	while (title())
		game_loop();

	resource_deinit();
	bl_enable_bios_timi();
	bl_grp_deinit();

	if (get_msx_version() == MSXTR)
		set_cpu_mode_tr(CPU_TR_Z80);

	return 0;
}

/*****************************************************************************/

#ifdef SHOW_CPU_IDLE
static void wait_frame(void)
{
	bl_grp_set_color_border(4);
	bl_wait_frame_rate();
	bl_grp_set_color_border(0);
}
#define bl_wait_frame_rate() wait_frame()
#endif

void bgm_init(void);
void bgm_deinit(void);
void bgm_enqueue_main(void);
void bgm_play(void);
void bgm_stop(void);
void bgm_effect_rotate(void);
void bgm_effect_fall(void);
void bgm_effect_move(void);
void bgm_effect_erase(void);

#define LAYOUT_W	14
#define LAYOUT_H	23
#define LAYOUT_POS_X	(8 * 9)
#define LAYOUT_POS_Y	(8 * 2)

#define LEVEL_MAX	10
#define FALL_STEP_1BLK	16
#define STOP_DELAY	10
#define KEY_RPT_DELAY	15
#define KEY_RPT_MAX	90

#define STATE_IDLE	0
#define STATE_LEFT	1
#define STATE_RIGHT	2
#define STATE_DROP	3

#define BLOCK_NEW_POS_X	5
#define BLOCK_NEW_POS_Y	0

static uint16_t score, level;
static uint16_t blk_stat[7];
static uint8_t blk_next0, blk_next1;
static uint8_t blk_type, blk_dir;
static uint16_t blk_pos_x, blk_pos_y, blk_scr_x, blk_scr_y;

static uint8_t blk_pos_main[4][2];
static uint8_t ghost_pos[4][2];

static uint8_t req_left, req_right;
static uint8_t req_up, req_down;
static uint8_t req_drop;
static uint8_t key_left_time, key_right_time;
static uint8_t key_up_last, key_space_last;

static uint8_t blk_move_cnt;
static uint16_t blk_fall_cnt;
static uint8_t blk_stop_cnt;
static uint8_t fall_step[] = { 0, 2, 3, 5, 7, 11, 16, 23, 32, 48, 64 };
static uint16_t move_offset[] = { 3, 2, 2, 1, 0 };

static uint8_t layout[LAYOUT_H][LAYOUT_W];
static uint8_t layout_init[LAYOUT_H][LAYOUT_W] = {
	{ 0,0,9,9,9,9,9,9,9,9,9,9,0,0 },
	{ 0,0,9,9,9,9,9,9,9,9,9,9,0,0 },
	{ 0,0,9,9,9,9,9,9,9,9,9,9,0,0 },
	{ 0,0,9,9,9,9,9,9,9,9,9,9,0,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,9,9,9,9,9,9,9,9,9,9,8,0 },
	{ 0,8,8,8,8,8,8,8,8,8,8,8,8,0 },
};

static uint8_t blk_pal[8][3] = {
	{ 1, 6, 7 },
	{ 1, 1, 7 },
	{ 7, 4, 0 },
	{ 6, 6, 0 },
	{ 0, 6, 0 },
	{ 7, 0, 7 },
	{ 7, 0, 0 },
	{ 5, 5, 5 }				/* border block */
};

void setup_palette(void)
{
	uint8_t n;

	bl_grp_set_palette(0, 0, 0, 0);
	bl_grp_set_palette(1, 0, 0, 0);

	for (n = 0; n < 8; n++) {
		bl_grp_set_palette(n + 2, blk_pal[n][0], blk_pal[n][1], blk_pal[n][2]);
	}

	bl_grp_set_palette(13, 4, 4, 4);	/* block grey */
	bl_grp_set_palette(14, 7, 7, 7);	/* block white */
	bl_grp_set_palette(15, 6, 0, 0);	/* block main */
}

static uint8_t blk0_spr[] = {
	0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x00
};
static uint8_t blk0_col[] = {
	0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E
};
static uint8_t blk1_spr[] = {
	0x01,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0xFF
};
static uint8_t blk1_col[] = {
	0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D,0x4D
};
static uint8_t blk2_spr[] = {
	0xAA,0x00,0x82,0x00,0x82,0x00,0xAA,0x00
};
static uint8_t blk2_col[] = {
	0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F
};

void setup_sprite(void)
{
	bl_grp_clear_sprite();

	bl_grp_set_sprite_mode(GRP_SPR_8);
	bl_grp_set_sprite_pattern_8x8(0, blk0_spr);	/* center */
	bl_grp_set_sprite_pattern_8x8(1, blk1_spr);	/* border */
	bl_grp_set_sprite_pattern_8x8(2, blk2_spr);	/* ghost */

	bl_grp_set_sprite_color(0, blk0_col);
	bl_grp_set_sprite_color(1, blk1_col);
	bl_grp_set_sprite_color(2, blk0_col);
	bl_grp_set_sprite_color(3, blk1_col);
	bl_grp_set_sprite_color(4, blk0_col);
	bl_grp_set_sprite_color(5, blk1_col);
	bl_grp_set_sprite_color(6, blk0_col);
	bl_grp_set_sprite_color(7, blk1_col);
	bl_grp_set_sprite_color(8, blk2_col);
	bl_grp_set_sprite_color(9, blk2_col);
	bl_grp_set_sprite_color(10, blk2_col);
	bl_grp_set_sprite_color(11, blk2_col);
}

static uint16_t block_shape[7][4] = {
	{ 0xCDEF, 0x159D, 0xCDEF, 0x159D },
	{ 0x89AE, 0x569D, 0x8CDE, 0x6ADE },
	{ 0x89AC, 0x59DE, 0xACDE, 0x56AE },
	{ 0x9ADE, 0x9ADE, 0x9ADE, 0x9ADE },
	{ 0x9ACD, 0x59AE, 0x9ACD, 0x59AE },
	{ 0x89AD, 0x59AD, 0x9CDE, 0x589D },
	{ 0x89DE, 0x69AD, 0x89DE, 0x69AD }
};

static uint8_t block_pos[16][2] = {		/* x, y offset */
	{ 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 },
	{ 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 },
	{ 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 },
	{ 0, 3 }, { 1, 3 }, { 2, 3 }, { 3, 3 }
};

static uint8_t block_pos2[16][2] = {		/* x, y offset */
	{ 0*8, 0*8 }, { 1*8, 0*8 }, { 2*8, 0*8 }, { 3*8, 0*8 },
	{ 0*8, 1*8 }, { 1*8, 1*8 }, { 2*8, 1*8 }, { 3*8, 1*8 },
	{ 0*8, 2*8 }, { 1*8, 2*8 }, { 2*8, 2*8 }, { 3*8, 2*8 },
	{ 0*8, 3*8 }, { 1*8, 3*8 }, { 2*8, 3*8 }, { 3*8, 3*8 }
};

void draw_block(void)
{
	uint16_t shape, x, y;
	uint8_t n, pos;

	shape = block_shape[blk_type][blk_dir];
	for (n = 0; n < 4; n++, shape >>= 4) {
		pos = shape & 0x0F;
		x = blk_pos_x + block_pos[pos][0];
		y = blk_pos_y + block_pos[pos][1];
		layout[y][x] = blk_type;

		x <<= 3;
		y <<= 3;
		x += LAYOUT_POS_X;
		y += LAYOUT_POS_Y;

		bl_grp_hcopy_v2v_p(0, blk_type * 8, 8, 8, x, y, 0);
	}
}

void draw_block_any(uint16_t sx, uint16_t sy, uint8_t type, uint8_t dir)
{
	uint16_t shape;
	uint16_t x, y;
	uint8_t n, pos;

	shape = block_shape[type][dir];
	for (n = 0; n < 4; n++, shape >>= 4) {
		pos = shape & 0x0F;
		x = sx + block_pos2[pos][0];
		y = sy + block_pos2[pos][1];
		bl_grp_hcopy_v2v_p(0, type * 8, 8, 8, x, y, 0);
	}
}

uint8_t check_layout_empty(uint8_t sx, uint8_t sy)
{
	uint16_t shape;
	uint8_t n, pos;
	uint8_t x, y;

	shape = block_shape[blk_type][blk_dir];
	for (n = 0; n < 4; n++, shape >>= 4) {
		pos = shape & 0x0F;
		x = sx + block_pos[pos][0];
		y = sy + block_pos[pos][1];
		if (layout[y][x] < 9)
			return 0;
	}
	return 1;
}

void setup_block_pos(void)
{
	uint16_t shape, x, y;
	uint8_t n, pos;

	shape = block_shape[blk_type][blk_dir];
	for (n = 0; n < 4; n++, shape >>= 4) {
		pos = shape & 0x0F;
		x = blk_scr_x + block_pos2[pos][0];
		y = blk_scr_y + block_pos2[pos][1];

		blk_pos_main[n][0] = x;
		blk_pos_main[n][1] = y;
	}
}

void setup_ghost_pos(void)
{
	uint16_t shape, x, y, sy;
	uint8_t n, pos;

	shape = block_shape[blk_type][blk_dir];
	for (sy = blk_pos_y; sy < LAYOUT_H - 4; sy++){
		if (!check_layout_empty(blk_pos_x, sy + 1)) {
			break;
		}
	}

	for (n = 0; n < 4; n++, shape >>= 4) {
		pos = shape & 0x0F;
		x = blk_pos_x + block_pos[pos][0];
		y = sy + block_pos[pos][1];

		x <<= 3;
		y <<= 3;
		x += LAYOUT_POS_X;
		y += LAYOUT_POS_Y;

		ghost_pos[n][0] = x;
		ghost_pos[n][1] = y;
	}
}

void update_block_spr(void)
{
	bl_grp_put_sprite(0, blk_pos_main[0][0], blk_pos_main[0][1], 0, 0);
	bl_grp_put_sprite(1, blk_pos_main[0][0], blk_pos_main[0][1], 0, 1);

	bl_grp_put_sprite(2, blk_pos_main[1][0], blk_pos_main[1][1], 0, 0);
	bl_grp_put_sprite(3, blk_pos_main[1][0], blk_pos_main[1][1], 0, 1);

	bl_grp_put_sprite(4, blk_pos_main[2][0], blk_pos_main[2][1], 0, 0);
	bl_grp_put_sprite(5, blk_pos_main[2][0], blk_pos_main[2][1], 0, 1);

	bl_grp_put_sprite(6, blk_pos_main[3][0], blk_pos_main[3][1], 0, 0);
	bl_grp_put_sprite(7, blk_pos_main[3][0], blk_pos_main[3][1], 0, 1);
}

void update_ghost_spr(void)
{
	bl_grp_put_sprite(8, ghost_pos[0][0], ghost_pos[0][1], 0, 2);
	bl_grp_put_sprite(9, ghost_pos[1][0], ghost_pos[1][1], 0, 2);
	bl_grp_put_sprite(10, ghost_pos[2][0], ghost_pos[2][1], 0, 2);
	bl_grp_put_sprite(11, ghost_pos[3][0], ghost_pos[3][1], 0, 2);
}

uint8_t sprite_page = 0;
void sprite_page_flip(void)
{
	bl_grp_set_sprite_view(sprite_page);
	sprite_page = sprite_page ? 0 : 1;
	bl_grp_set_sprite_active(sprite_page);

}

void turn_off_block_spr(void)
{
	uint8_t n;

	for (n = 0; n < 12; n++)
		bl_grp_put_sprite(n, 0, 240, 0, 0);

	sprite_page_flip();
}

void scroll_layout(uint8_t dy)
{
	uint8_t x;
	uint16_t sx, sy, n, nn;

	bl_wait_frame_rate();
	sy = LAYOUT_POS_Y + dy * 8;
	for (n = 1, nn = 1 * 8; n < 8; n++, nn += 8) {
		sx = LAYOUT_POS_X + 2 * 8;
		bl_wait_frame_rate();
		for (x = 2; x < 12; x++, sx += 8) {
			bl_grp_hcopy_v2v_p(nn, layout[dy][x] << 3, 8, 8, sx, sy, 0);
		}
	}

	sx = LAYOUT_POS_X + 2 * 8;
	for (n = dy - 1; dy > 0; n--, dy--, sy -= 8) {
		for (x = 2; x < 12; x++) {
			layout[dy][x] = layout[n][x];
		}
		bl_grp_hcopy_v2v(sx, sy - 8, 10 * 8, 8, sx, sy);
	}
}

uint8_t check_line_full(uint8_t y)
{
	uint8_t x;

	for (x = 2; x < 12; x++) {
		if (layout[y][x] == 9)
			return 0;
	}

	return 1;
}

uint8_t check_avail(void)
{
	if (!blk_pos_y)
		return 0;
	if (!check_layout_empty(blk_pos_x, blk_pos_y))
		return 0;
	if (!blk_fall_cnt)
		return 1;
	if (!check_layout_empty(blk_pos_x, blk_pos_y - 1))
		return 0;

	return 1;
}

uint8_t check_left_avail(void)
{
	if (!blk_pos_y)
		return 0;
	if (!check_layout_empty(blk_pos_x - 1, blk_pos_y))
		return 0;
	if (!blk_fall_cnt)
		return 1;
	if (!check_layout_empty(blk_pos_x - 1, blk_pos_y - 1))
		return 0;

	return 1;
}

uint8_t check_right_avail(void)
{
	if (!blk_pos_y)
		return 0;
	if (!check_layout_empty(blk_pos_x + 1, blk_pos_y))
		return 0;
	if (!blk_fall_cnt)
		return 1;
	if (!check_layout_empty(blk_pos_x + 1, blk_pos_y - 1))
		return 0;

	return 1;
}

void check_layout_fill(uint8_t y)
{
	uint8_t n;

	for (n = y + 3; n >= y; n--) {
		if (check_line_full(n)) {
			bgm_effect_erase();
			scroll_layout(n);
			n++;
			score++;
			if (!(score % 10) && (level < LEVEL_MAX))
				level++;
		}
	}
}

void setup_block_image(void)
{
	uint16_t n, k, sx, sy, size;

	for (n = 0; n < 8; n++) {
		sx = 0;
		sy = n * 8;
		for (k = 0, size = 7; k < 3; k++, sx += 8, size--) {
			bl_grp_box(sx, sy, sx + size, sy + size, 13, GRP_OP_IMP);
			bl_grp_box(sx, sy, sx + size - 1, sy + size - 1, 14, GRP_OP_IMP);
			bl_grp_boxfill(sx + 1, sy + 1, sx + size - 1, sy + size - 1, n + 2, GRP_OP_IMP);

			sx += 8;
			sx++;
			sy++;
			size--;
			bl_grp_box(sx, sy, sx + size, sy + size, 13, GRP_OP_IMP);
			bl_grp_box(sx, sy, sx + size - 1, sy + size - 1, 14, GRP_OP_IMP);
			bl_grp_boxfill(sx + 1, sy + 1, sx + size - 1, sy + size - 1, n + 2, GRP_OP_IMP);
		}
		bl_grp_box(sx, sy, sx + 1, sy + 1, 13, GRP_OP_IMP);
		bl_grp_box(sx, sy, sx + 0, sy + 0, 14, GRP_OP_IMP);
	}
	bl_grp_boxfill(0 + 2, 56 + 2, 0 + 5, 56 + 5, 13, GRP_OP_IMP);
}

static char *str_buf;
void print_init(void)
{
	str_buf = (char *)bl_malloc(256);
	bl_grp_set_font_size(6, 8);
}

void print_deinit(void)
{
	bl_free(str_buf);
}

void print(uint16_t x, uint16_t y, char *str)
{
	strcpy(str_buf, str);
	bl_grp_print(x, y, str_buf);
}

void resource_init(void)
{
	bgm_init();

	bl_grp_set_display_on(0);
	bl_grp_set_sprite_on(0);

	bl_grp_set_screen_mode(GRP_SCR_G4);
	bl_grp_set_display_mode(GRP_DISP_240P);
	bl_grp_set_color_border(0);
	bl_grp_set_active(0);
	bl_grp_set_view(0);

	bl_grp_erase(0, 0x11);		/* block image & sprite */
	bl_grp_erase(1, 0x11);		/* title */
	bl_grp_erase(2, 0x11);		/* main (bg color = 1)*/
/*	bl_grp_erase(3, 0x11);*/

	print_init();
	sprite_page_flip();
	setup_sprite();
	sprite_page_flip();
	setup_sprite();
	setup_palette();
	setup_block_image();
}

void resource_deinit(void)
{
	print_deinit();

	bl_grp_set_color_border(4);
	bl_grp_set_active(0);
	bl_grp_set_view(0);
	bl_grp_reset_palette();

	bgm_deinit();
}

uint8_t title_layout[7][6 + 6 + 6 + 6 + 2 + 5] = {
	{ 1,1,1,1,1,7, 2,2,2,2,2,7, 3,3,3,3,3,7, 7,4,4,4,4,7, 5,7, 7,6,6,6,7 },
	{ 7,7,1,7,7,7, 2,7,7,7,7,7, 7,7,3,7,7,7, 4,7,7,7,4,7, 5,7, 6,7,7,7,6 },
	{ 7,7,1,7,7,7, 2,7,7,7,7,7, 7,7,3,7,7,7, 4,7,7,7,4,7, 5,7, 6,7,7,7,7 },
	{ 7,7,1,7,7,7, 2,2,2,2,7,7, 7,7,3,7,7,7, 7,4,4,4,4,7, 5,7, 7,6,6,6,7 },
	{ 7,7,1,7,7,7, 2,7,7,7,7,7, 7,7,3,7,7,7, 7,7,4,7,4,7, 5,7, 7,7,7,7,6 },
	{ 7,7,1,7,7,7, 2,7,7,7,7,7, 7,7,3,7,7,7, 7,4,7,7,4,7, 5,7, 6,7,7,7,6 },
	{ 7,7,1,7,7,7, 2,2,2,2,2,7, 7,7,3,7,7,7, 4,7,7,7,4,7, 5,7, 7,6,6,6,7 },
};

uint8_t title(void)
{
	uint16_t x, y;
	int8_t n;

	bl_set_frame_rate(1);
	bl_wait_frame_rate();
	bl_grp_set_display_on(0);
	bl_grp_set_font_color(14, 1);
	bl_wait_frame_rate();
	bl_grp_set_active(1);
	bl_grp_set_view(1);
	bl_grp_boxfill(0, 0, 255, 211, 1, GRP_OP_IMP);
	bl_wait_frame_rate();
	bl_grp_set_display_on(1);

	bl_set_frame_rate(2);
	for (y = 0; y < 7; y++) {
		bl_wait_frame_rate();
		bl_grp_hcopy_v2v(0, 80 - (6 * 8), 31 * 8, 7 * 8, 0, 80 - (7 * 8));
		for (n = 6; n >=0; n--) {
			bl_wait_frame_rate();
			for (x = 0; x < 31; x++) {
				bl_grp_hcopy_v2v_p(n * 8, title_layout[y][x] * 8, 8, 8, x * 8, 80, 0);
			}
		}
	}

	for (n = 1; n <= 8; n++) {
		bl_grp_set_font_size(6, n);
		bl_wait_frame_rate();
		print(128 + 48, 128 - 24 - n, "By sharksym");
		print(84, 140 - n, "Push SPACE key");
	}

	for (n = 0; !key_clicked(KEY_SPACE); n = (n + 1) & 0xF) {
		bl_grp_set_font_color(n, 1);
		print(84, 140 - 8, "Push SPACE key");
		if (key_clicked(KEY_ESC))
			return 0;
	}

	return 1;
}

void reset_layout(void)
{
	uint16_t x, y, sx, sy;

	memcpy(layout, layout_init, sizeof(layout_init));

	for (y = 0; y < LAYOUT_H; y++) {
		for (x = 0; x < LAYOUT_W; x++) {
			sx = LAYOUT_POS_X + x * 8;
			sy = LAYOUT_POS_Y + y * 8;
			if (layout[y][x] == 8) {
				bl_grp_hcopy_v2v_p(0, 7 * 8, 8, 8, sx, sy, 0);
			} else if (layout[y][x] == 9) {	/* empty */
				bl_grp_hcopy_v2v_p(0, 8 * 8, 8, 8, sx, sy, 0);
				bl_grp_boxfill(sx, sy, sx + 7, sy + 7, 1, GRP_OP_IMP);
			}
		}
	}

	print(0, 8, "STATISTICS");
	for (y = 0; y < 7; y++) {
		draw_block_any(0, 8 + y * 8 * 3, y, 0);
		blk_stat[y] = 0;
	}

	blk_next0 = bl_random() % 7;
	blk_next1 = bl_random() % 7;
	print(8 * 23, 8, "NEXT:");
	bl_grp_boxfill(8 * 23, 8 + 8 * 2, (8 * 32), 8 + 8 * 4, 1, GRP_OP_IMP);
	draw_block_any((8 * 23), 8, blk_next0, 0);
	draw_block_any((8 * 28), 8, blk_next1, 0);

	print(8 * 23, 72, "SCORE:");
	print(8 * 23, 72 + 16, "LEVEL:");
}

void update_score(void)
{
	uint16_t y;
	char str[32];

	for (y = 0; y < 7; y++) {
		sprintf(str, "%03d", blk_stat[y]);
		print(8 * 5, 32 + y * 8 * 3, str);
	}

	sprintf(str, "%05d", score);
	print(8 * 23 + 36, 72, str);

	sprintf(str, "%02d", level);
	print(8 * 23 + 36, 72 + 16, str);
}

uint8_t get_next_blk(void)
{
	uint8_t type = blk_next0;

	blk_next0 = blk_next1;
	blk_next1 = bl_random() % 7;

	bl_wait_frame_rate();
	bl_grp_hcopy_v2v(8 * 28, 8 + 8 * 2, (8 * 32), 8 + 8 * 4, 8 * 23, 8 + 8 * 2);
	bl_grp_boxfill(8 * 28, 8 + 8 * 2, (8 * 32), 8 + 8 * 4, 1, GRP_OP_IMP);
	draw_block_any(8 * 28, 8, blk_next1, 0);

	blk_stat[type]++;

	return type;
}

void setup_new_blk(void)
{
	blk_pos_x = BLOCK_NEW_POS_X;
	blk_pos_y = BLOCK_NEW_POS_Y;
	blk_scr_x = LAYOUT_POS_X + blk_pos_x * 8;
	blk_scr_y = LAYOUT_POS_Y + blk_pos_y * 8;
	blk_type = get_next_blk();
	blk_dir = 0;
}

#define set_blk_color(type) \
	bl_grp_set_palette(15, blk_pal[type][0], blk_pal[type][1], blk_pal[type][2]);

void key_req_init(void)
{
	key_left_time = 0;
	key_right_time = 0;
	key_up_last = 0;
	key_space_last = 0;
}

void key_req_clear(void)
{
	req_left = 0;
	req_right = 0;
	req_up = 0;
	req_down = 0;
	req_drop = 0;
}

void key_req_update(void)
{
	if (key_pressed(KEY_SPACE)) {
		if (!key_space_last) {
			key_space_last = 1;
			req_drop = 1;
		}
	} else {
		key_space_last = 0;
	}

	if (key_pressed(KEY_UP)) {
		if (!key_up_last) {
			key_up_last = 1;
			req_up = 1;
		}
	} else {
		key_up_last = 0;
	}

	if (key_pressed(KEY_LEFT)) {
		if (!key_left_time || (key_left_time > KEY_RPT_DELAY))
			req_left = 1;

		if (key_left_time < KEY_RPT_MAX)
			key_left_time++;
	} else {
		key_left_time = 0;
	}

	if (key_pressed(KEY_RIGHT)) {
		if (!key_right_time || (key_right_time > KEY_RPT_DELAY))
			req_right = 1;

		if (key_right_time < KEY_RPT_MAX)
			key_right_time++;
	} else {
		key_right_time = 0;
	}

	if (key_pressed(KEY_DOWN))
		req_down = 1;
}

void update_screen(void)
{
	setup_block_pos();
	update_block_spr();
	update_ghost_spr();
	bl_wait_frame_rate();
	sprite_page_flip();
}

void game_loop(void)
{
	uint8_t state;
	uint8_t n;

	bl_set_frame_rate(1);
	bl_grp_set_active(2);
	bl_grp_set_view(2);
	bl_grp_set_font_color(14, 1);

	bl_grp_set_display_on(0);

	score = 0;
	level = 1;
	reset_layout();
	sprite_page_flip();
	bl_grp_set_display_on(1);
	bl_grp_set_sprite_on(1);

	setup_new_blk();
	setup_ghost_pos();
	state = STATE_IDLE;
	blk_move_cnt = 0;
	blk_fall_cnt = 0;
	blk_stop_cnt = 0;

	turn_off_block_spr();
	set_blk_color(blk_type);
	update_score();
	key_req_init();

	bgm_enqueue_main();
	bgm_play();

	while (!key_pressed(KEY_ESC)) {
		key_req_clear();
		key_req_update();

		switch (state) {
		case STATE_IDLE:
			if (req_drop) {
				state = STATE_DROP;
				bgm_effect_fall();
				if (blk_fall_cnt) {
					blk_scr_y &= 0xF8;
					blk_fall_cnt = 0;
				}
			} else if (req_left) {
				if (check_left_avail()) {
					bgm_effect_move();
					state = STATE_LEFT;
					blk_move_cnt = 0;
					blk_scr_x -= move_offset[blk_move_cnt++];
					blk_pos_x--;
					setup_ghost_pos();
				}
			} else if (req_right) {
				if (check_right_avail()) {
					bgm_effect_move();
					state = STATE_RIGHT;
					blk_move_cnt = 0;
					blk_scr_x += move_offset[blk_move_cnt++];
					blk_pos_x++;
					setup_ghost_pos();
				}
			}
			break;
		case STATE_LEFT:
			if (req_drop) {
				while (move_offset[blk_move_cnt])
					blk_scr_x -= move_offset[blk_move_cnt++];
				bgm_effect_fall();
				state = STATE_DROP;
			} else {
				blk_scr_x -= move_offset[blk_move_cnt++];
				if (!move_offset[blk_move_cnt])
					state = STATE_IDLE;
			}
			break;
		case STATE_RIGHT:
			if (req_drop) {
				while (move_offset[blk_move_cnt])
					blk_scr_x += move_offset[blk_move_cnt++];
				bgm_effect_fall();
				state = STATE_DROP;
			} else {
				blk_scr_x += move_offset[blk_move_cnt++];
				if (!move_offset[blk_move_cnt])
					state = STATE_IDLE;
			}
			break;
		case STATE_DROP:
			if (blk_fall_cnt) {
				blk_scr_y &= 0xF8;
				blk_fall_cnt = 0;
			}
			break;
		default:
			break;
		}

		if (req_up && (state != STATE_DROP)) {
			bgm_effect_rotate();
			n = blk_dir;
			blk_dir = (blk_dir + 1) & 0x03;
			if (check_avail())
				setup_ghost_pos();
			else
				blk_dir = n;
		}

		if (!blk_fall_cnt) {
			if (check_layout_empty(blk_pos_x, blk_pos_y + 1)) {
				blk_stop_cnt = 0;
				if (state == STATE_DROP) {
					blk_scr_y += 8;
					blk_pos_y++;
				} else {
					blk_fall_cnt = 1;
					blk_scr_y++;
					blk_pos_y++;
				}
			} else {
			 	blk_stop_cnt++;
				if ((blk_stop_cnt == STOP_DELAY) || (state == STATE_DROP)) {
					state = STATE_IDLE;
					draw_block();
					bl_wait_frame_rate();
					turn_off_block_spr();
					check_layout_fill(blk_pos_y);
					setup_new_blk();
					setup_ghost_pos();
					bl_wait_frame_rate();
					set_blk_color(blk_type);
					update_score();

					if (!check_layout_empty(blk_pos_x, blk_pos_y + 1)) {
						update_screen();
						break;
					}
				}
			}
		} else {
			if (req_down && (fall_step[level] < (FALL_STEP_1BLK * 2))) {
				for (n = 0; n < 2; n++) {
					blk_fall_cnt += FALL_STEP_1BLK;
					blk_scr_y++;
					if (blk_fall_cnt >= (7 * FALL_STEP_1BLK + 1)) {
						blk_fall_cnt = 0;
						break;
					}
				}
			} else {
				for (n = 0; n < fall_step[level]; n++) {
					blk_fall_cnt++;
					if ((blk_fall_cnt % FALL_STEP_1BLK) == 1) {
						blk_scr_y++;
						if (blk_fall_cnt == (7 * FALL_STEP_1BLK + 1)) {
							blk_fall_cnt = 0;
							break;
						}
					}
				}
			}
		}

		update_screen();
	}

	bgm_stop();

	turn_off_block_spr();
	while (key_pressed(KEY_ESC) || key_pressed(KEY_SPACE));

	for (n = 0; !key_clicked(KEY_ESC) && !key_clicked(KEY_SPACE); n = (n + 1) & 0xF) {
		bl_grp_set_font_color(n, 1);
		print(LAYOUT_POS_X + 30, 96, "GAME OVER");
	}
}
