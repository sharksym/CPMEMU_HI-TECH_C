/*
 * BLGRPDAT.H
 * Total 256bytes (bl_grp, etc)
 */

struct bl_grp_var_t {			/* bl_grp_var max. 224bytes */
	uint8_t reg_shadow[48];		/* must be the first variable */

	uint8_t vdp_ver;

	uint8_t screen_mode;
	uint8_t yae_yjk_mode;
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
	uint8_t text_width;		/* T1:40, T2:80, G1:32 */

	uint8_t view_page;
	uint8_t active_page;
	uint8_t active_page_a16_a14;	/* vram offset a16,a15,a14 */

	uint8_t sprite_view_page;
	uint8_t sprite_active_page;
	uint8_t sprite_gen_view_page;
	uint8_t sprite_gen_active_page;

	int8_t adjust_h;
	int8_t adjust_v;

	uint8_t scroll_mode;
	uint16_t scroll_h;
	uint8_t scroll_v;

	uint16_t width;
	uint8_t height;

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

#define BL_GRP_SHARED_MEM	(2048)	/* min. 2Kbytes shared memory */

extern struct bl_grp_var_t bl_grp;
