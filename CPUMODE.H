/* MSX CPU Mode (turboR, WX, WSX) */

#define MSX1                0
#define MSX2                1
#define MSX2P               2
#define MSXTR               3

#define CPU_TR_Z80          0
#define CPU_TR_R800_ROM     1
#define CPU_TR_R800_DRAM    2
#define CPU_TR_LED_OFF      0x80

#define CPU_WX_3_58MHz      0
#define CPU_WX_5_37MHz      1

extern char get_msx_version(void);
extern void set_cpu_mode_tr(char mode);
extern char get_cpu_mode_tr(void);
extern void set_cpu_mode_wx(char mode);

