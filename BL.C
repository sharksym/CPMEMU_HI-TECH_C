/**********************************************************************
 *
 * BL: MSX-DOS2 Banked Application OVL
 *
 * 0000H~00FFH: Page ZERO
 * 0100H~7FFFH: Banked Area
 *
 *********************************************************************/

#include <blstd.h>

#asm
	global	_bl_data, _bl_bss

;uint8_t bl_data = 0;
	psect	data
_bl_data:
	defb	0

;uint8_t bl_bss;
	psect	bss
_bl_bss:
	defs	1
#endasm

#ifdef BL_DISABLE
#else
#asm
	global	_bl_comm
;uint8_t bl_comm;
	psect	comm
_bl_comm:
	defs	1
#endasm
#endif
