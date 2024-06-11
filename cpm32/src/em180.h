/************************************************************************/
/*		header file for HD64180(Z80) emulator "em180.c"		*/
/*				em180.h					*/
/*		    Copyleft (C) 2004 by K.Murakami			*/
/************************************************************************/

typedef unsigned char  byte;
typedef unsigned short word;

/*======================================================================*/
/* export from em180 							*/
/*======================================================================*/

/*----------------------------------------------------------------------*/
/*  em180 -- HD64180(Z80) emulator entry				*/
/* 	return value: 							*/
/*		0      .. emulation stoped (RETEM)			*/
/*		1      .. break point (HALT)				*/
/*		0x1vv  .. function call (CALLN vv)			*/
/*		0x200- .. illigal op-code 				*/

int em180( void);

#define EMST_STOP 0
#define EMST_HALT 1
#define EMST_FUNC 0x100
#define EMST_UNKOWN 0x200

/*----------------------------------------------------------------------*/
/*  reg -- HD64180(Z80) emulatin registors				*/
union ZREG {
    struct {
	word bc, de, hl, af, ix, iy; 
	word bc2, de2, hl2, af2;
	word sp, pc;
	byte i, r, ie, dmy;
    } x;
    struct { 
#if BIG_ENDIAN
	byte b,c, d,e, h,l, a,f, ixh,ixl, iyh,iyl;
#else
	byte c,b, e,d, l,h, f,a, ixl,ixh, iyl,iyh;
#endif
	word bc2, de2, hl2, af2;
	word sp, pc;
	byte i, r, ie, dmy;
    } b;
};

extern union ZREG reg;

/*----------------------------------------------------------------------*/
/*  mem -- HD64180(Z80) emulatin memory					*/
extern byte mem[ 0x10000];
extern byte guard[ 0x10];

/*======================================================================*/
/* import to em180 							*/
/*======================================================================*/

/*----------------------------------------------------------------------*/
/*  io_input -- i/o read emulation callback function			*/
int io_input( int add);

/*----------------------------------------------------------------------*/
/*  io_output -- i/o write emulation callback function			*/
void io_output( int add, int data);

