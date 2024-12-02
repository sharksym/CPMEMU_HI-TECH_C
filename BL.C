/**********************************************************************
 *
 * BL: MSX-DOS2 Banked Application OVL
 *
 * 0000H~00FFH: Page ZERO
 * 0100H~7FFFH: Banked Area
 *
 *********************************************************************/

#asm
;	global	bldata, blbss

	psect	data
bldata:	defb	0

	psect	bss
blbss:	defs	1
#endasm
