/* Standard Banking Library (BLSTDMEM.H) */

struct bl_lmem_t {
	uint8_t page_max;
	uint8_t page;
	uint16_t addr14;		/* 14bit addr */
	uint8_t cache_page;
	uint16_t cache_addr14;		/* 14bit addr */
	uint8_t page_tbl[64];		/* 16KB x 64page = 1024KB */
	uint8_t cache_data[256];
};
struct bl_lmem_t *bl_lmem_alloc(uint32_t size);
void bl_lmem_free(struct bl_lmem_t *ptr);
void bl_lmem_write(struct bl_lmem_t *ptr, uint32_t addr32, uint8_t data);
uint8_t bl_lmem_read(struct bl_lmem_t *ptr, uint32_t addr32);
void bl_lmem_copy_to(struct bl_lmem_t *dest, uint32_t addr32, uint8_t *src, uint16_t size);
void bl_lmem_copy_from(uint8_t *dest, struct bl_lmem_t *src, uint32_t addr32, uint16_t size);
