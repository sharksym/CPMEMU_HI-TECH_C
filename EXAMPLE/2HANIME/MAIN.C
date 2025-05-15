/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  2HANIME.ROM (BLGRP Hangul IME Demo)
 *
 *  app-mode = 1
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 */

#include <stddef.h>
#include <stdio.h>
#include <io.h>
#include <blstd.h>
#include <blstdint.h>
#include <blgrp.h>

void screen_init(void);
void hangul_ime_demo(void);

int main(int argc, char* argv[])
{
	while (bl_grp_init() < 0);

	bl_disable_bios_timi();

	screen_init();
	hangul_ime_demo();

	bl_enable_bios_timi();

	bl_grp_deinit();

	return 0;
}

void screen_init(void)
{
	bl_set_frame_rate(1);
	bl_wait_frame_rate();
	bl_wait_frame_rate();

	bl_grp_set_display_on(0);
	bl_grp_set_screen_mode(GRP_SCR_G6);
	bl_grp_set_display_mode(GRP_DISP_240P);

	bl_grp_set_color_border(0);
	bl_grp_set_active(0);
	bl_grp_set_view(0);
	bl_grp_erase(0, 0);

	bl_grp_set_font_color(15, 0);
	bl_grp_set_font_size(8, 16);
	bl_grp_set_print_kr(1);

	bl_wait_frame_rate();
	bl_grp_set_display_on(1);
}
