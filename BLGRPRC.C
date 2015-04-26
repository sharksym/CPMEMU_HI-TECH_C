/*
 * BLGRPRC
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <msxbdos.h>
#include <blstd.h>
#include <blstdvdp.h>
#include <blgrp.h>
#include <blgrpcmd.h>
#include <blgrpfnt.h>
#include <blgrpdat.h>

static uint16_t vram_addr;
static uint16_t vram_offset;
void bl_grp_load_ge5_pic(char *filename, uint16_t dx, uint16_t dy)
{
	FILE *fp;
	uint16_t y, buf_lines, buf_bytes;

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return;

	fseek(fp, 7, SEEK_SET);		/* skip bsave-header */

	vram_offset = (dx / 2) + (dy * 128);
	buf_lines = BL_GRP_SHARED_MEM / 128;
	buf_bytes = buf_lines * 128;

	bl_vdp_cmd_wait();

	for (vram_addr = vram_offset, y = 0; y < 212; y += buf_lines, vram_addr += buf_bytes) {
		if (y + buf_lines >= 212) {
			buf_lines = 212 - y;
			buf_bytes = buf_lines * 128;
		}
		fread(bl_grp.shared_mem, buf_bytes, 1, fp);

		bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
		bl_vdp_vram_h |= bl_grp.active_page_a16_a14;
		bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
		bl_vdp_vram_l = (uint8_t)vram_addr;
		bl_vdp_vram_cnt = buf_bytes;
		bl_copy_to_vram_nn(bl_grp.shared_mem);
	}

	fclose(fp);
}

void bl_grp_load_ge5_pat(char *filename, uint16_t dx, uint16_t dy)
{
	FILE *fp;
	uint16_t width, height;
	uint16_t y, buf_lines, buf_bytes;
	uint16_t pat_row_bytes, n;

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return;

	fread(&width, 2, 1, fp);
	fread(&height, 2, 1, fp);

	vram_offset = (dx / 2) + (dy * 128);
	pat_row_bytes = width / 2;
	buf_lines = BL_GRP_SHARED_MEM / pat_row_bytes;
	buf_bytes = buf_lines * pat_row_bytes;

	bl_vdp_cmd_wait();

	for (y = 0; y < height; y += buf_lines) {
		if (y + buf_lines >= 212) {
			buf_lines = 212 - y;
			buf_bytes = buf_lines * pat_row_bytes;
		}
		fread(bl_grp.shared_mem, buf_bytes, 1, fp);

		vram_addr = y * 128 + vram_offset;
		for (n = 0; n < buf_lines; n++, vram_addr += 128) {
			bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
			bl_vdp_vram_h |= bl_grp.active_page_a16_a14;
			bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
			bl_vdp_vram_l = (uint8_t)vram_addr;
			bl_vdp_vram_cnt = pat_row_bytes;
			bl_copy_to_vram_nn(&bl_grp.shared_mem[pat_row_bytes * n]);
		}
	}

	fclose(fp);
}
#if 0
static uint16_t rc_palette[] = {
	/* pal_no << 12, g << 8, r << 4, b */
	0x0000, 0x1000, 0x2611, 0x3733, 0x4117, 0x5327, 0x6151, 0x7617,
	0x8171, 0x9373, 0xA661, 0xB663, 0xC411, 0xD265, 0xE555, 0xF777,
	0xFFFF	/* end mark */
};
#endif
void bl_grp_load_ge5_pic_pal(char *filename)
{
	FILE *fp;
	uint16_t n;

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return;

	fseek(fp, -32, SEEK_END);	/* palette data */
	fread(bl_grp.palette, 32, 1, fp);

	for (n = 0; n < 16; n++) {
		bl_grp.palette[n] |= n << 12;
	}

	bl_grp_update_palette(bl_grp.palette);

	fclose(fp);
}

void bl_grp_load_ge5_pat_pal(char *filename)
{
	FILE *fp;
	uint16_t width, height, pat_bytes;
	uint16_t n;

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return;

	fread(&width, 2, 1, fp);
	fread(&height, 2, 1, fp);

	pat_bytes = width * height / 2;

	fseek(fp, pat_bytes, SEEK_CUR);	/* palette data */
	fread(bl_grp.palette, 32, 1, fp);

	for (n = 0; n < 16; n++) {
		bl_grp.palette[n] |= n << 12;
	}

	bl_grp_update_palette(bl_grp.palette);

	fclose(fp);
}

void bl_grp_get_ge5_pat_pal(char *filename, uint16_t *palette)
{
	FILE *fp;
	uint16_t width, height, pat_bytes;
	uint16_t n;

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return;

	fread(&width, 2, 1, fp);
	fread(&height, 2, 1, fp);

	pat_bytes = width * height / 2;

	fseek(fp, pat_bytes, SEEK_CUR);	/* palette data */
	fread(palette, 32, 1, fp);

	for (n = 0; n < 16; n++) {
		palette[n] |= n << 12;
	}

	fclose(fp);
}

;
