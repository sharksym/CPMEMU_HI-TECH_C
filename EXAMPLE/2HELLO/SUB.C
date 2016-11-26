#include <stddef.h>
#include <stdio.h>
#include <blstd.h>

int sub(void)
{
	printf("[BANK #%d] Hello World!\n", bl_bank);

	return 0;
}

