/* Standard Banking Library (BLSTDSLT.H) */

uint8_t bl_read_slot_(uint8_t *addr, uint8_t slot);
#define bl_read_slot(slot, addr) bl_read_slot_(addr, slot)
void bl_write_slot_(uint8_t *addr, uint16_t slot_data);
#define bl_write_slot(slot, addr, data) bl_write_slot_(addr, ((uint16_t)(slot) << 8) | (data))
void bl_copy_to_slot(uint8_t slot, uint8_t *dest, uint8_t *src, uint16_t count);
void bl_copy_from_slot(uint8_t slot, uint8_t *src, uint8_t *dest, uint16_t count);

void bl_select_page1_slot(uint8_t slot);
void bl_restore_page1_slot(void);
