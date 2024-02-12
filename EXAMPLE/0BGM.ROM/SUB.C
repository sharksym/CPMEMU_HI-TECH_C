#include <stddef.h>
#include <stdio.h>
#include <keyio.h>
#include <blstd.h>
#include <blstdint.h>
#include <blsnd.h>

void shot(uint8_t ch)
{
	puts("Effect Ch.2 (Push SPACE key)");
	printf("Buffer Ch.%d Ch.%d Ch.%d\n", ch, ch + 1, ch + 2);
	printf("Avail= %04X %04X %04X\n",
		bl_snd_bgm_get_avail(ch), bl_snd_bgm_get_avail(ch + 1), bl_snd_bgm_get_avail(ch + 2));

	while (!key_clicked(KEY_ESC) && (bl_snd_get_stat() != BL_SND_STAT_IDLE)) {
		printf("Pos  = %04X %04X %04X\r",
			bl_snd_bgm_get_pos(ch), bl_snd_bgm_get_pos(ch + 1), bl_snd_bgm_get_pos(ch + 2));

		if (key_pressed(KEY_SPACE)) {
			bl_snd_bgm_overlay(2, "T120 S0 L128 O7 V15 Z2 C<<C<<C<<C Z0");
			while (key_pressed(KEY_SPACE));
		}
	}

	puts("\n");
}

