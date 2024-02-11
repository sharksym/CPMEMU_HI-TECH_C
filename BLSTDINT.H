/* Standard Banking Library (BLSTDINT.H)*/

/* BIOS TIMI routine */
void bl_enable_bios_timi(void);
void bl_disable_bios_timi(void);

/* IRQ requests */
#define IRQ_EXT0	0
#define IRQ_VDP_HBLK	1
#define IRQ_EXT1	2
#define IRQ_SND_CYCL	3
#define IRQ_VDP_SPRT	4
#define IRQ_VDP_VBLK	5
#define IRQ_EXT2	6

int16_t bl_request_irq(uint8_t irq, void *handler);
int16_t bl_free_irq(uint8_t irq);
void bl_enable_irq(uint8_t irq);
void bl_disable_irq(uint8_t irq);

/* Frame rate control */
void bl_set_frame_rate(uint16_t rate);
void bl_wait_frame_rate(void);
