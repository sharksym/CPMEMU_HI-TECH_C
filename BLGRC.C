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

#ifdef BL_ROM

static uint16_t vram_addr;
static uint16_t vram_offset;
void bl_grp_romload_vram_16k(uint16_t rom_addr)
{
	uint16_t n, max;

	max = 16 / (BL_GRP_SHARED_MEM / 1024);

	bl_vdp_cmd_wait();

	bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
	bl_vdp_vram_h |= bl_grp.active_page_a16_a14;

	for (vram_addr = 0, n = 0; n < max; n++, vram_addr += BL_GRP_SHARED_MEM) {
		bl_rom_read_page3(rom_addr, (uint16_t)bl_grp.shared_mem, BL_GRP_SHARED_MEM);
		rom_addr += BL_GRP_SHARED_MEM;

		bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
		bl_vdp_vram_l = (uint8_t)vram_addr;
		bl_vdp_vram_cnt = BL_GRP_SHARED_MEM;
		bl_copy_to_vram_nn(bl_grp.shared_mem);
	}
}

void bl_grp_romload_ge5_pic(uint16_t rom_addr, uint16_t dx, uint16_t dy)
{
	uint16_t y, buf_lines, buf_bytes;

	/* Max. 16KB */
	vram_offset = (dx / 2) + (dy * 128);
	buf_lines = BL_GRP_SHARED_MEM / 128;
	buf_bytes = buf_lines * 128;

	bl_vdp_cmd_wait();

	for (vram_addr = vram_offset, y = 0; y < 64; y += buf_lines, vram_addr += buf_bytes) {
		if (y + buf_lines >= 212) {
			buf_lines = 212 - y;
			buf_bytes = buf_lines * 128;
		}
		bl_rom_read_page3(rom_addr, (uint16_t)bl_grp.shared_mem, buf_bytes);
		rom_addr += buf_bytes;
		/*read(fh, bl_grp.shared_mem, buf_bytes);*/

		bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
		bl_vdp_vram_h |= bl_grp.active_page_a16_a14;
		bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
		bl_vdp_vram_l = (uint8_t)vram_addr;
		bl_vdp_vram_cnt = buf_bytes;
		bl_copy_to_vram_nn(bl_grp.shared_mem);
	}
}

void bl_grp_romload_ge5_pat(uint16_t rom_addr, uint16_t dx, uint16_t dy)
{
	uint16_t y, buf_lines, buf_bytes;
	uint16_t n;

	vram_offset = (dx / 2) + (dy * 128);
	buf_lines = BL_GRP_SHARED_MEM / 64;
	buf_bytes = buf_lines * 64;

	bl_vdp_cmd_wait();

	for (y = 0; y < 128; y += buf_lines) {
		if (y + buf_lines >= 212) {
			buf_lines = 212 - y;
			buf_bytes = buf_lines * 64;
		}
		bl_rom_read_page3(rom_addr, (uint16_t)bl_grp.shared_mem, buf_bytes);
		rom_addr += buf_bytes;
		/*read(fh, bl_grp.shared_mem, buf_bytes);*/

		vram_addr = y * 128 + vram_offset;
		for (n = 0; n < buf_lines; n++, vram_addr += 128, rom_addr += buf_bytes) {
			bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
			bl_vdp_vram_h |= bl_grp.active_page_a16_a14;
			bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
			bl_vdp_vram_l = (uint8_t)vram_addr;
			bl_vdp_vram_cnt = 64;
			bl_copy_to_vram_nn(&bl_grp.shared_mem[64 * n]);
		}
	}
}

#else

static uint16_t vram_addr;
static uint16_t vram_offset;
void bl_grp_load_ge5_pic(char *filename, uint16_t dx, uint16_t dy)
{
	uint8_t fh;
	uint16_t y, buf_lines, buf_bytes;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	lseek(fh, 7, SEEK_SET);		/* skip bsave-header */

	vram_offset = (dx / 2) + (dy * 128);
	buf_lines = BL_GRP_SHARED_MEM / 128;
	buf_bytes = buf_lines * 128;

	bl_vdp_cmd_wait();

	for (vram_addr = vram_offset, y = 0; y < 212; y += buf_lines, vram_addr += buf_bytes) {
		if (y + buf_lines >= 212) {
			buf_lines = 212 - y;
			buf_bytes = buf_lines * 128;
		}
		read(fh, bl_grp.shared_mem, buf_bytes);

		bl_vdp_vram_h = (uint8_t)(vram_addr >> 14);
		bl_vdp_vram_h |= bl_grp.active_page_a16_a14;
		bl_vdp_vram_m = (uint8_t)((vram_addr >> 8)& 0x3F);
		bl_vdp_vram_l = (uint8_t)vram_addr;
		bl_vdp_vram_cnt = buf_bytes;
		bl_copy_to_vram_nn(bl_grp.shared_mem);
	}

	close(fh);
}

void bl_grp_load_ge5_pat(char *filename, uint16_t dx, uint16_t dy)
{
	uint8_t fh;
	uint16_t width, height;
	uint16_t y, buf_lines, buf_bytes;
	uint16_t pat_row_bytes, n;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	read(fh, &width, 2);
	read(fh, &height, 2);

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
		read(fh, bl_grp.shared_mem, buf_bytes);

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

	close(fh);
}

void bl_grp_load_ge5_pic_pal(char *filename)
{
	uint8_t fh;
	uint16_t n;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	lseek(fh, -32, SEEK_END);	/* palette data */
	read(fh, bl_grp.palette, 32);

	for (n = 0; n < 16; n++) {
		bl_grp.palette[n] |= n << 12;
	}

	bl_grp_update_palette(bl_grp.palette);

	close(fh);
}

void bl_grp_load_ge5_pat_pal(char *filename)
{
	uint8_t fh;
	uint16_t width, height, pat_bytes;
	uint16_t n;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	read(fh, &width, 2);
	read(fh, &height, 2);

	pat_bytes = width * height / 2;

	lseek(fh, pat_bytes, SEEK_CUR);	/* palette data */
	read(fh, bl_grp.palette, 32);

	for (n = 0; n < 16; n++) {
		bl_grp.palette[n] |= n << 12;
	}

	bl_grp_update_palette(bl_grp.palette);

	close(fh);
}

void bl_grp_get_ge5_pat_pal(char *filename, uint16_t *palette)
{
	uint8_t fh;
	uint16_t width, height, pat_bytes;
	uint16_t n;

	fh = open(filename, O_RDONLY);
	if (fh == 0xFF)
		return;

	read(fh, &width, 2);
	read(fh, &height, 2);

	pat_bytes = width * height / 2;

	lseek(fh, pat_bytes, SEEK_CUR);	/* palette data */
	read(fh, palette, 32);

	for (n = 0; n < 16; n++) {
		palette[n] |= n << 12;
	}

	close(fh);
}

#endif
