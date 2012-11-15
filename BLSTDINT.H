/* Standard Banking Library (BLSTDINT.H)*/

void bl_enable_bios_timi(void);
void bl_disable_bios_timi(void);

void bl_enable_mouse_poll(uint8_t port);		/* port 1 or 2 */
void bl_disable_mouse_poll(void);
#define bl_mouse_pos_x()	(*((uint8_t *)0x82F4))
#define bl_mouse_pos_y()	(*((uint8_t *)0x82F5))
#define bl_mouse_pressed_a()	(*((uint8_t *)0x82F6) & 0x01)
#define bl_mouse_pressed_b()	(*((uint8_t *)0x82F6) >> 1)

int16_t bl_request_irq_(uint8_t irq, uint16_t handler, uint8_t bank);
#define bl_request_irq(irq, handle) bl_request_irq_(irq, (uint16_t)handle, bl_bank)
int16_t bl_free_irq(uint8_t irq);
void bl_enable_irq(uint8_t irq);
void bl_disable_irq(uint8_t irq);

#define IRQ_EXT0	0
#define IRQ_EXT1	1
#define IRQ_EXT2	2
#define IRQ_EXT3	3
#define IRQ_VDP_HBLK	4
#define IRQ_VDP_VBLK	5
#define IRQ_VDP_SPRT	6

/* Frame rate control */
void bl_set_frame_rate(uint16_t rate);
void bl_wait_frame_rate(void);
