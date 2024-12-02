/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  0BGM
 *
 *  app-mode = 0
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 */

#include <stddef.h>
#include <stdio.h>
#include <blstd.h>
#include <blstdint.h>
#include <cpumode.h>

extern int sub(void);

int main(int argc, char* argv[])
{
/*
	if (get_msx_version() == MSXTR) {
		set_cpu_mode_tr(CPU_TR_R800_DRAM);
	}
*/
	bl_disable_bios_timi();

	sub();

	bl_enable_bios_timi();

	return 0;
}
