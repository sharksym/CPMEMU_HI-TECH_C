/* Standard Banking Library (BLSTDVDP.H) */

uint8_t bl_read_vdp(uint16_t reg);
void bl_write_vdp_(uint16_t reg_val);
#define bl_write_vdp(reg, val) bl_write_vdp_(((uint16_t)(reg) << 8) | (val))
void bl_set_palette_(uint16_t palette);
#define bl_set_palette(pal, r, g, b) \
bl_set_palette_(((uint16_t)(pal) << 12) | ((uint16_t)(g) << 8) | ((r) << 4) | (b))
void bl_grp_set_palette_mute(uint8_t on);

extern uint8_t bl_vdp_vram_h, bl_vdp_vram_m, bl_vdp_vram_l;
extern uint16_t bl_vdp_vram_cnt;
void bl_set_vram_addr16(uint16_t addr);
uint8_t bl_read_vram(void);
uint8_t bl_read_vram_i(void);
void bl_write_vram(uint16_t val);
void bl_write_vram_i(uint16_t val);
void bl_copy_from_vram_32(uint8_t *dest);
void bl_copy_from_vram_16(uint8_t *dest);
void bl_copy_from_vram_8(uint8_t *dest);
void bl_copy_from_vram_4(uint8_t *dest);
void bl_copy_from_vram_nn(uint8_t *dest);
void bl_copy_to_vram_32(uint8_t *src);
void bl_copy_to_vram_16(uint8_t *src);
void bl_copy_to_vram_8(uint8_t *src);
void bl_copy_to_vram_4(uint8_t *src);
void bl_copy_to_vram_3(uint8_t *src);
void bl_copy_to_vram_nn(uint8_t *src);
void bl_erase_vram(uint16_t val);

extern uint8_t bl_draw_font_w, bl_draw_font_h, bl_draw_font_fgc, bl_draw_font_bgc;
void bl_draw_font_mc(uint8_t *font);
void bl_draw_font_g4(uint8_t *font);
void bl_draw_font_g4i(uint8_t *font);
void bl_draw_font_g5(uint8_t *font);
void bl_draw_font_g5i(uint8_t *font);
void bl_draw_font_g6(uint8_t *font);
void bl_draw_font_g6i(uint8_t *font);
void bl_draw_font_g7(uint8_t *font);
void bl_draw_font_g7i(uint8_t *font);

void bl_vdp_cmd_wait(void);
void bl_vdp_cmd_mmm(uint8_t *reg_addr);
void bl_vdp_cmd_hmmv(uint8_t *reg_addr);
void bl_vdp_cmd_line(uint8_t *reg_addr);
void bl_vdp_cmd_pset(uint8_t *reg_addr);
