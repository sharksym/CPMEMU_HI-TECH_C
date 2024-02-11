/* Mouse I/O */

#define MOUSE_PORT_A	1
#define MOUSE_PORT_B	2

#define MOUSE_MOV_X	0
#define MOUSE_MOV_Y	1
#define MOUSE_BTN_A	2
#define MOUSE_BTN_B	3

extern char mouse_data[];
extern void mouse_poll_data(char port);
#define mouse_offset(mov_x_y)		mouse_data[mov_x_y]
#define mouse_pressed(button)		mouse_data[button]
