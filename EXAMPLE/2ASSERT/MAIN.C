/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  2ASSERT
 *
 *  app-mode = 2
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 */

#include <stddef.h>
#include <stdio.h>
#include <blstd.h>

extern int sub(void);

int main(int argc, char* argv[])
{
	printf("[BANK #%d] Assert Test...\n", bl_bank);

	sub();

	printf("This message canbe displayed whith NDEBUG.");

	return 0;
}
