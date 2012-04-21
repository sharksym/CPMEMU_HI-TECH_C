/*
 * BLGRP.H
 */

#define GRP_VER_9938	0		/* VDP V9938 */
#define GRP_VER_9958	1		/* VDP V9958 */

#define GRP_SCR_T1	0		/* SCREEN 0 */
#define GRP_SCR_T2	1		/* SCREEN 0 80COL */
#define GRP_SCR_MC	2		/* SCREEN 3 */
#define GRP_SCR_G1	3		/* SCREEN 1 */
#define GRP_SCR_G2	4		/* SCREEN 2 */
#define GRP_SCR_G3	5		/* SCREEN 4 */
#define GRP_SCR_G4	6		/* SCREEN 5 */
#define GRP_SCR_G5	7		/* SCREEN 6 */
#define GRP_SCR_G6	8		/* SCREEN 7 */
#define GRP_SCR_G7	9		/* SCREEN 8 */

#define GRP_SPR_8	0x00		/* 8x8   normal */
#define GRP_SPR_8EX	0x01		/* 8x8   expanded */
#define GRP_SPR_16	0x02		/* 16x16 normal */
#define GRP_SPR_16EX	0x03		/* 16x16 expanded */

#define GRP_DISP_240P	0x00		/* 240p normal */
#define GRP_DISP_240P2	0x04		/* 240p two field toggle */
#define GRP_DISP_480I	0x0C		/* 480i two field */
#define GRP_DISP_288P	0x02		/* 288p normal */
#define GRP_DISP_288P2	0x06		/* 288p two field toggle */
#define GRP_DISP_576I	0x0E		/* 576i two field */
#define GRP_DISP_IL_E0	0x0C		/* bit mask for interlaced two field */

#define GRP_SCROLL_P1	0x00		/* hor 1page */
#define GRP_SCROLL_P2	0x01		/* hor 2page */
#define GRP_SCROLL_P1M	0x02		/* hor 1page left mask */
#define GRP_SCROLL_P2M	0x03		/* hor 2page left mask */

char bl_grp_init(void);
void bl_grp_deinit(void);
uint8_t bl_grp_get_vdp_ver(void);

void bl_grp_set_screen_mode(uint8_t mode);
void bl_grp_set_display_on(uint8_t on);
uint8_t bl_grp_get_screen_mode(void);
uint8_t bl_grp_get_display_on(void);

void bl_grp_set_sprite_mode(uint8_t mode);
void bl_grp_set_sprite_on(uint8_t on);
uint8_t bl_grp_get_sprite_mode(void);
uint8_t bl_grp_get_sprite_on(void);

void bl_grp_set_palette0_on(uint8_t on);
void bl_grp_set_line_212(uint8_t on);
void bl_grp_set_display_mode(uint8_t mode);
uint8_t bl_grp_get_palette0_on(void);
uint8_t bl_grp_get_line_212(void);
uint8_t bl_grp_get_display_mode(void);

void bl_grp_set_color_text_fg(uint8_t color);
void bl_grp_set_color_text_bg(uint8_t color);
void bl_grp_set_color_border(uint8_t color);
uint8_t bl_grp_get_color_text_fg(void);
uint8_t bl_grp_get_color_text_bg(void);
uint8_t bl_grp_get_color_border(void);

void bl_grp_set_width(uint8_t width);

void bl_grp_set_view(uint8_t page);
void bl_grp_set_active(uint8_t page);
void bl_grp_erase(uint8_t page, uint8_t c);

void bl_grp_set_sprite_view(uint8_t page);
void bl_grp_set_sprite_active(uint8_t page);
void bl_grp_set_sprite_gen_view(uint8_t page);
void bl_grp_set_sprite_gen_active(uint8_t page);

void bl_grp_set_adjust_hor(int8_t h);
void bl_grp_set_adjust_ver(int8_t v);

void bl_grp_set_scroll_mode(uint8_t mode);
void bl_grp_set_scroll_hor(uint16_t h);
void bl_grp_set_scroll_ver(uint8_t v);

void bl_grp_set_palette(uint8_t no, uint8_t r, uint8_t g, uint8_t b);
uint16_t bl_grp_get_palette(uint8_t no);
#define bl_grp_update_palette(addr) bl_set_palette_array(addr)
void bl_grp_reset_palette(void);

uint16_t bl_grp_get_vramaddr_spr_gen(uint16_t no);
#define bl_grp_set_sprite_pattern_8x8(no, addr)	\
{bl_set_vram_addr16(bl_grp_get_vramaddr_spr_gen(no)); bl_copy_to_vram_8(addr);}
#define bl_grp_set_sprite_pattern_16x16(no, addr)	\
{bl_set_vram_addr16(bl_grp_get_vramaddr_spr_gen(no<<2)); bl_copy_to_vram_32(addr);}
uint16_t bl_grp_get_vramaddr_spr_col(uint16_t no);
#define bl_grp_set_sprite_color(no, addr)	\
{bl_set_vram_addr16(bl_grp_get_vramaddr_spr_col(no)); bl_copy_to_vram_16(addr);}
uint16_t bl_grp_get_vramaddr_spr_attr(uint16_t layer);

void bl_grp_clear_sprite(void);
void bl_grp_put_sprite(uint16_t layer, uint8_t x, uint8_t y, uint8_t c, uint8_t no);

void bl_grp_write_vram(uint8_t *src, uint16_t y, uint16_t size);
void bl_grp_read_vram(uint8_t *dest, uint16_t y, uint16_t size);

void bl_grp_load_ge5_pic(char *filename, uint16_t dx, uint16_t dy);
void bl_grp_load_ge5_pat(char *filename, uint16_t dx, uint16_t dy);
void bl_grp_load_ge5_pic_pal(char *filename);
void bl_grp_load_ge5_pat_pal(char *filename);
void bl_grp_get_ge5_pat_pal(char *filename, uint16_t *palette);

