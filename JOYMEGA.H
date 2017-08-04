/* JoyMega adapter I/O (MegaDrive Pad) */

#define JOYMEGA_PORT_A	1
#define JOYMEGA_PORT_B	2

#define JOYMEGA_START	0x80
#define JOYMEGA_A	0x40
#define JOYMEGA_C	0x20
#define JOYMEGA_B	0x10

#define JOYMEGA_RIGHT	0x08
#define JOYMEGA_LEFT	0x04
#define JOYMEGA_DOWN	0x02
#define JOYMEGA_UP	0x01

extern unsigned char joymega_data[];
extern void joymega_poll_data(void);
#define joymega_pressed(port, button)	(joymega_data[port] & button)
