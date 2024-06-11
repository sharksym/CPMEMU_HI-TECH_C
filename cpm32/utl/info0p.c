/*************************/
/* ZERO PAGE INFO        */
/*     by Keiji Murakami */
/*************************/

#include <stdio.h>

int main( int argc, char *argv[])
{
	unsigned i, n;
	unsigned char *p;

	printf( "BDOS ENTRY: %04x\n", *(unsigned short *)0x06);
	printf( "BIOS WBOOT: %04x\n", *(unsigned short *)0x01);
	printf( "BIOS START: %04x\n", *(unsigned short *)0x01 - 3);
	n = *(unsigned short *)0x06 - 0x100;
    printf( "TPA SIZE: %u (%04x) bytes\n", n, n);

	p = (unsigned char *)0x5c;
	n = *p++ + '@';
	printf( "FCB1: %c:%.13s\n", n, p);
	p = (unsigned char *)0x6c;
	n = *p++ + '@';
	printf( "FCB2: %c:%.13s\n", n, p);

	p = (unsigned char *)0x80;
	n = *p++;
	printf( "CMD LINE: (%d) ", n);
	if ( n > 127) n = 127;
	for ( i = 0; i < n; i++) putchar( *p++);
	printf( "\n");

	return 0;
}

