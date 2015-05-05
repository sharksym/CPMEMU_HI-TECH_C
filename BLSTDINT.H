/* Standard Banking Library (BLSTDINT.H)*/

/* BIOS TIMI routine */
void bl_enable_bios_timi(void);
void bl_disable_bios_timi(void);

/* Mouse handler */
struct bl_mouse_t {
	uint8_t move_x;
	uint8_t move_y;
	uint8_t butn_a;			/* 1: pressed, 0: released */
	uint8_t butn_b;
};

void bl_enable_mouse_poll(uint8_t port);		/* port 1 or 2 */
void bl_disable_mouse_poll(void);
void bl_mouse_get_data(struct bl_mouse_t *mouse);

/* IRQ requests */
#define IRQ_EXT0	0
#define IRQ_EXT1	1
#define IRQ_EXT2	2
#define IRQ_EXT3	3
#define IRQ_VDP_HBLK	4
#define IRQ_VDP_VBLK	5
#define IRQ_VDP_SPRT	6

int16_t bl_request_irq(uint8_t irq, void *handler);
int16_t bl_free_irq(uint8_t irq);
void bl_enable_irq(uint8_t irq);
void bl_disable_irq(uint8_t irq);

/* Frame rate control */
void bl_set_frame_rate(uint16_t rate);
void bl_wait_frame_rate(void);
