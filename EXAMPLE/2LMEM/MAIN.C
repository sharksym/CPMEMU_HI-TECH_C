/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  2LMEM
 *
 *  app-mode = 2
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 */

#include <stddef.h>
#include <stdio.h>
#include <blstd.h>
#include <blstdmem.h>

#define SEG_SIZE	((uint32_t)16 * 1024)

int main(int argc, char* argv[])
{
	struct bl_lmem_t *lmem_buf;
	uint32_t addr;
	int n;

	printf("[ BL_LMEM Test ]\n");

	printf("\nFree lmem = %ldbytes\n", bl_lmem_get_free());

	printf("\nAllocating 64KB buffer...");
	lmem_buf = bl_lmem_alloc(SEG_SIZE * 4);			/* 4 segments */
	if (lmem_buf == NULL) {
		printf("NG\nERROR: lmem alloc fails!\n");
		return 0;
	}
	printf("Ok\n");
	printf("Free lmem = %ldbytes\n", bl_lmem_get_free());

	printf("\nSegment No = ");
	for (n = 0, addr = 0; n < 4; n++, addr += SEG_SIZE) {
		printf("%02XH ", bl_lmem_get_seg(lmem_buf, addr));
	}
	printf("\n");

	bl_lmem_free(lmem_buf);
	printf("\nBuffer released\n");
	printf("Free lmem = %ldbytes\n", bl_lmem_get_free());

	printf("\nTest done\n");

	return 0;
}
