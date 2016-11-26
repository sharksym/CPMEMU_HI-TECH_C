/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  2HELLO
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
	printf("[BANK #%d] Hello World!\n", bl_bank);

	sub();

	return 0;
}
