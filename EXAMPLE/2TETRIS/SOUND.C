/*
 *  TETRIS (SOUND)
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <msxbdos.h>
#include <blstd.h>
#include <blsnd.h>


void bgm_init(void)
{
	bl_snd_init(BL_SND_MODE_REPEAT | BL_SND_MODE_FM | BL_SND_MODE_FM_RTH);
}

void bgm_deinit(void)
{
	bl_snd_deinit();
}

void bgm_enqueue_main(void)
{
	uint8_t ch;
	/* MML from http://www.mabinogiworld.com/showthread.php?12888-Request-Tetris-theme */

#if 0	/* PSG */
	ch = BL_SND_BGM_PSG_A;
	bl_snd_bgm_enqueue(ch + 0, "s2 v14");
	bl_snd_bgm_enqueue(ch + 1, "s2 v14");
	bl_snd_bgm_enqueue(ch + 2, "s2 v13");
#else	/* FM */
	ch = BL_SND_BGM_FM_1;
	bl_snd_bgm_enqueue(ch + 0, "s1 v14");	/* 1 violin */
	bl_snd_bgm_enqueue(ch + 1, "s1 v14");	/* 1 violin */
	bl_snd_bgm_enqueue(ch + 2, "s10 v13");	/* 10 synth */
#endif
	bl_snd_bgm_enqueue(ch + 0, "t80l16o4>erc-cde32d32c<bara>cerdcc-r8cdrercr<arar4>drfargfer8cef32e32dcc-r8cdn56en56cr");
	bl_snd_bgm_enqueue(ch + 0, "<ara4>erc-cde32d32c<bara>cerdcc-r8cdrercr<arar4>drfargfer8cef32e32dcc-r8cdn56en56cr<ar");
	bl_snd_bgm_enqueue(ch + 0, "l4aecdc-c<ag+b>ecdc-c8e8ag+2l16>erc-cde32d32c<bara>cerdcc-r8cdrercr<arar4>drfargfer8ce");
	bl_snd_bgm_enqueue(ch + 0, "f32e32dcc-r8cdn56en56cr<ara4");

	bl_snd_bgm_enqueue(ch + 1, "t80l16o4brg+abb+32b32ag+e8eab+rbag+eg+ab8b+8a8e8eo2ab>cd>frab+rbagr8egrfeg+eg+abrb+rarere4br");
	bl_snd_bgm_enqueue(ch + 1, "g+abb+32b32ag+e8eab+rbag+eg+ab8b+8a8e8e<<ab>cd>frab+rbagr8egrfeg+eg+abrb+rarer");
	bl_snd_bgm_enqueue(ch + 1, "l4ec<abg+aeeg+b+abg+a8>c8ee2l16brg+abb+32b32ag+e8eab+rbag+eg+ab8b+8a8e8er4frab+rbagr8");
	bl_snd_bgm_enqueue(ch + 1, "egrfeg+eg+abrb+rarere4");

	bl_snd_bgm_enqueue(ch + 2, "t80l16o2e>e<e>e<e>e<e>e<a>a<a>a<a>a<a>an32g+n32g+<e>e<e>e<a>a<a>a<a>ac-cd<drdrd32d32agc");
	bl_snd_bgm_enqueue(ch + 2, "b+cb+cgcgb>brberg+r<a>e<a>e<a>eee<e>e<e>e<e>e<e>e<a>a<a>a<a>a<a>an32g+n32g+<e>e<e>e<a>a");
	bl_snd_bgm_enqueue(ch + 2, "<a>a<a>ac-cd<drdrd32d32agcb+cbcgcgb>brbergr<a>e<a>e<a>eee<a>e<a>e<a>e<a>en32en32en32en32");
	bl_snd_bgm_enqueue(ch + 2, "e<a>e<a>e<a>e<a>en32en32en32en32e<a>e<a>e<a>e<a>en32en32en32en32e<a>e<a>e<a>e<a>en32");
	bl_snd_bgm_enqueue(ch + 2, "en32en32en32e<e>e<e>e<e>e<e>e<a>a<a>a<a>a<a>an32g+n32g+<e>e<e>e<a>a<a>a<a>ac-cd<drdr");
	bl_snd_bgm_enqueue(ch + 2, "d32d32agcb+cb+cgcgb>brberg+r<a>e<a>e<a4");
}

void bgm_play(void)
{
	bl_snd_bgm_play();
}

void bgm_stop(void)
{
	bl_snd_bgm_stop();
}

void bgm_effect_rotate(void)
{
	bl_snd_bgm_overlay(2, "T120 S0 L128 V13 O6 Z0 C<<C>C<<C V12 >C<<C>C");
}

void bgm_effect_fall(void)
{
	bl_snd_bgm_overlay(2, "T120 S0 L64 V15 O4 Z2 C V14 <C V13 <C V12 <C V11 <C V10 <C");
}

void bgm_effect_move(void)
{
	bl_snd_bgm_overlay(2, "T120 S0 L128 V14 O4 Z2 C<<C");
}

void bgm_effect_erase(void)
{
	bl_snd_bgm_overlay(2, "T120 S0 L128 V15 O7 Z0 E<<E<<ER O6E<<E<<ER");
}

;
