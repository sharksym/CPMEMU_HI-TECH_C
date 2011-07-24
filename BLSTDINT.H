/* Standard Banking Library (BLSTDINT.H)*/

void bl_enable_bios_timi(void);
void bl_disable_bios_timi(void);
int16_t bl_request_irq_(uint8_t irq, uint16_t handler, uint8_t bank);
#define bl_request_irq(irq, handle) bl_request_irq_(irq, (uint16_t)handle, bl_bank)
int16_t bl_free_irq(uint8_t irq);
void bl_enable_irq(uint8_t irq);
void bl_disable_irq(uint8_t irq);

#define IRQ_EXT0	0
#define IRQ_EXT1	1
#define IRQ_EXT2	2
#define IRQ_EXT3	3
#define IRQ_EXT4	4
#define IRQ_EXT5	5
#define IRQ_EXT6	6
#define IRQ_EXT7	7

#define IRQ_VDP_HBLK	8
#define IRQ_VDP_VBLK	9
#define IRQ_VDP_SPRT	10

/* Frame rate control */
void bl_set_frame_rate(uint16_t rate);
void bl_wait_frame_rate(void);
