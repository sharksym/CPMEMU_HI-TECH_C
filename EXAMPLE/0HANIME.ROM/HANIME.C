/*
 *  HANGUL IME DEMO
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <keyio.h>
#include <led.h>
#include <ime.h>
#include <blstd.h>
#include <blgrp.h>
#include <blgrpcmd.h>

#define __ENABLE_CURSOR__

#ifndef __ENABLE_CURSOR__
#define bl_grp_print_cursor_draw()
#define bl_grp_print_cursor_erase()
#endif

static char hangul_ime = 0;
static void enable_hangul_ime(void)
{
	hangul_ime = 1;
	led_kana_on();
	ime_kr_init(IME_KR_JOHAB);
}

static void disable_hangul_ime(void)
{
	hangul_ime = 0;
	led_kana_off();
	ime_kr_deinit();
}

void toggle_hangul_ime(void)
{
	if (hangul_ime)
		disable_hangul_ime();
	else
		enable_hangul_ime();
}

void hangul_ime_demo(void)
{
	static unsigned char *ime_out;
	static unsigned char code;

	bl_grp_print(0,  0, "[ HANGUL IME DEMO ]");
	bl_grp_print(0,  20, "스크린7 240p");
	bl_grp_print(0,  40, "한글을 입력하세요...");
	bl_grp_box(0, 64 - 4, 511, 64 + 16 * 8 + 4, 15, GRP_OP_TIMP);

	bl_grp_set_print_kr(2);
	bl_grp_print_pos(4, 64 + 16 * 6);
	bl_grp_print_cursor_draw();

	enable_hangul_ime();

	do {
		bl_wait_frame_rate();
		code = key_getch();
		if (code) {
			if (code == KEY_CH_KANA) {
				toggle_hangul_ime();
				continue;
			}

			if (hangul_ime) {
				ime_out = ime_kr_send(code);
				if (*ime_out) {
					bl_grp_print_cursor_erase();
					bl_grp_print_str(ime_out);
					bl_grp_print_cursor_draw();
					continue;
				}
			}

			bl_grp_print_cursor_erase();
			if (code == '\r') {
				bl_grp_hcopy_v2v(0, 64 + 16, 512, 16 * 7, 0, 64);
				bl_grp_cmd_wait();
				bl_grp_print_pos(4, 64 + 16 * 6);
			} else {
				bl_grp_print_asc(code);
			}
			bl_grp_print_cursor_draw();
		}
	} while (code != KEY_CH_ESC);		/* ESC key */

	disable_hangul_ime();
}
