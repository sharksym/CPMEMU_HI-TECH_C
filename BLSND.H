/*
 * BLSND.H
 */

/*
 * MML instructions
 *
 * A to G: Note
 * L:      Length of Note (1 to 128)
 * O:      Octave (1 to 8)
 * R:      Rest
 * S:      Envelope (0=Off, 1=Piano, 2=Trumpet, 3=Violin)
 * T:      Tempo (32 to 255)
 * V:      Volume (0 to 15)
 * Z:      Noise (0 to 31)
 * <:      Octave Down
 * >:      Octave Up
 *
 */

#define BL_SND_STAT_IDLE	0
#define BL_SND_STAT_STOP	1
#define BL_SND_STAT_PLAY	2
#define BL_SND_STAT_PAUSE	3

uint8_t bl_snd_get_stat(void);
void bl_snd_init(uint8_t mode);
void bl_snd_deinit(void);
void bl_snd_bgm_enqueue(uint8_t ch, char *mml);
void bl_snd_bgm_overlay(uint8_t ch, char *mml);
uint16_t bl_snd_bgm_get_pos(uint8_t ch);
uint16_t bl_snd_bgm_get_avail(uint8_t ch);
void bl_snd_bgm_play(void);
void bl_snd_bgm_stop(void);
void bl_snd_bgm_pause(void);
void bl_snd_bgm_resume(void);
