/*
 * BLGRPFNT.H
 */

#define GRP_FONT_ORGIN	0x00
#define GRP_FONT_CENTER	0x01
#define GRP_FONT_BOLD	0x02
#define GRP_FONT_WIDE	0x03

void bl_grp_load_font(char *filename);
void bl_grp_set_font_style(uint8_t style);
void bl_grp_set_font_size(uint8_t w, uint8_t h);
void bl_grp_set_font_color(uint8_t fg, uint8_t bg);

void bl_grp_print(uint16_t x, uint16_t y, char *str);
void bl_grp_print_pos(uint16_t x, uint16_t y);
void bl_grp_print_str(char *str);
void bl_grp_print_chr(char chr);
void bl_grp_print_cursor(void);
void bl_grp_print_backspace(void);
void bl_grp_print_back(char count);

