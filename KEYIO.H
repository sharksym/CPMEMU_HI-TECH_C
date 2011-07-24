/* MSX Keyboard I/O */

#define KEY_7           0x0080
#define KEY_6           0x0040
#define KEY_5           0x0020
#define KEY_4           0x0010
#define KEY_3           0x0008
#define KEY_2           0x0004
#define KEY_1           0x0002
#define KEY_0           0x0001

#define KEY_SEMICOLON   0x0180
#define KEY_BRACKET_L   0x0140
#define KEY_AT          0x0120
#define KEY_YEN         0x0110
#define KEY_WON         0x0110
#define KEY_BACKSLASH   0x0110
#define KEY_CARET       0x0108
#define KEY_MINUS       0x0104
#define KEY_9           0x0102
#define KEY_8           0x0101

#define KEY_B           0x0280
#define KEY_A           0x0240
#define KEY_UNDERSCORE  0x0220
#define KEY_SLASH       0x0210
#define KEY_DOT         0x0208
#define KEY_COMMA       0x0204
#define KEY_BRACKET_R   0x0202
#define KEY_COLONE      0x0201

#define KEY_J           0x0380
#define KEY_I           0x0340
#define KEY_H           0x0320
#define KEY_G           0x0310
#define KEY_F           0x0308
#define KEY_E           0x0304
#define KEY_D           0x0302
#define KEY_C           0x0301

#define KEY_R           0x0480
#define KEY_Q           0x0440
#define KEY_P           0x0420
#define KEY_O           0x0410
#define KEY_N           0x0408
#define KEY_M           0x0404
#define KEY_L           0x0402
#define KEY_K           0x0401

#define KEY_Z           0x0580
#define KEY_Y           0x0540
#define KEY_X           0x0520
#define KEY_W           0x0510
#define KEY_V           0x0508
#define KEY_U           0x0504
#define KEY_T           0x0502
#define KEY_S           0x0501

#define KEY_F3          0x0680
#define KEY_F2          0x0640
#define KEY_F1          0x0620
#define KEY_KANA        0x0610
#define KEY_HANGUL      0x0610
#define KEY_CAPS        0x0608
#define KEY_GRAPH       0x0604
#define KEY_CTRL        0x0602
#define KEY_SHIFT       0x0601

#define KEY_RETURN      0x0780
#define KEY_SELECT      0x0740
#define KEY_BS          0x0720
#define KEY_STOP        0x0710
#define KEY_TAB         0x0708
#define KEY_ESC         0x0704
#define KEY_F5          0x0702
#define KEY_F4          0x0701

#define KEY_RIGHT       0x0880
#define KEY_DOWN        0x0840
#define KEY_UP          0x0820
#define KEY_LEFT        0x0810
#define KEY_DEL         0x0808
#define KEY_INS         0x0804
#define KEY_HOME        0x0802
#define KEY_SPACE       0x0801

#define KEY_NUM_4       0x0980
#define KEY_NUM_3       0x0940
#define KEY_NUM_2       0x0920
#define KEY_NUM_1       0x0910
#define KEY_NUM_0       0x0908
#define KEY_NUM_SLASH   0x0904
#define KEY_NUM_PLUS    0x0902
#define KEY_NUM_ASTER   0x0901

#define KEY_NUM_DOT     0x0A80
#define KEY_NUM_COMMA   0x0A40
#define KEY_NUM_MINUS   0x0A20
#define KEY_NUM_9       0x0A10
#define KEY_NUM_8       0x0A08
#define KEY_NUM_7       0x0A04
#define KEY_NUM_6       0x0A02
#define KEY_NUM_5       0x0A01

#define KEY_CANCEL      0x0B08
#define KEY_RUN         0x0B02

extern char key_pressed(unsigned short keycode);
extern char key_clicked(unsigned short keycode);
extern unsigned short key_get_code(void);

