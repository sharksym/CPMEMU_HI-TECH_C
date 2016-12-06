/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  2BGM
 *
 *  app-mode = 2
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 */

#include <stddef.h>
#include <stdio.h>
#include <blstd.h>
#include <blstdint.h>

extern int sub(void);

int main(int argc, char* argv[])
{
	bl_disable_bios_timi();

	sub();

	bl_enable_bios_timi();

	return 0;
}
