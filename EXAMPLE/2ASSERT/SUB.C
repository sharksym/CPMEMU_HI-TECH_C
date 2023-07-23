#include <stddef.h>
#include <stdio.h>
#include <blstd.h>
#include <blassert.h>

int sub(void)
{
	printf("[BANK #%d] Hello World!\n", bl_bank);

	assert(bl_bank == 1);

	return 0;
}
