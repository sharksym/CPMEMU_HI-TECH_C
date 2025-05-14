/*
 * Level meter for turboR LED
 * CAPS, KANA, PAUSE, Turbo
 */

extern void led_save(void);
extern void led_restore(void);
extern void led_meter(unsigned char val);

extern void led_caps_on(void);
extern void led_caps_off(void);

extern void led_kana_on(void);
extern void led_kana_off(void);