int sub(void)
{
	uint8_t ch = BL_SND_BGM_FM_1;

	bl_snd_init(BL_SND_MODE_FM | BL_SND_MODE_FM_RTH);

#if 0
	puts("FM Test Ch.1");
	bl_snd_bgm_enqueue(3, "L2 O4 V15 T120 S3");
	bl_snd_bgm_enqueue(3, "N12 N14 N16 N17 N19 N21 N23 R");
	bl_snd_bgm_enqueue(3, "N24 N26 N28 N29 N31 N33 N35 R");
	bl_snd_bgm_enqueue(3, "N36 N38 N40 N41 N43 N45 N47 R");
	bl_snd_bgm_enqueue(3, "N48 N50 N52 N53 N55 N57 N59 R");
	bl_snd_bgm_enqueue(3, "N60 N62 N64 N65 N67 N69 N71 R");
	bl_snd_bgm_enqueue(3, "N72 N74 N76 N77 N79 N81 N83 R");

	bl_snd_bgm_play();
	shot(3);
	bl_snd_bgm_stop();
#endif

	/* 도레미파솔라시도 */
	puts("[ PSG 1Ch. ]");

	/* Piano */
	bl_snd_bgm_enqueue(0, "L2 O4 V15 T120 S1");
	bl_snd_bgm_enqueue(0, "N36 N38 N40N41 N43 N45 N47N48 R");

	bl_snd_bgm_play();
	shot(0);
	bl_snd_bgm_stop();

	/* 도레미파솔라시도 */
	puts("[ FM 1Ch. ]");

	/* Piano */
	bl_snd_bgm_enqueue(ch, "L2 O4 V15 T120 S3");
	bl_snd_bgm_enqueue(ch, "N36 N38 N40N41 N43 N45 N47N48 R");

	bl_snd_bgm_play();
	shot(ch);
	bl_snd_bgm_stop();

	/* 꽃 */
	puts("[ PSG 2Ch. ]");

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
	shot(0);
	bl_snd_bgm_stop();

	/* 환희의 송가 */
	puts("[ FM 3Ch. Piano ]");

	/* Piano */
	bl_snd_bgm_enqueue(ch + 0, "L4 O4 V15 T120 S3");
	bl_snd_bgm_enqueue(ch + 1, "L8 O4 V13 T120 S3");
	bl_snd_bgm_enqueue(ch + 2, "L1 O3 V13 T120 S3");

	bl_snd_bgm_enqueue(ch + 0, "O4EEFG GFED CCDE EDD2");
	bl_snd_bgm_enqueue(ch + 1, "O4CO3GO4CO3G O4DO3GO4EO3G O4EO3GO4DO3G O4CO3GBG O4CO3GEGF4G4 O4C4O3B4B2");
	bl_snd_bgm_enqueue(ch + 2, "O3C O2G O3C O2G2.G4");

	bl_snd_bgm_enqueue(ch + 0, "O4EEFG GFED CCDE DCC2");
	bl_snd_bgm_enqueue(ch + 1, "O4CO3GO4CO3G O4DO3GO4EO3G O4EO3GO4DO3G O4CO3GBG O4CO3GEGF4G4 O3BGFGE2");
	bl_snd_bgm_enqueue(ch + 2, "O3C O2G O3C O2G2O3C2");

	bl_snd_bgm_enqueue(ch + 0, "O4DDEC DFEC DFED CDO3G2");
	bl_snd_bgm_enqueue(ch + 1, "O3BGAB O4C4O3G4 BGBO4D C4O3G4 B4O4D4O3B4O4D4 O3A4O3F+4B2");
	bl_snd_bgm_enqueue(ch + 2, "O2G2O3C2 O2G2O3C2 O2G2G+4E4 A4D4G4A8B8");

	bl_snd_bgm_enqueue(ch + 0, "O4EEFG GFED CCDE DCC2");
	bl_snd_bgm_enqueue(ch + 1, "O4CO3GO4CO3G O4DO3GO4EO3G O4EO3GO4DO3G O4CO3GBG O4CO3GEGF4G4 BGFGE2");
	bl_snd_bgm_enqueue(ch + 2, "O3C O2G O3C O2G2O3C2");

	bl_snd_bgm_play();
	shot(ch);
	bl_snd_bgm_stop();

	/* 테트리스 */
	puts("[ PSG 3Ch. ]");

	/* Piano */
	bl_snd_bgm_enqueue(0, "V14 S1");
	bl_snd_bgm_enqueue(1, "V14 S1");
	bl_snd_bgm_enqueue(2, "V13 S1");

	/* MML from http://www.mabinogiworld.com/showthread.php?12888-Request-Tetris-theme */
	bl_snd_bgm_enqueue(0, "t80l16o4>erc-cde32d32c<bara>cerdcc-r8cdrercr<arar4>drfargfer8cef32e32dcc-r8cdn56en56cr");
	bl_snd_bgm_enqueue(0, "<ara4>erc-cde32d32c<bara>cerdcc-r8cdrercr<arar4>drfargfer8cef32e32dcc-r8cdn56en56cr<ar");
	bl_snd_bgm_enqueue(0, "l4aecdc-c<ag+b>ecdc-c8e8ag+2l16>erc-cde32d32c<bara>cerdcc-r8cdrercr<arar4>drfargfer8ce");
	bl_snd_bgm_enqueue(0, "f32e32dcc-r8cdn56en56cr<ara4");

	bl_snd_bgm_enqueue(1, "t80l16o4brg+abb+32b32ag+e8eab+rbag+eg+ab8b+8a8e8eo2ab>cd>frab+rbagr8egrfeg+eg+abrb+rarere4br");
	bl_snd_bgm_enqueue(1, "g+abb+32b32ag+e8eab+rbag+eg+ab8b+8a8e8e<<ab>cd>frab+rbagr8egrfeg+eg+abrb+rarer");
	bl_snd_bgm_enqueue(1, "l4ec<abg+aeeg+b+abg+a8>c8ee2l16brg+abb+32b32ag+e8eab+rbag+eg+ab8b+8a8e8er4frab+rbagr8");
	bl_snd_bgm_enqueue(1, "egrfeg+eg+abrb+rarere4");

	bl_snd_bgm_enqueue(2, "t80l16o2e>e<e>e<e>e<e>e<a>a<a>a<a>a<a>an32g+n32g+<e>e<e>e<a>a<a>a<a>ac-cd<drdrd32d32agc");
	bl_snd_bgm_enqueue(2, "b+cb+cgcgb>brberg+r<a>e<a>e<a>eee<e>e<e>e<e>e<e>e<a>a<a>a<a>a<a>an32g+n32g+<e>e<e>e<a>a");
	bl_snd_bgm_enqueue(2, "<a>a<a>ac-cd<drdrd32d32agcb+cbcgcgb>brbergr<a>e<a>e<a>eee<a>e<a>e<a>e<a>en32en32en32en32");
	bl_snd_bgm_enqueue(2, "e<a>e<a>e<a>e<a>en32en32en32en32e<a>e<a>e<a>e<a>en32en32en32en32e<a>e<a>e<a>e<a>en32");
	bl_snd_bgm_enqueue(2, "en32en32en32e<e>e<e>e<e>e<e>e<a>a<a>a<a>a<a>an32g+n32g+<e>e<e>e<a>a<a>a<a>ac-cd<drdr");
	bl_snd_bgm_enqueue(2, "d32d32agcb+cb+cgcgb>brberg+r<a>e<a>e<a4");

	bl_snd_bgm_play();
	shot(0);
	bl_snd_bgm_stop();

	/* Drum Loop */
	puts("[ FM Drum Loop ]");
	bl_snd_bgm_enqueue(BL_SND_BGM_FM_RTH, "T120");

	bl_snd_bgm_enqueue(BL_SND_BGM_FM_RTH, "BSH8H8S!H!8H8 BSH8H8S!H!8H8 ");

	bl_snd_bgm_play();

	puts("Buffer Ch.Drum");
	while (!key_pressed(KEY_ESC)) {
		printf("Pos = %04X, Avail = %04X\r",
			bl_snd_bgm_get_pos(BL_SND_BGM_FM_RTH), bl_snd_bgm_get_avail(BL_SND_BGM_FM_RTH));
		if (bl_snd_bgm_get_avail(BL_SND_BGM_FM_RTH) > 128)
			bl_snd_bgm_enqueue(BL_SND_BGM_FM_RTH, "BSH8H8S!H!8H8 BSH8H8S!H!8H8 ");
	}
	bl_snd_bgm_stop();

	bl_snd_deinit();

	return 0;
}
