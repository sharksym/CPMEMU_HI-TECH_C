/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  2TETRIS (BLGRP, BLSND DEMO)
 *
 *  app-mode = 2
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys.h>
#include <io.h>
#include <msxbdos.h>
#include <blstd.h>
#include <blstdint.h>
#include <blgrp.h>


void resource_init(void);
uint8_t title(void);
void game_loop(void);
void resource_deinit(void);

int main(int argc, char* argv[])
{
	if (bl_grp_init() < 0) {
		printf("grp init error!\n");
		return 0;
	}

	bl_disable_bios_timi();
	resource_init();

	while (title())
		game_loop();

	resource_deinit();
	bl_enable_bios_timi();
	bl_grp_deinit();

	return 0;
}

;