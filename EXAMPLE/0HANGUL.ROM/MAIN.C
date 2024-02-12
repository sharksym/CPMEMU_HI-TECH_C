/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  0HANGUL.ROM (BLGRP Hangul Demo, 48KB ROM)
 *
 *  app-mode = 0
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <blstd.h>
#include <blstdint.h>
#include <blgrp.h>


void hangul_demo(void);

int main(int argc, char* argv[])
{
	while (bl_grp_init() < 0);

	bl_disable_bios_timi();
	hangul_demo();
	bl_enable_bios_timi();

	bl_grp_deinit();

	return 0;
}
