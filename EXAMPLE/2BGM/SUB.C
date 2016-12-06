#include <stddef.h>
#include <stdio.h>
#include <keyio.h>
#include <blstd.h>
#include <blstdint.h>
#include <blsnd.h>

void shot(void)
{
	puts("Effect Ch: C (Push SPACE key)");
	puts("Ch: A, Buffer info");

	while (!key_clicked(KEY_ESC) && (bl_snd_get_stat() != BL_SND_STAT_IDLE)) {
		printf("Pos = %04X, Avail = %04X\r",
			bl_snd_bgm_get_pos(0), bl_snd_bgm_get_avail(0));

		if (key_pressed(KEY_SPACE)) {
			bl_snd_bgm_overlay(2, "T120 S0 L128 O7 V15 Z2 C<<C<<C<<C Z0");
			while (key_pressed(KEY_SPACE));
		}
	}

	puts("\n");
}

int sub(void)
{
	bl_snd_init();

	/* 꽃 */
	puts("BGM Ch: A, B");

	/* Violin */
	bl_snd_bgm_enqueue(0, "O5 V15 T60 S3");
	bl_snd_bgm_enqueue(1, "O5 V11 T60 S3");

	bl_snd_bgm_enqueue(0, "L8D.L16DL8G.L16GAGF#EL4D");
	bl_snd_bgm_enqueue(1, "L8D.L16D<L8B.L16BC>ED<CL4B");

	bl_snd_bgm_enqueue(0, "L16<B>DGAL8B>D<L4A.R8");
	bl_snd_bgm_enqueue(1, "L8BL16B>DL8GGL4F#.R8");

	bl_snd_bgm_enqueue(0, "L8D.L16DL8G.L16GAGF#EL4D");
	bl_snd_bgm_enqueue(1, "L8D.L16D<L8B.L16BC>ED<CL4B");

	bl_snd_bgm_enqueue(0, "L8A.L16AL8ABL4G.R8");
	bl_snd_bgm_enqueue(1, "L8A.L16>F#F#EDC<L4B.R8");

	bl_snd_bgm_play();
	shot();
	bl_snd_bgm_stop();

	/* 환희의 송가 */
	puts("BGM Ch: A, B, C");

	/* Piano */
	bl_snd_bgm_enqueue(0, "L4 O4 V15 T120 S1");
	bl_snd_bgm_enqueue(1, "L8 O4 V13 T120 S1");
	bl_snd_bgm_enqueue(2, "L1 O3 V13 T120 S1");

	bl_snd_bgm_enqueue(0, "O4EEFG GFED CCDE EDD2");
	bl_snd_bgm_enqueue(1, "O4CO3GO4CO3G O4DO3GO4EO3G O4EO3GO4DO3G O4CO3GBG O4CO3GEGF4G4 O4C4O3B4B2");
	bl_snd_bgm_enqueue(2, "O3C O2G O3C O2G2.G4");

	bl_snd_bgm_enqueue(0, "O4EEFG GFED CCDE DCC2");
	bl_snd_bgm_enqueue(1, "O4CO3GO4CO3G O4DO3GO4EO3G O4EO3GO4DO3G O4CO3GBG O4CO3GEGF4G4 O3BGFGE2");
	bl_snd_bgm_enqueue(2, "O3C O2G O3C O2G2O3C2");

	bl_snd_bgm_enqueue(0, "O4DDEC DFEC DFED CDO3G2");
	bl_snd_bgm_enqueue(1, "O3BGAB O4C4O3G4 BGBO4D C4O3G4 B4O4D4O3B4O4D4 O3A4O3F+4B2");
	bl_snd_bgm_enqueue(2, "O2G2O3C2 O2G2O3C2 O2G2G+4E4 A4D4G4A8B8");

	bl_snd_bgm_enqueue(0, "O4EEFG GFED CCDE DCC2");
	bl_snd_bgm_enqueue(1, "O4CO3GO4CO3G O4DO3GO4EO3G O4EO3GO4DO3G O4CO3GBG O4CO3GEGF4G4 BGFGE2");
	bl_snd_bgm_enqueue(2, "O3C O2G O3C O2G2O3C2");

	bl_snd_bgm_play();
	shot();
	bl_snd_bgm_stop();

	/* 학교종이 땡땡땡 */
	puts("BGM Ch: A (Enqueue test)");

	bl_snd_bgm_enqueue(0, "L8O4V15 T240 S1");
	bl_snd_bgm_enqueue(0, "GGAAGGE4 GGEED4. R GGAAGGE4 GEDEC4. R");

	bl_snd_bgm_play();

	puts("Ch: A, Buffer info");
	while (!key_pressed(KEY_ESC)) {
		printf("Pos = %04X, Avail = %04X\r",
			bl_snd_bgm_get_pos(0), bl_snd_bgm_get_avail(0));
		if (bl_snd_bgm_get_avail(0) > 128)
			bl_snd_bgm_enqueue(0, "GGAAGGE4 GGEED4. R GGAAGGE4 GEDEC4. R");
	}
	bl_snd_bgm_stop();

	bl_snd_deinit();

	return 0;
}
