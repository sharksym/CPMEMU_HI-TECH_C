/*
 * BLGRPDAT.H
 */

struct bl_grp_var_t {
	uint8_t vdp_ver;

	uint8_t screen_mode;
	uint8_t display_on;

	uint8_t sprite_mode;
	uint8_t sprite_on;

	uint8_t palette0_on;
	uint8_t line_212;
	uint8_t display_mode;
	uint8_t interlace_on;

	uint8_t color_text_fg;
	uint8_t color_text_bg;
	uint8_t color_border;

	uint8_t width_L40;		/* for T1, T2 */
	uint8_t width_L32;		/* for G1 */
	uint8_t width;			/* current */

	uint8_t view_page;
	uint8_t active_page;
	uint8_t active_page_a16_a14;	/* vram offset a16,a15,a14 */

	uint8_t sprite_view_page;
	uint8_t sprite_active_page;
	uint8_t sprite_gen_view_page;
	uint8_t sprite_gen_active_page;

	uint8_t scroll_mode;
	uint16_t scroll_h;
	uint8_t scroll_v;

	uint8_t *reg_shadow_0_7;
	uint8_t reg_shadow_8_27[27 - 8 + 1];

	uint16_t palette[17];

	uint16_t pattern_name_addr;
	uint16_t color_addr;
	uint16_t pattern_gen_addr;
	uint16_t sprite_color_view_addr;
	uint16_t sprite_color_active_addr;
	uint16_t sprite_attr_view_addr;
	uint16_t sprite_attr_active_addr;
	uint16_t sprite_gen_view_addr;
	uint16_t sprite_gen_active_addr;

	uint16_t row_byte;
	uint8_t bpp_shift;

	uint8_t font_width;
	uint8_t font_height;
	uint8_t font_fgc;
	uint8_t font_bgc;

	uint8_t *shared_mem;
};

#define BL_GRP_SHARED_MEM	(4096)	/* 4Kbytes shared memory */

