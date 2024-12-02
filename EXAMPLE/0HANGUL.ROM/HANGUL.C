/*
 *  HANGUL DEMO
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <keyio.h>
#include <blstd.h>
#include <blgrp.h>

static char *str_buf;
void print_init(void)
{
	str_buf = (char *)bl_malloc(256);
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

void print_demo(uint8_t scr_mode, uint8_t disp_mode, char *str, uint8_t font_col)
{
	bl_set_frame_rate(1);
	bl_wait_frame_rate();
	bl_wait_frame_rate();

	bl_grp_set_display_on(0);
	bl_grp_set_screen_mode(scr_mode);
	bl_grp_set_display_mode(disp_mode);

	bl_grp_set_color_border(0);
	bl_grp_set_active(0);
	bl_grp_set_view(0);
	bl_grp_erase(0, 0);

	bl_grp_set_font_color(font_col, 0);
	bl_grp_set_font_size(8, 16);
	bl_grp_set_print_kr(1);

	bl_wait_frame_rate();
	bl_grp_set_display_on(1);

	print(20,  60, "[ HANGUL DEMO ]");
	print(20,  80, str);
	print(20, 100, "EUC-KR 입니다!");
	print(20, 120, "ESC 키를 누르세요...");

	while (!key_clicked(KEY_ESC));
}

void hangul_demo(void)
{
	print_init();

	/* SCREEN 5,6,7,8 */
	print_demo(GRP_SCR_G4, GRP_DISP_240P, "스크린5 240p",  15);
	print_demo(GRP_SCR_G4, GRP_DISP_480I, "스크린5 480i",  15);
	print_demo(GRP_SCR_G5, GRP_DISP_240P, "스크린6 240p",   3);
	print_demo(GRP_SCR_G5, GRP_DISP_480I, "스크린6 480i",   3);
	print_demo(GRP_SCR_G6, GRP_DISP_240P, "스크린7 240p",  15);
	print_demo(GRP_SCR_G6, GRP_DISP_480I, "스크린7 480i",  15);
	print_demo(GRP_SCR_G7, GRP_DISP_240P, "스크린8 240p", 255);
	print_demo(GRP_SCR_G7, GRP_DISP_480I, "스크린8 480i", 255);

	print_deinit();
}

;
