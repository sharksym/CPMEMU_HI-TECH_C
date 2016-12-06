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

#define BL_SND_BGM_MAX		3

struct bl_snd_var_t {
	uint8_t enable;
	uint8_t state;
	uint8_t command;

	uint8_t vars[BL_SND_BGM_MAX * 2][32];
};

struct bl_snd_var_t bl_snd = {
	0, BL_SND_STAT_IDLE, BL_BGM_CMD_NONE,
};

void bl_bgm_init(void);
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

void bl_snd_init(void)
{
	if (bl_snd.enable)
		return;

	bl_bgm_init();

	bl_request_irq(IRQ_SND_CYCL, bl_bgm_isr);
	bl_enable_irq(IRQ_SND_CYCL);

	bl_snd.enable = 1;
}

void bl_snd_deinit(void)
{
	if (!bl_snd.enable)
		return;

	bl_snd.enable = 0;

	bl_disable_irq(IRQ_SND_CYCL);
	bl_free_irq(IRQ_SND_CYCL);

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
	PUSH	HL
	JR	_call_bl_bgm_cmd

	GLOBAL	_bl_snd_bgm_stop
_bl_snd_bgm_stop:
	LD	HL, BL_BGM_CMD_STOP
	PUSH	HL
	JR	_call_bl_bgm_cmd

	GLOBAL	_bl_snd_bgm_pause
_bl_snd_bgm_pause:
	LD	HL, BL_BGM_CMD_PAUSE
	PUSH	HL
	JR	_call_bl_bgm_cmd

	GLOBAL	_bl_snd_bgm_resume
_bl_snd_bgm_resume:
	LD	HL, BL_BGM_CMD_RESUME
	PUSH	HL
	JR	_call_bl_bgm_cmd

	GLOBAL	_bl_bgm_cmd
_call_bl_bgm_cmd:
	CALL	_bl_bgm_cmd
	POP	BC
	RET
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
