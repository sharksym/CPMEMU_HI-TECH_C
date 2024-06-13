/*
 *  HI-TECH C demo by Yeongman Seo <sharksym@hitel.net>
 *
 *  1TIME
 *
 *  app-mode = 1
 *  bank no. = 0
 *  lib-float = 0
 *  lib-r800 = 0
 */

#include <stddef.h>
#include <stdio.h>
#include <time.h>

char str[80];

int main(int argc, char* argv[])
{
	int len;
	char *fmt;
	time_t now;

	time(&now);

	fmt = (argc == 1) ? "%I:%M %p\n%c\n" : argv[1];
	len = strftime(str, sizeof(str), fmt, localtime(&now));

	puts(str);

	return !len;
}
