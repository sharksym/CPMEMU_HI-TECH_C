/* JoyMega adapter I/O (MegaDrive Pad) */

#define JOYMEGA_START	0x0080
#define JOYMEGA_A	0x0040
#define JOYMEGA_C	0x0020
#define JOYMEGA_B	0x0010

#define JOYMEGA_RIGHT	0x0008
#define JOYMEGA_LEFT	0x0004
#define JOYMEGA_DOWN	0x0002
#define JOYMEGA_UP	0x0001

extern unsigned short joymega_data;
extern void joymega_poll_data(void);
#define joymega_pressed(button)		(joymega_data & button ? 1 : 0)
