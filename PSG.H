/* PSG I/O
 * Mixer BIT210: Tone CBA
 * Mixer BIT543: Noise CBA
 * Volume BIT4 : Envelope switch (1:ON, 0:OFF)
 */

#define PSG_CH_A	0
#define PSG_CH_B	1
#define PSG_CH_C	2

extern void write_psg_(unsigned short data_reg);
extern unsigned char read_psg(unsigned short reg);
#define write_psg(reg, data)	write_psg_((unsigned short)(data) << 8 | (reg))

extern void psg_tone_(unsigned short ch_tone);
#define psg_tone(ch, data)	psg_tone_((unsigned short)(ch) << 12 | (data))
#define psg_noise(data)		write_psg(6, data)
#define psg_mixer(data)		write_psg(7, 0x80 | data)
#define psg_volume(ch, data)	write_psg((8 + ch), data)
extern void psg_envelope_period(unsigned short env);
#define psg_envelope_type(data)	write_psg(13, data)

