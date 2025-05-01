/* Hangul IME (Johab, CP949) */

#define IME_KR_JOHAB		0
#define IME_KR_CP949		1

void ime_kr_init(char mode);
void ime_kr_deinit(void);
unsigned char *ime_kr_send(unsigned char code);
