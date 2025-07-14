#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <psg.h>
#include <blstd.h>
#include <blstdint.h>
#include <blsnd.h>

#define BL_BGM_CMD_NONE		0
#define BL_BGM_CMD_STOP		1
#define BL_BGM_CMD_PLAY		2
#define BL_BGM_CMD_PAUSE	3
#define BL_BGM_CMD_RESUME	4

struct bl_snd_var_t {
	uint8_t enable;
	uint8_t state;
	uint8_t command;
};

#ifdef BLBGM_T		/* Tiny version (PSG only) */
#define bl_snd			bl_sndt
#define bl_bgm_init		bl_bgmt_init
#define bl_bgm_deinit		bl_bgmt_deinit
#define bl_bgm_cmd		bl_bgmt_cmd
#define bl_bgm_enqueue		bl_bgmt_enqueue
#define bl_bgm_get_avail	bl_bgmt_get_avail
#define bl_bgm_get_pos		bl_bgmt_get_pos
#define bl_bgm_init_bgm		bl_bgmt_init_bgm
#define bl_bgm_play_bgm		bl_bgmt_play_bgm
#define bl_bgm_stop_bgm		bl_bgmt_stop_bgm
#define bl_bgm_init_ovl		bl_bgmt_init_ovl
#define bl_bgm_play_ovl		bl_bgmt_play_ovl
#define bl_bgm_stop_ovl		bl_bgmt_stop_ovl
#define bl_bgm_isr		bl_bgmt_isr

#define _bl_bgm_get_pos		_bl_bgmt_get_pos
#define _bl_bgm_get_avail	_bl_bgmt_get_avail
#define _bl_bgm_cmd		_bl_bgmt_cmd
uint8_t BLBGM_Tiny;
#else
uint8_t BLBGM_Normal;
#endif

extern struct bl_snd_var_t bl_snd;

void bl_bgm_init(uint8_t mode);
void bl_bgm_deinit(void);
void bl_bgm_cmd(uint8_t cmd);
void bl_bgm_enqueue(uint8_t ch, uint8_t overlay, uint8_t *mml_ch);
uint16_t bl_bgm_get_avail(uint8_t ch);
uint16_t bl_bgm_get_pos(uint8_t ch);
void bl_bgm_init_bgm(uint8_t ch);
void bl_bgm_play_bgm(uint8_t ch);
void bl_bgm_stop_bgm(uint8_t ch);
void bl_bgm_init_ovl(uint8_t ch);
void bl_bgm_play_ovl(uint8_t ch);
void bl_bgm_stop_ovl(uint8_t ch);
void bl_bgm_isr(void);

uint8_t bl_snd_get_stat(void)
{
	return bl_snd.state;
}

void bl_snd_init(uint8_t mode)
{
	if (bl_snd.enable)
		return;

	bl_bgm_init(mode);
#ifdef BLBGM_HBLK
	bl_request_irq(IRQ_VDP_HBLK, bl_bgm_isr);
	bl_enable_irq(IRQ_VDP_HBLK);
	bl_grp_set_irq_hblank_line(BLBGM_HBLK);
	bl_grp_set_irq_hblank(1);
#else
	bl_request_irq(IRQ_SND_CYCL, bl_bgm_isr);
	bl_enable_irq(IRQ_SND_CYCL);
#endif
	bl_snd.enable = 1;
}

void bl_snd_deinit(void)
{
	if (!bl_snd.enable)
		return;

	bl_snd.enable = 0;
#ifdef BLBGM_HBLK
	bl_grp_set_irq_hblank(0);
	bl_disable_irq(IRQ_VDP_HBLK);
	bl_free_irq(IRQ_VDP_HBLK);
#else
	bl_disable_irq(IRQ_SND_CYCL);
	bl_free_irq(IRQ_SND_CYCL);
#endif
	bl_bgm_deinit();
}

void bl_snd_bgm_enqueue(uint8_t ch, char *mml)
{
	bl_bgm_enqueue(ch, 0, (uint8_t *)mml);
}

void bl_snd_bgm_overlay(uint8_t ch, char *mml)
{
	bl_bgm_stop_ovl(ch);
	bl_bgm_enqueue(ch, 1, (uint8_t *)mml);
	bl_bgm_play_ovl(ch);
}

#if 1	/* Assembly version */
#asm
	PSECT	text

	GLOBAL	_bl_snd_bgm_get_pos, _bl_bgm_get_pos
_bl_snd_bgm_get_pos:
	JP	_bl_bgm_get_pos

	GLOBAL	_bl_snd_bgm_get_avail, _bl_bgm_get_avail
_bl_snd_bgm_get_avail:
	JP	_bl_bgm_get_avail

	GLOBAL	_bl_snd_bgm_play
_bl_snd_bgm_play:
	LD	HL, BL_BGM_CMD_PLAY
call_bl_bgm_cmd:
	PUSH	HL
	GLOBAL	_bl_bgm_cmd
	CALL	_bl_bgm_cmd
	POP	BC
	RET

	GLOBAL	_bl_snd_bgm_stop
_bl_snd_bgm_stop:
	LD	HL, BL_BGM_CMD_STOP
	JR	call_bl_bgm_cmd

	GLOBAL	_bl_snd_bgm_pause
_bl_snd_bgm_pause:
	LD	HL, BL_BGM_CMD_PAUSE
	JR	call_bl_bgm_cmd

	GLOBAL	_bl_snd_bgm_resume
_bl_snd_bgm_resume:
	LD	HL, BL_BGM_CMD_RESUME
	JR	call_bl_bgm_cmd
#endasm
#else
uint16_t bl_snd_bgm_get_pos(uint8_t ch)
{
	return bl_bgm_get_pos(ch);
}

uint16_t bl_snd_bgm_get_avail(uint8_t ch)
{
	return bl_bgm_get_avail(ch);
}

void bl_snd_bgm_play(void)
{
	bl_bgm_cmd(BL_BGM_CMD_PLAY);
}

void bl_snd_bgm_stop(void)
{
	bl_bgm_cmd(BL_BGM_CMD_STOP);
}

void bl_snd_bgm_pause(void)
{
	bl_bgm_cmd(BL_BGM_CMD_PAUSE);
}

void bl_snd_bgm_resume(void)
{
	bl_bgm_cmd(BL_BGM_CMD_RESUME);
}
#endif
